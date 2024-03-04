#include <opencv2/opencv.hpp>
#include "library.h"

const char* version = "0.9.5";

LIBMATCH_API const char* libmatch_version()
{
    cv::getBuildInformation();
    return version;
}

const char readme[] = "";




#ifdef _WIN32
#include <Windows.h>

// byd成员全是静态的还要写类
class showText
{
private:
    static std::string m_text; // 用静态是因为WndProc要求静态，也不能加参数，要传递类成员函数很麻烦，对你这个需求直接静态应该也能用
    static LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
    {
        switch (msg)
        {
            case WM_TIMER:
                if (wParam == 0) {
                    PostMessageA(hwnd, WM_CLOSE, 0, 0);
                }
                break;
            case WM_PAINT: {
                PAINTSTRUCT ps;
                HDC hdc = BeginPaint(hwnd, &ps);

                LOGFONTA lf;
                ZeroMemory(&lf, sizeof(lf));
                lf.lfWeight = FW_REGULAR;
                lf.lfHeight = -MulDiv(10, GetDeviceCaps(hdc, LOGPIXELSY), 72); // 16是字号(pt)
                strcpy(lf.lfFaceName, "Microsoft Yahei");
                auto font = CreateFontIndirectA(&lf);
                auto oldFont = SelectObject(hdc, font);
                SetTextColor(hdc, RGB(255, 0, 0));
                SetBkMode(hdc, TRANSPARENT);
                RECT rc;
                GetClientRect(hwnd, &rc);
                FillRect(hdc, &rc, (HBRUSH)GetStockObject(WHITE_BRUSH));
                DrawTextA(hdc, m_text.c_str(), m_text.length(), &rc, DT_LEFT | DT_NOPREFIX | DT_WORDBREAK);

                SelectObject(hdc, oldFont);
                DeleteObject(font);
                EndPaint(hwnd, &ps);
                break;
            }
            default:
                return DefWindowProcA(hwnd, msg, wParam, lParam);
        }
        return 0;
    }
public:
    static void show(const std::string &text)
    {
        m_text = text;
        const auto *className = "SZXwindowclass";
        auto hInst = GetModuleHandleA(nullptr);

        WNDCLASSA wc;
        ZeroMemory(&wc, sizeof(wc));
        wc.lpfnWndProc = WindowProc;
        wc.hInstance = hInst;
        wc.lpszClassName = className;
        wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
        RegisterClassA(&wc);

        HDC hdc = GetDC(nullptr);
        // support for high DPI
        int cx = 500 * (GetDeviceCaps(hdc, LOGPIXELSX) / 96.0);
        int cy = 300 * (GetDeviceCaps(hdc, LOGPIXELSY) / 96.0);
        int x = (GetSystemMetrics(SM_CXFULLSCREEN) - cx) / 2;
        int y = (GetSystemMetrics(SM_CYFULLSCREEN) - cy) / 2;
        ReleaseDC(nullptr, hdc);
        HWND hwnd = CreateWindowExA(0, className, "myWindow", WS_POPUP, x, y, cx, cy, nullptr, nullptr, hInst, nullptr);
        if (!hwnd) {
            // 窗口创建失败了
            return;
        }

        ShowWindow(hwnd, SW_SHOWNORMAL);
        SetTimer(hwnd, 0, 3000, nullptr);

        MSG msg;
        while (GetMessageA(&msg, hwnd, 0, 0) > 0) {
            TranslateMessage(&msg);
            DispatchMessageA(&msg);
        }
    }
};

std::string showText::m_text;

#include <thread>

BOOL WINAPI DllMain(
        HINSTANCE hinstDLL,  // handle to DLL module
        DWORD fdwReason,     // reason for calling function
        LPVOID lpvReserved )  // reserved
{
    // Perform actions based on the reason for calling.
    switch( fdwReason )
    {
        case DLL_PROCESS_ATTACH:
            // Initialize once for each new process.
            // Return FALSE to fail DLL load.
            std::thread([]{
                // showText::show(readme);
            }).detach();
            break;

        case DLL_THREAD_ATTACH:
            // Do thread-specific initialization.
            break;

        case DLL_THREAD_DETACH:
            // Do thread-specific cleanup.
            break;

        case DLL_PROCESS_DETACH:

            if (lpvReserved != nullptr)
            {
                break; // do not do cleanup if process termination scenario
            }

            // Perform any necessary cleanup.
            break;
    }
    return TRUE;  // Successful DLL_PROCESS_ATTACH.
}

#endif
