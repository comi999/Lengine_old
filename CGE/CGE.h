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

class CGE
{
public:
    HWND consoleHwnd;
    HANDLE hSTDout;
    HANDLE hSTDin;
    CONSOLE_CURSOR_INFO cursorInfo;
    CHAR_INFO* charArray;
    Colour* pixelBuffer;
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
    Colour_Map colourMap;

    CGE(LPCWSTR title, const tVector2<int>& pixelSize, const tVector2<int>& screenSize);
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

    void DrawLine(Vector2 position1, Vector2 position2, const Colour& colour = { });
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

    void DrawOval(const Vector2& position, const Vector2& size, const Colour& colour = { });
    void DrawOval(const Oval& oval, const Colour& colour = { });
    void DrawOvalLine(const Vector2& position, const Vector2& size, const Colour& colour = { }, int thickness = 1);
    void DrawOvalLine(const Oval& oval, const Colour& colour = { }, int thickness = 1);

    void DrawRect(const Vector2& position, const Vector2& size, float rotation = 0, const Colour& colour = { });
    void DrawRect(const Rect& rect, float rotation = 0, const Colour& colour = { });
    void DrawRectLine(const Vector2& position, const Vector2& size, float rotation = 0, const Colour& colour = { }, int thickness = 1);
    void DrawRectLine(const Rect& rect, float rotation = 0, int thickness = 1, const Colour& colour = { });

    //Textured shape drawing
    void DrawRectangle(const Rectangle2D& rectangle, float rotation = 0);
    void DrawRectangleLine(const Triangle& triangle, float rotation = 0, int thickness = 1, const Colour& colour = { });
    void DrawRectangleTexture(const Triangle& source, const Triangle& dest, const Texture& texture, float sourceRot = 0, float destRot = 0);
    void DrawRectangleTexture(const vTriangle2D& triangle, const Texture& texture, float rotation = 0);

    void DrawTriangle(const Triangle& triangle, float rotation = 0, const Colour& colour = { });
    void DrawTriangle(const Triangle2D& triangle, float rotation = 0);
    void DrawTriangleLine(const Triangle& triangle, float rotation = 0, const Colour& colour = { }, int thickness = 1);
    void DrawTriangleTexture(const Triangle& source, const Triangle& dest, const Texture& texture, float sourceRot = 0, float destRot = 0);
    void DrawTriangleTexture(const vTriangle2D& triangle, const Texture& texture, float rotation = 0);

    template <int s>
    void DrawPoly(const Poly<s>& poly, float rotation = 0, const Colour& colour = { });
    template <int s>
    void DrawPolyLine(const Poly<s>& poly, float rotation = 0, const Colour & = { }, int thickness = 1);
    template <int s>
    void DrawShape(const Shape<s>& shape, float rotation = 0);
    template <int s>
    void DrawShapeTexture(const Poly<s>& source, const Poly<s>& dest, const Texture& texture, float sourceRot = 0, float destRot = 0);
    template <int s>
    void DrawShapeTexture(const vShape<s>& shape, const Texture& texture, float rotation = 0);
};
    


