/*************************************************************************
> File Name: MS3DModel.h
> Project Name: Cubby
> Author: Chan-Ho Chris Ohk
> Purpose: Milk Shape 3D Model.
> Created Time: 2016/06/27
> Copyright (c) 2016, Chan-Ho Chris Ohk
*************************************************************************/

#include <fstream>

#include "MS3DModel.h"

// Constructor, Destructor
MS3DModel::MS3DModel(Renderer* pRenderer) :
	m_pRenderer(pRenderer),
	m_numVertices(0), m_pVertices(nullptr),
	m_numTriangles(0), m_pTriangles(nullptr),
	m_numMaterials(0), m_pMaterials(nullptr),
	m_numMeshes(0), m_pMeshes(nullptr),
	m_numJoints(0), m_pJoints(nullptr),
	m_animationFPS(0),
	m_isStatic(false), m_staticRenderBuffers(nullptr)
{
	
}

MS3DModel::~MS3DModel()
{
	for (int i = 0; i < m_numMeshes; ++i)
	{
		delete[] m_pMeshes[i].pTriangleIndices;
	}

	for (int i = 0; i < m_numMaterials; ++i)
	{
		delete[] m_pMaterials[i].pTextureFileName;
	}

	m_numMeshes = 0;
	if (m_pMeshes != nullptr)
	{
		delete[] m_pMeshes;
		m_pMeshes = nullptr;
	}

	m_numMaterials = 0;
	if (m_pMaterials != nullptr)
	{
		delete[] m_pMaterials;
		m_pMaterials = nullptr;
	}

	m_numTriangles = 0;
	if (m_pTriangles != nullptr)
	{
		delete[] m_pTriangles;
		m_pTriangles = nullptr;
	}

	m_numVertices = 0;
	if (m_pVertices != nullptr)
	{
		delete[] m_pVertices;
		m_pVertices = nullptr;
	}

	for (int i = 0; i < m_numJoints; ++i)
	{
		delete[] m_pJoints[i].pRotationKeyframes;
		delete[] m_pJoints[i].pTranslationKeyframes;
	}

	m_numJoints = 0;
	if (m_pJoints != nullptr)
	{
		delete[] m_pJoints;
		m_pJoints = nullptr;
	}
}

