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

#include "../framework.h"
#include "../extern.h"
#include "DebugDraw.h"
#include "StepTimer.h"
#include "Camera/Camera.h"
#include "../fromloader/fromloader.h"

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

class Scene
{
public:
	struct Settings
	{
		DirectX::SimpleMath::Vector4 m_backgroundColor = DirectX::SimpleMath::Vector4(0.06f, 0.06f, 0.06f, 1.f);
		float m_gridScale = 100.f;
	} m_settings;

	Camera m_camera;

	HWND m_window;
	int m_width, m_height;
	DX::StepTimer m_timer;

	IDXGISwapChain* m_swapChain;
	ID3D11Device* m_device;
	ID3D11DeviceContext* m_deviceContext;
	ID3D11Texture2D* m_renderTargetTextureViewport;
	ID3D11RenderTargetView* m_renderTargetView;
	ID3D11ShaderResourceView* m_shaderResourceViewViewport;
	ID3D11DepthStencilView* m_depthStencilView;
	ID3D11DepthStencilState* m_depthStencilState;

	std::unique_ptr<DirectX::CommonStates> m_states;
	std::unique_ptr<DirectX::BasicEffect> m_effect;
	std::unique_ptr<DirectX::PrimitiveBatch<DirectX::VertexPositionColor>> m_batch;
	std::unique_ptr<DirectX::SpriteBatch> m_sprite;
	std::unique_ptr<DirectX::SpriteFont> m_font;
	std::unique_ptr<DirectX::SpriteFont> m_fontBold;
	std::unique_ptr<DirectX::SpriteFont> m_fontItalic;

	Microsoft::WRL::ComPtr<ID3D11InputLayout> m_inputLayout;

	DirectX::SimpleMath::Matrix m_world;
	DirectX::SimpleMath::Matrix m_view;
	DirectX::SimpleMath::Matrix m_proj;

	Microsoft::WRL::ComPtr<ID3D11Texture2D> m_offscreenRenderTarget;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_offscreenRenderTargetSRV;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> m_depthStencilSRV;

	std::vector<TextItem> m_texts;

	Scene();
	~Scene();

	void Initialise(HWND hwnd, IDXGISwapChain* pSwapChain, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, ID3D11RenderTargetView* pRenderTargetView);
	void CreateResources();
	void Update();
	void Clear();
	void Render();
	void SetRenderResolution(int width, int height);
	void AddText(std::string text, Matrix position, DirectX::XMVECTORF32 color = DirectX::Colors::White);
	void DrawFlverModel(AnimPlayer* animPlayer, MR::AnimRigDef* rig);
};