// ******************************************************************************
//
// Filename:	freetypefont.cpp
// Project:		Vox
// Author:		Steven Ball
//
// Purpose:
//   
//
// Revision History:
//   Initial Revision - 11/10/08
//
// Copyright (c) 2005-20015, Steven Ball
//
// ******************************************************************************

#include "freetypefont.h"

#include <stdio.h>
#include <assert.h>

#include <freetype/freetype.h>
#include <freetype/ftglyph.h>
#include <freetype/ftoutln.h>
#include <freetype/fttrigon.h>


inline int next_p2(int a)
{
	int rval = 2;

	while(rval < a)
		rval <<= 1;

	return rval;
}

FreeTypeFont::FreeTypeFont()
{
	m_inited = false;
}

FreeTypeFont::~FreeTypeFont()
{
	if(m_inited)
	{
		glDeleteLists(m_base, 128);
		glDeleteTextures(128, m_textures);
		delete [] m_textures;

		FT_Done_Face(m_face);

		FT_Done_FreeType(m_library);
	}
}

void FreeTypeFont::BuildFont(const char* fontName, int size)
{
	if (FT_Init_FreeType( &m_library )) 
	{
		// Failed to initalize the freetype library
		assert(0);
	}

	FT_Error l_error = FT_New_Face( m_library, fontName, 0, &m_face );

	if ( l_error == FT_Err_Unknown_File_Format )
	{
		// The font format is not supported
		assert(0);
	}
	else if ( l_error )
	{
		// The font could not be opened
		assert(0);
	}

	//FT_Set_Char_Size( m_face, size << 6, size << 6, 96, 96);
	l_error = FT_Set_Pixel_Sizes(m_face, 0, size);
	assert(l_error == 0);

	// Keep track of the font size
	m_size = size;

	m_textures = new GLuint[128];
	m_base = glGenLists(128);
	glGenTextures(128, m_textures);

	for(unsigned char i = 0; i < 128; i++)
	{
		MakeDisplayList(m_face, i, m_base, m_textures);
	}

	m_inited = true;
}

void FreeTypeFont::MakeDisplayList(FT_Face face, unsigned char ch, GLuint list_base, GLuint * tex_base)
{
	//The first thing we do is get FreeType to render our character
	//into a bitmap.  This actually requires a couple of FreeType commands:

	//Load the Glyph for our character.
	if(FT_Load_Glyph( face, FT_Get_Char_Index( face, ch ), FT_LOAD_DEFAULT ))
	{
		// Load glyph failed
		assert(0);
	}

	//Move the face's glyph into a Glyph object.
	FT_Glyph glyph;
	if(FT_Get_Glyph( face->glyph, &glyph ))
	{
		// Get glyph failed
		assert(0);
	}

	//Convert the glyph to a bitmap.
	FT_Glyph_To_Bitmap( &glyph, ft_render_mode_normal, 0, 1 );
	FT_BitmapGlyph bitmap_glyph = (FT_BitmapGlyph)glyph;

	//This reference will make accessing the bitmap easier
	FT_Bitmap& bitmap=bitmap_glyph->bitmap;

	//Use our helper function to get the widths of
	//the bitmap data that we will need in order to create
	//our texture.
	int width = next_p2( bitmap.width );
	int height = next_p2( bitmap.rows );

	//Allocate memory for the texture data.
	GLubyte* expanded_data = new GLubyte[ 2 * width * height];

	//Here we fill in the data for the expanded bitmap.
	//Notice that we are using two channel bitmap (one for
	//luminocity and one for alpha), but we assign
	//both luminocity and alpha to the value that we
	//find in the FreeType bitmap. 
	//We use the ?: operator so that value which we use
	//will be 0 if we are in the padding zone, and whatever
	//is the the Freetype bitmap otherwise.
	for(int j=0; j < height;j++) for(int i=0; i < width; i++) {
		expanded_data[2*(i+j*width)] = 255;
		expanded_data[2*(i+j*width)+1] =
			(i>=bitmap.width || j>=bitmap.rows) ?
			0 : bitmap.buffer[i + bitmap.width*j];
	}

	//Now we just setup some texture paramaters.
	glBindTexture( GL_TEXTURE_2D, tex_base[ch]);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);

	//Here we actually create the texture itself, notice
	//that we are using GL_LUMINANCE_ALPHA to indicate that
	//we are using 2 channel data.
	glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, width, height,
		0, GL_LUMINANCE_ALPHA, GL_UNSIGNED_BYTE, expanded_data );

	//With the texture created, we don't need to expanded data anymore
	delete [] expanded_data;

	//So now we can create the display list
	glNewList(list_base+ch,GL_COMPILE);

	glBindTexture(GL_TEXTURE_2D,tex_base[ch]);

	glPushMatrix();

	//first we need to move over a little so that
	//the character has the right amount of space
	//between it and the one before it.
	glTranslatef(bitmap_glyph->left,0,0);

	//Now we move down a little in the case that the
	//bitmap extends past the bottom of the line 
	//(this is only true for characters like 'g' or 'y'.
	glTranslatef(0,bitmap_glyph->top-bitmap.rows,0);

	//Now we need to account for the fact that many of
	//our textures are filled with empty padding space.
	//We figure what portion of the texture is used by 
	//the actual character and store that information in 
	//the x and y variables, then when we draw the
	//quad, we will only reference the parts of the texture
	//that we contain the character itself.
	float	x=(float)bitmap.width / (float)width,
		y=(float)bitmap.rows / (float)height;

	//Here we draw the texturemaped quads.
	//The bitmap that we got from FreeType was not 
	//oriented quite like we would like it to be,
	//so we need to link the texture to the quad
	//so that the result will be properly aligned.
	glBegin(GL_QUADS);
	glTexCoord2d(0,0); glVertex2f(0,bitmap.rows);
	glTexCoord2d(0,y); glVertex2f(0,0);
	glTexCoord2d(x,y); glVertex2f(bitmap.width,0);
	glTexCoord2d(x,0); glVertex2f(bitmap.width,bitmap.rows);
	glEnd();
	glPopMatrix();
	glTranslatef(face->glyph->advance.x >> 6 ,0,0);


	//increment the raster position as if we were a bitmap font.
	//(only needed if you want to calculate text length)
	glBitmap(0,0,0,0,face->glyph->advance.x >> 6,0,NULL);

	//Finnish the display list
	glEndList();

	FT_Done_Glyph(glyph);
}

