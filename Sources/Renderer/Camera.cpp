/*************************************************************************
> File Name: Camera.cpp
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

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include <Maths/3DMaths.h>

#include "Camera.h"
#include "Renderer.h"

// Angle conversions
inline float DegreeToRadian(const float degrees)
{
	return (degrees * PI) / 180;
}

inline float RadianToDegree(const float radians)
{
	return (radians * 180) / PI;
}

// Constructor
Camera::Camera(Renderer* pRenderer) :
	m_pRenderer(pRenderer), m_zoomAmount(10.0f),
	m_minZoomAmount(0.0f), m_maxZoomAmount(100.0f)

{
	SetPosition(glm::vec3(0.0f, 0.0f, 0.0f));
	SetFacing(glm::vec3(0.0f, 0.0f, -1.0f));
	SetUp(glm::vec3(0.0f, 1.0f, 0.0f));
	SetRight(glm::vec3(1.0f, 0.0f, 0.0f));
}

const glm::vec3 Camera::GetPosition() const
{
	return m_position;
}

const glm::vec3 Camera::GetFakePosition() const
{
	return m_fakePosition;
}

const glm::vec3 Camera::GetFacing() const
{
	return glm::vec3(m_facing.x, m_facing.y, m_facing.z);
}

const glm::vec3 Camera::GetUp() const
{
	return m_up;
}

const glm::vec3 Camera::GetRight() const
{
	return m_right;
}

const glm::vec3 Camera::GetView() const
{
	return m_position + (m_facing * m_zoomAmount);
}

const float Camera::GetZoomAmount() const
{
	return m_zoomAmount;
}

void Camera::SetPosition(const glm::vec3& position)
{
	m_position = position;
}

void Camera::SetFakePosition(const glm::vec3& fakePosition)
{
	m_fakePosition = fakePosition;
}

void Camera::SetFacing(const glm::vec3& facing)
{
	m_facing = facing;
}

void Camera::SetUp(const glm::vec3& up)
{
	m_up = up;
}

void Camera::SetRight(const glm::vec3& right)
{
	m_right = right;
}

const void Camera::SetZoomAmount(float amount)
{
	m_zoomAmount = amount;
}

// Camera movement
void Camera::Fly(const float speed, bool useFakePosition)
{
	if (useFakePosition)
	{
		m_fakePosition = m_fakePosition + m_facing * speed;
	}
	else
	{
		m_position = m_position + m_facing * speed;
	}
}

void Camera::Move(const float speed, bool useFakePosition)
{
	glm::vec3 directionToMove = m_facing;
	directionToMove.y = 0.0f;
	directionToMove = normalize(directionToMove);

	if (useFakePosition)
	{
		m_fakePosition.x = m_fakePosition.x + directionToMove.x * speed;
		m_fakePosition.z = m_fakePosition.z + directionToMove.z * speed;
	}
	else
	{
		m_position.x = m_position.x + directionToMove.x * speed;
		m_position.z = m_position.z + directionToMove.z * speed;
	}
}

void Camera::Levitate(const float speed, bool useFakePosition)
{
	if (useFakePosition)
	{
		m_fakePosition.y = m_fakePosition.y + 1.0f * speed;
	}
	else
	{
		m_position.y = m_position.y + 1.0f * speed;
	}
}

void Camera::Strafe(const float speed, bool useFakePosition)
{
	if (useFakePosition)
	{
		m_fakePosition.x = m_fakePosition.x + m_right.x * speed;
		m_fakePosition.y = m_fakePosition.y + m_right.y * speed;
		m_fakePosition.z = m_fakePosition.z + m_right.z * speed;
	}
	else
	{
		m_position.x = m_position.x + m_right.x * speed;
		m_position.y = m_position.y + m_right.y * speed;
		m_position.z = m_position.z + m_right.z * speed;
	}
}

void Camera::Rotate(const float xAmount, const float yAmount, const float zAmount)
{
	glm::quat xRotation = angleAxis(DegreeToRadian(xAmount), m_right);
	glm::quat yRotation = angleAxis(DegreeToRadian(yAmount), m_up);
	glm::quat zRotation = angleAxis(DegreeToRadian(zAmount), m_facing);

	glm::quat rotation = xRotation * yRotation * zRotation;

	m_right = normalize(rotation * m_right);
	m_up = normalize(rotation * m_up);
	m_facing = normalize(rotation * m_facing);
}

void Camera::RotateY(const float yAmount)
{
	glm::quat rotation = angleAxis(DegreeToRadian(yAmount), glm::vec3(0.0f, 1.0f, 0.0f));

	m_right = normalize(rotation * m_right);
	m_up = normalize(rotation * m_up);
	m_facing = normalize(rotation * m_facing);
}

void Camera::RotateAroundPoint(const float xAmount, const float yAmount, const float zAmount, bool useFakePosition)
{
	glm::quat xRotation = angleAxis(DegreeToRadian(xAmount), m_right);
	glm::quat yRotation = angleAxis(DegreeToRadian(yAmount), m_up);
	glm::quat zRotation = angleAxis(DegreeToRadian(zAmount), m_facing);

	glm::quat rotation = xRotation * yRotation * zRotation;

	if (useFakePosition)
	{
		// Get the view position, based on the facing and the zoom amount
		glm::vec3 view = m_fakePosition + (m_facing * m_zoomAmount);

		// Translate the position to the origin, relative to the view position (that is the facing zoomed)
		m_fakePosition -= view;  
		m_fakePosition = rotation * m_fakePosition;
		// Translate back to relative view position
		m_fakePosition += view;
	}
	else
	{
		// Get the view position, based on the facing and the zoom amount
		glm::vec3 view = m_position + (m_facing * m_zoomAmount);

		// Translate the position to the origin, relative to the view position (that is the facing zoomed)
		m_position -= view;
		m_position = rotation * m_position;
		// Translate back to relative view position
		m_position += view;
	}

	m_right = normalize(rotation * m_right);
	m_facing = normalize(rotation * m_facing);
	m_up = normalize(rotation * m_up);
}

void Camera::RotateAroundPointY(const float yAmount, bool useFakePosition)
{
	glm::quat rotation = angleAxis(DegreeToRadian(yAmount), glm::vec3(0.0f, 1.0f, 0.0f));

	if (useFakePosition)
	{
		// Get the view position, based on the facing and the zoom amount
		glm::vec3 view = m_fakePosition + (m_facing * m_zoomAmount);

		// Translate the position to the origin, relative to the view position (that is the facing zoomed)
		m_fakePosition -= view;
		m_fakePosition = (rotation * m_fakePosition);
		// Translate back to relative view position
		m_fakePosition += view;
	}
	else
	{
		// Get the view position, based on the facing and the zoom amount
		glm::vec3 view = m_position + (m_facing * m_zoomAmount);

		// Translate the position to the origin, relative to the view position (that is the facing zoomed)
		m_position -= view;
		m_position = (rotation * m_position);
		// Translate back to relative view position
		m_position += view;
	}

	m_right = normalize(rotation * m_right);
	m_facing = normalize(rotation * m_facing);
	m_up = normalize(rotation * m_up);
}

void Camera::Zoom(const float amount, bool useFakePosition)
{
	m_zoomAmount += amount;
	if (m_zoomAmount <= m_minZoomAmount)
	{
		m_zoomAmount = m_minZoomAmount;
	}
	else if (m_zoomAmount >= m_maxZoomAmount)
	{
		m_zoomAmount = m_maxZoomAmount;
	}
	else
	{
		if (useFakePosition)
		{
			m_fakePosition = m_fakePosition - m_facing * amount;
		}
		else
		{
			m_position = m_position - m_facing * amount;
		}
	}
}

// Viewing
void Camera::Look() const
{
	glm::vec3 view = m_position + m_facing;
	gluLookAt(m_position.x, m_position.y, m_position.z, view.x, view.y, view.z, m_up.x, m_up.y, m_up.z);
	m_pRenderer->GetFrustum(m_pRenderer->GetActiveViewport())->SetCamera(m_position, view, m_up);
}