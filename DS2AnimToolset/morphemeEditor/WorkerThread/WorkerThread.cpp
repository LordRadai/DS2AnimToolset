#include "WorkerThread.h"
#include <algorithm>

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>

namespace
{
	constexpr const wchar_t* kProgressWindowClass = L"MorphemeEditorProgressWindow";
	constexpr UINT kRepaintIntervalMs = 33;
	constexpr ULONGLONG kShowDelayMs = 200;

	constexpr int kWidth = 420;
	constexpr int kPadding = 12;
	constexpr int kTextHeight = 18;
	constexpr int kBarHeight = 18;
	constexpr int kRowSpacing = 10;

	const COLORREF kBackgroundColor = RGB(36, 36, 36);
	const COLORREF kFrameColor = RGB(60, 60, 60);
	const COLORREF kFillColor = RGB(230, 179, 0);
	const COLORREF kTextColor = RGB(255, 255, 255);

	int scaled(int value, UINT dpi)
	{
		return MulDiv(value, int(dpi), 96);
	}

	int getClientHeight(int numProcesses, UINT dpi)
	{
		const int rows = (std::max)(numProcesses, 1);

		return scaled(kPadding * 2 + rows * (kTextHeight + kBarHeight) + (rows - 1) * kRowSpacing, dpi);
	}

	void paintProgressWindow(HWND hwnd, const std::vector<ProgressIndicator>& processes)
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hwnd, &ps);

		RECT client;
		GetClientRect(hwnd, &client);

		const UINT dpi = GetDpiForWindow(hwnd);

		// Double buffer to avoid flicker on every repaint
		HDC memDC = CreateCompatibleDC(hdc);
		HBITMAP memBmp = CreateCompatibleBitmap(hdc, client.right, client.bottom);
		HGDIOBJ oldBmp = SelectObject(memDC, memBmp);

		HFONT font = CreateFontW(-scaled(13, dpi), 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, DEFAULT_PITCH, L"Segoe UI");
		HGDIOBJ oldFont = SelectObject(memDC, font);

		HBRUSH bgBrush = CreateSolidBrush(kBackgroundColor);
		HBRUSH frameBrush = CreateSolidBrush(kFrameColor);
		HBRUSH fillBrush = CreateSolidBrush(kFillColor);

		FillRect(memDC, &client, bgBrush);
		SetBkMode(memDC, TRANSPARENT);
		SetTextColor(memDC, kTextColor);

		const int padding = scaled(kPadding, dpi);
		const int textHeight = scaled(kTextHeight, dpi);
		const int barHeight = scaled(kBarHeight, dpi);
		const int rowSpacing = scaled(kRowSpacing, dpi);

		int y = padding;

		for (const ProgressIndicator& process : processes)
		{
			const int step = process.getStep();
			const int numSteps = process.getNumSteps();

			std::string label = process.getProcessName() + ": " + (step < numSteps ? process.getStepName() : "finished");

			RECT textRect = { padding, y, client.right - padding, y + textHeight };
			DrawTextA(memDC, label.c_str(), -1, &textRect, DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS | DT_NOPREFIX);

			y += textHeight;

			RECT barRect = { padding, y, client.right - padding, y + barHeight };
			FillRect(memDC, &barRect, frameBrush);

			const float progress = numSteps > 0 ? std::clamp(float(step) / float(numSteps), 0.f, 1.f) : 0.f;

			RECT fillRect = barRect;
			fillRect.right = barRect.left + LONG((barRect.right - barRect.left) * progress);
			FillRect(memDC, &fillRect, fillBrush);

			char buf[64];
			sprintf_s(buf, "%d/%d", step, numSteps);
			DrawTextA(memDC, buf, -1, &barRect, DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);

			y += barHeight + rowSpacing;
		}

		BitBlt(hdc, 0, 0, client.right, client.bottom, memDC, 0, 0, SRCCOPY);

		SelectObject(memDC, oldFont);
		SelectObject(memDC, oldBmp);
		DeleteObject(font);
		DeleteObject(bgBrush);
		DeleteObject(frameBrush);
		DeleteObject(fillBrush);
		DeleteObject(memBmp);
		DeleteDC(memDC);

		EndPaint(hwnd, &ps);
	}

	LRESULT CALLBACK progressWindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		switch (msg)
		{
		case WM_PAINT:
		{
			WorkerThread* worker = reinterpret_cast<WorkerThread*>(GetWindowLongPtrW(hwnd, GWLP_USERDATA));

			paintProgressWindow(hwnd, worker ? worker->getProcessesSnapshot() : std::vector<ProgressIndicator>());
			return 0;
		}
		case WM_ERASEBKGND:
			return 1;
		case WM_CLOSE:
			// The window lives exactly as long as the task, the user can't dismiss it
			return 0;
		}

		return DefWindowProcW(hwnd, msg, wParam, lParam);
	}

	void registerProgressWindowClass()
	{
		static std::once_flag registered;

		std::call_once(registered, []() {
			WNDCLASSEXW wc = { sizeof(wc) };
			wc.lpfnWndProc = progressWindowProc;
			wc.hInstance = GetModuleHandleW(nullptr);
			wc.hCursor = LoadCursor(nullptr, IDC_WAIT);
			wc.lpszClassName = kProgressWindowClass;

			RegisterClassExW(&wc);
			});
	}
}

WorkerThread* WorkerThread::getInstance()
{
	if (_instance == nullptr)
		_instance = new WorkerThread;

	return _instance;
}

void WorkerThread::beginTask(std::string name)
{
	{
		std::lock_guard<std::mutex> lock(this->m_mutex);

		this->m_done = false;
		this->m_threadName = name;
		this->m_processes.clear();
	}

	this->m_uiThread = std::thread(&WorkerThread::progressWindowThread, this, this->m_mainWnd);
}

