#pragma once

class CEngine : public CSingleton<CEngine>
{
    SINGLE(CEngine);

private:
    HWND m_hMainWnd;    // ���� ������ �ڵ�
    Vec2 m_vResolution; // �ػ� ����
    bool m_UseImGui;

public:
    HWND GetMainWind() const { return m_hMainWnd; }
    bool IsUseImGui() const { return m_UseImGui; }

public:
    int init(HWND _hWnd, Vec2 _vResolution);
    void progress();

private:
    int InitImGui();
};
