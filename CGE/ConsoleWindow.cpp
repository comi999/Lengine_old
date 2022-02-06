#include "ConsoleWindow.hpp"

ConsoleWindow::ConsoleHandle ConsoleWindow::s_ConsoleHandle;
ConsoleWindow::WindowHandle  ConsoleWindow::s_WindowHandle;
ConsoleWindow::WindowRegion  ConsoleWindow::s_WindowRegion;
tVector2< short >            ConsoleWindow::s_PixelSize;
wchar_t                      ConsoleWindow::s_Title[ 64 ];