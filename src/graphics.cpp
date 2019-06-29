#include "../include/graphics.h"
#include "../include/resource.h"
#include "../include/module_earth.h"
#include "../include/module_satellite.h"
#include "../include/magnetic_field_circular.h"
#include <string>

namespace Simulation {
    long double FieldLinesWidth = 10.0f;
    long double BaseArrowLength = 150.0l, BaseArrowWidth = 25.0l;

    Graphics::Graphics(HWND hWnd) : hResult(S_OK), d2d1() {
        CreateFactory();
        CreateRenderTarget(hWnd);
        LoadModules();
        CreateDWriteFactory();
        CreateDefaultTextFormat();
        CreateBrush();
        CreateSatelliteTrajectoryLineStrokeStyle();
        CreateArrowStrokeStyle();
        CreateTextLayouts();
    }

    Graphics::~Graphics() {
        SafeRelease(&d2d1.factory);
        SafeRelease(&d2d1.renderTarget);
        SafeRelease(&d2d1.bmpEarth);
        SafeRelease(&d2d1.bmpSatellite);
        SafeRelease(&d2d1.dWriteFactory);
        SafeRelease(&d2d1.textFormatDefault);
        SafeRelease(&d2d1.brush);
        SafeRelease(&d2d1.strokeStyleSatelliteTrajectoryLine);
        SafeRelease(&d2d1.strokeStyleArrow);
        SafeRelease(&d2d1.textLayoutPeriod);
        SafeRelease(&d2d1.textLayoutFrequency);
        SafeRelease(&d2d1.textLayoutAngularSpeed);
        SafeRelease(&d2d1.textLayoutCircularMagneticField);
        SafeRelease(&d2d1.textLayoutRadialAxis);
        SafeRelease(&d2d1.textLayoutTangentAxis);
    }

    void Graphics::Draw() {
        d2d1.renderTarget->BeginDraw();
        d2d1.renderTarget->Clear(D2D1::ColorF(0, 0, 0)); // Black background
        DrawEarth();
        DrawTrajectory();
        DrawMagneticFieldsLines();
        DrawSatellite();
        DrawAxes();
        DrawMagneticFieldsDirections();
        DrawInfo();
        d2d1.renderTarget->EndDraw();
    }

    ////////////////////////////////////////////////////////////////////////////////////////

    void Graphics::CreateFactory() {
        if (Success()) {
            auto type = D2D1_FACTORY_TYPE_SINGLE_THREADED;
            hResult = D2D1CreateFactory(type, &d2d1.factory);
        }
    }

    void Graphics::CreateRenderTarget(HWND hWnd) {
        if (Success()) {
            auto size = D2D1::SizeU(Width, Height);
            auto props1 = D2D1::RenderTargetProperties();
            auto props2 = D2D1::HwndRenderTargetProperties(hWnd, size);
            hResult = d2d1.factory->CreateHwndRenderTarget(props1, props2, &d2d1.renderTarget);
        }
    }

    void Graphics::LoadModules() {
        if (Success()) {
            auto wicFactory = CreateWICFactory();
            LoadEarthModule(wicFactory);
            LoadSatelliteModule(wicFactory);
            SafeRelease(&wicFactory);
        }
    }
    
    void Graphics::CreateDWriteFactory() {
        if (Success()) {
            auto type = DWRITE_FACTORY_TYPE_SHARED;
            auto uuid = __uuidof(IDWriteFactory);
            auto factory = reinterpret_cast<IUnknown * *>(&d2d1.dWriteFactory);
            hResult = DWriteCreateFactory(type, uuid, factory);
        }
    }

    void Graphics::CreateDefaultTextFormat() {
        if (Success()) {
            auto family = DefaultFontFamily;
            auto weight = DefaultFontWeight;
            auto style = DefaultFontStyle;
            auto stretch = DefaultFontStretch;
            auto size = DefaultFontSize;
            hResult = d2d1.dWriteFactory->CreateTextFormat(family, NULL, weight, style, stretch, size, L"", &d2d1.textFormatDefault);
        }
    }

