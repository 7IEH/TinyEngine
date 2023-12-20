#pragma once

class CEngine : public CSingleton<CEngine>
{
    SINGLE(CEngine);

private:
    HWND m_hMainWnd;    // ���� ������ �ڵ�
    Vec2 m_vResolution; // �ػ� ����

public:
    void SetResolution(Vec2 resolution) { m_vResolution = resolution; }
    HWND GetMainWind() const { return m_hMainWnd; }

public:
    int init(HWND _hWnd, Vec2 _vResolution);
    void progress();

};
