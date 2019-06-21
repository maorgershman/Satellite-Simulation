#pragma once

#include "main.h"
#include <wincodec.h>
#include <dwrite.h>

#include <string>

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

        template<class T>
        inline void SafeRelease(T**) const;
        inline void CreateFactory();
        inline void CreateRenderTarget(HWND hWnd);
        inline void LoadModules();
        inline void CreateDWriteFactory();
        inline void CreateDefaultTextFormat();
        inline void CreateBrush();
        inline void CreateSatelliteTrajectoryLineStrokeStyle();
        inline void CreateTextLayout(std::wstring, IDWriteTextLayout**);
        inline void UpdateTextLayoutPeriod();
        inline void UpdateTextLayoutFrequency();
        inline void UpdateTextLayoutAngularSpeed();
        inline void LoadEarthModule(IWICImagingFactory*);
        inline void LoadSatelliteModule(IWICImagingFactory*);

        void LoadBitmapFromResource(IWICImagingFactory*, int, ID2D1Bitmap**);
        void DrawEarth() const; 
        void DrawTrajectory() const;
        void DrawSatellite() const;
        void DrawInfo() const;
        void DrawInfoAngle(float, float) const;
        void DrawInfoPeriod(float, float) const;
        void DrawInfoFrequency(float, float) const;
        void DrawInfoAngularSpeed(float, float) const;

        inline IWICImagingFactory* CreateWICFactory();
        const std::pair<void*, DWORD> GetResourcePointerAndSize(int);
    public:
        Graphics(HWND);
        ~Graphics();

        void Draw();
        void UpdateTextLayouts();

        const ID2D1Bitmap* const GetEarthBitmap() const;
        const ID2D1Bitmap* const GetSatelliteBitmap() const;

        bool Success() const;
        bool Failure() const;
    };
}