// Load
bool MS3DModel::LoadModel(const char *modelFileName, bool isStatic)
{
	std::ifstream inputFile;

	// Open the file
	inputFile.open(modelFileName, std::ios::in | std::ios::binary);
	if (inputFile.fail())
	{
		std::cerr << "Couldn't open the model file." << std::endl;
		return false;
	}

	char pathTemp[PATH_MAX + 1];
	int pathLength;
	for (pathLength = strlen(modelFileName); --pathLength;)
	{
		if (modelFileName[pathLength] == '/' || modelFileName[pathLength] == '\\')
		{
			break;
		}
	}

	strncpy(pathTemp, modelFileName, pathLength);

	int i;

	if (pathLength > 0)
	{
		pathTemp[pathLength++] = '/';
	}

	inputFile.seekg(0, std::ios::end);
	long fileSize = static_cast<long>(inputFile.tellg());
	inputFile.seekg(0, std::ios::beg);

	byte* pBuffer = new byte[fileSize];

	// Read the whole file into pBuffer
	inputFile.read(reinterpret_cast<char*>(pBuffer), fileSize);
	inputFile.close();

	// Now go through each byte of the file with *pPtr
	byte* pPtr = pBuffer;

	// Load the Header
	MS3DHeader* pHeader = reinterpret_cast<MS3DHeader*>(pPtr);
	pPtr += sizeof(MS3DHeader);

	if (strncmp(pHeader->ID, "MS3D000000", 10) != 0)
	{
		std::cerr << "Not an MS3D file." << std::endl;
		return false;
	}

	if (pHeader->version < 3)
	{
		std::cerr << "I know nothing about MS3D v1.2" << std::endl;
		return false;
	}

	// Load the Vertices
	int nVertices = *reinterpret_cast<word*>(pPtr);
	m_numVertices = nVertices;
	m_pVertices = new Vertex[nVertices];
	pPtr += sizeof(word);

	for (i = 0; i < nVertices; ++i)
	{
		MS3DVertex *pVertex = reinterpret_cast<MS3DVertex*>(pPtr);
		m_pVertices[i].boneID = pVertex->boneID;
		memcpy(m_pVertices[i].location, pVertex->vertex, sizeof(float) * 3);
		pPtr += sizeof(MS3DVertex);
	}

	// Load the Triangles
	int nTriangles = *reinterpret_cast<word*>(pPtr);
	m_numTriangles = nTriangles;
	m_pTriangles = new Triangle[nTriangles];
	pPtr += sizeof(word);

	for (i = 0; i < nTriangles; i++)
	{
		MS3DTriangle *pTriangle = reinterpret_cast<MS3DTriangle*>(pPtr);
		int vertexIndices[3] = { pTriangle->vertexIndices[0], pTriangle->vertexIndices[1], pTriangle->vertexIndices[2] };
		float t[3] = { 1.0f - pTriangle->t[0], 1.0f - pTriangle->t[1], 1.0f - pTriangle->t[2] };
		memcpy(m_pTriangles[i].vertexNormals, pTriangle->vertexNormals, sizeof(float) * 3 * 3);
		memcpy(m_pTriangles[i].s, pTriangle->s, sizeof(float) * 3);
		memcpy(m_pTriangles[i].t, t, sizeof(float) * 3);
		memcpy(m_pTriangles[i].vertexIndices, vertexIndices, sizeof(int) * 3);
		pPtr += sizeof(MS3DTriangle);
	}

	// Load the Meshes
	int nGroups = *reinterpret_cast<word*>(pPtr);
	m_numMeshes = nGroups;
	m_pMeshes = new Mesh[nGroups];
	pPtr += sizeof(word);

	for (i = 0; i < nGroups; ++i)
	{
		pPtr += sizeof(byte);		// flags
		pPtr += 32;					// name

		word numTriangles = *reinterpret_cast<word*>(pPtr);
		pPtr += sizeof(word);
		int* pTriangleIndices = new int[numTriangles];
		for (int j = 0; j < numTriangles; ++j)
		{
			pTriangleIndices[j] = *reinterpret_cast<word*>(pPtr);
			pPtr += sizeof(word);
		}

		char materialIndex = *reinterpret_cast<char*>(pPtr);
		pPtr += sizeof(char);

		m_pMeshes[i].materialIndex = materialIndex;
		m_pMeshes[i].numTriangles = numTriangles;
		m_pMeshes[i].pTriangleIndices = pTriangleIndices;
	}

	// Load the Materials
	int nMaterials = *reinterpret_cast<word*>(pPtr);
	m_numMaterials = nMaterials;
	m_pMaterials = new MaterialModel[nMaterials];
	pPtr += sizeof(word);

	for (i = 0; i < nMaterials; ++i)
	{
		MS3DMaterial *pMaterial = reinterpret_cast<MS3DMaterial*>(pPtr);
		memcpy(m_pMaterials[i].ambient, pMaterial->ambient, sizeof(float) * 4);
		memcpy(m_pMaterials[i].diffuse, pMaterial->diffuse, sizeof(float) * 4);
		memcpy(m_pMaterials[i].specular, pMaterial->specular, sizeof(float) * 4);
		memcpy(m_pMaterials[i].emissive, pMaterial->emissive, sizeof(float) * 4);
		m_pMaterials[i].shininess = pMaterial->shininess;

		if (strncmp(pMaterial->texture, ".\\", 2) == 0)
		{
			// MS3D 1.5.x relative path
			strcpy(pathTemp + pathLength, pMaterial->texture + 2);
			m_pMaterials[i].pTextureFileName = new char[strlen(pathTemp) + 1];
			strcpy(m_pMaterials[i].pTextureFileName, pathTemp);
		}
		else
		{
			// MS3D 1.4.x or earlier - absolute path
			strcpy(pathTemp + pathLength, pMaterial->texture);
			m_pMaterials[i].pTextureFileName = new char[strlen(pathTemp) + 1];
			strcpy(m_pMaterials[i].pTextureFileName, pathTemp);
		}
		pPtr += sizeof(MS3DMaterial);
	}

	// Get the Animation Speed
	m_animationFPS = *reinterpret_cast<float*>(pPtr);
	pPtr += sizeof(float);

	// Skip currentTime
	pPtr += sizeof(float);

	// Get the total frames for the animation
	// ReSharper disable CppEntityNeverUsed
	int totalFrames = *reinterpret_cast<int*>(pPtr);
	// ReSharper restore CppEntityNeverUsed
	pPtr += sizeof(int);

	// Get the number of joints
	m_numJoints = *reinterpret_cast<word*>(pPtr);
	pPtr += sizeof(word);

	m_pJoints = new Joint[m_numJoints];

	struct JointNameListRec
	{
		int jointIndex;
		const char* pName;
	};

	byte *pTempPtr = pPtr;

	JointNameListRec *pNameList = new JointNameListRec[m_numJoints];
	for (i = 0; i < m_numJoints; ++i)
	{
		MS3DJoint *pJoint = reinterpret_cast<MS3DJoint*>(pTempPtr);
		pTempPtr += sizeof(MS3DJoint);
		pTempPtr += sizeof(MS3DKeyframe) * (pJoint->numRotationKeyframes + pJoint->numTranslationKeyframes);

		pNameList[i].jointIndex = i;
		pNameList[i].pName = pJoint->name;

		sprintf(m_pJoints[i].name, "%s", pJoint->name);
	}

	// Load the Joints
	for (i = 0; i < m_numJoints; ++i)
	{
		MS3DJoint *pJoint = reinterpret_cast<MS3DJoint*>(pPtr);
		pPtr += sizeof(MS3DJoint);

		int parentIndex = -1;
		if (strlen(pJoint->parentName) > 0)
		{
			for (int j = 0; j < m_numJoints; ++j)
			{
				#ifdef _WIN32
				if (_strcmpi(pNameList[j].pName, pJoint->parentName) == 0)
				#else
				if (strcasecmp(pNameList[j].pName, pJoint->parentName) == 0)
				#endif //_WIN32
				{
					parentIndex = pNameList[j].jointIndex;
					break;
				}
			}

			if (parentIndex == -1)
			{
				std::cerr << "Unable to find parent bone in MS3D file" << std::endl;
				return false;
			}
		}

		memcpy(m_pJoints[i].localRotation, pJoint->rotation, sizeof(float) * 3);
		memcpy(m_pJoints[i].localTranslation, pJoint->translation, sizeof(float) * 3);
		m_pJoints[i].parent = parentIndex;
		m_pJoints[i].numRotationKeyframes = pJoint->numRotationKeyframes;
		m_pJoints[i].pRotationKeyframes = new Keyframe[pJoint->numRotationKeyframes];
		m_pJoints[i].numTranslationKeyframes = pJoint->numTranslationKeyframes;
		m_pJoints[i].pTranslationKeyframes = new Keyframe[pJoint->numTranslationKeyframes];

		// Load the Rotation keyframes
		for (int j = 0; j < pJoint->numRotationKeyframes; ++j)
		{
			MS3DKeyframe *pKeyframe = reinterpret_cast<MS3DKeyframe*>(pPtr);
			pPtr += sizeof(MS3DKeyframe);

			SetJointKeyframe(i, j, pKeyframe->time*1000.0f, pKeyframe->parameter, true);
		}

		//Load the Translation keyframes
		for (int j = 0; j < pJoint->numTranslationKeyframes; ++j)
		{
			MS3DKeyframe *pKeyframe = reinterpret_cast<MS3DKeyframe*>(pPtr);
			pPtr += sizeof(MS3DKeyframe);

			SetJointKeyframe(i, j, pKeyframe->time*1000.0f, pKeyframe->parameter, false);
		}
	}

	// Setup the joints
	SetupJoints();

	// Load the textures
	if (!LoadTextures())
	{
		return false;
	}

	// Delete the temporary arrays
	delete[] pNameList;
	delete[] pBuffer;

	// Calculate the bounding box
	CalculateBoundingBox();

	m_isStatic = isStatic;

	if (m_isStatic)
	{
		SetupStaticBuffer();
	}

	return true;
}

