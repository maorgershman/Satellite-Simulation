#include "../include/graphics.h"
#include "../include/resource.h"
#include "../include/module_earth.h"
#include "../include/module_satellite.h"
#include <string>

namespace Simulation {
    Graphics::Graphics(HWND hWnd) : hResult(S_OK), d2d1() {
        CreateFactory();
        CreateRenderTarget(hWnd);
        LoadModules();
        CreateDWriteFactory();
        CreateDefaultTextFormat();
        CreateBrush();
        CreateSatelliteTrajectoryLineStrokeStyle();
        UpdateTextLayouts();
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
        SafeRelease(&d2d1.textLayoutPeriod);
        SafeRelease(&d2d1.textLayoutFrequency);
        SafeRelease(&d2d1.textLayoutAngularSpeed);
    }

    void Graphics::Draw() {
        d2d1.renderTarget->BeginDraw();
        d2d1.renderTarget->Clear(D2D1::ColorF(0, 0, 0)); // Black background
        DrawEarth();
        DrawTrajectory();
        DrawSatellite();
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