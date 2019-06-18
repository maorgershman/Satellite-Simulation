#pragma once

#include "main.h"
#include <wincodec.h>
#include <dwrite.h>

namespace Simulation {
    class Graphics {
    private:
        // Default font
        static constexpr auto DefaultFontFamily = L"Comic Sans MS";
        static constexpr auto DefaultFontWeight = DWRITE_FONT_WEIGHT_BOLD;
        static constexpr auto DefaultFontStyle = DWRITE_FONT_STYLE_NORMAL;
        static constexpr auto DefaultFontStretch = DWRITE_FONT_STRETCH_NORMAL;
        static constexpr auto DefaultFontSize = 36.0f;
        static constexpr auto DefaultTextColor = D2D1::ColorF::DarkGray;

        // Satellite trajectory line
        static constexpr auto SatelliteTrajectoryLineWidth = 10.0f;
        static constexpr auto SatelliteTrajectoryLineColor = D2D1::ColorF::Gray;
        static constexpr auto SatelliteTrajectoryLineDashStyle = D2D1_DASH_STYLE_DASH;

        HRESULT hResult;
        struct {
            ID2D1Factory* factory;
            ID2D1HwndRenderTarget* renderTarget;
            ID2D1Bitmap* bmpEarth;
            ID2D1Bitmap* bmpSatellite;
            IDWriteFactory* dWriteFactory;
            IDWriteTextFormat* textFormatDefault;
            ID2D1SolidColorBrush* brush;
            ID2D1StrokeStyle* strokeStyleSatelliteTrajectoryLine;
            IDWriteTextLayout* textLayoutPeriod;
            IDWriteTextLayout* textLayoutFrequency;
            IDWriteTextLayout* textLayoutAngularSpeed;
        } d2d1;

        inline void CreateFactory();
        
        inline void CreateRenderTarget(HWND hWnd);

        inline void LoadModules();
        inline IWICImagingFactory* CreateWICFactory();
        inline void LoadEarthModule(IWICImagingFactory* wicFactory);
        inline void LoadSatelliteModule(IWICImagingFactory* wicFactory);
        void LoadBitmapFromResource(IWICImagingFactory* wicFactory, int resId, ID2D1Bitmap** pBmp);
        const std::pair<void*, DWORD> GetResourcePointerAndSize(int resId);

        inline void CreateDWriteFactory();
        
        inline void CreateDefaultTextFormat();

        inline void CreateBrush();

        inline void CreateSatelliteTrajectoryLineStrokeStyle();

        inline void CreateTextLayouts();
        inline void CreateTextLayoutPeriod();
        inline void CreateTextLayoutFrequency();
        inline void CreateTextLayoutAngularSpeed();

        void DrawEarth() const; 
        void DrawTrajectory() const;
        void DrawSatellite() const;
        void DrawInfo() const;
        void DrawInfoAngle(float x, float y) const;
        void DrawInfoPeriod(float x, float y) const;
        void DrawInfoFrequency(float x, float y) const;
        void DrawInfoAngularSpeed(float x, float y) const;

        template<class T> 
        inline void SafeRelease(T** t) const;
    public:
        Graphics(HWND hWnd);
        ~Graphics();

        void Draw();

        const ID2D1Bitmap* const GetEarthBitmap() const;
        const ID2D1Bitmap* const GetSatelliteBitmap() const;

        bool Success() const;
        bool Failure() const;
    };
}