    void Graphics::CreateBrush() {
        if (Success()) {
            auto color = D2D1::ColorF(0, 0); // Black transparent
            hResult = d2d1.renderTarget->CreateSolidColorBrush(color, &d2d1.brush);
        }
    }

    void Graphics::CreateSatelliteTrajectoryLineStrokeStyle() {
        if (Success()) {
            auto startCap = D2D1_CAP_STYLE_FLAT;
            auto endCap = D2D1_CAP_STYLE_FLAT;
            auto dashCap = D2D1_CAP_STYLE_FLAT;
            auto lineJoin = D2D1_LINE_JOIN_BEVEL;
            auto miterLimit = 10.0f;
            auto dashStyle = SatelliteTrajectoryLineDashStyle;
            auto dashOffset = 0.0f;
            auto props = D2D1::StrokeStyleProperties(startCap, endCap, dashCap, lineJoin, miterLimit, dashStyle, dashOffset);
            hResult = d2d1.factory->CreateStrokeStyle(props, NULL, 0, &d2d1.strokeStyleSatelliteTrajectoryLine);
        }
    }

    void Graphics::CreateArrowStrokeStyle() {
        if (Success()) {
            auto startCap = D2D1_CAP_STYLE_ROUND;
            auto endCap = D2D1_CAP_STYLE_TRIANGLE;
            auto dashCap = D2D1_CAP_STYLE_FLAT;
            auto lineJoin = D2D1_LINE_JOIN_BEVEL;
            auto miterLimit = 10.0f;
            auto dashStyle = D2D1_DASH_STYLE_SOLID;
            auto dashOffset = 0.0f;
            auto props = D2D1::StrokeStyleProperties(startCap, endCap, dashCap, lineJoin, miterLimit, dashStyle, dashOffset);
            hResult = d2d1.factory->CreateStrokeStyle(props, NULL, 0, &d2d1.strokeStyleArrow);
        }
    }

    void Graphics::CreateTextLayouts() {
        if (Success()) {
            UpdateTextLayoutPeriod();
            UpdateTextLayoutFrequency();
            UpdateTextLayoutAngularSpeed();
            CreateTextLayoutCircularMagneticField();
            CreateTextLayoutRadialAxis();
            CreateTextLayoutTangentAxis();
        }
    }

    void Graphics::UpdateTextLayouts() {
        if (Success()) {
            // Cleanup the previous layouts
            SafeRelease(&d2d1.textLayoutPeriod);
            SafeRelease(&d2d1.textLayoutFrequency);
            SafeRelease(&d2d1.textLayoutAngularSpeed);

            UpdateTextLayoutPeriod();
            UpdateTextLayoutFrequency();
            UpdateTextLayoutAngularSpeed();
        }
    }

    ////////////////////////////////////////////////////////////////////////////////////////

    void Graphics::DrawEarth() const {
        auto x = Earth::X - Earth::Radius;
        auto y = Earth::Y - Earth::Radius;
        auto size = 2 * Earth::Radius;
        auto rect = D2D1::RectF(x, y, x + size, y + size);
        d2d1.renderTarget->DrawBitmap(d2d1.bmpEarth, rect);
    }

    void Graphics::DrawTrajectory() const {
        d2d1.brush->SetColor(D2D1::ColorF(SatelliteTrajectoryLineColor));
        auto center = D2D1::Point2F(Earth::X, Earth::Y);
        auto radius = Satellite::RadiusTrajectory;
        auto ellipse = D2D1::Ellipse(center, radius, radius);
        d2d1.renderTarget->DrawEllipse(ellipse, d2d1.brush, SatelliteTrajectoryLineWidth, d2d1.strokeStyleSatelliteTrajectoryLine);
    }

