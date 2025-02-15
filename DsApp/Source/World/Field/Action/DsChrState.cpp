#include "DsAppPch.h"
#include "World/Field/Action/DsChrState.h"
//他のヘッダ
#ifndef _DS_ACTION_REQUEST_
#include "World/Field/Action/DsActionRequest.h"
#endif
#include "World/Field/Action/DsChrStateDefine.h"
#include "World/Field/Action/DsActionFlags.h"

using namespace DsApp;


/*
次のステートの決め方：m_nextStateにenumを代入する
次のステートへの進ませ方：IsNext をtrueを返すようにする
アニメのループ設定はここでやっている
ツールに出すメリットがないのでコードに直接書く
*/


namespace
{

	//ステートグラフID登録
	static const int DS_REGISTER_STATE_GRAPH_ID = DsChrStateDefine::CHR_STATE_GRAPH_ID;
}


/*********************************************************
@brief 共通処理
**********************************************************/
//virtual
void DsChrState::OnActive(double dt)
{
	//現在のステートに設定
	m_nextState = m_myState;

	if (m_pAnimClip) {
		//補完時間をデフォルトに戻す
		m_pAnimClip->ClearInterpolationTime();
	}
}

//virtual
void DsChrState::Update(double dt)
{
	//アニメイベントからアニメ補間秒数設定
	double interpolationTime = 0;
	if (m_animFlags.GetAnimInterpolationTime(interpolationTime)) {
		//ブレンドされるので無効値では上書きしない
		//有効値ならどちらか不定
		if (m_pAnimClip) {
			m_pAnimClip->SetInterpolationTime(interpolationTime);
		}
	}
}

//virtual
void DsChrState::OnDeactive(double dt)
{

}


namespace
{
	bool _GetDmgState(CHR_STATE& state, DsDmgDir dmgDir) 
	{
		bool ret = false;
		switch (dmgDir)
		{
		case DsDmgDir::F:
			state = CHR_STATE::DAMAGE_F; ret = true;
			break;
		case DsDmgDir::B:
			state = CHR_STATE::DAMAGE_B; ret = true;
			break;
		case DsDmgDir::L:
			state = CHR_STATE::DAMAGE_L; ret = true;
			break;
		case DsDmgDir::R:
			state = CHR_STATE::DAMAGE_R; ret = true;
			break;
		}
		return ret;
	}

	/*********************************************************
	@brief 待機
	**********************************************************/
	class DsChrStateIdle : public DsChrState
	{
	public:
		DsChrStateIdle(const INIT_ARG& arg) :DsChrState(arg)
		{
			if (m_pAnimClip) {
				m_pAnimClip->SetLoop(true);
			}
		}

	private:
		virtual void Update(double dt) override
		{
			DsChrState::Update(dt);

			m_nextState = m_myState;

			//いつでもキャンセル可能
			m_actReq.SetCancelAll();

			if (m_actReq.IsAction(ACTION_TYPE::STEP)) {
				m_nextState = CHR_STATE::STEP_F;
			}
			else if (m_actReq.IsAction(ACTION_TYPE::ATTACK)) {
				m_nextState = CHR_STATE::ATTACK1;
			}
			else if (m_actReq.IsMove()) {
				m_nextState = CHR_STATE::RUN;
			}

			//ダメージ
			_GetDmgState(m_nextState, m_animFlags.GetDmgDir());

		};
	};
	DS_REGISTER_STATE(DsChrStateIdle)


	/*********************************************************
	@brief 走行
	**********************************************************/
	class DsChrStateRun : public DsChrState
	{
	public:
		DsChrStateRun(const INIT_ARG& arg) :DsChrState(arg)
		{
			if (m_pAnimClip) {
				m_pAnimClip->SetLoop(true);
			}
		}

	private:
		virtual void Update(double dt) override
		{
			DsChrState::Update(dt);

			m_nextState = m_myState;

			//いつでもキャンセル可能
			m_actReq.SetCancelAll();

			if (m_actReq.IsAction(ACTION_TYPE::STEP)) {
				m_nextState = CHR_STATE::STEP_F;
			}
			else if (m_actReq.IsAction(ACTION_TYPE::ATTACK)) {
				m_nextState = CHR_STATE::ATTACK1;
			}
			else if (!m_actReq.IsMove()) {
				m_nextState = CHR_STATE::IDLE;
			}
			
			//ダメージ
			_GetDmgState(m_nextState, m_animFlags.GetDmgDir());

		};
	};
	DS_REGISTER_STATE(DsChrStateRun)


