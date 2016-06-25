/*************************************************************************
> File Name: Texture.h
> Project Name: Cubby
> Author: Chan-Ho Chris Ohk
> Purpose: An OpenGL texture object.
> Created Time: 2016/06/22
> Copyright (c) 2016, Chan-Ho Chris Ohk
*************************************************************************/

#include "Texture.h"

#ifndef GL_CLAMP_TO_EDGE
#define GL_CLAMP_TO_EDGE 0x812F
#endif

inline int NextPower2(int num)
{
	int ret = 2;

	while (ret < num)
	{
		ret <<= 1;
	}

	return ret;
}

// Constructor
Texture::Texture() :
	m_fileType(TextureFileType::BMP), m_width(0), m_height(0), m_widthPower2(0), m_heightPower2(0), m_id(0)
{
	
}

// Destructor
Texture::~Texture()
{
	
}

// Getter, Setter
std::string Texture::GetFileName() const
{
	return m_fileName;
}

TextureFileType Texture::GetFileType() const
{
	return m_fileType;
}

int Texture::GetWidth() const
{
	return m_width;
}

int Texture::GetHeight() const
{
	return m_height;
}
int Texture::GetWidthPower2() const
{
	return m_widthPower2;
}

int Texture::GetHeightPower2() const
{
	return m_heightPower2;
}

GLuint Texture::GetID() const
{
	return m_id;
}

bool Texture::Load(std::string fileName, int* width, int* height, int* widthPower2, int* heightPower2, bool refresh)
{
	m_fileName = fileName;

	bool needScaling = false;
	int numChannels = 0;
	unsigned char* texData = nullptr;
	unsigned char* texDataPower2 = nullptr;

	bool loaded = false;

	if (strstr(fileName.c_str(), ".jpg"))
	{
		// TODO: Add back in JPG support
	}
	else if (strstr(fileName.c_str(), ".tga"))
	{
		loaded = LoadFileTGA(fileName.c_str(), &texData, width, height, true) == 1;
		m_fileType = TextureFileType::TGA;
		// Was initially true but not really sure if this is needed or not...
		needScaling = false;
		numChannels = 4;
	}
	else if (strstr(fileName.c_str(), ".bmp"))
	{
		// TODO: Add back in BMP support
	}

	if (loaded == false)
	{
		return false;
	}

	// Store the real width and height of this texture
	m_width = *width;
	m_height = *height;

	if (needScaling)
	{
		// Now get the next power of 2 for width and height, since we want our final texture to be a power of 2
		m_widthPower2 = NextPower2(m_width);
		m_heightPower2 = NextPower2(m_height);

		*widthPower2 = m_widthPower2;
		*heightPower2 = m_heightPower2;

		// Create space for the new power of 2 data
		texDataPower2 = new unsigned char[m_widthPower2 * m_heightPower2 * 4];

		// Pointer to the original data
		unsigned char* ptr = (&texData)[0];

		// Pointer into the new power of 2 data
		unsigned char* ptrPower2 = (&texDataPower2)[0];

		// Go through the old data and insert it into the new expanded structure, or add padding if we need to...
		for (int y = 0; y < m_heightPower2; ++y)
		{
			for (int x = 0; x < m_widthPower2; ++x)
			{
				if (y < m_height && x < m_width)
				{
					// We don't need padding
					ptrPower2[0] = ptr[0];
					ptrPower2[1] = ptr[1];
					ptrPower2[2] = ptr[2];
					ptrPower2[3] = ptr[3];

					ptr += 4;
				}
				else
				{
					// We need padding
					ptrPower2[0] = 0;
					ptrPower2[1] = 0;
					ptrPower2[2] = 0;
					ptrPower2[3] = 0;
				}

				ptrPower2 += 4;
			}
		}
	}
	else
	{
		m_widthPower2 = m_width;
		m_heightPower2 = m_height;
		*widthPower2 = m_widthPower2;
		*heightPower2 = m_heightPower2;
	}

	if (refresh == false)
	{
		// Create a new texture id, since we are loading a fully new texture
		glGenTextures(1, &m_id);
	}

	glBindTexture(GL_TEXTURE_2D, m_id);

	// TODO: Are these good parameters for MAG and MIN filters?
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);  // GL_LINEAR
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);  // GL_LINEAR

	int textureType = GL_RGB;
	if (numChannels == 4)
	{
		textureType = GL_RGBA;
	}

	if (needScaling)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_widthPower2, m_heightPower2, 0, textureType, GL_UNSIGNED_BYTE, texDataPower2);
	}
	else
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_width, m_height, 0, textureType, GL_UNSIGNED_BYTE, texData);
	}

	if (needScaling)
	{
		// Delete the data now
		if (texDataPower2)
		{
			delete[] texDataPower2;
		}
	}

	if (texData)
	{
		delete[] texData;
	}

	return true;
}

void Texture::GenerateEmptyTexture()
{
	// Create a new texture id, since we are loading a fully new texture
	glGenTextures(1, &m_id);

	m_width = -1;
	m_height = -1;
	m_widthPower2 = -1;
	m_heightPower2 = -1;
}

void Texture::Bind() const {
	glBindTexture(GL_TEXTURE_2D, m_id);
}