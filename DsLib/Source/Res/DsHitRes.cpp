#include "DsPch.h"

#ifndef _DS_HITRES_H_
#include "Res/DsHitRes.h"
#endif
#ifndef _DS_ANIM_RES_
#include "Res/DsAnimRes.h"
#endif
#ifndef _DS_RESOURCE_
#include "Res/DsResource.h"
#endif


using namespace std;
using namespace DsLib;


namespace
{
	struct _ReadFormat
	{
		const char* param;
		const char* type;
	};

	static const _ReadFormat s_readFormat[] =
	{
		{ "sizeX:",		"%lf"},
		{ "sizeY:",		"%lf"},
		{ "sizeZ:",		"%lf"},
		{ "weight:",	"%lf"},
		{ "boundCoef:",	"%lf"},
	};


	DsHitRes::Shape _LoadRes(const char* path)
	{
		DsHitRes::Shape ret;
		DsFile fs(path);
		if (fs.IsFail())
		{
			DS_ASSERT(false, "ファイルオープンに失敗");
			return ret;
		}

		for (string str; fs.GetLine(str);)
		{
			//スペース削除
			{
				string::size_type pos = str.find(" ");
				while (pos != string::npos)
				{
					str.replace(pos, 1, "");
					pos = str.find(" ", pos);
				}
			}
			//タブ削除
			{
				string deleteStr = "\t";
				string::size_type pos = str.find(deleteStr);
				while (pos != string::npos)
				{
					str.replace(pos, 1, "");
					pos = str.find(deleteStr, pos);
				}
			}

			//s_readFormatが読み込むパラメータなので、１行ごとにチェック
			const int num = sizeof(s_readFormat) / sizeof(s_readFormat[0]);
			for (int i = 0; i < num; ++i)
			{
				const string::size_type pos = str.rfind(s_readFormat[i].param);
				if (pos != string::npos)
				{
					string tmp = s_readFormat[i].param;
					tmp += s_readFormat[i].type;
					sscanf_s(str.c_str(), tmp.c_str(), &ret.params[i]);
				}
			}
			//if (str.find("sizeX:") != string::npos)
			//{
			//	scanf_s("sizeX:%lf", &ret.sizeX);
			//}
			//else if (str.find("sizeY:") != string::npos)
			//{
			//
			//}
			//else if (str.find("sizeZ:") != string::npos)
			//{
			//
			//}
			//else if (str.find("weight:") != string::npos)
			//{
			//
			//}
			//else if (str.find("boundCoef:") != string::npos)
			//{
			//
			//}
		}
		return ret;
	}
}

DsHitRes::DsHitRes()
	: DsResItem()
	, m_pAnimRes(NULL)
	, m_shape()
{
}

DsHitRes::DsHitRes(const Shape& shape)
	: DsResItem()
	, m_pAnimRes(NULL)
	, m_shape(shape)
	, m_path()
	, m_pResource(NULL)
{

}

DsHitRes::~DsHitRes()
{
	if (m_pResource) {
		m_pAnimRes = m_pResource->UnregisterItem<DsAnimRes>(m_path);
	}
	m_pResource = NULL;
}

//virtual
bool DsHitRes::Initialize(const char* path, DsResource& resource)
{
	bool ret = false;
	m_path = path;
	m_pResource = &resource;
	if (m_path.find(".dmdl") != string::npos)
	{
		//アニメモデルからヒット作成
		m_pAnimRes = resource.RegisterItem<DsAnimRes>(path);
		ret = true;
	}
	else if (m_path.find(".txt") != string::npos)
	{
		m_shape = _LoadRes(path);
		m_pAnimRes = NULL;
		ret = true;
	}
	else
	{
		DS_ASSERT(false, "HitResで未知のファイル");
		ret = false;
	}

	return ret;
}