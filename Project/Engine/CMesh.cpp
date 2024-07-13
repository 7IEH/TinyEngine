#include "pch.h"
#include "CMesh.h"

#include "CAssetMgr.h"
#include "CLevelMgr.h"

#include "CLevel.h"
#include "CLayer.h"

#include "CDevice.h"
#include "CInstancingBuffer.h"

#include "CTexture.h"
#include "CAnimator.h"

CMesh::CMesh(bool _bEngineAsset)
    : CAsset(ASSET_TYPE::MESH, _bEngineAsset)
    , m_VB{}
    , m_VBDesc{}
    , m_VtxCount(0)
    , m_VtxSysMem(nullptr)
    , m_vecIdxInfo{}
    , m_vecAnimClip{}
    , m_vecBones{}
    , vecBoneSocket{}
    , m_pBoneFrameData(nullptr)
    , m_pBoneOffset(nullptr)
{
}

CMesh::~CMesh()
{
    if (nullptr != m_VtxSysMem)
    {
        delete m_VtxSysMem;
        m_VtxSysMem = nullptr;
    }

    for (size_t i = 0; i < m_vecIdxInfo.size(); ++i)
    {
        if (nullptr != m_vecIdxInfo[i].pIdxSysMem)
        {
            delete m_vecIdxInfo[i].pIdxSysMem;
            m_vecIdxInfo[i].pIdxSysMem = nullptr;
        }
    }

    for (UINT i = 0; i < (UINT)m_vecBones.size(); ++i)
    {
        Delete_Vec(m_vecBones[i].vecBoneSocket);
    }

    if (nullptr != m_pBoneFrameData)
    {
        delete m_pBoneFrameData;
        m_pBoneFrameData = nullptr;
    }

    if (nullptr != m_pBoneOffset)
    {
        delete m_pBoneOffset;
        m_pBoneOffset = nullptr;
    }
}

void CMesh::UpdateData(UINT _iSubset)
{
    UINT iStride = sizeof(Vtx);
    UINT iOffset = 0;

    CONTEXT->IASetVertexBuffers(0, 1, m_VB.GetAddressOf(), &iStride, &iOffset);
    CONTEXT->IASetIndexBuffer(m_vecIdxInfo[_iSubset].pIB.Get(), DXGI_FORMAT_R32_UINT, 0);
}

void CMesh::UpdateData_Inst(UINT _iSubset)
{
    if (_iSubset >= m_vecIdxInfo.size())
        assert(nullptr);

    ID3D11Buffer* arrBuffer[2] = {m_VB.Get(), CInstancingBuffer::GetInst()->GetBuffer().Get()};
    UINT iStride[2] = {sizeof(Vtx), sizeof(tInstancingData)};
    UINT iOffset[2] = {0, 0};

    CONTEXT->IASetVertexBuffers(0, 2, arrBuffer, iStride, iOffset);
    CONTEXT->IASetIndexBuffer(m_vecIdxInfo[_iSubset].pIB.Get(), DXGI_FORMAT_R32_UINT, 0);
}

void CMesh::AddBoneSocket(int _BoneIndex, tBoneSocket* _BoneSocket)
{
    if (_BoneIndex <= -1 || _BoneIndex >= m_vecBones.size() || nullptr == _BoneSocket)
    {
        return;
    }

    _BoneSocket->BoneIndex = _BoneIndex;
    m_vecBones[_BoneIndex].vecBoneSocket.push_back(_BoneSocket);
    vecBoneSocket.push_back(_BoneSocket);
}

void CMesh::RemoveBoneSocket(int _BoneIndex, tBoneSocket* _BoneSocket)
{
    if (_BoneIndex <= -1 || _BoneIndex >= m_vecBones.size() || nullptr == _BoneSocket)
    {
        return;
    }

    m_vecBones[_BoneIndex].vecBoneSocket.erase(
        remove(m_vecBones[_BoneIndex].vecBoneSocket.begin(), m_vecBones[_BoneIndex].vecBoneSocket.end(), _BoneSocket),
        m_vecBones[_BoneIndex].vecBoneSocket.end());

    vecBoneSocket.erase(remove(vecBoneSocket.begin(), vecBoneSocket.end(), _BoneSocket), vecBoneSocket.end());

    // ������ ��� ������Ʈ�� ��ȸ�Ͽ� �ش� BoneSocket�� �����ϰ� �ִ� �ڽĿ�����Ʈ ����
    CLevel* pCurLevel = CLevelMgr::GetInst()->GetCurrentLevel();
    for (UINT i = 0; i < LAYER_MAX; i++)
    {
        const vector<CGameObject*>& vecParentObj = pCurLevel->GetLayer(i)->GetParentObjects();

        for (const auto& ParentObj : vecParentObj)
        {
            list<CGameObject*> queue;
            queue.push_back(ParentObj);

            while (!queue.empty())
            {
                CGameObject* pObject = queue.front();
                queue.pop_front();

                const vector<CGameObject*>& vecChildObj = pObject->GetChildObject();

                for (size_t i = 0; i < vecChildObj.size(); ++i)
                {
                    queue.push_back(vecChildObj[i]);
                }

                // BoneSocket ���� ����
                if (pObject->GetBoneSocket() == _BoneSocket)
                {
                    pObject->SetBoneSocket(nullptr);
                }
            }
        }
    }
}

