/*************************************************************************
> File Name: Matrix4.h
> Project Name: Cubby
> Author: Chan-Ho Chris Ohk
> Purpose: 3d maths - Matrix4 library.
> Created Time: 2016/06/18
> Copyright (c) 2016, Chan-Ho Chris Ohk
*************************************************************************/

#ifndef CUBBY_MATRIX4_H
#define CUBBY_MATRIX4_H

#include <glm/vec3.hpp>

struct Matrix4
{
	// Constructors
	Matrix4();
	Matrix4(float data[16]);

	// Setup matrices
	void SetXRotation(const float x);
	void SetYRotation(const float y);
	void SetZRotation(const float z);
	void SetRotation(const float x, const float y, const float z);
	void SetTranslation(float translation[3]);
	void SetTranslation(glm::vec3 translation);
	void SetScale(glm::vec3 scale);

	void AddTranslation(float* translation);
	void AddRotationByRadians(float* angles);

	// Properties
	void GetMatrix(float* matrix) const;
	const float GetDeterminant() const;
	const Matrix4 GetNegative() const;
	const Matrix4 GetTranspose() const;
	const Matrix4 GetInverse() const;
	const Matrix4 GetOrthoNormal() const;

	const glm::vec3 GetRightVector() const;
	const glm::vec3 GetUpVector() const;
	const glm::vec3 GetForwardVector() const;
	const glm::vec3 GetTranslationVector() const;
	const void GetEuler(float* x, float* y, float* z) const;
	
	// Operations
	void LoadIdentity();
	void Negate();
	void Transpose();
	void Inverse();
	void OrthoNormalize();

	void SetValues(float data[16]);							// Set the values of the matrix
	void PostMultiply(Matrix4& matrix);						// Post multiple with another matrix
	void InverseTranslateVector(float* pVector);			// Translate a vector by the inverse of the translation part of this matrix
	void InverseRotateVector(float* pVector);				// Rotate a vector by the inverse of the rotation part of this matrix
	void SetValuesRotationAll(float x, float y, float z);	// Generate rotation matrix
	void SetRotationRadians(float* angles);					// Set the Rotation matrix

	// Arithmetic
	static Matrix4& Add(const Matrix4& m1, const Matrix4& m2, Matrix4& result);
	static Matrix4& Subtract(const Matrix4& m1, const Matrix4& m2, Matrix4& result);
	static Matrix4& Scale(const Matrix4& m1, const float &scale, Matrix4& result);
	static Matrix4& Multiply(const Matrix4& m1, const Matrix4& m2, Matrix4& result);
	static glm::vec3& Multiply(const Matrix4& m, const glm::vec3& v, glm::vec3& result);
	static bool Equal(const Matrix4& m1, const Matrix4& m2);

	// Operators
	Matrix4 operator+(const Matrix4& m) const
	{
		Matrix4 result;
		return Add(*this, m, result);
	}
	Matrix4& operator+=(const Matrix4& m)
	{
		Add(*this, m, *this);
		return *this;
	}
	Matrix4 operator-(const Matrix4& m) const
	{
		Matrix4 result;
		return Subtract(*this, m, result);
	}
	Matrix4& operator-=(const Matrix4& m)
	{
		Subtract(*this, m, *this);
		return *this;
	}
	Matrix4 operator*(const float& scale) const
	{
		Matrix4 result;
		return Scale(*this, scale, result);
	}
	Matrix4& operator*=(const float& scale)
	{
		Scale(*this, scale, *this);
		return *this;
	}
	Matrix4 operator/(const float& scale) const
	{
		Matrix4 result;
		return Scale(*this, 1 / scale, result);
	}
	Matrix4& operator/=(const float& scale)
	{
		Scale(*this, 1 / scale, *this);
		return *this;
	}
	Matrix4 operator*(const Matrix4& m) const
	{
		Matrix4 result;
		return Multiply(*this, m, result);
	}
	glm::vec3 operator*(const glm::vec3 v) const
	{
		glm::vec3 result;
		return Multiply(*this, v, result);
	}
	bool operator==(const Matrix4& m) const
	{
		return Equal(*this, m);
	}
	bool operator!=(const Matrix4& m) const
	{
		return !Equal(*this, m);
	}

	float m_data[16];
};

#endif