	/*********************************************************
	@brief 攻撃１
	**********************************************************/
	class DsChrStateAttack1 : public DsChrState
	{
	public:
		DsChrStateAttack1(const INIT_ARG& arg) :DsChrState(arg)
		{
			if (m_pAnimClip) {
				m_pAnimClip->SetLoop(false);
			}
		}

	private:
		virtual void OnActive(double dt) override
		{
			DsChrState::OnActive(dt);
			m_animFlags.SetLockOnTurn(true);
		}

		virtual void Update(double dt) override
		{
			DsChrState::Update(dt);
			m_animFlags.SetLockOnTurn(true);

			m_nextState = m_myState;

			if (m_actReq.IsAction(ACTION_TYPE::ATTACK)) {
				//2段目攻撃
				m_nextState = CHR_STATE::ATTACK2;
			}
			else if (m_actReq.IsAction(ACTION_TYPE::MOVE)) {
				m_nextState = CHR_STATE::RUN;
			}
			else if (m_pAnimClip->IsEnd()) {
				m_nextState = CHR_STATE::IDLE;
			}

		};
	};
	DS_REGISTER_STATE(DsChrStateAttack1)


	/*********************************************************
	@brief 攻撃２
	**********************************************************/
	class DsChrStateAttack2 : public DsChrState
	{
	public:
		DsChrStateAttack2(const INIT_ARG& arg) :DsChrState(arg)
		{
			if (m_pAnimClip) {
				m_pAnimClip->SetLoop(false);
			}
		}

	private:
		virtual void OnActive(double dt) override
		{
			DsChrState::OnActive(dt);
			m_animFlags.SetLockOnTurn(true);
		}
		virtual void Update(double dt) override
		{
			DsChrState::Update(dt);
			//m_animFlags.SetLockOnTurn(true);

			m_nextState = m_myState;

			if (m_actReq.IsAction(ACTION_TYPE::ATTACK)) {
				m_nextState = CHR_STATE::ATTACK3;
			}
			else if (m_actReq.IsAction(ACTION_TYPE::MOVE)) {
				m_nextState = CHR_STATE::RUN;
			}
			else if (m_pAnimClip->IsEnd()) {
				m_nextState = CHR_STATE::IDLE;
			}

		};
	};
	DS_REGISTER_STATE(DsChrStateAttack2)


	/*********************************************************
	@brief 攻撃３
	**********************************************************/
	class DsChrStateAttack3 : public DsChrState
	{
	public:
		DsChrStateAttack3(const INIT_ARG& arg) :DsChrState(arg)
		{
			if (m_pAnimClip) {
				m_pAnimClip->SetLoop(false);
			}
		}

	private:
		virtual void OnActive(double dt) override
		{
			DsChrState::OnActive(dt);
			m_animFlags.SetLockOnTurn(true);
		}
		virtual void Update(double dt) override
		{
			DsChrState::Update(dt);
			//m_animFlags.SetLockOnTurn(true);

			m_nextState = m_myState;

			if (m_actReq.IsAction(ACTION_TYPE::ATTACK)) {
				m_nextState = CHR_STATE::ATTACK4;
			}
			else if (m_actReq.IsAction(ACTION_TYPE::MOVE)) {
				m_nextState = CHR_STATE::RUN;
			}
			else if (m_pAnimClip->IsEnd()) {
				m_nextState = CHR_STATE::IDLE;
			}

		};
	};
	DS_REGISTER_STATE(DsChrStateAttack3)



	/*********************************************************
	@brief 攻撃４
	**********************************************************/
	class DsChrStateAttack4 : public DsChrState
	{
	public:
		DsChrStateAttack4(const INIT_ARG& arg) :DsChrState(arg)
		{
			if (m_pAnimClip) {
				m_pAnimClip->SetLoop(false);
			}
		}