tBoneSocket* CMesh::GetBoneSocket(const wstring& _SocketName)
{
    for (tBoneSocket* BoneSocket : vecBoneSocket)
    {
        if (_SocketName == BoneSocket->SoketName)
            return BoneSocket;
    }

    return nullptr;
}

bool CMesh::LoadAnimationFBX(const wstring& _strPath)
{
    CFBXLoader loader;
    loader.init();
    loader.LoadFbx(_strPath);

    // �޽� ��������
    Ptr<CMesh> pAinmMesh = CMesh::CreateFromContainer(loader);

    // ����ó��
    if (nullptr == pAinmMesh)
    {
        MessageBox(nullptr, L"Mesh�� �������� �ʽ��ϴ�.", L"�ִϸ��̼� �ε� ����", MB_ICONHAND);
        return false;
    }
    if (!pAinmMesh->IsAnimMesh())
    {
        MessageBox(nullptr, L"�ִϸ��̼� Mesh�� �ƴմϴ�.", L"�ִϸ��̼� �ε� ����", MB_ICONHAND);
        return false;
    }
    if (GetBoneCount() != pAinmMesh->GetBoneCount())
    {
        MessageBox(nullptr, L"������ Bone�� �ƴմϴ�.", L"�ִϸ��̼� �ε� ����", MB_ICONHAND);
        return false;
    }

    // ������ �� ���� üũ
    const vector<tMTBone>& vecBones = *pAinmMesh->GetBones();
    for (UINT i = 0; i < (UINT)vecBones.size(); ++i)
    {
        if (m_vecBones[i].strBoneName != vecBones[i].strBoneName)
        {
            MessageBox(nullptr, L"������ Bone�� �ƴմϴ�.", L"�ִϸ��̼� �ε� ����", MB_ICONHAND);
            return false;
        }
    }

    // �ִϸ��̼� Ŭ�� �̾���̱�
    vector<tMTAnimClip>& vecAnimClip = const_cast<vector<tMTAnimClip>&>(*pAinmMesh->GetAnimClip());
    std::map<wstring, tMTAnimClip*> mapAnimClip;
    vector<tMTAnimClip*> vecOriginAnimClip;

    // ���� �̸��� �ִϸ��̼� ����
    for (UINT i = 0; i < (UINT)m_vecAnimClip.size(); ++i)
    {
        mapAnimClip.insert(make_pair(m_vecAnimClip[i].strAnimName, &m_vecAnimClip[i]));
    }

    for (UINT i = 0; i < (UINT)vecAnimClip.size(); ++i)
    {
        auto iter = mapAnimClip.insert(make_pair(vecAnimClip[i].strAnimName, &vecAnimClip[i]));

        // �ߺ��� �̸��� �������� �ʾҴ�
        if (iter.second)
        {
            vecOriginAnimClip.push_back(&vecAnimClip[i]);
        }
    }

    // �߰��� �ִϸ��̼��� �������� �ʾҴ�.
    if (vecOriginAnimClip.empty())
    {
        return false;
    }

    UINT iFrameCount = 0;
    UINT OffsetFrame = 1 + m_vecAnimClip.back().iEndFrame;
    for (UINT i = 0; i < (UINT)vecOriginAnimClip.size(); ++i)
    {
        tMTAnimClip& AnimClip = *vecOriginAnimClip[i];

        // Ű������ �̾���̱�
        for (UINT j = AnimClip.iStartFrame; j <= (UINT)AnimClip.iEndFrame; ++j)
        {
            for (UINT k = 0; k < (UINT)vecBones.size(); ++k)
            {
                UINT FrameOffset = (UINT)m_vecBones[k].vecKeyFrame.size();

                // ������ �����Ͱ� ���� Bone�� ���
                if (0 == FrameOffset)
                    continue;

                tMTKeyFrame KeyFrame = vecBones[k].vecKeyFrame[j];
                KeyFrame.iFrame = (UINT)m_vecBones[k].vecKeyFrame.size();
                m_vecBones[k].vecKeyFrame.push_back(KeyFrame);

                iFrameCount = max(iFrameCount, (UINT)m_vecBones[k].vecKeyFrame.size());
            }
        }

        // �ִϸ��̼� �̾���̱�
        AnimClip.iStartFrame = OffsetFrame;
        AnimClip.iEndFrame = OffsetFrame + AnimClip.iFrameLength - 1;

        double FrameRate = FbxTime::GetFrameRate(AnimClip.eMode);
        AnimClip.dStartTime = AnimClip.iStartFrame / FrameRate;
        AnimClip.dEndTime = AnimClip.iEndFrame / FrameRate;
        AnimClip.dTimeLength = AnimClip.dEndTime - AnimClip.dStartTime;

        m_vecAnimClip.push_back(AnimClip);

        OffsetFrame = 1 + AnimClip.iEndFrame;
    }

    // BoneFrame ��� �����
    vector<tFrameTrans> vecFrameTrans;
    vecFrameTrans.resize((UINT)m_vecBones.size() * iFrameCount);

    for (size_t i = 0; i < m_vecBones.size(); ++i)
    {
        for (size_t j = 0; j < m_vecBones[i].vecKeyFrame.size(); ++j)
        {
            vecFrameTrans[(UINT)m_vecBones.size() * j + i] =
                tFrameTrans{Vec4(m_vecBones[i].vecKeyFrame[j].vTranslate, 0.f), Vec4(m_vecBones[i].vecKeyFrame[j].vScale, 0.f),
                            m_vecBones[i].vecKeyFrame[j].qRot};
        }
    }

    if (nullptr != m_pBoneFrameData)
    {
        delete m_pBoneFrameData;
        m_pBoneFrameData = nullptr;
    }

    m_pBoneFrameData = new CStructuredBuffer;
    m_pBoneFrameData->Create(sizeof(tFrameTrans), (UINT)vecFrameTrans.size(), SB_TYPE::READ_ONLY, false, vecFrameTrans.data());

    // ���� �Žø� �����ϰ� �ִ� Animator �缳��
    CLevel* pCurLevel = CLevelMgr::GetInst()->GetCurrentLevel();
    for (UINT i = 0; i < LAYER_MAX; i++)
    {
        const vector<CGameObject*>& vecParentObj = pCurLevel->GetLayer(i)->GetParentObjects();

        for (const auto& ParentObj : vecParentObj)
        {
            list<CGameObject*> queue;
            queue.push_back(ParentObj);

            while (!queue.empty())
            {
                CGameObject* pObject = queue.front();
                queue.pop_front();

                const vector<CGameObject*>& vecChildObj = pObject->GetChildObject();

                for (size_t i = 0; i < vecChildObj.size(); ++i)
                {
                    queue.push_back(vecChildObj[i]);
                }

                if (nullptr != pObject->Animator() && this == pObject->Animator()->GetSkeletalMesh())
                {
                    pObject->Animator()->SetSkeletalMesh(this);
                }
            }
        }
    }

    return true;
}

