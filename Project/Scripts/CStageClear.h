#pragma once

#include <Engine/CScript.h>

class CStageClear : public CScript
{
private:
    bool m_bKirbyDance;
    
    Vec3 m_KirbyPos;
    Vec3 m_KirbyDir;

public:
    virtual UINT SaveToLevelFile(FILE* _File) override;
    virtual UINT LoadFromLevelFile(FILE* _File) override;

    void SetKirbyDance(bool _bDance) { m_bKirbyDance = _bDance; }
    void SetKirbyPos(Vec3 _Pos) { m_KirbyPos = _Pos; }
    void SetKirbyDir(Vec3 _Dir) { m_KirbyDir = _Dir; }

private:
    virtual void OnTriggerEnter(CCollider* _OtherCollider) override;
    virtual void OnTriggerStay(CCollider* _OtherCollider) override;
    virtual void OnTriggerExit(CCollider* _OtherCollider) override;

public:
    CLONE(CStageClear);
    CStageClear();
    CStageClear(const CStageClear& _Origin);
    virtual ~CStageClear();

};