	private:
		virtual void OnActive(double dt) override
		{
			DsChrState::OnActive(dt);
			m_animFlags.SetLockOnTurn(true);
		}
		virtual void Update(double dt) override
		{
			DsChrState::Update(dt);
			//m_animFlags.SetLockOnTurn(true);

			m_nextState = m_myState;

			if (m_actReq.IsAction(ACTION_TYPE::ATTACK)) {
				m_nextState = CHR_STATE::ATTACK1;
			}
			else if (m_actReq.IsAction(ACTION_TYPE::MOVE)) {
				m_nextState = CHR_STATE::RUN;
			}
			else if (m_pAnimClip->IsEnd()) {
				m_nextState = CHR_STATE::IDLE;
			}

		};
	};
	DS_REGISTER_STATE(DsChrStateAttack4)


	/*********************************************************
	@brief ダメージ前
	**********************************************************/
	class DsChrStateDamageF : public DsChrState
	{
	public:
		DsChrStateDamageF(const INIT_ARG& arg) :DsChrState(arg)
		{
			if (m_pAnimClip) {
				m_pAnimClip->SetLoop(false);
			}
		}

	private:
		virtual void OnActive(double dt) override
		{
			DsChrState::OnActive(dt);
		}
		virtual void Update(double dt) override
		{
			DsChrState::Update(dt);

			m_nextState = m_myState;

			if (m_actReq.IsAction(ACTION_TYPE::ATTACK)) {
				m_nextState = CHR_STATE::ATTACK1;
			}
			else if (m_actReq.IsAction(ACTION_TYPE::MOVE)) {
				m_nextState = CHR_STATE::RUN;
			}
			else if (m_pAnimClip->IsEnd()) {
				m_nextState = CHR_STATE::IDLE;
			}

			//ダメージ
			bool isDmg = _GetDmgState(m_nextState, m_animFlags.GetDmgDir());
			if (isDmg) {//同じ方向かもしれないので再度アニメ再生かける
				m_pAnimClip->ResetAnim();
			}

		};
	};
	DS_REGISTER_STATE(DsChrStateDamageF)

	/*********************************************************
	@brief ダメージ後ろ
	**********************************************************/
	class DsChrStateDamageB : public DsChrState
	{
	public:
		DsChrStateDamageB(const INIT_ARG& arg) :DsChrState(arg)
		{
			if (m_pAnimClip) {
				m_pAnimClip->SetLoop(false);
			}
		}

	private:
		virtual void OnActive(double dt) override
		{
			DsChrState::OnActive(dt);
		}
		virtual void Update(double dt) override
		{
			DsChrState::Update(dt);

			m_nextState = m_myState;

			if (m_actReq.IsAction(ACTION_TYPE::ATTACK)) {
				m_nextState = CHR_STATE::ATTACK1;
			}
			else if (m_actReq.IsAction(ACTION_TYPE::MOVE)) {
				m_nextState = CHR_STATE::RUN;
			}
			else if (m_pAnimClip->IsEnd()) {
				m_nextState = CHR_STATE::IDLE;
			}

			//ダメージ
			bool isDmg = _GetDmgState(m_nextState, m_animFlags.GetDmgDir());
			if (isDmg) {//同じ方向かもしれないので再度アニメ再生かける
				m_pAnimClip->ResetAnim();
			}
		};
	};
	DS_REGISTER_STATE(DsChrStateDamageB)
		
	/*********************************************************
	@brief ダメージ左
	**********************************************************/
	class DsChrStateDamageL : public DsChrState
	{
	public:
		DsChrStateDamageL(const INIT_ARG& arg) :DsChrState(arg)
		{
			if (m_pAnimClip) {
				m_pAnimClip->SetLoop(false);
			}
		}

	private:
		virtual void OnActive(double dt) override
		{
			DsChrState::OnActive(dt);
		}
		virtual void Update(double dt) override
		{
			DsChrState::Update(dt);

			m_nextState = m_myState;

			if (m_actReq.IsAction(ACTION_TYPE::ATTACK)) {
				m_nextState = CHR_STATE::ATTACK1;
			}
			else if (m_actReq.IsAction(ACTION_TYPE::MOVE)) {
				m_nextState = CHR_STATE::RUN;
			}
			else if (m_pAnimClip->IsEnd()) {
				m_nextState = CHR_STATE::IDLE;
			}

			//ダメージ
			bool isDmg = _GetDmgState(m_nextState, m_animFlags.GetDmgDir());
			if (isDmg) {//同じ方向かもしれないので再度アニメ再生かける
				m_pAnimClip->ResetAnim();
			}
		};
	};
	DS_REGISTER_STATE(DsChrStateDamageL)

