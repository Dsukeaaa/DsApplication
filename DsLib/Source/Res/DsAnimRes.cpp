#include "DsPch.h"

#ifndef _DS_ANIM_RES_
#include "Res/DsAnimRes.h"
#endif
//他のヘッダ
#ifndef _DS_AMIM_SKELETON_
#include "Animation/DsAnimSkeleton.h"
#endif
#ifndef _DS_KEYFRAME_ANIM_
#include "Animation/DsKeyframeAnim.h"
#endif
#ifndef _DS_KEYFRAME_ANIM_SET_
#include "Animation/DsKeyframeAnimSet.h"
#endif
#ifndef _DS_MODEL_
#include "Model/DsModel.h"
#endif
#ifndef _DS_AMIM_RAGDOLL_INFO_
#include "Animation/DsAnimCustomProperty.h"
#endif
#include "Res/Param/DsMaterialParam.h"

using namespace DsLib;

namespace
{
//--------------------------------------------------------------------------------
//ここから構造体定義
//--------------------------------------------------------------------------------
	typedef unsigned long vari_size;

	struct DS_VERTEX
	{
		union
		{
			float v[4];
			struct
			{
				float x;
				float y;
				float z;
				float w;
			};
		};

		unsigned long long GetSize() const
		{
			return sizeof(DS_VERTEX);
		}
	};


	struct DS_FACE
	{
		DS_FACE()
			: vn(0)
			, pIndex(0)
			, normal()
		{
		}

		~DS_FACE()
		{
			vn = 0;
			delete[] pIndex;
			pIndex = 0;
		}

		vari_size vn;
		vari_size* pIndex;
		DS_VERTEX normal;

		unsigned long long GetSize() const
		{
			return (sizeof(vari_size) + normal.GetSize() + (vn*sizeof(vari_size)));
		}
	};

	struct DS_DMYPOLY
	{
		vari_size vIndex[3];
		vari_size id;
	};

	struct DS_TEXTURE
	{
		struct UV
		{
			UV()
				:vertexIndex(-1)
			{
				uv[0] = 0;
				uv[1] = 0;
			}
			float uv[2];
			int vertexIndex;
		};

		struct UV_FACE
		{
			UV_FACE()
				:uvpNum(0)
				, uvpIndex(0)
				, refGeomFaceIndex(-1)
			{}
			~UV_FACE()
			{
				delete[] uvpIndex;
				uvpIndex = 0;
			}
			int uvpNum;
			int* uvpIndex;
			int refGeomFaceIndex;
		};


		DS_TEXTURE()
			: albedoTexPath(0)
			, normalTexPath(0)
			, specularTexPath(0)
			, uvNum(0)
			, uv(0)
		{
		}

		~DS_TEXTURE()
		{
			delete[] albedoTexPath;
			albedoTexPath = 0;
			delete[] normalTexPath;
			normalTexPath = 0;
			delete[] specularTexPath;
			specularTexPath = 0;
			delete[] uv;
			uv = 0;
			delete[] uvFace;
			uvFaceNum = 0;
		}

		char* albedoTexPath;
		char* normalTexPath;
		char* specularTexPath;
		int uvNum;
		UV* uv;
		int uvFaceNum;
		UV_FACE * uvFace;
	};

	struct DS_MATERIAL
	{
		DS_MATERIAL()
			: textureNum(0)
			, texture(0)
			, ambient{}
			, diffuse{}
			, emissive{}
			, specular{}
			, shininess(0)
			, materialParamId(0)
		{
		}
		~DS_MATERIAL()
		{
			delete[] texture;
			texture = 0;
		}
		int textureNum;
		DS_TEXTURE* texture;
		float ambient[3];
		float diffuse[3];
		float emissive[3];
		float specular[3];
		float shininess;
		int materialParamId;
	};

	struct DS_ANIM_MODEL
	{
		DS_ANIM_MODEL()
			: vn(0)
			, pVertex(0)
			, fn(0)
			, pFace(0)
			, dn(0)
			, pDmypoly(0)
			, mn(0)
			, pMtr(0)
			, vertexSize(0)
			, faceSize(0)
		{
		}

		~DS_ANIM_MODEL()
		{
			vn = 0;
			delete[] pVertex;
			pVertex = 0;
			fn = 0;
			delete[] pFace;
			pFace = 0;
			delete[] pDmypoly;
			pDmypoly = 0;
			delete[] pMtr;
			pMtr = 0;
		}

		vari_size vn;
		DS_VERTEX* pVertex;

		vari_size fn;
		DS_FACE* pFace;

		vari_size dn;
		DS_DMYPOLY* pDmypoly;

		vari_size mn;
		DS_MATERIAL* pMtr;

		unsigned long long vertexSize;	//vertexのみのサイズ
		unsigned long long faceSize;		//faceのみのサイズ

	};

	struct DS_MAT
	{
		enum{ SIZE = 64 };

		DS_MAT(){}

		DS_MAT& operator =(const double src[4][4])
		{
			for (int col = 0; col < 4; ++col)
			{
				for (int row = 0; row < 4; ++row)
				{
					m[col][row] = static_cast<float>(src[col][row]);
				}
			}
			return (*this);
		}

		float m[4][4];
	};

	struct DS_BONE
	{
		DS_BONE()
			: indexNum(0)
			, pIndex(0)
			, pWeight(0)
			, initMat()
			, nameSize(0)
			, name(0)
			, parentIdx(0)
			, childNum(0)
			, pChildIdx(0)
			, isMaster(0)
		{}

		~DS_BONE()
		{
			indexNum = 0;
			delete[] pIndex;
			pIndex = 0;
			delete[] pWeight;
			pWeight = 0;

			delete[] name;
			name = 0;
			nameSize = 0;

			delete[] pChildIdx;
			pChildIdx = 0;
		}

		int indexNum;
		vari_size* pIndex;
		float* pWeight;

		DS_MAT initMat;

		size_t nameSize;
		char* name;

		int parentIdx;
		int childNum;
		int* pChildIdx;

		ds_uint8 isMaster;

	};

	struct DS_KEY_FRAME
	{
		float localTimeMs;
		float value;
	};



	struct DS_ANIM_POSE
	{
		DS_ANIM_POSE()
			: keyFrameNumTX(0)
			, keyFrameTX(0)
			, keyFrameNumTY(0)
			, keyFrameTY(0)
			, keyFrameNumTZ(0)
			, keyFrameTZ(0)

			, keyFrameNumRX(0)
			, keyFrameRX(0)
			, keyFrameNumRY(0)
			, keyFrameRY(0)
			, keyFrameNumRZ(0)
			, keyFrameRZ(0)
			, keyFrameNumRQ(0)
			, keyFrameRQ(0)

			, keyFrameNumSX(0)
			, keyFrameSX(0)
			, keyFrameNumSY(0)
			, keyFrameSY(0)
			, keyFrameNumSZ(0)
			, keyFrameSZ(0)
		{
		}

		~DS_ANIM_POSE()
		{
			delete[] keyFrameTX;
			keyFrameTX = 0;
			delete[] keyFrameTY;
			keyFrameTY = 0;
			delete[] keyFrameTZ;
			keyFrameTZ = 0;

			delete[] keyFrameRX;
			keyFrameRX = 0;
			delete[] keyFrameRY;
			keyFrameRY = 0;
			delete[] keyFrameRZ;
			keyFrameRZ = 0;
			delete[] keyFrameRQ;
			keyFrameRQ = 0;

			delete[] keyFrameSX;
			keyFrameSX = 0;
			delete[] keyFrameSY;
			keyFrameSY = 0;
			delete[] keyFrameSZ;
			keyFrameSZ = 0;

		}

