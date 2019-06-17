#include "graphics.hpp"
#include "main.hpp"
#include "resource.hpp"
#include "earth.hpp"
#include "satellite.hpp"

#define _USE_MATH_DEFINES
#include <math.h>

Simulation::Graphics::Graphics(HWND hWnd) :
    mutex(),
    hResult(),
    factory(),
    renderTarget(),
    bmpEarth(),
    bmpSatellite(),
    dWriteFactory(),
    fontDefault(),
    brush(),
    strokeStyleTrajectory(),
    colorTrajectoryLine(D2D1::ColorF(D2D1::ColorF::Gray)),
    widthTrajectoryLine(10.0f)
{
    // Create factory
    {
        hResult = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &factory);
        if (HasFailedLastOperation()) {
            ShowError(L"Can't create graphics! (can't create factory)");
            return;
        }
    }
    // Create render target
    {
        auto size = D2D1::SizeU(Width, Height);
        auto props1 = D2D1::RenderTargetProperties();
        auto props2 = D2D1::HwndRenderTargetProperties(hWnd, size);
        hResult = factory->CreateHwndRenderTarget(props1, props2, &renderTarget);
        if (HasFailedLastOperation()) {
            ShowError(L"Can't create graphics! (can't create render target)");
            return;
        }
    }
    // Load resources
    {
        // Create WIC factory
        IWICImagingFactory* wicFactory = nullptr;
        {
            hResult = CoCreateInstance(CLSID_WICImagingFactory, nullptr, CLSCTX_INPROC_SERVER, IID_IWICImagingFactory, (void**)& wicFactory);
            if (HasFailedLastOperation()) {
                SafeRelease(&wicFactory);
                ShowError(L"Can't create graphics! (can't create WIC factory)");
                return;
            }
        }
        // Load earth bitmap
        {
            LoadBitmapFromResource(wicFactory, IMG_EARTH, &bmpEarth);
            if (HasFailedLastOperation()) {
                SafeRelease(&wicFactory);
                ShowError(L"Can't create graphics! (can't load earth bitmap)");
                return;
            }
        }
        // Load satellite bitmap
        {
            LoadBitmapFromResource(wicFactory, IMG_SATELLITE, &bmpSatellite);
            if (HasFailedLastOperation()) {
                SafeRelease(&wicFactory);
                ShowError(L"Can't create graphics! (can't load satellite bitmap)");
                return;
            }
        }
        SafeRelease(&wicFactory);
    }
    // Create DWrite factory
    {
        hResult = DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), reinterpret_cast<IUnknown * *>(&dWriteFactory));
        if (HasFailedLastOperation()) {
            ShowError(L"Can't create graphics! (can't create DWrite factory)");
            return;
        }
    }
    // Create default font
    {
        auto family = L"Comic Sans MS";
        auto weight = DWRITE_FONT_WEIGHT_BOLD;
        auto style = DWRITE_FONT_STYLE_NORMAL;
        auto stretch = DWRITE_FONT_STRETCH_NORMAL;
        auto size = 36.0f;
        hResult = dWriteFactory->CreateTextFormat(family, nullptr, weight, style, stretch, size, L"", &fontDefault);
        if (HasFailedLastOperation()) {
            ShowError(L"Can't create graphics! (can't create default font)");
            return;
        }
    }
    // Create a brush
    {
        hResult = renderTarget->CreateSolidColorBrush(D2D1::ColorF(0, 0, 0, 0), &brush);
        if (HasFailedLastOperation()) {
            ShowError(L"Can't create graphics! (can't create brush)");
            return;
        }
    }
    // Create the trajectory stroke style
    {
        auto startCap = D2D1_CAP_STYLE_FLAT;
        auto endCap = D2D1_CAP_STYLE_FLAT;
        auto dashCap = D2D1_CAP_STYLE_FLAT;
        auto lineJoin = D2D1_LINE_JOIN_BEVEL;
        auto miterLimit = 10.0f;
        auto dashStyle = D2D1_DASH_STYLE_DASH;
        auto dashOffset = 0.0f;
        auto props = D2D1::StrokeStyleProperties(startCap, endCap, dashCap, lineJoin, miterLimit, dashStyle, dashOffset);
        hResult = factory->CreateStrokeStyle(props, nullptr, 0, &strokeStyleTrajectory);
        if (HasFailedLastOperation()) {
            ShowError(L"Can't create graphics! (can't create stroke style for the trajectory)");
            return;
        }
    }
}

