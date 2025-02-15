// ShaderTest.cpp : アプリケーションのエントリ ポイントを定義します。
//
#include "stdafx.h"
#include "Graphics/GL/DsGLFunc.h"
#include "Graphics/Shader/DsShader.h"


using namespace DsLib;


#define DS_SHADER_STR(str) #str

/***************************************************
@brief		バーテックスシェーダー
***************************************************/
static const char s_vertex[] = DS_SHADER_STR(
	void main(void)
{
	gl_Position = ftransform();
	gl_TexCoord[0] = gl_MultiTexCoord0;
}
);
/***************************************************
@brief		フラグメントシェーダー
***************************************************/
static const char s_fragment[] = DS_SHADER_STR(
	uniform sampler2D colTexEff;

void main(void)
{
	vec3 texel = max(vec3(0.0), (texture2D(colTexEff, gl_TexCoord[0].st) - 0.8).rgb);
	gl_FragColor = vec4(texel, 1.0);
}
);

int main()
{
	DsInitGLFunc();
	DsShader& shader = DsShader::Create();
	std::string a = s_vertex;
	std::string b = s_fragment;
	shader.Initialize(a, b);

    return 0;
}

