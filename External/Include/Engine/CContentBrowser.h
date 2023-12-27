#pragma once
#include "CEntity.h"

class CTexture;

class CContentBrowser : public CEntity
{
private:
    std::filesystem::path m_CurrentDirectory;

    CTexture* m_DirectoryIcon;
    CTexture* m_FileIcon;

public:
    void begin();
    void render();

private:
    CContentBrowser();
    ~CContentBrowser();

    friend class CLevelEditor; // CLevelEditor ������ ���������ϵ��� ����
};
