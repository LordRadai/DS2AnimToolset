//--------------------------------------------------------------------------------------
// File: DebugDraw.h
//
// Helpers for drawing various debug shapes using PrimitiveBatch
//
// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.
//-------------------------------------------------------------------------------------

#pragma once
#include <DirectXColors.h>
#include <DirectXCollision.h>

#include "PrimitiveBatch.h"
#include "VertexTypes.h"
#include "BufferHelpers.h"
#include "CommonStates.h"
#include "DDSTextureLoader.h"
#include "DirectXHelpers.h"
#include "Effects.h"
#include "GamePad.h"
#include "GeometricPrimitive.h"
#include "GraphicsMemory.h"
#include "Keyboard.h"
#include "Model.h"
#include "Mouse.h"
#include "PostProcess.h"
#include "PrimitiveBatch.h"
#include "ScreenGrab.h"
#include "SimpleMath.h"
#include "SpriteBatch.h"
#include "SpriteFont.h"
#include "VertexTypes.h"
#include "WICTextureLoader.h"

#include "../Application/Application.h"
#include "../framework.h"
#include "../extern.h"
#include "../fromloader/fromloader.h"

#include "MorphemeSystem/MorphemeSystem.h"

class Camera;

#define CIRCLE_RESOLUTION 1000
#define SPHERE_STACKS 4
#define SPHERE_SLICES 12
#define CYLINDER_SLICES 12
#define ARC_SLICES 100
#define CAPSULE_SEG 12

#define VERTEX_BUF_SIZE 50000
#define LINE_INDEX_BUF_SIZE 300000
#define INDEX_BUF_SIZE 100000

enum TextFlags : BYTE
{
    TextFlags_None = 0,
    TextFlags_Shadow = 1 << 0,
    TextFlags_Outline = 1 << 1
};

namespace DX
{
    void XM_CALLCONV Draw(DirectX::PrimitiveBatch<DirectX::VertexPositionColor>* batch,
        const DirectX::BoundingSphere& sphere,
        DirectX::FXMVECTOR color = DirectX::Colors::White);

    void XM_CALLCONV Draw(DirectX::PrimitiveBatch<DirectX::VertexPositionColor>* batch,
        const DirectX::BoundingBox& box,
        DirectX::FXMVECTOR color = DirectX::Colors::White);

    void XM_CALLCONV Draw(DirectX::PrimitiveBatch<DirectX::VertexPositionColor>* batch,
        const DirectX::BoundingOrientedBox& obb,
        DirectX::FXMVECTOR color = DirectX::Colors::White);

    void XM_CALLCONV Draw(DirectX::PrimitiveBatch<DirectX::VertexPositionColor>* batch,
        const DirectX::BoundingFrustum& frustum,
        DirectX::FXMVECTOR color = DirectX::Colors::White);

    void XM_CALLCONV DrawGrid(DirectX::PrimitiveBatch<DirectX::VertexPositionColor>* batch,
        DirectX::FXMVECTOR xAxis, DirectX::FXMVECTOR yAxis,
        DirectX::FXMVECTOR origin, size_t xdivs, size_t ydivs,
        DirectX::GXMVECTOR color = DirectX::Colors::White);

    void XM_CALLCONV DrawRing(DirectX::PrimitiveBatch<DirectX::VertexPositionColor>* batch,
        DirectX::FXMVECTOR origin, DirectX::FXMVECTOR majorAxis, DirectX::FXMVECTOR minorAxis,
        DirectX::GXMVECTOR color = DirectX::Colors::White);

    void XM_CALLCONV DrawRay(DirectX::PrimitiveBatch<DirectX::VertexPositionColor>* batch,
        DirectX::FXMVECTOR origin, DirectX::FXMVECTOR direction, bool normalize = true,
        DirectX::FXMVECTOR color = DirectX::Colors::White);

    void XM_CALLCONV DrawTriangle(DirectX::PrimitiveBatch<DirectX::VertexPositionColor>* batch,
        DirectX::FXMVECTOR pointA, DirectX::FXMVECTOR pointB, DirectX::FXMVECTOR pointC,
        DirectX::GXMVECTOR color = DirectX::Colors::White);