CMesh* CMesh::CreateFromContainer(CFBXLoader& _loader)
{
    vector<Vtx> vecVtx = {};
    for (int ContainerIndex = 0; ContainerIndex < _loader.GetContainerCount(); ++ContainerIndex)
    {
        const tContainer* container = &_loader.GetContainer(ContainerIndex);

        vector<Vtx> vecVtxContainer = {};
        UINT iVtxCount = (UINT)container->vecPos.size();
        vecVtxContainer.resize(iVtxCount);

        for (UINT i = 0; i < iVtxCount; ++i)
        {
            vecVtxContainer[i].vPos = container->vecPos[i];
            vecVtxContainer[i].vNormal = container->vecNormal[i];
            vecVtxContainer[i].vTangent = container->vecTangent[i];
            vecVtxContainer[i].vBiTangent = container->vecBinormal[i];
            vecVtxContainer[i].vColor = container->vecColor[i];
            vecVtxContainer[i].vUV0 = container->vecUV0[i];
            vecVtxContainer[i].vUV1 = container->vecUV1[i];
            vecVtxContainer[i].vUV2 = container->vecUV2[i];
            vecVtxContainer[i].vUV3 = container->vecUV3[i];
            vecVtxContainer[i].vIndices = container->vecIndices[i];
            vecVtxContainer[i].vWeights = container->vecWeights[i];
        }

        // Container�� ��� ������ �����Ͽ� �ϳ��� Mesh����
        vecVtx.insert(vecVtx.end(), vecVtxContainer.begin(), vecVtxContainer.end());
    }

    D3D11_BUFFER_DESC tVtxDesc = {};
    tVtxDesc.ByteWidth = UINT(sizeof(Vtx) * vecVtx.size());
    tVtxDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    tVtxDesc.Usage = D3D11_USAGE_DEFAULT;
    if (D3D11_USAGE_DYNAMIC == tVtxDesc.Usage)
        tVtxDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

    void* pVecVtxSysMem = malloc(tVtxDesc.ByteWidth);
    memcpy(pVecVtxSysMem, vecVtx.data(), tVtxDesc.ByteWidth);

    D3D11_SUBRESOURCE_DATA tSub = {};
    tSub.pSysMem = pVecVtxSysMem;

    ComPtr<ID3D11Buffer> pVB = NULL;
    if (FAILED(DEVICE->CreateBuffer(&tVtxDesc, &tSub, pVB.GetAddressOf())))
    {
        return NULL;
    }

    CMesh* pMesh = new CMesh;
    pMesh->m_VB = pVB;
    pMesh->m_VBDesc = tVtxDesc;
    pMesh->m_VtxSysMem = pVecVtxSysMem;
    pMesh->m_VtxCount = (UINT)vecVtx.size();

    // �ε��� ����
    UINT idxOffset = 0;
    for (int ContainerIndex = 0; ContainerIndex < _loader.GetContainerCount(); ++ContainerIndex)
    {
        const tContainer* container = &_loader.GetContainer(ContainerIndex);

        UINT iIdxBufferCount = (UINT)container->vecIdx.size();
        D3D11_BUFFER_DESC tIdxDesc = {};

        for (UINT i = 0; i < iIdxBufferCount; ++i)
        {
            vector<UINT> vecIdx(container->vecIdx[i].size());
            for (UINT j = 0; j < vecIdx.size(); ++j)
            {
                vecIdx[j] = idxOffset + container->vecIdx[i][j];
            }

            tIdxDesc.ByteWidth = (UINT)container->vecIdx[i].size() * sizeof(UINT); // Index Format �� R32_UINT �̱� ����
            tIdxDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
            tIdxDesc.Usage = D3D11_USAGE_DEFAULT;
            if (D3D11_USAGE_DYNAMIC == tIdxDesc.Usage)
                tIdxDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

            void* pVecIdxSysMem = malloc(tIdxDesc.ByteWidth);
            memcpy(pVecIdxSysMem, vecIdx.data(), tIdxDesc.ByteWidth);
            tSub.pSysMem = pVecIdxSysMem;

            ComPtr<ID3D11Buffer> pIB = nullptr;
            if (FAILED(DEVICE->CreateBuffer(&tIdxDesc, &tSub, pIB.GetAddressOf())))
            {
                return NULL;
            }

            tIndexInfo info = {};
            info.tIBDesc = tIdxDesc;
            info.iIdxCount = (UINT)container->vecIdx[i].size();
            info.pIdxSysMem = pVecIdxSysMem;
            info.pIB = pIB;
            info.IBName = container->strName;

            pMesh->m_vecIdxInfo.push_back(info);
            idxOffset += (UINT)container->vecPos.size(); // ���� �����̳��� ��������ŭ offset �߰�
        }
    }

    // Animator
    bool bHasAnim = false;
    for (int ContainerIndex = 0; ContainerIndex < _loader.GetContainerCount(); ++ContainerIndex)
    {
        const tContainer* container = &_loader.GetContainer(ContainerIndex);
        bHasAnim = container->bAnimation;
        if (bHasAnim)
            break;
    }

    // �ִϸ��̼��� ���������� �ʾҴ�.
    if (!bHasAnim)
        return pMesh;

    vector<tBone*>& vecBone = _loader.GetBones();
    UINT iFrameCount = 0;
    for (UINT i = 0; i < vecBone.size(); ++i)
    {
        tMTBone bone = {};
        bone.strBoneName = vecBone[i]->strBoneName;
        bone.iDepth = vecBone[i]->iDepth;
        bone.iIdx = i;
        bone.iParentIdx = vecBone[i]->iParentIndx;
        bone.matOffset = GetMatrixFromFbxMatrix(vecBone[i]->matOffset);

        for (UINT j = 0; j < vecBone[i]->vecKeyFrame.size(); ++j)
        {
            tMTKeyFrame tKeyframe = {};
            tKeyframe.dTime = vecBone[i]->vecKeyFrame[j].dTime;
            tKeyframe.iFrame = j;
            tKeyframe.vTranslate.x = (float)vecBone[i]->vecKeyFrame[j].matTransform.GetT().mData[0];
            tKeyframe.vTranslate.y = (float)vecBone[i]->vecKeyFrame[j].matTransform.GetT().mData[1];
            tKeyframe.vTranslate.z = (float)vecBone[i]->vecKeyFrame[j].matTransform.GetT().mData[2];

            tKeyframe.vScale.x = (float)vecBone[i]->vecKeyFrame[j].matTransform.GetS().mData[0];
            tKeyframe.vScale.y = (float)vecBone[i]->vecKeyFrame[j].matTransform.GetS().mData[1];
            tKeyframe.vScale.z = (float)vecBone[i]->vecKeyFrame[j].matTransform.GetS().mData[2];

            tKeyframe.qRot.x = (float)vecBone[i]->vecKeyFrame[j].matTransform.GetQ().mData[0];
            tKeyframe.qRot.y = (float)vecBone[i]->vecKeyFrame[j].matTransform.GetQ().mData[1];
            tKeyframe.qRot.z = (float)vecBone[i]->vecKeyFrame[j].matTransform.GetQ().mData[2];
            tKeyframe.qRot.w = (float)vecBone[i]->vecKeyFrame[j].matTransform.GetQ().mData[3];

            bone.vecKeyFrame.push_back(tKeyframe);
        }

        iFrameCount = max(iFrameCount, (UINT)bone.vecKeyFrame.size());

        pMesh->m_vecBones.push_back(bone);
    }

    UINT OffsetFrame = 0;
    vector<tAnimClip*>& vecAnimClip = _loader.GetAnimClip();
    for (UINT i = 0; i < vecAnimClip.size(); ++i)
    {
        tMTAnimClip tClip = {};

        tClip.strAnimName = vecAnimClip[i]->strName;

        tClip.iStartFrame = OffsetFrame + (int)vecAnimClip[i]->tStartTime.GetFrameCount(vecAnimClip[i]->eMode);
        tClip.iEndFrame = OffsetFrame + (int)vecAnimClip[i]->tEndTime.GetFrameCount(vecAnimClip[i]->eMode);
        tClip.iFrameLength = 1 + tClip.iEndFrame - tClip.iStartFrame;
        tClip.eMode = vecAnimClip[i]->eMode;

        double FrameRate = FbxTime::GetFrameRate(tClip.eMode);
        tClip.dStartTime = tClip.iStartFrame / FrameRate;
        tClip.dEndTime = tClip.iEndFrame / FrameRate;
        tClip.dTimeLength = tClip.dEndTime - tClip.dStartTime;

        pMesh->m_vecAnimClip.push_back(tClip);

        // ���� Clip�� End ���� �����ϵ��� Offset ����
        OffsetFrame += tClip.iFrameLength;
    }

    // Animation �� �ִ� Mesh ��� structuredbuffer �����α�
    if (pMesh->IsAnimMesh())
    {
        // BoneOffet ���
        vector<Matrix> vecOffset;
        vector<tFrameTrans> vecFrameTrans;
        vecFrameTrans.resize((UINT)pMesh->m_vecBones.size() * iFrameCount);

        for (size_t i = 0; i < pMesh->m_vecBones.size(); ++i)
        {
            vecOffset.push_back(pMesh->m_vecBones[i].matOffset);

            for (size_t j = 0; j < pMesh->m_vecBones[i].vecKeyFrame.size(); ++j)
            {
                vecFrameTrans[(UINT)pMesh->m_vecBones.size() * j + i] =
                    tFrameTrans{Vec4(pMesh->m_vecBones[i].vecKeyFrame[j].vTranslate, 0.f), Vec4(pMesh->m_vecBones[i].vecKeyFrame[j].vScale, 0.f),
                                pMesh->m_vecBones[i].vecKeyFrame[j].qRot};
            }
        }

        pMesh->m_pBoneOffset = new CStructuredBuffer;
        pMesh->m_pBoneOffset->Create(sizeof(Matrix), (UINT)vecOffset.size(), SB_TYPE::READ_ONLY, false, vecOffset.data());

        pMesh->m_pBoneFrameData = new CStructuredBuffer;
        pMesh->m_pBoneFrameData->Create(sizeof(tFrameTrans), (UINT)vecOffset.size() * iFrameCount, SB_TYPE::READ_ONLY, false, vecFrameTrans.data());
    }

    return pMesh;
}