		long keyFrameNumTX;
		DS_KEY_FRAME* keyFrameTX;

		long keyFrameNumTY;
		DS_KEY_FRAME* keyFrameTY;

		long keyFrameNumTZ;
		DS_KEY_FRAME* keyFrameTZ;

		long keyFrameNumRX;
		DS_KEY_FRAME* keyFrameRX;

		long keyFrameNumRY;
		DS_KEY_FRAME* keyFrameRY;

		long keyFrameNumRZ;
		DS_KEY_FRAME* keyFrameRZ;

		long keyFrameNumRQ;
		DS_KEY_FRAME* keyFrameRQ;

		long keyFrameNumSX;
		DS_KEY_FRAME* keyFrameSX;

		long keyFrameNumSY;
		DS_KEY_FRAME* keyFrameSY;

		long keyFrameNumSZ;
		DS_KEY_FRAME* keyFrameSZ;
	};

	struct DS_ANIM
	{
		static long PoseNum;

		DS_ANIM()
			: animNameSize(0)
			, animName(0)
			, poseNum(PoseNum)
			, pose(0)
			, masterMove()
		{
			pose = new DS_ANIM_POSE[poseNum];
		}

		~DS_ANIM()
		{
			delete[] animName;
			animName = 0;
			delete[] pose;
			pose = 0;
		}

		size_t animNameSize;
		char* animName;

		long poseNum;
		DS_ANIM_POSE* pose;

		DS_ANIM_POSE masterMove;
	};
	long DS_ANIM::PoseNum;

	struct DS_ANIM_BONE
	{
		DS_ANIM_BONE()
			: bn(0)
			, pBone(0)
			, an(0)
			, pAnim(0)
		{}

		~DS_ANIM_BONE()
		{
			bn = 0;
			delete[] pBone;
			pBone = 0;
			an = 0;
			delete[] pAnim;
			pAnim = 0;
		}

		long bn;
		DS_BONE* pBone;

		long an;
		DS_ANIM* pAnim;
	};

	struct DS_RAGDOLL_PARAM_ID
	{
		DS_RAGDOLL_PARAM_ID()
			: id(0)
			, boneIdx(0)
		{}
		int id;
		unsigned long boneIdx;
	};

	struct DS_CUSTOM_PROPERTY
	{
		DS_CUSTOM_PROPERTY()
			: pRagdollParamId(NULL)
			, ragdollNum(0)
		{}
		~DS_CUSTOM_PROPERTY()
		{
			delete[] pRagdollParamId;
			pRagdollParamId = NULL;
		}
		DS_RAGDOLL_PARAM_ID* pRagdollParamId;
		long ragdollNum;
	};

	struct DS_LIGHT
	{
		DS_LIGHT()
			: type(0)
			, distance(0)
			, energy(0)
			//, color{0}
		{}
		int type;
		float distance;
		float energy;
		float color[3];
	};

	struct DS_LIGHT_DATA
	{
		std::vector<DS_LIGHT> lights;
	};

	class OutputRes
	{
	public:
		OutputRes()
			: dsAnimModel()
			, dsAnimBone()
			, dsCustomProperty()
			, dsLight()
			, dataSize(0)
		{};
		virtual ~OutputRes(){};

	public:
		DS_ANIM_MODEL dsAnimModel;
		DS_ANIM_BONE dsAnimBone;
		DS_CUSTOM_PROPERTY dsCustomProperty;
		DS_LIGHT_DATA dsLight;
		unsigned long long dataSize;
	};
//--------------------------------------------------------------------------------
//ここまで構造体定義
//--------------------------------------------------------------------------------



