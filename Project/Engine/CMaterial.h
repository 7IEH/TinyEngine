#pragma once
#include "CAsset.h"

#include "CTexture.h"
#include "CGraphicsShader.h"

class CMaterial : public CAsset
{
private:
    tMtrlConst m_Const;
    Ptr<CTexture> m_arrTex[(UINT)TEX_PARAM::TEX_END];
    Ptr<CGraphicsShader> m_pShader;

public:
    void SetMaterialCoefficient(Vec4 _vAlbe = Vec4(), Vec4 _vDiff = Vec4(), Vec4 _vSpec = Vec4(), float _vMetal = 0.f, float _vRough = 0.f,
                                Vec4 _vEmis = Vec4())
    {
        m_Const.mtrl.vAlbedo = _vAlbe;
        m_Const.mtrl.vDiffuse = _vDiff;
        m_Const.mtrl.vSpecular = _vSpec;
        m_Const.mtrl.vMetallic = _vMetal;
        m_Const.mtrl.vRoughness = _vRough;
        m_Const.mtrl.vEmission = _vEmis;
    }

    void SetAlbedo(Vec4 _Albedo) { m_Const.mtrl.vAlbedo = _Albedo; }
    Vec4 GetAlbedo() const { return m_Const.mtrl.vAlbedo; }

    void SetDiffuse(Vec4 _Diffuse) { m_Const.mtrl.vDiffuse = _Diffuse; }
    Vec4 GetDiffuse() const { return m_Const.mtrl.vDiffuse; }

    void SetSpecular(Vec4 _Specular) { m_Const.mtrl.vSpecular = _Specular; }
    Vec4 GetSpecular() const { return m_Const.mtrl.vSpecular; }

    void SetMetallic(float _Metallic) { m_Const.mtrl.vMetallic = _Metallic; }
    float GetMetallic() const { return m_Const.mtrl.vMetallic; }

    void SetRoughness(float _Roughness) { m_Const.mtrl.vRoughness = _Roughness; }
    float GetRoughness() const { return m_Const.mtrl.vRoughness; }

    void SetEmission(Vec4 _Emission) { m_Const.mtrl.vEmission = _Emission; }
    Vec4 GetEmission() const { return m_Const.mtrl.vEmission; }

    void SetAnim3D(bool _bUseAnim) { m_Const.arrAnimData[0] = (int)_bUseAnim; }
    void SetBoneCount(int _iBoneCount) { m_Const.arrAnimData[1] = _iBoneCount; }

    void SetMotionBlur(bool _bUse) { m_Const.bMotionBlur = (int)_bUse; }

    template <typename T>
    void SetScalarParam(SCALAR_PARAM _ParamType, const T& _Value);
    void* GetScalarParam(SCALAR_PARAM _ParamType);

    void SetTexParam(TEX_PARAM _Param, Ptr<CTexture> _Tex) { m_arrTex[(UINT)_Param] = _Tex; }
    Ptr<CTexture> GetTexParam(TEX_PARAM _param) const { return m_arrTex[(UINT)_param]; }

    void SetShader(Ptr<CGraphicsShader> _Shader) { m_pShader = _Shader; }
    Ptr<CGraphicsShader> GetShader() const { return m_pShader; }

public:
    void UpdateData();
    void UpdateData_Inst();

public:
    const tMtrlConst& GetMtrlConst() const { return m_Const; }

public:
    virtual int Save(const wstring& _strRelativePath) override;
    virtual int Load(const wstring& _strFilePath) override;

    CLONE(CMaterial);

public:
    CMaterial(bool _bEngineAsset = false);
    virtual ~CMaterial();

    friend class CMaterialEditor;
};

template <typename T>
void CMaterial::SetScalarParam(SCALAR_PARAM _ParamType, const T& _Value)
{
    const T* pValue = &_Value;

    switch (_ParamType)
    {
    case INT_0:
    case INT_1:
    case INT_2:
    case INT_3:
        m_Const.arrInt[_ParamType] = *((int*)pValue);
        break;

    case FLOAT_0:
    case FLOAT_1:
    case FLOAT_2:
    case FLOAT_3:
        m_Const.arrFloat[_ParamType - FLOAT_0] = *((float*)pValue);
        break;

    case VEC2_0:
    case VEC2_1:
    case VEC2_2:
    case VEC2_3:
        m_Const.arrVec2[_ParamType - VEC2_0] = *((Vec2*)pValue);

        break;
    case VEC4_0:
    case VEC4_1:
    case VEC4_2:
    case VEC4_3:
        m_Const.arrVec4[_ParamType - VEC4_0] = *((Vec4*)pValue);

        break;
    case MAT_0:
    case MAT_1:
    case MAT_2:
    case MAT_3:
        m_Const.arrMat[_ParamType - MAT_0] = *((Matrix*)pValue);
        break;
    }
}