    void Graphics::DrawMagneticFieldsLines() const {
        DrawMagneticFieldLinesCircular();
    }

    void Graphics::DrawSatellite() const {
        auto x = Satellite::X - Satellite::Radius;
        auto y = Satellite::Y - Satellite::Radius;
        auto size = 2 * Satellite::Radius;
        auto rect = D2D1::RectF(x, y, x + size, y + size);
        auto center = D2D1::Point2F(Satellite::X, Satellite::Y);
        auto rotation = D2D1::Matrix3x2F::Rotation(-Satellite::AngleDegrees, center);
        d2d1.renderTarget->SetTransform(rotation);
        d2d1.renderTarget->DrawBitmap(d2d1.bmpSatellite, rect);
        d2d1.renderTarget->SetTransform(D2D1::Matrix3x2F::Identity());
    }

    void Graphics::DrawAxes() const {
        DrawRadialAxis();
        DrawTangentAxis();
    }

    void Graphics::DrawMagneticFieldsDirections() const {
        DrawMagneticFieldDirectionCircular();
    }

    void Graphics::DrawInfo() const {
        float x = 20.0f, y = 10.0f;
        d2d1.brush->SetColor(D2D1::ColorF(DefaultTextColor));

        DrawInfoAngle(x, y);
        DrawInfoPeriod(x, y + 50.0f);
        DrawInfoFrequency(x, y + 100.0f);
        DrawInfoAngularSpeed(x, y + 150.0f);
    }

    ////////////////////////////////////////////////////////////////////////////////////////

    IWICImagingFactory* Graphics::CreateWICFactory() {
        if (Success()) {
            IWICImagingFactory* wicFactory = NULL;
            auto clsid = CLSID_WICImagingFactory;
            auto context = CLSCTX_INPROC_SERVER;
            auto iid = IID_IWICImagingFactory;
            hResult = CoCreateInstance(clsid, NULL, context, iid, (void**)& wicFactory);
            return wicFactory;
        }
        return nullptr;
    }

    void Graphics::LoadEarthModule(IWICImagingFactory* wicFactory) {
        if (Success()) {
            LoadBitmapFromResource(wicFactory, IMG_EARTH, &d2d1.bmpEarth);
        }
    }

    void Graphics::LoadSatelliteModule(IWICImagingFactory* wicFactory) {
        if (Success()) {
            LoadBitmapFromResource(wicFactory, IMG_SATELLITE, &d2d1.bmpSatellite);
        }
    }

    void Graphics::LoadBitmapFromResource(IWICImagingFactory* wicFactory, int resId, ID2D1Bitmap** pBmp) {
        auto resInfo = GetResourcePointerAndSize(resId);
        // Create a WIC stream to map onto the memory
        IWICStream* stream = NULL;
        hResult = wicFactory->CreateStream(&stream);
        if (Success()) {
            // Initialize the stream with the memory pointer and size
            auto pbBuffer = reinterpret_cast<BYTE*>(resInfo.first);
            auto bSize = resInfo.second;
            hResult = stream->InitializeFromMemory(pbBuffer, bSize);
            if (Success()) {
                // Create a decoder for the stream
                IWICBitmapDecoder* decoder = NULL;
                auto options = WICDecodeMetadataCacheOnLoad;
                hResult = wicFactory->CreateDecoderFromStream(stream, NULL, options, &decoder);
                if (Success()) {
                    // Create the initial frame
                    IWICBitmapFrameDecode* source = NULL;
                    hResult = decoder->GetFrame(0, &source);
                    if (Success()) {
                        // Create a format converter
                        IWICFormatConverter* converter = NULL;
                        hResult = wicFactory->CreateFormatConverter(&converter);
                        if (Success()) {
                            // Convert the image format to 32bppPBGRA
                            auto format = GUID_WICPixelFormat32bppPBGRA;
                            auto dither = WICBitmapDitherTypeNone;
                            auto palette = WICBitmapPaletteTypeMedianCut;
                            hResult = converter->Initialize(source, format, dither, NULL, 0, palette);
                            if (Success()) {
                                // Create a Direct2D bitmap from the WIC bitmap
                                hResult = d2d1.renderTarget->CreateBitmapFromWicBitmap(converter, NULL, pBmp);
                            }
                        }
                        SafeRelease(&converter);
                    }
                    SafeRelease(&source);
                }
                SafeRelease(&decoder);
            }
        }
        SafeRelease(&stream);
    }

