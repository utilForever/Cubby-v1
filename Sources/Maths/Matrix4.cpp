/*************************************************************************
> File Name: Matrix4.h
> Project Name: Cubby
> Author: Chan-Ho Chris Ohk
> Purpose: 3d maths - Matrix4 library.
> Created Time: 2016/06/18
> Copyright (c) 2016, Chan-Ho Chris Ohk
*************************************************************************/

#include <cstring>
#include <cmath>

#include <glm/glm.hpp>

#include "3DMaths.h"
#include "Matrix4.h"

// Constructors
Matrix4::Matrix4()
{
	LoadIdentity();
}

Matrix4::Matrix4(float data[16])
{
	memcpy(this->m_data, data, 16 * sizeof(float));
}

// Setup matrices
void Matrix4::SetXRotation(const float x)
{
	float cosAngle = cos(x);
	float sinAngle = sin(x);

	if (fabs(cosAngle) < 1.0e-7f)
	{
		cosAngle = 0.0f;
	}

	if (fabs(sinAngle) < 1.0e-7f)
	{
		sinAngle = 0.0f;
	}

	m_data[5] = cosAngle;
	m_data[6] = sinAngle;
	m_data[9] = -sinAngle;
	m_data[10] = cosAngle;
}

void Matrix4::SetYRotation(const float y)
{
	float cosAngle = cos(y);
	float sinAngle = sin(y);

	if (fabs(cosAngle) < 1.0e-7f)
	{
		cosAngle = 0.0f;
	}

	if (fabs(sinAngle) < 1.0e-7f)
	{
		sinAngle = 0.0f;
	}

	m_data[0] = cosAngle;
	m_data[2] = -sinAngle;
	m_data[8] = sinAngle;
	m_data[10] = cosAngle;
}

void Matrix4::SetZRotation(const float z)
{
	float cosAngle = cos(z);
	float sinAngle = sin(z);

	if (fabs(cosAngle) < 1.0e-7f)
	{
		cosAngle = 0.0f;
	}

	if (fabs(sinAngle) < 1.0e-7f)
	{
		sinAngle = 0.0f;
	}

	m_data[0] = cosAngle;
	m_data[1] = sinAngle;
	m_data[4] = -sinAngle;
	m_data[5] = cosAngle;
}

void Matrix4::SetRotation(const float x, const float y, const float z)
{
	Matrix4 rotationX, rotationY, rotationZ;

	rotationX.SetXRotation(x);
	rotationY.SetYRotation(y);
	rotationZ.SetZRotation(z);

	// Possible gimbal lock?
	*this = rotationX * rotationY * rotationZ;
}

void Matrix4::SetTranslation(float translation[3])
{
	m_data[12] = translation[0];
	m_data[13] = translation[1];
	m_data[14] = translation[2];
}

void Matrix4::SetTranslation(glm::vec3 translation)
{
	m_data[12] = translation.x;
	m_data[13] = translation.y;
	m_data[14] = translation.z;
}

void Matrix4::SetScale(glm::vec3 scale)
{
	m_data[0] = scale.x;
	m_data[5] = scale.y;
	m_data[10] = scale.z;
}

void Matrix4::AddTranslation(float* translation)
{
	m_data[12] = translation[0];
	m_data[13] = translation[1];
	m_data[14] = translation[2];
}

void Matrix4::AddRotationByRadians(float* angles)
{
	float cosRoll  = cos(angles[0]);
	float sinRoll  = sin(angles[0]);
	float cosPitch = cos(angles[1]);
	float sinPitch = sin(angles[1]);
	float cosYaw   = cos(angles[2]);
	float sinYaw   = sin(angles[2]);

	m_data[0] = cosPitch * cosYaw;
	m_data[1] = cosPitch * sinYaw;
	m_data[2] = -sinPitch;

	double sinRollsinPitch = sinRoll * sinPitch;
	double cosRollsinPitch = cosRoll * sinPitch;

	m_data[4] = sinRollsinPitch * cosYaw - cosRoll * sinYaw;
	m_data[5] = sinRollsinPitch * sinYaw + cosRoll * cosYaw;
	m_data[6] = sinRoll * cosPitch;

	m_data[8] = cosRollsinPitch * cosYaw + sinRoll * sinYaw;
	m_data[9] = cosRollsinPitch * sinYaw - sinRoll * cosYaw;
	m_data[10] = cosRoll * cosPitch;
}

