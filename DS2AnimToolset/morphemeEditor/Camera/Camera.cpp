#include "Camera.h"
#include "framework.h"
#include "extern.h"
#include "imgui/imgui.h"

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
	this->m_lookAt = Vector3::UnitY;

	this->m_nearZ = 0.1f;
	this->m_farZ = 5000.f;

	this->m_fov = XM_PI / 3;

	this->m_width = 1920;
	this->m_height = 1080;

	this->m_aspectRatio = this->m_width / this->m_height;

	this->m_view = Matrix::CreateLookAt(this->m_position, this->m_targetPos, this->m_lookAt);
	this->m_proj = Matrix::CreatePerspectiveFieldOfView(this->m_fov, this->m_aspectRatio, this->m_nearZ, this->m_farZ);
}

Camera::~Camera()
{
	this->m_registerInput = false;

	this->m_targetPos = Vector3::Zero;
	this->m_radius = 0;

	this->m_angles = Vector3::Zero;
	this->m_position = Vector3::Zero;
	this->m_lookAt = Vector3::UnitY;

	this->m_nearZ = 0;
	this->m_farZ = 0;

	this->m_fov = 0;

	this->m_width = 0;
	this->m_height = 0;

	this->m_aspectRatio = this->m_width / this->m_height;

	this->m_view = Matrix::Identity;
	this->m_proj = Matrix::Identity;
}

void Camera::update(float width, float height, float delta_time)
{
	this->m_width = width;
	this->m_height = height;
	this->m_aspectRatio = this->m_width / this->m_height;

	this->handleInput(delta_time);

	this->m_focus = this->m_targetPos + this->m_offset;
	this->m_position = this->m_focus + Vector3(m_radius * cosf(m_angles.y) * cosf(m_angles.x), m_radius * sinf(m_angles.y), m_radius * cosf(m_angles.y) * sinf(m_angles.x));

	this->m_view = Matrix::CreateLookAt(this->m_position, this->m_focus, this->m_lookAt);
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

	if (io.MouseDown[0])
	{
		Vector2 drag_delta(ImGui::GetMousePos().x - old_mouse_pos.x, ImGui::GetMousePos().y - old_mouse_pos.y);

		this->updateTargetAngleXZ(this->m_settings->dragSpeed * drag_delta.y, delta_time);
		this->updateTargetAngleY(this->m_settings->dragSpeed * drag_delta.x, delta_time);

		register_input = true;
	}

	if (io.MouseDown[1])
	{
		Vector2 drag_delta(ImGui::GetMousePos().x - old_mouse_pos.x, ImGui::GetMousePos().y - old_mouse_pos.y);

		this->updateTargetPosition(Vector3(this->m_settings->dragSpeed * drag_delta.x, this->m_settings->dragSpeed * drag_delta.y, this->m_settings->dragSpeed * drag_delta.x), delta_time);

		register_input = true;
	}

	if ((io.MouseWheel > FLT_EPSILON) || (io.MouseWheel < -FLT_EPSILON))
	{
		this->updateRadius(-io.MouseWheel * this->m_settings->zoomSpeed, delta_time);

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

void Camera::updateTargetAngleXZ(float omega, float delta_time)
{
	if (this->m_settings->rotInvertY)
		this->m_angles.y += -omega * delta_time;
	else
		this->m_angles.y += omega * delta_time;

	if (this->m_angles.y > XM_PIDIV2)
		this->m_angles.y = XM_PIDIV2 - FLT_EPSILON;

	if (this->m_angles.y < -XM_PIDIV2)
		this->m_angles.y = -XM_PIDIV2 + FLT_EPSILON;
}

void Camera::updateTargetAngleY(float omega, float delta_time)
{
	if (this->m_settings->rotInvertX)
		this->m_angles.x += -omega * delta_time;
	else
		this->m_angles.x += omega * delta_time;

	if (this->m_angles.x > XM_2PI)
		this->m_angles.x -= XM_2PI;

	if (this->m_angles.x < 0)
		this->m_angles.x += XM_2PI;
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