	OutputRes* _LoadAnim(const char* path)
	{
		OutputRes& res= *(new OutputRes());

		DsFile fs(path, std::ios::out | std::ios::binary);
		if (fs.IsFail())
		{
			DS_ASSERT(false, "ファイルオープンに失敗");
			return &res;
		}

		long version=0;
		{//header
			fs.Read((ds_uint8*)(&version), sizeof(long));

			unsigned long long vertexOffset;
			fs.Read((ds_uint8*)(&vertexOffset), sizeof(vertexOffset));

			unsigned long long faceOffset;
			fs.Read((ds_uint8*)(&faceOffset), sizeof(faceOffset));
		}


		///////////////////////////////ここからモデル
		{//vertex
			vari_size vn;
			fs.Read((ds_uint8*)(&vn), sizeof(vn));
			res.dsAnimModel.vn = vn;

			res.dsAnimModel.pVertex = new DS_VERTEX[vn];
			fs.Read((ds_uint8*)(res.dsAnimModel.pVertex), sizeof(DS_VERTEX)*vn);
		}
		{//face
			vari_size fn;
			fs.Read((ds_uint8*)(&fn), sizeof(fn));
			res.dsAnimModel.fn = fn;

			res.dsAnimModel.pFace = new DS_FACE[fn];
			for (vari_size fIdx = 0; fIdx < fn; ++fIdx)
			{
				vari_size vn;
				fs.Read((ds_uint8*)(&vn), sizeof(vn));
				res.dsAnimModel.pFace[fIdx].vn = vn;

				fs.Read((ds_uint8*)(res.dsAnimModel.pFace[fIdx].normal.v), sizeof(DS_VERTEX));

				res.dsAnimModel.pFace[fIdx].pIndex = new vari_size[vn];
				fs.Read((ds_uint8*)(res.dsAnimModel.pFace[fIdx].pIndex), sizeof(vari_size)*vn);
			}
		}
		{//dmypoly
			vari_size dn;
			fs.Read((ds_uint8*)(&dn), sizeof(dn));
			res.dsAnimModel.dn = dn;

			res.dsAnimModel.pDmypoly = new DS_DMYPOLY[dn];
			fs.Read((ds_uint8*)(res.dsAnimModel.pDmypoly), sizeof(DS_DMYPOLY)*dn);
		}

		{//material
			vari_size mn;
			fs.Read((ds_uint8*)(&mn), sizeof(mn));
			res.dsAnimModel.pMtr = new DS_MATERIAL[mn];
			res.dsAnimModel.mn = mn;

			for (vari_size mi = 0; mi < mn; ++mi)
			{
				fs.Read((ds_uint8*)(&res.dsAnimModel.pMtr[mi].ambient[0]), sizeof(res.dsAnimModel.pMtr[mi].ambient));
				fs.Read((ds_uint8*)(&res.dsAnimModel.pMtr[mi].diffuse[0]), sizeof(res.dsAnimModel.pMtr[mi].diffuse));
				fs.Read((ds_uint8*)(&res.dsAnimModel.pMtr[mi].emissive[0]), sizeof(res.dsAnimModel.pMtr[mi].emissive));
				fs.Read((ds_uint8*)(&res.dsAnimModel.pMtr[mi].specular[0]), sizeof(res.dsAnimModel.pMtr[mi].specular));
				fs.Read((ds_uint8*)(&res.dsAnimModel.pMtr[mi].shininess), sizeof(res.dsAnimModel.pMtr[mi].shininess));

				fs.Read((ds_uint8*)(&res.dsAnimModel.pMtr[mi].materialParamId), sizeof(res.dsAnimModel.pMtr[mi].materialParamId));

				vari_size tn;
				fs.Read((ds_uint8*)(&tn), sizeof(tn));
				res.dsAnimModel.pMtr[mi].textureNum = tn;
				res.dsAnimModel.pMtr[mi].texture = new DS_TEXTURE[tn];
				
				for (vari_size ti = 0; ti < tn; ++ti)
				{
					{
						int nameSize;
						fs.Read((ds_uint8*)(&nameSize), sizeof(nameSize));
						res.dsAnimModel.pMtr[mi].texture[ti].albedoTexPath = new char[nameSize + 1];
						fs.Read((ds_uint8*)res.dsAnimModel.pMtr[mi].texture[ti].albedoTexPath, nameSize);
						res.dsAnimModel.pMtr[mi].texture[ti].albedoTexPath[nameSize] = '\0'; //終端文字が入ってないので。
					}
					{
						int nameSize;
						fs.Read((ds_uint8*)(&nameSize), sizeof(nameSize));
						res.dsAnimModel.pMtr[mi].texture[ti].normalTexPath = new char[nameSize + 1];
						fs.Read((ds_uint8*)res.dsAnimModel.pMtr[mi].texture[ti].normalTexPath, nameSize);
						res.dsAnimModel.pMtr[mi].texture[ti].normalTexPath[nameSize] = '\0'; //終端文字が入ってないので。
					}
					{
						int nameSize;
						fs.Read((ds_uint8*)(&nameSize), sizeof(nameSize));
						res.dsAnimModel.pMtr[mi].texture[ti].specularTexPath = new char[nameSize + 1];
						fs.Read((ds_uint8*)res.dsAnimModel.pMtr[mi].texture[ti].specularTexPath, nameSize);
						res.dsAnimModel.pMtr[mi].texture[ti].specularTexPath[nameSize] = '\0'; //終端文字が入ってないので。
					}

					vari_size uvNum;
					fs.Read((ds_uint8*)(&uvNum), sizeof(uvNum));
					res.dsAnimModel.pMtr[mi].texture[ti].uvNum = uvNum;
					res.dsAnimModel.pMtr[mi].texture[ti].uv = new DS_TEXTURE::UV[uvNum];
					fs.Read((ds_uint8*)(res.dsAnimModel.pMtr[mi].texture[ti].uv), sizeof(DS_TEXTURE::UV)*uvNum);

					vari_size uvFaceNum;
					fs.Read((ds_uint8*)(&uvFaceNum), sizeof(uvFaceNum));
					res.dsAnimModel.pMtr[mi].texture[ti].uvFaceNum = uvFaceNum;
					res.dsAnimModel.pMtr[mi].texture[ti].uvFace = new DS_TEXTURE::UV_FACE[uvFaceNum];
					for (vari_size uvfi = 0; uvfi < uvFaceNum; ++uvfi)
					{
						vari_size uvpNum;
						fs.Read((ds_uint8*)(&uvpNum), sizeof(uvpNum));
						res.dsAnimModel.pMtr[mi].texture[ti].uvFace[uvfi].uvpNum = uvpNum;
						vari_size refGeomFaceIndex;
						fs.Read((ds_uint8*)(&refGeomFaceIndex), sizeof(refGeomFaceIndex));
						res.dsAnimModel.pMtr[mi].texture[ti].uvFace[uvfi].refGeomFaceIndex = refGeomFaceIndex;

						res.dsAnimModel.pMtr[mi].texture[ti].uvFace[uvfi].uvpIndex = new int[uvpNum];
						fs.Read((ds_uint8*)(res.dsAnimModel.pMtr[mi].texture[ti].uvFace[uvfi].uvpIndex), sizeof(int)*uvpNum);
					}
				}
			}
		}



		/////////////////////////ここからキーフレームアニメ
		{//bone
			unsigned long boneNum;
			fs.Read((ds_uint8*)(&boneNum), sizeof(boneNum));
			res.dsAnimBone.bn = boneNum;
			res.dsAnimBone.pBone = new DS_BONE[boneNum];

			for (unsigned long bIdx = 0; bIdx < boneNum; ++bIdx)
			{
				unsigned long nameSize;
				fs.Read((ds_uint8*)(&nameSize), sizeof(nameSize));
				res.dsAnimBone.pBone[bIdx].nameSize = nameSize;
				res.dsAnimBone.pBone[bIdx].name = new char[nameSize];

				fs.Read((ds_uint8*)res.dsAnimBone.pBone[bIdx].name, nameSize);

				fs.Read((ds_uint8*)(&res.dsAnimBone.pBone[bIdx].initMat.m[0][0]), DS_MAT::SIZE);

				long indexNum;
				fs.Read((ds_uint8*)(&indexNum), sizeof(indexNum));
				res.dsAnimBone.pBone[bIdx].indexNum = indexNum;

				//ボーンに紐づく頂点インデックス
				res.dsAnimBone.pBone[bIdx].pIndex = new vari_size[indexNum];
				fs.Read((ds_uint8*)(res.dsAnimBone.pBone[bIdx].pIndex), sizeof(vari_size)*indexNum);
				
				//ボーンに紐づく頂点に対する重み
				res.dsAnimBone.pBone[bIdx].pWeight = new float[indexNum];
				fs.Read((ds_uint8*)(res.dsAnimBone.pBone[bIdx].pWeight), sizeof(float)*indexNum);

				int parentIdx = -1;
				fs.Read((ds_uint8*)(&parentIdx), sizeof(parentIdx));
				res.dsAnimBone.pBone[bIdx].parentIdx = parentIdx;
				int childNum = 0;
				fs.Read((ds_uint8*)(&childNum), sizeof(childNum));
				res.dsAnimBone.pBone[bIdx].childNum = childNum;
				if (0 < childNum)
				{
					res.dsAnimBone.pBone[bIdx].pChildIdx = new int[childNum];
					fs.Read((ds_uint8*)(res.dsAnimBone.pBone[bIdx].pChildIdx), sizeof(int)*childNum);
				}
				ds_uint8 isMaster = 0;
				fs.Read((ds_uint8*)(&isMaster), sizeof(isMaster));
				res.dsAnimBone.pBone[bIdx].isMaster = isMaster;
			}
		}

		{//anim
			unsigned long animNum;
			fs.Read((ds_uint8*)(&animNum), sizeof(animNum));
			res.dsAnimBone.an = animNum;
			res.dsAnimBone.pAnim = new DS_ANIM[animNum];

			//アニメーション
			for (unsigned long aIdx = 0; aIdx < animNum; ++aIdx)
			{
				unsigned long animNameSize;
				fs.Read((ds_uint8*)(&animNameSize), sizeof(animNameSize));
				res.dsAnimBone.pAnim[aIdx].animNameSize = animNameSize;
				res.dsAnimBone.pAnim[aIdx].animName = new char[animNameSize];

				fs.Read((ds_uint8*)res.dsAnimBone.pAnim[aIdx].animName, animNameSize);

				unsigned long poseNum;
				fs.Read((ds_uint8*)(&poseNum), sizeof(poseNum));
				res.dsAnimBone.pAnim[aIdx].poseNum = poseNum;
				res.dsAnimBone.pAnim[aIdx].pose = new DS_ANIM_POSE[poseNum];

				//ポーズ
				for (unsigned long poseIdx = 0; poseIdx < poseNum; ++poseIdx)
				{
					{
						long keyFrameNum;
						fs.Read((ds_uint8*)(&keyFrameNum), sizeof(keyFrameNum));
						res.dsAnimBone.pAnim[aIdx].pose[poseIdx].keyFrameNumTX = keyFrameNum;
						res.dsAnimBone.pAnim[aIdx].pose[poseIdx].keyFrameTX = new DS_KEY_FRAME[keyFrameNum];
						fs.Read((ds_uint8*)(res.dsAnimBone.pAnim[aIdx].pose[poseIdx].keyFrameTX), sizeof(DS_KEY_FRAME)*keyFrameNum);
					}
					{
						long keyFrameNum;
						fs.Read((ds_uint8*)(&keyFrameNum), sizeof(keyFrameNum));
						res.dsAnimBone.pAnim[aIdx].pose[poseIdx].keyFrameNumTY = keyFrameNum;
						res.dsAnimBone.pAnim[aIdx].pose[poseIdx].keyFrameTY = new DS_KEY_FRAME[keyFrameNum];
						fs.Read((ds_uint8*)(res.dsAnimBone.pAnim[aIdx].pose[poseIdx].keyFrameTY), sizeof(DS_KEY_FRAME)*keyFrameNum);
					}
					{
						long keyFrameNum;
						fs.Read((ds_uint8*)(&keyFrameNum), sizeof(keyFrameNum));
						res.dsAnimBone.pAnim[aIdx].pose[poseIdx].keyFrameNumTZ = keyFrameNum;
						res.dsAnimBone.pAnim[aIdx].pose[poseIdx].keyFrameTZ = new DS_KEY_FRAME[keyFrameNum];
						fs.Read((ds_uint8*)(res.dsAnimBone.pAnim[aIdx].pose[poseIdx].keyFrameTZ), sizeof(DS_KEY_FRAME)*keyFrameNum);
					}


					{
						long keyFrameNum;
						fs.Read((ds_uint8*)(&keyFrameNum), sizeof(keyFrameNum));
						res.dsAnimBone.pAnim[aIdx].pose[poseIdx].keyFrameNumRX = keyFrameNum;
						res.dsAnimBone.pAnim[aIdx].pose[poseIdx].keyFrameRX = new DS_KEY_FRAME[keyFrameNum];
						fs.Read((ds_uint8*)(res.dsAnimBone.pAnim[aIdx].pose[poseIdx].keyFrameRX), sizeof(DS_KEY_FRAME)*keyFrameNum);
					}
					{
						long keyFrameNum;
						fs.Read((ds_uint8*)(&keyFrameNum), sizeof(keyFrameNum));
						res.dsAnimBone.pAnim[aIdx].pose[poseIdx].keyFrameNumRY = keyFrameNum;
						res.dsAnimBone.pAnim[aIdx].pose[poseIdx].keyFrameRY = new DS_KEY_FRAME[keyFrameNum];
						fs.Read((ds_uint8*)(res.dsAnimBone.pAnim[aIdx].pose[poseIdx].keyFrameRY), sizeof(DS_KEY_FRAME)*keyFrameNum);
					}
					{
						long keyFrameNum;
						fs.Read((ds_uint8*)(&keyFrameNum), sizeof(keyFrameNum));
						res.dsAnimBone.pAnim[aIdx].pose[poseIdx].keyFrameNumRZ = keyFrameNum;
						res.dsAnimBone.pAnim[aIdx].pose[poseIdx].keyFrameRZ = new DS_KEY_FRAME[keyFrameNum];
						fs.Read((ds_uint8*)(res.dsAnimBone.pAnim[aIdx].pose[poseIdx].keyFrameRZ), sizeof(DS_KEY_FRAME)*keyFrameNum);
					}
					{
						long keyFrameNum;
						fs.Read((ds_uint8*)(&keyFrameNum), sizeof(keyFrameNum));
						res.dsAnimBone.pAnim[aIdx].pose[poseIdx].keyFrameNumRQ = keyFrameNum;
						res.dsAnimBone.pAnim[aIdx].pose[poseIdx].keyFrameRQ = new DS_KEY_FRAME[keyFrameNum];
						fs.Read((ds_uint8*)(res.dsAnimBone.pAnim[aIdx].pose[poseIdx].keyFrameRQ), sizeof(DS_KEY_FRAME)*keyFrameNum);
					}


					{
						long keyFrameNum;
						fs.Read((ds_uint8*)(&keyFrameNum), sizeof(keyFrameNum));
						res.dsAnimBone.pAnim[aIdx].pose[poseIdx].keyFrameNumSX = keyFrameNum;
						res.dsAnimBone.pAnim[aIdx].pose[poseIdx].keyFrameSX = new DS_KEY_FRAME[keyFrameNum];
						fs.Read((ds_uint8*)(res.dsAnimBone.pAnim[aIdx].pose[poseIdx].keyFrameSX), sizeof(DS_KEY_FRAME)*keyFrameNum);
					}
					{
						long keyFrameNum;
						fs.Read((ds_uint8*)(&keyFrameNum), sizeof(keyFrameNum));
						res.dsAnimBone.pAnim[aIdx].pose[poseIdx].keyFrameNumSY = keyFrameNum;
						res.dsAnimBone.pAnim[aIdx].pose[poseIdx].keyFrameSY = new DS_KEY_FRAME[keyFrameNum];
						fs.Read((ds_uint8*)(res.dsAnimBone.pAnim[aIdx].pose[poseIdx].keyFrameSY), sizeof(DS_KEY_FRAME)*keyFrameNum);
					}
					{
						long keyFrameNum;
						fs.Read((ds_uint8*)(&keyFrameNum), sizeof(keyFrameNum));
						res.dsAnimBone.pAnim[aIdx].pose[poseIdx].keyFrameNumSZ = keyFrameNum;
						res.dsAnimBone.pAnim[aIdx].pose[poseIdx].keyFrameSZ = new DS_KEY_FRAME[keyFrameNum];
						fs.Read((ds_uint8*)(res.dsAnimBone.pAnim[aIdx].pose[poseIdx].keyFrameSZ), sizeof(DS_KEY_FRAME)*keyFrameNum);
					}
				}

				//マスター移動量
				{
					long keyFrameNum;
					fs.Read((ds_uint8*)(&keyFrameNum), sizeof(keyFrameNum));
					res.dsAnimBone.pAnim[aIdx].masterMove.keyFrameNumTX = keyFrameNum;
					res.dsAnimBone.pAnim[aIdx].masterMove.keyFrameTX = new DS_KEY_FRAME[keyFrameNum];
					fs.Read((ds_uint8*)(res.dsAnimBone.pAnim[aIdx].masterMove.keyFrameTX), sizeof(DS_KEY_FRAME)*keyFrameNum);
				}
				{
					long keyFrameNum;
					fs.Read((ds_uint8*)(&keyFrameNum), sizeof(keyFrameNum));
					res.dsAnimBone.pAnim[aIdx].masterMove.keyFrameNumTY = keyFrameNum;
					res.dsAnimBone.pAnim[aIdx].masterMove.keyFrameTY = new DS_KEY_FRAME[keyFrameNum];
					fs.Read((ds_uint8*)(res.dsAnimBone.pAnim[aIdx].masterMove.keyFrameTY), sizeof(DS_KEY_FRAME)*keyFrameNum);
				}
				{
					long keyFrameNum;
					fs.Read((ds_uint8*)(&keyFrameNum), sizeof(keyFrameNum));
					res.dsAnimBone.pAnim[aIdx].masterMove.keyFrameNumTZ = keyFrameNum;
					res.dsAnimBone.pAnim[aIdx].masterMove.keyFrameTZ = new DS_KEY_FRAME[keyFrameNum];
					fs.Read((ds_uint8*)(res.dsAnimBone.pAnim[aIdx].masterMove.keyFrameTZ), sizeof(DS_KEY_FRAME)*keyFrameNum);
				}

				{
					long keyFrameNum;
					fs.Read((ds_uint8*)(&keyFrameNum), sizeof(keyFrameNum));
					res.dsAnimBone.pAnim[aIdx].masterMove.keyFrameNumRX = keyFrameNum;
					res.dsAnimBone.pAnim[aIdx].masterMove.keyFrameRX = new DS_KEY_FRAME[keyFrameNum];
					fs.Read((ds_uint8*)(res.dsAnimBone.pAnim[aIdx].masterMove.keyFrameRX), sizeof(DS_KEY_FRAME)*keyFrameNum);
				}
				{
					long keyFrameNum;
					fs.Read((ds_uint8*)(&keyFrameNum), sizeof(keyFrameNum));
					res.dsAnimBone.pAnim[aIdx].masterMove.keyFrameNumRY = keyFrameNum;
					res.dsAnimBone.pAnim[aIdx].masterMove.keyFrameRY = new DS_KEY_FRAME[keyFrameNum];
					fs.Read((ds_uint8*)(res.dsAnimBone.pAnim[aIdx].masterMove.keyFrameRY), sizeof(DS_KEY_FRAME)*keyFrameNum);
				}
				{
					long keyFrameNum;
					fs.Read((ds_uint8*)(&keyFrameNum), sizeof(keyFrameNum));
					res.dsAnimBone.pAnim[aIdx].masterMove.keyFrameNumRZ = keyFrameNum;
					res.dsAnimBone.pAnim[aIdx].masterMove.keyFrameRZ = new DS_KEY_FRAME[keyFrameNum];
					fs.Read((ds_uint8*)(res.dsAnimBone.pAnim[aIdx].masterMove.keyFrameRZ), sizeof(DS_KEY_FRAME)*keyFrameNum);
				}
				{
					long keyFrameNum;
					fs.Read((ds_uint8*)(&keyFrameNum), sizeof(keyFrameNum));
					res.dsAnimBone.pAnim[aIdx].masterMove.keyFrameNumRQ = keyFrameNum;
					res.dsAnimBone.pAnim[aIdx].masterMove.keyFrameRQ = new DS_KEY_FRAME[keyFrameNum];
					fs.Read((ds_uint8*)(res.dsAnimBone.pAnim[aIdx].masterMove.keyFrameRQ), sizeof(DS_KEY_FRAME)*keyFrameNum);
				}
			}
		}

		////////////////////ここからカスタムプロパティ
		{//ragdollParam
			unsigned long ragdollParamNum=0;
			fs.Read((ds_uint8*)(&ragdollParamNum), sizeof(ragdollParamNum));
			const long ragParaSize = ragdollParamNum * sizeof(DS_RAGDOLL_PARAM_ID);
			res.dsCustomProperty.ragdollNum = ragdollParamNum;
			if (0 < ragdollParamNum) {
				DS_RAGDOLL_PARAM_ID* pRagdollaramId= new DS_RAGDOLL_PARAM_ID[ragdollParamNum];
				fs.Read((ds_uint8*)(pRagdollaramId), sizeof(DS_RAGDOLL_PARAM_ID)*ragdollParamNum);
				res.dsCustomProperty.pRagdollParamId = pRagdollaramId;
			}

		}


		////////////////////ここからライトデータ
		if(1 <= version)
		{
			unsigned int lightNum = 0;
			fs.Read((ds_uint8*)(&lightNum), sizeof(lightNum));
			if (0 < lightNum) {
				DS_LIGHT* pLight = new DS_LIGHT[lightNum];
				fs.Read((ds_uint8*)pLight, sizeof(DS_LIGHT)*lightNum);
				res.dsLight.lights.assign(pLight, pLight + (lightNum-1));
				delete[] pLight;
			}
		}


		return &res;
	}
}


