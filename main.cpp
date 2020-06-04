
#define _CRT_SECURE_NO_WARNINGS

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <tchar.h>

#include <ctime>
#include <stdio.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "shadertoy_ispc.h"

using namespace ispc;

const int nWidth = 800;
const int nHeight = 450;

#define TEXTURE_FILE "texture.jpg"

static float* fBuffer;
static unsigned char* bBuffer;
static float iMouse[4] = { 0, 0, 0, 0 };

static HWND hwnd = 0;
static BITMAPINFO bmi = { 0 };

static clock_t startTime = 0;
static clock_t thenTime = 0;
static clock_t fpsCountStartTime = 0;
static unsigned int frameCount = 0;

static CombinedImageSampler iChannel0Sampler;

bool appInitialize() {
    fBuffer = new float[3 * nWidth * nHeight];
    bBuffer = new unsigned char[3 * nWidth * nHeight];

    BITMAPINFOHEADER& h = bmi.bmiHeader;
    h.biSize = sizeof(BITMAPINFOHEADER);
    h.biWidth = nWidth;
    h.biHeight = -nHeight;
    h.biPlanes = 1;
    h.biBitCount = 24;
    h.biCompression = BI_RGB;
    h.biSizeImage = nWidth * nHeight;

    startTime = clock();
    thenTime = startTime;

    // load texture iChannel0
    stbi_uc* result = 0;
    int ncomp = 0;
    result = stbi_load(TEXTURE_FILE, &iChannel0Sampler.width, &iChannel0Sampler.height, &ncomp, STBI_rgb_alpha);
    if (result) {
        uint8_t* texdata = (uint8_t*)result;
        iChannel0Sampler.data = new float[iChannel0Sampler.width * iChannel0Sampler.height * 4];
        for (uint32_t y = 0; y < iChannel0Sampler.height; y++) {
            for (uint32_t x = 0; x < iChannel0Sampler.width; x++) {
                uint32_t idx = (y * iChannel0Sampler.width + x) * 4;
                for (uint32_t n = 0; n < 4; n++) {
                    iChannel0Sampler.data[idx + n] = texdata[idx + n] / 255.f;
                }
            }
        }
        stbi_image_free(result);
    }
    else {
        MessageBoxA(NULL, "Can't load texture file:  " TEXTURE_FILE, "Error", MB_OK);
        return false;
    }
    return true;
}

void appFinalize() {
    delete[] fBuffer;
    delete[] bBuffer;
    delete[] iChannel0Sampler.data;
}

inline unsigned char clamp(float f) {
    int i = (int)(f * 255.5);
    if (i < 0) i = 0;
    if (i > 255) i = 255;
    return (unsigned char)i;
}

void present() {
    HDC dc = GetDC(hwnd);
    if (dc) {
        for (int y = 0; y < nHeight; y++) {
            for (int x = 0; x < nWidth; x++) {
                bBuffer[3 * (y * nWidth + x) + 0] = clamp(fBuffer[3 * (y * nWidth + x) + 2]);
                bBuffer[3 * (y * nWidth + x) + 1] = clamp(fBuffer[3 * (y * nWidth + x) + 1]);
                bBuffer[3 * (y * nWidth + x) + 2] = clamp(fBuffer[3 * (y * nWidth + x) + 0]);
            }
        }
        SetDIBitsToDevice(dc, 0, 0, nWidth, nHeight,
            0, 0, 0, nHeight,
            bBuffer, &bmi, DIB_RGB_COLORS);
        ReleaseDC(hwnd, dc);
    }
}

void appMainLoop() {
    clock_t now = clock();
    float iTime = ((float)now - startTime) / CLOCKS_PER_SEC;
    float iDeltaTime = ((float)now - thenTime) / CLOCKS_PER_SEC;
    thenTime = now;

    shadertoy_ispc(nWidth, nHeight, fBuffer, iTime, iMouse, iChannel0Sampler);
    present();
    frameCount++;

    float seconds = ((float)now - fpsCountStartTime) / CLOCKS_PER_SEC;
    if (seconds > 1.0f) {
        float fps = frameCount / seconds;
        frameCount = 0;
        fpsCountStartTime = now;
        static char title[0x100];
        sprintf(title, "CPU ShaderToy - %.2f FPS", fps);
        SendMessageA(hwnd, WM_SETTEXT, 0, reinterpret_cast<LPARAM>(title));
    }
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
    case WM_LBUTTONDOWN:
        iMouse[2] = 1.0f;
        break;
    case WM_LBUTTONUP:
        iMouse[2] = 0.0f;
        break;
    case WM_RBUTTONDOWN:
        iMouse[3] = 1.0f;
        break;
    case WM_RBUTTONUP:
        iMouse[3] = 0.0f;
        break;
    case WM_MOUSEMOVE:
        iMouse[0] = LOWORD(lParam);
        iMouse[1] = nHeight - HIWORD(lParam);
        break;
    case WM_PAINT: {
        PAINTSTRUCT ps;
        BeginPaint(hWnd, &ps);
        EndPaint(hWnd, &ps);
        return 0;
    }
    case WM_ERASEBKGND:
        return 0;
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProc(hWnd, message, wParam, lParam);
}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    const TCHAR* szClassName = _T("CPUShaderToyWndClass");

    WNDCLASSEX wcex;
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = 0;
    wcex.lpszClassName = szClassName;
    wcex.hIconSm = 0;
    RegisterClassEx(&wcex);

    // calculate client size
    RECT clientSize;
    clientSize.top = 0;
    clientSize.left = 0;
    clientSize.right = nWidth;
    clientSize.bottom = nHeight;

    DWORD style = WS_SYSMENU | WS_BORDER | WS_CAPTION | WS_CLIPCHILDREN | WS_CLIPSIBLINGS;
    AdjustWindowRect(&clientSize, style, FALSE);

    const int realWidth = clientSize.right - clientSize.left;
    const int realHeight = clientSize.bottom - clientSize.top;

    int windowLeft = (GetSystemMetrics(SM_CXSCREEN) - realWidth) / 2;
    int windowTop = (GetSystemMetrics(SM_CYSCREEN) - realHeight) / 2;

    hwnd = CreateWindow(szClassName, _T(""), style, windowLeft, windowTop, realWidth, realHeight, NULL, NULL, hInstance, NULL);

    ShowWindow(hwnd, SW_SHOW);
    UpdateWindow(hwnd);

    SetActiveWindow(hwnd);
    SetForegroundWindow(hwnd);

    if (!appInitialize()) return 1;

    while (TRUE) {
        MSG msg;
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
            if (msg.message == WM_QUIT)
                break;
            DispatchMessage(&msg);
        }
        appMainLoop();
    }

    appFinalize();
    return 0;
}