// Properties
void Matrix4::GetMatrix(float* matrix) const
{
	matrix[0] = this->m_data[0];	matrix[1] = this->m_data[1];	matrix[2] = this->m_data[2];	matrix[3] = this->m_data[3];
	matrix[4] = this->m_data[4];	matrix[5] = this->m_data[5];	matrix[6] = this->m_data[6];	matrix[7] = this->m_data[7];
	matrix[8] = this->m_data[8];	matrix[9] = this->m_data[9];	matrix[10] = this->m_data[10];	matrix[11] = this->m_data[11];
	matrix[12] = this->m_data[12];	matrix[13] = this->m_data[13];	matrix[14] = this->m_data[14];	matrix[15] = this->m_data[15];
}

const float Matrix4::GetDeterminant() const
{
	float value = 
		m_data[3] * m_data[6] * m_data[9]  * m_data[12] - m_data[2] * m_data[7] * m_data[9]  * m_data[12] - m_data[3] * m_data[5] * m_data[10] * m_data[12] + m_data[1] * m_data[7] * m_data[10] * m_data[12] +
		m_data[2] * m_data[5] * m_data[11] * m_data[12] - m_data[1] * m_data[6] * m_data[11] * m_data[12] - m_data[3] * m_data[6] * m_data[8]  * m_data[13] + m_data[2] * m_data[7] * m_data[8]  * m_data[13] +
		m_data[3] * m_data[4] * m_data[10] * m_data[13] - m_data[0] * m_data[7] * m_data[10] * m_data[13] - m_data[2] * m_data[4] * m_data[11] * m_data[13] + m_data[0] * m_data[6] * m_data[11] * m_data[13] +
		m_data[3] * m_data[5] * m_data[8]  * m_data[14] - m_data[1] * m_data[7] * m_data[8]  * m_data[14] - m_data[3] * m_data[4] * m_data[9]  * m_data[14] + m_data[0] * m_data[7] * m_data[9]  * m_data[14] +
		m_data[1] * m_data[4] * m_data[11] * m_data[14] - m_data[0] * m_data[5] * m_data[11] * m_data[14] - m_data[2] * m_data[5] * m_data[8]  * m_data[15] + m_data[1] * m_data[6] * m_data[8]  * m_data[15] +
		m_data[2] * m_data[4] * m_data[9]  * m_data[15] - m_data[0] * m_data[6] * m_data[9]  * m_data[15] - m_data[1] * m_data[4] * m_data[10] * m_data[15] + m_data[0] * m_data[5] * m_data[10] * m_data[15];

	return value;
}

const Matrix4 Matrix4::GetNegative() const
{
	Matrix4 negative = *this;

	negative.Negate();

	return negative;
}

const Matrix4 Matrix4::GetTranspose() const
{
	Matrix4 transpose = *this;

	transpose.Transpose();

	return transpose;
}

const Matrix4 Matrix4::GetInverse() const
{
	Matrix4 inverse = *this;

	inverse.Inverse();

	return inverse;
}

const Matrix4 Matrix4::GetOrthoNormal() const
{
	Matrix4 normal = *this;

	normal.OrthoNormalize();

	return normal;
}

const glm::vec3 Matrix4::GetRightVector() const
{
	return glm::vec3(m_data[0], m_data[1], m_data[2]);
}

const glm::vec3 Matrix4::GetUpVector() const
{
	return glm::vec3(m_data[4], m_data[5], m_data[6]);
}