bool DsAnimRes::Initialize(const char* path, DsResource& resource)
{
	OutputRes* pRes = _LoadAnim(path);
	m_resTop = pRes;

	m_name = path;

	size_t pos1;
	pos1 = m_name.rfind('\\');
	if (pos1 != std::string::npos){
		m_name = m_name.substr(pos1 + 1, m_name.size() - pos1 - 1);
	}

	pos1 = m_name.rfind('/');
	if (pos1 != std::string::npos){
		m_name = m_name.substr(pos1 + 1, m_name.size() - pos1 - 1);
	}

	return true;
}

//子ボーン
void DsAnimRes::_CreateBone(DsAnimBone* pParent, const void* pParentSrcData, std::vector<DsAnimBone*>& boneArray) const
{
	const DS_BONE* pParentSrc = static_cast<const DS_BONE*>(pParentSrcData);
	const OutputRes* pRes = static_cast<OutputRes*>(m_resTop);

	for (int cIdx = 0; cIdx < pParentSrc->childNum; ++cIdx)
	{
		const int tmpIdx = pParentSrc->pChildIdx[cIdx];
		const DS_BONE* tmp = &pRes->dsAnimBone.pBone[tmpIdx];
		
		DsAnimBone* pBone = new DsAnimBone;
		DS_ASSERT(pBone, "メモリ確保失敗");
		pBone->pParent = pParent;
		pBone->name = tmp->name;
		
		//グローバルからローカルを求める
		pBone->initWorldPose = tmp->initMat.m;
		const DsMat44d parentWorldMat = pParent->initWorldPose;
		const DsVec3d localPos = pBone->initWorldPose.GetPos() - parentWorldMat.GetPos();
		const DsMat33d localMat = parentWorldMat.ToMat33().ToTransposition()*pBone->initWorldPose.ToMat33();
		pBone->localPose = DsMat44d::Identity();
		pBone->modelPose = DsMat44d::Identity();
		pBone->initParentToChildPose = DsMat44d::Get(localMat, localPos);
		pBone->arrayIdx = tmpIdx;	//配列でのインデックス保存
		pBone->vIndexNum = tmp->indexNum;
		pBone->pIndex = new int[tmp->indexNum];
		DS_ASSERT(pBone->pIndex, "メモリ確保失敗");
		pBone->pWeight = new double[tmp->indexNum];
		DS_ASSERT(pBone->pWeight, "メモリ確保失敗");
		pBone->isMaster = (0 != pBone->isMaster);
		for (int vIdx = 0; vIdx < tmp->indexNum; ++vIdx)
		{
			pBone->pIndex[vIdx] = tmp->pIndex[vIdx];
			pBone->pWeight[vIdx] = tmp->pWeight[vIdx];
		}
		boneArray[pBone->arrayIdx] = pBone;
		pParent->child.push_back(pBone);
	}
	for (int cIdx = 0; cIdx < pParentSrc->childNum; ++cIdx)
	{
		//DS_BONEは子ボーンへのインデックスを持ってるのでそれを元に生成
		const int tmpIdx = pParentSrc->pChildIdx[cIdx];
		_CreateBone(pParent->child[cIdx], &pRes->dsAnimBone.pBone[tmpIdx], boneArray);
	}
}

