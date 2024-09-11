#include "RenderManager.h"
#include "MorphemeSystem/MorphemeSystem.h"
#include "IconsFontAwesome6.h"
#include "GuiManager/GuiManager.h"
#include "MorphemeEditorApp/MorphemeEditorApp.h"

using namespace DirectX;
using namespace SimpleMath;
using Microsoft::WRL::ComPtr;

RenderManager::RenderManager()
{
    this->m_window = nullptr;
    this->m_swapChain = nullptr;
    this->m_device = nullptr;
    this->m_deviceContext = nullptr;
    this->m_renderTargetView = nullptr;

    this->m_height = 1920;
    this->m_width = 1080;
}

RenderManager::~RenderManager()
{
    this->m_window = nullptr;
    this->m_swapChain = nullptr;
    this->m_device = nullptr;
    this->m_deviceContext = nullptr;
    this->m_renderTargetView = nullptr;
}

TextItem::TextItem(std::string text, Matrix world, DirectX::XMVECTORF32 color)
{
    this->m_position = world;
    this->m_text = text;
    this->m_color = color;

    this->m_depth = 0.f;
}

inline bool TextItem::operator<(TextItem text)
{
    if (text.m_depth > this->m_depth)
        return false;

    return true;
}

RenderManager* RenderManager::getInstance()
{
    if (_instance == nullptr)
        _instance = new RenderManager();

    return _instance;
}

void RenderManager::initialise(HWND hwnd)
{
    this->m_window = hwnd;

    if (!this->createD3DDevice())
        throw("Failed to create D3DDevice\n");

    this->createResources();

    m_states = std::make_unique<CommonStates>(this->m_device);

    m_debugEffect = std::make_unique<BasicEffect>(this->m_device);
    m_debugEffect->SetVertexColorEnabled(true);

    DX::ThrowIfFailed(
        CreateInputLayoutFromEffect<VertexPositionColor>(this->m_device, m_debugEffect.get(),
            m_inputLayout.ReleaseAndGetAddressOf())
    );

    m_physicalEffect = std::make_unique<BasicEffect>(this->m_device);
    m_physicalEffect->SetVertexColorEnabled(true);
    m_physicalEffect->SetLightingEnabled(true);

    m_physicalEffect->SetAmbientLightColor(Colors::White);

    m_physicalEffect->SetDiffuseColor(Vector4(0.5f, 0.5f, 0.5f, 1.f));
    m_physicalEffect->SetSpecularColor(Vector4(0.04f, 0.04f, 0.04f, 1.f));
    m_physicalEffect->SetSpecularPower(32.f);

    m_physicalEffect->SetLightEnabled(0, true);
    m_physicalEffect->SetLightDirection(0, Vector3(0.577f, -0.577f, -0.577f));
    m_physicalEffect->SetLightDiffuseColor(0, Colors::White);
    m_physicalEffect->SetLightSpecularColor(0, Colors::White);

    m_physicalEffect->SetLightEnabled(1, true);
    m_physicalEffect->SetLightDirection(1, Vector3(-0.577f, -0.577f, 0.577f));
    m_physicalEffect->SetLightDiffuseColor(1, Colors::Gray);
    m_physicalEffect->SetLightSpecularColor(1, Colors::Gray);

    m_physicalEffect->SetLightEnabled(2, true);
    m_physicalEffect->SetLightDirection(2, Vector3(0.0f, 0.577f, -0.577f));
    m_physicalEffect->SetLightDiffuseColor(2, Colors::Gray);
    m_physicalEffect->SetLightSpecularColor(2, Colors::White);

    DX::ThrowIfFailed(
        CreateInputLayoutFromEffect<VertexPositionNormalColor>(this->m_device, m_physicalEffect.get(),
            m_physicalInputLayout.ReleaseAndGetAddressOf())
    );

    m_view = Matrix::CreateLookAt(Vector3(2.f, 2.f, 2.f),
        Vector3::Zero, Vector3::UnitY);
    m_proj = Matrix::CreatePerspectiveFieldOfView(XM_PI / 4.f,
        float(this->m_width) / float(this->m_height), 0.1f, 10.f);
    m_origin = Matrix::Identity;

    m_debugEffect->SetView(m_view);
    m_debugEffect->SetProjection(m_proj);
    m_debugEffect->SetWorld(m_origin);

    m_physicalEffect->SetView(m_view);
    m_physicalEffect->SetProjection(m_proj);
    m_physicalEffect->SetWorld(m_origin);

    this->m_sprite = std::make_unique<DirectX::SpriteBatch>(this->m_deviceContext);
    this->m_font = std::make_unique<DirectX::SpriteFont>(this->m_device, L".//Data//font//font.spritefont");
    this->m_fontBold = std::make_unique<DirectX::SpriteFont>(this->m_device, L".//Data//font//font_bold.spritefont");
    this->m_fontItalic = std::make_unique<DirectX::SpriteFont>(this->m_device, L".//Data//font//font_italic.spritefont");

    this->loadSettings();

    this->m_initialised = true;
}

