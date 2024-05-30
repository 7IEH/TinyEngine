#pragma once
#include <Engine\\CEntity.h>

#include <Engine\\CTimeMgr.h>
#include <Engine\\CKeyMgr.h>
#include <Engine\\components.h>

class CFSMScript;

class CState : public CEntity
{
private:
    CFSMScript* m_OwnerFSM; // �� state�� ������ FSM (AI)

public:
    virtual void tick() = 0;
    virtual void Enter() = 0;
    virtual void Exit() = 0;

public:
    CGameObject* GetOwner();

protected:
    void ChangeState(const wstring& _strStateName);
    CFSMScript* GetOwnerFSM() const { return m_OwnerFSM; }

public:

    CState* Clone() = 0;
public:
    CState();
    virtual ~CState();

    friend class CFSMScript;
};