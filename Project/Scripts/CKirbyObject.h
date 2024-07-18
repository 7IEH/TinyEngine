#pragma once
#include "CPlayerMgr.h"
#include "CKirbyFSM.h"
#include "CKirbyMoveController.h"
// ====================================================================================
// USAGE: Kirby�� Object �������� States�� ��� �����ص� States Group Class
// ====================================================================================
// - Object �������� ���(�ش� class�� ���Ե� �Լ�)�� State���� call�ϴ� ������� ����
// - Object�� ������� ������ �� �ִ� ����� State���� ���� ����
// - Ability�� Object: ���� ���� �����ϴ� ����� �ٸ��Ƿ� ���� �ٸ� parent class�� ���

class CKirbyObject
{
protected:
    Ptr<CMeshData> m_Mesh;
    Ptr<CMeshData> m_DemoMesh;
    Ptr<CPrefab> m_OriginObject;
    UINT m_DemoMeshIdx_BodyA;
    UINT m_DemoMeshIdx_BodyB;
    int m_MeshChangeIdx;
    bool m_bFrmEnter;

protected:
    // ��� �޽�: mesh
    void ParseDemoMesh(Ptr<CMeshData> _pMesh);
    void ChangeState(const wstring& _strStateName);

public:
    virtual void Idle(){};
    virtual void IdleEnter();
    virtual void IdleExit();

    virtual void Run(){};
    virtual void RunEnter();
    virtual void RunExit(){};

    virtual void RunStart(){};
    virtual void RunStartEnter(){};
    virtual void RunStartExit(){};

    virtual void RunEnd(){};
    virtual void RunEndEnter();
    virtual void RunEndExit(){};

    virtual void Jump(){};
    virtual void JumpEnter(){};
    virtual void JumpExit(){};

    virtual void JumpStart(){};
    virtual void JumpStartEnter();
    virtual void JumpStartExit(){};

    virtual void JumpFall(){};
    virtual void JumpFallEnter();
    virtual void JumpFallExit(){};

    virtual void Landing(){};
    virtual void LandingEnter();
    virtual void LandingExit();

    virtual void LandingEnd(){};
    virtual void LandingEndEnter();
    virtual void LandingEndExit();

    virtual void Attack() = 0;
    virtual void AttackEnter() = 0;
    virtual void AttackExit() = 0;

    virtual void AttackEnd(){};
    virtual void AttackEndEnter(){};
    virtual void AttackEndExit(){};

    virtual void AttackStart(){};
    virtual void AttackStartEnter(){};
    virtual void AttackStartExit(){};

    virtual void AttackCombo1(){};
    virtual void AttackCombo1Enter(){};
    virtual void AttackCombo1Exit(){};

    virtual void AttackCombo1End(){};
    virtual void AttackCombo1EndEnter(){};
    virtual void AttackCombo1EndExit(){};

    virtual void AttackCharge1(){};
    virtual void AttackCharge1Enter(){};
    virtual void AttackCharge1Exit(){};

    virtual void AttackCharge1Start(){};
    virtual void AttackCharge1StartEnter(){};
    virtual void AttackCharge1StartExit(){};

    virtual void AttackCharge1End(){};
    virtual void AttackCharge1EndEnter(){};
    virtual void AttackCharge1EndExit(){};

    virtual void AttackCharge2(){};
    virtual void AttackCharge2Enter(){};
    virtual void AttackCharge2Exit(){};

    virtual void AttackCharge2Start(){};
    virtual void AttackCharge2StartEnter(){};
    virtual void AttackCharge2StartExit(){};

    virtual void AttackCharge3(){};
    virtual void AttackCharge3Enter(){};
    virtual void AttackCharge3Exit(){};

    virtual void AttackCharge3Start(){};
    virtual void AttackCharge3StartEnter(){};
    virtual void AttackCharge3StartExit(){};

    virtual void AttackCharge3End(){};
    virtual void AttackCharge3EndEnter(){};
    virtual void AttackCharge3EndExit(){};

    virtual void JumpAttack(){};
    virtual void JumpAttackEnter(){};
    virtual void JumpAttackExit(){};

    virtual void JumpAttackStart(){};
    virtual void JumpAttackStartEnter(){};
    virtual void JumpAttackStartExit(){};

    virtual void DropObject();
    virtual void DropObjectEnter();
    virtual void DropObjectExit();

    virtual void DropObjectStart(){};
    virtual void DropObjectStartEnter();
    virtual void DropObjectStartExit();

    virtual void ChangeObject();
    virtual void ChangeObjectEnter();
    virtual void ChangeObjectExit();

    virtual void ChangeObjectEnd(){};
    virtual void ChangeObjectEndEnter();
    virtual void ChangeObjectEndExit();

public:
    virtual CKirbyObject* Clone() = 0;

public:
    CKirbyObject();
    virtual ~CKirbyObject();
};
