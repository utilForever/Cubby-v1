/*************************************************************************
> File Name: QubicleBinary.cpp
> Project Name: Cubby
> Author: Chan-Ho Chris Ohk
> Purpose: Import Qubicle binary file.
> Created Time: 2016/07/02
> Copyright (c) 2016, Chan-Ho Chris Ohk
*************************************************************************/

#include "QubicleBinary.h"
#include "VoxelCharacter.h"

// Constructor, Destructor
QubicleBinary::QubicleBinary(Renderer* pRenderer) :
	m_pRenderer(pRenderer), m_isLoaded(false),
	m_colorFormat(0), m_zAxisOrientation(0), m_compressed(0), m_visibilityMaskEncoded(0), m_numMatrices(0),
	m_isRenderWireFrame(false), m_meshAlpha(0.0f),
	m_isSingleMeshColor(false), m_meshSingleColorR(0.0f), m_meshSingleColorG(false), m_meshSingleColorB(false),
	m_materialID(0)
{
	Reset();

	pRenderer->CreateMaterial(Color(1.0f, 1.0f, 1.0f, 1.0f), Color(1.0f, 1.0f, 1.0f, 1.0f), Color(1.0f, 1.0f, 1.0f, 1.0f), Color(0.0f, 0.0f, 0.0f, 1.0f), 64, &m_materialID);
}

QubicleBinary::~QubicleBinary()
{
	Unload();
	Reset();
}

void QubicleBinary::SetNullLinkage(QubicleBinary* pBinary)
{
	for (auto pMatrix : m_vpMatrices)
	{
		for (int i = 0; i < pBinary->GetNumMatrices(); ++i)
		{
			if (pMatrix == pBinary->GetQubicleMatrix(i))
			{
				pMatrix = nullptr;
			}
		}
	}
}

void QubicleBinary::Unload()
{
	if (m_isLoaded)
	{
		ClearMatrices();
	}
}

void QubicleBinary::ClearMatrices()
{
	for (auto pMatrix : m_vpMatrices)
	{
		if (pMatrix == nullptr)
		{
			continue;
		}

		if (pMatrix->isRemoved == true)
		{
			continue;
		}

		m_pRenderer->ClearMesh(pMatrix->pMesh);
		pMatrix->pMesh = nullptr;

		delete[] pMatrix->pColor;

		delete pMatrix;
		// ReSharper disable once CppAssignedValueIsNeverUsed
		pMatrix = nullptr;
	}

	m_vpMatrices.clear();
}

void QubicleBinary::Reset()
{
	m_version[0] = 0;
	m_version[1] = 0;
	m_version[2] = 0;
	m_version[3] = 0;
	m_colorFormat = 0;
	m_zAxisOrientation = 0;
	m_compressed = 0;
	m_visibilityMaskEncoded = 0;
	m_numMatrices = 0;

	m_isLoaded = false;

	ClearMatrices();

	m_isRenderWireFrame = false;
}

std::string QubicleBinary::GetFileName() const
{
	return m_fileName;
}

unsigned int QubicleBinary::GetMaterial() const
{
	return m_materialID;
}

Matrix4 QubicleBinary::GetModelMatrix(int qubicleMatrixIndex)
{
	if (m_isLoaded == false)
	{
		return Matrix4();
	}

	return m_vpMatrices[qubicleMatrixIndex]->modelMatrix;
}

int QubicleBinary::GetMatrixIndexForName(const char* matrixName)
{
	for (unsigned int i = 0; i < m_numMatrices; ++i)
	{
		if (strcmp(m_vpMatrices[i]->name, matrixName) == 0)
		{
			return i;
		}
	}

	return -1;
}

void QubicleBinary::GetMatrixPosition(int index, int* aX, int* aY, int* aZ)
{
	*aX = m_vpMatrices[index]->matrixPosX;
	*aY = m_vpMatrices[index]->matrixPosY;
	*aZ = m_vpMatrices[index]->matrixPosZ;
}

bool QubicleBinary::Import(const char* fileName, bool faceMerging)
{
	m_fileName = fileName;

	char qbFilename[256];
	sprintf(qbFilename, fileName);;

	FILE* pQBfile = nullptr;
	fopen_s(&pQBfile, qbFilename, "rb");

	const unsigned int CODEFLAG = 2;
	const unsigned int NEXTSLICEFLAG = 6;

	if (pQBfile != nullptr)
	{
		fread(&m_version[0], sizeof(char) * 4, 1, pQBfile) == 1;
		fread(&m_colorFormat, sizeof(unsigned int), 1, pQBfile) == 1;
		fread(&m_zAxisOrientation, sizeof(unsigned int), 1, pQBfile) == 1;
		fread(&m_compressed, sizeof(unsigned int), 1, pQBfile) == 1;
		fread(&m_visibilityMaskEncoded, sizeof(unsigned int), 1, pQBfile) == 1;
		fread(&m_numMatrices, sizeof(unsigned int), 1, pQBfile) == 1;

		for (unsigned int i = 0; i < m_numMatrices; ++i)
		{
			QubicleMatrix* pNewMatrix = new QubicleMatrix();

			fread(static_cast<char*>(&pNewMatrix->nameLength), sizeof(char), 1, pQBfile) == 1;
			pNewMatrix->name = new char[pNewMatrix->nameLength + 1];
			fread(&pNewMatrix->name[0], sizeof(char) * pNewMatrix->nameLength, 1, pQBfile) == 1;
			pNewMatrix->name[pNewMatrix->nameLength] = 0;

			fread(&pNewMatrix->matrixSizeX, sizeof(unsigned int), 1, pQBfile) == 1;
			fread(&pNewMatrix->matrixSizeY, sizeof(unsigned int), 1, pQBfile) == 1;
			fread(&pNewMatrix->matrixSizeZ, sizeof(unsigned int), 1, pQBfile) == 1;

			fread(&pNewMatrix->matrixPosX, sizeof(int), 1, pQBfile) == 1;
			fread(&pNewMatrix->matrixPosY, sizeof(int), 1, pQBfile) == 1;
			fread(&pNewMatrix->matrixPosZ, sizeof(int), 1, pQBfile) == 1;

			pNewMatrix->boneIndex = -1;
			pNewMatrix->pMesh = nullptr;

			pNewMatrix->scale = 1.0f;
			pNewMatrix->offsetX = 0.0f;
			pNewMatrix->offsetY = 0.0f;
			pNewMatrix->offsetZ = 0.0f;

			pNewMatrix->isRemoved = false;
			
			pNewMatrix->pColor = new unsigned int[pNewMatrix->matrixSizeX * pNewMatrix->matrixSizeY * pNewMatrix->matrixSizeZ];

			if (m_compressed == 0)
			{
				for (unsigned int z = 0; z < pNewMatrix->matrixSizeZ; ++z)
				{
					for (unsigned int y = 0; y < pNewMatrix->matrixSizeY; ++y)
					{
						for (unsigned int x = 0; x < pNewMatrix->matrixSizeX; ++x)
						{
							unsigned int color = 0;
							fread(&color, sizeof(unsigned int), 1, pQBfile) == 1;

							pNewMatrix->pColor[x + pNewMatrix->matrixSizeX * (y + pNewMatrix->matrixSizeY * z)] = color;
						}
					}
				}
			}
			else
			{
				unsigned int z = 0;

				while (z < pNewMatrix->matrixSizeZ)
				{
					unsigned int index = 0;

					while (true)
					{
						unsigned int data = 0;
						fread(&data, sizeof(unsigned int), 1, pQBfile) == 1;

						if (data == NEXTSLICEFLAG)
							break;
						
						if (data == CODEFLAG)
						{
							unsigned int count = 0;
							fread(&count, sizeof(unsigned int), 1, pQBfile) == 1;
							fread(&data, sizeof(unsigned int), 1, pQBfile) == 1;

							for (unsigned int j = 0; j < count; ++j)
							{
								unsigned int x = index % pNewMatrix->matrixSizeX;
								unsigned int y = index / pNewMatrix->matrixSizeX;

								pNewMatrix->pColor[x + pNewMatrix->matrixSizeX * (y + pNewMatrix->matrixSizeY * z)] = data;

								index++;
							}
						}
						else
						{
							unsigned int x = index % pNewMatrix->matrixSizeX;
							unsigned int y = index / pNewMatrix->matrixSizeX;

							pNewMatrix->pColor[x + pNewMatrix->matrixSizeX * (y + pNewMatrix->matrixSizeY * z)] = data;

							index++;
						}
					}

					z++;
				}
			}

			m_vpMatrices.push_back(pNewMatrix);
		}

		fclose(pQBfile);

		CreateMesh(faceMerging);

		m_isLoaded = true;

		return true;
	}

	return false;
}

bool QubicleBinary::Export(const char* fileName)
{
	char qbFilename[256];
	sprintf(qbFilename, fileName);;

	FILE* pQBfile = nullptr;
	fopen_s(&pQBfile, qbFilename, "wb");

	const unsigned int CODEFLAG = 2;
	const unsigned int NEXTSLICEFLAG = 6;

	if (qbFilename != nullptr)
	{
		fwrite(&m_version[0], sizeof(char) * 4, 1, pQBfile) == 1;
		fwrite(&m_colorFormat, sizeof(unsigned int), 1, pQBfile) == 1;
		fwrite(&m_zAxisOrientation, sizeof(unsigned int), 1, pQBfile) == 1;
		fwrite(&m_compressed, sizeof(unsigned int), 1, pQBfile) == 1;
		fwrite(&m_visibilityMaskEncoded, sizeof(unsigned int), 1, pQBfile) == 1;
		fwrite(&m_numMatrices, sizeof(unsigned int), 1, pQBfile) == 1;

		for (unsigned int i = 0; i < m_numMatrices; ++i)
		{
			QubicleMatrix* pMatrix = m_vpMatrices[i];

			fwrite(static_cast<char*>(&pMatrix->nameLength), sizeof(char), 1, pQBfile) == 1;
			fwrite(&pMatrix->name[0], sizeof(char)*pMatrix->nameLength, 1, pQBfile) == 1;

			fwrite(&pMatrix->matrixSizeX, sizeof(unsigned int), 1, pQBfile) == 1;
			fwrite(&pMatrix->matrixSizeY, sizeof(unsigned int), 1, pQBfile) == 1;
			fwrite(&pMatrix->matrixSizeZ, sizeof(unsigned int), 1, pQBfile) == 1;

			fwrite(&pMatrix->matrixPosX, sizeof(int), 1, pQBfile) == 1;
			fwrite(&pMatrix->matrixPosY, sizeof(int), 1, pQBfile) == 1;
			fwrite(&pMatrix->matrixPosZ, sizeof(int), 1, pQBfile) == 1;

			if (m_compressed == 0)
			{
				for (unsigned int z = 0; z < pMatrix->matrixSizeZ; ++z)
				{
					for (unsigned int y = 0; y < pMatrix->matrixSizeY; ++y)
					{
						for (unsigned int x = 0; x < pMatrix->matrixSizeX; ++x)
						{
							unsigned int color = pMatrix->pColor[x + pMatrix->matrixSizeX * (y + pMatrix->matrixSizeY * z)];
							fwrite(&color, sizeof(unsigned int), 1, pQBfile) == 1;
						}
					}
				}
			}
			else
			{
				// Simple RLE compression
				unsigned int previousColor = 0;
				int runLength = 0;

				for (unsigned int z = 0; z < pMatrix->matrixSizeZ; ++z)
				{
					for (unsigned int y = 0; y < pMatrix->matrixSizeY; ++y)
					{
						for (unsigned int x = 0; x < pMatrix->matrixSizeX; ++x)
						{
							unsigned int color = pMatrix->pColor[x + pMatrix->matrixSizeX * (y + pMatrix->matrixSizeY * z)];

							if (x == 0 && y == 0)
							{
								// Start slice
								previousColor = color;
								runLength = 1;
							}
							else if (x == pMatrix->matrixSizeX - 1 && y == pMatrix->matrixSizeY - 1 && z == pMatrix->matrixSizeZ - 1)
							{
								// End
								fwrite(&CODEFLAG, sizeof(unsigned int), 1, pQBfile) == 1;
								fwrite(&runLength, sizeof(unsigned int), 1, pQBfile) == 1;
								fwrite(&previousColor, sizeof(unsigned int), 1, pQBfile) == 1;

								previousColor = color;
								runLength = 1;
							}
							else
							{
								if (color == previousColor)
								{
									// Match
									runLength++;
								}
								else
								{
									// No match
									fwrite(&CODEFLAG, sizeof(unsigned int), 1, pQBfile) == 1;
									fwrite(&runLength, sizeof(unsigned int), 1, pQBfile) == 1;
									fwrite(&previousColor, sizeof(unsigned int), 1, pQBfile) == 1;

									previousColor = color;
									runLength = 1;
								}
							}
						}
					}

					// Next slice
					fwrite(&CODEFLAG, sizeof(unsigned int), 1, pQBfile) == 1;
					fwrite(&runLength, sizeof(unsigned int), 1, pQBfile) == 1;
					fwrite(&previousColor, sizeof(unsigned int), 1, pQBfile) == 1;
					fwrite(&NEXTSLICEFLAG, sizeof(unsigned int), 1, pQBfile) == 1;
				}
			}
		}

		fclose(pQBfile);

		return true;
	}

	return false;
}

