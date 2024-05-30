#include <SimpleMath.h>

class Camera
{
public:
	struct Settings
	{
		struct CameraSpeed
		{
			float m_zoomSpeed = 10.f;
			float m_dragSpeed = 0.3f;
		} m_speedParams;

		bool m_dragInvertX = true;
		bool m_dragInvertY = false;
		bool m_rotInvertX = false;
		bool m_rotInvertY = false;
	} m_settings;

	DirectX::SimpleMath::Matrix m_proj;
	DirectX::SimpleMath::Matrix m_view;
	DirectX::SimpleMath::Vector3 m_position;
	DirectX::SimpleMath::Vector3 m_angles;
	DirectX::SimpleMath::Vector3 m_lookAt;
	DirectX::SimpleMath::Vector3 m_targetPos;
	DirectX::SimpleMath::Vector3 m_focus;
	DirectX::SimpleMath::Vector3 m_offsets;

	float m_width, m_height;
	float m_fov;
	float m_farZ, m_nearZ;
	float m_aspectRatio;
	float m_radius;
	bool m_registerInput;

	Camera();
	~Camera();

	void Update(float width, float height, float delta_time);
	void CameraInput(float delta_time);
	void UpdateRadius(float speed, float delta_time);
	void UpdateTargetAngleXZ(float omega, float delta_time);
	void UpdateTargetAngleY(float omega, float delta_time);
	void UpdatePosition(DirectX::SimpleMath::Vector3 speed, float delta_time);
	void UpdateTargetPosition(DirectX::SimpleMath::Vector3 speed, float delta_time);
	void SetTarget(DirectX::SimpleMath::Vector3 target_pos);
};