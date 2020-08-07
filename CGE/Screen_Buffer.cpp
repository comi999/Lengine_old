#include "Screen_Buffer.h"
#include "Colour.h"

Screen_Buffer::Screen_Buffer()
{

}

Screen_Buffer::~Screen_Buffer()
{
	delete[] charBuffer;
	delete[] pixelBuffer;
	delete[] edgeBuffer;
	delete[] depthBuffer;
}

void Screen_Buffer::InitialiseBuffer(const tVector2<int>& screenSize)
{
	int screenArea = screenSize.i * screenSize.j;
	charBuffer = new CHAR_INFO[screenArea];
	pixelBuffer = new Colour[screenArea];
	edgeBuffer = new int[screenSize.j];
	depthBuffer = new float[screenArea];
}

void Screen_Buffer::ResetCharBuffer(int screenArea)
{
	ZeroMemory(charBuffer, sizeof(CHAR_INFO) * screenArea);
}

void Screen_Buffer::ResetPixelBuffer(int screenArea)
{
	Colour colour;
	for (int i = 0; i < screenArea; i++) 
		memcpy(pixelBuffer + i, &colour, sizeof(Colour));
}

void Screen_Buffer::ResetEdgeBuffer(int screenHeight)
{
	ZeroMemory(edgeBuffer, sizeof(float) * screenHeight);
}

void Screen_Buffer::ResetDepthBuffer(int screenArea)
{
	ZeroMemory(depthBuffer, sizeof(float) * screenArea);
}

void Screen_Buffer::SetCharBuffer(int screenArea, CHAR_INFO pixel)
{
	for (int i = 0; i < screenArea; i++)
		memcpy(charBuffer + i, &pixel, sizeof(CHAR_INFO));
}

void Screen_Buffer::SetPixelBuffer(int screenArea, Colour colour)
{
	for (int i = 0; i < screenArea; i++)
		memcpy(pixelBuffer + i, &colour, sizeof(Colour));
}

void Screen_Buffer::SetEdgeBuffer(int screenHeight, int column)
{
	for (int i = 0; i < screenHeight; i++)
		memcpy(edgeBuffer + i, &column, sizeof(int));
}

void Screen_Buffer::SetDepthBuffer(int screenArea, float depth)
{
	for (int i = 0; i < screenArea; i++)
		memcpy(depthBuffer + i, &depth, sizeof(float));
}

void Screen_Buffer::ResetBuffer2D(const tVector2<int>& screenSize)
{
	int screenArea = screenSize.i * screenSize.j;

	ResetCharBuffer(screenArea);
	ResetPixelBuffer(screenArea);
	ResetEdgeBuffer(screenSize.j);
}

void Screen_Buffer::ResetBuffer3D(const tVector2<int>& screenSize)
{
	int screenArea = screenSize.i * screenSize.j;

	ResetCharBuffer(screenArea);
	ResetPixelBuffer(screenArea);
	ResetEdgeBuffer(screenSize.j);
	ResetDepthBuffer(screenArea);
}