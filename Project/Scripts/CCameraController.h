#pragma once

#include <Engine/CScript.h>

enum class CameraSetup
{
    NORMAL,
    PROGRESS,
    TWOTARGET,
};

class CCameraController : public CScript
{
private:
    CameraSetup                 m_Setup;             // ���� ī�޶��� Setup

    // Target
    CGameObject*                m_Target;               // ī�޶� �ٶ�����ϴ� Ÿ��
    Vec3                        m_TargetPos;            // ���� �����ӿ��� Ÿ���� ��ġ
    Vec3                        m_Offset;               // Ÿ�����κ����� ������

    CGameObject*                m_SubTarget;
    Vec3                        m_SubTargetPos;

    // ���� ��
    Vec3                        m_LookEyePos;           // ī�޶� �־�� �ϴ� ��ġ
    Vec3                        m_LookAtPos;            // ī�޶� �ٶ�����ϴ� ��ġ
    Vec3                        m_LookDir;              // ī�޶� �ٶ󺸴� ����
    float                       m_LookDist;             // ī�޶�� Ÿ�� ������ �Ÿ�

    float                       m_MinSpeed;             // ī�޶��� �ּ� ���ǵ�
    float                       m_MaxSpeed;             // ī�޶��� �ִ� ���ǵ�
    float                       m_ThresholdDistance;    // ī�޶� �ִ� ���ǵ�� �̵��ϱ� ���� �Ӱ� �Ÿ�

    float                       m_RotationSpeed;        // ȸ�� �ӵ�
    float                       m_ZoomSpeed;            // Zoom �ӵ�

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
    
    //Edit
    bool                        m_EditMode;             // EditMode ����ġ
    float                       m_EditRotSpeed;         // EditMode������ ī�޶� ȸ�� ���� �ӵ�
    float                       m_EditZoomSpeed;        // EditMode������ ī�޶� �Ÿ� ���� �ӵ�

 public:
    CameraSetup GetCameraSetup() const { return m_Setup; }
    CGameObject* GetMainTarget() const { return m_Target; }
    CGameObject* GetSubTarget() const { return m_SubTarget; }
    Vec3 GetOffset() const { return m_Offset; }
    Vec3 GetLookDir() const { return m_LookDir; }
    float GetLookDist() const { return m_LookDist; }
    float GetMinSpeed() const { return m_MinSpeed; }
    float GetMaxSpeed() const { return m_MaxSpeed; }
    float GetThresholdDistance() const { return m_ThresholdDistance; }
    float GetRotationSpeed() const { return m_RotationSpeed; }
    float GetZoomSpeed() const { return m_ZoomSpeed; }

    void SetCameraSetup(CameraSetup _Setup) { m_Setup = _Setup; }
    void SetMainTarget(CGameObject* _Target) { m_Target = _Target; } 
    void SetMainTarget(wstring _TargetName);
    void SetSubTarget(CGameObject* _SubTarget) { m_SubTarget = _SubTarget; }
    void SetSubTarget(wstring _TargetName);
    void SetOffset(Vec3 _Offset) { m_Offset = _Offset; }
    void SetLookDir(Vec3 _LookDir) { m_LookDir = _LookDir; }
    void SetLookDist(float _LookDist) { m_LookDist = _LookDist; }
    void SetMinSpeed(float _MinSpeed) { m_MinSpeed = _MinSpeed; }
    void SetMaxSpeed(float _MaxSpeed) { m_MaxSpeed = _MaxSpeed; }
    void SetThresholdDistance(float _Threshold) { m_ThresholdDistance = _Threshold; }
    void SetRotationSpeed(float _RotationSpeed) { m_RotationSpeed = _RotationSpeed; }
    void SetZoomSpeed(float _ZoomSpeed) { m_ZoomSpeed = _ZoomSpeed; }
    void SetDistanceOffset(float _DistOffset) { m_DistanceOffset = _DistOffset; } 

public:
    virtual void begin() override;
    virtual void tick() override;

private:
    void UpdateTargetPos(); 
    void SetUpProc();
    void ApplyOffset();
    void UpdateLookAtPos();
    void UpdateLookDir();
    void UpdateLookDistance();
    
    Vec3 CalCamPos(Vec3 _TargetWorldPos, Vec3 _LookDir, float _CamDist);
    
    void Normal();
    void Progress();
    void TwoTarget();
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

public:
    virtual UINT SaveToLevelFile(FILE* _File) override;
    virtual UINT LoadFromLevelFile(FILE* _File) override;

public:
    CLONE(CCameraController);
    CCameraController();
    virtual ~CCameraController();
};