	/*********************************************************
	@brief ダメージ右
	**********************************************************/
	class DsChrStateDamageR : public DsChrState
	{
	public:
		DsChrStateDamageR(const INIT_ARG& arg) :DsChrState(arg)
		{
			if (m_pAnimClip) {
				m_pAnimClip->SetLoop(false);
			}
		}

	private:
		virtual void OnActive(double dt) override
		{
			DsChrState::OnActive(dt);
		}
		virtual void Update(double dt) override
		{
			DsChrState::Update(dt);

			m_nextState = m_myState;

			if (m_actReq.IsAction(ACTION_TYPE::ATTACK)) {
				m_nextState = CHR_STATE::ATTACK1;
			}
			else if (m_actReq.IsAction(ACTION_TYPE::MOVE)) {
				m_nextState = CHR_STATE::RUN;
			}
			else if (m_pAnimClip->IsEnd()) {
				m_nextState = CHR_STATE::IDLE;
			}

			//ダメージ
			bool isDmg = _GetDmgState(m_nextState, m_animFlags.GetDmgDir());
			if (isDmg) {//同じ方向かもしれないので再度アニメ再生かける
				m_pAnimClip->ResetAnim();
			}
		};
	};
	DS_REGISTER_STATE(DsChrStateDamageR)


	/*********************************************************
	@brief ステップ前
	**********************************************************/
	class DsChrStateStepF : public DsChrState
	{
	public:
		DsChrStateStepF(const INIT_ARG& arg) : DsChrState(arg)
		{
			if (m_pAnimClip) {
				m_pAnimClip->SetLoop(false);
			}
		}

	private:
		virtual void OnActive(double dt) override
		{
			DsChrState::OnActive(dt);
		}
		virtual void Update(double dt) override
		{
			DsChrState::Update(dt);

			m_nextState = m_myState;

			if (m_actReq.IsAction(ACTION_TYPE::ATTACK)) {
				m_nextState = CHR_STATE::ATTACK2;
			}
			else if (m_actReq.IsAction(ACTION_TYPE::STEP)) {
				m_nextState = CHR_STATE::STEP_F;
			}
			else if (m_actReq.IsAction(ACTION_TYPE::MOVE)) {
				m_nextState = CHR_STATE::RUN;
			}
			else if (m_pAnimClip->IsEnd()) {
				m_nextState = CHR_STATE::IDLE;
			}

			//ダメージ
			bool isDmg = _GetDmgState(m_nextState, m_animFlags.GetDmgDir());
			if (isDmg) {//同じ方向かもしれないので再度アニメ再生かける
				m_pAnimClip->ResetAnim();
			}
		};
	};
	DS_REGISTER_STATE(DsChrStateStepF)

	
	//デフォルトクラス情報
	static STATE_CLASS_TYPE s_registerClass[] =
	{
		{ typeid(DsChrStateIdle), CHR_STATE::IDLE, "idle" },
		{ typeid(DsChrStateRun), CHR_STATE::RUN, "run" },
		{ typeid(DsChrStateAttack1), CHR_STATE::ATTACK1, "Attack1" },
		{ typeid(DsChrStateAttack2), CHR_STATE::ATTACK2, "Attack2" },
		{ typeid(DsChrStateAttack3), CHR_STATE::ATTACK3, "Attack3" },
		{ typeid(DsChrStateAttack4), CHR_STATE::ATTACK4, "Attack4" },
		{ typeid(DsChrStateDamageF), CHR_STATE::DAMAGE_F, "DamageF" },
		{ typeid(DsChrStateDamageB), CHR_STATE::DAMAGE_B, "DamageB" },
		{ typeid(DsChrStateDamageL), CHR_STATE::DAMAGE_L, "DamageL" },
		{ typeid(DsChrStateDamageR), CHR_STATE::DAMAGE_R, "DamageR" },
		{ typeid(DsChrStateStepF), CHR_STATE::STEP_F, "StepF" },
		{ typeid(DsChrStateStepF), CHR_STATE::STEP_B, "StepB" },
		{ typeid(DsChrStateStepF), CHR_STATE::STEP_L, "StepL" },
		{ typeid(DsChrStateStepF), CHR_STATE::STEP_R, "StepR" },
	};
	//派生クラス登録コンテナ
	static std::map<int, DsRegisterClass*> s_registerClassContainer;
	//ステートインスタンス生成機
	static DsStateFactory* s_factoryTop = NULL;
}