int CMesh::Create(void* _Vtx, UINT _VtxCount, void* _Idx, UINT _IdxCount)
{
    m_VtxCount = _VtxCount;

    // ���ؽ� ���� ����
    m_VBDesc = {};

    m_VBDesc.ByteWidth = sizeof(Vtx) * _VtxCount;
    m_VBDesc.StructureByteStride = sizeof(Vtx);
    m_VBDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

    // ���ۿ� ������ ���� ����
    m_VBDesc.CPUAccessFlags = 0;
    m_VBDesc.Usage = D3D11_USAGE_DEFAULT;

    // g_Vtx �迭�� �����͸� �ʱ� �����ͷ� ����
    D3D11_SUBRESOURCE_DATA tSubData = {};
    tSubData.pSysMem = _Vtx;

    // ���ؽ� ���� ����
    if (FAILED(DEVICE->CreateBuffer(&m_VBDesc, &tSubData, m_VB.GetAddressOf())))
    {
        MessageBox(nullptr, L"���ؽ� ���� ���� ����", L"����", MB_OK);
        return E_FAIL;
    }

    // �ε��� ���� ����
    tIndexInfo IndexInfo = {};
    IndexInfo.iIdxCount = _IdxCount;

    IndexInfo.tIBDesc.ByteWidth = sizeof(UINT) * _IdxCount;

    // ���� ���� ���Ŀ���, ������ ������ ���� �� �� �ִ� �ɼ�
    IndexInfo.tIBDesc.CPUAccessFlags = 0;
    IndexInfo.tIBDesc.Usage = D3D11_USAGE::D3D11_USAGE_DEFAULT;

    // ������ �����ϴ� ������ ���� ���� �˸�
    IndexInfo.tIBDesc.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_INDEX_BUFFER;
    IndexInfo.tIBDesc.MiscFlags = 0;
    IndexInfo.tIBDesc.StructureByteStride = 0;

    // �ʱ� �����͸� �Ѱ��ֱ� ���� ���� ����ü
    tSubData.pSysMem = _Idx;

    if (FAILED(DEVICE->CreateBuffer(&IndexInfo.tIBDesc, &tSubData, IndexInfo.pIB.GetAddressOf())))
    {
        assert(nullptr);
    }

    // ���� �������� �� �ε��� ������ �����Ҵ��� �����ٰ� ������ѵΰ� ����
    m_VtxSysMem = new Vtx[m_VtxCount];
    IndexInfo.pIdxSysMem = new UINT[IndexInfo.iIdxCount];

    memcpy(m_VtxSysMem, _Vtx, sizeof(Vtx) * m_VtxCount);
    memcpy(IndexInfo.pIdxSysMem, _Idx, sizeof(UINT) * IndexInfo.iIdxCount);

    m_vecIdxInfo.push_back(IndexInfo);

    return S_OK;
}

