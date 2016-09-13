/*************************************************************************
> File Name: QubicleBinary.h
> Project Name: Cubby
> Author: Chan-Ho Chris Ohk
> Purpose: Import Qubicle binary file.
> Created Time: 2016/06/29
> Copyright (c) 2016, Chan-Ho Chris Ohk
*************************************************************************/

#ifndef CUBBY_QUBICLE_BINARY_H
#define CUBBY_QUBICLE_BINARY_H

#include "MS3DAnimator.h"

class VoxelCharacter;

enum class MergedSide
{
	None = 0,
	XPositive = 1,
	XNegative = 2,
	YPositive = 4,
	YNegative = 8,
	ZPositive = 16,
	ZNegative = 32,
};

bool IsMergedXNegative(int* merged, int x, int y, int z, int width, int height);
bool IsMergedXPositive(int* merged, int x, int y, int z, int width, int height);
bool IsMergedYNegative(int* merged, int x, int y, int z, int width, int height);
bool IsMergedYPositive(int* merged, int x, int y, int z, int width, int height);
bool IsMergedZNegative(int* merged, int x, int y, int z, int width, int height);
bool IsMergedZPositive(int* merged, int x, int y, int z, int width, int height);

struct QubicleMatrix
{
	QubicleMatrix() :
		m_name(nullptr), m_nameLength(0),
		m_matrixSizeX(0), m_matrixSizeY(0), m_matrixSizeZ(0),
		m_matrixPosX(0), m_matrixPosY(0), m_matrixPosZ(0),
		m_pColor(nullptr), m_boneIndex(0), m_scale(0),
		m_offsetX(0.0f), m_offsetY(0.0f), m_offsetZ(0.0f),
		m_isRemoved(false), m_pMesh(nullptr)
	{
	}

	void GetColor(int x, int y, int z, float* r, float* g, float* b, float* a) const
	{
		unsigned color = m_pColor[x + m_matrixSizeX * (y + m_matrixSizeY * z)];
		unsigned int blue = (color & 0x00FF0000) >> 16;
		unsigned int green = (color & 0x0000FF00) >> 8;
		unsigned int red = (color & 0x000000FF);

		*r = red / 255.0f;
		*g = green / 255.0f;
		*b = blue / 255.0f;

		*a = 1.0f;
	}

	unsigned int GetColorCompact(int x, int y, int z) const
	{
		return m_pColor[x + m_matrixSizeX * (y + m_matrixSizeY * z)];
	}

	bool GetActive(int x, int y, int z) const
	{
		unsigned color = m_pColor[x + m_matrixSizeX * (y + m_matrixSizeY * z)];
		unsigned int alpha = (color & 0xFF000000) >> 24;

		if (alpha == 0)
		{
			return false;
		}

		return true;
	}

	char* m_name;
	char m_nameLength;

	unsigned int m_matrixSizeX;
	unsigned int m_matrixSizeY;
	unsigned int m_matrixSizeZ;

	int m_matrixPosX;
	int m_matrixPosY;
	int m_matrixPosZ;

	unsigned int* m_pColor;

	int m_boneIndex;

	Matrix4 m_modelMatrix;

	float m_scale;
	float m_offsetX;
	float m_offsetY;
	float m_offsetZ;

	bool m_isRemoved;

	TriangleMesh* m_pMesh;
};

using QubicleMatrixList = std::vector<QubicleMatrix*>;

class QubicleBinary
{
public:
	// Constructor, Destructor
	QubicleBinary(Renderer* pRenderer);
	~QubicleBinary();

	void SetNullLinkage(QubicleBinary* pBinary);

	void Unload();
	void ClearMatrices();

	void Reset();

	std::string GetFileName() const;

	unsigned int GetMaterial() const;

	Matrix4 GetModelMatrix(int qubicleMatrixIndex);

	int GetMatrixIndexForName(const char* matrixName);
	void GetMatrixPosition(int index, int* aX, int* aY, int* aZ);

