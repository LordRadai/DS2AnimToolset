#pragma once
#include <SimpleMath.h>

using namespace DirectX::SimpleMath;

class Camera
{
public:
	struct Settings
	{
		float zoomSpeed = 10.f;
		float dragSpeed = 0.3f;
		bool dragInvertX = true;
		bool dragInvertY = false;
		bool rotInvertX = false;
		bool rotInvertY = false;
	};

	enum CameraView
	{
		kCamViewPerspective,
		kCamViewSide,
		kCamViewFront,
		kCamViewTop,

		kCamViewMax
	};

	Camera();
	~Camera();

	void update(float width, float height, float delta_time);

	void setRadius(float radius) { this->m_radius = radius; }
	void setFov(float fov) { this->m_fov = fov; }
	void setTarget(DirectX::SimpleMath::Vector3 target_pos) { this->m_targetPos = target_pos; }
	void setOffset(DirectX::SimpleMath::Vector3 offset) { this->m_offset = offset; }
	void setAngles(DirectX::SimpleMath::Vector3 angles) { this->m_angles = angles; }
	void setInputEnabled(bool enabled) { this->m_registerInput = enabled; }
	void setResolution(int width, int height) { this->m_width = width; this->m_height = height; }
	void setCameraView(CameraView view) { this->m_cameraView = view; }

	float getRadius() const { return this->m_radius; }
	float getFov() const { return this->m_fov; }
	Vector3 getTargetPos() const { return this->m_targetPos; }
	Vector3 getPosition() const { return this->m_position; }
	Vector3 getOffset() const { return this->m_offset; }
	Matrix getProjectionMatrix() const { return this->m_proj; }
	Matrix getViewMatrix() const { return this->m_view; }
	Settings* getCameraSettings() const { return this->m_settings; }
	bool isInputEnabled() const { return this->m_registerInput; }
	int getWidth() const { return this->m_width; }
	int getHeight() const { return this->m_height; }
	float getNearZ() const { return this->m_nearZ; }
	float getFarZ() const { return this->m_farZ; }
	CameraView getCameraView() const { return this->m_cameraView; }

private:
	Settings* m_settings;
	DirectX::SimpleMath::Matrix m_proj;
	DirectX::SimpleMath::Matrix m_view;
	DirectX::SimpleMath::Vector3 m_position;
	DirectX::SimpleMath::Vector3 m_angles;
	DirectX::SimpleMath::Vector3 m_upAxis;
	DirectX::SimpleMath::Vector3 m_targetPos;
	DirectX::SimpleMath::Vector3 m_focus;
	DirectX::SimpleMath::Vector3 m_offset;
	CameraView m_cameraView;

	float m_width, m_height;
	float m_fov;
	float m_farZ, m_nearZ;
	float m_aspectRatio;
	float m_radius;
	bool m_registerInput;

	void handleInput(float delta_time);
	void updateRadius(float speed, float delta_time);
	void updateVerticalAngle(float omega, float delta_time);
	void updatePlaneAngle(float omega, float delta_time);
	void updatePosition(DirectX::SimpleMath::Vector3 speed, float delta_time);
	void updateTargetPosition(DirectX::SimpleMath::Vector3 speed, float delta_time);
};