/*************************************************************************
> File Name: Texture.h
> Project Name: Cubby
> Author: Chan-Ho Chris Ohk
> Purpose: An OpenGL texture object.
> Created Time: 2016/06/22
> Copyright (c) 2016, Chan-Ho Chris Ohk
*************************************************************************/

#ifndef CUBBY_TEXTURE_H
#define CUBBY_TEXTURE_H

#ifdef _WIN32
#include <windows.h>
#endif

#include <GL/gl.h>

#include <string>

#pragma comment (lib, "opengl32")
#pragma comment (lib, "glu32")

// TODO: Add back in JPG, TGA, BMP support
// int LoadFileJPG(const char* fileName, unsigned char** pixels, int* width, int* height);
int LoadFileTGA(const char* fileName, unsigned char** pixels, int* width, int* height, bool filpVert);
// int LoadFileBMP(const char* fileName, unsigned char** pixels, int* width, int* height);

enum class TextureFileType
{
	BMP = 0,
	JPG,
	TGA,
};

class Texture
{
public:
	// Constructor
	Texture();
	// Destructor
	~Texture();

	// Getter, Setter
	std::string GetFileName() const;
	TextureFileType GetFileType() const;
	int GetWidth() const;
	int GetHeight() const;
	int GetWidthPower2() const;
	int GetHeightPower2() const;
	GLuint GetID() const;

	// Operations
	bool Load(std::string fileName, int* width, int* height, int* widthPower2, int* heightPower2, bool refresh);
	void GenerateEmptyTexture();
	void Bind() const;

private:
	std::string m_fileName;
	TextureFileType m_fileType;

	int m_width;
	int m_height;

	int m_widthPower2;
	int m_heightPower2;

	GLuint m_id;
};

#endif