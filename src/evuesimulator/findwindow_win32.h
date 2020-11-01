#ifndef FINDWINDOW_H
#define FINDWINDOW_H


#include <Windows.h>
#include <TlHelp32.h>
#include <winuser.h>

char * wchar2char(const wchar_t* wchar );

typedef struct tagWNDINFO
{
    DWORD dwProcessId;
    HWND hWnd;
} WNDINFO, *LPWNDINFO;

//2.枚举所有进程
BOOL CALLBACK MyEnumProc(HWND hWnd, LPARAM lParam); //枚举所有进程

//3.获取给定进程ID的窗口handle
HWND GetProcessMainWnd(DWORD dwProcessId);//获取给定进程ID的窗口handle

HWND GetWndHwnd(char *strExeName);



#endif // FINDWINDOW_H