const glm::vec3 Matrix4::GetForwardVector() const
{
	return glm::vec3(m_data[8], m_data[9], m_data[10]);
}

const glm::vec3 Matrix4::GetTranslationVector() const
{
	return glm::vec3(m_data[12], m_data[13], m_data[14]);
}

const void Matrix4::GetEuler(float* x, float* y, float* z) const
{
	// Assuming the angles are in radians
	if (m_data[6] > 0.998f || m_data[6] < -0.998f)
	{
		*y = atan2(-m_data[8], m_data[0]);
		*z = 0.0f;
	}
	else
	{
		*y = atan2(m_data[2], m_data[10]);
		*z = atan2(m_data[4], m_data[5]);
	}

	*x = asin(m_data[6]);
}

// Operations
void Matrix4::LoadIdentity()
{
	memset(m_data, 0, 16 * sizeof(float));

	m_data[0] = 1;
	m_data[5] = 1;
	m_data[10] = 1;
	m_data[15] = 1;
}

void Matrix4::Negate()
{
	for (int i = 0; i < 16; ++i)
	{
		m_data[i] = -m_data[i];
	}
}

void Matrix4::Transpose()
{
	Swap(m_data[1], m_data[4]);
	Swap(m_data[2], m_data[8]);
	Swap(m_data[3], m_data[12]);
	Swap(m_data[6], m_data[9]);
	Swap(m_data[7], m_data[13]);
	Swap(m_data[11], m_data[14]);
}