void CMesh::render(UINT _iSubset)
{
    UpdateData(_iSubset);

    CONTEXT->DrawIndexed(m_vecIdxInfo[_iSubset].iIdxCount, 0, 0);
}

void CMesh::render_draw(UINT _iSubset)
{
    UpdateData(_iSubset);

    CONTEXT->Draw(m_vecIdxInfo[_iSubset].iIdxCount, 0);
}

void CMesh::render_IndexedInstanced(UINT _InstanceCount)
{
    UpdateData(0);

    CONTEXT->DrawIndexedInstanced(m_vecIdxInfo[0].iIdxCount, _InstanceCount, 0, 0, 0);
}

void CMesh::render_instancing(UINT _iSubset)
{
    UpdateData_Inst(_iSubset);

    CONTEXT->DrawIndexedInstanced(m_vecIdxInfo[_iSubset].iIdxCount, CInstancingBuffer::GetInst()->GetInstanceCount(), 0, 0, 0);
}

int CMesh::Save(const wstring& _strRelativePath)
{
    if (IsEngineAsset())
        return E_FAIL;

    // ���� ��� �����
    wstring strFilePath = CPathMgr::GetContentPath() + _strRelativePath;

    // ���� ������� ����
    FILE* pFile = nullptr;
    _wfopen_s(&pFile, strFilePath.c_str(), L"wb");

    if (nullptr == pFile)
        return E_FAIL;

    // ����� ����
    SetRelativePath(_strRelativePath);

    // Ű��, ��� ���
    SaveWStringToFile(GetName(), pFile);
    SaveWStringToFile(GetKey(), pFile);
    SaveWStringToFile(GetRelativePath(), pFile);

    // ���� ������ ����
    UINT iByteSize = m_VBDesc.ByteWidth;
    fwrite(&iByteSize, sizeof(UINT), 1, pFile);
    fwrite(m_VtxSysMem, iByteSize, 1, pFile);

    // �ε��� ����
    UINT iMtrlCount = (UINT)m_vecIdxInfo.size();
    fwrite(&iMtrlCount, sizeof(UINT), 1, pFile);

    for (UINT i = 0; i < iMtrlCount; ++i)
    {
        fwrite(&m_vecIdxInfo[i].tIBDesc, sizeof(D3D11_BUFFER_DESC), 1, pFile);
        fwrite(&m_vecIdxInfo[i].iIdxCount, sizeof(UINT), 1, pFile);
        fwrite(m_vecIdxInfo[i].pIdxSysMem, m_vecIdxInfo[i].iIdxCount * sizeof(UINT), 1, pFile);
        SaveWStringToFile(m_vecIdxInfo[i].IBName, pFile);
    }

    // Animator ����
    UINT iClipCount = (UINT)m_vecAnimClip.size();
    fwrite(&iClipCount, sizeof(UINT), 1, pFile);
    for (UINT i = 0; i < iClipCount; ++i)
    {
        SaveWStringToFile(m_vecAnimClip[i].strAnimName, pFile);
        fwrite(&m_vecAnimClip[i].dStartTime, sizeof(double), 1, pFile);
        fwrite(&m_vecAnimClip[i].dEndTime, sizeof(double), 1, pFile);
        fwrite(&m_vecAnimClip[i].dTimeLength, sizeof(double), 1, pFile);
        fwrite(&m_vecAnimClip[i].eMode, sizeof(FbxTime::EMode), 1, pFile);
        fwrite(&m_vecAnimClip[i].iStartFrame, sizeof(int), 1, pFile);
        fwrite(&m_vecAnimClip[i].iEndFrame, sizeof(int), 1, pFile);
        fwrite(&m_vecAnimClip[i].iFrameLength, sizeof(int), 1, pFile);
    }

    UINT iBoneCount = (UINT)m_vecBones.size();
    fwrite(&iBoneCount, sizeof(UINT), 1, pFile);

    for (UINT i = 0; i < iBoneCount; ++i)
    {
        SaveWStringToFile(m_vecBones[i].strBoneName, pFile);
        fwrite(&m_vecBones[i].iDepth, sizeof(int), 1, pFile);
        fwrite(&m_vecBones[i].iIdx, sizeof(int), 1, pFile);
        fwrite(&m_vecBones[i].iParentIdx, sizeof(int), 1, pFile);
        fwrite(&m_vecBones[i].matOffset, sizeof(Matrix), 1, pFile);

        // KeyFrame
        UINT iFrameCount = (UINT)m_vecBones[i].vecKeyFrame.size();
        fwrite(&iFrameCount, sizeof(UINT), 1, pFile);
        for (UINT j = 0; j < iFrameCount; ++j)
        {
            fwrite(&m_vecBones[i].vecKeyFrame[j], sizeof(tMTKeyFrame), 1, pFile);
        }

        // BoneSocket
        UINT iBoneSocketCount = (UINT)m_vecBones[i].vecBoneSocket.size();
        fwrite(&iBoneSocketCount, sizeof(UINT), 1, pFile);
        for (UINT j = 0; j < iBoneSocketCount; ++j)
        {
            SaveWStringToFile(m_vecBones[i].vecBoneSocket[j]->SoketName, pFile);
            fwrite(&m_vecBones[i].vecBoneSocket[j]->BoneIndex, sizeof(int), 1, pFile);
            fwrite(&m_vecBones[i].vecBoneSocket[j]->RelativePosition, sizeof(Vec3), 1, pFile);
            fwrite(&m_vecBones[i].vecBoneSocket[j]->RelativeRotation, sizeof(Vec3), 1, pFile);
            fwrite(&m_vecBones[i].vecBoneSocket[j]->RelativeScale, sizeof(Vec3), 1, pFile);
            fwrite(&m_vecBones[i].vecBoneSocket[j]->matSocket, sizeof(Matrix), 1, pFile);
        }
    }

    fclose(pFile);

    return S_OK;
}

