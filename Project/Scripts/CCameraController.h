#pragma once

#include <Engine/CScript.h>

enum class EFFECT_TYPE
{
    TILT_ANGLE,
    SHAKE,
    END,
};

struct CamEffect
{
    EFFECT_TYPE EffetType;

    bool Running;
    float Acc;
    float Duration;
};

enum class CameraSetup
{
    NORMAL,
    PROGRESS,
    TWOTARGET,
    BOSSSETUP,
};

struct CameraSetting
{
    Vec3 Offset; // LookAtPos�� ���� Offset

    Vec3 LookDir;    // ī�޶� �ٶ󺸴� ����
    float LookDist; // ī�޶�� Ÿ�� ������ �Ÿ�

    float MinSpeed;          // ī�޶��� �ּ� ���ǵ�
    float MaxSpeed;          // ī�޶��� �ִ� ���ǵ�
    float ThresholdDistance; // ī�޶� �ִ� ���ǵ�� �̵��ϱ� ���� �Ӱ� �Ÿ�

    float RotationSpeed; // ȸ�� �ӵ�

    float ZoomMinSpeed;  // Zoom �ּ� �ӵ�
    float ZoomMaxSpeed;  // Zoom �ִ� �ӵ�
    float ZoomThreshold; // Zoom �Ӱ� �Ÿ�
};

class CCameraController : public CScript
{
private:
    CameraSetting               m_InitSetting;
    CameraSetting               m_SaveSetting;

    CameraSetup                 m_Setup;             // ���� ī�޶��� Setup

    // Effect
    CamEffect                   m_Effect[(UINT)EFFECT_TYPE::END];
    float                       m_ShakeIntencity;
    Vec3                        m_ShakeFrequency;
    float                       m_TiltFrequency;

    // Target
    CGameObject*                m_Target;               // ī�޶� �ٶ�����ϴ� Ÿ��
    Vec3                        m_TargetPos;            // ���� �����ӿ��� Ÿ���� ��ġ
    Vec3                        m_TargetOffset;         // Ÿ�����κ����� ������

    CGameObject*                m_SubTarget;            // SubTarget
    Vec3                        m_SubTargetPos;         // SubTarget�� ��ġ
    Vec3                        m_SubTargetOffset;      // SubTarget Offset

    Vec3                        m_Offset;               // LookAtPos�� ���� Offset

    // ���� ��
    Vec3                        m_LookEyePos;           // ī�޶� �־�� �ϴ� ��ġ
    Vec3                        m_LookAtPos;            // ī�޶� �ٶ�����ϴ� ��ġ
    Vec3                        m_LookDir;              // ī�޶� �ٶ󺸴� ����
    Quat                        m_LookDirQuat;
    float                       m_LookDist;             // ī�޶�� Ÿ�� ������ �Ÿ�

    float                       m_MinSpeed;             // ī�޶��� �ּ� ���ǵ�
    float                       m_MaxSpeed;             // ī�޶��� �ִ� ���ǵ�
    float                       m_ThresholdDistance;    // ī�޶� �ִ� ���ǵ�� �̵��ϱ� ���� �Ӱ� �Ÿ�

    float                       m_RotationSpeed;        // ȸ�� �ӵ�

    float                       m_ZoomMinSpeed;         // Zoom �ּ� �ӵ�
    float                       m_ZoomMaxSpeed;         // Zoom �ִ� �ӵ�
    float                       m_ZoomThreshold;        // Zoom �Ӱ� �Ÿ�

    // ���� ������
    Vec3                        m_CurLookDir;           // ���� �����ӿ��� ī�޶��� ����
    Vec3                        m_CurLookAtPos;         // ���� �����ӿ��� ī�޶� �ٶ�����ϴ� ��ġ
    float                       m_CurDistance;          // ���� �����ӿ��� ī�޶�� LookAt�� �Ÿ�

    // ���� ������
    Vec3                        m_PrevLookDir;          // ���� �����ӿ��� ī�޶��� ����
    Vec3                        m_PrevLookAtPos;        // ���� �����ӿ��� ī�޶� �ٶ�����ϴ� ��ġ
    float                       m_PrevDistance;         // ���� �����ӿ��� ī�޶�� LookAt�� �Ÿ�

    // Progress Setup
    Vec3                        m_ProgressStartOffset;
    Vec3                        m_ProgressEndOffset;
    Vec3                        m_ProgressStartPos;
    Vec3                        m_ProgressStartDir;
    float                       m_ProgressStartDist;
    Vec3                        m_ProgressEndPos;
    Vec3                        m_ProgressEndDir;
    float                       m_ProgressEndDist;

    // TwoTarget
    float                       m_DistanceOffset;       // �� ��ü�� Pos�� �������� ��� ������ �� ��ü�� �Ž��� ������ ���̷��� Offset ��ŭ Distance�� �� ����� �Ѵ�.
    float                       m_Weight;               // �� Ÿ���� ��� ������ ���� ����ġ 0(Main Target) ~ 1(SubTarget)
    float                       m_MinDegreeX;            // Weight�� 0�� ��(Kirby�� �� ��)�� X�� ȸ�� ����
    float                       m_MaxDegreeX;            // Weight�� 1�� ��(SubTarget�� �� ��)�� X�� ȸ�� ����
    float                       m_MinDegreeY;            // Weight�� 0�� ��(Kirby�� �� ��)�� X�� ȸ�� ����
    float                       m_MaxDegreeY;            // Weight�� 1�� ��(SubTarget�� �� ��)�� X�� ȸ�� ����
    float                       m_MaxBetweenTargetDist;    

