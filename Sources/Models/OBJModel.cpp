/*************************************************************************
> File Name: OBJModel.cpp
> Project Name: Cubby
> Author: Chan-Ho Chris Ohk
> Purpose: Object model.
> Created Time: 2016/07/05
> Copyright (c) 2016, Chan-Ho Chris Ohk
*************************************************************************/

#include "OBJModel.h"

// Constructor, Destructor
OBJModel::OBJModel(Renderer* pRenderer) :
	m_pRenderer(pRenderer),
	m_numVertices(0), m_pVertices(nullptr),
	m_numTexCoordinates(0), m_pTextureCoordinates(nullptr),
	m_numNormals(0), m_pNormals(nullptr),
	m_numFaces(0), m_pFaces(nullptr),
	m_texture(-1)
{
	
}

OBJModel::~OBJModel()
{
	// Delete the vertices
	delete[] m_pVertices;
	m_pVertices = nullptr;

	// Delete the texture coordinates
	delete[] m_pTextureCoordinates;
	m_pTextureCoordinates = nullptr;

	// Delete the normals
	delete[] m_pNormals;
	m_pNormals = nullptr;

	// Delete the indices from the polygon list
	for (int i = 0; i < m_numFaces; ++i)
	{
		delete[] m_pFaces[i].pIndices;
		m_pFaces[i].pIndices = nullptr;
	}

	// Delete the faces
	delete[] m_pFaces;
	m_pFaces = nullptr;
}