bool MS3DModel::LoadTextures() const
{
	for (int i = 0; i < m_numMaterials; ++i)
	{
		if (strlen(m_pMaterials[i].pTextureFileName) > 0)
		{
			int textureWidth, textureHeight, textureWidth2, textureHeight2;
			if (!m_pRenderer->LoadTexture(m_pMaterials[i].pTextureFileName, &textureWidth, &textureHeight, &textureWidth2, &textureHeight2, &m_pMaterials[i].texture))
			{
				return false;
			}
		}
		else
		{
			m_pMaterials[i].texture = -1;
		}
	}

	return true;
}

// Setup
void MS3DModel::SetupStaticBuffer()
{
	m_staticRenderBuffers = new unsigned int[m_numMeshes];

	for (int i = 0; i < m_numMeshes; ++i)
	{
		// Create the buffer used for drawing the image
		PositionNormalVertex* vertices;
		vertices = new PositionNormalVertex[m_pMeshes[i].numTriangles * 3];

		UVCoordinate* textureCoordinates;
		textureCoordinates = new UVCoordinate[m_pMeshes[i].numTriangles * 3];

		int indexCounter = 0;
		for (int j = 0; j < m_pMeshes[i].numTriangles; ++j)
		{
			int triangleIndex = m_pMeshes[i].pTriangleIndices[j];
			const Triangle* pTri = &m_pTriangles[triangleIndex];

			for (int k = 0; k < 3; ++k)
			{
				int index = pTri->vertexIndices[k];

				Matrix4& finalMatrix = m_pJoints[m_pVertices[index].boneID].absolute;

				glm::vec3 newVertex(m_pVertices[index].location[0], m_pVertices[index].location[1], m_pVertices[index].location[2]);
				newVertex = finalMatrix * newVertex;

				vertices[indexCounter].x = newVertex.x;
				vertices[indexCounter].y = newVertex.y;
				vertices[indexCounter].z = newVertex.z;

				vertices[indexCounter].nx = pTri->vertexNormals[k][0];
				vertices[indexCounter].ny = pTri->vertexNormals[k][1];
				vertices[indexCounter].nz = pTri->vertexNormals[k][2];

				textureCoordinates[indexCounter].u = pTri->s[k];
				textureCoordinates[indexCounter].v = pTri->t[k];

				indexCounter++;
			}
		}

		int materialIndex = m_pMeshes[i].materialIndex;

		m_pRenderer->CreateStaticBuffer(VertexType::POSITION_NORMAL_UV, -1, m_pMaterials[materialIndex].texture, m_pMeshes[i].numTriangles * 3, m_pMeshes[i].numTriangles * 3, 0, vertices, textureCoordinates, nullptr, &m_staticRenderBuffers[i]);
	}
}