    //Edit
    bool                        m_EditMode;             // EditMode ����ġ
    float                       m_EditRotSpeed;         // EditMode������ ī�޶� ȸ�� ���� �ӵ�
    float                       m_EditZoomSpeed;        // EditMode������ LookDist ���� �ӵ�(���� Zoom�ӵ��� �޶����� ����)

 public:
    CameraSetup GetCameraSetup() const { return m_Setup; }
    CGameObject* GetMainTarget() const { return m_Target; }
    CGameObject* GetSubTarget() const { return m_SubTarget; }
    Vec3 GetOffset() const { return m_Offset; }
    Vec3 GetMainTargetOffset() const { return m_TargetOffset; }
    Vec3 GetSubTargetOffset() const { return m_SubTargetOffset; }
    Vec3 GetLookDir() const { return m_LookDir; }
    float GetLookDist() const { return m_LookDist; }
    float GetMinSpeed() const { return m_MinSpeed; }
    float GetMaxSpeed() const { return m_MaxSpeed; }
    float GetThresholdDistance() const { return m_ThresholdDistance; }
    float GetRotationSpeed() const { return m_RotationSpeed; }
    float GetZoomMinSpeed() const { return m_ZoomMinSpeed; }
    float GetZoomMaxSpeed() const { return m_ZoomMaxSpeed; }
    float GetZoomThreshold() const { return m_ZoomThreshold; }

    void SetCameraSetup(CameraSetup _Setup) { m_Setup = _Setup; }
    void SetMainTarget(CGameObject* _Target) { m_Target = _Target; } 
    void SetMainTarget(wstring _TargetName);
    void SetSubTarget(CGameObject* _SubTarget) { m_SubTarget = _SubTarget; }
    void SetSubTarget(wstring _TargetName);
    void SetOffset(Vec3 _Offset) { m_Offset = _Offset; }
    void SetTargetOffset(Vec3 _TargetOffset) { m_TargetOffset = _TargetOffset; }
    void SetSubTargetOffset(Vec3 _TargetOffSet) { m_SubTargetOffset = _TargetOffSet; }
    void SetLookDir(Vec3 _LookDir) { m_LookDir = _LookDir; }
    void SetLookDist(float _LookDist) { m_LookDist = _LookDist; }
    void SetMinSpeed(float _MinSpeed) { m_MinSpeed = _MinSpeed; }
    void SetMaxSpeed(float _MaxSpeed) { m_MaxSpeed = _MaxSpeed; }
    void SetThresholdDistance(float _Threshold) { m_ThresholdDistance = _Threshold; }
    void SetRotationSpeed(float _RotationSpeed) { m_RotationSpeed = _RotationSpeed; }
    void SetZoomMinSpeed(float _MinSpeed) { m_ZoomMinSpeed = _MinSpeed; }
    void SetZoomMaxSpeed(float _MaxSpeed) { m_ZoomMaxSpeed = _MaxSpeed; }
    void SetZoomThreshold(float _Threshold) { m_ZoomThreshold = _Threshold; } 
    void SetDistanceOffset(float _DistOffset) { m_DistanceOffset = _DistOffset; } 

    void SaveSetting();
    void SaveInitSetting();
    void LoadSetting();
    void LoadInitSetting();

public:
    virtual void begin() override;
    virtual void tick() override;

private:
    void UpdateTargetPos(); 
    void ApplyTargetOffset();
    void SetUpProc();
    void ApplyOffset();
    void UpdateLookAtPos();
    void UpdateLookDir();
    void UpdateLookDistance();
    
    Vec3 CalCamPos(Vec3 _TargetWorldPos, Vec3 _LookDir, float _CamDist);
    
    // Proc
    void Normal();
    void Progress();
    void TwoTarget();
    void Boss();

    void ProcessEffet();

    // EditMode
    void EditMode();

public:
    void ResetCamera(); // ������ ��������� ī�޶� �ٷ� �̵��Ѵ�.
    void ChangeMainTarget(CGameObject* _Target); // Target ���� �� ī�޶� ���� �̵�
    void ChangeMainTarget(wstring _TargetName); // Target ���� �� ī�޶� ���� �̵� 
    void ChangeLookSetting(Vec3 _LookDir, float _LookDist); // Look Setting ����
    void ChangeFollwSpeedSetting(float _MinSpeed, float _MaxSpeed, float _Threshold); // Follow Setting ����
    void ProgressSetup(Vec3 _StartPos, Vec3 _EndPos,Vec3 _StartOffset, Vec3 _EndOffset, Vec3 _StartDir, Vec3 _EndDir, float _StartDist, float _EndDist); // Progress�� Camera Setup ���� ����
    void TwoTarget(CGameObject* _SubTarget, bool _bChangeLookDir, Vec3 _LookDir, float _DistanceOffset);
    void TwoTarget(wstring _SubTargetName, Vec3 _LookDir, float _DistanceOffset);
    void Boss(CGameObject* _SubTarget, float _DistanceOffset, float _MinDegree, float _MaxDegree, float _m_MaxBetweenTargetDist,
                      float _Weight = 0.5f);
    void Boss(wstring _SubTargetName, float _DistanceOffset, float _MinDegree, float _MaxDegree, float _m_MaxBetweenTargetDist,
                      float _Weight = 0.5f);
    void Shake(float _Duration, float _Frequency, float _Intencity);
    void Tilt(float _Duration, float _Frequency);


public:
    virtual UINT SaveToLevelFile(FILE* _File) override;
    virtual UINT LoadFromLevelFile(FILE* _File) override;

public:
    CLONE(CCameraController);
    CCameraController();
    virtual ~CCameraController();
};
