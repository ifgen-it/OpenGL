class BMPClass
{
public:	
	int sizeX, sizeY;
	BYTE* data;	// OpenGL formatted pixels
	
	BMPClass() { data=0; }
	~BMPClass() { delete[] data; };

	BYTE& Pixel (int x, int y, int c)
	{
		return data[(y * sizeX + x) * 3 + c];
	}
	void AllocateMem()
	{
		delete[] data;
		data = new BYTE[sizeX * sizeY * 3];
	}
	char BMPLoad (string fname)
	{
		if (sizeof(int) != 4)
			return 'i';
		
		FILE*f;
		fopen_s(&f, fname.c_str(),"rb"); // open for reading in binary mode
		if (!f)
			return 'o';
		
		char header[54];
		fread(header,54,1,f);			// read the 54bit main header

		if (header[0]!='B' || header[1]!='M') 
		{
			fclose(f);
			return 'b';		//all bitmaps should start "BM"
		}

		//it seems gimp sometimes makes its headers small, so we have to do this. hence all the fseeks
		int offset = *(unsigned int*)(header+10);
	
		sizeX = *(int*)(header+18);
		sizeY = *(int*)(header+22);
		//now the bitmap knows how big it is it can allocate its memory
		AllocateMem();

		int bits = int(header[28]);		//colourdepth

		BYTE cols[256 * 4];				//colourtable
		switch(bits)
		{
		case 24:
			fseek (f, offset, SEEK_SET);
			fread (data, sizeX * sizeY * 3, 1, f);	//24bit is easy
			for(int x=0; x < sizeX * sizeY * 3; x += 3)	//except the format is BGR, grr
			{
				BYTE temp = data[x];
				data[x] = data[x+2];
				data[x+2] = temp;
			}
			break;

		case 8:
			fread (cols, 256 * 4, 1, f);	//read colortable
			fseek (f, offset, SEEK_SET);
			for (int y=0; y < sizeY; ++y)	//(Notice 4bytes/col for some reason)
			for (int x=0; x < sizeX; ++x)
			{
				BYTE byte;			
				fread (&byte, 1, 1, f);			//just read byte					
				for (int c=0; c<3; ++c)
					Pixel(x,y,c) = cols[byte * 4 + 2 - c];	//and look up in the table
			}
			break;

		case 4:
			fread (cols, 16 * 4, 1, f);
			fseek (f, offset, SEEK_SET);
			for (int y=0; y<256; ++y)
			{
				for (int x=0; x<256; x+=2)
				{
					BYTE byte;
					fread (&byte, 1, 1, f);			//as above, but need to exract two
					for (int c=0; c<3; ++c)			//pixels from each byte
						Pixel(x,y,c) = cols[byte/16*4+2-c];
					for (int c=0; c<3; ++c)
						Pixel(x+1,y,c)=cols[byte%16*4+2-c];
				}
			}
			break;

		case 1:
			fread(cols,8,1,f);
			fseek(f,offset,SEEK_SET);
			for (int y=0; y < sizeY; ++y)
			{
				for (int x=0; x < sizeX; x+=8)
				{
					BYTE byte;
					fread (&byte, 1, 1, f);
					// Every byte is eight pixels so I'm shifting the byte to the relevant position, then masking out
					// all but the lowest bit in order to get the index into the colourtable.
					for (int x2=0; x2<8; ++x2)
					for (int c=0; c<3; ++c)
						Pixel(x+x2, y, c) = cols[((byte >> (7-x2)) & 1) * 4 + 2 - c];
				}
			}
			break;

		default: fclose(f);
			return 'u';
		}

		if (ferror(f))
		{
			fclose(f);
			return 'f';
		}
	
		fclose(f);

		return '\0';
	}

	void BMPLoadFromBuffer (BYTE* buf, int w, int h)
	{
		sizeX = w;
		sizeY = h;
		//now the bitmap knows how big it is it can allocate its memory
		AllocateMem();

		int bits = 24;		//colourdepth

		data = buf;
	}

	string TranslateBMPError(char err)
	{
		switch(err)
		{
		case 'b': return "This file is not a bitmap, specifically it doesn't start 'BM'";
		case 'o': return "Failed to open the file, suspect it doesn't exist";
		case 'f': return "ferror said we had an error. This error seems to not always mean anything, try ignoring it";
		case 'i': return "sizeof(int)!=4 quite a lot of rewriting probably needs to be done on the code";
		case '\0': return "No errors detected";
		case 'u': return "Unknown bmp format, ie not 24bit, 256,16 or 2 colour";
		default: return "Not a valid error code";
		}
	}
};
/*BMPLoader - loads Microsoft .bmp format
    Copyright (C) 2006  Chris Backhouse

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.


  cjbackhouse@hotmail.com 		www.backhouse.tk
  
  I would appreciate it if anyone using this in something cool would tell me
  so I can see where it ends up.

  Takes a filename, returns an array of RGB pixel data
  Loads:
  24bit bitmaps
  256 colour bitmaps
  16 colour bitmaps
  2 colour bitmaps  (Thanks to Charles Rabier)

  This code is designed for use in openGL programs, so bitmaps not correctly padded will not
  load properly, I believe this only applies to: 
  256cols if sizeX is not a multiple of 4
  16cols if sizeX is not a multiple of 8
  2cols if sizeX is not a multiple of 32

  Sample code:

	BMPClass bmp;
	BMPLoad(fname,bmp);
	glTexImage2D(GL_TEXTURE_2D,0,3,bmp.sizeX,bmp.sizeY,0,GL_RGB,GL_UNSIGNED_BYTE,bmp.data);
*/