    const std::pair<void*, DWORD> Graphics::GetResourcePointerAndSize(int resId) {
        // Locate the resource
        HRSRC imageResHandle = FindResource(NULL, MAKEINTRESOURCE(resId), L"Image");
        if (imageResHandle != NULL) {
            // Load the resource
            HGLOBAL imageResDataHandle = LoadResource(NULL, imageResHandle);
            if (imageResDataHandle != NULL) {
                // Lock it to get a system memory pointer
                void* pImageFile = LockResource(imageResDataHandle);
                if (pImageFile != NULL) {
                    // Calculate the size of the resource
                    DWORD imageFileSize = SizeofResource(NULL, imageResHandle);
                    if (imageFileSize > 0) {
                        hResult = S_OK;
                        return std::pair(pImageFile, imageFileSize);
                    }
                }
            }
        }
        hResult = E_FAIL;
        return std::pair(nullptr, 0);
    }

    void Graphics::DrawMagneticFieldLinesCircular() const {
        // Brush color = weak red
        auto color = D2D1::ColorF(D2D1::ColorF::Red, 0.5f);
        d2d1.brush->SetColor(color);

        if (Satellite::AngleDegrees == 90.0l || Satellite::AngleDegrees == 270.0l) {
            // Draw straight line
            auto p1 = D2D1::Point2F(Earth::X, 0.0f);
            auto p2 = D2D1::Point2F(Earth::X, (float)Height);
            d2d1.renderTarget->DrawLine(p1, p2, d2d1.brush, FieldLinesWidth);
        }
        else {
            auto r = abs(MagneticFields::Circular::Radius);
            auto rE = r / 2.0l;
            // Left
            {
                auto center = D2D1::Point2F(Earth::X - rE, Earth::Y);
                auto ellipse = D2D1::Ellipse(center, rE, rE);
                d2d1.renderTarget->DrawEllipse(ellipse, d2d1.brush, FieldLinesWidth);
            }
            // Right
            {
                auto center = D2D1::Point2F(Earth::X + rE, Earth::Y);
                auto ellipse = D2D1::Ellipse(center, rE, rE);
                d2d1.renderTarget->DrawEllipse(ellipse, d2d1.brush, FieldLinesWidth);
            }
        }
    }
    
    void Graphics::DrawRadialAxis() const {
        // Brush color = white
        auto color = D2D1::ColorF(D2D1::ColorF::White);
        d2d1.brush->SetColor(color);

        auto dx = Satellite::RadialDirectionX;
        auto dy = Satellite::RadialDirectionY;

        auto ax = dx * BaseArrowLength;
        auto ay = dy * BaseArrowLength;

        auto p1 = D2D1::Point2F(Satellite::X, Satellite::Y);
        auto p2 = D2D1::Point2F(Satellite::X + ax, Satellite::Y - ay);
        d2d1.renderTarget->DrawLine(p1, p2, d2d1.brush, BaseArrowWidth, d2d1.strokeStyleArrow);

        auto size = BaseArrowLength * sqrt(pow(dx, 2.0l) + pow(dy, 2.0l));
        auto len = BaseArrowLength * (size + 50.0l) / size;
        auto p3 = D2D1::Point2F(Satellite::X + dx * len - textLayoutTangentAxisOffsetX / 2.0l, Satellite::Y - dy * len - textLayoutTangentAxisOffsetY / 2.0l);

        d2d1.renderTarget->DrawTextLayout(p3, d2d1.textLayoutRadialAxis, d2d1.brush);
    }