void Matrix4::Inverse()
{
	float det = GetDeterminant();

	Matrix4 matrixCopy;
	memcpy(matrixCopy.m_data, this->m_data, 16 * sizeof(float));

	m_data[0]  = matrixCopy.m_data[6] * matrixCopy.m_data[11] * matrixCopy.m_data[13] - matrixCopy.m_data[7] * matrixCopy.m_data[10] * matrixCopy.m_data[13] + matrixCopy.m_data[7] * matrixCopy.m_data[9] * matrixCopy.m_data[14] - matrixCopy.m_data[5] * matrixCopy.m_data[11] * matrixCopy.m_data[14] - matrixCopy.m_data[6] * matrixCopy.m_data[9] * matrixCopy.m_data[15] + matrixCopy.m_data[5] * matrixCopy.m_data[10] * matrixCopy.m_data[15];
	m_data[1]  = matrixCopy.m_data[3] * matrixCopy.m_data[10] * matrixCopy.m_data[13] - matrixCopy.m_data[2] * matrixCopy.m_data[11] * matrixCopy.m_data[13] - matrixCopy.m_data[3] * matrixCopy.m_data[9] * matrixCopy.m_data[14] + matrixCopy.m_data[1] * matrixCopy.m_data[11] * matrixCopy.m_data[14] + matrixCopy.m_data[2] * matrixCopy.m_data[9] * matrixCopy.m_data[15] - matrixCopy.m_data[1] * matrixCopy.m_data[10] * matrixCopy.m_data[15];
	m_data[2]  = matrixCopy.m_data[2] * matrixCopy.m_data[7]  * matrixCopy.m_data[13] - matrixCopy.m_data[3] * matrixCopy.m_data[6]  * matrixCopy.m_data[13] + matrixCopy.m_data[3] * matrixCopy.m_data[5] * matrixCopy.m_data[14] - matrixCopy.m_data[1] * matrixCopy.m_data[7]  * matrixCopy.m_data[14] - matrixCopy.m_data[2] * matrixCopy.m_data[5] * matrixCopy.m_data[15] + matrixCopy.m_data[1] * matrixCopy.m_data[6]  * matrixCopy.m_data[15];
	m_data[3]  = matrixCopy.m_data[3] * matrixCopy.m_data[6]  * matrixCopy.m_data[9]  - matrixCopy.m_data[2] * matrixCopy.m_data[7]  * matrixCopy.m_data[9]  - matrixCopy.m_data[3] * matrixCopy.m_data[5] * matrixCopy.m_data[10] + matrixCopy.m_data[1] * matrixCopy.m_data[7]  * matrixCopy.m_data[10] + matrixCopy.m_data[2] * matrixCopy.m_data[5] * matrixCopy.m_data[11] - matrixCopy.m_data[1] * matrixCopy.m_data[6]  * matrixCopy.m_data[11];
	m_data[4]  = matrixCopy.m_data[7] * matrixCopy.m_data[10] * matrixCopy.m_data[12] - matrixCopy.m_data[6] * matrixCopy.m_data[11] * matrixCopy.m_data[12] - matrixCopy.m_data[7] * matrixCopy.m_data[8] * matrixCopy.m_data[14] + matrixCopy.m_data[4] * matrixCopy.m_data[11] * matrixCopy.m_data[14] + matrixCopy.m_data[6] * matrixCopy.m_data[8] * matrixCopy.m_data[15] - matrixCopy.m_data[4] * matrixCopy.m_data[10] * matrixCopy.m_data[15];
	m_data[5]  = matrixCopy.m_data[2] * matrixCopy.m_data[11] * matrixCopy.m_data[12] - matrixCopy.m_data[3] * matrixCopy.m_data[10] * matrixCopy.m_data[12] + matrixCopy.m_data[3] * matrixCopy.m_data[8] * matrixCopy.m_data[14] - matrixCopy.m_data[0] * matrixCopy.m_data[11] * matrixCopy.m_data[14] - matrixCopy.m_data[2] * matrixCopy.m_data[8] * matrixCopy.m_data[15] + matrixCopy.m_data[0] * matrixCopy.m_data[10] * matrixCopy.m_data[15];
	m_data[6]  = matrixCopy.m_data[3] * matrixCopy.m_data[6]  * matrixCopy.m_data[12] - matrixCopy.m_data[2] * matrixCopy.m_data[7]  * matrixCopy.m_data[12] - matrixCopy.m_data[3] * matrixCopy.m_data[4] * matrixCopy.m_data[14] + matrixCopy.m_data[0] * matrixCopy.m_data[7]  * matrixCopy.m_data[14] + matrixCopy.m_data[2] * matrixCopy.m_data[4] * matrixCopy.m_data[15] - matrixCopy.m_data[0] * matrixCopy.m_data[6]  * matrixCopy.m_data[15];
	m_data[7]  = matrixCopy.m_data[2] * matrixCopy.m_data[7]  * matrixCopy.m_data[8]  - matrixCopy.m_data[3] * matrixCopy.m_data[6]  * matrixCopy.m_data[8]  + matrixCopy.m_data[3] * matrixCopy.m_data[4] * matrixCopy.m_data[10] - matrixCopy.m_data[0] * matrixCopy.m_data[7]  * matrixCopy.m_data[10] - matrixCopy.m_data[2] * matrixCopy.m_data[4] * matrixCopy.m_data[11] + matrixCopy.m_data[0] * matrixCopy.m_data[6]  * matrixCopy.m_data[11];
	m_data[8]  = matrixCopy.m_data[5] * matrixCopy.m_data[11] * matrixCopy.m_data[12] - matrixCopy.m_data[7] * matrixCopy.m_data[9]  * matrixCopy.m_data[12] + matrixCopy.m_data[7] * matrixCopy.m_data[8] * matrixCopy.m_data[13] - matrixCopy.m_data[4] * matrixCopy.m_data[11] * matrixCopy.m_data[13] - matrixCopy.m_data[5] * matrixCopy.m_data[8] * matrixCopy.m_data[15] + matrixCopy.m_data[4] * matrixCopy.m_data[9]  * matrixCopy.m_data[15];
	m_data[9]  = matrixCopy.m_data[3] * matrixCopy.m_data[9]  * matrixCopy.m_data[12] - matrixCopy.m_data[1] * matrixCopy.m_data[11] * matrixCopy.m_data[12] - matrixCopy.m_data[3] * matrixCopy.m_data[8] * matrixCopy.m_data[13] + matrixCopy.m_data[0] * matrixCopy.m_data[11] * matrixCopy.m_data[13] + matrixCopy.m_data[1] * matrixCopy.m_data[8] * matrixCopy.m_data[15] - matrixCopy.m_data[0] * matrixCopy.m_data[9]  * matrixCopy.m_data[15];
	m_data[10] = matrixCopy.m_data[1] * matrixCopy.m_data[7]  * matrixCopy.m_data[12] - matrixCopy.m_data[3] * matrixCopy.m_data[5]  * matrixCopy.m_data[12] + matrixCopy.m_data[3] * matrixCopy.m_data[4] * matrixCopy.m_data[13] - matrixCopy.m_data[0] * matrixCopy.m_data[7]  * matrixCopy.m_data[13] - matrixCopy.m_data[1] * matrixCopy.m_data[4] * matrixCopy.m_data[15] + matrixCopy.m_data[0] * matrixCopy.m_data[5]  * matrixCopy.m_data[15];
	m_data[11] = matrixCopy.m_data[3] * matrixCopy.m_data[5]  * matrixCopy.m_data[8]  - matrixCopy.m_data[1] * matrixCopy.m_data[7]  * matrixCopy.m_data[8]  - matrixCopy.m_data[3] * matrixCopy.m_data[4] * matrixCopy.m_data[9]  + matrixCopy.m_data[0] * matrixCopy.m_data[7]  * matrixCopy.m_data[9]  + matrixCopy.m_data[1] * matrixCopy.m_data[4] * matrixCopy.m_data[11] - matrixCopy.m_data[0] * matrixCopy.m_data[5]  * matrixCopy.m_data[11];
	m_data[12] = matrixCopy.m_data[6] * matrixCopy.m_data[9]  * matrixCopy.m_data[12] - matrixCopy.m_data[5] * matrixCopy.m_data[10] * matrixCopy.m_data[12] - matrixCopy.m_data[6] * matrixCopy.m_data[8] * matrixCopy.m_data[13] + matrixCopy.m_data[4] * matrixCopy.m_data[10] * matrixCopy.m_data[13] + matrixCopy.m_data[5] * matrixCopy.m_data[8] * matrixCopy.m_data[14] - matrixCopy.m_data[4] * matrixCopy.m_data[9]  * matrixCopy.m_data[14];
	m_data[13] = matrixCopy.m_data[1] * matrixCopy.m_data[10] * matrixCopy.m_data[12] - matrixCopy.m_data[2] * matrixCopy.m_data[9]  * matrixCopy.m_data[12] + matrixCopy.m_data[2] * matrixCopy.m_data[8] * matrixCopy.m_data[13] - matrixCopy.m_data[0] * matrixCopy.m_data[10] * matrixCopy.m_data[13] - matrixCopy.m_data[1] * matrixCopy.m_data[8] * matrixCopy.m_data[14] + matrixCopy.m_data[0] * matrixCopy.m_data[9]  * matrixCopy.m_data[14];
	m_data[14] = matrixCopy.m_data[2] * matrixCopy.m_data[5]  * matrixCopy.m_data[12] - matrixCopy.m_data[1] * matrixCopy.m_data[6]  * matrixCopy.m_data[12] - matrixCopy.m_data[2] * matrixCopy.m_data[4] * matrixCopy.m_data[13] + matrixCopy.m_data[0] * matrixCopy.m_data[6]  * matrixCopy.m_data[13] + matrixCopy.m_data[1] * matrixCopy.m_data[4] * matrixCopy.m_data[14] - matrixCopy.m_data[0] * matrixCopy.m_data[5]  * matrixCopy.m_data[14];
	m_data[15] = matrixCopy.m_data[1] * matrixCopy.m_data[6]  * matrixCopy.m_data[8]  - matrixCopy.m_data[2] * matrixCopy.m_data[5]  * matrixCopy.m_data[8]  + matrixCopy.m_data[2] * matrixCopy.m_data[4] * matrixCopy.m_data[9]  - matrixCopy.m_data[0] * matrixCopy.m_data[6]  * matrixCopy.m_data[9]  - matrixCopy.m_data[1] * matrixCopy.m_data[4] * matrixCopy.m_data[10] + matrixCopy.m_data[0] * matrixCopy.m_data[5]  * matrixCopy.m_data[10];

	Scale(*this, 1 / det, *this);
}

