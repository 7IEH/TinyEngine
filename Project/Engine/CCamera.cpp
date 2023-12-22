#include "pch.h"
#include "CCamera.h"

#include "CDevice.h"
#include "CTransform.h"

CCamera::CCamera()
    : CComponent(COMPONENT_TYPE::CAMERA)
    , m_ProjType(PROJ_TYPE::ORTHOGRAPHIC)
    , m_FOV(XM_PI / 2.f)
    , m_Width(0.f)
    , m_Scale(1.f)
    , m_AspectRatio(1.f)
    , m_Near(1.f)
    , m_Far(10000.f)
{
    Vec2 vResol = CDevice::GetInst()->GetRenderResolution();
    m_AspectRatio = vResol.x / vResol.y;
}

CCamera::~CCamera()
{
}

void CCamera::finaltick()
{
    // �������� ȭ��� ���
    Vec2 vResol = CDevice::GetInst()->GetRenderResolution();
    m_Width = vResol.x;
    m_AspectRatio = vResol.x / vResol.y;

    // �� ����� ����Ѵ�.
    // ī�޶� �������� �̵���Ű�� �̵� ���
    Vec3 vCamPos = Transform()->GetRelativePos();
    Matrix matTrans = XMMatrixTranslation(-vCamPos.x, -vCamPos.y, -vCamPos.z);

    //// ī�޶��� �� ��, ��, �� �� ������ �������̶� ��ġ��Ű���� ȸ���ϴ� ȸ�����
    // Vec3 vRight = Transform()->GetDir(DIR_TYPE::RIGHT);
    // Vec3 vUp = Transform()->GetDir(DIR_TYPE::UP);
    // Vec3 vFront = Transform()->GetDir(DIR_TYPE::FRONT);

    // Matrix matRotate = XMMatrixIdentity();
    // matRotate._11 = vRight.x;
    // matRotate._12 = vUp.x;
    // matRotate._13 = vFront.x;

    // matRotate._21 = vRight.y;
    // matRotate._22 = vUp.y;
    // matRotate._23 = vFront.y;

    // matRotate._31 = vRight.z;
    // matRotate._32 = vUp.z;
    // matRotate._33 = vFront.z;

    Vec3 vCamRot = Transform()->GetRelativeRotation();
    Matrix matRotate =
        Matrix::CreateRotationX(vCamRot.x) * Matrix::CreateRotationY(vCamRot.y) * Matrix::CreateRotationZ(vCamRot.z);
    matRotate = matRotate.Transpose(); // ��������� ������� ��ġ���

    // �̵� x ȸ�� = view ���
    m_matView = matTrans * matRotate;

    // ���� ��Ŀ� ���� ���� ����� ����Ѵ�.
    m_matProj = XMMatrixIdentity();

    if (PROJ_TYPE::ORTHOGRAPHIC == m_ProjType)
    {
        // ��������
        Vec2 vResol = CDevice::GetInst()->GetRenderResolution();
        m_matProj = XMMatrixOrthographicLH(vResol.x * m_Scale, (vResol.x / m_AspectRatio) * m_Scale, m_Near, m_Far);
    }
    else
    {
        // ��������
        m_matProj = XMMatrixPerspectiveFovLH(m_FOV, m_AspectRatio, m_Near, m_Far);
    }

    // ����� view ��İ� proj ����� ���������� ��Ƶд�.
    g_Transform.matView = m_matView;
    g_Transform.matProj = m_matProj;
}