DsAnimSkeleton* DsAnimRes::CreateSkeleton() const
{
	OutputRes* pRes = static_cast<OutputRes*>(m_resTop);
	const int bn = pRes->dsAnimBone.bn;
	if (0 >= bn) {
		return NULL;
	}

	std::vector<DsAnimBone*> roots;
	std::vector<DsAnimBone*> boneArray(bn, NULL);

	for (int bIdx = 0; bIdx < bn; ++bIdx)
	{
		if (pRes->dsAnimBone.pBone[bIdx].parentIdx < 0)
		{
			const DS_BONE* rootSrc = &pRes->dsAnimBone.pBone[bIdx];
			DsAnimBone* root = new DsAnimBone;
			DS_ASSERT(root, "メモリ確保失敗");
			root->name = rootSrc->name;
			root->localPose = DsMat44d::Identity();
			root->modelPose = DsMat44d::Identity();
			root->initParentToChildPose = DsMat44d::Identity();
			root->initWorldPose = rootSrc->initMat.m;
			root->arrayIdx = bIdx;
			root->vIndexNum = rootSrc->indexNum;
			root->pIndex = new int[rootSrc->indexNum];
			DS_ASSERT(root->pIndex, "メモリ確保失敗");
			root->pWeight = new double[rootSrc->indexNum];
			DS_ASSERT(root->pWeight, "メモリ確保失敗");
			root->isMaster = (0 != rootSrc->isMaster);
			for (int vIdx = 0; vIdx < rootSrc->indexNum; ++vIdx)
			{
				root->pIndex[vIdx] = rootSrc->pIndex[vIdx];
				root->pWeight[vIdx] = rootSrc->pWeight[vIdx];
			}
			boneArray[root->arrayIdx] = root;
			_CreateBone(root, rootSrc, boneArray);
			roots.push_back(root);
		}
	}

	DsAnimSkeleton* pSkeleton = new DsAnimSkeleton(roots, boneArray);
	DS_ASSERT(pSkeleton, "メモリ確保失敗");
	return pSkeleton;
}

