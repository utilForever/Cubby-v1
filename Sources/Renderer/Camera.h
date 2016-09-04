/*************************************************************************
> File Name: Camera.h
> Project Name: Cubby
> Author: Chan-Ho Chris Ohk
> Purpose:
>    A quaternion based camera class that encapsulates camera and looking
>    functionality. Ability to move and rotate freely with 6 degrees of freedom.
>    Allows for movement, rotation (including arcball rotation around a point),
>    zooming and strafing along the camera's right vector.
> Created Time: 2016/06/26
> Copyright (c) 2016, Chan-Ho Chris Ohk
*************************************************************************/

#ifndef CUBBY_CAMERA_H
#define CUBBY_CAMERA_H

#include <glm/vec3.hpp>

// Forward declaration
class Renderer;

class Camera
{
public:
	// Constructor
	Camera(Renderer* pRenderer);

	// Getter, Setter
	const glm::vec3 GetPosition() const;
	const glm::vec3 GetFakePosition() const;
	const glm::vec3 GetFacing() const;
	const glm::vec3 GetUp() const;
	const glm::vec3 GetRight() const;
	const glm::vec3 GetView() const;
	const float GetZoomAmount() const;
	void SetPosition(const glm::vec3& position);
	void SetFakePosition(const glm::vec3& fakePosition);
	void SetFacing(const glm::vec3& facing);
	void SetUp(const glm::vec3& up);
	void SetRight(const glm::vec3& right);
	const void SetZoomAmount(float amount);

	// Camera movement
	void Fly(const float speed, bool useFakePosition = false);
	void Move(const float speed, bool useFakePosition = false);
	void Levitate(const float speed, bool useFakePosition = false);
	void Strafe(const float speed, bool useFakePosition = false);
	void Rotate(const float xAmount, const float yAmount, const float zAmount);
	void RotateY(const float yAmount);
	void RotateAroundPoint(const float xAmount, const float yAmount, const float zAmount, bool useFakePosition = false);
	void RotateAroundPointY(const float yAmount, bool useFakePosition = false);
	void Zoom(const float amount, bool useFakePosition = false);

	// Viewing
	void Look() const;

private:
	Renderer* m_pRenderer;

	// The camera's world position
	glm::vec3 m_position;
	glm::vec3 m_fakePosition;

	// Local up vector
	glm::vec3 m_up;
	// Local facing vector
	glm::vec3 m_facing;
	// Local right vector
	glm::vec3 m_right;

	// Zoom values used when rotating around a point, zoom is essentially how much projection into the
	// facing direction we are looking at, since facing is a local unit vector relative to the camera.
	float m_zoomAmount;
	float m_minZoomAmount;
	float m_maxZoomAmount;
};

#endif