bool OBJModel::Load(const char* modelFileName, const char* textureFileName)
{
	m_ModelFileName = modelFileName;

	FILE *pFile;
	char buffer[256] = { 0 };

	// Make sure that we have passed a filename
	if (modelFileName == nullptr)
	{
		return false;
	}

	// Open the file
	if (fopen_s(&pFile, modelFileName, "r"))
	{
		return false;
	}

	// Go through the obj file and count the number of vertices and faces
	while (!feof(pFile))
	{
		fgets(buffer, sizeof(buffer), pFile);

		if (buffer[0] == 'v' && buffer[1] == ' ')
		{
			m_numVertices++;
		}
		else if (buffer[0] == 'v' && buffer[1] == 't')
		{
			m_numTexCoordinates++;
		}
		else if (buffer[0] == 'v' && buffer[1] == 'n')
		{
			m_numNormals++;
		}
		else if (buffer[0] == 'f' && buffer[1] == ' ')
		{
			m_numFaces++;
		}
	}

	// Allocate the arrays
	m_pVertices = new OBJVertex[m_numVertices];
	m_pTextureCoordinates = new OBJTextureCoordinate[m_numTexCoordinates];
	m_pNormals = new glm::vec3[m_numNormals];
	m_pFaces = new OBJFace[m_numFaces];

	memset(m_pVertices, 0, sizeof(OBJVertex) * m_numVertices);
	memset(m_pTextureCoordinates, 0, sizeof(OBJTextureCoordinate) * m_numTexCoordinates);
	memset(m_pNormals, 0, sizeof(glm::vec3) * m_numNormals);
	memset(m_pFaces, 0, sizeof(OBJFace) * m_numFaces);

	// Reset the buffer
	memset(buffer, '\0', sizeof(buffer));

	// Get back to the beginning of the file
	rewind(pFile);

	int vertexIndex = 0;
	int textureCoordinateIndex = 0;
	int normalIndex = 0;
	int faceIndex = 0;
	char* token;
	char tempBuffer[256];
	char tempChar;

	while (!feof(pFile))
	{
		// Read in the character of the line
		fscanf(pFile, "%c", &tempChar);

		// If it is a v then we are reading either a vertex point, texture coordinate or a vertex normal
		if (tempChar == 'v')
		{
			fscanf(pFile, "%c", &tempChar);

			if (tempChar == ' ')
			{
				// Vertex point
				// Read in the x, y, z values of this vertex point
				fscanf(pFile, "%f", &m_pVertices[vertexIndex].position.x);
				fscanf(pFile, "%f", &m_pVertices[vertexIndex].position.y);
				fscanf(pFile, "%f", &m_pVertices[vertexIndex].position.z);

				// Set all the vertices to initially not be assigned to a mass
				m_pVertices[vertexIndex].massID = -1;

				// Read in the newline
				fscanf(pFile, "%c", &tempChar);

				// Increment the index
				vertexIndex++;
			}
			else if (tempChar == 't')
			{
				// Texture coordinate
				// Read in the u, v, values of this texture coordinate
				fscanf(pFile, "%f", &m_pTextureCoordinates[textureCoordinateIndex].u);
				fscanf(pFile, "%f", &m_pTextureCoordinates[textureCoordinateIndex].v);

				// Read in the newline
				fscanf(pFile, "%c", &tempChar);

				// Increment the index
				textureCoordinateIndex++;
			}
			else if (tempChar == 'n')
			{
				// Vertex normal
				// Read in the x, y, z values of this vertex normal
				fscanf(pFile, "%f", &m_pNormals[normalIndex].x);
				fscanf(pFile, "%f", &m_pNormals[normalIndex].y);
				fscanf(pFile, "%f", &m_pNormals[normalIndex].z);

				// Read in the newline
				fscanf(pFile, "%c", &tempChar);

				// Increment the index
				normalIndex++;
			}
		}
		else if (tempChar == 'f')
		{
			// If we read in a f check that the next char is a space
			fscanf(pFile, "%c", &tempChar);

			if (tempChar == ' ')
			{
				// Read in the rest of the line
				fgets(buffer, sizeof(buffer), pFile);

				// Copy to a temp buffer so we can mess with it.
				buffer[strlen(buffer)] = '\0';
				strcpy(tempBuffer, buffer);

				// Set the search token. Used to distinguish between each polygon
				token = strtok(tempBuffer, " ");

				// Loop through the line and get the number of times we see the search token
				int count;
				for (count = 0; token != nullptr; ++count)
				{
					token = strtok(nullptr, " ");
				}

				m_pFaces[faceIndex].pIndices = new int[count * 3];
				m_pFaces[faceIndex].numPoints = count;

				// HACK: This is a *very* bad way to do this!
				if (m_numNormals == 0 && m_numTexCoordinates == 0)
				{
					if (count == 1)
					{
						sscanf(buffer, "%i", &m_pFaces[faceIndex].pIndices[0]);
					}
					else if (count == 2)
					{
						sscanf(buffer, "%i %i", &m_pFaces[faceIndex].pIndices[0], &m_pFaces[faceIndex].pIndices[3]);
					}
					else if (count == 3)
					{
						sscanf(buffer, "%i %i %i", &m_pFaces[faceIndex].pIndices[0], &m_pFaces[faceIndex].pIndices[3], &m_pFaces[faceIndex].pIndices[6]);
					}
					else if (count == 4)
					{
						sscanf(buffer, "%i %i %i %i", &m_pFaces[faceIndex].pIndices[0], &m_pFaces[faceIndex].pIndices[3], &m_pFaces[faceIndex].pIndices[6], &m_pFaces[faceIndex].pIndices[9]);
					}
					else if (count == 5)
					{
						sscanf(buffer, "%i %i %i %i %i", &m_pFaces[faceIndex].pIndices[0], &m_pFaces[faceIndex].pIndices[3], &m_pFaces[faceIndex].pIndices[6], &m_pFaces[faceIndex].pIndices[9], &m_pFaces[faceIndex].pIndices[12]);
					}
					else if (count == 6)
					{
						sscanf(buffer, "%i %i %i %i %i %i", &m_pFaces[faceIndex].pIndices[0], &m_pFaces[faceIndex].pIndices[3], &m_pFaces[faceIndex].pIndices[6], &m_pFaces[faceIndex].pIndices[9], &m_pFaces[faceIndex].pIndices[12], &m_pFaces[faceIndex].pIndices[15]);
					}
				}
				else if (m_numNormals == 0)
				{
					if (count == 1)
					{
						sscanf(buffer, "%i/%i/", &m_pFaces[faceIndex].pIndices[0], &m_pFaces[faceIndex].pIndices[1]);
					}
					else if (count == 2)
					{
						sscanf(buffer, "%i/%i %i/%i", &m_pFaces[faceIndex].pIndices[0], &m_pFaces[faceIndex].pIndices[1],
							&m_pFaces[faceIndex].pIndices[3], &m_pFaces[faceIndex].pIndices[4]);
					}
					else if (count == 3)
					{
						sscanf(buffer, "%i/%i %i/%i %i/%i", &m_pFaces[faceIndex].pIndices[0], &m_pFaces[faceIndex].pIndices[1],
							&m_pFaces[faceIndex].pIndices[3], &m_pFaces[faceIndex].pIndices[4],
							&m_pFaces[faceIndex].pIndices[6], &m_pFaces[faceIndex].pIndices[7]);
					}
					else if (count == 4)
					{
						sscanf(buffer, "%i/%i %i/%i %i/%i %i/%i", &m_pFaces[faceIndex].pIndices[0], &m_pFaces[faceIndex].pIndices[1],
							&m_pFaces[faceIndex].pIndices[3], &m_pFaces[faceIndex].pIndices[4],
							&m_pFaces[faceIndex].pIndices[6], &m_pFaces[faceIndex].pIndices[7],
							&m_pFaces[faceIndex].pIndices[9], &m_pFaces[faceIndex].pIndices[10]);
					}
					else if (count == 5)
					{
						sscanf(buffer, "%i/%i %i/%i %i/%i %i/%i %i/%i", &m_pFaces[faceIndex].pIndices[0], &m_pFaces[faceIndex].pIndices[1],
							&m_pFaces[faceIndex].pIndices[3], &m_pFaces[faceIndex].pIndices[4],
							&m_pFaces[faceIndex].pIndices[6], &m_pFaces[faceIndex].pIndices[7],
							&m_pFaces[faceIndex].pIndices[9], &m_pFaces[faceIndex].pIndices[10],
							&m_pFaces[faceIndex].pIndices[12], &m_pFaces[faceIndex].pIndices[13]);
					}
					else if (count == 6)
					{
						sscanf(buffer, "%i/%i %i/%i %i/%i %i/%i %i/%i %i/%i", &m_pFaces[faceIndex].pIndices[0], &m_pFaces[faceIndex].pIndices[1],
							&m_pFaces[faceIndex].pIndices[3], &m_pFaces[faceIndex].pIndices[4],
							&m_pFaces[faceIndex].pIndices[6], &m_pFaces[faceIndex].pIndices[7],
							&m_pFaces[faceIndex].pIndices[9], &m_pFaces[faceIndex].pIndices[10],
							&m_pFaces[faceIndex].pIndices[12], &m_pFaces[faceIndex].pIndices[13],
							&m_pFaces[faceIndex].pIndices[15], &m_pFaces[faceIndex].pIndices[16]);
					}
				}
				else if (m_numTexCoordinates == 0)
				{
					if (count == 1)
					{
						sscanf(buffer, "%i//%i", &m_pFaces[faceIndex].pIndices[0], &m_pFaces[faceIndex].pIndices[2]);
					}
					else if (count == 2)
					{
						sscanf(buffer, "%i//%i %i//%i", &m_pFaces[faceIndex].pIndices[0], &m_pFaces[faceIndex].pIndices[2],
							&m_pFaces[faceIndex].pIndices[3], &m_pFaces[faceIndex].pIndices[5]);
					}
					else if (count == 3)
					{
						sscanf(buffer, "%i//%i %i//%i %i//%i", &m_pFaces[faceIndex].pIndices[0], &m_pFaces[faceIndex].pIndices[2],
							&m_pFaces[faceIndex].pIndices[3], &m_pFaces[faceIndex].pIndices[5],
							&m_pFaces[faceIndex].pIndices[6], &m_pFaces[faceIndex].pIndices[8]);
					}
					else if (count == 4)
					{
						sscanf(buffer, "%i//%i %i//%i %i//%i %i//%i", &m_pFaces[faceIndex].pIndices[0], &m_pFaces[faceIndex].pIndices[2],
							&m_pFaces[faceIndex].pIndices[3], &m_pFaces[faceIndex].pIndices[5],
							&m_pFaces[faceIndex].pIndices[6], &m_pFaces[faceIndex].pIndices[8],
							&m_pFaces[faceIndex].pIndices[9], &m_pFaces[faceIndex].pIndices[11]);
					}
					else if (count == 5)
					{
						sscanf(buffer, "%i//%i %i//%i %i//%i %i//%i %i//%i", &m_pFaces[faceIndex].pIndices[0], &m_pFaces[faceIndex].pIndices[2],
							&m_pFaces[faceIndex].pIndices[3], &m_pFaces[faceIndex].pIndices[5],
							&m_pFaces[faceIndex].pIndices[6], &m_pFaces[faceIndex].pIndices[8],
							&m_pFaces[faceIndex].pIndices[9], &m_pFaces[faceIndex].pIndices[11],
							&m_pFaces[faceIndex].pIndices[12], &m_pFaces[faceIndex].pIndices[14]);
					}
					else if (count == 6)
					{
						sscanf(buffer, "%i//%i %i//%i %i//%i %i//%i %i//%i %i//%i", &m_pFaces[faceIndex].pIndices[0], &m_pFaces[faceIndex].pIndices[2],
							&m_pFaces[faceIndex].pIndices[3], &m_pFaces[faceIndex].pIndices[5],
							&m_pFaces[faceIndex].pIndices[6], &m_pFaces[faceIndex].pIndices[8],
							&m_pFaces[faceIndex].pIndices[9], &m_pFaces[faceIndex].pIndices[11],
							&m_pFaces[faceIndex].pIndices[12], &m_pFaces[faceIndex].pIndices[14],
							&m_pFaces[faceIndex].pIndices[15], &m_pFaces[faceIndex].pIndices[17]);
					}
				}
				else
				{
					if (count == 1)
					{
						sscanf(buffer, "%i/%i/%i", &m_pFaces[faceIndex].pIndices[0], &m_pFaces[faceIndex].pIndices[1], &m_pFaces[faceIndex].pIndices[2]);
					}
					else if (count == 2)
					{
						sscanf(buffer, "%i/%i/%i %i/%i/%i", &m_pFaces[faceIndex].pIndices[0], &m_pFaces[faceIndex].pIndices[1], &m_pFaces[faceIndex].pIndices[2],
							&m_pFaces[faceIndex].pIndices[3], &m_pFaces[faceIndex].pIndices[4], &m_pFaces[faceIndex].pIndices[5]);
					}
					else if (count == 3)
					{
						sscanf(buffer, "%i/%i/%i %i/%i/%i %i/%i/%i", &m_pFaces[faceIndex].pIndices[0], &m_pFaces[faceIndex].pIndices[1], &m_pFaces[faceIndex].pIndices[2],
							&m_pFaces[faceIndex].pIndices[3], &m_pFaces[faceIndex].pIndices[4], &m_pFaces[faceIndex].pIndices[5],
							&m_pFaces[faceIndex].pIndices[6], &m_pFaces[faceIndex].pIndices[7], &m_pFaces[faceIndex].pIndices[8]);
					}
					else if (count == 4)
					{
						sscanf(buffer, "%i/%i/%i %i/%i/%i %i/%i/%i %i/%i/%i", &m_pFaces[faceIndex].pIndices[0], &m_pFaces[faceIndex].pIndices[1], &m_pFaces[faceIndex].pIndices[2],
							&m_pFaces[faceIndex].pIndices[3], &m_pFaces[faceIndex].pIndices[4], &m_pFaces[faceIndex].pIndices[5],
							&m_pFaces[faceIndex].pIndices[6], &m_pFaces[faceIndex].pIndices[7], &m_pFaces[faceIndex].pIndices[8],
							&m_pFaces[faceIndex].pIndices[9], &m_pFaces[faceIndex].pIndices[10], &m_pFaces[faceIndex].pIndices[11]);
					}
					else if (count == 5)
					{
						sscanf(buffer, "%i/%i/%i %i/%i/%i %i/%i/%i %i/%i/%i %i/%i/%i", &m_pFaces[faceIndex].pIndices[0], &m_pFaces[faceIndex].pIndices[1], &m_pFaces[faceIndex].pIndices[2],
							&m_pFaces[faceIndex].pIndices[3], &m_pFaces[faceIndex].pIndices[4], &m_pFaces[faceIndex].pIndices[5],
							&m_pFaces[faceIndex].pIndices[6], &m_pFaces[faceIndex].pIndices[7], &m_pFaces[faceIndex].pIndices[8],
							&m_pFaces[faceIndex].pIndices[9], &m_pFaces[faceIndex].pIndices[10], &m_pFaces[faceIndex].pIndices[11],
							&m_pFaces[faceIndex].pIndices[12], &m_pFaces[faceIndex].pIndices[13], &m_pFaces[faceIndex].pIndices[14]);
					}
					else if (count == 6)
					{
						sscanf(buffer, "%i/%i/%i %i/%i/%i %i/%i/%i %i/%i/%i %i/%i/%i %i/%i/%i", &m_pFaces[faceIndex].pIndices[0], &m_pFaces[faceIndex].pIndices[1], &m_pFaces[faceIndex].pIndices[2],
							&m_pFaces[faceIndex].pIndices[3], &m_pFaces[faceIndex].pIndices[4], &m_pFaces[faceIndex].pIndices[5],
							&m_pFaces[faceIndex].pIndices[6], &m_pFaces[faceIndex].pIndices[7], &m_pFaces[faceIndex].pIndices[8],
							&m_pFaces[faceIndex].pIndices[9], &m_pFaces[faceIndex].pIndices[10], &m_pFaces[faceIndex].pIndices[11],
							&m_pFaces[faceIndex].pIndices[12], &m_pFaces[faceIndex].pIndices[13], &m_pFaces[faceIndex].pIndices[14],
							&m_pFaces[faceIndex].pIndices[15], &m_pFaces[faceIndex].pIndices[16], &m_pFaces[faceIndex].pIndices[17]);
					}
				}
				// HACK: End of hack!

				faceIndex++;
			}
		}
		else
		{
			// Read the rest of the line
			if (tempChar != '\n')
			{
				fgets(buffer, sizeof(buffer), pFile);
			}
		}

		memset(buffer, '0', sizeof(buffer));
	}

	// Close the file
	fclose(pFile);

	// Also load a texture for this model
	int textureWidth;
	int textureHeight;
	int textureWidthPower2;
	int textureHeightPower2;

	if (textureFileName)
	{
		m_TextureFileName = textureFileName;
		m_pRenderer->LoadTexture(textureFileName, &textureWidth, &textureHeight, &textureWidthPower2, &textureHeightPower2, &m_texture);
	}

	// Calculate the bounding box
	CalculateBoundingBox();

	return true;
}