DsKeyframeAnimSet* DsAnimRes::CreateKeyframeAnim() const
{
	OutputRes* pRes = static_cast<OutputRes*>(m_resTop);

	if (0 >= pRes->dsAnimBone.an) return NULL;//アニメがない
	if (0 >= pRes->dsAnimBone.bn) return NULL;//ボーンがない

	DsKeyframeAnim* pKeyAnim = new DsKeyframeAnim[pRes->dsAnimBone.an];
	DS_ASSERT(pKeyAnim, "メモリ確保失敗");
	for (int aIdx = 0; aIdx < pRes->dsAnimBone.an; ++aIdx)
	{
		const DS_ANIM& anim = pRes->dsAnimBone.pAnim[aIdx];
		DS_ASSERT(anim.poseNum == pRes->dsAnimBone.bn, "キーフレームアニメボーン数とスケルトンボーン数が一致しない");

		pKeyAnim[aIdx].m_name = anim.animName;
		pKeyAnim[aIdx].m_pBone = new DsKeyframeAnim::Pose[anim.poseNum];
		DS_ASSERT(pKeyAnim[aIdx].m_pBone, "メモリ確保失敗");
		pKeyAnim[aIdx].m_boneNum = anim.poseNum;
		
		for (int bIdx = 0; bIdx < anim.poseNum; ++bIdx)
		{
			if (anim.pose[bIdx].keyFrameNumRX != anim.pose[bIdx].keyFrameNumRY ||
				anim.pose[bIdx].keyFrameNumRY != anim.pose[bIdx].keyFrameNumRZ ||
				anim.pose[bIdx].keyFrameNumRZ != anim.pose[bIdx].keyFrameNumRQ )
			{
				DS_ASSERT(false, "回転キーフレーム数が成分間で一致しない");
			}
			if (anim.pose[bIdx].keyFrameNumTX != anim.pose[bIdx].keyFrameNumTY ||
				anim.pose[bIdx].keyFrameNumTY != anim.pose[bIdx].keyFrameNumTZ)
			{
				DS_ASSERT(false, "位置キーフレーム数が成分間で一致しない");
			}			
			if (anim.pose[bIdx].keyFrameNumSX != anim.pose[bIdx].keyFrameNumSY ||
				anim.pose[bIdx].keyFrameNumSY != anim.pose[bIdx].keyFrameNumSZ )
			{
				DS_ASSERT(false, "拡大キーフレーム数が成分間で一致しない");
			}
			


			//回転
			pKeyAnim[aIdx].m_pBone[bIdx].m_keyFrameNumRot = anim.pose[bIdx].keyFrameNumRX;
			pKeyAnim[aIdx].m_pBone[bIdx].m_pRot = new DsKeyframeAnim::Vec4Key[anim.pose[bIdx].keyFrameNumRX];
			DS_ASSERT(pKeyAnim[aIdx].m_pBone[bIdx].m_pRot, "メモリ確保失敗");
			for (int k = 0; k < anim.pose[bIdx].keyFrameNumRX; ++k)
			{
				if (anim.pose[bIdx].keyFrameRX[k].localTimeMs != anim.pose[bIdx].keyFrameRY[k].localTimeMs ||
					anim.pose[bIdx].keyFrameRY[k].localTimeMs != anim.pose[bIdx].keyFrameRZ[k].localTimeMs ||
					anim.pose[bIdx].keyFrameRZ[k].localTimeMs != anim.pose[bIdx].keyFrameRQ[k].localTimeMs )
				{
					DS_ASSERT(false, "回転ローカルタイムが成分間で一致しない");
				}
				pKeyAnim[aIdx].m_pBone[bIdx].m_pRot[k].localTime = anim.pose[bIdx].keyFrameRX[k].localTimeMs;
				pKeyAnim[aIdx].m_pBone[bIdx].m_pRot[k].val.x = anim.pose[bIdx].keyFrameRX[k].value;
				pKeyAnim[aIdx].m_pBone[bIdx].m_pRot[k].val.y = anim.pose[bIdx].keyFrameRY[k].value;
				pKeyAnim[aIdx].m_pBone[bIdx].m_pRot[k].val.z = anim.pose[bIdx].keyFrameRZ[k].value;
				pKeyAnim[aIdx].m_pBone[bIdx].m_pRot[k].val.w = anim.pose[bIdx].keyFrameRQ[k].value;
				
			}


			//位置
			pKeyAnim[aIdx].m_pBone[bIdx].m_keyFrameNumPos = anim.pose[bIdx].keyFrameNumTX;
			pKeyAnim[aIdx].m_pBone[bIdx].m_pPos = new DsKeyframeAnim::Vec3Key[anim.pose[bIdx].keyFrameNumTX];
			DS_ASSERT(pKeyAnim[aIdx].m_pBone[bIdx].m_pPos, "メモリ確保失敗");
			for (int k = 0; k < anim.pose[bIdx].keyFrameNumTX; ++k)
			{
				if (anim.pose[bIdx].keyFrameTX[k].localTimeMs != anim.pose[bIdx].keyFrameTY[k].localTimeMs ||
					anim.pose[bIdx].keyFrameTY[k].localTimeMs != anim.pose[bIdx].keyFrameTZ[k].localTimeMs )
				{
					DS_ASSERT(false, "位置ローカルタイムが成分間で一致しない");
				}
				pKeyAnim[aIdx].m_pBone[bIdx].m_pPos[k].localTime = anim.pose[bIdx].keyFrameTX[k].localTimeMs;
				pKeyAnim[aIdx].m_pBone[bIdx].m_pPos[k].val.x = anim.pose[bIdx].keyFrameTX[k].value;
				pKeyAnim[aIdx].m_pBone[bIdx].m_pPos[k].val.y = anim.pose[bIdx].keyFrameTY[k].value;
				pKeyAnim[aIdx].m_pBone[bIdx].m_pPos[k].val.z = anim.pose[bIdx].keyFrameTZ[k].value;

			}


			//拡大
			pKeyAnim[aIdx].m_pBone[bIdx].m_keyFrameNumScale = anim.pose[bIdx].keyFrameNumSX;
			pKeyAnim[aIdx].m_pBone[bIdx].m_pScale = new DsKeyframeAnim::Vec3Key[anim.pose[bIdx].keyFrameNumSX];
			DS_ASSERT(pKeyAnim[aIdx].m_pBone[bIdx].m_pScale, "メモリ確保失敗");
			for (int k = 0; k < anim.pose[bIdx].keyFrameNumSX; ++k)
			{
				if (anim.pose[bIdx].keyFrameSX[k].localTimeMs != anim.pose[bIdx].keyFrameSY[k].localTimeMs ||
					anim.pose[bIdx].keyFrameSY[k].localTimeMs != anim.pose[bIdx].keyFrameSZ[k].localTimeMs)
				{
					DS_ASSERT(false, "拡大ローカルタイムが成分間で一致しない");
				}
				pKeyAnim[aIdx].m_pBone[bIdx].m_pScale[k].localTime = anim.pose[bIdx].keyFrameSX[k].localTimeMs;
				pKeyAnim[aIdx].m_pBone[bIdx].m_pScale[k].val.x = anim.pose[bIdx].keyFrameSX[k].value;
				pKeyAnim[aIdx].m_pBone[bIdx].m_pScale[k].val.y = anim.pose[bIdx].keyFrameSY[k].value;
				pKeyAnim[aIdx].m_pBone[bIdx].m_pScale[k].val.z = anim.pose[bIdx].keyFrameSZ[k].value;

			}
		}

		//マスター移動量
		if (anim.masterMove.keyFrameNumRX != anim.masterMove.keyFrameNumRY ||
			anim.masterMove.keyFrameNumRY != anim.masterMove.keyFrameNumRZ ||
			anim.masterMove.keyFrameNumRZ != anim.masterMove.keyFrameNumRQ)
		{
			DS_ASSERT(false, "回転キーフレーム数が成分間で一致しない");
		}
		if (anim.masterMove.keyFrameNumTX != anim.masterMove.keyFrameNumTY ||
			anim.masterMove.keyFrameNumTY != anim.masterMove.keyFrameNumTZ)
		{
			DS_ASSERT(false, "位置キーフレーム数が成分間で一致しない");
		}
		if (anim.masterMove.keyFrameNumSX != anim.masterMove.keyFrameNumSY ||
			anim.masterMove.keyFrameNumSY != anim.masterMove.keyFrameNumSZ)
		{
			DS_ASSERT(false, "拡大キーフレーム数が成分間で一致しない");
		}

		//回転
		pKeyAnim[aIdx].m_masterMove.m_keyFrameNumRot = anim.masterMove.keyFrameNumRX;
		pKeyAnim[aIdx].m_masterMove.m_pRot = new DsKeyframeAnim::Vec4Key[anim.masterMove.keyFrameNumRX];
		DS_ASSERT(pKeyAnim[aIdx].m_masterMove.m_pRot, "メモリ確保失敗");
		for (int k = 0; k < anim.masterMove.keyFrameNumRX; ++k)
		{
			if (anim.masterMove.keyFrameRX[k].localTimeMs != anim.masterMove.keyFrameRY[k].localTimeMs ||
				anim.masterMove.keyFrameRY[k].localTimeMs != anim.masterMove.keyFrameRZ[k].localTimeMs ||
				anim.masterMove.keyFrameRZ[k].localTimeMs != anim.masterMove.keyFrameRQ[k].localTimeMs)
			{
				DS_ASSERT(false, "回転ローカルタイムが成分間で一致しない");
			}
			pKeyAnim[aIdx].m_masterMove.m_pRot[k].localTime = anim.masterMove.keyFrameRX[k].localTimeMs;
			pKeyAnim[aIdx].m_masterMove.m_pRot[k].val.x = anim.masterMove.keyFrameRX[k].value;
			pKeyAnim[aIdx].m_masterMove.m_pRot[k].val.y = anim.masterMove.keyFrameRY[k].value;
			pKeyAnim[aIdx].m_masterMove.m_pRot[k].val.z = anim.masterMove.keyFrameRZ[k].value;
			pKeyAnim[aIdx].m_masterMove.m_pRot[k].val.w = anim.masterMove.keyFrameRQ[k].value;

		}
		//位置
		pKeyAnim[aIdx].m_masterMove.m_keyFrameNumPos = anim.masterMove.keyFrameNumTX;
		pKeyAnim[aIdx].m_masterMove.m_pPos = new DsKeyframeAnim::Vec3Key[anim.masterMove.keyFrameNumTX];
		DS_ASSERT(pKeyAnim[aIdx].m_masterMove.m_pPos, "メモリ確保失敗");
		for (int k = 0; k < anim.masterMove.keyFrameNumTX; ++k)
		{
			if (anim.masterMove.keyFrameTX[k].localTimeMs != anim.masterMove.keyFrameTY[k].localTimeMs ||
				anim.masterMove.keyFrameTY[k].localTimeMs != anim.masterMove.keyFrameTZ[k].localTimeMs)
			{
				DS_ASSERT(false, "位置ローカルタイムが成分間で一致しない");
			}
			pKeyAnim[aIdx].m_masterMove.m_pPos[k].localTime = anim.masterMove.keyFrameTX[k].localTimeMs;
			pKeyAnim[aIdx].m_masterMove.m_pPos[k].val.x = anim.masterMove.keyFrameTX[k].value;
			pKeyAnim[aIdx].m_masterMove.m_pPos[k].val.y = anim.masterMove.keyFrameTY[k].value;
			pKeyAnim[aIdx].m_masterMove.m_pPos[k].val.z = anim.masterMove.keyFrameTZ[k].value;

		}

		pKeyAnim[aIdx].Initialize();
	}

	DsKeyframeAnimSet* ret = new DsKeyframeAnimSet(pKeyAnim, pRes->dsAnimBone.an);
	return ret;
}

