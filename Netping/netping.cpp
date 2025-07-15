#include <stdlib.h>
#include <stdio.h>
#include <windows.h>

int main (void)
{
    HWND hwnd = GetConsoleWindow();
    if (hwnd)
    {
        int i = 1;
        char orden[70] = { 0 };
        wchar_t titulo[24] = { 0 };
        MoveWindow(hwnd, 0, 400, 560, 240, TRUE);
        while (i)
        {
            wsprintfW(titulo, L".::: NetPing [%.3d] :::.", i);
            SetConsoleTitle(titulo);
            sprintf_s(orden, sizeof(orden), "cls & netheartbeat & timeout 300"); //5minutes refresh netHearbeat results
            system(orden);
            i++;
        }
    }
return 0;
}