void OBJModel::CalculateBoundingBox()
{
	for (int i = 0; i < m_numVertices; ++i)
	{
		// Always set the first vertex values
		if (i == 0)
		{
			m_BoundingBox.SetMinX(m_pVertices[i].position.x);
			m_BoundingBox.SetMinY(m_pVertices[i].position.y);
			m_BoundingBox.SetMinZ(m_pVertices[i].position.z);

			m_BoundingBox.SetMaxX(m_pVertices[i].position.x);
			m_BoundingBox.SetMaxY(m_pVertices[i].position.y);
			m_BoundingBox.SetMaxZ(m_pVertices[i].position.z);
		}
		else
		{
			if (m_pVertices[i].position.x < m_BoundingBox.GetMinX())
			{
				m_BoundingBox.SetMinX(m_pVertices[i].position.x);
			}

			if (m_pVertices[i].position.y < m_BoundingBox.GetMinY())
			{
				m_BoundingBox.SetMinY(m_pVertices[i].position.y);
			}

			if (m_pVertices[i].position.z < m_BoundingBox.GetMinZ())
			{
				m_BoundingBox.SetMinZ(m_pVertices[i].position.z);
			}

			if (m_pVertices[i].position.x > m_BoundingBox.GetMaxX())
			{
				m_BoundingBox.SetMaxX(m_pVertices[i].position.x);
			}

			if (m_pVertices[i].position.y > m_BoundingBox.GetMaxY())
			{
				m_BoundingBox.SetMaxY(m_pVertices[i].position.y);
			}

			if (m_pVertices[i].position.z > m_BoundingBox.GetMaxZ())
			{
				m_BoundingBox.SetMaxZ(m_pVertices[i].position.z);
			}
		}
	}
}