void RenderManager::shutdown()
{
    this->saveSettings();

    this->m_states.reset();
    this->m_debugEffect.reset();
    this->m_physicalEffect.reset();
    this->m_inputLayout.Reset();
    this->m_physicalInputLayout.Reset();

    this->m_sprite.reset();
    this->m_font.reset();
    this->m_fontBold.reset();
    this->m_fontItalic.reset();

    this->clearD3DDevice();
}

void RenderManager::createResources()
{
    this->m_deviceContext->OMSetRenderTargets(0, nullptr, nullptr);

    if (this->m_depthStencilView)
        this->m_depthStencilView->Release();

    if (this->m_offScreenRenderTarget)
        this->m_offScreenRenderTarget->Release();

    if (this->m_renderTargetTexture)
        this->m_renderTargetTexture->Release();

    if (this->m_renderTargetView)
        this->m_renderTargetView->Release();

    if (this->m_shaderResourceViewViewport)
        this->m_shaderResourceViewViewport->Release();

    if (this->m_depthStencilState)
        this->m_depthStencilState->Release();

    const int msaaCount = this->m_settings.msaaCount;
    const int msaaQuality = this->m_settings.msaaQuality;

    D3D11_TEXTURE2D_DESC depthStencilDesc = {};
    depthStencilDesc.Width = this->m_width;
    depthStencilDesc.Height = this->m_height;
    depthStencilDesc.MipLevels = 1;
    depthStencilDesc.ArraySize = 1;
    depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    depthStencilDesc.SampleDesc.Count = msaaCount;
    depthStencilDesc.SampleDesc.Quality = msaaQuality;
    depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
    depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;

    ComPtr<ID3D11Texture2D> depthStencil;
    DX::ThrowIfFailed(this->m_device->CreateTexture2D(&depthStencilDesc, nullptr, depthStencil.GetAddressOf()));

    D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
    dsvDesc.Format = depthStencilDesc.Format;
    dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;

    DX::ThrowIfFailed(this->m_device->CreateDepthStencilView(depthStencil.Get(), &dsvDesc, &this->m_depthStencilView));

    const DXGI_FORMAT textureFmt = DXGI_FORMAT_R32G32B32A32_FLOAT;

    D3D11_TEXTURE2D_DESC textureDescOffscren;
    textureDescOffscren.Width = m_width;
    textureDescOffscren.Height = m_height;
    textureDescOffscren.MipLevels = 1;
    textureDescOffscren.ArraySize = 1;
    textureDescOffscren.Format = textureFmt;
    textureDescOffscren.SampleDesc.Count = msaaCount;
    textureDescOffscren.SampleDesc.Quality = msaaQuality;
    textureDescOffscren.Usage = D3D11_USAGE_DEFAULT;
    textureDescOffscren.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
    textureDescOffscren.CPUAccessFlags = 0;
    textureDescOffscren.MiscFlags = 0;

    m_device->CreateTexture2D(&textureDescOffscren, nullptr, &this->m_offScreenRenderTarget);

    D3D11_TEXTURE2D_DESC textureDesc;
    textureDesc.Width = m_width;
    textureDesc.Height = m_height;
    textureDesc.MipLevels = 1;
    textureDesc.ArraySize = 1;
    textureDesc.Format = textureFmt;
    textureDesc.SampleDesc.Count = 1;
    textureDesc.Usage = D3D11_USAGE_DEFAULT;
    textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
    textureDesc.CPUAccessFlags = 0;
    textureDesc.MiscFlags = 0;

    m_device->CreateTexture2D(&textureDesc, nullptr, &this->m_renderTargetTexture);

    D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
    renderTargetViewDesc.Format = textureFmt;
    renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DMS;
    renderTargetViewDesc.Texture2D.MipSlice = 0;

    this->m_device->CreateRenderTargetView(this->m_offScreenRenderTarget, &renderTargetViewDesc, &this->m_renderTargetView);

    D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
    shaderResourceViewDesc.Format = textureFmt;
    shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DMS;
    shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
    shaderResourceViewDesc.Texture2D.MipLevels = 1;

    this->m_device->CreateShaderResourceView(this->m_offScreenRenderTarget, &shaderResourceViewDesc, &this->m_shaderResourceViewViewport);

    D3D11_DEPTH_STENCIL_DESC depth_stencil_desc;
    depth_stencil_desc.DepthEnable = false;
    depth_stencil_desc.DepthFunc = D3D11_COMPARISON_ALWAYS;
    depth_stencil_desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
    depth_stencil_desc.StencilEnable = true;
    depth_stencil_desc.StencilReadMask = 0xFF;
    depth_stencil_desc.StencilWriteMask = 0xFF;

    D3D11_DEPTH_STENCILOP_DESC stencil_op_desc;
    stencil_op_desc.StencilFunc = D3D11_COMPARISON_ALWAYS;
    stencil_op_desc.StencilPassOp = D3D11_STENCIL_OP_INCR_SAT;
    stencil_op_desc.StencilFailOp = D3D11_STENCIL_OP_INCR_SAT;

    depth_stencil_desc.FrontFace = stencil_op_desc;
    depth_stencil_desc.BackFace = stencil_op_desc;

    this->m_device->CreateDepthStencilState(&depth_stencil_desc, &this->m_depthStencilState);
}

