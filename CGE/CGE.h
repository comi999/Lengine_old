#pragma once
#include <Windows.h>
#include <thread>
#include <string>
#include "Colour_Map.h"
#include "Timer.h"
#include "Math.h"
#include "Polygon.h"
#include "Image.h"
#include "Texture.h"
#include "Sprite.h"
#include "Screen_Buffer.h"

class CGE
{
public:
    HWND consoleHwnd;
    HANDLE hSTDout;
    HANDLE hSTDin;
    CONSOLE_CURSOR_INFO cursorInfo;
    SMALL_RECT windowArea;
    wchar_t windowTitle[64];
    char windowTitleLength = 0;
    Timer* gameTime;
    int startTime = 0, endTime = 0, currentDelta = 1;
    float deltaTime = 1;
    int frameTimes[10];
    char currentFrame = 0;std::thread* titleManager;
    bool engineActive = true;
    tVector2<int> screenSize;
    bool thirdDimension;
    Colour_Map colourMap;
    Screen_Buffer screenBuffer;

    CGE(LPCWSTR title, const tVector2<int>& pixelSize, const tVector2<int>& screenSize, bool thirdDimension = false);
    ~CGE();

    void virtual Startup();
    void virtual Update();
    void virtual Shutdown();
    void Run();

    void StartTimer();
    void UpdateTimer();

    void SetTitle(LPCWSTR title);
    void UpdateTitle();

    void SetBuffer(Colour colour);
    void ResetBuffer();
    void DrawBuffer();

    void SetPixel(const tVector2<int>& position, const Colour& colour = { });
    void SetPixel(const Point2D& point);

    void DrawLine(tVector2<int> position1, tVector2<int> position2, const Colour& colour = { });
    void DrawLine(Line line, const Colour& colour = { });
    void DrawLineEx(Vector2 position1, Vector2 position2, const Colour& colour = { }, int thickness = 1);
    void DrawLineEx(Line line, const Colour& colour = { }, int thickness = 1);

    void DrawEdge(Edge2D edge);
    void DrawEdgeEx(Edge2D edge, int thickness = 1);
    //Zone drawing functions
    void DrawCircle(const Vector2& position, float radius, const Colour& colour = { });
    void DrawCircle(const Circle& circle, const Colour& colour = { });
    void DrawCircleLine(const Vector2& position, float radius, const Colour& colour = { }, int thickness = 1);
    void DrawCircleLine(const Circle& circle, const Colour& colour = { }, int thickness = 1);

    void DrawOval(const Vector2& position, const Vector2& size, float rotation = 0, const Colour& colour = { });
    void DrawOval(const Oval& oval, float rotation = 0, const Colour& colour = { });
    void DrawOvalLine(const Vector2& position, const Vector2& size, float rotation = 0, const Colour& colour = { }, int thickness = 1);
    void DrawOvalLine(const Oval& oval, float rotation = 0, const Colour& colour = { }, int thickness = 1);

    void DrawRect(const Vector2& position, const Vector2& size, float rotation = 0, const Colour& colour = { });
    void DrawRect(const Rect& rect, float rotation = 0, const Colour& colour = { });
    void DrawRectLine(const Vector2& position, const Vector2& size, float rotation = 0, const Colour& colour = { }, int thickness = 1);
    void DrawRectLine(const Rect& rect, float rotation = 0, const Colour& colour = { }, int thickness = 1);

    //Textured shape drawing
    void DrawRectangle(const Rectangle2D& rectangle, float rotation = 0);
    void DrawRectangleLine(const Triangle& triangle, float rotation = 0, int thickness = 1, const Colour& colour = { });
    void DrawRectangleTexture(const Triangle& source, const Triangle& dest, const Texture& texture, float sourceRot = 0, float destRot = 0);
    void DrawRectangleTexture(const vTriangle2D& triangle, const Texture& texture, float rotation = 0);

    void DrawTriangle(const Vector2& p0, const Vector2& p1, const Vector2& p2, const Colour& colour = { });
    void DrawTriangle(const Triangle& triangle, float rotation = 0, const Colour& colour = { });
    void DrawTriangle(const Triangle2D& triangle, float rotation = 0);
    void DrawTriangleLine(const Triangle& triangle, float rotation = 0, const Colour& colour = { }, int thickness = 1);
    void DrawTriangleTexture(const Triangle& source, const Triangle& dest, const Texture& texture, float sourceRot = 0, float destRot = 0);
    void DrawTriangleTexture(const vTriangle2D& triangle, const Texture& texture, float rotation = 0);

    template <int s>
    void DrawPoly(const Poly<s>& poly, float rotation = 0, const Colour& colour = { });
    template <int s>
    void DrawPolyLine(const Poly<s>& poly, float rotation = 0, const Colour& colour = { }, int thickness = 1);
    template <int s>
    void DrawShape(const Shape<s>& shape, float rotation = 0);
    template <int s>
    void DrawShapeTexture(const Poly<s>& source, const Poly<s>& dest, const Texture& texture, float sourceRot = 0, float destRot = 0);
    template <int s>
    void DrawShapeTexture(const vShape<s>& shape, const Texture& texture, float rotation = 0);
};

template <int s>
void CGE::DrawPoly(const Poly<s>& poly, float rotation, const Colour& colour)
{
    //NEEDS WORK
}
template <int s>
void CGE::DrawPolyLine(const Poly<s>& poly, float rotation, const Colour& colour, int thickness)
{
    //NEEDS WORK
}
template <int s>
void CGE::DrawShape(const Shape<s>& shape, float rotation)
{
    //NEEDS WORK
}
template <int s>
void CGE::DrawShapeTexture(const Poly<s>& source, const Poly<s>& dest, const Texture& texture, float sourceRot, float destRot)
{
    //NEEDS WORK
}
template <int s>
void CGE::DrawShapeTexture(const vShape<s>& shape, const Texture& texture, float rotation)
{
    //NEEDS WORK
}