void MS3DModel::SetJointKeyframe(int jointIndex, int keyframeIndex, float time, float* parameter, bool isRotation) const
{
	Keyframe& keyframe = isRotation ? m_pJoints[jointIndex].pRotationKeyframes[keyframeIndex] : m_pJoints[jointIndex].pTranslationKeyframes[keyframeIndex];

	keyframe.jointIndex = jointIndex;
	keyframe.time = time;
	memcpy(keyframe.parameter, parameter, sizeof(float) * 3);
}

void MS3DModel::SetupJoints() const
{
	for (int i = 0; i < m_numJoints; ++i)
	{
		Joint& joint = m_pJoints[i];

		joint.relative.AddRotationByRadians(joint.localRotation);
		joint.relative.AddTranslation(joint.localTranslation);

		if (joint.parent != -1)
		{
			joint.absolute = m_pJoints[joint.parent].absolute;

			joint.absolute.PostMultiply(joint.relative);
		}
		else
		{
			joint.absolute = joint.relative;
		}
	}

	for (int i = 0; i < m_numVertices; ++i)
	{
		Vertex& vertex = m_pVertices[i];

		if (vertex.boneID != -1)
		{
			Matrix4& matrix = m_pJoints[vertex.boneID].absolute;

			matrix.InverseTranslateVector(vertex.location);
			matrix.InverseRotateVector(vertex.location);
		}
	}

	for (int i = 0; i < m_numTriangles; ++i)
	{
		Triangle& triangle = m_pTriangles[i];

		for (int j = 0; j < 3; ++j)
		{
			const Vertex& vertex = m_pVertices[triangle.vertexIndices[j]];
			if (vertex.boneID != -1)
			{
				Matrix4& matrix = m_pJoints[vertex.boneID].absolute;
				matrix.InverseRotateVector(triangle.vertexNormals[j]);
			}
		}
	}
}