void QubicleBinary::GetColor(int matrixIndex, int x, int y, int z, float* r, float* g, float* b, float* a)
{
	if (m_isSingleMeshColor)
	{
		*r = m_meshSingleColorR;
		*g = m_meshSingleColorG;
		*b = m_meshSingleColorB;
		*a = 1.0f;
	}
	else
	{
		QubicleMatrix* pMatrix = m_vpMatrices[matrixIndex];

		pMatrix->GetColor(x, y, z, r, g, b, a);
	}
}

unsigned int QubicleBinary::GetColorCompact(int matrixIndex, int x, int y, int z)
{
	QubicleMatrix* pMatrix = m_vpMatrices[matrixIndex];

	return pMatrix->GetColorCompact(x, y, z);
}

bool QubicleBinary::GetSingleMeshColor(float* r, float* g, float* b, float* a) const
{
	*r = m_meshSingleColorR;
	*g = m_meshSingleColorG;
	*b = m_meshSingleColorB;
	*a = 1.0f;

	return m_isSingleMeshColor;
}

bool QubicleBinary::GetActive(int matrixIndex, int x, int y, int z)
{
	QubicleMatrix* pMatrix = m_vpMatrices[matrixIndex];

	return pMatrix->GetActive(x, y, z);
}

void QubicleBinary::SetMeshAlpha(float alpha)
{
	m_meshAlpha = alpha;

	for (unsigned int i = 0; i < m_vpMatrices.size(); i++)
	{
		m_pRenderer->ModifyMeshAlpha(alpha, m_vpMatrices[i]->pMesh);
	}
}

void QubicleBinary::SetMeshSingleColor(float r, float g, float b)
{
	m_isSingleMeshColor = true;
	m_meshSingleColorR = r;
	m_meshSingleColorG = g;
	m_meshSingleColorB = b;

	for (unsigned int i = 0; i < m_vpMatrices.size(); ++i)
	{
		m_pRenderer->ModifyMeshColor(r, g, b, m_vpMatrices[i]->pMesh);
	}
}


bool IsMergedXNegative(int* merged, int x, int y, int z, int width, int height)
{
	return (merged[x + y * width + z * width * height] & MergedSide::X_Negative) == MergedSide::X_Negative;
}

bool IsMergedXPositive(int* merged, int x, int y, int z, int width, int height)
{
	return (merged[x + y * width + z * width * height] & MergedSide::X_Positive) == MergedSide::X_Positive;
}

bool IsMergedYNegative(int* merged, int x, int y, int z, int width, int height)
{
	return (merged[x + y * width + z * width * height] & MergedSide::Y_Negative) == MergedSide::Y_Negative;
}

bool IsMergedYPositive(int* merged, int x, int y, int z, int width, int height)
{
	return (merged[x + y * width + z * width * height] & MergedSide::Y_Positive) == MergedSide::Y_Positive;
}

bool IsMergedZNegative(int* merged, int x, int y, int z, int width, int height)
{
	return (merged[x + y * width + z * width * height] & MergedSide::Z_Negative) == MergedSide::Z_Negative;
}

bool IsMergedZPositive(int* merged, int x, int y, int z, int width, int height)
{
	return (merged[x + y * width + z * width * height] & MergedSide::Z_Positive) == MergedSide::Z_Positive;
}