Simulation::Graphics::~Graphics() {
    std::lock_guard<std::mutex> lock(mutex);
    // Cleanup
    SafeRelease(&factory);
    SafeRelease(&renderTarget);
    SafeRelease(&bmpEarth);
    SafeRelease(&bmpSatellite);
    SafeRelease(&dWriteFactory);
    SafeRelease(&fontDefault);
    SafeRelease(&brush);
    SafeRelease(&strokeStyleTrajectory);
}

void Simulation::Graphics::LoadBitmapFromResource(IWICImagingFactory* wicFactory, int resourceId, ID2D1Bitmap** pBitmap) {
    // Locate the resource
    HRSRC imageResHandle = FindResource(nullptr, MAKEINTRESOURCE(resourceId), L"Image");
    if (imageResHandle == nullptr) {
        return;
    }

    // Load the resource
    HGLOBAL imageResDataHandle = LoadResource(nullptr, imageResHandle);
    if (imageResDataHandle == nullptr) {
        return;
    }

    // Lock it to get a system memory pointer
    void* pImageFile = LockResource(imageResDataHandle);
    if (pImageFile == nullptr) {
        return;
    }

    // Calculate the size of the resource
    DWORD imageFileSize = SizeofResource(nullptr, imageResHandle);
    if (imageFileSize == 0) {
        return;
    }

    IWICStream* stream = nullptr;
    IWICBitmapDecoder* decoder = nullptr;
    IWICBitmapFrameDecode* source = nullptr;
    IWICFormatConverter* converter = nullptr;

    // Create a WIC stream to map onto the memory
    hResult = wicFactory->CreateStream(&stream);
    if (HasFailedLastOperation()) {
        SafeRelease(&stream);
        return;
    }

    // Initialize the stream with the memory pointer and size
    hResult = stream->InitializeFromMemory(reinterpret_cast<BYTE*>(pImageFile), imageFileSize);
    if (HasFailedLastOperation()) {
        SafeRelease(&stream);
        return;
    }

    // Create a decoder for the stream
    hResult = wicFactory->CreateDecoderFromStream(stream, nullptr, WICDecodeMetadataCacheOnLoad, &decoder);
    if (HasFailedLastOperation()) {
        SafeRelease(&stream);
        SafeRelease(&decoder);
        return;
    }

    // Create the initial frame
    hResult = decoder->GetFrame(0, &source);
    if (HasFailedLastOperation()) {
        SafeRelease(&stream);
        SafeRelease(&decoder);
        SafeRelease(&source);
        return;
    }

    // Create a format converter
    hResult = wicFactory->CreateFormatConverter(&converter);
    if (HasFailedLastOperation()) {
        SafeRelease(&stream);
        SafeRelease(&decoder);
        SafeRelease(&source);
        SafeRelease(&converter);
        return;
    }

    // Convert the image format to 32bppPBGRA
    hResult = converter->Initialize(source, GUID_WICPixelFormat32bppPBGRA, WICBitmapDitherTypeNone, nullptr, 0, WICBitmapPaletteTypeMedianCut);
    if (HasFailedLastOperation()) {
        SafeRelease(&stream);
        SafeRelease(&decoder);
        SafeRelease(&source);
        SafeRelease(&converter);
        return;
    }

    // Create a Direct2D bitmap from the WIC bitmap
    hResult = renderTarget->CreateBitmapFromWicBitmap(converter, nullptr, pBitmap);

    SafeRelease(&stream);
    SafeRelease(&decoder);
    SafeRelease(&source);
    SafeRelease(&converter);
}

