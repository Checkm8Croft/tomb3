#include "../tomb3/pch.h"
#include "picture.h"
#include "hwrender.h"
#include "dxshell.h"
#include "time.h"
#include "texture.h"
#include "file.h"
#include "../3dsystem/3d_gen.h"
#include "winmain.h"
#include "../game/camera.h"
#include "../tomb3/tomb3.h"
#include "../global/types.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <GL/gl.h>

long OldPicTexIndices[5];
long CurPicTexIndices[5];
long nLoadedPictures;
bool forceFadeDown;
bool pictureFading;
bool dontFadePicture;
bool bDontGreyOut;

bool LoadPicture(const char* name, GLuint* textureID, int* width, int* height);
void ConvertSurfaceToTextures(GLuint textureID);
void SetPictureToFade(long fade)
{
	pictureFading = fade;
}

void ForceFadeDown(long fade)
{
	forceFadeDown = fade;
}

void DoInventoryPicture()
{
	glDisable(GL_DEPTH_TEST);
	TRDrawPicture(0, CurPicTexIndices, f_zfar);
}

void FadePictureUp(long steps)
{
	if (nLoadedPictures > 1)
		CrossFadePicture();
	else
	{
		for (int i = 0; i < steps; i++)
		{
			HWR_BeginScene();
			TRDrawPicture(255 - (i * (256 / steps)), CurPicTexIndices, f_znear);
			HWR_EndScene();
			DXUpdateFrame(1, 0);
		}
	}

	HWR_BeginScene();
	TRDrawPicture(0, CurPicTexIndices, f_znear);
	HWR_EndScene();
	DXUpdateFrame(1, 0);
	TIME_Init();
}

void FadePictureDown(long steps)
{
	if (forceFadeDown)
	{
		for (int i = 0; i < steps; i++)
		{
			HWR_BeginScene();
			TRDrawPicture((256 % steps) - (i * (-256 / steps)), CurPicTexIndices, f_znear);
			HWR_EndScene();
			DXUpdateFrame(1, 0);
		}

		HWR_BeginScene();
		TRDrawPicture(255, CurPicTexIndices, f_znear);
		HWR_EndScene();
		DXUpdateFrame(1, 0);
		forceFadeDown = 0;
		nLoadedPictures = 0;
		FreePictureTextures(CurPicTexIndices);
		TIME_Init();
	}
	else
	{
		HWR_BeginScene();
		TRDrawPicture(0, CurPicTexIndices, f_znear);
		HWR_EndScene();
		DXUpdateFrame(1, 0);
	}
}

void CrossFadePicture()
{
	for (int i = 0, j = 255; i < 256; i += 8, j -= 8)
	{
		HWR_BeginScene();
		DrawPictureAlpha(i, CurPicTexIndices, f_znear);
		DrawPictureAlpha(j, OldPicTexIndices, f_zfar);
		HWR_EndScene();
		DXUpdateFrame(1, 0);
	}

	FreePictureTextures(CurPicTexIndices);
	memcpy(CurPicTexIndices, OldPicTexIndices, sizeof(OldPicTexIndices));
	
	glDisable(GL_BLEND);
	glDisable(GL_ALPHA_TEST);

	if (dontFadePicture)
		pictureFading = 0;
	else
		pictureFading = 1;

	HWR_BeginScene();
	TRDrawPicture(0, CurPicTexIndices, f_znear);
	HWR_EndScene();
	DXUpdateFrame(1, 0);
	TIME_Init();
}

void S_FadePicture()
{
	static long pos, max = 64;
	long nframes;

	if (!pictureFading || dontFadePicture)
		return;

	nframes = camera.number_frames;

	if (nframes > TICKS_PER_FRAME * 5)
		nframes = TICKS_PER_FRAME;

	DrawPictureAlpha(256 - 256 / max * (max - pos), CurPicTexIndices, f_znear);
	pos += nframes;

	if (pos >= max)
	{
		pos = 0;
		pictureFading = 0;
		nLoadedPictures = 0;
		FreePictureTextures(CurPicTexIndices);
	}
}

void S_FadeToBlack()
{
	// In OpenGL, we capture the screen by reading pixels
	GLint viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);
	
	int width = viewport[2];
	int height = viewport[3];
	
	GLubyte* pixels = new GLubyte[width * height * 4];
	glReadPixels(viewport[0], viewport[1], width, height, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
	
	// Create texture from screen capture
	GLuint screenTexture;
	glGenTextures(1, &screenTexture);
	glBindTexture(GL_TEXTURE_2D, screenTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	
	delete[] pixels;
	
	// Store texture ID in CurPicTexIndices
	CurPicTexIndices[0] = screenTexture;
	nLoadedPictures++;
	TIME_Init();
}

bool LoadPicture(const char* name, GLuint* textureID, int* width, int* height)
{
    SDL_Surface* loadedSurface = IMG_Load(name);
    if (!loadedSurface)
    {
        return false;
    }

    // Convert to RGBA format
    SDL_Surface* convertedSurface = SDL_ConvertSurfaceFormat(
        loadedSurface, SDL_PIXELFORMAT_RGBA32, 0
    );
    
    SDL_FreeSurface(loadedSurface);
    
    if (!convertedSurface)
    {
        return false;
    }

    // Create OpenGL texture
    glGenTextures(1, textureID);
    glBindTexture(GL_TEXTURE_2D, *textureID);
    
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 
                 convertedSurface->w, convertedSurface->h, 0,
                 GL_RGBA, GL_UNSIGNED_BYTE, convertedSurface->pixels);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    *width = convertedSurface->w;
    *height = convertedSurface->h;
    
    SDL_FreeSurface(convertedSurface);
    
    return true;
}

