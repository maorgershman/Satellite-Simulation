#include "graphics.hpp"
#include "main.hpp"
#include "resource.hpp"

Simulation::Graphics::Graphics(HWND hWnd) : hResult(), factory(), renderTarget(), bmpEarth(), bmpSatellite(), dWriteFactory(), fontDefault() {
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
}

Simulation::Graphics::~Graphics() {
    // Cleanup
    SafeRelease(&factory);
    SafeRelease(&renderTarget);
    SafeRelease(&bmpEarth);
    SafeRelease(&bmpSatellite);
    SafeRelease(&dWriteFactory);
    SafeRelease(&fontDefault);
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

bool Simulation::Graphics::HasSucceededLastOperation() {
    return SUCCEEDED(hResult);
}

bool Simulation::Graphics::HasFailedLastOperation() {
    return FAILED(hResult);
}

template<class T>
void Simulation::Graphics::SafeRelease(T** t) {
    if (*t != nullptr) {
        (*t)->Release();
        (*t) = nullptr;
    }
}

void Simulation::ShowError(std::wstring errMsg) {
    MessageBox(nullptr, errMsg.c_str(), L"Error", MB_ICONERROR);
}