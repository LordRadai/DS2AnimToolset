#pragma once
#include <CommonStates.h>
#include <Effects.h>
#include <BufferHelpers.h>
#include <DirectXHelpers.h>
#include <SimpleMath.h>
#include <PrimitiveBatch.h>
#include <SpriteBatch.h>
#include <SpriteFont.h>
#include <GeometricPrimitive.h>
#include <VertexTypes.h>

#include "framework.h"
#include "extern.h"
#include "DebugDraw/DebugDraw.h"
#include "StepTimer/StepTimer.h"

namespace DX
{
	inline void ThrowIfFailed(HRESULT hr)
	{
		if (FAILED(hr))
		{
			// Set a breakpoint on this line to catch DirectX API errors
			throw std::exception();
		}
	}
}

struct TextItem
{
	std::string m_text;
	Matrix m_position;
	DirectX::XMVECTORF32 m_color;
	float m_depth;

	TextItem(std::string text, Matrix world, DirectX::XMVECTORF32 color);
	inline bool operator<(TextItem text);
};

enum InputLayoutType
{
	kDebugLayout,
	kPhysicalLayout
};

class RenderManager
{
public:
	struct Settings
	{
		DirectX::SimpleMath::Vector4 clearColor = DirectX::SimpleMath::Vector4(0.06f, 0.06f, 0.06f, 1.f);
		float gridScale = 100.f;
		int msaaCount = 4;
		int msaaQuality = 0;
	};

	~RenderManager();

	RenderManager(const RenderManager&) = delete;
	void operator=(const RenderManager&) = delete;

	static RenderManager* getInstance();

	void initialise(HWND hwnd);
	void shutdown();

	void update(float dt);
	void clear();
	void render();
	void setResolution(int width, int height);
	void resize(int width, int height);
	void present();

	void addText(std::string text, Matrix position, DirectX::XMVECTORF32 color = DirectX::Colors::White);

	bool isInitialised() const { return this->m_initialised; }
	HWND getHwnd() const { return this->m_window; }
	ID3D11Device* getDevice() const { return this->m_device; }
	ID3D11DeviceContext* getDeviceContext() const { return this->m_deviceContext; }
	ID3D11RenderTargetView* getRenderTargetView() const { return this->m_renderTargetView; }
	IDXGISwapChain* getSwapChain() const { return this->m_swapChain; }
	ID3D11ShaderResourceView* getShaderResourceViewport() const { return this->m_shaderResourceViewViewport; }
	DirectX::BasicEffect* getDebugShader() const { return this->m_debugEffect.get(); }
	DirectX::BasicEffect* getShader() const { return this->m_physicalEffect.get(); }
	DirectX::SimpleMath::Matrix getRootPosition() const { return this->m_origin; }
	DirectX::SimpleMath::Matrix getViewMatrix() const { return this->m_view; }
	DirectX::SimpleMath::Matrix getProjectionMatrix() const { return this->m_proj; }
	int getWidth() const { return this->m_width; }
	int getHeight() const { return this->m_height; }
	Settings* getSettings() { return &this->m_settings; }

	void applyDebugEffect(DirectX::SimpleMath::Matrix world);
	void applyEffect(DirectX::SimpleMath::Matrix world, float alpha);
	void setInputLayout(InputLayoutType type);

private:
	void loadSettings();
	void saveSettings();

	bool createD3DDevice();
	void createGuiRenderTarget();
	void clearD3DDevice();
	void clearGuiRenderTarget();

	void createResources();

	Settings m_settings;

	bool m_initialised = false;

	HWND m_window;
	int m_width, m_height;
	float m_dt;

	IDXGISwapChain* m_swapChain;
	ID3D11Device* m_device;
	ID3D11DeviceContext* m_deviceContext;
	ID3D11Texture2D* m_offScreenRenderTarget;
	ID3D11Texture2D* m_renderTargetTexture;
	ID3D11RenderTargetView* m_renderTargetView;
	ID3D11RenderTargetView* m_guiRenderTargetView;
	ID3D11ShaderResourceView* m_shaderResourceViewViewport;
	ID3D11DepthStencilView* m_depthStencilView;
	ID3D11DepthStencilState* m_depthStencilState;

	std::unique_ptr<DirectX::CommonStates> m_states;
	std::unique_ptr<DirectX::BasicEffect> m_debugEffect;
	std::unique_ptr<DirectX::BasicEffect> m_physicalEffect;
	std::unique_ptr<DirectX::SpriteBatch> m_sprite;
	std::unique_ptr<DirectX::SpriteFont> m_font;
	std::unique_ptr<DirectX::SpriteFont> m_fontBold;
	std::unique_ptr<DirectX::SpriteFont> m_fontItalic;

	Microsoft::WRL::ComPtr<ID3D11InputLayout> m_inputLayout;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> m_physicalInputLayout;

	DirectX::SimpleMath::Matrix m_origin;
	DirectX::SimpleMath::Matrix m_view;
	DirectX::SimpleMath::Matrix m_proj;

	std::vector<TextItem> m_texts;

	RenderManager();

	inline static RenderManager* _instance = nullptr;
};