#include "DsAppPch.h"
#include "World/Field/Action/DsChrStateDefine.h"
//���̃w�b�_
#include "World/Field/Action/DsChrState.h"
#include "World/Field/Action/Player/DsPlayerState.h"
#include "World/Field/Action/Enemy/DsEnemyState1.h"

using namespace DsApp;
using namespace DsChrStateDefine;


void DsChrStateDefine::InitializeState()
{
	static bool isInit = false;
	if (!isInit) {
		DsPlayerState::Initialize();
		DsEnemyState1::Initialize();
		isInit = true;
	}
}