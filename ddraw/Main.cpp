#include <stdio.h>
#include <tchar.h>
#include <locale.h>
#include <iostream>
#include <windows.h>
#include <d2d1.h>
#include <wincodec.h>
#include <wincodecsdk.h>



// lib
#pragma comment( lib, "d2d1.lib" )
#pragma comment( lib, "WindowsCodecs.lib" )

ID2D1Factory* pD2d1Factory = NULL;
ID2D1HwndRenderTarget* pRenderTarget = NULL;

BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam)
{
    HWND p = FindWindowEx(hwnd, NULL, L"SHELLDLL_DefView", NULL);
    HWND* ret = (HWND*)lParam;

    if (p)
    {
        *ret = FindWindowEx(NULL, hwnd, L"WorkerW", NULL);
    }
    return true;
}

HWND get_wallpaper_window()
{
    HWND progman = FindWindow(L"ProgMan", NULL);
    SendMessageTimeout(progman, 0x052C, 0, 0, SMTO_NORMAL, 1000, nullptr);
    HWND wallpaper_hwnd = nullptr;
    EnumWindows(EnumWindowsProc, (LPARAM)&wallpaper_hwnd);
    return wallpaper_hwnd;
}

int main() {
    HWND hWnd = get_wallpaper_window();
    RECT rect;
    if (GetWindowRect(hWnd, &rect))
    {
        int width = rect.right - rect.left;
        int height = rect.bottom - rect.top;

        HRESULT hResult = S_OK;

        hResult = ::D2D1CreateFactory(D2D1_FACTORY_TYPE_MULTI_THREADED, &pD2d1Factory);
        if (FAILED(hResult)) {
            std::wcout << L"D2D1CreateFactoryŽ¸”s" << std::endl;
            return 0;
        }

        {
            D2D1_SIZE_U oPixelSize = {
                  width
                , height
            };

            D2D1_RENDER_TARGET_PROPERTIES oRenderTargetProperties = D2D1::RenderTargetProperties();

            D2D1_HWND_RENDER_TARGET_PROPERTIES oHwndRenderTargetProperties = D2D1::HwndRenderTargetProperties(hWnd, oPixelSize);


            hResult = pD2d1Factory->CreateHwndRenderTarget(
                oRenderTargetProperties
                , oHwndRenderTargetProperties
                , &pRenderTarget
            );
            if (FAILED(hResult)) {

                std::wcout << L"CreateHwndRenderTargetŽ¸”s" << std::endl;
                return 0;
            }

            while (true)
            {
                HRESULT hResult = S_OK;

                D2D1_SIZE_F oTargetSize = pRenderTarget->GetSize();

                PAINTSTRUCT tPaintStruct;
                ::BeginPaint(hWnd, &tPaintStruct);

                pRenderTarget->BeginDraw();

                D2D1_COLOR_F oBKColor = { 1.0f, 1.0f, 1.0f, 1.0f };
                pRenderTarget->Clear(oBKColor);

                ID2D1SolidColorBrush* pBrush = NULL;
                {
                    pRenderTarget->CreateSolidColorBrush(
                        D2D1::ColorF(
                            0.0f                                            // R
                            , 0.0f                                          // G
                            , 0.0f                                          // B
                            , 1.0f                                          // A
                        )
                        , &pBrush
                    );
                }
                POINT p;
                GetCursorPos(&p);
                D2D1_RECT_F tRectF = D2D1::RectF(
                    p.x - 10
                    , p.y - 10
                    , p.x + 10
                    , p.y + 10
                );


                float fStrokeWidth = 100;

                pRenderTarget->DrawRectangle(&tRectF, pBrush, fStrokeWidth);

                pBrush->Release();

                pRenderTarget->EndDraw();

                ::EndPaint(hWnd, &tPaintStruct);
            }

        }

    }

    if (NULL != pRenderTarget) {
        pRenderTarget->Release();
    }

    if (NULL != pD2d1Factory) {
        pD2d1Factory->Release();
    }
    return 0;
}