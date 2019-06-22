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
            ID2D1StrokeStyle* strokeStyleArrow;
            IDWriteTextLayout* textLayoutPeriod;
            IDWriteTextLayout* textLayoutFrequency;
            IDWriteTextLayout* textLayoutAngularSpeed;
            IDWriteTextLayout* textLayoutCircularMagneticField;
            IDWriteTextLayout* textLayoutRadialAxis;
            IDWriteTextLayout* textLayoutTangentAxis;
        } d2d1;

        void CreateFactory();
        void CreateRenderTarget(HWND);
        void LoadModules(); 
        void CreateDWriteFactory();
        void CreateDefaultTextFormat();
        void CreateBrush();
        void CreateSatelliteTrajectoryLineStrokeStyle();
        void CreateArrowStrokeStyle();
        void CreateTextLayouts();

        void DrawEarth() const;
        void DrawTrajectory() const;
        void DrawMagneticFieldsLines() const;
        void DrawSatellite() const;
        void DrawAxes() const;
        void DrawMagneticFieldsDirections() const;
        void DrawInfo() const;

        IWICImagingFactory* CreateWICFactory();
        void LoadEarthModule(IWICImagingFactory*);
        void LoadSatelliteModule(IWICImagingFactory*);
        void LoadBitmapFromResource(IWICImagingFactory*, int, ID2D1Bitmap**);
        const std::pair<void*, DWORD> GetResourcePointerAndSize(int);
        void DrawMagneticFieldLinesCircular() const;
        void DrawRadialAxis() const;
        void DrawTangentAxis() const;
        void DrawMagneticFieldDirectionCircular() const;
        void CreateTextLayout(std::wstring, IDWriteTextLayout**);
        void UpdateTextLayoutPeriod();
        void UpdateTextLayoutFrequency();
        void UpdateTextLayoutAngularSpeed();
        void CreateTextLayoutCircularMagneticField();
        void CreateTextLayoutRadialAxis();
        void CreateTextLayoutTangentAxis();

        void DrawInfoAngle(float, float) const;
        void DrawInfoPeriod(float, float) const;
        void DrawInfoFrequency(float, float) const;
        void DrawInfoAngularSpeed(float, float) const;

        template<class T>
        void SafeRelease(T**) const;
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

    extern long double FieldLinesWidth;
    extern long double BaseArrowLength, BaseArrowWidth;
}