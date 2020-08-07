#pragma once
#include <windows.h>
#include "Math.h"

class Colour;

class Screen_Buffer
{
public:
	Screen_Buffer();
	~Screen_Buffer();

	void InitialiseBuffer(const tVector2<int>& screenSize);

	void ResetCharBuffer(int screenArea);
	void ResetPixelBuffer(int screenArea);
	void ResetEdgeBuffer(int screenHeight);
	void ResetDepthBuffer(int screenArea);

	void SetCharBuffer(int screenArea, CHAR_INFO pixel);
	void SetPixelBuffer(int screenArea, Colour colour);
	void SetEdgeBuffer(int screenHeight, int column);
	void SetDepthBuffer(int screenArea, float depth);

	void ResetBuffer2D(const tVector2<int>& screenSize);
	void ResetBuffer3D(const tVector2<int>& screenSize);

	CHAR_INFO* charBuffer;
	Colour* pixelBuffer;
	int* edgeBuffer;
	float* depthBuffer;
};