void RenderManager::update(float dt)
{
    if (!this->m_initialised)
        throw("Called update() without calling initialise() first\n");

    this->m_dt = dt;

    Camera* camera = MorphemeEditorApp::getInstance()->getCamera();

    this->m_view = camera->getViewMatrix();
    this->m_proj = camera->getProjectionMatrix();

    this->m_texts.clear();

    this->createResources();
}

void RenderManager::clear()
{
    // Clear the views.
    float color[4] = { this->m_settings.clearColor.x, this->m_settings.clearColor.y, this->m_settings.clearColor.z, this->m_settings.clearColor.w };
    this->m_deviceContext->ClearRenderTargetView(this->m_renderTargetView, color);
    this->m_deviceContext->ClearDepthStencilView(this->m_depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

    this->m_deviceContext->OMSetDepthStencilState(this->m_depthStencilState, 0);
    this->m_deviceContext->OMSetRenderTargets(1, &this->m_renderTargetView, this->m_depthStencilView);

    // Set the viewport.
    D3D11_VIEWPORT viewport = { 0.0f, 0.0f, static_cast<float>(this->m_width), static_cast<float>(this->m_height), 0.f, 1.f };
    this->m_deviceContext->RSSetViewports(1, &viewport);
}

void RenderManager::render()
{
    this->clear();

    if (this->m_renderTargetView)
    {
        ID3D11DeviceContext* context = this->m_deviceContext;

        context->OMSetBlendState(m_states->AlphaBlend(), nullptr, 0xFFFFFFFF);
        context->RSSetState(m_states->CullNone());

        m_debugEffect->SetWorld(m_origin);
        m_debugEffect->SetView(m_view);
        m_debugEffect->SetProjection(m_proj);
        m_debugEffect->Apply(context);

        context->IASetInputLayout(m_inputLayout.Get());

        DirectX::PrimitiveBatch<DirectX::VertexPositionColor> prim(this->m_deviceContext);
        prim.Begin();

        DX::DrawGrid(&prim, this->m_settings.gridScale * Vector3::UnitX, this->m_settings.gridScale * Vector3::UnitZ, Vector3::Zero, 100, 100, Colors::Gray);
        DX::DrawOriginMarker(&prim, Matrix::Identity, 0.5f, Colors::DarkCyan);

        prim.End();

        Character* character = MorphemeEditorApp::getInstance()->getCharacter();

        if (character)
            character->draw(this);

        this->m_sprite->Begin();

        std::string frametime = RString::floatToString(this->m_dt) + " ms";
        DX::AddOverlayText(this->m_sprite.get(), this->m_font.get(), frametime.c_str(), Vector2(10, 40), 0, 0.5f, Colors::White, TextFlags_Shadow);

        Camera* camera = MorphemeEditorApp::getInstance()->getCamera();

        if (camera)
        {
            for (size_t i = 0; i < this->m_texts.size(); i++)
                DX::AddWorldSpaceText(this->m_sprite.get(), m_font.get(), this->m_texts[i].m_text, Vector3::Zero, this->m_texts[i].m_position, camera, this->m_texts[i].m_color);
        }

        this->m_sprite->End();

        context->ResolveSubresource(this->m_renderTargetTexture, 0,
            this->m_offScreenRenderTarget, 0,
            DXGI_FORMAT_D24_UNORM_S8_UINT);
    }

    if (this->m_guiRenderTargetView)
        GuiManager::getInstance()->render(this->m_deviceContext, this->m_guiRenderTargetView);
}

void RenderManager::setResolution(int width, int height)
{
    this->m_width = (std::max)(width, 1);
    this->m_height = (std::max)(height, 1);
}

void RenderManager::resize(int width, int height)
{
    this->clear();
    this->clearGuiRenderTarget();

    this->setResolution(width, height);

    this->m_swapChain->ResizeBuffers(0, width, height, DXGI_FORMAT_UNKNOWN, 0);

    this->createGuiRenderTarget();
    this->createResources();
}

void RenderManager::addText(std::string text, Matrix position, DirectX::XMVECTORF32 color)
{
    this->m_texts.push_back(TextItem(text, position, color));
}

void RenderManager::present()
{
    try
    {
        HRESULT hr = this->m_swapChain->Present(1, 0);

        DX::ThrowIfFailed(hr);
    }
    catch (const std::exception& e)
    {
        g_appLog->alertMessage(MsgLevel_Error, e.what());
    }
}

bool RenderManager::createD3DDevice()
{
    // Setup swap chain
    DXGI_SWAP_CHAIN_DESC sd;
    ZeroMemory(&sd, sizeof(sd));
    sd.BufferCount = 2;
    sd.BufferDesc.Width = 0;
    sd.BufferDesc.Height = 0;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferDesc.RefreshRate.Numerator = 60;
    sd.BufferDesc.RefreshRate.Denominator = 1;
    sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = this->m_window;
    sd.SampleDesc.Count = 1;
    sd.SampleDesc.Quality = 0;
    sd.Windowed = TRUE;
    sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

    UINT createDeviceFlags = 0;
    //createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
    D3D_FEATURE_LEVEL featureLevel;
    const D3D_FEATURE_LEVEL featureLevelArray[2] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0, };
    HRESULT res = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, createDeviceFlags, featureLevelArray, 2, D3D11_SDK_VERSION, &sd, &this->m_swapChain, &this->m_device, &featureLevel, &this->m_deviceContext);
    if (res == DXGI_ERROR_UNSUPPORTED) // Try high-performance WARP software driver if hardware is not available.
        res = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_WARP, nullptr, createDeviceFlags, featureLevelArray, 2, D3D11_SDK_VERSION, &sd, &this->m_swapChain, &this->m_device, &featureLevel, &this->m_deviceContext);
    if (res != S_OK)
        return false;

    this->createGuiRenderTarget();
    return true;
}