void FreeTypeFont::DrawString(const char *text, float scale)
{
	int textWidth = GetTextWidth(text);
	int textHeight = GetCharHeight('a');

	GLsizei len = GLsizei(strlen(text));
	if(text && len > 0)
	{
		glPushAttrib(GL_LIST_BIT | GL_CURRENT_BIT  | GL_ENABLE_BIT | GL_TRANSFORM_BIT);	
		{
			glDisable(GL_LIGHTING);
			glEnable(GL_TEXTURE_2D);
			glDisable(GL_DEPTH_TEST);
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);	
			
			glTranslatef(textWidth*0.5f, textHeight*0.5f, 0.0f);
			glScalef(scale, scale, scale);
			glTranslatef(-textWidth*0.5f, -textHeight*0.5f, 0.0f);

			glListBase(m_base);
			glCallLists(len, GL_UNSIGNED_BYTE, (const GLvoid*)text);
		}
		glPopAttrib();
	}
}

int FreeTypeFont::GetTextWidth(const char *text)
{
	float width = 0.0f;

	GLsizei len = GLsizei(strlen(text));
	if(text && len > 0)
	{
		glPushAttrib(GL_LIST_BIT | GL_CURRENT_BIT  | GL_ENABLE_BIT | GL_TRANSFORM_BIT);	
		{
			glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);  // Disabled rendering

			glPushMatrix();

			// Store the position before we start to draw the text
			float lpos[4];
			glGetFloatv(GL_CURRENT_RASTER_POSITION, lpos);

			// Draw the text
			glListBase(m_base);
			glCallLists(len, GL_UNSIGNED_BYTE, (const GLvoid*)text);

			// Now get the position after we have drawn the text
			float rpos[4];
			glGetFloatv(GL_CURRENT_RASTER_POSITION, rpos);

			// Now we can work out the width
			width = rpos[0] - lpos[0];

			glPopMatrix();

			glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);   // Enable rendering
		}
		glPopAttrib();
	}

	return (int)width;
}

int FreeTypeFont::GetCharWidth(int c)
{
	// TODO : Write me...
	return 0;
}

int FreeTypeFont::GetCharHeight(int c)
{
	return m_size;
}

int FreeTypeFont::GetAscent()
{
	FT_Size_Metrics*  metrics = &m_face->size->metrics;
	return metrics->ascender >> 6;
}

int FreeTypeFont::GetDescent()
{
	FT_Size_Metrics*  metrics = &m_face->size->metrics;
	return metrics->descender >> 6;
}