//static
DsStateFactory* DsChrState::GetFactoryTop()
{
	return s_factoryTop;
}
//static
void DsChrState::SetFactoryTop(DsStateFactory* pFactory)
{
	s_factoryTop = pFactory;
}


//static
size_t DsChrState::GetStateClassTypesNum()
{
	return std::size(s_registerClass);
}
//static
STATE_CLASS_TYPE* DsChrState::GetStateClassTypes()
{
	return s_registerClass;
}

//static 
DsChrState* DsChrState::CreateIns(INIT_ARG arg, const std::string& animName, const int id )
{
	DsChrState* ret = NULL;

	auto it = s_registerClassContainer.find(id);
	if (it == s_registerClassContainer.end()) {
		DS_ASSERT(false, "ID[%d]は存在しません", id);
	}
	else {
		const std::vector<STATE_CLASS_TYPE>& registerClass = it->second->RefRegisterClasses();
		//クラス一覧から引数のステートに該当する型情報検索
		for (const STATE_CLASS_TYPE& classType : registerClass) {
			if (animName == classType.pName) {

				//登録されたファクトリーを検索
				DsStateFactory* pFactory = DsChrState::GetFactoryTop();
				DsStateFactory* pBaseFactory = NULL;
				while (pFactory) {
					if (pFactory->GetType() == classType.type) {
						if (pFactory->GetId() == id) {
							pBaseFactory = NULL;//共通処理はなし
							break;//目的のものを見つけたので終わり
						}
						else if (pFactory->GetId() == DsChrStateDefine::CHR_STATE_GRAPH_ID) {
							//とりあえず共通のはあったのでこれを入れておく
							pBaseFactory = pFactory;
							//まだ探索は続ける
						}
					}
					pFactory = pFactory->GetNext();
				}

				if ((!pFactory) && pBaseFactory) {
					//共通のしかなかったのでこれを採用
					pFactory = pBaseFactory;
				}

				if (pFactory) {
					arg.myState = classType.state;//ここでステート確定
					ret = pFactory->Create(arg);
					break;
				}

			}
		}
	}

	return ret;
}

//static
void DsChrState::AddRegisterClass(DsRegisterClass& registerClass)
{
	const int id = registerClass.GetId();
	auto it = s_registerClassContainer.find(id);
	if (it == s_registerClassContainer.end()) {
		s_registerClassContainer[id] = &registerClass;
		registerClass.Register();
	}
	else {
		DS_ASSERT(false, "キャラステート登録IDが重複しています %d", id);
	}
}


/*--------------------------------------------------------------
クラス登録
---------------------------------------------------------------*/
void DsRegisterClass::Register()
{
	if (m_registerClasses.empty()) {

		STATE_CLASS_TYPE* pClassTypes = DsChrState::GetStateClassTypes();
		for (int i = 0; i < DsChrState::GetStateClassTypesNum(); ++i) {
			STATE_CLASS_TYPE copy = pClassTypes[i];

			//基底のクラス名と同じものを派生のクラス名の中から探す
			std::string srcName = copy.type.name();

			for (int typeIdx = 0; typeIdx < m_overrideTypesNum; ++typeIdx) {
				const std::type_index& typeIndex = m_pOverwriteTypes[typeIdx];
				std::string myName = typeIndex.name();

				//namespaceを取り除いたクラス名
				auto getClassName = [](std::string name)->std::string
				{
					const std::string token = "::";
					auto pos = name.rfind(token);
					if (pos == std::string::npos) {
						return name;//namespaceがないのでそのまま
					}
					else {
						std::string ret = name.c_str() + pos + token.length();
						return ret;
					}
				};
			

				std::string cName1 = getClassName(myName);
				std::string cName2 = getClassName(srcName);

				if(cName1 == cName2){
					//派生に同じクラス名があったので型を上書き
					copy.type = typeIndex;
					break;
				}
			}
			m_registerClasses.push_back(copy);
		}
	}
}

