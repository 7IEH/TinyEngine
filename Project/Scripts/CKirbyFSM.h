#pragma once
#include "CFSMScript.h"

enum class LastJumpType
{
    LOW,
    HIGH,
};

enum class DodgeType
{
    NONE,
    FRONT,
    RIGHT,
    BACK,
    LEFT,
};
    
class CKirbyAbility;
class CKirbyObject;
class CKirbyVacuumCollider;

class CKirbyFSM : public CFSMScript
{
private:
    CKirbyAbility*          m_arrAbility[(UINT)AbilityCopyType::END];
    AbilityCopyType         m_CurAbility;
    AbilityCopyType         m_NextAbility;

    CKirbyObject*           m_arrObject[(UINT)ObjectCopyType::END];
    ObjectCopyType          m_CurObject;
    
    CGameObject*            m_CurHat;
    CGameObject*            m_CurWeapon;
    CGameObject*            m_StuffedCopyObj;
    CKirbyVacuumCollider*   m_VacuumCollider;

    // ���� ������ ���� ����
    const float             m_HoveringLimitTime;
    float                   m_HoveringAccTime;
    bool                    m_bHovering;

    UINT                    m_ComboLevel;
    float                   m_ComboAccTime;
    float                   m_ChargeAccTime;
    UINT                    m_SlideComboLevel;

    LastJumpType            m_LastJump;
    DodgeType               m_DodgeType;
    bool                    m_bStuffed;

    Vec3                    m_KnockbackDir;
    float                   m_InvincibleAcc;
    float                   m_InvincibleDuration;
    float                   m_EmissiveCoef;
    bool                    m_bInvincible;

    float                   m_GlidingDuration;
    float                   m_GlidingAcc;

    float                   m_YPressedTime;
    float                   m_DropCopyTime;
    bool                    m_bDroppable;

public:
    void begin() override;
    void tick() override;

    void ChangeAbilityCopy(AbilityCopyType _Type);
    void ChangeObjectCopy(ObjectCopyType _Type);
    void StartStuffed(CGameObject* _Target);
    void DrawingCollisionEnter(CGameObject* _CollisionObject);
    void AddGlidingTime(float _Time) { m_GlidingAcc += _Time; }

public:
    void SetCurHat(CGameObject* _Hat);
    void SetCurWeapon(CGameObject* _Weapon);
    void SetComboLevel(int _Level) { m_ComboAccTime = 0.f; m_ComboLevel = _Level; }
    void SetLastJump(LastJumpType _Type) { m_LastJump = _Type; }
    void SetHovering(bool _bHovering);
    void SetDodgeType(DodgeType _Type) { m_DodgeType = _Type; }
    void SetSlideComboLevel(UINT _bCombo) { m_SlideComboLevel = _bCombo; }
    void SetKnockBackDir(Vec3 _Dir) { m_KnockbackDir = _Dir; }
    void SetInvincible() { m_bInvincible = true;  m_InvincibleAcc = 0.f;}
    void SetInvincible(bool _bInvicinvle) { } //@TODO ���ľ���
    void SetDroppable(bool _bDroppable) { m_bDroppable = _bDroppable; }
    void ClearCurHatWeapon();
    void ClearChargeAccTime() { m_ChargeAccTime = 0.f; }
    void ClearHoveringAccTime() { m_HoveringAccTime = 0.f; }
    void ClearStuff();
    void ClearGlidingTime() { m_GlidingAcc = 0.f; }
    void ClearYPressedTime() { m_YPressedTime = 0.f; }

    CKirbyAbility* GetCurAbility() const { return m_arrAbility[(UINT)m_CurAbility]; }
    CKirbyAbility* GetNextAbility() const { return m_arrAbility[(UINT)m_NextAbility]; } 
    CKirbyObject* GetCurObject() const { return m_arrObject[(UINT)m_CurObject]; }
    AbilityCopyType GetCurAbilityIdx() const { return m_CurAbility; }
    ObjectCopyType GetCurObjectIdx() const { return m_CurObject; }
    CKirbyVacuumCollider* GetVacuumCol() const { return m_VacuumCollider; }
    LastJumpType GetLastJump() const { return m_LastJump; }
    CGameObject* GetCurHat() { return m_CurHat; }
    CGameObject* GetCurWeapon() { return m_CurWeapon; }
    CGameObject* GetStuffedCopyObj() { return m_StuffedCopyObj; }
    DodgeType GetDodgeType() const { return m_DodgeType; }
    float GetComboAccTime() const { return m_ComboAccTime; }
    float GetChargeAccTime() const { return m_ChargeAccTime; }
    float GetHoveringAccTime() const { return m_HoveringAccTime; }
    float GetHoveringLimitTime() const { return m_HoveringLimitTime; }
    UINT GetComboLevel() const { return m_ComboLevel; }
    UINT GetSlideComboLevel() const { return m_SlideComboLevel; }
    float GetGlidingDuration() const { return m_GlidingDuration; }
    float GetCurGlidingTime() const { return m_GlidingAcc; }
    float GetYPressedTime() const { return m_YPressedTime; }
    float GetDropCopyTime() const { return m_DropCopyTime; }
    bool IsStuffed() const { return m_bStuffed; }
    bool IsHovering() const { return m_bHovering; }
    bool IsDrawing() const;
    bool IsInvincible() const { return m_bInvincible; }
    Vec3 GetKnockBackDir() const { return m_KnockbackDir; }

public:
    virtual void SaveToLevelFile(FILE* _File) override;
    virtual void LoadFromLevelFile(FILE* _File) override;

public:
    CLONE(CKirbyFSM)
    CKirbyFSM();
    CKirbyFSM(const CKirbyFSM& _Origin);
    virtual ~CKirbyFSM();

    friend class CKirbyRun;
};
