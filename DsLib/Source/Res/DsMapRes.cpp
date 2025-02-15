#include "DsPch.h"
#ifndef _DS_MAP_RES_H_
#include "Res/DsMapRes.h"
#endif

using namespace DsLib;

namespace
{
	struct ObjInsFormat
	{
		ObjInsFormat()
			: objInsName("")
			, drawModelPath("")
			, hitModelPath("")
			, posX(0.0)
			, posY(0.0)
			, posZ(0.0)
			, angX(0.0)
			, angY(0.0)
			, angZ(0.0)
			, objType(0)
			, fieldObjType(0)
		{
		}
		std::string objInsName;
		std::string drawModelPath;
		std::string hitModelPath;
		double posX;
		double posY;
		double posZ;
		double angX;
		double angY;
		double angZ;
		int objType;
		int fieldObjType;
	};

	struct OutputRes
	{
		~OutputRes()
		{
			for(ObjInsFormat* obj : objs)
			{
				delete obj;
			}
			objs.clear();
		}

		int version;
		std::vector<ObjInsFormat*> objs;
	};

	OutputRes* _LoadRes(const char* path)
	{
		OutputRes* pRes = new OutputRes();

		DsFile fs(path, std::ios::out | std::ios::binary);
		if (fs.IsFail())
		{
			DS_ASSERT(false, "ファイルオープンに失敗");
			return pRes;
		}

		{//version
			long ver;
			fs.Read((ds_uint8*)(&ver), sizeof(long));
			pRes->version = ver;
		}

		int objNum = 0;
		{//objNum
			fs.Read((ds_uint8*)(&objNum), sizeof(int));
		}

		for (int oi = 0; oi < objNum; ++oi)
		{//obj
			ObjInsFormat* obj = new ObjInsFormat();

			int tmpCharNum = 0;
			char* tmpChar = 0;

			fs.Read((ds_uint8*)(&tmpCharNum), sizeof(int));
			tmpChar = new char[tmpCharNum+1];
			fs.Read((ds_uint8*)tmpChar, tmpCharNum);
			tmpChar[tmpCharNum] = '\0';
			obj->objInsName = tmpChar;
			delete[] tmpChar;

			fs.Read((ds_uint8*)(&tmpCharNum), sizeof(int));
			tmpChar = new char[tmpCharNum+1];
			fs.Read((ds_uint8*)tmpChar, tmpCharNum);
			tmpChar[tmpCharNum] = '\0';
			obj->drawModelPath = tmpChar;
			delete[] tmpChar;

			fs.Read((ds_uint8*)(&tmpCharNum), sizeof(int));
			tmpChar = new char[tmpCharNum+1];
			fs.Read((ds_uint8*)tmpChar, tmpCharNum);
			tmpChar[tmpCharNum] = '\0';
			obj->hitModelPath = tmpChar;
			delete[] tmpChar;

			fs.Read((ds_uint8*)(&obj->posX), sizeof(double));
			fs.Read((ds_uint8*)(&obj->posY), sizeof(double));
			fs.Read((ds_uint8*)(&obj->posZ), sizeof(double));
			fs.Read((ds_uint8*)(&obj->angX), sizeof(double));
			fs.Read((ds_uint8*)(&obj->angY), sizeof(double));
			fs.Read((ds_uint8*)(&obj->angZ), sizeof(double));

			fs.Read((ds_uint8*)(&obj->objType), sizeof(int));
			fs.Read((ds_uint8*)(&obj->fieldObjType), sizeof(int));
			
			pRes->objs.push_back(obj);
		}
		return pRes;
	}
}

DsMapRes::DsMapRes()
	: DsResItem()
	, m_resTop(NULL)
	, m_data()
{
}

//virtual
bool DsMapRes::Initialize(const char* path, DsResource& resource)
{
	OutputRes* pRes = _LoadRes(path);
	m_resTop = pRes;

	
	for(const ObjInsFormat* obj : pRes->objs)
	{
		DATA data;
		data.pName = obj->objInsName.c_str();
		data.pAnimPath = obj->drawModelPath.c_str();
		data.pHitPath = obj->hitModelPath.c_str();
		data.objType = static_cast<DS_MAP_OBJ_TYPE>(obj->objType);
		data.fieldObjType = static_cast<DS_MAP_FIELD_OBJ_TYPE>(obj->fieldObjType);
		data.pos = DsVec3d(obj->posX, obj->posY, obj->posZ);
		data.ang = DsVec3d(DegToRad(obj->angX), DegToRad(obj->angY), DegToRad(obj->angZ));
		m_data.push_back(data);
	}

	return true;
}

DsMapRes::~DsMapRes()
{
	OutputRes* pRes = static_cast<OutputRes*>(m_resTop);
	delete pRes; 
	m_resTop = 0;
}