int CMesh::Load(const wstring& _strFilePath)
{
    // �б���� ���Ͽ���
    FILE* pFile = nullptr;

    _wfopen_s(&pFile, _strFilePath.c_str(), L"rb");

    if (nullptr == pFile)
        return E_FAIL;

    // Ű��, �����
    wstring strName, strKey, strRelativePath;
    LoadWStringFromFile(strName, pFile);
    LoadWStringFromFile(strKey, pFile);
    LoadWStringFromFile(strRelativePath, pFile);

    SetName(strName);
    SetKey(strKey);
    SetRelativePath(strRelativePath);

    // ����������
    UINT iByteSize = 0;
    fread(&iByteSize, sizeof(UINT), 1, pFile);

    m_VtxSysMem = (Vtx*)malloc(iByteSize);
    fread(m_VtxSysMem, 1, iByteSize, pFile);

    m_VBDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    m_VBDesc.ByteWidth = iByteSize;
    m_VBDesc.Usage = D3D11_USAGE_DEFAULT;

    D3D11_SUBRESOURCE_DATA tSubData = {};
    tSubData.pSysMem = m_VtxSysMem;

    if (FAILED(DEVICE->CreateBuffer(&m_VBDesc, &tSubData, m_VB.GetAddressOf())))
    {
        assert(nullptr);
    }

    // �ε��� ����
    UINT iMtrlCount = 0;
    fread(&iMtrlCount, sizeof(UINT), 1, pFile);

    for (UINT i = 0; i < iMtrlCount; ++i)
    {
        tIndexInfo info = {};
        fread(&info.tIBDesc, sizeof(D3D11_BUFFER_DESC), 1, pFile);
        fread(&info.iIdxCount, sizeof(UINT), 1, pFile);

        UINT iByteWidth = info.iIdxCount * sizeof(UINT);

        void* pSysMem = malloc(iByteWidth);
        info.pIdxSysMem = pSysMem;
        fread(info.pIdxSysMem, iByteWidth, 1, pFile);

        tSubData.pSysMem = info.pIdxSysMem;

        if (FAILED(DEVICE->CreateBuffer(&info.tIBDesc, &tSubData, info.pIB.GetAddressOf())))
        {
            assert(nullptr);
        }

        LoadWStringFromFile(info.IBName, pFile);

        m_vecIdxInfo.push_back(info);
    }

    // Animator ���� �б�
    UINT iClipCount = 0;
    fread(&iClipCount, sizeof(UINT), 1, pFile);
    for (UINT i = 0; i < iClipCount; ++i)
    {
        tMTAnimClip tClip = {};

        LoadWStringFromFile(tClip.strAnimName, pFile);
        fread(&tClip.dStartTime, sizeof(double), 1, pFile);
        fread(&tClip.dEndTime, sizeof(double), 1, pFile);
        fread(&tClip.dTimeLength, sizeof(double), 1, pFile);
        fread(&tClip.eMode, sizeof(FbxTime::EMode), 1, pFile);
        fread(&tClip.iStartFrame, sizeof(int), 1, pFile);
        fread(&tClip.iEndFrame, sizeof(int), 1, pFile);
        fread(&tClip.iFrameLength, sizeof(int), 1, pFile);

        m_vecAnimClip.push_back(tClip);
    }

    UINT iBoneCount = 0;
    fread(&iBoneCount, sizeof(UINT), 1, pFile);
    m_vecBones.resize(iBoneCount);

    UINT iMaxFrameCount = 0;
    for (UINT i = 0; i < iBoneCount; ++i)
    {
        LoadWStringFromFile(m_vecBones[i].strBoneName, pFile);
        fread(&m_vecBones[i].iDepth, sizeof(int), 1, pFile);
        fread(&m_vecBones[i].iIdx, sizeof(int), 1, pFile);
        fread(&m_vecBones[i].iParentIdx, sizeof(int), 1, pFile);
        fread(&m_vecBones[i].matOffset, sizeof(Matrix), 1, pFile);

        // KeyFrame
        UINT iFrameCount = 0;
        fread(&iFrameCount, sizeof(UINT), 1, pFile);
        m_vecBones[i].vecKeyFrame.resize(iFrameCount);
        iMaxFrameCount = max(iMaxFrameCount, iFrameCount);
        for (UINT j = 0; j < iFrameCount; ++j)
        {
            fread(&m_vecBones[i].vecKeyFrame[j], sizeof(tMTKeyFrame), 1, pFile);
        }

        // BoneSocket
        UINT iBoneSocketCount = 0;
        fread(&iBoneSocketCount, sizeof(UINT), 1, pFile);
        m_vecBones[i].vecBoneSocket.resize(iBoneSocketCount);
        for (UINT j = 0; j < iBoneSocketCount; ++j)
        {
            tBoneSocket* pBoneSocket = new tBoneSocket;
            LoadWStringFromFile(pBoneSocket->SoketName, pFile);
            fread(&pBoneSocket->BoneIndex, sizeof(int), 1, pFile);
            fread(&pBoneSocket->RelativePosition, sizeof(Vec3), 1, pFile);
            fread(&pBoneSocket->RelativeRotation, sizeof(Vec3), 1, pFile);
            fread(&pBoneSocket->RelativeScale, sizeof(Vec3), 1, pFile);
            fread(&pBoneSocket->matSocket, sizeof(Matrix), 1, pFile);
            m_vecBones[i].vecBoneSocket[j] = pBoneSocket;
            vecBoneSocket.push_back(pBoneSocket);
        }
    }

    // Animation �� �ִ� Mesh ��� Bone StructuredBuffer �����
    if (m_vecAnimClip.size() > 0 && m_vecBones.size() > 0)
    {
        wstring strBone = GetName();

        // BoneOffet ���
        vector<Matrix> vecOffset;
        vector<tFrameTrans> vecFrameTrans;
        vecFrameTrans.resize((UINT)m_vecBones.size() * iMaxFrameCount);

        for (size_t i = 0; i < m_vecBones.size(); ++i)
        {
            vecOffset.push_back(m_vecBones[i].matOffset);

            for (size_t j = 0; j < m_vecBones[i].vecKeyFrame.size(); ++j)
            {
                vecFrameTrans[(UINT)m_vecBones.size() * j + i] =
                    tFrameTrans{Vec4(m_vecBones[i].vecKeyFrame[j].vTranslate, 0.f), Vec4(m_vecBones[i].vecKeyFrame[j].vScale, 0.f),
                                Vec4(m_vecBones[i].vecKeyFrame[j].qRot)};
            }
        }

        m_pBoneOffset = new CStructuredBuffer;
        m_pBoneOffset->Create(sizeof(Matrix), (UINT)vecOffset.size(), SB_TYPE::READ_ONLY, false, vecOffset.data());

        m_pBoneFrameData = new CStructuredBuffer;
        m_pBoneFrameData->Create(sizeof(tFrameTrans), (UINT)vecOffset.size() * (UINT)iMaxFrameCount, SB_TYPE::READ_ONLY, false, vecFrameTrans.data());
    }

    fclose(pFile);

    return S_OK;
}