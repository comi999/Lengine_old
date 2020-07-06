#include "CGE.h"

void main()
{
	CGE game(L"test", { 4, 4 }, { 200, 200 });

	Rectangle2D a;
	Rectangle2D b;
	Rectangle2D c;
	a.colour = { 0, 0, 255 };
	b.colour = { 250, 0, 0, 128 };
	c.colour = { 0, 255, 0, 230 };

	for (int i = 0; i < 150; i++)
	{
		game.ResetBuffer();
		a.rect = { (float)100, (float)90, (float)30, (float)30 };
		b.rect = { (float)i, (float)110, (float)30, (float)30 };
		c.rect = { (float)100, (float)i, (float)30, (float)30 };
		game.DrawRectangle(a);
		game.DrawRectangle(b);
		game.DrawRectangle(c);
		game.DrawBuffer();
		Sleep(16);
		if (i == 149)
			i = 0;
	}

	

	while (true);
}