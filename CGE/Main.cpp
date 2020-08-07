#include "CGE.h"

void main()
{
	CGE game(L"test", { 4, 4 }, { 200, 200 });
	game.StartTimer();
	float f = 0;
	Rect rect;
	rect.position = { 100, 100 };
	rect.size = { 40, 25 };
	while (true)
	{
		f += 0.1f;
		game.ResetBuffer();
		
		game.DrawTriangle({ 30, 150 }, { 50, 100 }, { 180, 20 }, GREEN);
		game.DrawRect({ 100, 80 }, { 40, 50 }, f, { 128, 255, 0, 128 });
		game.DrawRect({ 100, 100 }, { 40, 50 }, f, { 255, 100, 0, 128 });
		game.DrawRect({ 100, 120 }, { 40, 50 }, f, { 128, 0, 255, 128 });

		

		game.DrawBuffer();
		game.UpdateTimer();
	}
}