void Matrix4::OrthoNormalize()
{
	glm::vec3 x(m_data[0], m_data[4], m_data[8]);
	glm::vec3 y(m_data[1], m_data[5], m_data[9]);
	glm::vec3 z;

	z = cross(x, y);
	y = cross(z, x);

	x = normalize(x);
	y = normalize(y);
	z = normalize(z);

	m_data[0]  = x.x;	m_data[1]  = y.x;	m_data[2]  = z.x;	m_data[3]  = 0.0f;
	m_data[4]  = x.y;	m_data[5]  = y.y;	m_data[6]  = z.y;	m_data[7]  = 0.0f;
	m_data[8]  = x.z;	m_data[9]  = y.z;	m_data[10] = z.z;	m_data[11] = 0.0f;
	m_data[12] = 0.0f;	m_data[13] = 0.0f;	m_data[14] = 0.0f;	m_data[15] = 1.0f;
}

void Matrix4::SetValues(float data[16])
{
	memcpy(this->m_data, data, 16 * sizeof(float));
}

void Matrix4::PostMultiply(Matrix4& matrix)
{
	float newMatrix[16];
	const float* m1 = m_data;
	const float* m2 = matrix.m_data;

	newMatrix[0] = m1[0] * m2[0] + m1[4] * m2[1] + m1[8] * m2[2];
	newMatrix[1] = m1[1] * m2[0] + m1[5] * m2[1] + m1[9] * m2[2];
	newMatrix[2] = m1[2] * m2[0] + m1[6] * m2[1] + m1[10] * m2[2];
	newMatrix[3] = 0;

	newMatrix[4] = m1[0] * m2[4] + m1[4] * m2[5] + m1[8] * m2[6];
	newMatrix[5] = m1[1] * m2[4] + m1[5] * m2[5] + m1[9] * m2[6];
	newMatrix[6] = m1[2] * m2[4] + m1[6] * m2[5] + m1[10] * m2[6];
	newMatrix[7] = 0;

	newMatrix[8] = m1[0] * m2[8] + m1[4] * m2[9] + m1[8] * m2[10];
	newMatrix[9] = m1[1] * m2[8] + m1[5] * m2[9] + m1[9] * m2[10];
	newMatrix[10] = m1[2] * m2[8] + m1[6] * m2[9] + m1[10] * m2[10];
	newMatrix[11] = 0;

	newMatrix[12] = m1[0] * m2[12] + m1[4] * m2[13] + m1[8] * m2[14] + m1[12];
	newMatrix[13] = m1[1] * m2[12] + m1[5] * m2[13] + m1[9] * m2[14] + m1[13];
	newMatrix[14] = m1[2] * m2[12] + m1[6] * m2[13] + m1[10] * m2[14] + m1[14];
	newMatrix[15] = 1;

	SetValues(newMatrix);
}

