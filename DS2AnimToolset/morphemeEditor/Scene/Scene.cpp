#include "Scene.h"
#include "StepTimer.h"
#include "Application/Application.h"
#include "MorphemeSystem/MorphemeSystem.h"

using namespace DirectX;
using namespace SimpleMath;
using Microsoft::WRL::ComPtr;

namespace
{
    constexpr UINT MSAA_COUNT = 4;
    constexpr UINT MSAA_QUALITY = 0;
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

Scene::Scene()
{
    this->m_window = nullptr;
    this->m_swapChain = nullptr;
    this->m_device = nullptr;
    this->m_deviceContext = nullptr;
    this->m_renderTargetView = nullptr;

    this->m_height = 1920;
    this->m_width = 1080;
}

Scene::~Scene()
{
    this->m_window = nullptr;
    this->m_swapChain = nullptr;
    this->m_device = nullptr;
    this->m_deviceContext = nullptr;
    this->m_renderTargetView = nullptr;
}

void Scene::Initialise(HWND hwnd, IDXGISwapChain* pSwapChain, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, ID3D11RenderTargetView* pRenderTargetView)
{
    this->m_window = hwnd;
    this->m_swapChain = pSwapChain;
    this->m_device = pDevice;
    this->m_deviceContext = pDeviceContext;

    this->CreateResources();

    m_states = std::make_unique<CommonStates>(this->m_device);

    m_effect = std::make_unique<BasicEffect>(this->m_device);
    m_effect->SetVertexColorEnabled(true);

    DX::ThrowIfFailed(
        CreateInputLayoutFromEffect<VertexPositionColor>(this->m_device, m_effect.get(),
            m_inputLayout.ReleaseAndGetAddressOf())
    );

    m_batch = std::make_unique<PrimitiveBatch<VertexPositionColor>>(this->m_deviceContext);

    m_view = Matrix::CreateLookAt(Vector3(2.f, 2.f, 2.f),
        Vector3::Zero, Vector3::UnitY);
    m_proj = Matrix::CreatePerspectiveFieldOfView(XM_PI / 4.f,
        float(this->m_width) / float(this->m_height), 0.1f, 10.f);
    m_world = Matrix::Identity;

    m_effect->SetView(m_view);
    m_effect->SetProjection(m_proj);
    m_effect->SetWorld(m_world);

    this->m_sprite = std::make_unique<DirectX::SpriteBatch>(this->m_deviceContext);
    this->m_font = std::make_unique<DirectX::SpriteFont>(this->m_device, L".//Data//font//font.spritefont");
    this->m_fontBold = std::make_unique<DirectX::SpriteFont>(this->m_device, L".//Data//font//font_bold.spritefont");
    this->m_fontItalic = std::make_unique<DirectX::SpriteFont>(this->m_device, L".//Data//font//font_italic.spritefont");
}

void Scene::CreateResources()
{
    this->m_deviceContext->OMSetRenderTargets(0, nullptr, nullptr);

    if (this->m_depthStencilView != nullptr)
        this->m_depthStencilView->Release();

    if (this->m_renderTargetTextureViewport != nullptr)
        this->m_renderTargetTextureViewport->Release();

    if (this->m_renderTargetView != nullptr)
        this->m_renderTargetView->Release();

    if (this->m_shaderResourceViewViewport != nullptr)
        this->m_shaderResourceViewViewport->Release();

    if (this->m_depthStencilState != nullptr)
        this->m_depthStencilState->Release();

    const UINT backBufferWidth = static_cast<UINT>(this->m_width);
    const UINT backBufferHeight = static_cast<UINT>(this->m_height);
    const DXGI_FORMAT backBufferFormat = DXGI_FORMAT_UNKNOWN;
    const DXGI_FORMAT depthBufferFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
    constexpr UINT backBufferCount = 0;

    // Obtain the backbuffer for this window which will be the final 3D rendertarget.

    // Allocate a 2-D surface as the depth/stencil buffer and
    // create a DepthStencil view on this surface to use on bind.
    CD3D11_TEXTURE2D_DESC depthStencilDesc(depthBufferFormat, backBufferWidth, backBufferHeight, 1, 1, D3D11_BIND_DEPTH_STENCIL);

    ComPtr<ID3D11Texture2D> depthStencil;
    DX::ThrowIfFailed(this->m_device->CreateTexture2D(&depthStencilDesc, nullptr, depthStencil.GetAddressOf()));

    DX::ThrowIfFailed(this->m_device->CreateDepthStencilView(depthStencil.Get(), nullptr, &this->m_depthStencilView));

    D3D11_TEXTURE2D_DESC textureDesc;
    D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
    D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;

    ///////////////////////// Map's Texture
    // Initialize the  texture description.
    ZeroMemory(&textureDesc, sizeof(textureDesc));

    // Setup the texture description.
    // We will have our map be a square
    // We will need to have this texture bound as a render target AND a shader resource
    textureDesc.Width = m_width;
    textureDesc.Height = m_height;
    textureDesc.MipLevels = 1;
    textureDesc.ArraySize = 1;
    textureDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
    textureDesc.SampleDesc.Count = 1;
    textureDesc.Usage = D3D11_USAGE_DEFAULT;
    textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
    textureDesc.CPUAccessFlags = 0;
    textureDesc.MiscFlags = 0;

    // Create the texture
    m_device->CreateTexture2D(&textureDesc, NULL, &this->m_renderTargetTextureViewport);

    /////////////////////// Map's Render Target
    // Setup the description of the render target view.
    renderTargetViewDesc.Format = textureDesc.Format;
    renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
    renderTargetViewDesc.Texture2D.MipSlice = 0;

    // Create the render target view.
    m_device->CreateRenderTargetView(this->m_renderTargetTextureViewport, &renderTargetViewDesc, &this->m_renderTargetView);

    /////////////////////// Map's Shader Resource View
    // Setup the description of the shader resource view.
    shaderResourceViewDesc.Format = textureDesc.Format;
    shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
    shaderResourceViewDesc.Texture2D.MipLevels = 1;

    // Create the shader resource view.
    m_device->CreateShaderResourceView(this->m_renderTargetTextureViewport, &shaderResourceViewDesc, &this->m_shaderResourceViewViewport);

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

    CD3D11_RASTERIZER_DESC rastDesc(D3D11_FILL_SOLID, D3D11_CULL_NONE, FALSE,
        D3D11_DEFAULT_DEPTH_BIAS, D3D11_DEFAULT_DEPTH_BIAS_CLAMP,
        D3D11_DEFAULT_SLOPE_SCALED_DEPTH_BIAS, TRUE, FALSE, TRUE, FALSE);

    auto device = this->m_device;
    auto width = this->m_width;
    auto height = this->m_height;

    CD3D11_TEXTURE2D_DESC rtDesc(DXGI_FORMAT_B8G8R8A8_UNORM,
        width, height, 1, 1,
        D3D11_BIND_RENDER_TARGET, D3D11_USAGE_DEFAULT, 0,
        MSAA_COUNT, MSAA_QUALITY);

    DX::ThrowIfFailed(
        device->CreateTexture2D(&rtDesc, nullptr,
            m_offscreenRenderTarget.ReleaseAndGetAddressOf()));

    CD3D11_RENDER_TARGET_VIEW_DESC rtvDesc(D3D11_RTV_DIMENSION_TEXTURE2DMS);

    DX::ThrowIfFailed(
        device->CreateRenderTargetView(m_offscreenRenderTarget.Get(),
            &rtvDesc,
            m_offscreenRenderTargetSRV.ReleaseAndGetAddressOf()));

    CD3D11_TEXTURE2D_DESC dsDesc(DXGI_FORMAT_D32_FLOAT,
        width, height, 1, 1,
        D3D11_BIND_DEPTH_STENCIL, D3D11_USAGE_DEFAULT, 0,
        MSAA_COUNT, MSAA_QUALITY);

    ComPtr<ID3D11Texture2D> depthBuffer;
    DX::ThrowIfFailed(
        device->CreateTexture2D(&dsDesc, nullptr, depthBuffer.GetAddressOf()));

    CD3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc(D3D11_DSV_DIMENSION_TEXTURE2DMS);

    DX::ThrowIfFailed(
        device->CreateDepthStencilView(depthBuffer.Get(),
            &dsvDesc,
            m_depthStencilSRV.ReleaseAndGetAddressOf()));
}

void Scene::Update()
{
    m_timer.Tick([&]()
    {
        float delta_time = float(m_timer.GetElapsedSeconds());

        this->m_camera.Update(this->m_width, this->m_height, delta_time);
    });

    this->m_world = Matrix::Identity;
    this->m_view = this->m_camera.m_view;
    this->m_proj = this->m_camera.m_proj;

    if (g_morphemeConnect.m_animPlayer.GetModel() && g_morphemeConnect.m_animPlayer.GetModel()->m_loaded)
        this->m_camera.SetTarget(g_morphemeConnect.m_animPlayer.GetModel()->m_focusPoint);

    this->CreateResources();

    this->Render();
}

void Scene::Clear()
{
    // Clear the views.
    float color[4] = { this->m_settings.m_backgroundColor.x, this->m_settings.m_backgroundColor.y, this->m_settings.m_backgroundColor.z, this->m_settings.m_backgroundColor.w };
    this->m_deviceContext->ClearRenderTargetView(this->m_renderTargetView, color);
    this->m_deviceContext->ClearDepthStencilView(this->m_depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

    this->m_deviceContext->OMSetDepthStencilState(this->m_depthStencilState, 0);
    this->m_deviceContext->OMSetRenderTargets(1, &this->m_renderTargetView, this->m_depthStencilView);

    // Set the viewport.
    D3D11_VIEWPORT viewport = { 0.0f, 0.0f, static_cast<float>(this->m_width), static_cast<float>(this->m_height), 0.f, 1.f };
    this->m_deviceContext->RSSetViewports(1, &viewport);

    this->m_texts.clear();
}

void Scene::Render()
{
    if (this->m_timer.GetFrameCount() == 0)
        return;

    this->Clear();

    if (m_renderTargetView)
    {
        ID3D11DeviceContext* context = this->m_deviceContext;

        context->OMSetBlendState(m_states->AlphaBlend(), nullptr, 0xFFFFFFFF);
        context->RSSetState(m_states->CullNone());

        m_effect->SetWorld(m_world);
        m_effect->SetView(m_view);
        m_effect->SetProjection(m_proj);
        m_effect->Apply(context);

        context->IASetInputLayout(m_inputLayout.Get());

        m_batch->Begin();

        DX::DrawGrid(this->m_batch.get(), this->m_settings.m_gridScale * Vector3::UnitX, this->m_settings.m_gridScale * Vector3::UnitZ, Vector3::Zero, 100, 100, Colors::Gray);      
        DX::DrawOriginMarker(this->m_batch.get(), Matrix::Identity, 0.5f, Colors::DarkCyan);
        
        CharacterDefBasic* characterDef = g_morphemeConnect.m_morphemeSystem.GetCharacterDef();

        if (characterDef != nullptr)
            this->DrawFlverModel(&g_morphemeConnect.m_animPlayer, characterDef->getNetworkDef()->getRig(0));

        m_batch->End();

        m_sprite.get()->Begin();

        for (size_t i = 0; i < this->m_texts.size(); i++)
            DX::AddWorldSpaceText(m_sprite.get(), m_font.get(), this->m_texts[i].m_text, Vector3::Zero, this->m_texts[i].m_position, this->m_camera, this->m_texts[i].m_color);

        m_sprite.get()->End();

        context->ResolveSubresource(this->m_renderTargetTextureViewport, 0,
            m_offscreenRenderTarget.Get(), 0,
            DXGI_FORMAT_B8G8R8A8_UNORM);
    }
}

void Scene::SetRenderResolution(int width, int height)
{
    this->m_width = (std::max)(width, 1);
    this->m_height = (std::max)(height, 1);
}

void Scene::AddText(std::string text, Matrix position, DirectX::XMVECTORF32 color)
{
    this->m_texts.push_back(TextItem(text, position, color));
}

void Scene::DrawFlverModel(AnimPlayer* animPlayer, MR::AnimRigDef* rig)
{
    FlverModel* model = animPlayer->GetModel();

    if (model == nullptr)
        return;

    Matrix world = Matrix::CreateTranslation(model->m_position) * Matrix::CreateScale(model->m_scale);    

    int boneCount = model->m_boneTransforms.size();

    int trajectoryBoneIndex = animPlayer->GetFlverBoneIndexByMorphemeBoneIndex(rig->getTrajectoryBoneIndex());
    int characterRootBoneIdx = animPlayer->GetFlverBoneIndexByMorphemeBoneIndex(rig->getCharacterRootBoneIndex());

    if (model->m_settings.m_selectedBone != -1)
    {
        DX::DrawSphere(this->m_batch.get(), model->m_boneTransforms[model->m_settings.m_selectedBone] * world, 0.03f, Colors::MediumAquamarine);
        this->AddText(RString::ToNarrow(model->m_flver->bones[model->m_settings.m_selectedBone].name).c_str(), model->m_boneTransforms[model->m_settings.m_selectedBone] * world);
    }

    if (model->m_settings.m_drawDummyPolygons)
    {
        for (size_t i = 0; i < model->m_flver->header.dummyCount; i++)
        {
            std::string dummy_name = "Dmy_" + std::to_string(model->m_flver->dummies[i].referenceID);

            DX::DrawReferenceFrame(this->m_batch.get(), model->m_dummyPolygons[i] * world);
            this->AddText(dummy_name.c_str(), model->m_dummyPolygons[i] * world);
        }
    }
    else if (model->m_settings.m_selectedDummy != -1)
    {
        std::string dummy_name = "Dmy_" + std::to_string(model->m_flver->dummies[model->m_settings.m_selectedDummy].referenceID);

        DX::DrawReferenceFrame(this->m_batch.get(), model->m_dummyPolygons[model->m_settings.m_selectedDummy] * world);
        this->AddText(dummy_name.c_str(), model->m_dummyPolygons[model->m_settings.m_selectedDummy] * world);
    }

    for (size_t i = 0; i < boneCount; i++)
    {
        int morphemeBoneIdx = animPlayer->GetFlverToMorphemeBoneMap()[i];

        if ((morphemeBoneIdx == -1) || (i == trajectoryBoneIndex) || (i == characterRootBoneIdx))
            continue;

        int parentIndex = model->m_flver->bones[i].parentIndex;

        if (parentIndex != -1)
        {
            Vector3 boneA = Vector3::Transform(Vector3::Zero, model->m_boneTransforms[i] * world);
            Vector3 boneB = Vector3::Transform(Vector3::Zero, model->m_boneTransforms[parentIndex] * world);

            DX::DrawLine(this->m_batch.get(), boneA, boneB, Colors::Orange);
        }
    }

    DX::DrawSphere(this->m_batch.get(), model->m_boneTransforms[characterRootBoneIdx] * world, 0.03f, Colors::MediumBlue);
    DX::DrawSphere(this->m_batch.get(), model->m_boneTransforms[trajectoryBoneIndex] * world, 0.03f, Colors::Red);

    if (!model->m_settings.m_xray)
        DX::DrawFlverModel(this->m_batch.get(), world, model);
}