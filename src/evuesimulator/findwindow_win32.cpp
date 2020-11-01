#include "findwindow_win32.h"


using namespace std;

char * wchar2char(const wchar_t* wchar )
{
    char * m_char;
    int len= WideCharToMultiByte( CP_ACP ,0,wchar ,wcslen( wchar ), NULL,0, NULL ,NULL );
    m_char= new char[len+1];
    WideCharToMultiByte( CP_ACP ,0,wchar ,wcslen( wchar ),m_char,len, NULL ,NULL );
    m_char[len]= '\0';
    return m_char;
}

BOOL CALLBACK MyEnumProc(HWND hWnd, LPARAM lParam)//枚举所有进程
{
    DWORD dwProcId;
    GetWindowThreadProcessId(hWnd, &dwProcId);
    LPWNDINFO pInfo = (LPWNDINFO)lParam;
    //cout << "dwProcId:" << dwProcId << " pInfo->dwProcessId:" << pInfo->dwProcessId << endl;
    if (dwProcId == pInfo->dwProcessId)
    {

        HWND p = GetParent(hWnd);
        HWND ph = GetParent(p);
        if (ph != 0) {
            pInfo->hWnd = ph;
            return FALSE;
        }

    }

    return TRUE;
}

HWND GetProcessMainWnd(DWORD dwProcessId)//获取给定进程ID的窗口handle
{
    WNDINFO wi;
    wi.dwProcessId = dwProcessId;
    wi.hWnd = NULL;
    EnumWindows(MyEnumProc, (LPARAM)&wi);

    return wi.hWnd;
}

HWND GetWndHwnd(char *strExeName)
{
    PROCESSENTRY32 pe32;
    //在使用这个结构前，先设置它的大小
    pe32.dwSize = sizeof(pe32);
    //给系统内所有的进程拍个快照
    HANDLE hProcessSnap = ::CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hProcessSnap == INVALID_HANDLE_VALUE)
    {
        //printf("CreateToolhelp32Snapshot 调用失败.\n");
        return NULL;
    }
    //遍历进程快照，轮流显示每个进程的信息
    BOOL bMore = ::Process32First(hProcessSnap, &pe32);
    while (bMore)
    {
        //if(pe32.szExeFile == exeName)//找到了进程
        if (!strcmp(wchar2char(pe32.szExeFile), strExeName))
        {

            HWND game_window_hwnd = GetProcessMainWnd(pe32.th32ProcessID);
            if (NULL != game_window_hwnd)
            {
                ::CloseHandle(hProcessSnap);
                return game_window_hwnd;
            }
        }
        bMore = ::Process32Next(hProcessSnap, &pe32);
    }
    //不要忘记清除掉snapshot对象
    ::CloseHandle(hProcessSnap);
    return NULL;
}


