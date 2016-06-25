//
//	tga.h - header file
//
//	David Henry - tfc_duke@hotmail.com
//


#ifndef		__TARGA_H_
#define		__TARGA_H_



#pragma warning( disable : 4103 ) // used #pragma pack to change alignment



// --------------------------------------------
// TGAHEADER - targa header.
// --------------------------------------------

#pragma pack(1)

typedef struct tagTGAHEADER
{
	unsigned char	id_lenght;			// size of the structure
	unsigned char	color_map_type;		// must be equal to 0
	unsigned char	image_type;			// image compression type

	short int		cm_first_entry;		// colormap_origin (always 0)
	short int		cm_length;			// colormap_length (always 0)
	unsigned char	cm_size;			// colormap_size (always 0)

	short int		is_xorigin;			// lower left X coordinate (always 0)
	short int		is_yorigin;			// lower left Y coordinate (always 0)

	short int		is_width;			// image width (in pixels)
	short int		is_height;			// image height (in pixels)

	unsigned char	is_pixel_depth;		// number of bits per pixel: 16, 24, 32
	unsigned char	is_image_descriptor;// 24 bits = 0x00; 32 bits = 0x80

} TGAHEADER, *PTGAHEADER;

#pragma pack(4)



#ifdef WIN32
#include	<windows.h>
#else

// --------------------------------------------
// RGBTRIPLE - 24 bits pixel
// --------------------------------------------

typedef struct tagRGBTRIPLE			// rgbt
{
	unsigned char	rgbtBlue;		// blue
	unsigned char	rgbtGreen;		// green
	unsigned char	rgbtRed;		// red

} RGBTRIPLE, *PRGBTRIPLE;

#endif	// WIN32



// --------------------------------------------
// BGRAQUAD - 32 bits pixel
// --------------------------------------------

typedef struct tagBGRAQUAD			// rgbt
{
	unsigned char	bgraBlue;		// blue
	unsigned char	bgraGreen;		// green
	unsigned char	bgraRed;		// red
	unsigned char	bgraAlpha;		// alpha

} BGRAQUAD, *PBGRAQUAD;


#endif	// __TARGA_H_
