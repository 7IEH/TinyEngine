#pragma once
#include "CFSMScript.h"

enum class ElfilisG_Idle
{
    Idle,
    PUBLIC,
};

enum class ElfilisG_Move
{
    Teleport,
    PortalSlash,
    BackStep,
    PUBLIC,
};

enum class ElfilisG_MoveAtk
{
    TeleportNormalATK,
    PUBLIC,

    // �θ���� ���� �� random�� ���Ե��� �ʴ� State�� (�̹� �������� ���� ���� ����)
    TeleportRotSpear,
};

enum class ElfilisG_Atk
{
    NormalATK,
    RotSpear,
    PUBLIC,

    SwordWave,
    SwordWaveTornado,
    PortalSpear,
};

enum class ElfilisG_ToAir
{
    ToAir,
    PUBLIC,
};

enum class ElfilisA_Idle
{
    Idle,
    PUBLIC,
};

enum class ElfilisA_Move
{
    MoveSide,
    PUBLIC,
};

enum class ElfilisA_SmallAtk
{
    ThrowSpear,
    PortalLazer,
    PUBLIC,
};

enum class ElfilisA_BigAtk
{
    PUBLIC,

    DrawLazer,
    PortalStar,
    RotSpear,
    BigElfilisCombo, //? �̰� BigAtk�� �ƴ϶� BigElfilisAtkState�� ���� �ľ��ϳ�
};

enum class ElfilisA_ToGround
{
    Teleport,
    TeleportCombo,
    PUBLIC,

    FlyDown,
    FlyDownCombo,
    FlyScratch,
};

enum class ElfilisStateGroup
{
    GrondIdle,
    GroundMove,
    GroundMoveAtk,
    GroundAtk,
    GroundToAir,

    AirIdle,
    AirMove,
    AirSmallAtk,
    AirBigAtk,
    AirToGround,

    END,
};

// ==============================================

class CElfilisFSM : public CFSMScript
{
private:
    map<ElfilisStateGroup, vector<wstring>[2]> m_StateGroup; // ( StateGroup, {{PublicStates}, {PrivateStates}} )
    ElfilisStateGroup m_CurStateGroup;
    UINT m_GroundAttackCount;
    bool m_bAttackRepeat;

public:
    void ChangeStateGroup_RandState(ElfilisStateGroup _Group);
    void ChangeStateGroup_SetState(ElfilisStateGroup _Group, const wstring& _State);
    void RepeatState();
    ElfilisStateGroup FindNextStateGroup();

public:
    virtual void begin() override;
    virtual void tick() override;

public:
    ElfilisStateGroup GetCurStateGroup() const { return m_CurStateGroup; }
    const vector<wstring>& GetCurPublicStates() const;

private:
    void ChangStateGroup(ElfilisStateGroup _Group);

private:
    void AddGroupPublicState(ElfilisStateGroup _Group, const wstring& _StateName, CState* _State);
    void AddGroupPrivateState(ElfilisStateGroup _Group, const wstring& _StateName, CState* _State);

public:
    virtual UINT SaveToLevelFile(FILE* _File) override;
    virtual UINT LoadFromLevelFile(FILE* _File) override;

    CLONE(CElfilisFSM)
    CElfilisFSM();
    CElfilisFSM(const CElfilisFSM& _Origin);
    virtual ~CElfilisFSM();
};