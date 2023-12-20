#pragma once

class CConstBuffer;

// DirectX11 �������� GPU ����
class CDevice : public CSingleton<CDevice>
{
    SINGLE(CDevice);

private:
    ComPtr<ID3D11Device> m_Device;         // GPU �޸� ����, ��ü ����
    ComPtr<ID3D11DeviceContext> m_Context; // GPU ������ ���

    ComPtr<IDXGISwapChain> m_SwapChain; // ����ü��(��� ���� ����)

    // OM(OutputMergeState)
    ComPtr<ID3D11Texture2D> m_RTTex;         // ����Ÿ�� �ؽ���
    ComPtr<ID3D11RenderTargetView> m_RTView; // ����Ÿ�� ��

    ComPtr<ID3D11Texture2D> m_DSTex;         // �X�� ���ٽ� �ؽ���
    ComPtr<ID3D11DepthStencilView> m_DSView; // �X�� ���ٽ� ��

    HWND m_hRenderWnd;
    Vec2 m_vRenderResolution;

    CConstBuffer* m_arrCB[(UINT)CB_TYPE::END];

private:
    // ImGui Viewport
    ComPtr<ID3D11Texture2D> m_ViewportRTTex;
    ComPtr<ID3D11ShaderResourceView> m_ViewportSRView;

public:
    int init(HWND _hWnd, Vec2 _vResolution);
    void ClearRenderTarget(float (&Color)[4]);
    void Present();

    ID3D11Device* GetDevice() const { return m_Device.Get(); }
    ID3D11DeviceContext* GetContext() const { return m_Context.Get(); }

    CConstBuffer* GetConstBuffer(CB_TYPE _type) const { return m_arrCB[(UINT)_type]; }
    Vec2 GetRenderResolution() { return m_vRenderResolution; }

public:
    ID3D11ShaderResourceView* GetViewportSRV() const { return m_ViewportSRView.Get(); }

public:
    void CopyToViewport();
    int ReSize(Vec2 resolution);

private:
    int CreateBuffers();
    void SetViewport();

private:
    int CreateSwapChain();
    int CreateTargetView();
    int CreateConstBuffer();
};