BoundingBox* OBJModel::GetBoundingBox()
{
	return &m_BoundingBox;
}

void OBJModel::Render()
{
	RenderMesh();
	RenderWireFrame();

	RenderBoundingBox();
}

void OBJModel::RenderMesh()
{
	m_pRenderer->PushMatrix();

	// NOTE: Lighting isn't supported yet!

	if (m_texture != -1)
	{
		m_pRenderer->BindTexture(m_texture);

		m_pRenderer->SetRenderMode(RenderMode::TEXTURED);
	}
	else
	{
		m_pRenderer->SetRenderMode(RenderMode::SOLID);
	}

	// Draw each face
	for (int i = 0; i < m_numFaces; ++i)
	{
		// Decide on what primitive type to use, depending on how many points there are
		if (m_pFaces[i].numPoints == 3)
		{
			m_pRenderer->EnableImmediateMode(ImmediateModePrimitive::TRIANGLES);
		}
		else if (m_pFaces[i].numPoints == 4)
		{
			m_pRenderer->EnableImmediateMode(ImmediateModePrimitive::QUADS);
		}
		else
		{
			m_pRenderer->EnableImmediateMode(ImmediateModePrimitive::POLYGON);
		}

		m_pRenderer->ImmediateColorAlpha(1.0f, 1.0f, 1.0f, 1.0f);

		// Draw the primitive
		for (int j = 0; j < m_pFaces[i].numPoints; ++j)
		{
			int vertexIndex = m_pFaces[i].pIndices[3 * j] - 1;
			int textureIndex = m_pFaces[i].pIndices[3 * j + 1] - 1;

			if (m_numTexCoordinates != 0)
			{
				m_pRenderer->ImmediateTextureCoordinate(m_pTextureCoordinates[textureIndex].u, 1 - m_pTextureCoordinates[textureIndex].v);
			}

			m_pRenderer->ImmediateVertex(m_pVertices[vertexIndex].position.x, m_pVertices[vertexIndex].position.y, m_pVertices[vertexIndex].position.z);
		}

		m_pRenderer->DisableImmediateMode();
	}

	if (m_texture != -1)
	{
		m_pRenderer->DisableTexture();
	}

	m_pRenderer->PopMatrix();
}