DsModel* DsAnimRes::CreateAnimModel() const
{
	OutputRes* pRes = static_cast<OutputRes*>(m_resTop);
	DsModel* pAnimModel = new DsModel();
	DS_ASSERT(pAnimModel, "メモリ確保失敗");

	//頂点データ
	pAnimModel->m_pVertex = new DsVec4d[pRes->dsAnimModel.vn];
	DS_ASSERT(pAnimModel->m_pVertex, "メモリ確保失敗");
	pAnimModel->m_vn = pRes->dsAnimModel.vn;
	for (unsigned int vIdx = 0; vIdx < pRes->dsAnimModel.vn; ++vIdx)
	{
		const DS_VERTEX& tmp = pRes->dsAnimModel.pVertex[vIdx];
		pAnimModel->m_pVertex[vIdx] = DsVec4d::ToVec4(tmp.x, tmp.y, tmp.z, tmp.w);
	}

	//面
	pAnimModel->m_pFace = new DsModel::Face[pRes->dsAnimModel.fn];
	DS_ASSERT(pAnimModel->m_pFace, "メモリ確保失敗");
	pAnimModel->m_fn = pRes->dsAnimModel.fn;
	for (unsigned int fIdx = 0; fIdx < pRes->dsAnimModel.fn; ++fIdx)
	{
		const DS_FACE& tmp = pRes->dsAnimModel.pFace[fIdx];
		pAnimModel->m_pFace[fIdx].pIndex = new int[tmp.vn];
		DS_ASSERT(pAnimModel->m_pFace[fIdx].pIndex, "メモリ確保失敗");
		pAnimModel->m_pFace[fIdx].vn = tmp.vn;
		for (unsigned int vIdx = 0; vIdx < tmp.vn; ++vIdx)
		{
			pAnimModel->m_pFace[fIdx].pIndex[vIdx] = tmp.pIndex[vIdx];
		}
		pAnimModel->m_pFace[fIdx].normal = DsVec3d::ToVec3(tmp.normal.x, tmp.normal.y, tmp.normal.z);//4vecあるけど使わないので3vec
	}

	//ダミポリ
	if (0 < pRes->dsAnimModel.dn) {
		pAnimModel->m_pDmypoly = new DsModel::Dmypoly[pRes->dsAnimModel.dn];
		DS_ASSERT(pAnimModel->m_pDmypoly, "メモリ確保失敗");
		pAnimModel->m_dn = pRes->dsAnimModel.dn;
		for (unsigned int dIdx = 0; dIdx < pRes->dsAnimModel.dn; ++dIdx) {
			pAnimModel->m_pDmypoly[dIdx].id = pRes->dsAnimModel.pDmypoly[dIdx].id;
			const int dvNum = sizeof(pAnimModel->m_pDmypoly[dIdx].index)/sizeof(pAnimModel->m_pDmypoly[dIdx].index[0]);
			for (int dvIdx = 0; dvIdx < dvNum; ++dvIdx) {
				pAnimModel->m_pDmypoly[dIdx].index[dvIdx] = pRes->dsAnimModel.pDmypoly[dIdx].vIndex[dvIdx];
			}
		}
	}

	//マテリアル
	pAnimModel->m_mn = pRes->dsAnimModel.mn;
	pAnimModel->m_pMaterial = new DsModel::Material[pAnimModel->m_mn];
	DS_ASSERT(pAnimModel->m_pMaterial, "メモリ確保失敗");
	for (int mi = 0; mi < pAnimModel->m_mn; ++mi)
	{
		pAnimModel->m_pMaterial[mi].ambient = DsVec3f::ToVec3( pRes->dsAnimModel.pMtr[mi].ambient);
		pAnimModel->m_pMaterial[mi].diffuse = DsVec3f::ToVec3(pRes->dsAnimModel.pMtr[mi].diffuse);
		pAnimModel->m_pMaterial[mi].emissive = DsVec3f::ToVec3(pRes->dsAnimModel.pMtr[mi].emissive);
		pAnimModel->m_pMaterial[mi].specular = DsVec3f::ToVec3(pRes->dsAnimModel.pMtr[mi].specular);
		pAnimModel->m_pMaterial[mi].shininess = pRes->dsAnimModel.pMtr[mi].shininess;

		DsMaterialParam mtrParam(pRes->dsAnimModel.pMtr[mi].materialParamId);
		pAnimModel->m_pMaterial[mi].isWaveNormal = mtrParam.IsWaveNormal();

		const int tn = pRes->dsAnimModel.pMtr[mi].textureNum;
		pAnimModel->m_pMaterial[mi].textureNum = tn;
		pAnimModel->m_pMaterial[mi].pTexture = new DsModel::Material::Texture[tn];
		DS_ASSERT(pAnimModel->m_pMaterial[mi].pTexture, "メモリ確保失敗");
		for (int ti = 0; ti < tn; ++ti)
		{
			DsModel::Material::Texture& texture = pAnimModel->m_pMaterial[mi].pTexture[ti];
			const int uvn = pRes->dsAnimModel.pMtr[mi].texture[ti].uvNum;
			texture.pathAlbedo = pRes->dsAnimModel.pMtr[mi].texture[ti].albedoTexPath;
			texture.pathNormal = pRes->dsAnimModel.pMtr[mi].texture[ti].normalTexPath;
			texture.pathSpecular = pRes->dsAnimModel.pMtr[mi].texture[ti].specularTexPath;

			texture.uvNum = uvn;
			texture.pUV = new DsModel::Material::Texture::UV[uvn];
			DS_ASSERT(texture.pUV, "メモリ確保失敗");
			for (int uvi = 0; uvi < uvn; ++uvi)
			{
				texture.pUV[uvi].x = pRes->dsAnimModel.pMtr[mi].texture[ti].uv[uvi].uv[0];
				texture.pUV[uvi].y = pRes->dsAnimModel.pMtr[mi].texture[ti].uv[uvi].uv[1];
				texture.pUV[uvi].vertexIdx = pRes->dsAnimModel.pMtr[mi].texture[ti].uv[uvi].vertexIndex;
			}

			const int uvFaceNum = pRes->dsAnimModel.pMtr[mi].texture[ti].uvFaceNum;
			for (int uvfi = 0; uvfi < uvFaceNum; ++uvfi)
			{
				const int refGeomFaceIndex = pRes->dsAnimModel.pMtr[mi].texture[ti].uvFace[uvfi].refGeomFaceIndex;
				DS_ASSERT(refGeomFaceIndex < pAnimModel->m_fn, "UVへの面への参照インデックスがオーバー");
				DsModel::Face* pRefFace = &pAnimModel->m_pFace[refGeomFaceIndex];
				texture.refGeomFaces.push_back(pRefFace);
				texture.refGeomFacesIndex.push_back(refGeomFaceIndex);
			}
		}
	}

	return pAnimModel;
}