	bool Import(const char* fileName, bool faceMerging);
	bool Export(const char* fileName);

	void GetColor(int matrixIndex, int x, int y, int z, float* r, float* g, float* b, float* a);
	unsigned int GetColorCompact(int matrixIndex, int x, int y, int z);
	bool GetSingleMeshColor(float* r, float* g, float* b, float* a) const;
	bool GetActive(int matrixIndex, int x, int y, int z);

	void SetMeshAlpha(float alpha);
	void SetMeshSingleColor(float r, float g, float b);

	void CreateMesh(bool doFaceMerging);
	void RebuildMesh(bool doFaceMerging);
	void UpdateMergedSide(int* merged, int matrixIndex, int blockX, int blockY, int blockZ, int width, int height, glm::vec3* p1, glm::vec3* p2, glm::vec3* p3, glm::vec3* p4, int startX, int startY, int maxX, int maxY, bool isPositive, bool zFace, bool xFace, bool yFace);

	int GetNumMatrices() const;
	QubicleMatrix* GetQubicleMatrix(int index);
	QubicleMatrix* GetQubicleMatrix(const char* matrixName);
	const char* GetMatrixName(int index);
	float GetMatrixScale(int index);
	glm::vec3 GetMatrixOffset(int index);

	void SetupMatrixBones(MS3DAnimator* pSkeleton);

	void SetScaleAndOffsetForMatrix(const char* matrixName, float scale, float xOffset, float yOffset, float zOffset);
	float GetScale(const char* matrixName);
	glm::vec3 GetOffset(const char* matrixName);

	void SwapMatrix(const char* matrixName, QubicleMatrix* pMatrix, bool copyMatrixParams);
	void AddQubicleMatrix(QubicleMatrix* pNewMatrix, bool copyMatrixParams);
	void RemoveQubicleMatrix(const char* matrixName);
	void SetQubicleMatrixRender(const char* matrixName, bool render);

	// Sub selection
	std::string GetSubSelectionName(int pickingID);

	// Rendering modes
	void SetWireFrameRender(bool wireframe);

	// Rendering
	void Render(bool renderOutline, bool reflection, bool silhouette, Color outlixneColor);
	void RenderWithAnimator(MS3DAnimator** pSkeleton, VoxelCharacter* pVoxelCharacter, bool renderOutline, bool reflection, bool silhouette, Color outlineColor, bool subSelectionNamePicking);
	void RenderSingleMatrix(MS3DAnimator** pSkeleton, VoxelCharacter* pVoxelCharacter, std::string matrixName, bool renderOutline, bool silhouette, Color outlineColor);
	void RenderFace(MS3DAnimator* pSkeleton, VoxelCharacter* pVoxelCharacter, bool transparency, bool useScale = true, bool useTranslate = true);
	void RenderPaperdoll(MS3DAnimator* pSkeletonLeft, MS3DAnimator* pSkeletonRight, VoxelCharacter* pVoxelCharacter);
	void RenderPortrait(MS3DAnimator* pSkeleton, VoxelCharacter* pVoxelCharacter, std::string matrixName);

	static const float BLOCK_RENDER_SIZE;
	static const int SUBSELECTION_NAMEPICKING_OFFSET = 10000000;

private:
	Renderer* m_pRenderer;

	// Loaded flag
	bool m_isLoaded;

	// File name
	std::string m_fileName;

	// Qubicle binary file information
	char m_version[4];
	unsigned int m_colorFormat;
	unsigned int m_zAxisOrientation;
	unsigned int m_compressed;
	unsigned int m_visibilityMaskEncoded;
	unsigned int m_numMatrices;

	// Matrix data for file
	QubicleMatrixList m_vpMatrices;

	// Render modes
	bool m_isRenderWireFrame;

	// Alpha
	float m_meshAlpha;

	// Single color
	bool m_isSingleMeshColor;
	float m_meshSingleColorR;
	float m_meshSingleColorG;
	float m_meshSingleColorB;

	// Material
	unsigned int m_materialID;
};


#endif