void QubicleBinary::CreateMesh(bool doFaceMerging)
{
	for (unsigned int matrixIndex = 0; matrixIndex < m_vpMatrices.size(); matrixIndex++)
	{
		QubicleMatrix* pMatrix = m_vpMatrices[matrixIndex];

		int* merged;

		merged = new int[pMatrix-> matrixSizeX * pMatrix->matrixSizeY * pMatrix->matrixSizeZ];

		for (unsigned int i = 0; i < pMatrix->matrixSizeX * pMatrix->matrixSizeY * pMatrix->matrixSizeZ; ++i)
		{
			merged[i] = MergedSide::None;
		}

		if (pMatrix->pMesh == nullptr)
		{
			pMatrix->pMesh = m_pRenderer->CreateMesh(MeshType::Textured);
		}

		float r = 1.0f;
		float g = 1.0f;
		float b = 1.0f;
		float a = 1.0f;

		for (unsigned int x = 0; x < pMatrix->matrixSizeX; ++x)
		{
			for (unsigned int y = 0; y < pMatrix->matrixSizeY; ++y)
			{
				for (unsigned int z = 0; z < pMatrix->matrixSizeZ; ++z)
				{
					if (GetActive(matrixIndex, x, y, z) == true)
					{
						GetColor(matrixIndex, x, y, z, &r, &g, &b, &a);

						a = 1.0f;

						glm::vec3 p1(x - BLOCK_RENDER_SIZE, y - BLOCK_RENDER_SIZE, z + BLOCK_RENDER_SIZE);
						glm::vec3 p2(x + BLOCK_RENDER_SIZE, y - BLOCK_RENDER_SIZE, z + BLOCK_RENDER_SIZE);
						glm::vec3 p3(x + BLOCK_RENDER_SIZE, y + BLOCK_RENDER_SIZE, z + BLOCK_RENDER_SIZE);
						glm::vec3 p4(x - BLOCK_RENDER_SIZE, y + BLOCK_RENDER_SIZE, z + BLOCK_RENDER_SIZE);
						glm::vec3 p5(x + BLOCK_RENDER_SIZE, y - BLOCK_RENDER_SIZE, z - BLOCK_RENDER_SIZE);
						glm::vec3 p6(x - BLOCK_RENDER_SIZE, y - BLOCK_RENDER_SIZE, z - BLOCK_RENDER_SIZE);
						glm::vec3 p7(x - BLOCK_RENDER_SIZE, y + BLOCK_RENDER_SIZE, z - BLOCK_RENDER_SIZE);
						glm::vec3 p8(x + BLOCK_RENDER_SIZE, y + BLOCK_RENDER_SIZE, z - BLOCK_RENDER_SIZE);

						glm::vec3 n1;
						unsigned int v1, v2, v3, v4;
						unsigned int t1, t2, t3, t4;

						bool doXPositive = (IsMergedXPositive(merged, x, y, z, pMatrix->matrixSizeX, pMatrix->matrixSizeY) == false);
						bool doXNegative = (IsMergedXNegative(merged, x, y, z, pMatrix->matrixSizeX, pMatrix->matrixSizeY) == false);
						bool doYPositive = (IsMergedYPositive(merged, x, y, z, pMatrix->matrixSizeX, pMatrix->matrixSizeY) == false);
						bool doYNegative = (IsMergedYNegative(merged, x, y, z, pMatrix->matrixSizeX, pMatrix->matrixSizeY) == false);
						bool doZPositive = (IsMergedZPositive(merged, x, y, z, pMatrix->matrixSizeX, pMatrix->matrixSizeY) == false);
						bool doZNegative = (IsMergedZNegative(merged, x, y, z, pMatrix->matrixSizeX, pMatrix->matrixSizeY) == false);

						// Front
						if (doZPositive && ((z == pMatrix->matrixSizeZ - 1) || z < pMatrix->matrixSizeZ - 1 && GetActive(matrixIndex, x, y, z + 1) == false))
						{
							int endX = pMatrix->matrixSizeX;
							int endY = pMatrix->matrixSizeY;

							if (doFaceMerging)
							{
								UpdateMergedSide(merged, matrixIndex, x, y, z, pMatrix->matrixSizeX, pMatrix->matrixSizeY, &p1, &p2, &p3, &p4, x, y, endX, endY, true, true, false, false);
							}

							n1 = glm::vec3(0.0f, 0.0f, 1.0f);
							v1 = m_pRenderer->AddVertexToMesh(p1, n1, r, g, b, a, pMatrix->pMesh);
							t1 = m_pRenderer->AddTextureCoordinatesToMesh(0.0f, 0.0f, pMatrix->pMesh);
							v2 = m_pRenderer->AddVertexToMesh(p2, n1, r, g, b, a, pMatrix->pMesh);
							t2 = m_pRenderer->AddTextureCoordinatesToMesh(1.0f, 0.0f, pMatrix->pMesh);
							v3 = m_pRenderer->AddVertexToMesh(p3, n1, r, g, b, a, pMatrix->pMesh);
							t3 = m_pRenderer->AddTextureCoordinatesToMesh(1.0f, 1.0f, pMatrix->pMesh);
							v4 = m_pRenderer->AddVertexToMesh(p4, n1, r, g, b, a, pMatrix->pMesh);
							t4 = m_pRenderer->AddTextureCoordinatesToMesh(0.0f, 1.0f, pMatrix->pMesh);

							assert(!(t1 == -1 || t2 == -1 || t3 == -1 || t4 == -1));

							m_pRenderer->AddTriangleToMesh(v1, v2, v3, pMatrix->pMesh);
							m_pRenderer->AddTriangleToMesh(v1, v3, v4, pMatrix->pMesh);
						}

						p1 = glm::vec3(x - BLOCK_RENDER_SIZE, y - BLOCK_RENDER_SIZE, z + BLOCK_RENDER_SIZE);
						p2 = glm::vec3(x + BLOCK_RENDER_SIZE, y - BLOCK_RENDER_SIZE, z + BLOCK_RENDER_SIZE);
						p3 = glm::vec3(x + BLOCK_RENDER_SIZE, y + BLOCK_RENDER_SIZE, z + BLOCK_RENDER_SIZE);
						p4 = glm::vec3(x - BLOCK_RENDER_SIZE, y + BLOCK_RENDER_SIZE, z + BLOCK_RENDER_SIZE);
						p5 = glm::vec3(x + BLOCK_RENDER_SIZE, y - BLOCK_RENDER_SIZE, z - BLOCK_RENDER_SIZE);
						p6 = glm::vec3(x - BLOCK_RENDER_SIZE, y - BLOCK_RENDER_SIZE, z - BLOCK_RENDER_SIZE);
						p7 = glm::vec3(x - BLOCK_RENDER_SIZE, y + BLOCK_RENDER_SIZE, z - BLOCK_RENDER_SIZE);
						p8 = glm::vec3(x + BLOCK_RENDER_SIZE, y + BLOCK_RENDER_SIZE, z - BLOCK_RENDER_SIZE);

						// Back
						if (doZNegative && ((z == 0) || (z > 0 && GetActive(matrixIndex, x, y, z - 1) == false)))
						{
							int endX = pMatrix->matrixSizeX;
							int endY = pMatrix->matrixSizeY;

							if (doFaceMerging)
							{
								UpdateMergedSide(merged, matrixIndex, x, y, z, pMatrix->matrixSizeX, pMatrix->matrixSizeY, &p6, &p5, &p8, &p7, x, y, endX, endY, false, true, false, false);
							}

							n1 = glm::vec3(0.0f, 0.0f, -1.0f);
							v1 = m_pRenderer->AddVertexToMesh(p5, n1, r, g, b, a, pMatrix->pMesh);
							t1 = m_pRenderer->AddTextureCoordinatesToMesh(0.0f, 0.0f, pMatrix->pMesh);
							v2 = m_pRenderer->AddVertexToMesh(p6, n1, r, g, b, a, pMatrix->pMesh);
							t2 = m_pRenderer->AddTextureCoordinatesToMesh(1.0f, 0.0f, pMatrix->pMesh);
							v3 = m_pRenderer->AddVertexToMesh(p7, n1, r, g, b, a, pMatrix->pMesh);
							t3 = m_pRenderer->AddTextureCoordinatesToMesh(1.0f, 1.0f, pMatrix->pMesh);
							v4 = m_pRenderer->AddVertexToMesh(p8, n1, r, g, b, a, pMatrix->pMesh);
							t4 = m_pRenderer->AddTextureCoordinatesToMesh(0.0f, 1.0f, pMatrix->pMesh);

							assert(!(t1 == -1 || t2 == -1 || t3 == -1 || t4 == -1));

							m_pRenderer->AddTriangleToMesh(v1, v2, v3, pMatrix->pMesh);
							m_pRenderer->AddTriangleToMesh(v1, v3, v4, pMatrix->pMesh);
						}

						p1 = glm::vec3(x - BLOCK_RENDER_SIZE, y - BLOCK_RENDER_SIZE, z + BLOCK_RENDER_SIZE);
						p2 = glm::vec3(x + BLOCK_RENDER_SIZE, y - BLOCK_RENDER_SIZE, z + BLOCK_RENDER_SIZE);
						p3 = glm::vec3(x + BLOCK_RENDER_SIZE, y + BLOCK_RENDER_SIZE, z + BLOCK_RENDER_SIZE);
						p4 = glm::vec3(x - BLOCK_RENDER_SIZE, y + BLOCK_RENDER_SIZE, z + BLOCK_RENDER_SIZE);
						p5 = glm::vec3(x + BLOCK_RENDER_SIZE, y - BLOCK_RENDER_SIZE, z - BLOCK_RENDER_SIZE);
						p6 = glm::vec3(x - BLOCK_RENDER_SIZE, y - BLOCK_RENDER_SIZE, z - BLOCK_RENDER_SIZE);
						p7 = glm::vec3(x - BLOCK_RENDER_SIZE, y + BLOCK_RENDER_SIZE, z - BLOCK_RENDER_SIZE);
						p8 = glm::vec3(x + BLOCK_RENDER_SIZE, y + BLOCK_RENDER_SIZE, z - BLOCK_RENDER_SIZE);

						// Right
						if (doXPositive && ((x == pMatrix->matrixSizeX - 1) || (x < pMatrix->matrixSizeX - 1 && GetActive(matrixIndex, x + 1, y, z) == false)))
						{
							int endX = pMatrix->matrixSizeZ;
							int endY = pMatrix->matrixSizeY;

							if (doFaceMerging)
							{
								UpdateMergedSide(merged, matrixIndex, x, y, z, pMatrix->matrixSizeX, pMatrix->matrixSizeY, &p5, &p2, &p3, &p8, z, y, endX, endY, true, false, true, false);
							}

							n1 = glm::vec3(1.0f, 0.0f, 0.0f);
							v1 = m_pRenderer->AddVertexToMesh(p2, n1, r, g, b, a, pMatrix->pMesh);
							t1 = m_pRenderer->AddTextureCoordinatesToMesh(0.0f, 0.0f, pMatrix->pMesh);
							v2 = m_pRenderer->AddVertexToMesh(p5, n1, r, g, b, a, pMatrix->pMesh);
							t2 = m_pRenderer->AddTextureCoordinatesToMesh(1.0f, 0.0f, pMatrix->pMesh);
							v3 = m_pRenderer->AddVertexToMesh(p8, n1, r, g, b, a, pMatrix->pMesh);
							t3 = m_pRenderer->AddTextureCoordinatesToMesh(1.0f, 1.0f, pMatrix->pMesh);
							v4 = m_pRenderer->AddVertexToMesh(p3, n1, r, g, b, a, pMatrix->pMesh);
							t4 = m_pRenderer->AddTextureCoordinatesToMesh(0.0f, 1.0f, pMatrix->pMesh);

							assert(!(t1 == -1 || t2 == -1 || t3 == -1 || t4 == -1));

							m_pRenderer->AddTriangleToMesh(v1, v2, v3, pMatrix->pMesh);
							m_pRenderer->AddTriangleToMesh(v1, v3, v4, pMatrix->pMesh);
						}

						p1 = glm::vec3(x - BLOCK_RENDER_SIZE, y - BLOCK_RENDER_SIZE, z + BLOCK_RENDER_SIZE);
						p2 = glm::vec3(x + BLOCK_RENDER_SIZE, y - BLOCK_RENDER_SIZE, z + BLOCK_RENDER_SIZE);
						p3 = glm::vec3(x + BLOCK_RENDER_SIZE, y + BLOCK_RENDER_SIZE, z + BLOCK_RENDER_SIZE);
						p4 = glm::vec3(x - BLOCK_RENDER_SIZE, y + BLOCK_RENDER_SIZE, z + BLOCK_RENDER_SIZE);
						p5 = glm::vec3(x + BLOCK_RENDER_SIZE, y - BLOCK_RENDER_SIZE, z - BLOCK_RENDER_SIZE);
						p6 = glm::vec3(x - BLOCK_RENDER_SIZE, y - BLOCK_RENDER_SIZE, z - BLOCK_RENDER_SIZE);
						p7 = glm::vec3(x - BLOCK_RENDER_SIZE, y + BLOCK_RENDER_SIZE, z - BLOCK_RENDER_SIZE);
						p8 = glm::vec3(x + BLOCK_RENDER_SIZE, y + BLOCK_RENDER_SIZE, z - BLOCK_RENDER_SIZE);

						// Left
						if (doXNegative && ((x == 0) || (x > 0 && GetActive(matrixIndex, x - 1, y, z) == false)))
						{
							int endX = pMatrix->matrixSizeZ;
							int endY = pMatrix->matrixSizeY;

							if (doFaceMerging)
							{
								UpdateMergedSide(merged, matrixIndex, x, y, z, pMatrix->matrixSizeX, pMatrix->matrixSizeY, &p6, &p1, &p4, &p7, z, y, endX, endY, false, false, true, false);
							}

							n1 = glm::vec3(-1.0f, 0.0f, 0.0f);
							v1 = m_pRenderer->AddVertexToMesh(p6, n1, r, g, b, a, pMatrix->pMesh);
							t1 = m_pRenderer->AddTextureCoordinatesToMesh(0.0f, 0.0f, pMatrix->pMesh);
							v2 = m_pRenderer->AddVertexToMesh(p1, n1, r, g, b, a, pMatrix->pMesh);
							t2 = m_pRenderer->AddTextureCoordinatesToMesh(1.0f, 0.0f, pMatrix->pMesh);
							v3 = m_pRenderer->AddVertexToMesh(p4, n1, r, g, b, a, pMatrix->pMesh);
							t3 = m_pRenderer->AddTextureCoordinatesToMesh(1.0f, 1.0f, pMatrix->pMesh);
							v4 = m_pRenderer->AddVertexToMesh(p7, n1, r, g, b, a, pMatrix->pMesh);
							t4 = m_pRenderer->AddTextureCoordinatesToMesh(0.0f, 1.0f, pMatrix->pMesh);

							assert(!(t1 == -1 || t2 == -1 || t3 == -1 || t4 == -1));

							m_pRenderer->AddTriangleToMesh(v1, v2, v3, pMatrix->pMesh);
							m_pRenderer->AddTriangleToMesh(v1, v3, v4, pMatrix->pMesh);
						}

						p1 = glm::vec3(x - BLOCK_RENDER_SIZE, y - BLOCK_RENDER_SIZE, z + BLOCK_RENDER_SIZE);
						p2 = glm::vec3(x + BLOCK_RENDER_SIZE, y - BLOCK_RENDER_SIZE, z + BLOCK_RENDER_SIZE);
						p3 = glm::vec3(x + BLOCK_RENDER_SIZE, y + BLOCK_RENDER_SIZE, z + BLOCK_RENDER_SIZE);
						p4 = glm::vec3(x - BLOCK_RENDER_SIZE, y + BLOCK_RENDER_SIZE, z + BLOCK_RENDER_SIZE);
						p5 = glm::vec3(x + BLOCK_RENDER_SIZE, y - BLOCK_RENDER_SIZE, z - BLOCK_RENDER_SIZE);
						p6 = glm::vec3(x - BLOCK_RENDER_SIZE, y - BLOCK_RENDER_SIZE, z - BLOCK_RENDER_SIZE);
						p7 = glm::vec3(x - BLOCK_RENDER_SIZE, y + BLOCK_RENDER_SIZE, z - BLOCK_RENDER_SIZE);
						p8 = glm::vec3(x + BLOCK_RENDER_SIZE, y + BLOCK_RENDER_SIZE, z - BLOCK_RENDER_SIZE);

						// Top
						if (doYPositive && ((y == pMatrix->matrixSizeY - 1) || (y < pMatrix->matrixSizeY - 1 && GetActive(matrixIndex, x, y + 1, z) == false)))
						{
							int endX = pMatrix->matrixSizeX;
							int endY = pMatrix->matrixSizeZ;

							if (doFaceMerging)
							{
								UpdateMergedSide(merged, matrixIndex, x, y, z, pMatrix->matrixSizeX, pMatrix->matrixSizeY, &p7, &p8, &p3, &p4, x, z, endX, endY, true, false, false, true);
							}

							n1 = glm::vec3(0.0f, 1.0f, 0.0f);
							v1 = m_pRenderer->AddVertexToMesh(p4, n1, r, g, b, a, pMatrix->pMesh);
							t1 = m_pRenderer->AddTextureCoordinatesToMesh(0.0f, 0.0f, pMatrix->pMesh);
							v2 = m_pRenderer->AddVertexToMesh(p3, n1, r, g, b, a, pMatrix->pMesh);
							t2 = m_pRenderer->AddTextureCoordinatesToMesh(1.0f, 0.0f, pMatrix->pMesh);
							v3 = m_pRenderer->AddVertexToMesh(p8, n1, r, g, b, a, pMatrix->pMesh);
							t3 = m_pRenderer->AddTextureCoordinatesToMesh(1.0f, 1.0f, pMatrix->pMesh);
							v4 = m_pRenderer->AddVertexToMesh(p7, n1, r, g, b, a, pMatrix->pMesh);
							t4 = m_pRenderer->AddTextureCoordinatesToMesh(0.0f, 1.0f, pMatrix->pMesh);

							assert(!(t1 == -1 || t2 == -1 || t3 == -1 || t4 == -1));

							m_pRenderer->AddTriangleToMesh(v1, v2, v3, pMatrix->pMesh);
							m_pRenderer->AddTriangleToMesh(v1, v3, v4, pMatrix->pMesh);
						}

						p1 = glm::vec3(x - BLOCK_RENDER_SIZE, y - BLOCK_RENDER_SIZE, z + BLOCK_RENDER_SIZE);
						p2 = glm::vec3(x + BLOCK_RENDER_SIZE, y - BLOCK_RENDER_SIZE, z + BLOCK_RENDER_SIZE);
						p3 = glm::vec3(x + BLOCK_RENDER_SIZE, y + BLOCK_RENDER_SIZE, z + BLOCK_RENDER_SIZE);
						p4 = glm::vec3(x - BLOCK_RENDER_SIZE, y + BLOCK_RENDER_SIZE, z + BLOCK_RENDER_SIZE);
						p5 = glm::vec3(x + BLOCK_RENDER_SIZE, y - BLOCK_RENDER_SIZE, z - BLOCK_RENDER_SIZE);
						p6 = glm::vec3(x - BLOCK_RENDER_SIZE, y - BLOCK_RENDER_SIZE, z - BLOCK_RENDER_SIZE);
						p7 = glm::vec3(x - BLOCK_RENDER_SIZE, y + BLOCK_RENDER_SIZE, z - BLOCK_RENDER_SIZE);
						p8 = glm::vec3(x + BLOCK_RENDER_SIZE, y + BLOCK_RENDER_SIZE, z - BLOCK_RENDER_SIZE);

						// Bottom
						if (doYNegative && ((y == 0) || (y > 0 && GetActive(matrixIndex, x, y - 1, z) == false)))
						{
							int endX = pMatrix->matrixSizeX;
							int endY = pMatrix->matrixSizeZ;

							if (doFaceMerging)
							{
								UpdateMergedSide(merged, matrixIndex, x, y, z, pMatrix->matrixSizeX, pMatrix->matrixSizeY, &p6, &p5, &p2, &p1, x, z, endX, endY, false, false, false, true);
							}

							n1 = glm::vec3(0.0f, -1.0f, 0.0f);
							v1 = m_pRenderer->AddVertexToMesh(p6, n1, r, g, b, a, pMatrix->pMesh);
							t1 = m_pRenderer->AddTextureCoordinatesToMesh(0.0f, 0.0f, pMatrix->pMesh);
							v2 = m_pRenderer->AddVertexToMesh(p5, n1, r, g, b, a, pMatrix->pMesh);
							t2 = m_pRenderer->AddTextureCoordinatesToMesh(1.0f, 0.0f, pMatrix->pMesh);
							v3 = m_pRenderer->AddVertexToMesh(p2, n1, r, g, b, a, pMatrix->pMesh);
							t3 = m_pRenderer->AddTextureCoordinatesToMesh(1.0f, 1.0f, pMatrix->pMesh);
							v4 = m_pRenderer->AddVertexToMesh(p1, n1, r, g, b, a, pMatrix->pMesh);
							t4 = m_pRenderer->AddTextureCoordinatesToMesh(0.0f, 1.0f, pMatrix->pMesh);

							assert(!(t1 == -1 || t2 == -1 || t3 == -1 || t4 == -1));

							m_pRenderer->AddTriangleToMesh(v1, v2, v3, pMatrix->pMesh);
							m_pRenderer->AddTriangleToMesh(v1, v3, v4, pMatrix->pMesh);
						}
					}
				}
			}
		}

		m_pRenderer->FinishMesh(-1, m_materialID, pMatrix->pMesh);

		// Delete the merged array
		delete[] merged;
	}
}

void QubicleBinary::RebuildMesh(bool doFaceMerging)
{
	for (unsigned int i = 0; i < m_vpMatrices.size(); ++i)
	{
		m_pRenderer->ClearMesh(m_vpMatrices[i]->pMesh);
		m_vpMatrices[i]->pMesh = nullptr;
	}

	CreateMesh(doFaceMerging);
}

