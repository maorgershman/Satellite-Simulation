#pragma once

#define UNICODE
#include <Windows.h>
#include <d2d1.h>
#include <wincodec.h>
#include <dwrite.h>

#include <string>
#include <mutex>

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
        inline void SafeRelease(T** t) const;
    public:
        // Fix thread racing when painting/cleaning up!
        static std::mutex mutex;

        Graphics(HWND hWnd);
        ~Graphics();

        void Paint();

        bool HasSucceededLastOperation() const;
        bool HasFailedLastOperation() const;

        const ID2D1Bitmap* const GetEarthBitmap() const;
        const ID2D1Bitmap* const GetSatelliteBitmap() const;
    };

    void ShowError(std::wstring errMsg);
}