void WorkerThread::endTask()
{
	{
		std::lock_guard<std::mutex> lock(this->m_mutex);
		this->m_done = true;
	}

	if (this->m_uiThread.joinable())
		this->m_uiThread.join();

	std::lock_guard<std::mutex> lock(this->m_mutex);
	this->m_processes.clear();
}

void WorkerThread::progressWindowThread(void* parentWnd)
{
	registerProgressWindowClass();

	// Deliberately neither a child nor owned by the main window: either relationship would attach this
	// thread's input queue to the main thread's, which is busy running the task. Instead the window is
	// a normal (non topmost) window slotted right above the main window in the z-order, so it doesn't
	// get lost behind morphemeEditor and other applications can still cover it.
	HWND mainWnd = (HWND)parentWnd;

	const std::string title = this->getThreadName();
	const DWORD style = WS_POPUP | WS_CAPTION;
	const DWORD exStyle = WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE;

	HWND hwnd = CreateWindowExW(exStyle, kProgressWindowClass, std::wstring(title.begin(), title.end()).c_str(),
		style, 0, 0, 0, 0, nullptr, nullptr, GetModuleHandleW(nullptr), nullptr);

	if (hwnd == nullptr)
		return;

	SetWindowLongPtrW(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));

	const ULONGLONG startTime = GetTickCount64();

	bool visible = false;
	bool placed = false;

	while (!this->isDone())
	{
		MSG msg;
		while (PeekMessageW(&msg, nullptr, 0U, 0U, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessageW(&msg);
		}

		const bool hasMainWnd = mainWnd != nullptr && IsWindow(mainWnd);

		// Hidden while the editor is minimised, and for short tasks that finish before the delay so it doesn't flash
		const bool shouldShow = (GetTickCount64() - startTime >= kShowDelayMs) && (!hasMainWnd || (IsWindowVisible(mainWnd) && !IsIconic(mainWnd)));

		if (shouldShow)
		{
			// Size from the editor's DPI so the window matches the monitor the editor is on
			const UINT dpi = hasMainWnd ? GetDpiForWindow(mainWnd) : GetDpiForWindow(hwnd);

			RECT rect = { 0, 0, scaled(kWidth, dpi), getClientHeight(this->getNumProcesses(), dpi) };
			AdjustWindowRectExForDpi(&rect, style, FALSE, exStyle, dpi);

			const int width = rect.right - rect.left;
			const int height = rect.bottom - rect.top;

			RECT current;
			GetWindowRect(hwnd, &current);

			// Centered over the editor when it first shows up, after that it stays wherever the user drags it
			int x = current.left;
			int y = current.top;

			if (!placed)
			{
				RECT area;
				if (hasMainWnd)
				{
					GetClientRect(mainWnd, &area);
					MapWindowPoints(mainWnd, nullptr, reinterpret_cast<POINT*>(&area), 2);
				}
				else
					SystemParametersInfoW(SPI_GETWORKAREA, 0, &area, 0);

				x = (std::max)(area.left, area.left + ((area.right - area.left) - width) / 2);
				y = (std::max)(area.top, area.top + ((area.bottom - area.top) - height) / 2);

				placed = true;
			}

			// Insert just below whatever sits right above the editor. A topmost window or nothing above it
			// means the editor is the top normal window, so HWND_TOP keeps this out of the topmost band.
			HWND insertAfter = HWND_TOP;

			if (hasMainWnd)
			{
				HWND above = GetWindow(mainWnd, GW_HWNDPREV);

				if (above == hwnd)
					insertAfter = nullptr;
				else if (above != nullptr && !(GetWindowLongW(above, GWL_EXSTYLE) & WS_EX_TOPMOST))
					insertAfter = above;
			}

			UINT flags = SWP_NOACTIVATE | SWP_NOOWNERZORDER;

			if (insertAfter == nullptr)
				flags |= SWP_NOZORDER;

			if (!visible)
				flags |= SWP_SHOWWINDOW;

			const bool moved = current.left != x || current.top != y || (current.right - current.left) != width || (current.bottom - current.top) != height;

			if (moved || !(flags & SWP_NOZORDER) || !visible)
				SetWindowPos(hwnd, insertAfter, x, y, width, height, flags);

			visible = true;

			InvalidateRect(hwnd, nullptr, FALSE);
			UpdateWindow(hwnd);
		}
		else if (visible)
		{
			ShowWindow(hwnd, SW_HIDE);
			visible = false;
		}

		MsgWaitForMultipleObjects(0, nullptr, FALSE, kRepaintIntervalMs, QS_ALLINPUT);
	}

	DestroyWindow(hwnd);
}

void WorkerThread::setMainWindow(void* hwnd)
{
	this->m_mainWnd = hwnd;
}

void WorkerThread::addProcess(std::string name, int numSteps)
{
	std::lock_guard<std::mutex> lock(this->m_mutex);

	// A process with no steps would never finish and swallow the parent's increaseProgressStep calls
	if (!this->m_done && numSteps > 0)
		this->m_processes.push_back(ProgressIndicator(name, numSteps));
}

void WorkerThread::increaseProgressStep()
{
	std::lock_guard<std::mutex> lock(this->m_mutex);

	if (this->m_processes.empty())
		return;

	int step = this->m_processes.back().getStep();
	this->m_processes.back().setStep(++step);

	this->m_processes.erase(std::remove_if(this->m_processes.begin(), this->m_processes.end(), [](const ProgressIndicator& process) { return !process.isBusy(); }), this->m_processes.end());
}

void WorkerThread::setProcessStepName(std::string name)
{
	std::lock_guard<std::mutex> lock(this->m_mutex);

	if (this->m_processes.empty())
		return;

	this->m_processes.back().setStepName(name);
}
