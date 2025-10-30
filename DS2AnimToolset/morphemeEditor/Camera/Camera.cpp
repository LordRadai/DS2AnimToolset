#include "Camera.h"
#include "framework.h"
#include "extern.h"
#include "imgui/imgui.h"
#include "RenderManager/RenderManager.h"

using namespace DirectX;
using namespace SimpleMath;

Camera::Camera()
{
	this->m_settings = new Settings;

	this->m_registerInput = false;

	this->m_targetPos = Vector3::Zero;
	this->m_offset = Vector3::Zero;

	this->m_radius = 5.f;

	this->m_angles = Vector3(XM_PI / 3, 0.4f, 0);
	this->m_position = m_targetPos + Vector3(m_radius * cosf(m_angles.y) * cosf(m_angles.x), m_radius * sinf(m_angles.y), m_radius * cosf(m_angles.y) * sinf(m_angles.x));
	this->m_upAxis = RenderManager::getInstance()->getUpAxis();

	this->m_cameraView = kCamViewPerspective;

	this->m_nearZ = 0.1f;
	this->m_farZ = 5000.f;

	this->m_fov = XM_PI / 3;

	this->m_width = 1920;
	this->m_height = 1080;

	this->m_aspectRatio = this->m_width / this->m_height;

	this->m_view = Matrix::CreateLookAt(this->m_position, this->m_targetPos, this->m_upAxis);
	this->m_proj = Matrix::CreatePerspectiveFieldOfView(this->m_fov, this->m_aspectRatio, this->m_nearZ, this->m_farZ);
}

Camera::~Camera()
{
}

void Camera::update(float width, float height, float delta_time)
{
	this->m_width = width;
	this->m_height = height;
	this->m_aspectRatio = this->m_width / this->m_height;

	switch (this->m_cameraView)
	{
	case kCamViewFront:
		this->m_angles = Vector3(XM_PIDIV2, 0.f, 0.f);
		this->m_offset = Vector3::Zero;
		break;
	case kCamViewTop:
		this->m_angles = Vector3(0.f, XM_PIDIV2, 0.f);
		this->m_offset = Vector3::Zero;
		break;
	case kCamViewSide:
		this->m_angles = Vector3(0.f, 0.f, 0.f);
		this->m_offset = Vector3::Zero;
		break;
	default:
		break;
	}

	this->handleInput(delta_time);

	this->m_focus = this->m_targetPos + this->m_offset;
	this->m_position = this->m_focus + Vector3(m_radius * cosf(m_angles.y) * cosf(m_angles.x), m_radius * sinf(m_angles.y), m_radius * cosf(m_angles.y) * sinf(m_angles.x));

	this->m_view = Matrix::CreateLookAt(this->m_position, this->m_focus, this->m_upAxis);
	this->m_proj = Matrix::CreatePerspectiveFieldOfView(this->m_fov, this->m_aspectRatio, this->m_nearZ, this->m_farZ);
}

void Camera::handleInput(float delta_time)
{
	bool register_input = false;
	static ImVec2 old_mouse_pos;

	if (this->m_registerInput == false)
	{
		old_mouse_pos = ImGui::GetMousePos();
		return;
	}

	ImGuiIO& io = ImGui::GetIO();

	if (this->m_cameraView == kCamViewPerspective)
	{
		if (io.MouseDown[0])
		{
			Vector2 drag_delta(ImGui::GetMousePos().x - old_mouse_pos.x, ImGui::GetMousePos().y - old_mouse_pos.y);

			this->updateVerticalAngle(this->m_settings->rotSpeed * drag_delta.y, delta_time);
			this->updatePlaneAngle(this->m_settings->rotSpeed * drag_delta.x, delta_time);

			register_input = true;
		}

		if (io.MouseDown[1])
		{
			Vector2 drag_delta(ImGui::GetMousePos().x - old_mouse_pos.x, ImGui::GetMousePos().y - old_mouse_pos.y);

			this->updateTargetPosition(Vector3(this->m_settings->moveSpeed * drag_delta.x, this->m_settings->moveSpeed * drag_delta.y, this->m_settings->moveSpeed * drag_delta.x), delta_time);

			register_input = true;
		}
	}

	if ((io.MouseWheel > FLT_EPSILON) || (io.MouseWheel < -FLT_EPSILON))
	{
		float zoomSpeed = this->m_settings->zoomSpeed;
		float fastZoomSpeed = this->m_settings->zoomSpeed * 5.f;

		if (io.KeyShift)
			zoomSpeed = fastZoomSpeed;

		this->updateRadius(-io.MouseWheel * zoomSpeed, delta_time);

		register_input = true;
	}

	old_mouse_pos = ImGui::GetMousePos();

	this->m_registerInput = register_input;
}

void Camera::updateRadius(float speed, float delta_time)
{
	constexpr float maxRadius = 9999.f;

	if (this->m_radius + speed * delta_time > 0.1f && this->m_radius + speed * delta_time < maxRadius)
		this->m_radius += speed * delta_time;

	if (this->m_radius + speed * delta_time > maxRadius)
		this->m_radius = maxRadius;
}

void Camera::updateVerticalAngle(float omega, float delta_time)
{
	float deltaPitch = omega * delta_time;

	if (this->m_settings->rotInvertY)
		deltaPitch = -deltaPitch;

	Vector3 offset = m_position - m_targetPos;
	Quaternion pitchRot = Quaternion::CreateFromAxisAngle(m_upAxis, deltaPitch);

	offset = Vector3::Transform(offset, pitchRot);

	m_position = m_targetPos + offset;

	m_angles.y += deltaPitch;
	float maxPitch = XM_PIDIV2 - FLT_EPSILON;

	if (m_angles.y > maxPitch)
		m_angles.y = maxPitch;

	if (m_angles.y < -maxPitch)
		m_angles.y = -maxPitch;
}

void Camera::updatePlaneAngle(float omega, float delta_time)
{
	float deltaYaw = omega * delta_time;

	if (m_settings->rotInvertX)
		deltaYaw = -deltaYaw;

	Vector3 offset = m_position - m_targetPos;

	Quaternion yawRot = Quaternion::CreateFromAxisAngle(m_upAxis, deltaYaw);

	offset = Vector3::Transform(offset, yawRot);

	m_position = m_targetPos + offset;

	m_angles.x += deltaYaw;

	if (m_angles.x > XM_2PI)
		m_angles.x -= XM_2PI;

	if (m_angles.x < 0)
		m_angles.x += XM_2PI;
}

void Camera::updatePosition(DirectX::SimpleMath::Vector3 speed, float delta_time)
{
	this->m_position += speed * delta_time;
}

void Camera::updateTargetPosition(DirectX::SimpleMath::Vector3 speed, float delta_time)
{
	Vector3 deltaPos = Vector3(speed.x * delta_time * cosf(this->m_angles.y) * cosf(this->m_angles.x), speed.y * delta_time, speed.z * delta_time * cosf(this->m_angles.y) * sinf(this->m_angles.x));
	
	if (this->m_settings->dragInvertX)
		deltaPos = Vector3::Transform(deltaPos, Matrix::CreateRotationY(-XM_PIDIV2));
	else
		deltaPos = Vector3::Transform(deltaPos, Matrix::CreateRotationY(XM_PIDIV2));

	if (this->m_settings->dragInvertY)
		deltaPos = Vector3::Transform(deltaPos, Matrix::CreateRotationX(XM_PI));

	this->m_offset += deltaPos;
}