void QubicleBinary::UpdateMergedSide(int* merged, int matrixIndex, int blockX, int blockY, int blockZ, int width, int height, glm::vec3* p1, glm::vec3* p2, glm::vec3* p3, glm::vec3* p4, int startX, int startY, int maxX, int maxY, bool positive, bool zFace, bool xFace, bool yFace)
{
	QubicleMatrix* pMatrix = m_vpMatrices[matrixIndex];

	bool doMore = true;
	unsigned int incrementX = 0;
	unsigned int incrementZ = 0;
	unsigned int incrementY = 0;

	int change = 1;

	if (zFace || yFace)
	{
		incrementX = 1;
		incrementY = 1;
	}
	if (xFace)
	{
		incrementZ = 1;
		incrementY = 1;
	}

	// 1st phase
	int incrementer = 1;
	while (doMore)
	{
		if (startX + incrementer >= maxX)
		{
			doMore = false;
		}
		else
		{
			bool doPhase1Merge = true;
			float r1, r2, g1, g2, b1, b2, a1, a2;
			GetColor(matrixIndex, blockX, blockY, blockZ, &r1, &g1, &b1, &a1);
			GetColor(matrixIndex, blockX + incrementX, blockY, blockZ + incrementZ, &r2, &g2, &b2, &a2);
			
			if (r1 != r2 || g1 != g2 || b1 != b2 || a1 != a2)
			{
				// Don't do any phase 1 merging if we don't have the same Color variation
				doMore = false;
			}
			else
			{
				if ((xFace && positive && blockX + incrementX + 1 == pMatrix->matrixSizeX) ||
					(xFace && !positive && blockX + incrementX == 0) ||
					(yFace && positive && blockY + 1 == pMatrix->matrixSizeY) ||
					(yFace && !positive && blockY == 0) ||
					(zFace && positive && blockZ + incrementZ + 1 == pMatrix->matrixSizeZ) ||
					(zFace && !positive && blockZ + incrementZ == 0))
				{
					doPhase1Merge = false;
					doMore = false;
				}
				// Don't do any phase 1 merging if we find an inactive block or already merged block in our path
				else if (xFace && positive && (blockX + incrementX + 1) < pMatrix->matrixSizeX && GetActive(matrixIndex, blockX + incrementX + 1, blockY, blockZ + incrementZ) == true)
				{
					doPhase1Merge = false;
					doMore = false;
				}
				else if (xFace && !positive && (blockX + incrementX) > 0 && GetActive(matrixIndex, blockX + incrementX - 1, blockY, blockZ + incrementZ) == true)
				{
					doPhase1Merge = false;
					doMore = false;
				}
				else if (yFace && positive && (blockY + 1) < pMatrix->matrixSizeY && GetActive(matrixIndex, blockX + incrementX, blockY + 1, blockZ + incrementZ) == true)
				{
					doPhase1Merge = false;
					doMore = false;
				}
				else if (yFace && !positive && blockY > 0 && GetActive(matrixIndex, blockX + incrementX, blockY - 1, blockZ + incrementZ) == true)
				{
					doPhase1Merge = false;
					doMore = false;
				}
				else if (zFace && positive && (blockZ + incrementZ + 1) < pMatrix->matrixSizeZ && GetActive(matrixIndex, blockX + incrementX, blockY, blockZ + incrementZ + 1) == true)
				{
					doPhase1Merge = false;
					doMore = false;
				}
				else if (zFace && !positive && (blockZ + incrementZ) > 0 && GetActive(matrixIndex, blockX + incrementX, blockY, blockZ + incrementZ - 1) == true)
				{
					doPhase1Merge = false;
					doMore = false;
				}
				else if (GetActive(matrixIndex, blockX + incrementX, blockY, blockZ + incrementZ) == false)
				{
					doPhase1Merge = false;
					doMore = false;
				}
				else
				{
					if (xFace)
					{
						doPhase1Merge = positive ? (IsMergedXPositive(merged, blockX + incrementX, blockY, blockZ + incrementZ, width, height) == false) : (IsMergedXNegative(merged, blockX + incrementX, blockY, blockZ + incrementZ, width, height) == false);
					}
					if (zFace)
					{
						doPhase1Merge = positive ? (IsMergedZPositive(merged, blockX + incrementX, blockY, blockZ + incrementZ, width, height) == false) : (IsMergedZNegative(merged, blockX + incrementX, blockY, blockZ + incrementZ, width, height) == false);
					}
					if (yFace)
					{
						doPhase1Merge = positive ? (IsMergedYPositive(merged, blockX + incrementX, blockY, blockZ + incrementZ, width, height) == false) : (IsMergedYNegative(merged, blockX + incrementX, blockY, blockZ + incrementZ, width, height) == false);
					}
				}

				if (doPhase1Merge)
				{
					if (zFace || yFace)
					{
						(*p2).x += change * (BLOCK_RENDER_SIZE * 2.0f);
						(*p3).x += change * (BLOCK_RENDER_SIZE * 2.0f);
					}
					if (xFace)
					{
						(*p2).z += change * (BLOCK_RENDER_SIZE * 2.0f);
						(*p3).z += change * (BLOCK_RENDER_SIZE * 2.0f);
					}

					if (positive)
					{
						if (zFace)
						{
							merged[(blockX + incrementX) + blockY * width + (blockZ + incrementZ) * width * height] |= MergedSide::Z_Positive;
						}
						if (xFace)
						{
							merged[(blockX + incrementX) + blockY * width + (blockZ + incrementZ) * width * height] |= MergedSide::X_Positive;
						}
						if (yFace)
						{
							merged[(blockX + incrementX) + blockY * width + (blockZ + incrementZ) * width * height] |= MergedSide::Y_Positive;
						}
					}
					else
					{
						if (zFace)
						{
							merged[(blockX + incrementX) + blockY * width + (blockZ + incrementZ) * width * height] |= MergedSide::Z_Negative;
						}
						if (xFace)
						{
							merged[(blockX + incrementX) + blockY * width + (blockZ + incrementZ) * width * height] |= MergedSide::X_Negative;
						}
						if (yFace)
						{
							merged[(blockX + incrementX) + blockY * width + (blockZ + incrementZ) * width * height] |= MergedSide::Y_Negative;
						}
					}
				}
				else
				{
					doMore = false;
				}
			}
		}

		if (zFace || yFace)
		{
			incrementX += change;
		}
		if (xFace)
		{
			incrementZ += change;
		}

		incrementer += change;
	}


	// 2nd phase
	int loop = incrementer;
	incrementer = incrementY;

	doMore = true;
	while (doMore)
	{
		if (startY + incrementer >= maxY)
		{
			doMore = false;
		}
		else
		{
			for (int i = 0; i < loop - 1; ++i)
			{
				// Don't do any phase 2 merging is we have any inactive blocks or already merged blocks on the row
				if (zFace)
				{
					float r1, r2, g1, g2, b1, b2, a1, a2;
					GetColor(matrixIndex, blockX, blockY, blockZ, &r1, &g1, &b1, &a1);
					GetColor(matrixIndex, blockX + i, blockY + incrementY, blockZ, &r2, &g2, &b2, &a2);

					if (positive && (blockZ + 1) < pMatrix->matrixSizeZ && GetActive(matrixIndex, blockX + i, blockY + incrementY, blockZ + 1) == true)
					{
						doMore = false;
					}
					else if (!positive && blockZ > 0 && GetActive(matrixIndex, blockX + i, blockY + incrementY, blockZ - 1) == true)
					{
						doMore = false;
					}
					else if (GetActive(matrixIndex, blockX + i, blockY + incrementY, blockZ) == false || (positive ? (IsMergedZPositive(merged, blockX + i, blockY + incrementY, blockZ, width, height) == true) : (IsMergedZNegative(merged, blockX + i, blockY + incrementY, blockZ, width, height) == true)))
					{
						// Failed active or already merged check
						doMore = false;
					}
					else if (r1 != r2 || g1 != g2 || b1 != b2 || a1 != a2)
					{
						// Failed Color check
						doMore = false;
					}
				}

				if (xFace)
				{
					float r1, r2, g1, g2, b1, b2, a1, a2;
					GetColor(matrixIndex, blockX, blockY, blockZ, &r1, &g1, &b1, &a1);
					GetColor(matrixIndex, blockX, blockY + incrementY, blockZ + i, &r2, &g2, &b2, &a2);

					if (positive && (blockX + 1) < pMatrix->matrixSizeX && GetActive(matrixIndex, blockX + 1, blockY + incrementY, blockZ + i) == true)
					{
						doMore = false;
					}
					else if (!positive && (blockX) > 0 && GetActive(matrixIndex, blockX - 1, blockY + incrementY, blockZ + i) == true)
					{
						doMore = false;
					}
					else if (GetActive(matrixIndex, blockX, blockY + incrementY, blockZ + i) == false || (positive ? (IsMergedXPositive(merged, blockX, blockY + incrementY, blockZ + i, width, height) == true) : (IsMergedXNegative(merged, blockX, blockY + incrementY, blockZ + i, width, height) == true)))
					{
						// Failed active or already merged check
						doMore = false;
					}
					else if (r1 != r2 || g1 != g2 || b1 != b2 || a1 != a2)
					{
						// Failed Color check
						doMore = false;
					}
				}

				if (yFace)
				{
					float r1, r2, g1, g2, b1, b2, a1, a2;
					GetColor(matrixIndex, blockX, blockY, blockZ, &r1, &g1, &b1, &a1);
					GetColor(matrixIndex, blockX + i, blockY, blockZ + incrementY, &r2, &g2, &b2, &a2);

					if (positive && (blockY + 1) < pMatrix->matrixSizeY && GetActive(matrixIndex, blockX + i, blockY + 1, blockZ + incrementY) == true)
					{
						doMore = false;
					}
					else if (!positive && blockY > 0 && GetActive(matrixIndex, blockX + i, blockY - 1, blockZ + incrementY) == true)
					{
						doMore = false;
					}
					else if (GetActive(matrixIndex, blockX + i, blockY, blockZ + incrementY) == false || (positive ? (IsMergedYPositive(merged, blockX + i, blockY, blockZ + incrementY, width, height) == true) : (IsMergedYNegative(merged, blockX + i, blockY, blockZ + incrementY, width, height) == true)))
					{
						// Failed active or already merged check
						doMore = false;
					}
					else if (r1 != r2 || g1 != g2 || b1 != b2 || a1 != a2)
					{
						// Failed Color check
						doMore = false;
					}
				}
			}

			if (doMore == true)
			{
				if (zFace || xFace)
				{
					(*p3).y += change * (BLOCK_RENDER_SIZE * 2.0f);
					(*p4).y += change * (BLOCK_RENDER_SIZE * 2.0f);
				}
				if (yFace)
				{
					(*p3).z += change * (BLOCK_RENDER_SIZE * 2.0f);
					(*p4).z += change * (BLOCK_RENDER_SIZE * 2.0f);
				}

				for (int i = 0; i < loop - 1; ++i)
				{
					if (positive)
					{
						if (zFace)
						{
							merged[(blockX + i) + (blockY + incrementY) * width + blockZ * width * height] |= MergedSide::Z_Positive;
						}
						if (xFace)
						{
							merged[blockX + (blockY + incrementY) * width + (blockZ + i) * width * height] |= MergedSide::X_Positive;
						}
						if (yFace)
						{
							merged[(blockX + i) + blockY * width + (blockZ + incrementY) * width * height] |= MergedSide::Y_Positive;
						}
					}
					else
					{
						if (zFace)
						{
							merged[(blockX + i) + (blockY + incrementY) * width + blockZ * width * height] |= MergedSide::Z_Negative;
						}
						if (xFace)
						{
							merged[blockX + (blockY + incrementY) * width + (blockZ + i) * width * height] |= MergedSide::X_Negative;
						}
						if (yFace)
						{
							merged[(blockX + i) + blockY * width + (blockZ + incrementY) * width * height] |= MergedSide::Y_Negative;
						}
					}
				}
			}
		}

		incrementY += change;
		incrementer += change;
	}
}

int QubicleBinary::GetNumMatrices() const
{
	return m_numMatrices;
}

QubicleMatrix* QubicleBinary::GetQubicleMatrix(int index)
{
	return m_vpMatrices[index];
}

QubicleMatrix* QubicleBinary::GetQubicleMatrix(const char* matrixName)
{
	for (unsigned int i = 0; i < m_numMatrices; ++i)
	{
		if (strcmp(m_vpMatrices[i]->name, matrixName) == 0)
		{
			return GetQubicleMatrix(i);
		}
	}

	return nullptr;
}

const char* QubicleBinary::GetMatrixName(int index)
{
	return m_vpMatrices[index]->name;
}

float QubicleBinary::GetMatrixScale(int index)
{
	return m_vpMatrices[index]->scale;
}

glm::vec3 QubicleBinary::GetMatrixOffset(int index)
{
	return glm::vec3(m_vpMatrices[index]->offsetX, m_vpMatrices[index]->offsetY, m_vpMatrices[index]->offsetZ);
}