void Simulation::Graphics::Paint() {
    // Make sure that the program isn't currently shutting down!!!
    if (renderTarget == nullptr) {
        return;
    }
    // Start drawing
    renderTarget->BeginDraw();
    // Clear the screen with black background
    renderTarget->Clear(D2D1::ColorF(0, 0, 0));
    // Draw the earth
    {
        float x = (float)(Earth::X - Earth::Radius);
        float y = (float)(Earth::Y - Earth::Radius);
        float size = 2.0f * Earth::Radius;
        auto rect = D2D1::RectF(x, y, x + size, y + size);
        renderTarget->DrawBitmap(bmpEarth, rect);
    }
    // Draw the trajectory
    {
        brush->SetColor(colorTrajectoryLine);
        auto center = D2D1::Point2F(Earth::X, Earth::Y);
        auto radius = (float)Satellite::Rotation::RadiusTrajectory;
        auto ellipse = D2D1::Ellipse(center, radius, radius);
        renderTarget->DrawEllipse(ellipse, brush, widthTrajectoryLine, strokeStyleTrajectory);
    }
    // Draw the satellite
    {
        float x = (float)(Satellite::X - Satellite::Radius);
        float y = (float)(Satellite::Y - Satellite::Radius);
        float size = 2.0f * Satellite::Radius;
        auto rect = D2D1::RectF(x, y, x + size, y + size);
        auto center = D2D1::Point2F(Satellite::X, Satellite::Y);
        auto rotation = D2D1::Matrix3x2F::Rotation(-Satellite::Rotation::AngleDegrees, center);
        renderTarget->SetTransform(rotation);
        renderTarget->DrawBitmap(bmpSatellite, rect);
        renderTarget->SetTransform(D2D1::Matrix3x2F::Identity());
    }
    // Print information
    {
        float x = 20.0f, y = 10.0f;
        // Draw angle of rotation
        {
            IDWriteTextLayout* layout = nullptr;
            // Create layout
            {
                auto angR = std::to_wstring(Satellite::Rotation::AngleRadians / M_PI);
                auto angD = std::to_wstring(Satellite::Rotation::AngleDegrees);
                auto text = L"φ = " + angR + L"π (" + angD + L"°)";
                hResult = dWriteFactory->CreateTextLayout(text.c_str(), text.length(), fontDefault, (float)Width, (float)Height, &layout);
                if (layout == nullptr) {
                    return;
                }
            }
            brush->SetColor(D2D1::ColorF(D2D1::ColorF::White));
            renderTarget->DrawTextLayout(D2D1::Point2F(x, y), layout, brush);
            SafeRelease(&layout);
            y += 50.0f;
        }
        // Draw period time
        {
            IDWriteTextLayout* layout = nullptr;
            // Create layout
            {
                auto text = L"T = " + std::to_wstring(Satellite::Rotation::PeriodSeconds) + L"sec";
                hResult = dWriteFactory->CreateTextLayout(text.c_str(), text.length(), fontDefault, (float)Width, (float)Height, &layout);
                if (layout == nullptr) {
                    return;
                }
            }
            brush->SetColor(D2D1::ColorF(D2D1::ColorF::White));
            renderTarget->DrawTextLayout(D2D1::Point2F(x, y), layout, brush);
            SafeRelease(&layout);
            y += 50.0f;
        }
        // Draw frequency
        {
            IDWriteTextLayout* layout = nullptr;
            // Create layout
            {
                auto freq = std::to_wstring(1.0 / Satellite::Rotation::PeriodSeconds);
                auto text = L"ƒ = " + freq + L"Hz";
                hResult = dWriteFactory->CreateTextLayout(text.c_str(), text.length(), fontDefault, (float)Width, (float)Height, &layout);
                if (layout == nullptr) {
                    return;
                }
            }
            brush->SetColor(D2D1::ColorF(D2D1::ColorF::White));
            renderTarget->DrawTextLayout(D2D1::Point2F(x, y), layout, brush);
            SafeRelease(&layout);
            y += 50.0f;
        }
        // Draw angular velocity
        {
            IDWriteTextLayout* layout = nullptr;
            // Create layout
            {
                auto vel1 = std::to_wstring(2.0 / Satellite::Rotation::PeriodSeconds);
                auto vel2 = std::to_wstring(360.0 / Satellite::Rotation::PeriodSeconds);
                auto text = L"ω = " + vel1 + L"π/sec (" + vel2 + L"°/sec)";
                hResult = dWriteFactory->CreateTextLayout(text.c_str(), text.length(), fontDefault, (float)Width, (float)Height, &layout);
                if (layout == nullptr) {
                    return;
                }
            }
            brush->SetColor(D2D1::ColorF(D2D1::ColorF::White));
            renderTarget->DrawTextLayout(D2D1::Point2F(x, y), layout, brush);
            SafeRelease(&layout);
            y += 50.0f;
        }
    }
    // End drawing
    hResult = renderTarget->EndDraw();
    if (HasFailedLastOperation()) {
        ShowError(L"Paint has failed!");
    }
}

const ID2D1Bitmap* const Simulation::Graphics::GetEarthBitmap() const {
    return bmpEarth;
}

const ID2D1Bitmap* const Simulation::Graphics::GetSatelliteBitmap() const {
    return bmpSatellite;
}

bool Simulation::Graphics::HasSucceededLastOperation() const {
    return SUCCEEDED(hResult);
}

bool Simulation::Graphics::HasFailedLastOperation() const {
    return FAILED(hResult);
}

template<class T>
void Simulation::Graphics::SafeRelease(T** t) const {
    if (*t != nullptr) {
        (*t)->Release();
        (*t) = nullptr;
    }
}

void Simulation::ShowError(std::wstring errMsg) {
    MessageBox(nullptr, errMsg.c_str(), L"Error", MB_ICONERROR);
}