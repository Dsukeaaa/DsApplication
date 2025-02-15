#ifndef _DS_SHADER_H_
#define _DS_SHADER_H_

namespace DsLib
{
	enum class SHADER_TYPE : int
	{
		DEFAULT,
		SHADOW_MAP,
		BLUR_HORIZON,
		BLUR_VERTICAL,
		BLOOM1,
		BLOOM2,
		SSAO,
		DEPTH_FIELD1,
		DEPTH_FIELD2,
		SSR1,
		SSR2,
		NUM,
	};

	enum class COMPUTE_SHADER_TYPE : int
	{
		RAY_TRACE,
		NUM,
	};

	class DsShader
	{
	public:
		struct BlurParam
		{
			int pixNum;
			float weight[100];
			float weightSum;
		};
	public:
		static DsShader& Create();
		static BlurParam GetBlurParam(const int pixNum, float level=1.0f);

	public:
		virtual void Initialize() = NULL;
		virtual void Initialize(std::string& vertex, std::string& flagment) = NULL;
		virtual void EnableShader( SHADER_TYPE sType ) = NULL;
		virtual void DisableShader() = NULL;
		virtual void SetModelTransform(const float m[16]) = NULL;
		virtual void SetProjectionTransform(const float m[16]) = NULL;
		virtual void SetModelViewProjectionTransform(const float m[16]) = NULL;
		virtual void SetProjectionInverseTransform(const float m[16]) = NULL;
		virtual void SetModelViewProjectionInverseTransform(const float m[16]) = NULL;
		virtual void SetCameraPosition(DsVec3f pos) = NULL;
		virtual void SetTextureUnit(int unit) = NULL;
		virtual void SetUseTexture(bool isUse) = NULL;
		virtual void SetUseLight(bool isUse)=NULL;
		virtual void SetUseShadow(bool isUse)=NULL;
		virtual void SetBlurParam(float s, int ts, const BlurParam& bp )=NULL;
		virtual void SetPostEffectParam(int effTex, int oriTex, int oriDepTex)=NULL;
		virtual void SetUseNormalMap(bool isUse) = NULL;
		virtual void SetUseWaveNormalMap(bool isUse) = NULL;
		virtual void SetUseSpecularMap(bool isUse) =NULL;
		virtual void SetTime(float t) = NULL;
		virtual void DepthFieldParam(int depTex, int blurTex) = NULL;
		virtual void NormalTex(int normalTex) = NULL;
		virtual void SpecularDepthTex(int tex) = NULL;
	};

}

#endif