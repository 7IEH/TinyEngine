#pragma once

// DirectX11 �������� GPU ����
class CDevice : public CSingleton<CDevice>
{
    SINGLE(CDevice);

  public:
    int init(HWND _hWnd, Vec2 _vResolution);

  private:
    ID3D11Device* m_Device;         // GPU �޸� ���� �� ��ü ����
    ID3D11DeviceContext* m_Context; // GPU ������ ���
};
