#include "DsPch.h"
#ifndef _DS_GL_FUNC_
#include "Graphics/GL/DsGLFunc.h"
#endif
#include <gl/GL.h>
#include <gl/GLU.h>
#include "Graphics/Render/DsShadowMap.h"
#include "Graphics/Render/DsRender.h"
#include "Graphics/Shader/DsShader.h"
#include "Graphics/Camera/DsCamera.h"
#include "Graphics/Light/DsLight.h"
#include "Graphics/Light/DsLightMan.h"
#include "Math/DsInverseMatrix.h"



using namespace DsLib;

namespace
{
	static const int s_shadowSize = 1024;
	static const double BLUR_COEF = 1.0;

	class DsShadowMapImp : public DsShadowMap
	{
	public:
		DsShadowMapImp( const DsRender& ren, DsShader& shader);
		virtual ~DsShadowMapImp();

	public:
		virtual void DrawShadow() override;
		virtual void DrawDepthTex() override;

	private:
		void _SetLightView(const DsLight& light, const DsCamera& cam);
		void _Blur();

	private:
		const DsRender& m_render;
		DsShader& m_shader;
		GLuint m_iFBODepth;		//!< 光源から見たときのデプスを格納するFramebuffer object
		GLuint m_iTexDepth;		//!< m_iFBODepthにattachするテクスチャ
		GLsizei m_fDepthSize[2];	//!< デプスを格納するテクスチャのサイズ
		GLuint m_colorTextureId;
		GLuint m_depthrRboId;
		GLuint m_blurFboId;
		GLuint m_blurFboIdColorTextureId;
	};