// Bounding box
void MS3DModel::CalculateBoundingBox()
{
	for (int i = 0; i < m_numVertices; ++i)
	{
		if (m_pVertices[i].boneID == -1)
		{
			// Don't use this vertex if it doesn't have a valid boneID, since this will mess up the matrix math
			continue;
		}

		Matrix4& finalMatrix = m_pJoints[m_pVertices[i].boneID].absolute;
		glm::vec3 newVertex(m_pVertices[i].location[0], m_pVertices[i].location[1], m_pVertices[i].location[2]);

		newVertex = finalMatrix * newVertex;

		float tempVertex[3];
		tempVertex[0] = newVertex.x;
		tempVertex[1] = newVertex.y;
		tempVertex[2] = newVertex.z;

		// Always set the first vertex values
		if (i == 0)
		{
			m_boundingBox.SetMinX(tempVertex[0]);
			m_boundingBox.SetMinY(tempVertex[1]);
			m_boundingBox.SetMinZ(tempVertex[2]);

			m_boundingBox.SetMaxX(tempVertex[0]);
			m_boundingBox.SetMaxY(tempVertex[1]);
			m_boundingBox.SetMaxZ(tempVertex[2]);
		}
		else
		{
			if (tempVertex[0] < m_boundingBox.GetMinX())
			{
				m_boundingBox.SetMinX(tempVertex[0]);
			}

			if (tempVertex[1] < m_boundingBox.GetMinY())
			{
				m_boundingBox.SetMinY(tempVertex[1]);
			}

			if (tempVertex[2] < m_boundingBox.GetMinZ())
			{
				m_boundingBox.SetMinZ(tempVertex[2]);
			}

			if (tempVertex[0] > m_boundingBox.GetMaxX())
			{
				m_boundingBox.SetMaxX(tempVertex[0]);
			}

			if (tempVertex[1] > m_boundingBox.GetMaxY())
			{
				m_boundingBox.SetMaxY(tempVertex[1]);
			}

			if (tempVertex[2] > m_boundingBox.GetMaxZ())
			{
				m_boundingBox.SetMaxZ(tempVertex[2]);
			}
		}
	}
}

BoundingBox* MS3DModel::GetBoundingBox()
{
	return &m_boundingBox;
}

// Bones
int MS3DModel::GetBoneIndex(const char* boneName) const
{
	for (int i = 0; i < m_numJoints; ++i)
	{
		if (strcmp(m_pJoints[i].name, boneName) == 0)
		{
			return i;
		}
	}

	return -1;
}

const char* MS3DModel::GetNameFromBoneIndex(int boneIndex) const
{
	return m_pJoints[boneIndex].name;
}

// Joints
int MS3DModel::GetNumJoints() const
{
	return m_numJoints;
}

Joint* MS3DModel::GetJoint(const char* jointName) const
{
	for (int i = 0; i < m_numJoints; ++i)
	{
		if (strcmp(m_pJoints[i].name, jointName) == 0)
		{
			return GetJoint(i);
		}
	}

	return nullptr;
}

Joint* MS3DModel::GetJoint(int index) const
{
	return &m_pJoints[index];
}

// Rendering
void MS3DModel::Render(bool isMesh, bool isNormal, bool isBone, bool isBoundingBox)
{
	if (isMesh)
	{
		if (m_isStatic)
		{
			RenderMeshStatic();
		}
		else
		{
			RenderMesh();
		}
	}

	if (isNormal)
	{
		RenderNormals();
	}

	if (isBone)
	{
		RenderBones();
	}

	if (isBoundingBox)
	{
		RenderBoundingBox();
	}
}

void MS3DModel::RenderMesh() const
{
	// Draw by group
	for (int i = 0; i < m_numMeshes; ++i)
	{
		int materialIndex = m_pMeshes[i].materialIndex;
		m_pRenderer->BindTexture(m_pMaterials[materialIndex].texture);

		glBegin(GL_TRIANGLES);
		{
			for (int j = 0; j < m_pMeshes[i].numTriangles; ++j)
			{
				int triangleIndex = m_pMeshes[i].pTriangleIndices[j];
				const Triangle* pTri = &m_pTriangles[triangleIndex];

				for (int k = 0; k < 3; ++k)
				{
					int index = pTri->vertexIndices[k];

					if (m_pVertices[index].boneID == -1)
					{
						// Don't draw the foot steps!
					}
					else
					{
						// Rotate according to transformation matrix
						Matrix4& finalMatrix = m_pJoints[m_pVertices[index].boneID].absolute;

						// Texture co-ordinate
						glTexCoord2f(pTri->s[k], pTri->t[k]);

						// Normal
						glm::vec3 newNormal(pTri->vertexNormals[k][0], pTri->vertexNormals[k][1], pTri->vertexNormals[k][2]);

						newNormal = finalMatrix * newNormal;

						newNormal = normalize(newNormal);

						float tempNormal[3];
						tempNormal[0] = newNormal.x;
						tempNormal[1] = newNormal.y;
						tempNormal[2] = newNormal.z;
						glNormal3fv(tempNormal);

						// Vertex
						glm::vec3 newVertex(m_pVertices[index].location[0], m_pVertices[index].location[1], m_pVertices[index].location[2]);

						newVertex = finalMatrix * newVertex;

						float tempVertex[3];
						tempVertex[0] = newVertex.x;
						tempVertex[1] = newVertex.y;
						tempVertex[2] = newVertex.z;
						glVertex3fv(tempVertex);
					}
				}
			}
		}
		glEnd();

		m_pRenderer->DisableTexture();
	}
}

