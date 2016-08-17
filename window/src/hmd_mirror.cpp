#include "hmd_mirror.h"

#include <cassert>
#include <QFile>
#include <QTextStream>
#include "openvr_system.h"

HmdMirror::HmdMirror(Controller& controller)
    : cursorGrabbed(false), controller(controller), eyeView(EyeView::LEFT)
{
    createDevice();
    createBuffers();
    createSamplerState();

    setAttribute(Qt::WA_PaintOnScreen, true);
    setAttribute(Qt::WA_NativeWindow, true);

    calculateCenter();

    vr::VRCompositor()->CompositorBringToFront();
    HWND window = FindWindow(nullptr, L"Headset Window");
    ShowWindow(window, SW_FORCEMINIMIZE);
    int32_t x;
    int32_t y;
    uint32_t w;
    uint32_t h;
    vr::VRExtendedDisplay()->GetWindowBounds(&x, &y, &w, &h);
    //OpenVrSystem::get().SetDisplayVisibility(false);
}

HmdMirror::~HmdMirror()
{
    releaseBuffers();
    swapChain->Release();
    device->Release();
    context->Release();
}

void HmdMirror::paintEvent(QPaintEvent* evt)
{
    if (vr::VRCompositor()->GetMirrorTextureD3D11(
        vr::Eye_Left,
        device,
        reinterpret_cast<void**>(&shaderResourceView)))
    {
        throw std::runtime_error("Could not retrieve the compositor mirror texture");
    }

    render();
}

void HmdMirror::setEyeView(EyeView newEyeView)
{
    eyeView = newEyeView;
}

float HmdMirror::getAspectRatio() const
{
    uint32_t renderWidth;
    uint32_t renderHeight;
    OpenVrSystem::get().GetRecommendedRenderTargetSize(&renderWidth, &renderHeight);
    float aspect = static_cast<float>(eyeView == EyeView::BOTH ? renderWidth * 2 : renderWidth) /
        static_cast<float>(renderHeight);
    return aspect;
}

QPaintEngine* HmdMirror::paintEngine() const
{
    return nullptr;
}

void HmdMirror::resizeEvent(QResizeEvent* evt)
{
    calculateCenter();

    releaseBuffers();
    swapChain->ResizeBuffers(1, width(), height(),
        swapChainDesc.BufferDesc.Format, 0);
    swapChain->GetDesc(&swapChainDesc);
    viewport.Width = width();
    viewport.Height = height();
    createBuffers();
}

void HmdMirror::createDevice()
{
    HRESULT hr;

    ZeroMemory(&swapChainDesc, sizeof(DXGI_SWAP_CHAIN_DESC));

    swapChainDesc.BufferCount = 1;
    swapChainDesc.BufferDesc.Width = width();
    swapChainDesc.BufferDesc.Height = height();
    swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapChainDesc.SampleDesc.Count = 4;
    swapChainDesc.SampleDesc.Quality = 0;
    swapChainDesc.Windowed = true;
    swapChainDesc.OutputWindow = (HWND)winId();
    swapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
    swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;

    hr = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, 0,
        NULL, NULL, D3D11_SDK_VERSION,
        &swapChainDesc, &swapChain, &device,
        NULL, &context);
    if (FAILED(hr))
    {
        throw std::runtime_error("Couldn't create d3d device");
    }
}

void HmdMirror::createBuffers()
{
    HRESULT hr;

    hr = swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D),
        (LPVOID*)&backBuffer);
    assert(!FAILED(hr));

    hr = device->CreateRenderTargetView(backBuffer, NULL, &rtView);
    assert(!FAILED(hr));

    ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));
    viewport.MinDepth = 0.0f;
    viewport.MaxDepth = 1.0f;
    viewport.TopLeftX = 0;
    viewport.TopLeftY = 0;
    viewport.Width = width();
    viewport.Height = height();


    ID3DBlob* vsBlob = NULL;
    ID3DBlob* psBlob = NULL;

    compileShader(":/shader-vs.hlsl", "main", "vs_5_0", &vsBlob);
    hr = device->CreateVertexShader(vsBlob->GetBufferPointer(),
        vsBlob->GetBufferSize(), NULL,
        &vshader);
    assert(!FAILED(hr));

    compileShader(":/shader-ps.hlsl", "main", "ps_5_0", &psBlob);
    hr = device->CreatePixelShader(psBlob->GetBufferPointer(),
        psBlob->GetBufferSize(), NULL,
        &pshader);
    assert(!FAILED(hr));

    D3D11_INPUT_ELEMENT_DESC elementLayout[] = {
        { "POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0,
        D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 2 * sizeof(float),
        D3D11_INPUT_PER_VERTEX_DATA, 0 }
    };
    UINT numElements = 2;

    hr = device->CreateInputLayout(elementLayout, numElements,
        vsBlob->GetBufferPointer(),
        vsBlob->GetBufferSize(), &layout);
    assert(!FAILED(hr));

    struct Vertex {
        float x, y, tu, tv;
    };

    std::vector<Vertex> vertices;
    vertices.push_back({ -1.0, -1.0, 0.0, 1.0 });
    vertices.push_back({ 1.0, -1.0, 1.0, 1.0 });
    vertices.push_back({ 1.0, 1.0, 1.0, 0.0 });

    vertices.push_back({ -1.0, -1.0, 0.0, 1.0 });
    vertices.push_back({ 1.0, 1.0, 1.0, 0.0 });
    vertices.push_back({ -1.0, 1.0, 0.0, 0.0 });

    D3D11_BUFFER_DESC desc;
    ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));
    desc.Usage = D3D11_USAGE_DEFAULT;
    desc.ByteWidth = sizeof(Vertex) * vertices.size();
    desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    desc.CPUAccessFlags = 0;
    D3D11_SUBRESOURCE_DATA initData;
    ZeroMemory(&initData, sizeof(D3D11_SUBRESOURCE_DATA));
    initData.pSysMem = &vertices[0];

    hr = device->CreateBuffer(&desc, &initData, &vbuffer);
    assert(!FAILED(hr));
}

