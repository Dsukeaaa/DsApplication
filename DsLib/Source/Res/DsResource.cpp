#include "DsPch.h"
#ifndef _DS_RESOURCE_
#include "Res/DsResource.h"
#endif
//他のヘッダ
#ifndef _DS_RES_ITEM_
#include "Res/DsResItem.h"
#endif
#ifndef _DS_MAP_RES_H_
#include "Res/DsMapRes.h"
#endif
#ifndef _DS_ANIM_SET_H_
#include "Res/DsAnimRes.h"
#endif
#ifndef _DS_HITRES_H_
#include "Res/DsHitRes.h"
#endif

using namespace std;
using namespace DsLib;


//static 
string DsResource::GetExt(const std::string& path)
{
	string::size_type extPos = path.find_last_of(".");
	return path.substr(extPos, path.size() - extPos);
}

DsResource::DsResource()
	: m_resItems()
{
}

DsResource::~DsResource()
{
	Finalize();
}

void DsResource::Finalize()
{
	if (!m_resItems.empty())
	{
		DS_ASSERT(0, "リソースの登録解除忘れがあります");
	}

	//一応解放しておく
	for (auto pRes : m_resItems) {
		delete pRes.second;
	}
	m_resItems.clear();
}