void MS3DModel::RenderMeshStatic() const
{
	m_pRenderer->PushMatrix();
	m_pRenderer->SetRenderMode(RenderMode::TEXTURED_LIGHTING);
	m_pRenderer->SetPrimitiveMode(PrimitiveMode::TRIANGLES);

	for (int i = 0; i < m_numMeshes; ++i)
	{
		m_pRenderer->RenderStaticBuffer(m_staticRenderBuffers[i]);
	}

	m_pRenderer->PopMatrix();
}

void MS3DModel::RenderNormals()
{
	// Make the color cyan
	glColor3ub(0, 255, 255);

	glDisable(GL_LIGHTING);
	m_pRenderer->SetRenderMode(RenderMode::SOLID);

	for (int i = 0; i < m_numMeshes; ++i)
	{
		glBegin(GL_LINES);
		{
			for (int j = 0; j < m_pMeshes[i].numTriangles; ++j)
			{
				int triangleIndex = m_pMeshes[i].pTriangleIndices[j];
				const Triangle* pTri = &m_pTriangles[triangleIndex];

				for (int k = 0; k < 3; ++k)
				{
					int index = pTri->vertexIndices[k];

					if (m_pVertices[index].boneID == -1)
					{
						// Don't draw the foot steps!
					}
					else
					{
						// Rotate according to transformation matrix
						Matrix4& finalMatrix = m_pJoints[m_pVertices[index].boneID].absolute;

						// Normal
						glm::vec3 newNormal(pTri->vertexNormals[k][0], pTri->vertexNormals[k][1], pTri->vertexNormals[k][2]);

						newNormal = finalMatrix * newNormal;

						newNormal = normalize(newNormal);
						// Scale normal down
						newNormal *= 0.3f; 

						float tempNormal[3];
						tempNormal[0] = newNormal.x;
						tempNormal[1] = newNormal.y;
						tempNormal[2] = newNormal.z;

						// Vertex
						glm::vec3 newVertex(m_pVertices[index].location[0], m_pVertices[index].location[1], m_pVertices[index].location[2]);

						newVertex = finalMatrix * newVertex;

						float tempVertex[3];
						tempVertex[0] = newVertex.x;
						tempVertex[1] = newVertex.y;
						tempVertex[2] = newVertex.z;

						// Draw a line for the normal
						glVertex3f(tempVertex[0], tempVertex[1], tempVertex[2]);
						glVertex3f(tempVertex[0] + tempNormal[0], tempVertex[1] + tempNormal[1], tempVertex[2] + tempNormal[2]);
					}
				}
			}
		}
		glEnd();
	}
}

void MS3DModel::RenderBones() const
{
	// Make the color white
	glColor3ub(255, 255, 255);

	for (int i = 0; i < m_numJoints; ++i)
	{
		glBegin(GL_LINES);
		{
			glm::vec3 newVertex;

			newVertex = m_pJoints[i].absolute * newVertex;

			float tempVertex[3];
			tempVertex[0] = newVertex.x;
			tempVertex[1] = newVertex.y;
			tempVertex[2] = newVertex.z;
			glVertex3fv(tempVertex);

			if (m_pJoints[i].parent != -1)
			{
				Matrix4& finalMatrix = m_pJoints[m_pJoints[i].parent].absolute;
				glm::vec3 newParentVertex;

				newParentVertex = finalMatrix * newParentVertex;

				float tempPVertex[3];
				tempPVertex[0] = newParentVertex.x;
				tempPVertex[1] = newParentVertex.y;
				tempPVertex[2] = newParentVertex.z;
				glVertex3fv(tempPVertex);
			}
		}

		glEnd();
	}
}

