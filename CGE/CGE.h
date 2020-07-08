#pragma once
#include <Windows.h>
#include <thread>
#include "Colour_Map.h"
#include "Timer.h"
#include "Math.h"
#include "Polygon.h"

class CGE
{
private:
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
public:
    float deltaTime = 1;
private:
    int frameTimes[10];
    char currentFrame = 0;

    std::thread* titleManager;
    
public:
    bool engineActive = true;
    tVector2<int> screenSize;
    Colour_Map colourMap;

    CGE(LPCWSTR title, const tVector2<int>& pixelSize, const tVector2<int>& screenSize);

    ~CGE();

public:
    void virtual Startup();
    
protected:
    void virtual Update();

public:
    void virtual Shutdown();

    void Run();

    void StartTimer();
    
    void UpdateTimer();

    void SetTitle(LPCWSTR title);

    void UpdateTitle();

public:
    void SetBuffer(Colour colour);
    void ResetBuffer();
    void DrawBuffer();

    void SetPixel(const tVector2<int>& position, const Colour& colour);
    void SetPixel(const Point2D& point);

    void DrawLine(Vector2 position1, Vector2 position2, const Colour& colour);
    void DrawLine(Line line, const Colour& colour);
    void DrawLineEx(Vector2 position1, Vector2 position2, const Colour& colour, int thickness);
    void DrawLineEx(Line line, const Colour& colour, int thickness);

    void DrawEdge(Edge2D edge);
    void DrawEdge(Edge3D edge);
    void DrawEdgeEx(Edge2D edge, int thickness);
    void DrawEdgeEx(Edge3D edge, int thickness);

    void DrawSquare(const Square& square);
    void DrawRect(const Rect& rect, float rotation, const Colour& colour);
    //void DrawRect

    void DrawTriangle();
    void DrawRectangle(const Rectangle2D& rect);
};

