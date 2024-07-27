#pragma once

#include <Engine/CScript.h>

enum class CameraSetup
{
    NORMAL,
    PROGRESS,
};

class CCameraController : public CScript
{
private:
    CameraSetup                 m_CurSetup;             // ���� ī�޶��� Setup

    // Target
    CGameObject*                m_Target;               // ī�޶� �ٶ�����ϴ� Ÿ��
    Vec3                        m_TargetPos;            // ���� �����ӿ��� Ÿ���� ��ġ
    Vec3                        m_Offset;               // Ÿ�����κ����� ������

    // ���� ��
    Vec3                        m_LookEyePos;           // ī�޶� �־�� �ϴ� ��ġ
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
    Vec3                        m_ProgressStartPos;
    Vec3                        m_ProgressStartDir;
    float                       m_ProgressStartDist;
    Vec3                        m_ProgressEndPos;
    Vec3                        m_ProgressEndDir;
    float                       m_ProgressEndDist;
    
    //Edit
    bool                        m_EditMode;             // EditMode ����ġ
    float                       m_EditRotSpeed;         // EditMode������ ī�޶� ȸ�� ���� �ӵ�
    float                       m_EditZoomSpeed;        // EditMode������ ī�޶� �Ÿ� ���� �ӵ�

public:
    virtual void begin() override;
    virtual void tick() override;


private:
    void SetUpProc();
    void UpdateTargetPos(); 
    void UpdateLookAtPos();
    void UpdateLookDir();
    void UpdateLookDistance();
    
    Vec3 CalCamPos(Vec3 _TargetWorldPos, Vec3 _LookDir, float _CamDist);
    
    // EditMode
    void EditMode();
    void Progress();

public:
    //void ChangeTarget();
    //void ChangeSetting(Vec3 _LookDir, float CamDist, );
    //void ProgressSetup(Vec3 _StartPos, Vec3 _EndPos, Vec3 _StartDir, Vec3 _EndDir, float _StartDist, float _EndDist);


public:
    virtual UINT SaveToLevelFile(FILE* _File) override;
    virtual UINT LoadFromLevelFile(FILE* _File) override;

public:
    CLONE(CCameraController);
    CCameraController();
    virtual ~CCameraController();
};
