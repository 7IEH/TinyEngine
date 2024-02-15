#pragma once
#include "CAsset.h"

class CTexture : public CAsset
{
private:
    ScratchImage m_Image;            // �ؽ��� �ε� �� �ý��۸޸� ����
    ComPtr<ID3D11Texture2D> m_Tex2D; // �ؽ��� �����͸� GPU �޸𸮿� ����
    D3D11_TEXTURE2D_DESC m_Desc;     // �ؽ��� ���� ����

    ComPtr<ID3D11RenderTargetView> m_RTV;   // ����Ÿ�� �뵵
    ComPtr<ID3D11DepthStencilView> m_DSV;   // �X�� ���ٽ� �뵵
    ComPtr<ID3D11ShaderResourceView> m_SRV; // ���̴����� ����ϴ� �뵵(�ؽ��� ��������(t) ���ε�)
    ComPtr<ID3D11UnorderedAccessView>
        m_UAV; // GPGPU(General Purpose GPU) - ComputeShader, �б� ���� ���ð���, (Unordered Register(u) �� ���ε� ����)

    UINT m_RecentNum_SRV;
    UINT m_RecentNum_UAV;

private:
    int Create(UINT _Width, UINT _Height, DXGI_FORMAT _pixelformat, UINT _BindFlag, D3D11_USAGE _Usage,
               const D3D11_DEPTH_STENCIL_VIEW_DESC* _dsvDesc, const D3D11_RENDER_TARGET_VIEW_DESC* _rtvDesc,
               const D3D11_SHADER_RESOURCE_VIEW_DESC* _srvDesc, const D3D11_UNORDERED_ACCESS_VIEW_DESC* _uavDesc);
    int Create(ComPtr<ID3D11Texture2D> _tex2D, const D3D11_DEPTH_STENCIL_VIEW_DESC* _dsvDesc,
               const D3D11_RENDER_TARGET_VIEW_DESC* _rtvDesc, const D3D11_SHADER_RESOURCE_VIEW_DESC* _srvDesc,
               const D3D11_UNORDERED_ACCESS_VIEW_DESC* _uavDesc);

private:
    virtual int Load(const wstring& _strFilePath) override;

public:
    void UpdateData(int _RegisterNum);

    int UpdateData_CS_SRV(int _RegisterNum);
    int UpdateData_CS_UAV(int _RegisterNum);

    static void Clear(int _iRegisterNum);
    void Clear_CS_SRV();
    void Clear_CS_UAV();

    UINT GetWidth() const { return m_Desc.Width; }
    UINT GetHeight() const { return m_Desc.Height; }

    ComPtr<ID3D11Texture2D> GetTex2D() const { return m_Tex2D; }
    const D3D11_TEXTURE2D_DESC& GetDesc() const { return m_Desc; }
    ComPtr<ID3D11RenderTargetView> GetRTV() const { return m_RTV; }
    ComPtr<ID3D11DepthStencilView> GetDSV() const { return m_DSV; }
    ComPtr<ID3D11ShaderResourceView> GetSRV() const { return m_SRV; }
    ComPtr<ID3D11UnorderedAccessView> GetUAV() const { return m_UAV; }

    tPixel* GetPixels();
    void CaptureTex();

public:
    CTexture();
    virtual ~CTexture();

    friend class CAssetMgr;
};
