#pragma once

#include <QWidget>
#include <dxgi.h>
#include <d3d11.h>
#include <D3Dcompiler.h>
#include <QMouseEvent>
#include "controller.h"

class HmdMirror : public QWidget
{
    Q_OBJECT

public:
    enum class EyeView
    {
        LEFT,
        RIGHT,
        BOTH
    };

    HmdMirror(Controller& controller);
    ~HmdMirror();

    void setEyeView(EyeView eyeView);
    float getAspectRatio() const;

private:

    //QT

    void paintEvent(QPaintEvent* evt) override;
    QPaintEngine* paintEngine() const override;
    void resizeEvent(QResizeEvent* evt) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;

    // D3D

    ID3D11Device* device;
    ID3D11DeviceContext* context;
    IDXGISwapChain* swapChain;
    ID3D11RenderTargetView* rtView;
    DXGI_SWAP_CHAIN_DESC swapChainDesc;
    ID3D11Texture2D* backBuffer;
    D3D11_VIEWPORT viewport;
    IDXGIAdapter* adapter;
    ID3D11VertexShader* vshader;
    ID3D11PixelShader* pshader;
    ID3D11InputLayout* layout;
    ID3D11Buffer* vbuffer;
    ID3D11ShaderResourceView* shaderResourceView;
    ID3D11SamplerState* samplerState;

    void createDevice();
    void createBuffers();
    void createSamplerState();
    void releaseBuffers();
    void render();
    void compileShader(
        const std::string& filename,
        const std::string& entry,
        const std::string& sm,
        ID3DBlob** blob);


    //Cursor grabbing

    bool cursorGrabbed;
    QPoint center;

    void grabCursor();
    void releaseCursor();
    void calculateCenter();

    //OpenVR
    Controller& controller;
    EyeView eyeView;
};