void HmdMirror::createSamplerState()
{
    D3D11_SAMPLER_DESC samplerDesc;
    memset(&samplerDesc, 0, sizeof(D3D11_SAMPLER_DESC));
    samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
    samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
    samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
    device->CreateSamplerState(&samplerDesc, &samplerState);
}

void HmdMirror::releaseBuffers()
{
    rtView->Release();
    backBuffer->Release();
    vshader->Release();
    pshader->Release();
    layout->Release();
    vbuffer->Release();
}

void HmdMirror::render()
{
    float clearColor[] = { 0.3f, 0.3f, 0.3f, 1.0f };

    context->OMSetRenderTargets(1, &rtView, NULL);
    context->RSSetViewports(1, &viewport);
    context->ClearRenderTargetView(rtView, clearColor);

    context->IASetInputLayout(layout);

    UINT stride = sizeof(float) * 4;
    UINT offset = 0;
    context->IASetVertexBuffers(0, 1, &vbuffer, &stride, &offset);
    context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    context->VSSetShader(vshader, NULL, 0);
    context->PSSetShader(pshader, NULL, 0);

    ID3D11RasterizerState* state;
    D3D11_RASTERIZER_DESC desc;
    ZeroMemory(&desc, sizeof(D3D11_RASTERIZER_DESC));
    desc.FillMode = D3D11_FILL_SOLID;
    desc.CullMode = D3D11_CULL_NONE;
    device->CreateRasterizerState(&desc, &state);
    context->RSSetState(state);
    state->Release();

    context->PSSetShaderResources(0, 1, &shaderResourceView);
    context->PSSetSamplers(0, 1, &samplerState);

    context->Draw(6, 0);

    swapChain->Present(0, 0);
}

void HmdMirror::compileShader(const std::string& filename, const std::string& entry, const std::string& sm, ID3DBlob** blob)
{
    HRESULT hr;
    DWORD flags = D3DCOMPILE_ENABLE_STRICTNESS | D3DCOMPILE_DEBUG;
    ID3DBlob* errBlob;

    QFile shaderFile(QString::fromStdString(filename));
    shaderFile.open(QFile::ReadOnly | QFile::Text);
    std::string source = QTextStream(&shaderFile).readAll().toStdString();

    hr = D3DCompile(source.c_str(), source.size(), filename.c_str(), NULL, NULL,
        entry.c_str(), sm.c_str(), flags, 0, blob, &errBlob);

    if (FAILED(hr)) {
        if (errBlob != NULL) {
            OutputDebugStringA((char*)errBlob->GetBufferPointer());
            errBlob->Release();
        }
        assert(0 && "FAIL");
    }

    if (errBlob != NULL) {
        errBlob->Release();
    }
}


void HmdMirror::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::RightButton && cursorGrabbed) {
        releaseCursor();
    } else if (event->button() == Qt::LeftButton && !cursorGrabbed) {
        grabCursor();
    }
}

void HmdMirror::mouseMoveEvent(QMouseEvent* event)
{
    if (cursorGrabbed) {
        QPoint move = event->pos() - center;
        controller.moveMouse(move.x(), move.y());
    }
}

void HmdMirror::grabCursor()
{
    cursorGrabbed = true;
    cursor().setPos(mapToGlobal(center));
    setCursor(Qt::BlankCursor);
    RECT rect;
    rect.left = geometry().left();
    rect.bottom = geometry().bottom();
    rect.right = geometry().right();
    rect.top = geometry().top();
    ClipCursor(&rect);
}

void HmdMirror::releaseCursor()
{
    cursorGrabbed = false;
    ClipCursor(nullptr);
    setCursor(Qt::ArrowCursor);
    cursor().setPos(mapToGlobal(center));
}


void HmdMirror::calculateCenter()
{
    center = QPoint(geometry().x() + geometry().width() / 2, geometry().y() + geometry().height() / 2);
}
