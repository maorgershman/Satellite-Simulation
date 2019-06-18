#include "../include/graphics.h"
#include "../include/resource.h"
#include "../include/module_earth.h"
#include "../include/module_satellite.h"
#include <string>

namespace Simulation {
    Graphics::Graphics(HWND hWnd) : hResult(S_OK), d2d1() {
        CreateFactory();
        if (Success()) {
            CreateRenderTarget(hWnd);
            if (Success()) {
                LoadModules();
                if (Success()) {
                    CreateDWriteFactory();
                    if (Success()) {
                        CreateDefaultTextFormat();
                        if (Success()) {
                            CreateBrush();
                            if (Success()) {
                                CreateSatelliteTrajectoryLineStrokeStyle();
                            }
                        }
                    }
                }
            }
        }
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
    }

    //////////////////////

    void Graphics::CreateFactory() {
        hResult = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &d2d1.factory);
    }

    void Graphics::CreateRenderTarget(HWND hWnd) {
        auto size = D2D1::SizeU(Width, Height);
        auto props1 = D2D1::RenderTargetProperties();
        auto props2 = D2D1::HwndRenderTargetProperties(hWnd, size);
        hResult = d2d1.factory->CreateHwndRenderTarget(props1, props2, &d2d1.renderTarget);
    }

    void Graphics::LoadModules() {
        auto wicFactory = CreateWICFactory();
        if (Success()) {
            LoadEarthModule(wicFactory);
            if (Success()) {
                LoadSatelliteModule(wicFactory);
            }
        }
        SafeRelease(&wicFactory);
    }

    IWICImagingFactory* Graphics::CreateWICFactory() {
        IWICImagingFactory* wicFactory = NULL;
        hResult = CoCreateInstance(CLSID_WICImagingFactory, NULL, CLSCTX_INPROC_SERVER, IID_IWICImagingFactory, (void**)& wicFactory);
        return wicFactory;
    }

    void Graphics::LoadEarthModule(IWICImagingFactory* wicFactory) {
        LoadBitmapFromResource(wicFactory, IMG_EARTH, &d2d1.bmpEarth);
    }

    void Graphics::LoadSatelliteModule(IWICImagingFactory* wicFactory) {
        LoadBitmapFromResource(wicFactory, IMG_SATELLITE, &d2d1.bmpSatellite);
    }

    void Graphics::LoadBitmapFromResource(IWICImagingFactory* wicFactory, int resId, ID2D1Bitmap** pBmp) {
        auto resInfo = GetResourcePointerAndSize(resId);
        // Create a WIC stream to map onto the memory
        IWICStream* stream = NULL;
        hResult = wicFactory->CreateStream(&stream);
        if (Success()) {
            // Initialize the stream with the memory pointer and size
            hResult = stream->InitializeFromMemory(reinterpret_cast<BYTE*>(resInfo.first), resInfo.second);
            if (Success()) {
                // Create a decoder for the stream
                IWICBitmapDecoder* decoder = NULL;
                hResult = wicFactory->CreateDecoderFromStream(stream, NULL, WICDecodeMetadataCacheOnLoad, &decoder);
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
                            hResult = converter->Initialize(source, GUID_WICPixelFormat32bppPBGRA, WICBitmapDitherTypeNone, NULL, 0, WICBitmapPaletteTypeMedianCut);
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

    void Graphics::CreateDWriteFactory() {
        hResult = DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), reinterpret_cast<IUnknown * *>(&d2d1.dWriteFactory));
    }

    void Graphics::CreateDefaultTextFormat() {
        hResult = d2d1.dWriteFactory->CreateTextFormat(DefaultFontFamily, NULL, DefaultFontWeight, DefaultFontStyle, DefaultFontStretch, DefaultFontSize, L"", &d2d1.textFormatDefault);
    }

    void Graphics::CreateBrush() {
        hResult = d2d1.renderTarget->CreateSolidColorBrush(D2D1::ColorF(0, 0), &d2d1.brush);
    }

    void Graphics::CreateSatelliteTrajectoryLineStrokeStyle() {
        auto startCap = D2D1_CAP_STYLE_FLAT;
        auto endCap = D2D1_CAP_STYLE_FLAT;
        auto dashCap = D2D1_CAP_STYLE_FLAT;
        auto lineJoin = D2D1_LINE_JOIN_BEVEL;
        auto miterLimit = 10.0f;
        auto dashOffset = 0.0f;
        auto props = D2D1::StrokeStyleProperties(startCap, endCap, dashCap, lineJoin, miterLimit, SatelliteTrajectoryLineDashStyle, dashOffset);
        hResult = d2d1.factory->CreateStrokeStyle(props, NULL, 0, &d2d1.strokeStyleSatelliteTrajectoryLine);
    }

    /////////////////////

    void Graphics::Draw() {
        d2d1.renderTarget->BeginDraw();
        d2d1.renderTarget->Clear(D2D1::ColorF(0, 0, 0)); // Black background
        DrawEarth();
        DrawTrajectory();
        DrawSatellite();
        DrawInfo();
        d2d1.renderTarget->EndDraw();
    }

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

        IDWriteTextLayout* layout = NULL;
        DrawInfoAngle(layout, x, y);
        DrawInfoPeriod(layout, x, y + 50.0f);
        DrawInfoFrequency(layout, x, y + 100.0f);
        DrawInfoAngularSpeed(layout, x, y + 150.0f);
        SafeRelease(&layout);
    }

    void Graphics::DrawInfoAngle(IDWriteTextLayout* layout, float x, float y) const {
        auto angR = std::to_wstring(Satellite::AngleRadians / PI);
        auto angD = std::to_wstring(Satellite::AngleDegrees);
        auto text = L"φ = " + angR + L"π (" + angD + L"°)";
        d2d1.dWriteFactory->CreateTextLayout(text.c_str(), text.length(), d2d1.textFormatDefault, (float)Width, (float)Height, &layout);
        if (layout != NULL) {
            d2d1.renderTarget->DrawTextLayout(D2D1::Point2F(x, y), layout, d2d1.brush);
        }
    }

    void Graphics::DrawInfoPeriod(IDWriteTextLayout* layout, float x, float y) const {
        auto text = L"T = " + std::to_wstring(Satellite::PeriodSeconds) + L"sec";
        d2d1.dWriteFactory->CreateTextLayout(text.c_str(), text.length(), d2d1.textFormatDefault, (float)Width, (float)Height, &layout);
        if (layout != NULL) {
            d2d1.renderTarget->DrawTextLayout(D2D1::Point2F(x, y), layout, d2d1.brush);
        }
    }

    void Graphics::DrawInfoFrequency(IDWriteTextLayout* layout, float x, float y) const {
        auto freq = std::to_wstring(1.0l / Satellite::PeriodSeconds);
        auto text = L"ƒ = " + freq + L"Hz";
        d2d1.dWriteFactory->CreateTextLayout(text.c_str(), text.length(), d2d1.textFormatDefault, (float)Width, (float)Height, &layout);
        if (layout != NULL) {
            d2d1.renderTarget->DrawTextLayout(D2D1::Point2F(x, y), layout, d2d1.brush);
        }
    }

    void Graphics::DrawInfoAngularSpeed(IDWriteTextLayout* layout, float x, float y) const {
        auto vel1 = std::to_wstring(2.0l / Satellite::PeriodSeconds);
        auto vel2 = std::to_wstring(360.0l / Satellite::PeriodSeconds);
        auto text = L"ω = " + vel1 + L"π/sec (" + vel2 + L"°/sec)";
        d2d1.dWriteFactory->CreateTextLayout(text.c_str(), text.length(), d2d1.textFormatDefault, (float)Width, (float)Height, &layout);
        if (layout != NULL) {
            d2d1.renderTarget->DrawTextLayout(D2D1::Point2F(x, y), layout, d2d1.brush);
        }
    }

    /////////////////////

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