    void XM_CALLCONV DrawQuad(DirectX::PrimitiveBatch<DirectX::VertexPositionColor>* batch,
        DirectX::FXMVECTOR pointA, DirectX::FXMVECTOR pointB, DirectX::FXMVECTOR pointC, DirectX::GXMVECTOR pointD,
        DirectX::HXMVECTOR color = DirectX::Colors::White);

    void XM_CALLCONV DrawLine(DirectX::PrimitiveBatch<DirectX::VertexPositionColor>* batch,
        DirectX::SimpleMath::Vector3 pointA, DirectX::SimpleMath::Vector3 pointB,
        DirectX::GXMVECTOR color = DirectX::Colors::White);

    void XM_CALLCONV DrawXMarker(DirectX::PrimitiveBatch<DirectX::VertexPositionColor>* batch,
        DirectX::XMMATRIX world, float size,
        DirectX::GXMVECTOR color = DirectX::Colors::White);

    void XM_CALLCONV DrawCircle(DirectX::PrimitiveBatch<DirectX::VertexPositionColor>* batch,
        DirectX::XMMATRIX world, float radius,
        DirectX::GXMVECTOR color = DirectX::Colors::White);

    void XM_CALLCONV DrawOriginMarker(DirectX::PrimitiveBatch<DirectX::VertexPositionColor>* batch,
        DirectX::XMMATRIX world, float size,
        DirectX::GXMVECTOR color = DirectX::Colors::White);

    void XM_CALLCONV DrawCapsule(DirectX::PrimitiveBatch<DirectX::VertexPositionColor>* batch,
        DirectX::XMMATRIX world, DirectX::SimpleMath::Vector3 pointA, DirectX::SimpleMath::Vector3 pointB, float radius,
        DirectX::GXMVECTOR color = DirectX::Colors::Yellow);

    void XM_CALLCONV DrawSphere(DirectX::PrimitiveBatch<DirectX::VertexPositionColor>* batch,
        DirectX::XMMATRIX world, float radius,
        DirectX::GXMVECTOR color = DirectX::Colors::Yellow);

    void XM_CALLCONV DrawCylinder(DirectX::PrimitiveBatch<DirectX::VertexPositionColor>* batch,
        DirectX::XMMATRIX world, DirectX::SimpleMath::Vector3 pointA, DirectX::SimpleMath::Vector3 pointB, float radius,
        DirectX::GXMVECTOR color = DirectX::Colors::Yellow, bool fill = false);

    void XM_CALLCONV DrawBox(DirectX::PrimitiveBatch<DirectX::VertexPositionColor>* batch,
        DirectX::XMMATRIX world, DirectX::SimpleMath::Vector3 center, DirectX::SimpleMath::Vector3 half_extents,
        DirectX::GXMVECTOR color = DirectX::Colors::Yellow, bool fill = false);

    void XM_CALLCONV DrawBoundingBox(DirectX::PrimitiveBatch<DirectX::VertexPositionColor>* batch,
        DirectX::XMMATRIX world, DirectX::SimpleMath::Vector3 center, DirectX::SimpleMath::Vector3 half_extents,
        DirectX::GXMVECTOR color = DirectX::Colors::Red);

    void XM_CALLCONV Draw3DArc(DirectX::PrimitiveBatch<DirectX::VertexPositionColor>* batch,
        DirectX::SimpleMath::Vector3 rotation, DirectX::SimpleMath::Vector3 center, float radius, float angle, float height, bool is_front,
        DirectX::GXMVECTOR color = DirectX::Colors::Yellow);

    void XM_CALLCONV DrawFlverModel(DirectX::PrimitiveBatch<DirectX::VertexPositionColor>* batch,
        DirectX::XMMATRIX world, FlverModel* model);

    void XM_CALLCONV DrawReferenceFrame(DirectX::PrimitiveBatch<DirectX::VertexPositionColor>* batch,
        DirectX::XMMATRIX world);

    void XM_CALLCONV AddOverlayText(DirectX::SpriteBatch* sprite, DirectX::SpriteFont* font, std::string text, DirectX::SimpleMath::Vector2 position, float depth, float scale, DirectX::XMVECTORF32 color, TextFlags flags);
    
    void XM_CALLCONV AddWorldSpaceText(DirectX::SpriteBatch* sprite, DirectX::SpriteFont* font, std::string text, DirectX::SimpleMath::Vector3 position, DirectX::XMMATRIX world, Camera cam, DirectX::XMVECTORF32 color);

}