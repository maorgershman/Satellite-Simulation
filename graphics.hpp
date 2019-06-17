#pragma once

#define UNICODE
#include <Windows.h>
#include <d2d1.h>
#include <wincodec.h>
#include <dwrite.h>

#include <string>

namespace Simulation {

    class Graphics {
    private:
        HRESULT hResult;

        ID2D1Factory* factory;
        ID2D1HwndRenderTarget* renderTarget;
        ID2D1Bitmap* bmpEarth;
        ID2D1Bitmap* bmpSatellite;
        IDWriteFactory* dWriteFactory;
        IDWriteTextFormat* fontDefault;

        void LoadBitmapFromResource(IWICImagingFactory* wicFactory, int resourceId, ID2D1Bitmap** pBitmap);

        template<class T>
        inline void SafeRelease(T** t);
    public:
        Graphics(HWND hWnd);
        ~Graphics();

        bool HasSucceededLastOperation();
        bool HasFailedLastOperation();
    };

    void ShowError(std::wstring errMsg);
}