    void Graphics::DrawTangentAxis() const {
        // Brush color = white
        auto color = D2D1::ColorF(D2D1::ColorF::White);
        d2d1.brush->SetColor(color);

        auto dx = Satellite::TangentDirectionX;
        auto dy = Satellite::TangentDirectionY;

        auto ax = dx * BaseArrowLength;
        auto ay = dy * BaseArrowLength;

        auto p1 = D2D1::Point2F(Satellite::X, Satellite::Y);
        auto p2 = D2D1::Point2F(Satellite::X + ax, Satellite::Y - ay);
        d2d1.renderTarget->DrawLine(p1, p2, d2d1.brush, BaseArrowWidth, d2d1.strokeStyleArrow);

        auto size = BaseArrowLength * sqrt(pow(dx, 2.0l) + pow(dy, 2.0l));
        auto len = BaseArrowLength * (size + 50.0l) / size;
        auto p3 = D2D1::Point2F(Satellite::X + dx * len - textLayoutRadialAxisOffsetX / 2.0l, Satellite::Y - dy * len - textLayoutRadialAxisOffsetY / 2.0l);

        d2d1.renderTarget->DrawTextLayout(p3, d2d1.textLayoutTangentAxis, d2d1.brush);
    }

    void Graphics::DrawMagneticFieldDirectionCircular() const {
        // Brush color = strong red
        auto color = D2D1::ColorF(D2D1::ColorF::Red);
        d2d1.brush->SetColor(color);

        auto dx = MagneticFields::Circular::DirectionX;
        auto dy = MagneticFields::Circular::DirectionY;

        auto ax = dx * BaseArrowLength;
        auto ay = dy * BaseArrowLength;

        auto p1 = D2D1::Point2F(Satellite::X, Satellite::Y);
        auto p2 = D2D1::Point2F(Satellite::X + ax, Satellite::Y - ay);
        d2d1.renderTarget->DrawLine(p1, p2, d2d1.brush, BaseArrowWidth, d2d1.strokeStyleArrow);

        auto size = BaseArrowLength * sqrt(pow(dx, 2.0l) + pow(dy, 2.0l));
        auto len = BaseArrowLength * (size + 50.0l) / size;
        auto p3 = D2D1::Point2F(Satellite::X + dx * len - textLayoutCircularMagneticFieldOffsetX / 2.0l, Satellite::Y - dy * len - textLayoutCircularMagneticFieldOffsetY / 2.0l);

        d2d1.renderTarget->DrawTextLayout(p3, d2d1.textLayoutCircularMagneticField, d2d1.brush);
    }

    void Graphics::CreateTextLayout(std::wstring text, IDWriteTextLayout** pLayout) {
        auto cstr = text.c_str();
        auto len = text.length();
        auto format = d2d1.textFormatDefault;
        auto width = (float)Width;
        auto height = (float)Height;
        hResult = d2d1.dWriteFactory->CreateTextLayout(cstr, len, format, width, height, pLayout);
    }

    void Graphics::UpdateTextLayoutPeriod() {
        if (Success()) {
            auto text = L"T = " + std::to_wstring(Satellite::PeriodSeconds) + L"sec";
            CreateTextLayout(text, &d2d1.textLayoutPeriod);
        }
    }

    void Graphics::UpdateTextLayoutFrequency() {
        if (Success()) {
            auto freq = std::to_wstring(1.0l / Satellite::PeriodSeconds);
            auto text = L"ƒ = " + freq + L"Hz";
            CreateTextLayout(text, &d2d1.textLayoutFrequency);
        }
    }