void FreePictureTextures(long* indices)
{
	for (int i = 0; i < 5; i++)
	{
		if (indices[i] != 0)
		{
			glDeleteTextures(1, (GLuint*)&indices[i]);
			indices[i] = 0;
		}
	}

	HWR_GetAllTextureHandles();

	for (int i = 0; i < nTextures; i++)
		HWR_SetCurrentTexture(TexturePtrs[i]);
}

void CreateMonoScreen()
{
	if (bDontGreyOut)
	{
		if (tomb3.psx_mono)
			glDisable(GL_COLOR_MATERIAL);
		else
			glEnable(GL_COLOR_MATERIAL);

		bDontGreyOut = 0;
	}
	else
		glEnable(GL_COLOR_MATERIAL);

	// Capture screen as in S_FadeToBlack
	GLint viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);
	
	int width = viewport[2];
	int height = viewport[3];
	
	GLubyte* pixels = new GLubyte[width * height * 4];
	glReadPixels(viewport[0], viewport[1], width, height, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
	
	// Create texture from screen capture
	GLuint screenTexture;
	glGenTextures(1, &screenTexture);
	glBindTexture(GL_TEXTURE_2D, screenTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	
	delete[] pixels;
	
	// Store texture ID in CurPicTexIndices
	CurPicTexIndices[0] = screenTexture;
	
	HWR_GetAllTextureHandles();

	for (int i = 0; i < nTextures; i++)
		HWR_SetCurrentTexture(TexturePtrs[i]);

	glDisable(GL_COLOR_MATERIAL);
	nLoadedPictures++;
	TIME_Init();
}

void DrawMonoScreen(long r, long g, long b)
{
	long x[4];
	long y[4];
	static long screenX[4] = { 0, 256, 512, 640 };
	static long screenY[3] = { 0, 256, 480 };
	long col;

	if (tomb3.psx_mono)
	{
		r <<= 1;
		g <<= 1;
		b <<= 1;

		col = RGBA(r, g, b, 0xFF);

		for (int i = 0; i < 3; i++)
		{
			x[i] = phd_winxmin + phd_winwidth * screenX[i] / 640;
			y[i] = phd_winymin + phd_winheight * screenY[i] / 480;
		}

		x[3] = phd_winxmin + phd_winwidth * screenX[3] / 640;

		glDisable(GL_BLEND);
		DrawTile(x[0], y[0], x[1] - x[0], y[1] - y[0], CurPicTexIndices[0], 0, 0, 256, 256, col, col, col, col, f_zfar);
		DrawTile(x[1], y[0], x[2] - x[1], y[1] - y[0], CurPicTexIndices[1], 0, 0, 256, 256, col, col, col, col, f_zfar);
		DrawTile(x[2], y[0], x[3] - x[2], y[1] - y[0], CurPicTexIndices[2], 0, 0, 128, 256, col, col, col, col, f_zfar);
		DrawTile(x[0], y[1], x[1] - x[0], y[2] - y[1], CurPicTexIndices[3], 0, 0, 256, 224, col, col, col, col, f_zfar);
		DrawTile(x[1], y[1], x[2] - x[1], y[2] - y[1], CurPicTexIndices[4], 0, 0, 256, 224, col, col, col, col, f_zfar);
		DrawTile(x[2], y[1], x[3] - x[2], y[2] - y[1], CurPicTexIndices[2], 128, 0, 128, 224, col, col, col, col, f_zfar);
	}
	else
		TRDrawPicture(0, CurPicTexIndices, f_zfar);
}

void RemoveMonoScreen(long fade)
{
	if (fade)
	{
		nLoadedPictures = 1;
		FadePictureDown(32);
	}
	else
	{
		FreePictureTextures(CurPicTexIndices);
		nLoadedPictures = 0;
	}
}

void ConvertSurfaceToTextures(GLuint textureID)
{
    // In OpenGL, textures are already ready to use
    glBindTexture(GL_TEXTURE_2D, textureID);
}

void DrawTile(long x, long y, long w, long h, long tpage, long tU, long tV, long tW, long tH, long c0, long c1, long c2, long c3, float z)
{
	float u1, v1, u2, v2;

	u1 = float(tU * (1.0F / 256.0F));
	v1 = float(tV * (1.0F / 256.0F));
	u2 = float((tW + tU) * (1.0F / 256.0F));
	v2 = float((tH + tV) * (1.0F / 256.0F));

	glBindTexture(GL_TEXTURE_2D, tpage);
	glDisable(GL_DEPTH_TEST);
	
	glBegin(GL_QUADS);
		glColor4ub((c0 >> 16) & 0xFF, (c0 >> 8) & 0xFF, c0 & 0xFF, (c0 >> 24) & 0xFF);
		glTexCoord2f(u1, v1);
		glVertex3f((float)x, (float)y, z);
		
		glColor4ub((c1 >> 16) & 0xFF, (c1 >> 8) & 0xFF, c1 & 0xFF, (c1 >> 24) & 0xFF);
		glTexCoord2f(u2, v1);
		glVertex3f(float(w + x), (float)y, z);
		
		glColor4ub((c3 >> 16) & 0xFF, (c3 >> 8) & 0xFF, c3 & 0xFF, (c3 >> 24) & 0xFF);
		glTexCoord2f(u2, v2);
		glVertex3f(float(w + x), float(h + y), z);
		
		glColor4ub((c2 >> 16) & 0xFF, (c2 >> 8) & 0xFF, c2 & 0xFF, (c2 >> 24) & 0xFF);
		glTexCoord2f(u1, v2);
		glVertex3f((float)x, float(h + y), z);
	glEnd();
}

void DrawPictureAlpha(long col, long* indices, float z)
{
	long x[4];
	long y[4];
	static long screenX[4] = { 0, 256, 512, 640 };
	static long screenY[3] = { 0, 256, 480 };
	long maxcol;

	if (tomb3.psx_contrast)
		maxcol = 0x808080;
	else
		maxcol = 0xFFFFFF;

	col = (0xFF000000 * (col + 1)) | maxcol;

	for (int i = 0; i < 3; i++)
	{
		x[i] = phd_winxmin + phd_winwidth * screenX[i] / 640;
		y[i] = phd_winymin + phd_winheight * screenY[i] / 480;
	}

	x[3] = phd_winxmin + phd_winwidth * screenX[3] / 640;

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	DrawTile(x[0], y[0], x[1] - x[0], y[1] - y[0], indices[0], 0, 0, 256, 256, col, col, col, col, z);
	DrawTile(x[1], y[0], x[2] - x[1], y[1] - y[0], indices[1], 0, 0, 256, 256, col, col, col, col, z);
	DrawTile(x[2], y[0], x[3] - x[2], y[1] - y[0], indices[2], 0, 0, 128, 256, col, col, col, col, z);
	DrawTile(x[0], y[1], x[1] - x[0], y[2] - y[1], indices[3], 0, 0, 256, 224, col, col, col, col, z);
	DrawTile(x[1], y[1], x[2] - x[1], y[2] - y[1], indices[4], 0, 0, 256, 224, col, col, col, col, z);
	DrawTile(x[2], y[1], x[3] - x[2], y[2] - y[1], indices[2], 128, 0, 128, 224, col, col, col, col, z);
	glDisable(GL_BLEND);
}

void TRDrawPicture(long col, long* indices, float z)
{
	long x[4];
	long y[4];
	static long screenX[4] = { 0, 256, 512, 640 };
	static long screenY[3] = { 0, 256, 480 };

	col = 255 - col;

	if (tomb3.psx_contrast)
		col >>= 1;

	col = RGBA(col, col, col, 0xFF);

	for (int i = 0; i < 3; i++)
	{
		x[i] = phd_winxmin + phd_winwidth * screenX[i] / 640;
		y[i] = phd_winymin + phd_winheight * screenY[i] / 480;
	}

	x[3] = phd_winxmin + phd_winwidth * screenX[3] / 640;

	glDisable(GL_BLEND);
	DrawTile(x[0], y[0], x[1] - x[0], y[1] - y[0], indices[0], 0, 0, 256, 256, col, col, col, col, z);
	DrawTile(x[1], y[0], x[2] - x[1], y[1] - y[0], indices[1], 0, 0, 256, 256, col, col, col, col, z);
	DrawTile(x[2], y[0], x[3] - x[2], y[1] - y[0], indices[2], 0, 0, 128, 256, col, col, col, col, z);
	DrawTile(x[0], y[1], x[1] - x[0], y[2] - y[1], indices[3], 0, 0, 256, 224, col, col, col, col, z);
	DrawTile(x[1], y[1], x[2] - x[1], y[2] - y[1], indices[4], 0, 0, 256, 224, col, col, col, col, z);
	DrawTile(x[2], y[1], x[3] - x[2], y[2] - y[1], indices[2], 128, 0, 128, 224, col, col, col, col, z);
}