void RenderManager::clearD3DDevice()
{
    this->clearGuiRenderTarget();
    if (this->m_swapChain) { this->m_swapChain->Release(); this->m_swapChain = nullptr; }
    if (this->m_deviceContext) { this->m_deviceContext->Release(); this->m_deviceContext = nullptr; }
    if (this->m_device) { this->m_device->Release(); this->m_device = nullptr; }
}

void RenderManager::createGuiRenderTarget()
{
    ID3D11Texture2D* pBackBuffer;
    this->m_swapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
    if (this->m_device) 
        this->m_device->CreateRenderTargetView(pBackBuffer, nullptr, &this->m_guiRenderTargetView);

    pBackBuffer->Release();
}

void RenderManager::clearGuiRenderTarget()
{
    if (this->m_guiRenderTargetView)
        this->m_guiRenderTargetView->Release(); this->m_guiRenderTargetView = nullptr;
}

void RenderManager::applyDebugEffect(DirectX::SimpleMath::Matrix world)
{
    this->m_debugEffect->SetWorld(world);
    this->m_debugEffect->SetView(this->m_view);
    this->m_debugEffect->SetProjection(this->m_proj);
    this->m_debugEffect->Apply(this->m_deviceContext);
}

void RenderManager::applyPhysicalEffect(DirectX::SimpleMath::Matrix world, float alpha)
{
    this->m_physicalEffect->SetWorld(world);
    this->m_physicalEffect->SetView(this->m_view);
    this->m_physicalEffect->SetProjection(this->m_proj);
    this->m_physicalEffect->SetAlpha(alpha);

    this->m_physicalEffect->Apply(this->m_deviceContext);
}

void RenderManager::setInputLayout(InputLayoutType type)
{
    switch (type)
    {
    case kDebugLayout:
        return this->m_deviceContext->IASetInputLayout(this->m_inputLayout.Get());
    case kPhysicalLayout:
        return this->m_deviceContext->IASetInputLayout(this->m_physicalInputLayout.Get());
    default:
        break;
    }
}

void RenderManager::loadSettings()
{
    RINI* settings = RINI::open("Data\\res\\settings.ini");

    if (settings == nullptr)
    {
        throw("Failed to open settings.ini");
        return;
    }

    this->m_settings.msaaCount = settings->getInt("Graphics", "msaa_count", 1);

    settings->destroy();
}

void RenderManager::saveSettings()
{
    RINI* settings = RINI::open("Data\\res\\settings.ini");

    if (settings == nullptr)
        settings = RINI::create("Data\\res\\settings.ini");

    settings->setInt("Graphics", "msaa_count", this->m_settings.msaaCount);

    settings->write();

    settings->destroy();
}