void Matrix4::InverseTranslateVector(float* pVector)
{
	pVector[0] = pVector[0] - m_data[12];
	pVector[1] = pVector[1] - m_data[13];
	pVector[2] = pVector[2] - m_data[14];
}

void Matrix4::InverseRotateVector(float* pVector)
{
	float vec[3];

	vec[0] = pVector[0] * m_data[0] + pVector[1] * m_data[1] + pVector[2] * m_data[2];
	vec[1] = pVector[0] * m_data[4] + pVector[1] * m_data[5] + pVector[2] * m_data[6];
	vec[2] = pVector[0] * m_data[8] + pVector[1] * m_data[9] + pVector[2] * m_data[10];

	memcpy(pVector, vec, 3 * sizeof(float));
}

void Matrix4::SetValuesRotationAll(float x, float y, float z)
{
	Matrix4 rotationX, rotationY, rotationZ;

	rotationX.SetXRotation(x);
	rotationY.SetYRotation(y);
	rotationZ.SetZRotation(z);

	// Order is different from SetRotation()?
	*this = rotationZ * rotationY * rotationX;
}

void Matrix4::SetRotationRadians(float* angles)
{
	float cosRoll  = cos(angles[0]);
	float sinRoll  = sin(angles[0]);
	float cosPitch = cos(angles[1]);
	float sinPitch = sin(angles[1]);
	float cosYaw   = cos(angles[2]);
	float sinYaw   = sin(angles[2]);

	m_data[0] = cosPitch * cosYaw;
	m_data[1] = cosPitch * sinYaw;
	m_data[2] = -sinPitch;

	double sinRollsinPitch = sinRoll * sinPitch;
	double cosRollsinPitch = cosRoll * sinPitch;

	m_data[4] = sinRollsinPitch * cosYaw - cosRoll * sinYaw;
	m_data[5] = sinRollsinPitch * sinYaw + cosRoll * cosYaw;
	m_data[6] = sinRoll * cosPitch;

	m_data[8] = cosRollsinPitch * cosYaw + sinRoll * sinYaw;
	m_data[9] = cosRollsinPitch * sinYaw - sinRoll * cosYaw;
	m_data[10] = cosRoll * cosPitch;
}

