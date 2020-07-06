#include <iostream>
#include <fstream>
#include <Windows.h>
#include "Colour_Map.h"

Colour_Map::Colour_Map()
{
    CONSOLE_CURSOR_INFO cursorInfo;
    GetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursorInfo);
    cursorInfo.bVisible = FALSE;
    SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursorInfo);

    
    colourCube = new WCHAR[256 * 256 * 256];

    bool fileFound = false;
    std::fstream mapFile;

    mapFile.open("colours.map", std::ios::binary | std::ios::in);
    if (mapFile.is_open())
    {
        std::cout << "Found colour map!";
        mapFile.read((char*)colourCube, 256 * 256 * 256 * sizeof(WCHAR));
        Sleep(1000);
    }
    else
    {
        mapFile.close();
        mapFile.open("colours.map", std::ios::binary | std::ios::out);

        for (int i = 0; i < 256 * 256 * 256; i++)
            colourCube[i] = 0x8000;

        for (int i = 0; i < 16; i++)
        {
            colourCube[consoleColours[i].r + consoleColours[i].g * 256 + consoleColours[i].b * 65536] = i;
            for (int j = i + 1; j < 16; j++)
            {
                for (int k = 1; k < 4; k++)
                {
                    Colour temp;
                    temp.r = (int)(consoleColours[i].r * k * 0.25f + consoleColours[j].r * (4 - k) * 0.25f);
                    temp.g = (int)(consoleColours[i].g * k * 0.25f + consoleColours[j].g * (4 - k) * 0.25f);
                    temp.b = (int)(consoleColours[i].b * k * 0.25f + consoleColours[j].b * (4 - k) * 0.25f);
                    colourCube[temp.r + temp.g * 256 + temp.b * 65536] = ((k << 8) | (i << 4) | j);
                    consoleColours[(int)(1.5f * (240 - (15 - i) * (16 - i) + 2 * (j - i - 1)) + k + 15)] = temp;
                }
            }
        }

        if (true)
        {
            for (int r = 0; r < 256; r += 8)
            {
                for (int g = 0; g < 256; g += 8)
                {
                    for (int b = 0; b < 256; b += 8)
                    {
                        std::cout << "Constructing new colour map: " << (int)((b + g * 256 + r * 65536) * 0.000005960464477539063f) << " % complete\r";
                        Colour currColour;
                        int diff = 195075;
                        for (Colour colour : consoleColours)
                        {
                            int newDiff = (r - colour.r) * (r - colour.r) + (g - colour.g) * (g - colour.g) + (b - colour.b) * (b - colour.b);
                            if (newDiff < diff)
                            {
                                currColour = colour;
                                diff = newDiff;
                            }
                        }
                        WCHAR b_currColour = colourCube[currColour.r + currColour.g * 256 + currColour.b * 65536];
                        for (int r_sub = r; r_sub < r + 8; r_sub++)
                        {
                            for (int g_sub = g; g_sub < g + 8; g_sub++)
                            {
                                for (int b_sub = b; b_sub < b + 8; b_sub++)
                                {
                                    colourCube[r_sub + g_sub * 256 + b_sub * 65536] = b_currColour;
                                }
                            }
                        }
                    }
                }
            }
        }

        for (int i = 0; i < 256 * 256 * 256; i++)
        {
            mapFile.write((char*)&colourCube[i], sizeof(WCHAR));
        }
    }
}

Colour_Map::~Colour_Map()
{
    delete[] colourCube;
}