void MS3DModel::RenderBoundingBox()
{
	m_pRenderer->PushMatrix();
	m_pRenderer->ImmediateColorAlpha(1.0f, 1.0f, 0.0f, 1.0f);

	m_pRenderer->EnableImmediateMode(ImmediateModePrimitive::LINES);

	// Bottom
	m_pRenderer->ImmediateVertex(m_boundingBox.GetMinX(), m_boundingBox.GetMinY(), m_boundingBox.GetMinZ());
	m_pRenderer->ImmediateVertex(m_boundingBox.GetMaxX(), m_boundingBox.GetMinY(), m_boundingBox.GetMinZ());

	m_pRenderer->ImmediateVertex(m_boundingBox.GetMaxX(), m_boundingBox.GetMinY(), m_boundingBox.GetMinZ());
	m_pRenderer->ImmediateVertex(m_boundingBox.GetMaxX(), m_boundingBox.GetMinY(), m_boundingBox.GetMaxZ());

	m_pRenderer->ImmediateVertex(m_boundingBox.GetMaxX(), m_boundingBox.GetMinY(), m_boundingBox.GetMaxZ());
	m_pRenderer->ImmediateVertex(m_boundingBox.GetMinX(), m_boundingBox.GetMinY(), m_boundingBox.GetMaxZ());

	m_pRenderer->ImmediateVertex(m_boundingBox.GetMinX(), m_boundingBox.GetMinY(), m_boundingBox.GetMaxZ());
	m_pRenderer->ImmediateVertex(m_boundingBox.GetMinX(), m_boundingBox.GetMinY(), m_boundingBox.GetMinZ());

	// Top
	m_pRenderer->ImmediateVertex(m_boundingBox.GetMinX(), m_boundingBox.GetMaxY(), m_boundingBox.GetMinZ());
	m_pRenderer->ImmediateVertex(m_boundingBox.GetMaxX(), m_boundingBox.GetMaxY(), m_boundingBox.GetMinZ());

	m_pRenderer->ImmediateVertex(m_boundingBox.GetMaxX(), m_boundingBox.GetMaxY(), m_boundingBox.GetMinZ());
	m_pRenderer->ImmediateVertex(m_boundingBox.GetMaxX(), m_boundingBox.GetMaxY(), m_boundingBox.GetMaxZ());

	m_pRenderer->ImmediateVertex(m_boundingBox.GetMaxX(), m_boundingBox.GetMaxY(), m_boundingBox.GetMaxZ());
	m_pRenderer->ImmediateVertex(m_boundingBox.GetMinX(), m_boundingBox.GetMaxY(), m_boundingBox.GetMaxZ());

	m_pRenderer->ImmediateVertex(m_boundingBox.GetMinX(), m_boundingBox.GetMaxY(), m_boundingBox.GetMaxZ());
	m_pRenderer->ImmediateVertex(m_boundingBox.GetMinX(), m_boundingBox.GetMaxY(), m_boundingBox.GetMinZ());

	// Sides
	m_pRenderer->ImmediateVertex(m_boundingBox.GetMinX(), m_boundingBox.GetMinY(), m_boundingBox.GetMinZ());
	m_pRenderer->ImmediateVertex(m_boundingBox.GetMinX(), m_boundingBox.GetMaxY(), m_boundingBox.GetMinZ());

	m_pRenderer->ImmediateVertex(m_boundingBox.GetMaxX(), m_boundingBox.GetMinY(), m_boundingBox.GetMinZ());
	m_pRenderer->ImmediateVertex(m_boundingBox.GetMaxX(), m_boundingBox.GetMaxY(), m_boundingBox.GetMinZ());

	m_pRenderer->ImmediateVertex(m_boundingBox.GetMaxX(), m_boundingBox.GetMinY(), m_boundingBox.GetMaxZ());
	m_pRenderer->ImmediateVertex(m_boundingBox.GetMaxX(), m_boundingBox.GetMaxY(), m_boundingBox.GetMaxZ());

	m_pRenderer->ImmediateVertex(m_boundingBox.GetMinX(), m_boundingBox.GetMinY(), m_boundingBox.GetMaxZ());
	m_pRenderer->ImmediateVertex(m_boundingBox.GetMinX(), m_boundingBox.GetMaxY(), m_boundingBox.GetMaxZ());

	m_pRenderer->DisableImmediateMode();
	m_pRenderer->PopMatrix();
}