void QubicleBinary::SetupMatrixBones(MS3DAnimator* pSkeleton)
{
	for (unsigned int i = 0; i < m_numMatrices; ++i)
	{
		int boneIndex = pSkeleton->GetModel()->GetBoneIndex(m_vpMatrices[i]->name);

		if (boneIndex != -1)
		{
			m_vpMatrices[i]->boneIndex = boneIndex;
		}
	}
}

void QubicleBinary::SetScaleAndOffsetForMatrix(const char* matrixName, float scale, float xOffset, float yOffset, float zOffset)
{
	for (unsigned int i = 0; i < m_numMatrices; ++i)
	{
		if (strcmp(m_vpMatrices[i]->name, matrixName) == 0)
		{
			m_vpMatrices[i]->scale = scale;
			m_vpMatrices[i]->offsetX = xOffset;
			m_vpMatrices[i]->offsetY = yOffset;
			m_vpMatrices[i]->offsetZ = zOffset;
		}
	}
}

float QubicleBinary::GetScale(const char* matrixName)
{
	for (unsigned int i = 0; i < m_numMatrices; ++i)
	{
		if (strcmp(m_vpMatrices[i]->name, matrixName) == 0)
		{
			return m_vpMatrices[i]->scale;
		}
	}

	return 1.0f;
}

glm::vec3 QubicleBinary::GetOffset(const char* matrixName)
{
	for (unsigned int i = 0; i < m_numMatrices; ++i)
	{
		if (strcmp(m_vpMatrices[i]->name, matrixName) == 0)
		{
			return glm::vec3(m_vpMatrices[i]->offsetX, m_vpMatrices[i]->offsetY, m_vpMatrices[i]->offsetZ);
		}
	}

	return glm::vec3(0.0f, 0.0f, 0.0f);
}

void QubicleBinary::SwapMatrix(const char* matrixName, QubicleMatrix* pMatrix, bool copyMatrixParams)
{
	int matrixIndex = GetMatrixIndexForName(matrixName);

	if (matrixIndex != -1)
	{
		if (copyMatrixParams)
		{
			pMatrix->nameLength = m_vpMatrices[matrixIndex]->nameLength;
			pMatrix->name = m_vpMatrices[matrixIndex]->name;
			pMatrix->boneIndex = m_vpMatrices[matrixIndex]->boneIndex;
			pMatrix->scale = m_vpMatrices[matrixIndex]->scale;
			pMatrix->offsetX = m_vpMatrices[matrixIndex]->offsetX;
			pMatrix->offsetY = m_vpMatrices[matrixIndex]->offsetY;
			pMatrix->offsetZ = m_vpMatrices[matrixIndex]->offsetZ;
		}

		m_vpMatrices[matrixIndex]->isRemoved = false;
		m_vpMatrices[matrixIndex] = pMatrix;
	}
}

void QubicleBinary::AddQubicleMatrix(QubicleMatrix* pNewMatrix, bool copyMatrixParams)
{
	// First check if this matrix already exists
	QubicleMatrix* pExistingMatrix = GetQubicleMatrix(pNewMatrix->name);

	if (pExistingMatrix != nullptr)
	{
		// Replace existing matrix
		SwapMatrix(pNewMatrix->name, pNewMatrix, copyMatrixParams);
	}
	else
	{
		// Add new matrix
		m_vpMatrices.push_back(pNewMatrix);
		pNewMatrix->isRemoved = false;
		m_numMatrices++;
	}
}

void QubicleBinary::RemoveQubicleMatrix(const char* matrixName)
{
	for (unsigned int i = 0; i < m_vpMatrices.size(); ++i)
	{
		if (strcmp(m_vpMatrices[i]->name, matrixName) == 0)
		{
			m_vpMatrices[i]->isRemoved = true;

			return;
		}
	}
}

void QubicleBinary::SetQubicleMatrixRender(const char* matrixName, bool render)
{
	for (unsigned int i = 0; i < m_vpMatrices.size(); ++i)
	{
		if (strcmp(m_vpMatrices[i]->name, matrixName) == 0)
		{
			m_vpMatrices[i]->isRemoved = (render == false);

			return;
		}
	}
}

// Sub selection
std::string QubicleBinary::GetSubSelectionName(int pickingId)
{
	int index = pickingId - SUBSELECTION_NAMEPICKING_OFFSET;

	if (index >= 0 && index <= m_numMatrices - 1)
	{
		return m_vpMatrices[index]->name;
	}

	return "";
}

// Rendering modes
void QubicleBinary::SetWireFrameRender(bool wireframe)
{
	m_isRenderWireFrame = wireframe;
}

//Rendering
void QubicleBinary::Render(bool renderOutline, bool reflection, bool silhouette, Color OutlineColor)
{
	m_pRenderer->PushMatrix();

	for (unsigned int i = 0; i < m_numMatrices; ++i)
	{
		if (m_vpMatrices[i]->isRemoved == true)
		{
			continue;
		}

		m_pRenderer->PushMatrix();
		// Scale for external matrix scale value
		m_pRenderer->ScaleWorldMatrix(m_vpMatrices[i]->scale, m_vpMatrices[i]->scale, m_vpMatrices[i]->scale);

		// Translate for initial block offset
		m_pRenderer->TranslateWorldMatrix(0.5f, 0.5f, 0.5f);

		// Translate to center of model
		m_pRenderer->TranslateWorldMatrix(-m_vpMatrices[i]->matrixSizeX * 0.5f, -m_vpMatrices[i]->matrixSizeY * 0.5f, -m_vpMatrices[i]->matrixSizeZ * 0.5f);

		// Translate for external matrix offset value
		m_pRenderer->TranslateWorldMatrix(m_vpMatrices[i]->offsetX, m_vpMatrices[i]->offsetY, m_vpMatrices[i]->offsetZ);

		// Store cull mode
		CullMode cullMode = m_pRenderer->GetCullMode();

		if (renderOutline)
		{
			m_pRenderer->DisableDepthTest();
			m_pRenderer->SetLineWidth(3.0f);
			m_pRenderer->SetCullMode(CullMode::FRONT);
			m_pRenderer->SetRenderMode(RenderMode::WIREFRAME);
			m_pRenderer->ImmediateColorAlpha(OutlineColor.GetRed(), OutlineColor.GetGreen(), OutlineColor.GetBlue(), OutlineColor.GetAlpha());
		}
		else if (silhouette)
		{
			m_pRenderer->DisableDepthTest();
			m_pRenderer->SetCullMode(CullMode::FRONT);
			m_pRenderer->SetRenderMode(RenderMode::SOLID);
			m_pRenderer->ImmediateColorAlpha(OutlineColor.GetRed(), OutlineColor.GetGreen(), OutlineColor.GetBlue(), OutlineColor.GetAlpha());
		}
		else if (m_isRenderWireFrame)
		{
			m_pRenderer->SetLineWidth(1.0f);
			m_pRenderer->SetRenderMode(RenderMode::WIREFRAME);
			m_pRenderer->SetCullMode(CullMode::NOCULL);
		}
		else
		{
			m_pRenderer->SetRenderMode(RenderMode::SOLID);
		}

		// Store the model matrix
		if (reflection == false)
		{
			m_pRenderer->GetModelMatrix(&m_vpMatrices[i]->modelMatrix);
		}

		m_pRenderer->PushMatrix();
		m_pRenderer->StartMeshRender();

		// Texture manipulation (for shadow rendering)
		Matrix4 worldMatrix;
		m_pRenderer->GetModelMatrix(&worldMatrix);

		m_pRenderer->PushTextureMatrix();
		m_pRenderer->MultiplyWorldMatrix(worldMatrix);

		if (m_meshAlpha < 1.0f)
		{
			m_pRenderer->EnableTransparency(BlendFunction::SRC_ALPHA, BlendFunction::ONE_MINUS_SRC_ALPHA);
		}
		m_pRenderer->EnableMaterial(m_materialID);

		if (renderOutline || silhouette)
		{
			m_pRenderer->EndMeshRender();
			m_pRenderer->RenderMeshNoColor(m_vpMatrices[i]->pMesh);
		}
		else
		{
			m_pRenderer->MeshStaticBufferRender(m_vpMatrices[i]->pMesh);
		}

		if (m_meshAlpha < 1.0f)
		{
			m_pRenderer->DisableTransparency();
		}

		// Texture manipulation (for shadow rendering)
		m_pRenderer->PopTextureMatrix();

		m_pRenderer->EndMeshRender();
		m_pRenderer->PopMatrix();

		// Restore cull mode
		m_pRenderer->SetCullMode(cullMode);

		if (renderOutline || silhouette)
		{
			m_pRenderer->EnableDepthTest(DepthTest::LESS);
		}
		m_pRenderer->PopMatrix();
	}
	m_pRenderer->PopMatrix();
}

