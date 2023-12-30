#pragma once

class CConstBuffer;

// DirectX11 �������� GPU ����
class CDevice : public CSingleton<CDevice>
{
    SINGLE(CDevice);

private:
    HWND m_hRenderWnd;
    Vec2 m_vRenderResolution;

    ComPtr<ID3D11Device> m_Device;         // GPU �޸� ����, ��ü ����
    ComPtr<ID3D11DeviceContext> m_Context; // GPU ������ ���

    ComPtr<IDXGISwapChain> m_SwapChain; // ����ü��(��� ���� ����)

    // OM(OutputMergeState)
    ComPtr<ID3D11Texture2D> m_RTTex;         // ����Ÿ�� �ؽ���
    ComPtr<ID3D11RenderTargetView> m_RTView; // ����Ÿ�� ��

    ComPtr<ID3D11Texture2D> m_DSTex;         // �X�� ���ٽ� �ؽ���
    ComPtr<ID3D11DepthStencilView> m_DSView; // �X�� ���ٽ� ��

    ComPtr<ID3D11RasterizerState> m_arrRS[(UINT)RS_TYPE::END];   // �����Ͷ�����
    ComPtr<ID3D11DepthStencilState> m_arrDS[(UINT)DS_TYPE::END]; // ���̽��Ľ�
    ComPtr<ID3D11BlendState> m_arrBS[(UINT)BS_TYPE::END];        // ����

    ComPtr<ID3D11SamplerState> m_arrSS[(UINT)SS_TYPE::END]; // Sampler
    
    CConstBuffer* m_arrCB[(UINT)CB_TYPE::END];

public:
    int init(HWND _hWnd, Vec2 _vResolution);
    void ClearRenderTarget(const Vec4& Color);
    void Present();

    ID3D11Device* GetDevice() const { return m_Device.Get(); }
    ID3D11DeviceContext* GetContext() const { return m_Context.Get(); }

    CConstBuffer* GetConstBuffer(CB_TYPE _type) const { return m_arrCB[(UINT)_type]; }
    Vec2 GetRenderResolution() const { return m_vRenderResolution; }

    ComPtr<ID3D11RasterizerState> GetRSState(RS_TYPE _Type) const { return m_arrRS[(UINT)_Type]; }
    ComPtr<ID3D11DepthStencilState> GetDSState(DS_TYPE _Type) const { return m_arrDS[(UINT)_Type]; }
    ComPtr<ID3D11BlendState> GetBSState(BS_TYPE _Type) const { return m_arrBS[(UINT)_Type]; }

public:
    void Resize(Vec2 resolution);
    ComPtr<ID3D11Texture2D> GetRenderTargetTexture() const { return m_RTTex; }

private:
    int CreateSwapChain();
    int CreateBufferAndView();
    int CreateRasterizerState();
    int CreateDepthStencilState();
    int CreateBlendState();
    int CreateConstBuffer();
    int CreateViewport();
    int CreateSamplerState();
};
