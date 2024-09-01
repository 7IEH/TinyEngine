#pragma once
#include "CKirbyAbility.h"

class CKirbyAbility_Sword : public CKirbyAbility
{
private:
    Ptr<CPrefab> m_KirbySwordSlashPref;
    Vec3 m_PrevWeaponScale;
    Vec3 m_BigWeaponScale;
    float m_PrevSpeed;
    float m_PrevRotSpeed;
    float m_PrevGravity;
    float m_AccTime;
    bool m_bFrmEnter;

public:
    // Idle
    virtual void IdleEnter() override;

    // Į�ֵθ��� (Lv0)
    virtual void Attack() override;
    virtual void AttackEnter() override;
    virtual void AttackExit() override;

    // Į�ֵθ��� (Lv1)
    virtual void AttackCombo1() override;
    virtual void AttackCombo1Enter() override;
    virtual void AttackCombo1Exit() override;

    // Į�ֵθ��� (Lv2)
    virtual void AttackCombo2() override;
    virtual void AttackCombo2Enter() override;
    virtual void AttackCombo2Exit() override;

    // ���ɰ��� (Lv1)
    virtual void AttackCharge1() override;
    virtual void AttackCharge1Enter() override;
    virtual void AttackCharge1Exit() override;

    virtual void AttackCharge1Start() override;
    virtual void AttackCharge1StartEnter() override;
    virtual void AttackCharge1StartExit() override;

    virtual void AttackCharge1End() override;
    virtual void AttackCharge1EndEnter() override;
    virtual void AttackCharge1EndExit() override;

    // ���ɰ��� (Lv2)
    virtual void AttackCharge2() override;
    virtual void AttackCharge2Enter() override;
    virtual void AttackCharge2Exit() override;

    virtual void AttackCharge2Start() override;
    virtual void AttackCharge2StartEnter() override;
    virtual void AttackCharge2StartExit() override;

    // ������ (Lv3)
    virtual void AttackCharge3() override;
    virtual void AttackCharge3Enter() override;
    virtual void AttackCharge3Exit() override;

    virtual void AttackCharge3Start() override;
    virtual void AttackCharge3StartEnter() override;
    virtual void AttackCharge3StartExit() override;

    virtual void AttackCharge3End() override;
    virtual void AttackCharge3EndEnter() override;
    virtual void AttackCharge3EndExit() override;

    // ����
    virtual void JumpFallEnter() override;

    // ��������
    virtual void JumpAttack() override;
    virtual void JumpAttackEnter() override;
    virtual void JumpAttackExit() override;

    virtual void JumpAttackStart() override;
    virtual void JumpAttackStartEnter() override;
    virtual void JumpAttackStartExit() override;

    // ����
    virtual void LandingEnter() override;
    virtual void LandingExit() override;

    // ����
    virtual void GuardEnter() override;

    virtual void GuardRun() override;
    virtual void GuardRunEnter() override;
    virtual void GuardRunExit() override;

    // �����̵� ���
    virtual void Slide() override;
    virtual void SlideEnter() override;
    virtual void SlideExit() override;

    virtual void SlideStart() override;
    virtual void SlideStartEnter() override;
    virtual void SlideStartExit() override;

    virtual void SlideEnd() override;
    virtual void SlideEndEnter() override;
    virtual void SlideEndExit() override;

    // �����̵� �޺�����
    virtual void SlideAttack() override;
    virtual void SlideAttackEnter() override;
    virtual void SlideAttackExit() override;

    // ���� ����
    virtual void ChangeAbility() override;
    virtual void ChangeAbilityEnter() override;
    virtual void ChangeAbilityExit() override;

private:
    void SpawnSwordSlash(Vec3 _SlashScale, bool _bVertical = false);

public:
    CLONE(CKirbyAbility_Sword)
    CKirbyAbility_Sword();
    CKirbyAbility_Sword(const CKirbyAbility_Sword& _Origin);
    virtual ~CKirbyAbility_Sword();
};