void QubicleBinary::RenderWithAnimator(MS3DAnimator** pSkeleton, VoxelCharacter* pVoxelCharacter, bool renderOutline, bool reflection, bool silhouette, Color OutlineColor, bool subSelectionNamePicking)
{
	if (pVoxelCharacter == nullptr)
	{
		return;
	}

	m_pRenderer->PushMatrix();
	m_pRenderer->StartMeshRender();

	for (unsigned int i = 0; i < m_numMatrices; ++i)
	{
		if (m_vpMatrices[i]->isRemoved == true)
		{
			continue;
		}

		if (subSelectionNamePicking)
		{
			m_pRenderer->LoadNameOntoStack(SUBSELECTION_NAMEPICKING_OFFSET + i);
		}

		m_pRenderer->PushMatrix();

		MS3DAnimator* pSkeletonToUse = pSkeleton[AnimationSections::FullBody];
		if (m_vpMatrices[i]->boneIndex == pVoxelCharacter->GetHeadBoneIndex() ||
			m_vpMatrices[i]->boneIndex == pVoxelCharacter->GetBodyBoneIndex())
		{
			pSkeletonToUse = pSkeleton[AnimationSections::HeadBody];

			m_pRenderer->TranslateWorldMatrix(0.0f, 0.0f, -pVoxelCharacter->GetHeadAndUpperBodyLookzTranslate());
			m_pRenderer->RotateWorldMatrix(pVoxelCharacter->GetHeadAndUpperBodyLookRotation() * 0.65f, 0.0f, 0.0f);
			m_pRenderer->TranslateWorldMatrix(0.0f, 0.0f, pVoxelCharacter->GetHeadAndUpperBodyLookzTranslate());
		}
		else if (m_vpMatrices[i]->boneIndex == pVoxelCharacter->GetLeftShoulderBoneIndex() ||
			m_vpMatrices[i]->boneIndex == pVoxelCharacter->GetLeftHandBoneIndex())
		{
			pSkeletonToUse = pSkeleton[AnimationSections::LeftArmHand];

			m_pRenderer->RotateWorldMatrix(pVoxelCharacter->GetHeadAndUpperBodyLookRotation(), 0.0f, 0.0f);
		}
		else if (m_vpMatrices[i]->boneIndex == pVoxelCharacter->GetRightShoulderBoneIndex() ||
			m_vpMatrices[i]->boneIndex == pVoxelCharacter->GetRightHandBoneIndex())
		{
			pSkeletonToUse = pSkeleton[AnimationSections::RightArmHand];

			m_pRenderer->RotateWorldMatrix(pVoxelCharacter->GetHeadAndUpperBodyLookRotation(), 0.0f, 0.0f);
		}
		else if (m_vpMatrices[i]->boneIndex == pVoxelCharacter->GetLegsBoneIndex() ||
			m_vpMatrices[i]->boneIndex == pVoxelCharacter->GetRightFootBoneIndex() ||
			m_vpMatrices[i]->boneIndex == pVoxelCharacter->GetLeftFootBoneIndex())
		{
			pSkeletonToUse = pSkeleton[AnimationSections::LegsFeet];

			m_pRenderer->TranslateWorldMatrix(0.0f, 0.0f, -pVoxelCharacter->GetHeadAndUpperBodyLookzTranslate());
			m_pRenderer->RotateWorldMatrix(pVoxelCharacter->GetHeadAndUpperBodyLookRotation() * 0.3f, 0.0f, 0.0f);
			m_pRenderer->TranslateWorldMatrix(0.0f, 0.0f, pVoxelCharacter->GetHeadAndUpperBodyLookzTranslate());
		}

		m_pRenderer->PushMatrix();

		// Breathing animation
		if (pVoxelCharacter->IsBreathingAnimationStarted())
		{
			float offsetAmount = 0.0f;
			if (m_vpMatrices[i]->boneIndex != -1)
			{
				offsetAmount = pVoxelCharacter->GetBreathingAnimationOffsetForBone(m_vpMatrices[i]->boneIndex);
			}

			m_pRenderer->TranslateWorldMatrix(0.0f, offsetAmount, 0.0f);
		}

		// Body and hands/shoulders looking direction
		if (m_vpMatrices[i]->boneIndex != -1)
		{
			if (m_vpMatrices[i]->boneIndex == pVoxelCharacter->GetBodyBoneIndex() ||
				m_vpMatrices[i]->boneIndex == pVoxelCharacter->GetLeftShoulderBoneIndex() ||
				m_vpMatrices[i]->boneIndex == pVoxelCharacter->GetLeftHandBoneIndex() ||
				m_vpMatrices[i]->boneIndex == pVoxelCharacter->GetRightShoulderBoneIndex() ||
				m_vpMatrices[i]->boneIndex == pVoxelCharacter->GetRightHandBoneIndex())
			{
				glm::vec3 vForward = normalize(pVoxelCharacter->GetFaceLookingDirection());
				vForward.y = 0.0f;
				vForward = normalize(vForward);
				glm::vec3 forwardDiff = vForward - glm::vec3(0.0f, 0.0f, 1.0f);
				vForward = normalize(glm::vec3(0.0f, 0.0f, 1.0f) + (forwardDiff * 0.5f));

				glm::vec3 vUp = glm::vec3(0.0f, 1.0f, 0.0f);
				glm::vec3 vRight = normalize(cross(vUp, vForward));
				vUp = normalize(cross(vForward, vRight));

				float matrix[16] =
				{
					vRight.x, vRight.y, vRight.z, 0.0f,
					vUp.x, vUp.y, vUp.z, 0.0f,
					vForward.x, vForward.y, vForward.z, 0.0f,
					0.0f, 0.0f, 0.0f, 1.0f
				};
				Matrix4 lookingMat;
				lookingMat.SetValues(matrix);
				m_pRenderer->MultiplyWorldMatrix(lookingMat);
			}
		}

		// Translate by attached bone matrix
		if (m_vpMatrices[i]->boneIndex != -1)
		{
			Matrix4 boneMatrix = pSkeletonToUse->GetBoneMatrix(m_vpMatrices[i]->boneIndex);
			glm::vec3 boneScale = pVoxelCharacter->GetBoneScale();
			m_pRenderer->ScaleWorldMatrix(boneScale.x, boneScale.y, boneScale.z);
			m_pRenderer->MultiplyWorldMatrix(boneMatrix);
			m_pRenderer->ScaleWorldMatrix(1.0f / boneScale.x, 1.0f / boneScale.y, 1.0f / boneScale.z);
		}

		// Rotation due to 3DS Max export affecting the bone rotations
		m_pRenderer->RotateWorldMatrix(0.0f, 0.0f, -90.0f);

		// Face looking direction
		if (m_vpMatrices[i]->boneIndex != -1)
		{
			if (m_vpMatrices[i]->boneIndex == pVoxelCharacter->GetHeadBoneIndex())
			{
				glm::vec3 vForward = normalize(pVoxelCharacter->GetFaceLookingDirection());
				glm::vec3 vUp = glm::vec3(0.0f, 1.0f, 0.0f);
				glm::vec3 vRight = normalize(cross(vUp, vForward));
				vUp = normalize(cross(vForward, vRight));

				float lMatrix[16] =
				{
					vRight.x, vRight.y, vRight.z, 0.0f,
					vUp.x, vUp.y, vUp.z, 0.0f,
					vForward.x, vForward.y, vForward.z, 0.0f,
					0.0f, 0.0f, 0.0f, 1.0f
				};
				Matrix4 lookingMat;
				lookingMat.SetValues(lMatrix);
				m_pRenderer->MultiplyWorldMatrix(lookingMat);
			}
		}

		// Scale for external matrix scale value
		m_pRenderer->ScaleWorldMatrix(m_vpMatrices[i]->scale, m_vpMatrices[i]->scale, m_vpMatrices[i]->scale);

		// Translate for initial block offset
		m_pRenderer->TranslateWorldMatrix(0.5f, 0.5f, 0.5f);

		// Translate to center of model
		m_pRenderer->TranslateWorldMatrix(-m_vpMatrices[i]->matrixSizeX * 0.5f, -m_vpMatrices[i]->matrixSizeY * 0.5f, -m_vpMatrices[i]->matrixSizeZ * 0.5f);

		// Translate for external matrix offset value
		m_pRenderer->TranslateWorldMatrix(m_vpMatrices[i]->offsetX, m_vpMatrices[i]->offsetY, m_vpMatrices[i]->offsetZ);

		// Store cull mode
		CullMode cullMode = m_pRenderer->GetCullMode();

		if (renderOutline)
		{
			m_pRenderer->DisableDepthTest();
			m_pRenderer->SetLineWidth(3.0f);
			m_pRenderer->SetCullMode(CullMode::FRONT);
			m_pRenderer->SetRenderMode(RenderMode::WIREFRAME);
			m_pRenderer->ImmediateColorAlpha(OutlineColor.GetRed(), OutlineColor.GetGreen(), OutlineColor.GetBlue(), OutlineColor.GetAlpha());
		}
		else if (silhouette)
		{
			m_pRenderer->DisableDepthTest();
			m_pRenderer->SetCullMode(CullMode::FRONT);
			m_pRenderer->SetRenderMode(RenderMode::SOLID);
			m_pRenderer->ImmediateColorAlpha(OutlineColor.GetRed(), OutlineColor.GetGreen(), OutlineColor.GetBlue(), OutlineColor.GetAlpha());
		}
		else if (m_isRenderWireFrame)
		{
			m_pRenderer->SetLineWidth(1.0f);
			m_pRenderer->SetRenderMode(RenderMode::WIREFRAME);
			m_pRenderer->SetCullMode(CullMode::NOCULL);
		}
		else
		{
			m_pRenderer->SetRenderMode(RenderMode::SOLID);
		}

		// Store the model matrix
		if (reflection == false)
		{
			m_pRenderer->GetModelMatrix(&m_vpMatrices[i]->modelMatrix);
		}

		// Texture manipulation (for shadow rendering)
		Matrix4 worldMatrix;
		m_pRenderer->GetModelMatrix(&worldMatrix);

		m_pRenderer->PushTextureMatrix();
		m_pRenderer->MultiplyWorldMatrix(worldMatrix);

		if (m_meshAlpha < 1.0f)
		{
			m_pRenderer->EnableTransparency(BlendFunction::SRC_ALPHA, BlendFunction::ONE_MINUS_SRC_ALPHA);
		}
		m_pRenderer->EnableMaterial(m_materialID);

		if (renderOutline || silhouette)
		{
			m_pRenderer->EndMeshRender();
			m_pRenderer->RenderMeshNoColor(m_vpMatrices[i]->pMesh);
		}
		else
		{
			m_pRenderer->MeshStaticBufferRender(m_vpMatrices[i]->pMesh);
		}

		if (m_meshAlpha < 1.0f)
		{
			m_pRenderer->DisableTransparency();
		}

		// Texture manipulation (for shadow rendering)
		m_pRenderer->PopTextureMatrix();

		// Restore cull mode
		m_pRenderer->SetCullMode(cullMode);

		if (renderOutline || silhouette)
		{
			m_pRenderer->EnableDepthTest(DepthTest::LESS);
		}
		m_pRenderer->PopMatrix();

		if (subSelectionNamePicking)
		{
			m_pRenderer->EndNameStack();
		}
		m_pRenderer->PopMatrix();
	}

	m_pRenderer->EndMeshRender();
	m_pRenderer->PopMatrix();
}

void QubicleBinary::RenderSingleMatrix(MS3DAnimator** pSkeleton, VoxelCharacter* pVoxelCharacter, std::string matrixName, bool renderOutline, bool silhouette, Color OutlineColor)
{
	if (pVoxelCharacter == nullptr)
	{
		return;
	}

	m_pRenderer->PushMatrix();
	m_pRenderer->StartMeshRender();

	int matrixIndex = GetMatrixIndexForName(matrixName.c_str());

	if (matrixIndex != -1 && m_vpMatrices[matrixIndex]->isRemoved == false)
	{
		m_pRenderer->PushMatrix();
		MS3DAnimator* pSkeletonToUse = pSkeleton[AnimationSections::FullBody];
		if (m_vpMatrices[matrixIndex]->boneIndex == pVoxelCharacter->GetHeadBoneIndex() ||
			m_vpMatrices[matrixIndex]->boneIndex == pVoxelCharacter->GetBodyBoneIndex())
		{
			pSkeletonToUse = pSkeleton[AnimationSections::HeadBody];

			m_pRenderer->TranslateWorldMatrix(0.0f, 0.0f, -pVoxelCharacter->GetHeadAndUpperBodyLookzTranslate());
			m_pRenderer->RotateWorldMatrix(pVoxelCharacter->GetHeadAndUpperBodyLookRotation() * 0.65f, 0.0f, 0.0f);
			m_pRenderer->TranslateWorldMatrix(0.0f, 0.0f, pVoxelCharacter->GetHeadAndUpperBodyLookzTranslate());
		}
		else if (m_vpMatrices[matrixIndex]->boneIndex == pVoxelCharacter->GetLeftShoulderBoneIndex() ||
			m_vpMatrices[matrixIndex]->boneIndex == pVoxelCharacter->GetLeftHandBoneIndex())
		{
			pSkeletonToUse = pSkeleton[AnimationSections::LeftArmHand];

			m_pRenderer->RotateWorldMatrix(pVoxelCharacter->GetHeadAndUpperBodyLookRotation(), 0.0f, 0.0f);
		}
		else if (m_vpMatrices[matrixIndex]->boneIndex == pVoxelCharacter->GetRightShoulderBoneIndex() ||
			m_vpMatrices[matrixIndex]->boneIndex == pVoxelCharacter->GetRightHandBoneIndex())
		{
			pSkeletonToUse = pSkeleton[AnimationSections::RightArmHand];

			m_pRenderer->RotateWorldMatrix(pVoxelCharacter->GetHeadAndUpperBodyLookRotation(), 0.0f, 0.0f);
		}
		else if (m_vpMatrices[matrixIndex]->boneIndex == pVoxelCharacter->GetLegsBoneIndex() ||
			m_vpMatrices[matrixIndex]->boneIndex == pVoxelCharacter->GetRightFootBoneIndex() ||
			m_vpMatrices[matrixIndex]->boneIndex == pVoxelCharacter->GetLeftFootBoneIndex())
		{
			pSkeletonToUse = pSkeleton[AnimationSections::LegsFeet];

			m_pRenderer->TranslateWorldMatrix(0.0f, 0.0f, -pVoxelCharacter->GetHeadAndUpperBodyLookzTranslate());
			m_pRenderer->RotateWorldMatrix(pVoxelCharacter->GetHeadAndUpperBodyLookRotation() * 0.3f, 0.0f, 0.0f);
			m_pRenderer->TranslateWorldMatrix(0.0f, 0.0f, pVoxelCharacter->GetHeadAndUpperBodyLookzTranslate());
		}

		// Breathing animation
		if (pVoxelCharacter->IsBreathingAnimationStarted())
		{
			float offsetAmount = 0.0f;
			if (m_vpMatrices[matrixIndex]->boneIndex != -1)
			{
				offsetAmount = pVoxelCharacter->GetBreathingAnimationOffsetForBone(m_vpMatrices[matrixIndex]->boneIndex);
			}

			m_pRenderer->TranslateWorldMatrix(0.0f, offsetAmount, 0.0f);
		}

		// Body and hands/shoulders looking direction
		if (m_vpMatrices[matrixIndex]->boneIndex != -1)
		{
			if (m_vpMatrices[matrixIndex]->boneIndex == pVoxelCharacter->GetBodyBoneIndex() ||
				m_vpMatrices[matrixIndex]->boneIndex == pVoxelCharacter->GetLeftShoulderBoneIndex() ||
				m_vpMatrices[matrixIndex]->boneIndex == pVoxelCharacter->GetLeftHandBoneIndex() ||
				m_vpMatrices[matrixIndex]->boneIndex == pVoxelCharacter->GetRightShoulderBoneIndex() ||
				m_vpMatrices[matrixIndex]->boneIndex == pVoxelCharacter->GetRightHandBoneIndex())
			{
				glm::vec3 vForward = normalize(pVoxelCharacter->GetFaceLookingDirection());
				vForward.y = 0.0f;
				vForward = normalize(vForward);
				glm::vec3 forwardDiff = vForward - glm::vec3(0.0f, 0.0f, 1.0f);
				vForward = normalize(glm::vec3(0.0f, 0.0f, 1.0f) + (forwardDiff * 0.5f));

				glm::vec3 vUp = glm::vec3(0.0f, 1.0f, 0.0f);
				glm::vec3 vRight = normalize(cross(vUp, vForward));
				vUp = normalize(cross(vForward, vRight));

				float matrix[16] =
				{
					vRight.x, vRight.y, vRight.z, 0.0f,
					vUp.x, vUp.y, vUp.z, 0.0f,
					vForward.x, vForward.y, vForward.z, 0.0f,
					0.0f, 0.0f, 0.0f, 1.0f
				};
				Matrix4 lookingMat;
				lookingMat.SetValues(matrix);
				m_pRenderer->MultiplyWorldMatrix(lookingMat);
			}
		}

		// Translate by attached bone matrix
		if (m_vpMatrices[matrixIndex]->boneIndex != -1)
		{
			Matrix4 boneMatrix = pSkeletonToUse->GetBoneMatrix(m_vpMatrices[matrixIndex]->boneIndex);
			glm::vec3 boneScale = pVoxelCharacter->GetBoneScale();
			m_pRenderer->ScaleWorldMatrix(boneScale.x, boneScale.y, boneScale.z);
			m_pRenderer->MultiplyWorldMatrix(boneMatrix);
			m_pRenderer->ScaleWorldMatrix(1.0f / boneScale.x, 1.0f / boneScale.y, 1.0f / boneScale.z);

			// Rotation due to 3DS Max export affecting the bone rotations
			m_pRenderer->RotateWorldMatrix(0.0f, 0.0f, -90.0f);
		}

		// Face looking direction
		if (m_vpMatrices[matrixIndex]->boneIndex != -1)
		{
			if (m_vpMatrices[matrixIndex]->boneIndex == pVoxelCharacter->GetHeadBoneIndex())
			{
				glm::vec3 vForward = normalize(pVoxelCharacter->GetFaceLookingDirection());
				glm::vec3 vUp = glm::vec3(0.0f, 1.0f, 0.0f);
				glm::vec3 vRight = normalize(cross(vUp, vForward));
				vUp = normalize(cross(vForward, vRight));

				float matrix[16] =
				{
					vRight.x, vRight.y, vRight.z, 0.0f,
					vUp.x, vUp.y, vUp.z, 0.0f,
					vForward.x, vForward.y, vForward.z, 0.0f,
					0.0f, 0.0f, 0.0f, 1.0f
				};
				Matrix4 lookingMat;
				lookingMat.SetValues(matrix);
				m_pRenderer->MultiplyWorldMatrix(lookingMat);
			}
		}

		// Scale for external matrix scale value
		m_pRenderer->ScaleWorldMatrix(m_vpMatrices[matrixIndex]->scale, m_vpMatrices[matrixIndex]->scale, m_vpMatrices[matrixIndex]->scale);

		// Translate for initial block offset
		m_pRenderer->TranslateWorldMatrix(0.5f, 0.5f, 0.5f);

		// Translate to center of model
		m_pRenderer->TranslateWorldMatrix(-m_vpMatrices[matrixIndex]->matrixSizeX * 0.5f, -m_vpMatrices[matrixIndex]->matrixSizeY * 0.5f, -m_vpMatrices[matrixIndex]->matrixSizeZ * 0.5f);

		// Translate for external matrix offset value
		m_pRenderer->TranslateWorldMatrix(m_vpMatrices[matrixIndex]->offsetX, m_vpMatrices[matrixIndex]->offsetY, m_vpMatrices[matrixIndex]->offsetZ);

		// Store cull mode
		CullMode cullMode = m_pRenderer->GetCullMode();

		if (renderOutline)
		{
			m_pRenderer->DisableDepthTest();
			m_pRenderer->SetLineWidth(3.0f);
			m_pRenderer->SetCullMode(CullMode::FRONT);
			m_pRenderer->SetRenderMode(RenderMode::WIREFRAME);
			m_pRenderer->ImmediateColorAlpha(OutlineColor.GetRed(), OutlineColor.GetGreen(), OutlineColor.GetBlue(), OutlineColor.GetAlpha());
		}
		else if (silhouette)
		{
			m_pRenderer->DisableDepthTest();
			m_pRenderer->SetCullMode(CullMode::FRONT);
			m_pRenderer->SetRenderMode(RenderMode::SOLID);
			m_pRenderer->ImmediateColorAlpha(OutlineColor.GetRed(), OutlineColor.GetGreen(), OutlineColor.GetBlue(), OutlineColor.GetAlpha());
		}
		else if (m_isRenderWireFrame)
		{
			m_pRenderer->SetLineWidth(1.0f);
			m_pRenderer->SetRenderMode(RenderMode::WIREFRAME);
			m_pRenderer->SetCullMode(CullMode::NOCULL);
		}
		else
		{
			m_pRenderer->SetRenderMode(RenderMode::SOLID);
		}

		// Texture manipulation (for shadow rendering)
		Matrix4 worldMatrix;
		m_pRenderer->GetModelMatrix(&worldMatrix);

		m_pRenderer->PushTextureMatrix();
		m_pRenderer->MultiplyWorldMatrix(worldMatrix);

		if (m_meshAlpha < 1.0f)
		{
			m_pRenderer->EnableTransparency(BlendFunction::SRC_ALPHA, BlendFunction::ONE_MINUS_SRC_ALPHA);
		}
		m_pRenderer->EnableMaterial(m_materialID);

		if (renderOutline || silhouette)
		{
			m_pRenderer->EndMeshRender();
			m_pRenderer->RenderMeshNoColor(m_vpMatrices[matrixIndex]->pMesh);
		}
		else
		{
			m_pRenderer->MeshStaticBufferRender(m_vpMatrices[matrixIndex]->pMesh);
		}

		if (m_meshAlpha < 1.0f)
		{
			m_pRenderer->DisableTransparency();
		}

		// Texture manipulation (for shadow rendering)
		m_pRenderer->PopTextureMatrix();

		// Restore cull mode
		m_pRenderer->SetCullMode(cullMode);

		if (renderOutline || silhouette)
		{
			m_pRenderer->EnableDepthTest(DepthTest::LESS);
		}
		m_pRenderer->PopMatrix();
	}

	m_pRenderer->EndMeshRender();
	m_pRenderer->PopMatrix();
}