	DsShadowMapImp::DsShadowMapImp( const DsRender& ren, DsShader& shader)
	:m_render(ren)
	,m_shader(shader)
	,m_iFBODepth(0)
	,m_iTexDepth(0)
	,m_colorTextureId(0)
	,m_blurFboId(0)
	,m_blurFboIdColorTextureId(0)
	{
		m_fDepthSize[0] = static_cast<GLsizei>(s_shadowSize);
		m_fDepthSize[1] = static_cast<GLsizei>(s_shadowSize);

		// デプス値テクスチャ
		glGenTextures(1, &m_iTexDepth);
		glBindTexture(GL_TEXTURE_2D, m_iTexDepth);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, DS_GL_CLAMP_TO_BORDER);//シャドウテクスチャの範囲を超えた場合、日向であってほしいので、ボーダーカラーになるように
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, DS_GL_CLAMP_TO_BORDER);
		GLfloat border_color[4] = {1, 1, 1, 1};
		glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, border_color);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, m_fDepthSize[0], m_fDepthSize[1], 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, 0);
		glBindTexture(GL_TEXTURE_2D, 0);


		//デプスな用途なんだけど複数成分使うのでカラー
		glGenTextures(1, &m_colorTextureId);
		glBindTexture(GL_TEXTURE_2D, m_colorTextureId);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, DS_GL_CLAMP_TO_BORDER);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, DS_GL_CLAMP_TO_BORDER);
		glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, border_color);
		glTexImage2D(GL_TEXTURE_2D, 0, DS_GL_RGB32F, m_fDepthSize[0], m_fDepthSize[1], 0, GL_RGB, GL_FLOAT, 0);
		DsGLGenerateMipmap(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, 0);


		// FBO作成h
		DsGLGenFramebuffers(1, &m_iFBODepth);
		DsGLBindFramebuffer(DS_GL_FRAMEBUFFER, m_iFBODepth);
		//glDrawBuffer(GL_NONE);
		//glReadBuffer(GL_NONE);
		// デプスマップテクスチャをFBOに接続
		DsGLFramebufferTexture2D(DS_GL_FRAMEBUFFER, DS_GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_iTexDepth, 0);
		DsGLFramebufferTexture2D(DS_GL_FRAMEBUFFER, DS_GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_colorTextureId, 0);
		GLenum FBOstatus = DsGLCheckFramebufferStatus(DS_GL_FRAMEBUFFER);
		if (FBOstatus != DS_GL_FRAMEBUFFER_COMPLETE) {
			printf("DS_GL_FRAMEBUFFER_COMPLETE failed for shadowmap FBO, CANNOT use FBO\n");
		}
		DsGLBindFramebuffer(DS_GL_FRAMEBUFFER, 0);


		// ブラー用
		DsGLGenFramebuffers(1, &m_blurFboId);
		DsGLBindFramebuffer(DS_GL_FRAMEBUFFER, m_blurFboId);
		glGenTextures(1, &m_blurFboIdColorTextureId);
		glBindTexture(GL_TEXTURE_2D, m_blurFboIdColorTextureId);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
		glTexImage2D(GL_TEXTURE_2D, 0, DS_GL_RGB32F, static_cast<GLsizei>(m_fDepthSize[0]*BLUR_COEF), static_cast<GLsizei>(m_fDepthSize[1]*BLUR_COEF), 0, GL_RGB, GL_FLOAT, 0);

		DsGLFramebufferTexture2D(DS_GL_FRAMEBUFFER, DS_GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_blurFboIdColorTextureId, 0);
		FBOstatus = DsGLCheckFramebufferStatus(DS_GL_FRAMEBUFFER);
		if (FBOstatus != DS_GL_FRAMEBUFFER_COMPLETE) {
			printf("DS_GL_FRAMEBUFFER_COMPLETE failed for blur FBO, CANNOT use FBO\n");
		}
		glBindTexture(GL_TEXTURE_2D, 0);
		DsGLBindFramebuffer(DS_GL_FRAMEBUFFER, 0);
	}

	//virtual
	DsShadowMapImp::~DsShadowMapImp()
	{
	}

	//virtual
	void DsShadowMapImp::DrawShadow()
	{
		double light_proj[16], camera_proj[16];
		double light_modelview[16], camera_modelview[16];
		const DsLight& light = m_render.RefLight();
		const DsCamera& cam = m_render.RefCam();

		//
		// 現在の視点行列，光源行列を取得 or 作成
		//
		glMatrixMode(GL_PROJECTION);

		// 視点プロジェクション行列の取得
		glGetDoublev(GL_PROJECTION_MATRIX, camera_proj);
		glPushMatrix();	// 今のプロジェクション行列を退避させておく

		// 光源プロジェクション行列の生成と取得
		glLoadIdentity();
		gluPerspective(50, (double)m_fDepthSize[0]/(double)m_fDepthSize[1], cam.GetNear(), cam.GetFar() );
		//glOrthoに入れるleftとかbottomは影をつける範囲のサイズになる。座標はモデルビュー座標。シャドウマップのサイズではないので注意
		//glOrtho(-2, 2, -2, 2, 1, 1000);
		glGetDoublev(GL_PROJECTION_MATRIX, light_proj);
		
		glMatrixMode(GL_MODELVIEW);

		// 視点モデルビュー行列の取得
		glGetDoublev(GL_MODELVIEW_MATRIX, camera_modelview);
		glPushMatrix();	// 今のモデルビュー行列を退避させておく

		// 光源モデルビュー行列の生成と取得
		glLoadIdentity();
		_SetLightView(light, cam);
		
		glGetDoublev(GL_MODELVIEW_MATRIX, light_modelview);

		// 今のビューポート情報を後で戻すために確保
		GLint viewport[4];
		glGetIntegerv(GL_VIEWPORT, viewport);


		// 光源からレンダリングしてシャドウマップを生成
		DsGLBindFramebuffer(DS_GL_FRAMEBUFFER, m_iFBODepth);	// FBOにレンダリング

		// カラー，デプスバッファのクリア
		glClearColor(1, 1, 1, 1);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glClearDepth(1.0f);

		// ビューポートをシャドウマップの大きさに変更
		glViewport(0, 0, m_fDepthSize[0], m_fDepthSize[1]);
			
		glPolygonOffset(1.1f, 4.0f);
		glEnable(GL_POLYGON_OFFSET_FILL);

		glEnable(GL_TEXTURE_2D);
	
		glDisable(GL_LIGHTING);
		if(false){
			//両面
			glDisable(GL_CULL_FACE);
		}
		else{
			//裏面
			//glEnable(GL_CULL_FACE);
			//glCullFace(GL_FRONT);
		}
		
		//ここで影用オブジェト描画
		m_shader.EnableShader(SHADER_TYPE::SHADOW_MAP);
		m_render.RendPolygon();
		m_shader.DisableShader();

		glDisable(GL_POLYGON_OFFSET_FILL);
	
		const double bias[16] = {	0.5, 0.0, 0.0, 0.0, 
									0.0, 0.5, 0.0, 0.0,
									0.0, 0.0, 0.5, 0.0,
									0.5, 0.5, 0.5, 1.0 };
	

	
		// テクスチャモードに移行
		glMatrixMode(GL_TEXTURE);
		DsGLActiveTexture(DS_GL_TEXTURE7);
		
		glLoadIdentity();
		glLoadMatrixd(bias);

		// 光源中心座標となるようにテクスチャ行列を設定
		// テクスチャ変換行列にモデルビュー，プロジェクションを設定
		glMultMatrixd(light_proj);
		glMultMatrixd(light_modelview);

		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
		_Blur();
		DsGLBindFramebuffer(DS_GL_FRAMEBUFFER, 0);

		// 無効にした色のレンダリングを有効にする
		glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE); 

		// 元のビューポート行列に戻す
		glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);
		// glMatrixMode(GL_TEXTURE)で変更したマトリクスを戻す
		//glPopMatrix();

		// 退避させておいた視点行列を元に戻す
		glMatrixMode(GL_PROJECTION);
		glPopMatrix();
		glMatrixMode(GL_MODELVIEW);
		glPopMatrix();

		// デプステクスチャを貼り付け
		glEnable(GL_TEXTURE_2D);
		DsGLActiveTexture(DS_GL_TEXTURE7);
		glBindTexture(GL_TEXTURE_2D, m_iTexDepth);
		DsGLActiveTexture(DS_GL_TEXTURE6);
		glBindTexture(GL_TEXTURE_2D, m_colorTextureId);
		DsGLGenerateMipmap(GL_TEXTURE_2D);

		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);

		DsGLActiveTexture(DS_GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, 0);

		//空の色
		glClearColor(0.6f, 0.7f, 0.9f, 1);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	/*!
	* デプスマップをテクスチャとして表示
	* @param[in] w,h ウィンドウサイズ
	*/
	void DsShadowMapImp::DrawDepthTex()
	{
		const double w = m_render.GetWidth();
		const double h = m_render.GetHeight();
		m_shader.DisableShader();
		glMatrixMode(GL_PROJECTION);
		glPushMatrix();
		glLoadIdentity();
		glOrtho(0, w, 0, h, 0, 1);
		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		glLoadIdentity();

		glDisable(GL_LIGHTING);
		glColor4f(1, 1, 1, 1);
		DsGLActiveTexture(DS_GL_TEXTURE0);
		glEnable(GL_TEXTURE_2D);
		//glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
		//glBindTexture(GL_TEXTURE_2D, m_iTexDepth);
		glBindTexture(GL_TEXTURE_2D, m_colorTextureId);
		//glBindTexture(GL_TEXTURE_2D, m_blurFboIdColorTextureId);
		glNormal3d(0, 0, 1);
		glBegin(GL_QUADS);
		glTexCoord2d(0, 0); glVertex3d(0.05*w, 0.05*h, 0);
		glTexCoord2d(1, 0); glVertex3d(0.05*w + 100, 0.05*h, 0);
		glTexCoord2d(1, 1); glVertex3d(0.05*w + 100, 0.05*h + 100, 0);
		glTexCoord2d(0, 1); glVertex3d(0.05*w, 0.05*h + 100, 0);
		glEnd();
		glBindTexture(GL_TEXTURE_2D, 0);
		glDisable(GL_TEXTURE_2D);
		glMatrixMode(GL_PROJECTION);
		glPopMatrix();
		glMatrixMode(GL_MODELVIEW);
		glPopMatrix();
		m_shader.DisableShader();
	}

	void DsShadowMapImp::_Blur()
	{
		m_shader.EnableShader(SHADER_TYPE::BLUR_HORIZON);

		DsGLBindFramebuffer(DS_GL_FRAMEBUFFER, m_blurFboId);
		//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		//glClearDepth(1.0f);
		//m_blurFboIdへ横方向にぼかす。m_blurFboIdはm_blurFboIdColorTextureIdと紐づいているのでここに結果が格納される。横をぼかすための一時テクスチャ
		glViewport(0, 0, static_cast<GLsizei>(m_fDepthSize[0] * BLUR_COEF), static_cast<GLsizei>(m_fDepthSize[1] * BLUR_COEF));
		
		// Bluring horinzontaly
		const DsShader::BlurParam blurParam = DsShader::GetBlurParam(3);
		m_shader.SetBlurParam(1.0f / (static_cast<float>(m_fDepthSize[0])*static_cast<float>(BLUR_COEF)),	0, blurParam);
		DsGLActiveTexture(DS_GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, m_colorTextureId);//シャドウマップ(6と7あって6の方)をテクスチャとしてバインド

		//Preparing to draw quad
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glOrtho(-m_fDepthSize[0] / 2, m_fDepthSize[0] / 2, -m_fDepthSize[1] / 2, m_fDepthSize[1] / 2, 1, 20);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		//Drawing quad 
		glTranslated(0, 0, -5);
		glBegin(GL_QUADS);
		glTexCoord2d(0, 0); glVertex3f(static_cast<float>(-m_fDepthSize[0] / 2), static_cast<float>(-m_fDepthSize[1] / 2), 0);
		glTexCoord2d(1, 0); glVertex3f(static_cast<float>(m_fDepthSize[0] / 2), static_cast<float>(-m_fDepthSize[1] / 2), 0);
		glTexCoord2d(1, 1); glVertex3f(static_cast<float>(m_fDepthSize[0] / 2), static_cast<float>(m_fDepthSize[1] / 2), 0);
		glTexCoord2d(0, 1); glVertex3f(static_cast<float>(-m_fDepthSize[0] / 2), static_cast<float>(m_fDepthSize[1] / 2), 0);
		glEnd();
		//DsGLGenerateMipmap(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, 0);

		// Bluring vertically
		m_shader.EnableShader(SHADER_TYPE::BLUR_VERTICAL);
		DsGLBindFramebuffer(DS_GL_FRAMEBUFFER, m_iFBODepth);
		//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		//glClearDepth(1.0f);
		//m_blurFboIdColorTextureId で横方にぼかされてるので、縦方向にぼかしたものをm_iFBODepthで描画。m_iFBODepthはm_colorTextureIdに紐づいているので最終的なシャドウマップがこれ
		glViewport(0, 0, m_fDepthSize[0], m_fDepthSize[1]);
		m_shader.SetBlurParam(1.0f / static_cast<float>(m_fDepthSize[1]), 0 , blurParam );
		glBindTexture(GL_TEXTURE_2D, m_blurFboIdColorTextureId);//m_blurFboIdColorTextureId

		glBegin(GL_QUADS);
		glTexCoord2d(0, 0); glVertex3f(static_cast<float>(-m_fDepthSize[0] / 2), static_cast<float>(-m_fDepthSize[1] / 2), 0);
		glTexCoord2d(1, 0); glVertex3f(static_cast<float>(m_fDepthSize[0] / 2), static_cast<float>(-m_fDepthSize[1] / 2), 0);
		glTexCoord2d(1, 1); glVertex3f(static_cast<float>(m_fDepthSize[0] / 2), static_cast<float>(m_fDepthSize[1] / 2), 0);
		glTexCoord2d(0, 1); glVertex3f(static_cast<float>(-m_fDepthSize[0] / 2), static_cast<float>(m_fDepthSize[1] / 2), 0);
		glEnd();
		glBindTexture(GL_TEXTURE_2D, 0);
		DsGLBindFramebuffer(DS_GL_FRAMEBUFFER, 0);

		m_shader.DisableShader();
	}

	void DsShadowMapImp::_SetLightView(const DsLight& light, const DsCamera& cam)
	{
		//視点に近くなるように平行移動させる。
		const DsVec3d rendPos = cam.GetPos();
		const DsVec3d pos = light.GetPos() + rendPos;
		const DsVec3d lookPos = pos + light.GetDir();
		const DsVec3d up = DsVec3d::Up();
		gluLookAt(pos.x, pos.y, pos.z,
			lookPos.x, lookPos.y, lookPos.z,
			up.x, up.y, up.z);
	}
}

//static
DsShadowMap& DsShadowMap::Create( const DsRender& ren, DsShader& shader )
{
	DsShadowMap* ret = new DsShadowMapImp( ren, shader);
	DS_ASSERT(ret, "メモリ確保失敗");
	return *ret;
}