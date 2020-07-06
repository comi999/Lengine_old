#include "CGE.h"

CGE::CGE(LPCWSTR title, const tVector2<int>& pixelSize, const tVector2<int>& screenSize)
{
    hSTDout = GetStdHandle(STD_OUTPUT_HANDLE);
    hSTDin = GetStdHandle(STD_INPUT_HANDLE);
    consoleHwnd = GetConsoleWindow();
    CONSOLE_FONT_INFOEX consoleFontInfo;
    consoleFontInfo.cbSize = sizeof(consoleFontInfo);
    consoleFontInfo.nFont = 0;
    consoleFontInfo.dwFontSize.X = pixelSize.i;
    consoleFontInfo.dwFontSize.Y = pixelSize.j;
    consoleFontInfo.FontFamily = FF_DONTCARE;
    consoleFontInfo.FontWeight = FW_NORMAL;
    wcscpy_s(consoleFontInfo.FaceName, L"Terminal");
    SetCurrentConsoleFontEx(hSTDout, 0, &consoleFontInfo);

    CONSOLE_SCREEN_BUFFER_INFOEX screenBufferInfo;
    screenBufferInfo.cbSize = sizeof(CONSOLE_SCREEN_BUFFER_INFOEX);
    GetConsoleScreenBufferInfoEx(hSTDout, &screenBufferInfo);
    for (int i = 0; i < 16; i++)
    {
        screenBufferInfo.ColorTable[i] = 
            colourMap.consoleColours[i].r + 
            colourMap.consoleColours[i].g * 256 + 
            colourMap.consoleColours[i].b * 65536;
    }
    SetConsoleScreenBufferInfoEx(hSTDout, &screenBufferInfo);

    COORD largestWindow = GetLargestConsoleWindowSize(hSTDout);
    if (largestWindow.X < screenSize.i)
        this->screenSize.i = largestWindow.X;
    else
        this->screenSize.i = screenSize.i;

    if (largestWindow.Y < screenSize.j)
        this->screenSize.j = largestWindow.Y;
    else
        this->screenSize.j = screenSize.j;

    SetConsoleScreenBufferSize(hSTDout, { (short)this->screenSize.i, (short)this->screenSize.j });
    windowArea = { 0, 0, (short)this->screenSize.i - 1, (short)this->screenSize.j - 1 };
    SetConsoleWindowInfo(hSTDout, TRUE, &windowArea);

    GetConsoleScreenBufferInfoEx(hSTDout, &screenBufferInfo);
    SetConsoleScreenBufferSize(hSTDout, {
        screenBufferInfo.srWindow.Right - screenBufferInfo.srWindow.Left + 1,
        screenBufferInfo.srWindow.Bottom - screenBufferInfo.srWindow.Top + 1 });

    DWORD newStyle = WS_CAPTION | DS_MODALFRAME | WS_MINIMIZEBOX | WS_SYSMENU;
    SetWindowLongW(consoleHwnd, GWL_STYLE, newStyle);
    SetWindowPos(consoleHwnd, NULL, 0, 0, 0, 0, SWP_FRAMECHANGED | SWP_NOSIZE | SWP_NOMOVE | SWP_NOZORDER | SWP_SHOWWINDOW);

    pixelBuffer = new Colour[this->screenSize.i * this->screenSize.j];
    charArray = new CHAR_INFO[this->screenSize.i * this->screenSize.j];
    ResetBuffer();

    StartTimer();

    titleManager = new std::thread([this]() 
        {
            while (engineActive) 
            { 
                UpdateTitle();
                Sleep(1000);
            } 
        });

    SetTitle(title);
}

CGE::~CGE()
{
    delete[] charArray;
    delete[] pixelBuffer;
    delete titleManager;
    delete gameTime;
}

void CGE::Startup() { }

void CGE::Shutdown() { }

void CGE::Update() { }

void CGE::Run()
{
    while (engineActive)
    {
        ResetBuffer();

        Update();

        DrawBuffer();

        UpdateTimer();
    }
}

void CGE::StartTimer()
{
    gameTime = new Timer();
    startTime = gameTime->elapsed() * 1000;
}