void QubicleBinary::RenderFace(MS3DAnimator* pSkeleton, VoxelCharacter* pVoxelCharacter, bool transparency, bool useScale, bool useTranslate)
{
	if (pVoxelCharacter == nullptr)
	{
		return;
	}

	// Render eyes
	int eyesBoneIndex = pVoxelCharacter->GetEyesBone();
	glm::vec3 eyeOffset = pVoxelCharacter->GetEyesOffset();

	if (eyesBoneIndex != -1)
	{
		m_pRenderer->PushMatrix();
		m_pRenderer->EnableMaterial(m_materialID);

		if (useScale)
		{
			m_pRenderer->TranslateWorldMatrix(0.0f, 0.0f, -pVoxelCharacter->GetHeadAndUpperBodyLookzTranslate());
			m_pRenderer->RotateWorldMatrix(pVoxelCharacter->GetHeadAndUpperBodyLookRotation() * 0.65f, 0.0f, 0.0f);
			m_pRenderer->TranslateWorldMatrix(0.0f, 0.0f, pVoxelCharacter->GetHeadAndUpperBodyLookzTranslate());

			// Breathing animation
			if (pVoxelCharacter->IsBreathingAnimationStarted())
			{
				float offsetAmount = 0.0f;
				if (eyesBoneIndex != -1)
				{
					offsetAmount = pVoxelCharacter->GetBreathingAnimationOffsetForBone(eyesBoneIndex);
				}

				m_pRenderer->TranslateWorldMatrix(0.0f, offsetAmount, 0.0f);
			}
		}

		// Translate by attached bone matrix
		Matrix4 boneMatrix = pSkeleton->GetBoneMatrix(eyesBoneIndex);
		glm::vec3 boneScale = pVoxelCharacter->GetBoneScale();
		m_pRenderer->ScaleWorldMatrix(boneScale.x, boneScale.y, boneScale.z);
		m_pRenderer->MultiplyWorldMatrix(boneMatrix);
		m_pRenderer->ScaleWorldMatrix(1.0f / boneScale.x, 1.0f / boneScale.y, 1.0f / boneScale.z);

		// Rotation due to 3DS Max export affecting the bone rotations
		m_pRenderer->RotateWorldMatrix(0.0f, 0.0f, -90.0f);

		// Face looking direction
		glm::vec3 vForward = normalize(pVoxelCharacter->GetFaceLookingDirection());
		glm::vec3 vUp = glm::vec3(0.0f, 1.0f, 0.0f);
		glm::vec3 vRight = normalize(cross(vUp, vForward));
		vUp = normalize(cross(vForward, vRight));

		float matrix[16] =
		{
			vRight.x, vRight.y, vRight.z, 0.0f,
			vUp.x, vUp.y, vUp.z, 0.0f,
			vForward.x, vForward.y, vForward.z, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f
		};
		Matrix4 lookingMat;
		lookingMat.SetValues(matrix);
		m_pRenderer->MultiplyWorldMatrix(lookingMat);

		int eyesMatrixIndex = pVoxelCharacter->GetEyesMatrixIndex();

		if (useScale)
		{
			// Scale for external matrix scale value
			m_pRenderer->ScaleWorldMatrix(m_vpMatrices[eyesMatrixIndex]->scale, m_vpMatrices[eyesMatrixIndex]->scale, m_vpMatrices[eyesMatrixIndex]->scale);
		}
		if (useTranslate)
		{
			// Translate for external matrix offset value
			m_pRenderer->TranslateWorldMatrix(m_vpMatrices[eyesMatrixIndex]->offsetX, m_vpMatrices[eyesMatrixIndex]->offsetY, m_vpMatrices[eyesMatrixIndex]->offsetZ);
		}

		m_pRenderer->TranslateWorldMatrix(eyeOffset.x, eyeOffset.y, eyeOffset.z);
		pVoxelCharacter->RenderFaceTextures(true, m_isRenderWireFrame, transparency);
		m_pRenderer->PopMatrix();
	}
	
	// Render mouth
	int mouthBoneIndex = pVoxelCharacter->GetMouthBone();
	glm::vec3 mouthOffset = pVoxelCharacter->GetMouthOffset();

	m_pRenderer->PushMatrix();
	m_pRenderer->EnableMaterial(m_materialID);

	if (useScale)
	{
		m_pRenderer->TranslateWorldMatrix(0.0f, 0.0f, -pVoxelCharacter->GetHeadAndUpperBodyLookzTranslate());
		m_pRenderer->RotateWorldMatrix(pVoxelCharacter->GetHeadAndUpperBodyLookRotation() * 0.65f, 0.0f, 0.0f);
		m_pRenderer->TranslateWorldMatrix(0.0f, 0.0f, pVoxelCharacter->GetHeadAndUpperBodyLookzTranslate());

		// Breathing animation
		if (pVoxelCharacter->IsBreathingAnimationStarted())
		{
			float offsetAmount = 0.0f;
			if (mouthBoneIndex != -1)
			{
				offsetAmount = pVoxelCharacter->GetBreathingAnimationOffsetForBone(mouthBoneIndex);
			}

			m_pRenderer->TranslateWorldMatrix(0.0f, offsetAmount, 0.0f);
		}
	}

	// Translate by attached bone matrix
	Matrix4 boneMatrix = pSkeleton->GetBoneMatrix(mouthBoneIndex);
	glm::vec3 boneScale = pVoxelCharacter->GetBoneScale();
	m_pRenderer->ScaleWorldMatrix(boneScale.x, boneScale.y, boneScale.z);
	m_pRenderer->MultiplyWorldMatrix(boneMatrix);
	m_pRenderer->ScaleWorldMatrix(1.0f / boneScale.x, 1.0f / boneScale.y, 1.0f / boneScale.z);

	// Rotation due to 3DS Max export affecting the bone rotations
	m_pRenderer->RotateWorldMatrix(0.0f, 0.0f, -90.0f);

	// Face looking direction
	glm::vec3 vForward = normalize(pVoxelCharacter->GetFaceLookingDirection());
	glm::vec3 vUp = glm::vec3(0.0f, 1.0f, 0.0f);
	glm::vec3 vRight = normalize(cross(vUp, vForward));
	vUp = normalize(cross(vForward, vRight));

	float matrix[16] =
	{
		vRight.x, vRight.y, vRight.z, 0.0f,
		vUp.x, vUp.y, vUp.z, 0.0f,
		vForward.x, vForward.y, vForward.z, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	};
	Matrix4 lookingMat;
	lookingMat.SetValues(matrix);
	m_pRenderer->MultiplyWorldMatrix(lookingMat);

	int mouthMatrixIndex = pVoxelCharacter->GetMouthMatrixIndex();

	if (useScale)
	{
		// Scale for external matrix scale value
		m_pRenderer->ScaleWorldMatrix(m_vpMatrices[mouthMatrixIndex]->scale, m_vpMatrices[mouthMatrixIndex]->scale, m_vpMatrices[mouthMatrixIndex]->scale);
	}
	if (useTranslate)
	{
		// Translate for external matrix offset value
		m_pRenderer->TranslateWorldMatrix(m_vpMatrices[mouthMatrixIndex]->offsetX, m_vpMatrices[mouthMatrixIndex]->offsetY, m_vpMatrices[mouthMatrixIndex]->offsetZ);
	}

	m_pRenderer->TranslateWorldMatrix(mouthOffset.x, mouthOffset.y, mouthOffset.z);
	pVoxelCharacter->RenderFaceTextures(false, m_isRenderWireFrame, transparency);
	m_pRenderer->PopMatrix();
}