int DsAnimRes::GetAnimNum() const
{
	OutputRes* pRes = static_cast<OutputRes*>(m_resTop);
	const int ret = pRes->dsAnimBone.an;
	return ret;
}

DsAnimCustomProperty* DsAnimRes::CustomProperty() const
{
	OutputRes* pRes = static_cast<OutputRes*>(m_resTop);
	DsAnimCustomProperty* ret = NULL;
	const long num = pRes->dsCustomProperty.ragdollNum;
	ret = new DsAnimCustomProperty();
	DS_ASSERT(ret, "メモリ確保失敗");
	if (0 < num) {
		ret->ragdollParamIds.reserve(num);
		for (int i = 0; i < num; ++i) {
			const DS_RAGDOLL_PARAM_ID& id = pRes->dsCustomProperty.pRagdollParamId[i];
			DsAnimRagdollParamId param;
			param.boneIndex = id.boneIdx;
			param.ragdollParamId = id.id;
			ret->ragdollParamIds.push_back(param);
		}
	}
	return ret;
}

void DsAnimRes::DbgDraw(int animIdx, float dt, DsDrawCommand& com)
{

}

DsAnimRes::~DsAnimRes()
{
	OutputRes* pRes = static_cast<OutputRes*>(m_resTop);
	delete pRes;
	m_resTop = NULL;
}