void CGE::UpdateTimer()
{
    endTime = gameTime->elapsed() * 1000;
    currentDelta = endTime - startTime;
    startTime = endTime;
    frameTimes[currentFrame] = currentDelta;
    currentFrame++;
    currentFrame %= 10;
    deltaTime = 0;
    for (int i = 0; i < 10; i++)
        deltaTime += frameTimes[i];
    deltaTime *= 0.1f;
}

void CGE::SetTitle(LPCWSTR title)
{
    windowTitleLength = 0;
    while (title[windowTitleLength] != '\0' && windowTitleLength < 48)
    {
        windowTitle[windowTitleLength] = title[windowTitleLength];
        windowTitleLength++;
    }
    char fpsText[10] = "    FPS: ";
    for (int i = 0; i < 10; i++)
    {
        windowTitle[i + windowTitleLength] = fpsText[i];
    }
    windowTitleLength += 9;
    SetWindowText(consoleHwnd, windowTitle);
}

void CGE::UpdateTitle()
{
    int fps = 1000 / max(deltaTime, 1);
    char fpsText[5];
    sprintf_s(fpsText, "%d", fps);
    for (int i = 0; i < 5; i++)
    {
        windowTitle[i + windowTitleLength] = fpsText[i];
    }
    SetWindowText(consoleHwnd, windowTitle);
}

void CGE::DrawBuffer()
{
    WriteConsoleOutput(hSTDout, charArray, { (short)screenSize.i, (short)screenSize.j }, { 0, 0 }, &windowArea);
}

void CGE::ResetBuffer()
{
    ZeroMemory(charArray, sizeof(CHAR_INFO) * screenSize.i * screenSize.j);

    Colour c;
    int screenArea = screenSize.i * screenSize.j;
    for (int i = 0; i < screenArea; i++)
        memcpy(pixelBuffer + i, &c, sizeof(Colour));
}

void CGE::SetPixel(const tVector2<int>& position, const Colour& colour)
{
    if (position.i < 0 || position.i >= screenSize.i ||
        position.j < 0 || position.j >= screenSize.j)
        return;

    if (colour.a == 0)
        return;
    
    Colour newColour;
    if (colour.a == 255)
        newColour = colour;
    else
        newColour = colour + pixelBuffer[screenSize.i * position.j + position.i];

    pixelBuffer[screenSize.i * position.j + position.i] = newColour;
    CHAR_INFO& pixel = charArray[screenSize.i * (screenSize.j - position.j - 1) + position.i];
    pixel.Attributes = colourMap.colourCube[newColour.r + newColour.g * 256 + newColour.b * 65536] & 0xFF;
    switch (colourMap.colourCube[newColour.r + newColour.g * 256 + newColour.b * 65536] >> 8)
    {
    case 0:
        pixel.Char.UnicodeChar = L'\x2588';
        break;
    case 1:
        pixel.Char.UnicodeChar = L'\x2593';
        break;
    case 2:
        pixel.Char.UnicodeChar = L'\x2592';
        break;
    case 3:
        pixel.Char.UnicodeChar = L'\x2591';
        break;
    }
}

void CGE::SetBuffer(Colour colour)
{
    for (int h = 0; h < screenSize.j; h++)
    {
        for (int w = 0; w < screenSize.i; w++)
            SetPixel({ w, h }, colour);
    }
}

void CGE::DrawLine(tVector2<int> position1, tVector2<int> position2, const Colour& colour)
{
    short dx = abs(position2.i - position1.i);
    short sx = position1.i < position2.i ? 1 : -1;
    short dy = -abs(position2.j - position1.j);
    short sy = position1.j < position2.j ? 1 : -1;
    short err = dx + dy;
    while (true)
    {
        SetPixel(position1, colour);
        if (position1.i == position2.i && position1.j == position2.j) break;
        short e2 = 2 * err;
        if (e2 >= dy)
        {
            err += dy;
            position1.i += sx;
        }
        if (e2 <= dx)
        {
            err += dx;
            position1.j += sy;
        }
    }
}

void CGE::DrawRectangle(const Rectangle2D& rectangle)
{
    if (rectangle.theta == 0)
    {
        for (int h = rectangle.rect.y; h < rectangle.rect.y + rectangle.rect.h; h++)
        {
            for (int w = rectangle.rect.x; w < rectangle.rect.x + rectangle.rect.w; w++)
                SetPixel({ w, h }, rectangle.colour);
        }
    }

}