void QubicleBinary::RenderPaperdoll(MS3DAnimator* pSkeletonLeft, MS3DAnimator* pSkeletonRight, VoxelCharacter* pVoxelCharacter)
{
	if (pVoxelCharacter == nullptr)
	{
		return;
	}

	m_pRenderer->PushMatrix();
	m_pRenderer->StartMeshRender();

	for (unsigned int i = 0; i < m_numMatrices; i++)
	{
		if (m_vpMatrices[i]->isRemoved == true)
		{
			continue;
		}

		MS3DAnimator* pSkeletonToUse = nullptr;

		if (m_vpMatrices[i]->boneIndex == pVoxelCharacter->GetLeftShoulderBoneIndex() ||
			m_vpMatrices[i]->boneIndex == pVoxelCharacter->GetLeftHandBoneIndex())
		{
			pSkeletonToUse = pSkeletonLeft;
		}
		else
		{
			pSkeletonToUse = pSkeletonRight;
		}

		m_pRenderer->PushMatrix();
		// Breathing animation
		if (pVoxelCharacter->IsBreathingAnimationStarted())
		{
			float offsetAmount = 0.0f;
			if (m_vpMatrices[i]->boneIndex != -1)
			{
				offsetAmount = pVoxelCharacter->GetBreathingAnimationOffsetForBone(m_vpMatrices[i]->boneIndex);
			}

			m_pRenderer->TranslateWorldMatrix(0.0f, offsetAmount, 0.0f);
		}

		// Body and hands/shoulders looking direction
		if (m_vpMatrices[i]->boneIndex != -1)
		{
			if (m_vpMatrices[i]->boneIndex == pVoxelCharacter->GetBodyBoneIndex() ||
				m_vpMatrices[i]->boneIndex == pVoxelCharacter->GetLeftShoulderBoneIndex() ||
				m_vpMatrices[i]->boneIndex == pVoxelCharacter->GetLeftHandBoneIndex() ||
				m_vpMatrices[i]->boneIndex == pVoxelCharacter->GetRightShoulderBoneIndex() ||
				m_vpMatrices[i]->boneIndex == pVoxelCharacter->GetRightHandBoneIndex())
			{
				glm::vec3 vForward = normalize(pVoxelCharacter->GetFaceLookingDirection());
				vForward.y = 0.0f;
				vForward = normalize(vForward);
				glm::vec3 forwardDiff = vForward - glm::vec3(0.0f, 0.0f, 1.0f);
				vForward = normalize(glm::vec3(0.0f, 0.0f, 1.0f) + (forwardDiff * 0.5f));

				glm::vec3 vUp = glm::vec3(0.0f, 1.0f, 0.0f);
				glm::vec3 vRight = normalize(cross(vUp, vForward));
				vUp = normalize(cross(vForward, vRight));

				float matrix[16] =
				{
					vRight.x, vRight.y, vRight.z, 0.0f,
					vUp.x, vUp.y, vUp.z, 0.0f,
					vForward.x, vForward.y, vForward.z, 0.0f,
					0.0f, 0.0f, 0.0f, 1.0f
				};
				Matrix4 lookingMat;
				lookingMat.SetValues(matrix);
				m_pRenderer->MultiplyWorldMatrix(lookingMat);
			}
		}

		// Translate by attached bone matrix
		if (m_vpMatrices[i]->boneIndex != -1)
		{
			Matrix4 boneMatrix = pSkeletonToUse->GetBoneMatrix(m_vpMatrices[i]->boneIndex);
			glm::vec3 boneScale = pVoxelCharacter->GetBoneScale();
			m_pRenderer->ScaleWorldMatrix(boneScale.x, boneScale.y, boneScale.z);
			m_pRenderer->MultiplyWorldMatrix(boneMatrix);
			m_pRenderer->ScaleWorldMatrix(1.0f / boneScale.x, 1.0f / boneScale.y, 1.0f / boneScale.z);

			// Rotation due to 3DS Max export affecting the bone rotations
			m_pRenderer->RotateWorldMatrix(0.0f, 0.0f, -90.0f);
		}

		// Face looking direction
		if (m_vpMatrices[i]->boneIndex != -1)
		{
			if (m_vpMatrices[i]->boneIndex == pVoxelCharacter->GetHeadBoneIndex())
			{
				glm::vec3 vForward = normalize(pVoxelCharacter->GetFaceLookingDirection());
				glm::vec3 vUp = glm::vec3(0.0f, 1.0f, 0.0f);
				glm::vec3 vRight = normalize(cross(vUp, vForward));
				vUp = normalize(cross(vForward, vRight));

				float matrix[16] =
				{
					vRight.x, vRight.y, vRight.z, 0.0f,
					vUp.x, vUp.y, vUp.z, 0.0f,
					vForward.x, vForward.y, vForward.z, 0.0f,
					0.0f, 0.0f, 0.0f, 1.0f
				};
				Matrix4 lookingMat;
				lookingMat.SetValues(matrix);
				m_pRenderer->MultiplyWorldMatrix(lookingMat);
			}
		}

		// Scale for external matrix scale value
		m_pRenderer->ScaleWorldMatrix(m_vpMatrices[i]->scale, m_vpMatrices[i]->scale, m_vpMatrices[i]->scale);

		// Translate for initial block offset
		m_pRenderer->TranslateWorldMatrix(0.5f, 0.5f, 0.5f);

		// Translate to center of model
		m_pRenderer->TranslateWorldMatrix(-m_vpMatrices[i]->matrixSizeX * 0.5f, -m_vpMatrices[i]->matrixSizeY * 0.5f, -m_vpMatrices[i]->matrixSizeZ * 0.5f);

		// Translate for external matrix offset value
		m_pRenderer->TranslateWorldMatrix(m_vpMatrices[i]->offsetX, m_vpMatrices[i]->offsetY, m_vpMatrices[i]->offsetZ);

		if (m_isRenderWireFrame)
		{
			m_pRenderer->SetLineWidth(1.0f);
			m_pRenderer->SetRenderMode(RenderMode::WIREFRAME);
			m_pRenderer->SetCullMode(CullMode::NOCULL);
		}
		else
		{
			m_pRenderer->SetRenderMode(RenderMode::SOLID);
		}

		// Texture manipulation (for shadow rendering)
		Matrix4 worldMatrix;
		m_pRenderer->GetModelMatrix(&worldMatrix);

		m_pRenderer->PushTextureMatrix();
		m_pRenderer->MultiplyWorldMatrix(worldMatrix);

		m_pRenderer->EnableMaterial(m_materialID);

		m_pRenderer->MeshStaticBufferRender(m_vpMatrices[i]->pMesh);

		// Texture manipulation (for shadow rendering)
		m_pRenderer->PopTextureMatrix();
		m_pRenderer->PopMatrix();
	}

	m_pRenderer->EndMeshRender();
	m_pRenderer->PopMatrix();
}

void QubicleBinary::RenderPortrait(MS3DAnimator* pSkeleton, VoxelCharacter* pVoxelCharacter, std::string matrixName)
{
	if (pVoxelCharacter == nullptr)
	{
		return;
	}

	m_pRenderer->PushMatrix();
	m_pRenderer->StartMeshRender();

	int matrixIndex = GetMatrixIndexForName(matrixName.c_str());

	if (matrixIndex != -1 && m_vpMatrices[matrixIndex]->isRemoved == false)
	{
		MS3DAnimator* pSkeletonToUse = pSkeleton;

		m_pRenderer->PushMatrix();

		// Body and hands/shoulders looking direction
		if (m_vpMatrices[matrixIndex]->boneIndex != -1)
		{
			if (m_vpMatrices[matrixIndex]->boneIndex == pVoxelCharacter->GetBodyBoneIndex() ||
				m_vpMatrices[matrixIndex]->boneIndex == pVoxelCharacter->GetLeftShoulderBoneIndex() ||
				m_vpMatrices[matrixIndex]->boneIndex == pVoxelCharacter->GetLeftHandBoneIndex() ||
				m_vpMatrices[matrixIndex]->boneIndex == pVoxelCharacter->GetRightShoulderBoneIndex() ||
				m_vpMatrices[matrixIndex]->boneIndex == pVoxelCharacter->GetRightHandBoneIndex())
			{
				glm::vec3 vForward = normalize(pVoxelCharacter->GetFaceLookingDirection());
				vForward.y = 0.0f;
				vForward = normalize(vForward);
				glm::vec3 forwardDiff = vForward - glm::vec3(0.0f, 0.0f, 1.0f);
				vForward = normalize(glm::vec3(0.0f, 0.0f, 1.0f) + (forwardDiff * 0.5f));

				glm::vec3 vUp = glm::vec3(0.0f, 1.0f, 0.0f);
				glm::vec3 vRight = normalize(cross(vUp, vForward));
				vUp = normalize(cross(vForward, vRight));

				float matrix[16] =
				{
					vRight.x, vRight.y, vRight.z, 0.0f,
					vUp.x, vUp.y, vUp.z, 0.0f,
					vForward.x, vForward.y, vForward.z, 0.0f,
					0.0f, 0.0f, 0.0f, 1.0f
				};
				Matrix4 lookingMat;
				lookingMat.SetValues(matrix);
				m_pRenderer->MultiplyWorldMatrix(lookingMat);
			}
		}

		// Translate by attached bone matrix
		if (m_vpMatrices[matrixIndex]->boneIndex != -1)
		{
			Matrix4 boneMatrix = pSkeletonToUse->GetBoneMatrix(m_vpMatrices[matrixIndex]->boneIndex);
			glm::vec3 boneScale = pVoxelCharacter->GetBoneScale();
			m_pRenderer->ScaleWorldMatrix(boneScale.x, boneScale.y, boneScale.z);
			m_pRenderer->MultiplyWorldMatrix(boneMatrix);
			m_pRenderer->ScaleWorldMatrix(1.0f / boneScale.x, 1.0f / boneScale.y, 1.0f / boneScale.z);

			// Rotation due to 3DS Max export affecting the bone rotations
			m_pRenderer->RotateWorldMatrix(0.0f, 0.0f, -90.0f);
		}

		// Face looking direction
		if (m_vpMatrices[matrixIndex]->boneIndex != -1)
		{
			if (m_vpMatrices[matrixIndex]->boneIndex == pVoxelCharacter->GetHeadBoneIndex())
			{
				glm::vec3 vForward = normalize(pVoxelCharacter->GetFaceLookingDirection());
				glm::vec3 vUp = glm::vec3(0.0f, 1.0f, 0.0f);
				glm::vec3 vRight = normalize(cross(vUp, vForward));
				vUp = normalize(cross(vForward, vRight));

				float matrix[16] =
				{
					vRight.x, vRight.y, vRight.z, 0.0f,
					vUp.x, vUp.y, vUp.z, 0.0f,
					vForward.x, vForward.y, vForward.z, 0.0f,
					0.0f, 0.0f, 0.0f, 1.0f
				};
				Matrix4 lookingMat;
				lookingMat.SetValues(matrix);
				m_pRenderer->MultiplyWorldMatrix(lookingMat);
			}
		}

		// Scale for external matrix scale value
		if (matrixName == "Helm")
		{
			// NOTE : ONLY scale for portrait when we are rendering the helm, the head should stay a normal size.
			m_pRenderer->ScaleWorldMatrix(m_vpMatrices[matrixIndex]->scale, m_vpMatrices[matrixIndex]->scale, m_vpMatrices[matrixIndex]->scale);
		}

		// Translate for initial block offset
		m_pRenderer->TranslateWorldMatrix(0.5f, 0.5f, 0.5f);

		// Translate to center of model
		m_pRenderer->TranslateWorldMatrix(-m_vpMatrices[matrixIndex]->matrixSizeX * 0.5f, -m_vpMatrices[matrixIndex]->matrixSizeY * 0.5f, -m_vpMatrices[matrixIndex]->matrixSizeZ * 0.5f);

		// Translate for external matrix offset value
		m_pRenderer->TranslateWorldMatrix(m_vpMatrices[matrixIndex]->offsetX, m_vpMatrices[matrixIndex]->offsetY, m_vpMatrices[matrixIndex]->offsetZ);

		if (m_isRenderWireFrame)
		{
			m_pRenderer->SetLineWidth(1.0f);
			m_pRenderer->SetRenderMode(RenderMode::WIREFRAME);
			m_pRenderer->SetCullMode(CullMode::NOCULL);
		}
		else
		{
			m_pRenderer->SetRenderMode(RenderMode::SOLID);
		}

		// Texture manipulation (for shadow rendering)
		Matrix4 worldMatrix;
		m_pRenderer->GetModelMatrix(&worldMatrix);

		m_pRenderer->PushTextureMatrix();
		m_pRenderer->MultiplyWorldMatrix(worldMatrix);

		m_pRenderer->EnableMaterial(m_materialID);

		m_pRenderer->MeshStaticBufferRender(m_vpMatrices[matrixIndex]->pMesh);

		// Texture manipulation (for shadow rendering)
		m_pRenderer->PopTextureMatrix();
		m_pRenderer->PopMatrix();
	}

	m_pRenderer->EndMeshRender();
	m_pRenderer->PopMatrix();
}