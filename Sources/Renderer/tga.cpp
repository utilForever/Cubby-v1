//
//	tga.cpp - source file / freeware
//
//	David Henry - tfc_duke@hotmail.com
//


#include	<fstream>
#include	"tga.h"



// --------------------------------------------------
// LoadFileTGA() - load a TrueVision TARGA image [.tga].
//
// parameters :
//    - filename [in]  : image source file
//    - pixels	 [out] : 32 bits rgba image data
//    - width    [out] : image width in pixels
//    - height   [out] : image height in pixels
//    - flipvert [in]  : flip vertically
//
// return value :
//    - -1 : no image data
//    -  0 : failure
//    -  1 : success
//
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// accepted image formats :
//     # RGB 8-16-24-32 bits
//     # RLE 8-16-24-32 bits
// --------------------------------------------------

int LoadFileTGA( const char *filename, unsigned char **pixels, int *width, int *height, bool flipvert )
{
	std::ifstream	file;				// file stream
	TGAHEADER		*tgah;				// targa header
	RGBTRIPLE		*palette = NULL;	// pointer on the color palette
	char			*buffer;			// buffer storing the entire file
	unsigned char	*ptr;				// pointer to pixels data
	int				row, col, i;		// temporary variables



	/////////////////////////////////////////////////////
	// read the entire file in the buffer

	file.open( filename, std::ios::in | std::ios::binary );

	if( file.fail() )
		return 0;

	file.seekg( 0, std::ios::end );
	long flen = (long)file.tellg();
	file.seekg( 0, std::ios::beg );

	buffer = new char[ flen + 1 ];
	file.read( buffer, flen );
	char *pBuff = buffer;

	file.close();

	/////////////////////////////////////////////////////


	// read the header
	tgah = (TGAHEADER *)pBuff;
	pBuff += sizeof( TGAHEADER );


	if( width )
		*width = tgah->is_width;

	if( height )
		*height = tgah->is_height;

	if( !pixels )
	{
		delete [] buffer;
		return (-1);
	}


	// allocate memory to store pixel data
	(*pixels) = new unsigned char[ tgah->is_width * tgah->is_height * 4 ];
	ptr = &(*pixels)[0];


	// move the pixel data pointer to the begening of bitmap data
	if( tgah->id_lenght )
		pBuff = buffer + (tgah->id_lenght * sizeof( unsigned char ));


	/////////////////////////////////////////////////////
	// read the palette

	if( tgah->color_map_type )
	{
		// 24 and 32 bits images are not paletted

		// ajust the palette pointer to the memory in the buffer
		palette = (RGBTRIPLE *)pBuff;

		pBuff += tgah->cm_length * (tgah->cm_size >> 3) * sizeof( unsigned char );
	}

	/////////////////////////////////////////////////////


	/////////////////////////////////////////////////////
	// read pixel data following the image compression
	// type and the number of bits per pixels
	/////////////////////////////////////////////////////

	switch( tgah->image_type )
	{
		case 0:
			// pas de données image
			break;

		case 1:
// COLOR-MAPPED BGR 8 BITS GREYSCALE

		case 3:
		{
// COLOR-MAPPED BGR 8 BITS
			for( row = tgah->is_height - 1; row >= 0; row-- )
			{
				if( flipvert )
					ptr = &(*pixels)[ row * tgah->is_width * 4 ];

				for( col = 0; col < tgah->is_width; col++, ptr += 4 )
				{
					// read the current pixel
					unsigned char color = *((unsigned char *)(pBuff++));

					// convert indexed pixel (8 bits) into rgba (32 bits) pixel
					ptr[0] = palette[ color ].rgbtRed;		// b->r
					ptr[1] = palette[ color ].rgbtGreen;	// g->g
					ptr[2] = palette[ color ].rgbtBlue;		// r->b
					ptr[3] = 255;							// alpha
				}
			}

			break;
		}

		case 2:
		{
			for( row = tgah->is_height - 1; row >= 0; row-- )
			{
				if( flipvert )
					ptr = &(*pixels)[ row * tgah->is_width * 4 ];

				for( col = 0; col < tgah->is_width; col++, ptr += 4 )
				{
					switch( tgah->is_pixel_depth )
					{
						case 16:
						{
// TRUE-COLOR BGR 16 BITS
							// read the current pixel
							unsigned short color = *((unsigned short *)pBuff);
							pBuff += sizeof( short );

							// convert bgr (16 bits) pixel into rgba (32 bits) pixel
							ptr[0] = ((color & 0x7C00) >> 10) << 3;	// b->r
							ptr[1] = ((color & 0x03E0) >>  5) << 3;	// g->g
							ptr[2] = ((color & 0x001F) >>  0) << 3;	// r->b
							ptr[3] = 255;							// alpha

							break;
						}

						case 24:
						{
// TRUE-COLOR BGR 24 BITS
							// convert bgr (24 bits) pixel into rgba (32 bits) pixel
							RGBTRIPLE *pix = (RGBTRIPLE *)pBuff;
							pBuff += sizeof( RGBTRIPLE );

							ptr[0] = pix->rgbtRed;
							ptr[1] = pix->rgbtGreen;
							ptr[2] = pix->rgbtBlue;
							ptr[3] = 255;

							break;
						}

						case 32:
						{
// TRUE-COLOR BGR 32 BITS
							// convert bgr (32 bits) pixel into rgba (32 bits) pixel
							BGRAQUAD *pix = (BGRAQUAD *)pBuff;
							pBuff += sizeof( BGRAQUAD );

							ptr[0] = pix->bgraRed;
							ptr[1] = pix->bgraGreen;
							ptr[2] = pix->bgraBlue;
							ptr[3] = pix->bgraAlpha;

							break;
						}
					}
				}
			}

			break;
		}

		case 9:
// RLE COLOR-MAPPED BGR 8 BITS

		case 11:
		{
// RLE COLOR-MAPPED BGR 8 BITS GREYSCALE

			unsigned char	packetHeader, packetSize, i;


			for( row = tgah->is_height - 1; row >= 0; row-- )
			{
				if( flipvert )
					ptr = &(*pixels)[ row * tgah->is_width * 4 ];

				for( col = 0; col < tgah->is_width; /* rien */ )
				{
					packetHeader	= *((unsigned char *)(pBuff++));
					packetSize		= 1 + (packetHeader & 0x7f);

					if( packetHeader & 0x80 )
					{
						// run-length packet

						// read the current pixel
						unsigned char color = *((unsigned char *)(pBuff++));

						// convert indexed pixel (8 bits) pixel into rgba (32 bits) pixel
						for( i = 0; i < packetSize; i++, ptr += 4, col++ )
						{
							ptr[0] = palette[ color ].rgbtRed;		// b->r
							ptr[1] = palette[ color ].rgbtGreen;	// g->g
							ptr[2] = palette[ color ].rgbtBlue;		// r->b
							ptr[3] = 255;							// alpha
						}
					}
					else
					{
						// non run-length packet

						for( i = 0; i < packetSize; i++, ptr += 4, col++ )
						{
							// read the current pixel
							unsigned char color = *((unsigned char *)(pBuff++));

							// convert indexed pixel (8 bits) pixel into rgba (32 bits) pixel
							ptr[0] = palette[ color ].rgbtRed;		// b->r
							ptr[1] = palette[ color ].rgbtGreen;	// g->g
							ptr[2] = palette[ color ].rgbtBlue;		// r->b
							ptr[3] = 255;							// alpha
						}
					}
				}
			}

			break;
		}

		case 10:
		{
			unsigned char	packetHeader, packetSize;


			for( row = tgah->is_height - 1; row >= 0; row-- )
			{
				if( flipvert )
					ptr = &(*pixels)[ row * tgah->is_width * 4 ];

				for( col = 0; col < tgah->is_width; /* rien */ )
				{
					packetHeader	= *((unsigned char *)(pBuff++));
					packetSize		= 1 + (packetHeader & 0x7f);

					if( packetHeader & 0x80 )
					{
						// run-length packet
						switch( tgah->is_pixel_depth )
						{
							case 16:
							{
// RLE TRUE-COLOR BGR 16 BITS
								// read the current pixel
								unsigned short color = *((unsigned short *)pBuff);
								pBuff += sizeof( short );

								// convert bgr (16 bits) pixel into rgba (32 bits) pixel
								for( i = 0; i < packetSize; i++, ptr += 4, col++ )
								{
									ptr[0] = ((color & 0x7C00) >> 10) << 3;	// b->r
									ptr[1] = ((color & 0x03E0) >>  5) << 3;	// g->g
									ptr[2] = ((color & 0x001F) >>  0) << 3;	// r->b
									ptr[3] = 255;
								}

								break;
							}

							case 24:
							{
// RLE TRUE-COLOR BGR 24 BITS
								// convert bgr (24 bits) pixel into rgba (32 bits) pixel
								RGBTRIPLE *pix = (RGBTRIPLE *)pBuff;
								pBuff += sizeof( RGBTRIPLE );

								for( i = 0; i < packetSize; i++, ptr += 4, col++ )
								{
									ptr[0] = pix->rgbtRed;
									ptr[1] = pix->rgbtGreen;
									ptr[2] = pix->rgbtBlue;
									ptr[3] = 255;
								}

								break;
							}

							case 32:
							{
// RLE TRUE-COLOR BGR 32 BITS
								// convert bgr (32 bits) pixel into rgba (32 bits) pixel
								BGRAQUAD *pix = (BGRAQUAD *)pBuff;
								pBuff += sizeof( BGRAQUAD );

								for( i = 0; i < packetSize; i++, ptr += 4, col++ )
								{
									ptr[0] = pix->bgraRed;
									ptr[1] = pix->bgraGreen;
									ptr[2] = pix->bgraBlue;
									ptr[3] = pix->bgraAlpha;
								}

								break;
							}
						}
					}
					else
					{
						// non run-length packet

						for( i = 0; i < packetSize; i++, ptr += 4, col++ )
						{
							switch( tgah->is_pixel_depth )
							{
								case 16:
								{
// RLE TRUE-COLOR BGR 16 BITS
									// read the current pixel
									unsigned short color = *((unsigned short *)pBuff);
									pBuff += sizeof( short );

									// convert bgr (16 bits) pixel into rgba (32 bits) pixel
									ptr[0] = ((color & 0x7C00) >> 10) << 3;	// b->r
									ptr[1] = ((color & 0x03E0) >>  5) << 3;	// g->g
									ptr[2] = ((color & 0x001F) >>  0) << 3;	// r->b
									ptr[3] = 255;							// alpha

									break;
								}

								case 24:
								{
// RLE TRUE-COLOR BGR 24 BITS
									// convert bgr (24 bits) pixel into rgba (32 bits) pixel
									RGBTRIPLE *pix = (RGBTRIPLE *)pBuff;
									pBuff += sizeof( RGBTRIPLE );

									ptr[0] = pix->rgbtRed;
									ptr[1] = pix->rgbtGreen;
									ptr[2] = pix->rgbtBlue;
									ptr[3] = 255;

									break;
								}

								case 32:
								{
// RLE TRUE-COLOR BGR 32 BITS
									// convert bgr (32 bits) pixel into rgba (32 bits) pixel
									BGRAQUAD *pix = (BGRAQUAD *)pBuff;
									pBuff += sizeof( BGRAQUAD );

									ptr[0] = pix->bgraRed;
									ptr[1] = pix->bgraGreen;
									ptr[2] = pix->bgraBlue;
									ptr[3] = pix->bgraAlpha;

									break;
								}
							}
						}
					}
				}
			}

			break;
		}

		default:
		{
			// unknown format
			delete [] pixels;
			delete [] buffer;
			return 0;
		}
	}


	// free buffer memory
	delete [] buffer;

	// return success
	return 1;
}