    void Graphics::UpdateTextLayoutAngularSpeed() {
        if (Success()) {
            auto vel1 = std::to_wstring(2.0l / Satellite::PeriodSeconds);
            auto vel2 = std::to_wstring(360.0l / Satellite::PeriodSeconds);
            auto text = L"ω = " + vel1 + L"π/sec (" + vel2 + L"°/sec)";
            CreateTextLayout(text, &d2d1.textLayoutAngularSpeed);
        }
    }

    void Graphics::CreateTextLayoutCircularMagneticField() {
        if (Success()) {
            CreateTextLayout(L"B", &d2d1.textLayoutCircularMagneticField);

            DWRITE_TEXT_METRICS metrics;
            hResult = d2d1.textLayoutCircularMagneticField->GetMetrics(&metrics);
            if (Success()) {
                textLayoutCircularMagneticFieldOffsetX = metrics.widthIncludingTrailingWhitespace;
                textLayoutCircularMagneticFieldOffsetY = metrics.height;
            }
        }
    }

    void Graphics::CreateTextLayoutRadialAxis() {
        if (Success()) {
            CreateTextLayout(L"R", &d2d1.textLayoutRadialAxis);

            DWRITE_TEXT_METRICS metrics;
            hResult = d2d1.textLayoutRadialAxis->GetMetrics(&metrics);
            if (Success()) {
                textLayoutRadialAxisOffsetX = metrics.widthIncludingTrailingWhitespace;
                textLayoutRadialAxisOffsetY = metrics.height;
            }
        }
    }

    void Graphics::CreateTextLayoutTangentAxis() {
        if (Success()) {
            CreateTextLayout(L"T", &d2d1.textLayoutTangentAxis);

            DWRITE_TEXT_METRICS metrics;
            hResult = d2d1.textLayoutTangentAxis->GetMetrics(&metrics);
            if (Success()) {
                textLayoutTangentAxisOffsetX = metrics.widthIncludingTrailingWhitespace;
                textLayoutTangentAxisOffsetY = metrics.height;
            }
        }
    }

    ////////////////////////////////////////////////////////////////////////////////////////

    void Graphics::DrawInfoAngle(float x, float y) const {
        auto angR = std::to_wstring(Satellite::AngleRadians / PI);
        auto angD = std::to_wstring(Satellite::AngleDegrees);
        auto text = L"φ = " + angR + L"π (" + angD + L"°)";
        auto rect = D2D1::RectF(x, y, (float)Width, (float)Height);
        d2d1.renderTarget->DrawTextW(text.c_str(), text.length(), d2d1.textFormatDefault, rect, d2d1.brush);
    }

    void Graphics::DrawInfoPeriod(float x, float y) const {
        d2d1.renderTarget->DrawTextLayout(D2D1::Point2F(x, y), d2d1.textLayoutPeriod, d2d1.brush);
    }

    void Graphics::DrawInfoFrequency(float x, float y) const {
        d2d1.renderTarget->DrawTextLayout(D2D1::Point2F(x, y), d2d1.textLayoutFrequency, d2d1.brush);
    }

    void Graphics::DrawInfoAngularSpeed(float x, float y) const {
        d2d1.renderTarget->DrawTextLayout(D2D1::Point2F(x, y), d2d1.textLayoutAngularSpeed, d2d1.brush);
    }

    ////////////////////////////////////////////////////////////////////////////////////////

    const ID2D1Bitmap* const Simulation::Graphics::GetEarthBitmap() const {
        return d2d1.bmpEarth;
    }

    const ID2D1Bitmap* const Simulation::Graphics::GetSatelliteBitmap() const {
        return d2d1.bmpSatellite;
    }
     
    bool Graphics::Success() const {
        return SUCCEEDED(hResult);
    }

    bool Graphics::Failure() const {
        return FAILED(hResult);
    }

    template<class T>
    void Graphics::SafeRelease(T** t) const {
        if (*t != nullptr) {
            (*t)->Release();
            (*t) = nullptr;
        }
    }
}