// Arithmetic
Matrix4& Matrix4::Add(const Matrix4& m1, const Matrix4& m2, Matrix4& result)
{
	for (int i = 0; i < 16; ++i)
	{
		result.m_data[i] = m1.m_data[i] + m2.m_data[i];
	}

	return result;
}

Matrix4& Matrix4::Subtract(const Matrix4& m1, const Matrix4& m2, Matrix4& result)
{
	for (int i = 0; i < 16; ++i)
	{
		result.m_data[i] = m1.m_data[i] - m2.m_data[i];
	}

	return result;
}

Matrix4& Matrix4::Scale(const Matrix4& m1, const float& scale, Matrix4& result)
{
	for (int i = 0; i < 16; ++i)
	{
		result.m_data[i] = m1.m_data[i] * scale;
	}

	result.m_data[0] = 1;
	result.m_data[5] = 1;
	result.m_data[10] = 1;
	result.m_data[15] = 1;

	return result;
}

Matrix4& Matrix4::Multiply(const Matrix4& m1, const Matrix4& m2, Matrix4& result)
{
	for (int index = 0; index < 4; ++index)
	{
		for (int alpha = 0; alpha < 4; ++alpha)
		{
			float sum = 0.0f;

			for (int beta = 0; beta < 4; ++beta)
			{
				sum += m2.m_data[index + beta * 4] * m1.m_data[alpha * 4 + beta];
			}

			result.m_data[index + alpha * 4] = sum;
		}
	}

	return result;
}

glm::vec3& Matrix4::Multiply(const Matrix4& m, const glm::vec3& v, glm::vec3& result)
{
	float vec[4], r[4];

	vec[0] = v.x;
	vec[1] = v.y;
	vec[2] = v.z;
	vec[3] = 1.0;

	for (int index = 0; index < 4; ++index)
	{
		float sum = 0.0f;

		for (int alpha = 0; alpha < 4; ++alpha)
		{
			sum += m.m_data[index + alpha * 4] * vec[alpha];
		}

		r[index] = sum;
	}

	result = glm::vec3(r[0], r[1], r[2]);

	return result;
}


bool Matrix4::Equal(const Matrix4& m1, const Matrix4& m2)
{
	for (int i = 0; i < 16; ++i)
	{
		if (fabs(m1.m_data[i] - m2.m_data[i]) > 1.0e-7f)
		{
			return false;
		}
	}

	return true;
}