void OBJModel::RenderWireFrame()
{
	m_pRenderer->PushMatrix();

	// Store cull mode
	CullMode cullMode = m_pRenderer->GetCullMode();

	m_pRenderer->SetRenderMode(RenderMode::WIREFRAME);
	m_pRenderer->SetCullMode(CullMode::NOCULL);

	// Draw each face
	for (int i = 0; i < m_numFaces; ++i)
	{
		// Decide on what primitive type to use, depending on how many points there are
		if (m_pFaces[i].numPoints == 3)
		{
			m_pRenderer->EnableImmediateMode(ImmediateModePrimitive::TRIANGLES);
		}
		else if (m_pFaces[i].numPoints == 4)
		{
			m_pRenderer->EnableImmediateMode(ImmediateModePrimitive::QUADS);
		}
		else
		{
			m_pRenderer->EnableImmediateMode(ImmediateModePrimitive::POLYGON);
		}

		m_pRenderer->ImmediateColorAlpha(0.0f, 0.0f, 0.0f, 1.0f);

		// Draw the primitive
		for (int j = 0; j < m_pFaces[i].numPoints; ++j)
		{
			int vertexIndex = m_pFaces[i].pIndices[(3 * j)] - 1;

			m_pRenderer->ImmediateVertex(m_pVertices[vertexIndex].position.x, m_pVertices[vertexIndex].position.y, m_pVertices[vertexIndex].position.z);
		}

		m_pRenderer->DisableImmediateMode();
	}

	// Restore cull mode
	m_pRenderer->SetCullMode(cullMode);

	m_pRenderer->PopMatrix();
}

void OBJModel::RenderBoundingBox() const
{
	m_pRenderer->PushMatrix();
	m_pRenderer->ImmediateColorAlpha(1.0f, 1.0f, 0.0f, 1.0f);

	m_pRenderer->EnableImmediateMode(ImmediateModePrimitive::LINES);

	// Bottom
	m_pRenderer->ImmediateVertex(m_BoundingBox.GetMinX(), m_BoundingBox.GetMinY(), m_BoundingBox.GetMinZ());
	m_pRenderer->ImmediateVertex(m_BoundingBox.GetMaxX(), m_BoundingBox.GetMinY(), m_BoundingBox.GetMinZ());

	m_pRenderer->ImmediateVertex(m_BoundingBox.GetMaxX(), m_BoundingBox.GetMinY(), m_BoundingBox.GetMinZ());
	m_pRenderer->ImmediateVertex(m_BoundingBox.GetMaxX(), m_BoundingBox.GetMinY(), m_BoundingBox.GetMaxZ());

	m_pRenderer->ImmediateVertex(m_BoundingBox.GetMaxX(), m_BoundingBox.GetMinY(), m_BoundingBox.GetMaxZ());
	m_pRenderer->ImmediateVertex(m_BoundingBox.GetMinX(), m_BoundingBox.GetMinY(), m_BoundingBox.GetMaxZ());

	m_pRenderer->ImmediateVertex(m_BoundingBox.GetMinX(), m_BoundingBox.GetMinY(), m_BoundingBox.GetMaxZ());
	m_pRenderer->ImmediateVertex(m_BoundingBox.GetMinX(), m_BoundingBox.GetMinY(), m_BoundingBox.GetMinZ());

	// Top
	m_pRenderer->ImmediateVertex(m_BoundingBox.GetMinX(), m_BoundingBox.GetMaxY(), m_BoundingBox.GetMinZ());
	m_pRenderer->ImmediateVertex(m_BoundingBox.GetMaxX(), m_BoundingBox.GetMaxY(), m_BoundingBox.GetMinZ());

	m_pRenderer->ImmediateVertex(m_BoundingBox.GetMaxX(), m_BoundingBox.GetMaxY(), m_BoundingBox.GetMinZ());
	m_pRenderer->ImmediateVertex(m_BoundingBox.GetMaxX(), m_BoundingBox.GetMaxY(), m_BoundingBox.GetMaxZ());

	m_pRenderer->ImmediateVertex(m_BoundingBox.GetMaxX(), m_BoundingBox.GetMaxY(), m_BoundingBox.GetMaxZ());
	m_pRenderer->ImmediateVertex(m_BoundingBox.GetMinX(), m_BoundingBox.GetMaxY(), m_BoundingBox.GetMaxZ());

	m_pRenderer->ImmediateVertex(m_BoundingBox.GetMinX(), m_BoundingBox.GetMaxY(), m_BoundingBox.GetMaxZ());
	m_pRenderer->ImmediateVertex(m_BoundingBox.GetMinX(), m_BoundingBox.GetMaxY(), m_BoundingBox.GetMinZ());

	// Sides
	m_pRenderer->ImmediateVertex(m_BoundingBox.GetMinX(), m_BoundingBox.GetMinY(), m_BoundingBox.GetMinZ());
	m_pRenderer->ImmediateVertex(m_BoundingBox.GetMinX(), m_BoundingBox.GetMaxY(), m_BoundingBox.GetMinZ());

	m_pRenderer->ImmediateVertex(m_BoundingBox.GetMaxX(), m_BoundingBox.GetMinY(), m_BoundingBox.GetMinZ());
	m_pRenderer->ImmediateVertex(m_BoundingBox.GetMaxX(), m_BoundingBox.GetMaxY(), m_BoundingBox.GetMinZ());

	m_pRenderer->ImmediateVertex(m_BoundingBox.GetMaxX(), m_BoundingBox.GetMinY(), m_BoundingBox.GetMaxZ());
	m_pRenderer->ImmediateVertex(m_BoundingBox.GetMaxX(), m_BoundingBox.GetMaxY(), m_BoundingBox.GetMaxZ());

	m_pRenderer->ImmediateVertex(m_BoundingBox.GetMinX(), m_BoundingBox.GetMinY(), m_BoundingBox.GetMaxZ());
	m_pRenderer->ImmediateVertex(m_BoundingBox.GetMinX(), m_BoundingBox.GetMaxY(), m_BoundingBox.GetMaxZ());

	m_pRenderer->DisableImmediateMode();
	m_pRenderer->PopMatrix();
}
