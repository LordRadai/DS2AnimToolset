#include "flver2.h"
#include <climits>

namespace cfr
{
	std::vector<FLVER2::BufferLayout> FLVER2::getMeshBufferLayouts(int meshIndex)
	{
		std::vector<FLVER2::BufferLayout> ret = std::vector<FLVER2::BufferLayout>();

		for (int i = 0; i < this->meshes[meshIndex].header.vertexBufferCount; i++)
		{
			int vbi = this->meshes[meshIndex].vertexBufferIndices[i];

			FLVER2::BufferLayout copy = this->bufferLayouts[vbi];

			ret.push_back(copy);
		}

		return ret;
	};

	void printVertexSemantic(VertexSemantic s)
	{
		switch (s)
		{
			case 0:
				printf("position"); break;
			case 1:
				printf("bone_weights"); break;
			case 2:
				printf("bone_indices"); break;
			case 3:
				printf("normal"); break;
			case 4:
				printf("UNKOWN"); break;
			case 5:
				printf("uv"); break;
			case 6:
				printf("tangent"); break;
			case 7:
				printf("bitangent"); break;
			case 10:
				printf("vert_color"); break;
		}
	};

	void printVertexType(VertexType t)
	{
		switch (t)
		{
			case 0x01:
				printf("vec2"); break;
			case 0x02:
				printf("vec3"); break;
			case 0x03:
				printf("vec4"); break;
			case 0x10:
				printf("unk0"); break;
			case 0x11:
				printf("byte4_0"); break;
			case 0x12:
				printf("short2"); break;
			case 0x13:
				printf("byte4_1"); break;
			case 0x15:
				printf("short2_uv"); break;
			case 0x16:
				printf("short4_uv_pair"); break;
			case 0x18:
				printf("short4_bone_indices"); break;
			case 0x1A:
				printf("short4"); break;
			case 0x2E:
				printf("unk1"); break;
			case 0x2F:
				printf("unk2"); break;
			case 0xF0:
				printf("unk3"); break;
		}
	};

	void FLVER2::BufferLayout::print()
	{
		for(int i = 0; i < this->header.memberCount; i++)
		{
			printf("index: %i\t:", this->members[i].index);
			printVertexSemantic((VertexSemantic)this->members[i].semantic);
			printf("\t\t\t\t");
			printVertexType((VertexType)this->members[i].type);
			printf("\t\t");
			printf("offset: %i\n", this->members[i].structOffset);
		}
	};

	char* FLVER2::Mesh::writeVertexData()
	{
		char* a = nullptr;
		return a;
	};

	//does not include degenerate faces
	void FLVER2::Faceset::triangulate()
	{
		std::vector<int> result;

		//its inverted. i don't know why
		if(this->header.triangleStrip == 0)
		{
			if(this->vertInfo.vertexIndexSize == 16)
			{
				for(int i = 0; i < this->header.vertexIndexCount; i++)
				{
					result.push_back((int)this->vertexIndicesShort[i]);
				}
			}
			else if(this->vertInfo.vertexIndexSize == 32)
			{
				//TODO: memcpy the vector, it'll be faster
				for(int i = 0; i < this->header.vertexIndexCount; i++)
				{
					result.push_back((int)this->vertexIndicesInt[i]);
				}
			}

			//return result;
			this->triList = (int*)malloc(this->header.vertexIndexCount * sizeof(int));
			this->triCount = (int)result.size();
			memcpy(&this->triList[0],result.data(),result.size() * sizeof(int));
		}
		else if(this->header.triangleStrip == 1)
		{
			bool flip = false;

			int vertCount = this->header.vertexIndexCount;
			
			for(int i = 0; i < vertCount - 2; i++)
			{
				int v1 = 0;
				int v2 = 0;
				int v3 = 0;

				if(this->vertexSize == 16)
				{
					v1 = (int)this->vertexIndicesShort[i + 0];
					v2 = (int)this->vertexIndicesShort[i + 1];
					v3 = (int)this->vertexIndicesShort[i + 2];
				}
				else if(this->vertexSize == 32)
				{
					v1 = (int)this->vertexIndicesInt[i + 0];
					v2 = (int)this->vertexIndicesInt[i + 1];
					v3 = (int)this->vertexIndicesInt[i + 2];
				}

				if(true && (v1 == 0xFFFF || v2 == 0xFFFF || v3 == 0xFFFF))
				{
					flip = false;
				}
				else
				{
					if ((v1 != v2) && (v2 != v3) && (v3 != v1))
					{
						if(flip)
						{
							result.push_back(v3);
							result.push_back(v2);
							result.push_back(v1);
						}
						else
						{
							result.push_back(v1);
							result.push_back(v2);
							result.push_back(v3);
						}
					}

					flip = !flip;
				}
			}
			//return result;
			this->triList = (int*)malloc(result.size() * sizeof(int));
			this->triCount = result.size();
			memcpy(&this->triList[0],result.data(),result.size() * sizeof(int));
		}
	};

	enum StandardType
	{
		STD_FLOAT,
		STD_SHORT,
		STD_USHORT,
		STD_INT,
		STD_UINT,
		STD_MIX, //for the stupid ones
	};

	/*typedef struct {
		int typeCount = 0;
		int typeSize  = 0;
		StandardType type;
	} SemanticInfo;*/

	//converts string to vertexsemantic
	VertexSemantic semanticStringToEnum(const char* semantic)
	{
		if(strncmp(semantic,"position",8) == 0)
			return VertexSemantic::FVS_POSITION; //vec3
		else if(strncmp(semantic,"bone_weights",12) == 0)
			return VertexSemantic::FVS_BONE_WEIGHTS; //vec4
		else if(strncmp(semantic,"bone_indices",12) == 0)
			return VertexSemantic::FVS_BONE_INDICES; //short4
		else if(strncmp(semantic,"normal",6) == 0)
			return VertexSemantic::FVS_NORMAL; //vec3
		//else if(strncmp(semantic,"normalw",7) == 0)
		//	return VertexSemantic::FVS_NORMAL; //int
		else if(strncmp(semantic,"uvs",3) == 0)
			return VertexSemantic::FVS_UV; //vec3
		else if(strncmp(semantic,"tangent",7) == 0)
			return VertexSemantic::FVS_TANGENT; //vec4
		else if(strncmp(semantic,"bitangent",9) == 0)
			return VertexSemantic::FVS_BITANGENT; //vec4
		else if(strncmp(semantic,"colors",6) == 0)
			return VertexSemantic::FVS_VERTEX_COLOR; //vec4
		else
			throw std::runtime_error("Uknown data type!\n");
	};

	//returns size, count, and type
	/*SemanticInfo getSemanticInfo(VertexSemantic semantic)
	{
		switch(semantic)
		{
			case(FVS_POSITION):
				return {3,4,STD_FLOAT};
			case(FVS_BONE_WEIGHTS):
				return {4,4,STD_FLOAT};
			case(FVS_BONE_INDICES):
				return {4,2,STD_SHORT};
			case(FVS_NORMAL):
				return {4,4,STD_MIX};
			case(FVS_UV):
				return {3,4,STD_FLOAT};
			case(FVS_TANGENT):
				return {4,4,STD_FLOAT};
			case(FVS_BITANGENT):
				return {4,4,STD_FLOAT};
			case(FVS_VERTEX_COLOR):
				return {4,4,STD_FLOAT};
		}
	};*/

	//gets byte size of a given vertexType
	int getVertexTypeSize(VertexType type)
	{
		switch(type)
		{
			case(0x01):
				return 8;
			case(0x02):
				return 12;
			case(0x03):
				return 16;
			case(0x10):
				return 4;
			case(0x11):
				return 4;
			case(0x12):
				return 4;
			case(0x13):
				return 4;
			case(0x15):
				return 4;
			case(0x16):
				return 8;
			case(0x18):
				return 8;
			case(0x1A):
				return 8;
			case(0x2E):
				return 8;
			case(0x2F):
				return 4;
			case(0xF0):
				return -1;
			default:
				return -1;
		}
	};

	void convertUByteNorm(UMEM* src, UMEM* dst)
	{
		uint8_t i;
		uread(&i,1,1,src);
		float f = ((float)i - 127.0f) / 127.0f;
		uwrite((char*)&f,4,1,dst);
	};

	void convertUByteXYZ(UMEM* src, UMEM* dst)
	{
		for(int i = 0; i < 3; i++)
		{
			convertUByteNorm(src,dst);
		}
	};

	void convertSByteZYX(UMEM* src, UMEM* dst)
	{
		float fbuffer[3] = {0.0f,0.0f,0.0f};
		int8_t ibuffer[3] = {0,0,0};
		
		uread(&ibuffer,3,1,src);

		fbuffer[2] = (float)ibuffer[0] / 127.0f;
		fbuffer[1] = (float)ibuffer[1] / 127.0f;
		fbuffer[0] = (float)ibuffer[2] / 127.0f;
			
		uwrite((char*)&fbuffer,12,1,dst);
	};

	void convertShortNorm(UMEM* src, UMEM* dst)
	{
		int16_t i = 0;
		uread(&i,2,1,src);
		float f = (float)i / 32767.0f;
		uwrite((char*)&f,4,1,dst);
	};

	void convertUShortNorm(UMEM* src, UMEM* dst)
	{
		uint16_t i = 0;
		uread(&i,2,1,src);
		float f = ((float)i - 32767) / 32767.0f;
		uwrite((char*)&f,4,1,dst);
	};

	//reads raw vertex data and writes it out in a sane manner, dst2 is for secondary type outputs
	void convertVertexType(VertexType type, VertexSemantic semantic, UMEM* src, UMEM* dst, UMEM* dst2, int uvFactor)
	{
		//SemanticInfo si = getSemanticInfo(semantic);
		int inputSize = getVertexTypeSize(type);

		uint8_t ui8 = 0;
		 int8_t si8 = 0;

		uint16_t ui16 = 0;
		 int16_t si16 = 0;

		uint32_t ui32 = 0;
		 int32_t si32 = 0;

		float f1    =  0.0f;
		float f3[3] = {0.0f,0.0f,0.0f};
		float f4[4] = {0.0f,0.0f,0.0f,0.0f};

		uint8_t c[4] = {0,0,0,0};

		switch(semantic)
		{
			case(FVS_POSITION):
				switch(type)
				{
					case(FVT_FLOAT3):
						//printf("attempt read");
						uread(&f3,12,1,src);
						/*if(dst->mem->size == dst->mem->position)
							printf("END OF DEST\n");*/
						uwrite((char*)&f3,12,1,dst);
						break;
					case(FVT_FLOAT4):
						uread(&f3,12,1,src);
						uwrite((char*)&f3,12,1,dst);
						useek(src,4,SEEK_CUR);
						break;
				}
				break;
			case(FVS_BONE_WEIGHTS):
				switch(type)
				{
					case(FVT_BYTE4A):
						for(int i = 0; i < 4; i++)
						{
							uread(&si8,1,1,src);
							f1 = (float)si8 / 127.0f;
							uwrite((char*)&f1,4,1,dst);
						}
						break;
					case(FVT_BYTE4C):
						for(int i = 0; i < 4; i++)
						{
							uread(&ui8,1,1,src);
							f1 = (float)ui8 / 255.0f;
							uwrite((char*)&f1,4,1,dst);
						}
						break;
					case(FVT_UV_PAIR):
					case(FVT_SHORT4_FLOAT4A):
						for(int i = 0; i < 4; i++)
						{
							uread(&si16,2,1,src);
							f1 = (float)si16 / 32767.0f;
							uwrite((char*)&f1,4,1,dst);
						}
						break;
					default:
						throw std::runtime_error("Unkown bone weight type!\n");
				}
				break;
			case(FVS_BONE_INDICES):
				switch(type)
				{
					case(FVT_BYTE4B):
					case(FVT_BYTE4E):
						for(int i = 0; i < 4; i++)
						{
							uread(&ui8,1,1,src);
							si16 = (short)ui8;
							uwrite((char*)&si16,2,1,dst);
						}
						break;
					case(FVT_SHORT_BONE_INDICES):
						for(int i = 0; i < 4; i++)
						{
							uread(&ui16,2,1,src);
							si16 = (float)ui16;
							uwrite((char*)&si16,2,1,dst);
						}
						break;
					default:
						throw std::runtime_error("Unkown bone index type!\n");
				}
				break;
			case(FVS_NORMAL): //always writes floats then int
				switch(type)
				{
					case(FVT_FLOAT3):
						uread(&f3,12,1,src);
						uwrite((char*)&f3,12,1,dst);
						uwrite((char*)&si32,4,1,dst2);
						break;
					case(FVT_FLOAT4):
						uread(&f3,12,1,src);
						uwrite((char*)&f3,12,1,dst);
						uread(&f1,4,1,src);
						si32 = (int)f1;
						uwrite((char*)&si32,4,1,dst2);
						break;
					case(FVT_BYTE4A):
					case(FVT_BYTE4B):
					case(FVT_BYTE4C):
					case(FVT_BYTE4E):
						convertUByteXYZ(src,dst);
						uread(&ui8,1,1,src);
						si32 = (int)ui8;
						uwrite((char*)&si32,4,1,dst2);
						break;
					case(FVT_SHORT2_FLOAT2):
						uread(&ui8,1,1,src);
						si32 = (int)ui8;
						convertSByteZYX(src,dst);
						uwrite((char*)&si32,4,1,dst2);
						break;
					case(FVT_SHORT4_FLOAT4A):
						for(int i = 0; i < 3; i++)
							convertShortNorm(src,dst);
						uread(&si16,2,1,src);
						si32 = (int)si16;
						uwrite((char*)&si32,4,1,dst2);
						break;
					case(FVT_SHORT4_FLOAT4B):
						for(int i = 0; i < 3; i++)
							convertUShortNorm(src,dst);
						uread(&si16,2,1,src);
						si32 = (int)si16;
						uwrite((char*)&si32,4,1,dst2);
						break;
					default:
						throw std::runtime_error("Unkown normal type!\n");
				}
				break;
			case(FVS_UV):
				switch(type)
				{
					case(FVT_FLOAT2):
						uread(&f3,8,1,src);
						uwrite((char*)&f3,8,1,dst);
						break;
					case(FVT_FLOAT3):
						uread(&f3,12,1,src);
						uwrite((char*)&f3,8,1,dst);
						break;
					case(FVT_FLOAT4):
						uread(&f3,8,1,src);
						uwrite((char*)&f3,8,1,dst);
						uread(&f3,8,1,src);
						uwrite((char*)&f3,8,1,dst);
						break;
					case(FVT_BYTE4A):
					case(FVT_BYTE4B):
					case(FVT_BYTE4C):
					case(FVT_SHORT2_FLOAT2):
					case(FVT_UV):
						uread(&si16,2,1,src);
						f1 = (float)si16 / uvFactor;
						uwrite((char*)&f1,4,1,dst);

						uread(&si16,2,1,src);
						f1 = (float)si16 / uvFactor;
						uwrite((char*)&f1,4,1,dst);

						f1 = 0;
						//uwrite((char*)&f1,4,1,dst);
						break;
					case(FVT_UV_PAIR):
						uread(&si16,2,1,src);
						f1 = (float)si16 / uvFactor;
						uwrite((char*)&f1,4,1,dst);

						uread(&si16,2,1,src);
						f1 = (float)si16 / uvFactor;
						uwrite((char*)&f1,4,1,dst);

						f1 = 0;
						//uwrite((char*)&f1,4,1,dst);

						uread(&si16,2,1,src);
						f1 = (float)si16 / uvFactor;
						uwrite((char*)&f1,4,1,dst);

						uread(&si16,2,1,src);
						f1 = (float)si16 / uvFactor;
						uwrite((char*)&f1,4,1,dst);

						f1 = 0;
						//uwrite((char*)&f1,4,1,dst);
						break;
					case(FVT_SHORT4_FLOAT4B):						
						uread(&si16,2,1,src);
						f1 = (float)si16 / uvFactor;
						uwrite((char*)&f1,4,1,dst);

						uread(&si16,2,1,src);
						f1 = (float)si16 / uvFactor;
						uwrite((char*)&f1,4,1,dst);

						uread(&si16,2,1,src);
						f1 = (float)si16 / uvFactor;
						///uwrite((char*)&f1,4,1,dst);

						useek(src,2,SEEK_CUR);
						break;
					default:
						printf("Unknown UV type! %i\n",type);
						throw std::runtime_error("Unknown UV type!\n");
				}
				break;
			case(FVS_TANGENT):
				switch(type)
				{
					case(FVT_FLOAT4):
						uread(&f4,16,1,src);
						uwrite((char*)&f4,16,1,dst);
						break;
					case(FVT_BYTE4A):
					case(FVT_BYTE4B):
					case(FVT_BYTE4C):
					case(FVT_BYTE4E):
						convertUByteXYZ(src,dst);
						convertUByteNorm(src,dst); //w
						break;
					case(FVT_SHORT4_FLOAT4A):
						convertShortNorm(src,dst);
						convertShortNorm(src,dst);
						convertShortNorm(src,dst);
						convertShortNorm(src,dst);
						break;
					default:
						throw std::runtime_error("Uknown tangent type!\n");
				}
				break;
			case(FVS_BITANGENT):
				switch(type)
				{
					case(FVT_BYTE4A):
					case(FVT_BYTE4B):
					case(FVT_BYTE4C):
					case(FVT_BYTE4E):
						convertUByteXYZ(src,dst);
						convertUByteNorm(src,dst);
						break;
					default:
						throw std::runtime_error("Unknown bitangent type!\n");
				}
				break;
			case(FVS_VERTEX_COLOR):
				switch(type)
				{
					case(FVT_FLOAT4):
						uread(&f4,16,1,src);
						uwrite((char*)&f4,16,1,dst);
						break;
					case(FVT_BYTE4A):
					case(FVT_BYTE4C):
						uread(&c,4,1,src);
						f4[0] = (float)c[0];
						f4[1] = (float)c[1];
						f4[2] = (float)c[2];
						f4[3] = (float)c[3];
						uwrite((char*)&f4,16,1,dst);
						break;
					default:
						printf("Unkown color type!\n");
						throw std::runtime_error("Unknown color type!\n");
				}
				break;
		}
	};

	//for now only "all" is supported
	void FLVER2::getVertexData(int meshIndex, int* uvCountOut, int* colorCountOut, int* tanCountOut)
	{
		std::vector<std::tuple<VertexSemantic,int>> requests;
		
		//printf("Meshindex: %i\n",meshIndex);

		int uvFactor = 1024;
		if(this->header.version >= 0x2000F)
			uvFactor = 2048;

		//printf("got uv factor: %i\n",uvFactor);

		//if(strncmp(type,"all",3) == 0)
		if(true)
		{
			//printf("got all\n");
			std::vector<int> allIds = {0,1,2,3,5,6,7,10};
			for(int i = 0; i < allIds.size(); i++)
				requests.push_back(std::make_tuple((VertexSemantic)allIds[i],0));
		}
		else
		{
			//requests.push_back(std::make_tuple(semanticStringToEnum(type),0));
		}

		int vertCount = 0;

		//printf("vertbuffcount: %i\n",this->meshes[meshIndex].header.vertexBufferCount);
		for(int mvbi = 0; mvbi < this->meshes[meshIndex].header.vertexBufferCount; mvbi++)
		{
			int vbi = this->meshes[meshIndex].vertexBufferIndices[mvbi];
			vertCount += this->vertexBuffers[vbi].header.vertexCount;
		}

		//for(int vbi = 0; vbi < this->vertexBuffers.size(); vbi++) //vertex buffer index
		//	vertCount += this->vertexBuffers[vbi]->header.vertexCount;

		//printf("got vertcount:%i\n",vertCount);

		int tanCount = 0;
		int uvCount = 0;
		int colorCount = 0;

		int loi = this->vertexBuffers[0].header.layoutIndex;
		//printf("member count: %i\n",this->bufferLayouts[loi].header.memberCount);

		//buffers should all have the same structure. TODO: confirm this wild assumption
		for(int bi = 0; bi < this->meshes[meshIndex].header.vertexBufferCount; bi++) //buffer index
		{
			int t_tanCount = 0;
			int t_uvCount = 0;
			int t_colorCount = 0;

			int index = this->meshes[meshIndex].vertexBufferIndices[bi];
			//printf("got index: %i\n",index);
			cfr::FLVER2::VertexBuffer buff = this->vertexBuffers[index];
			loi = buff.header.layoutIndex;
			for(int mi = 0; mi < this->bufferLayouts[loi].header.memberCount; mi++)
			{
				//printf("mi: %i\n",mi);
				VertexSemantic semantic = (VertexSemantic)this->bufferLayouts[loi].members[mi].semantic;
				VertexType type = (VertexType)this->bufferLayouts[loi].members[mi].type;
				//printf("meber s: %i\n",semantic);
				if(semantic == FVS_UV)
				{
					//printf("type: 0x%x\n",type);
					if(type == FVT_UV_PAIR)
						t_uvCount+=2; //two uvs are contained in this one
					else if(type == FVT_FLOAT4)
						t_uvCount+=2; //two uvs are contained in this one
					else
						t_uvCount++;
				}
				else if(semantic == FVS_VERTEX_COLOR)
					t_colorCount++;
				else if(semantic == FVS_TANGENT)
					t_tanCount++;
			}

			uvCount = std::max(t_uvCount,uvCount);
			colorCount = std::max(t_colorCount,colorCount);
			tanCount = std::max(t_tanCount,tanCount);
		}
		
		memcpy(tanCountOut,&tanCount,4);
		memcpy(uvCountOut,&uvCount,4);
		memcpy(colorCountOut,&colorCount,4);
		
		//printf("got colorCount:%i\n",colorCount);
		//printf("got uvCount:%i\n",uvCount);
		//printf("got tanCount:%i\n",tanCount);

		VertexData* vd = (VertexData*)malloc(sizeof(VertexData));

		//uvCount += 5; //this is needed, i don't know why

		int pos_size = vertCount * 3 * sizeof(float);
		int bnw_size = vertCount * 4 * sizeof(float);
		int bni_size = vertCount * 4 * sizeof(short);
		int nrm_size = vertCount * 3 * sizeof(float);
		int nrw_size = vertCount * 1 * sizeof(  int);
		int tan_size = vertCount * 4 * sizeof(float) * tanCount;
		int btn_size = vertCount * 4 * sizeof(float);
		int uvs_size = vertCount * 2 * sizeof(float) * uvCount;
		int clr_size = vertCount * 4 * sizeof(float) * colorCount;

		//uvs_size += 1;

		/*printf("c vert count: %i\n",vertCount);
		printf("c pos size: %i\n",pos_size);
		printf("c color count: %i\n",colorCount);
		printf("c color size: %i\n",clr_size);
		printf("c uv count: %i\n",uvCount);
		printf("c uv size: %i\n",uvs_size);*/

		vd->positions    = (float*)malloc(pos_size);
		vd->bone_weights = (float*)malloc(bnw_size);
		vd->bone_indices = (short*)malloc(bni_size);
		vd->normals      = (float*)malloc(nrm_size);
		vd->normalws     = (int*  )malloc(nrw_size);
		vd->tangents     = (float*)malloc(tan_size);
		vd->bitangents   = (float*)malloc(btn_size);
		vd->uvs          = (float*)malloc(uvs_size);
		vd->colors       = (float*)malloc(clr_size);

		//open up umem handles on the data outputs
		UMEM* dstPositions   = uopenMem((char*)&vd->positions[0],    pos_size);
		UMEM* dstBoneWeights = uopenMem((char*)&vd->bone_weights[0], bnw_size);
		UMEM* dstBoneIndices = uopenMem((char*)&vd->bone_indices[0], bni_size);
		UMEM* dstNormals     = uopenMem((char*)&vd->normals[0],      nrm_size);
		UMEM* dstNormalWs    = uopenMem((char*)&vd->normalws[0],     nrw_size);
		UMEM* dstTangents    = uopenMem((char*)&vd->tangents[0],     tan_size);
		UMEM* dstBitangents  = uopenMem((char*)&vd->bitangents[0],   btn_size);
		UMEM* dstUVs         = uopenMem((char*)&vd->uvs[0],          uvs_size);
		UMEM* dstColors      = uopenMem((char*)&vd->colors[0],       clr_size);

		//this is needed, i don't know why
		useek(dstPositions,0,SEEK_SET);
		useek(dstBoneWeights,0,SEEK_SET);
		useek(dstBoneIndices,0,SEEK_SET);
		useek(dstNormals,0,SEEK_SET);
		useek(dstNormalWs,0,SEEK_SET);
		useek(dstTangents,0,SEEK_SET);
		useek(dstBitangents,0,SEEK_SET);
		useek(dstUVs,0,SEEK_SET);
		useek(dstColors,0,SEEK_SET);

		//for now only "all" is supported, to support individual types, more complex logic will be needed
		if(requests.size() > 2) //check if all are selected
		{
			for(int mvbi = 0; mvbi < this->meshes[meshIndex].header.vertexBufferCount; mvbi++) //vertex buffer index
			{
				int vbi = this->meshes[meshIndex].vertexBufferIndices[mvbi];
				FLVER2::VertexBuffer buff = this->vertexBuffers[vbi];
				//FLVER2::VertexBuffer* buff = this->vertexBuffers[mvbi];

				UMEM* src = buff.data;
				//UMEM* src = uopenMem(buff->verts,buff->header.vertexCount*buff->header.vertexSize);
				useek(src,0,SEEK_SET);

				for(int vi = 0; vi < buff.header.vertexCount; vi++) //vert index
				{
					cfr::FLVER2::BufferLayout bl = this->bufferLayouts[buff.header.layoutIndex];
					for(int mli = 0; mli < bl.header.memberCount; mli++) //member layer index
					{
						LayoutMember* member = &bl.members[mli];

						VertexType vt = (VertexType)member->type;
						VertexSemantic vs = (VertexSemantic)member->semantic;

						if(ueod(src))
						{
							printf("end of memory\n");
							//throw std::runtime_error("END OF MEMORY!\n");
						}

						//printf("semantic: %i\n",member->semantic);
						switch(vs)
						{
							case(FVS_POSITION):
								convertVertexType(vt,vs,src,dstPositions,NULL,uvFactor);
								break;
							case(FVS_BONE_WEIGHTS):
								convertVertexType(vt,vs,src,dstBoneWeights,NULL,uvFactor);
								break;
							case(FVS_BONE_INDICES):
								convertVertexType(vt,vs,src,dstBoneIndices,NULL,uvFactor);
								break;
							case(FVS_NORMAL):
								convertVertexType(vt,vs,src,dstNormals,dstNormalWs,uvFactor);
								break;
							case(FVS_TANGENT):
								convertVertexType(vt,vs,src,dstTangents,NULL,uvFactor);
								break;
							case(FVS_BITANGENT):
								convertVertexType(vt,vs,src,dstBitangents,NULL,uvFactor);
								break;
							case(FVS_UV):
								convertVertexType(vt,vs,src,dstUVs,NULL,uvFactor);
								break;
							case(FVS_VERTEX_COLOR):
								convertVertexType(vt,vs,src,dstColors,NULL,uvFactor);
								break;
							default:
								printf("Unknown semantic: %i\n",vs);
								//throw std::runtime_error("Unknown semantic!\n");
						}
					}
				}
			}
		}

		//FIXUPS for XYZ differences between FROM and Blender!
		// X, Y, Z -> (-X, -Z, Y)
		//first reset the upositions on all umem handles!
		useek(dstPositions,0,SEEK_SET);
		useek(dstBoneWeights,0,SEEK_SET);
		useek(dstBoneIndices,0,SEEK_SET);
		useek(dstNormals,0,SEEK_SET);
		useek(dstNormalWs,0,SEEK_SET);
		useek(dstTangents,0,SEEK_SET);
		useek(dstBitangents,0,SEEK_SET);
		useek(dstUVs,0,SEEK_SET);
		useek(dstColors,0,SEEK_SET);

		float f3_0[3] = {0.0,0.0,0.0};
		float f3_1[3] = {0.0,0.0,0.0};

		//fixes the positions
		for(int i = 0; i < vertCount; i++)
		{
			uread(&f3_0[0],12,1,dstPositions);

			f3_1[0] = 0 - f3_0[0]; //X -> -X
			f3_1[1] = 0 - f3_0[2]; //Y -> -Z
			f3_1[2] =     f3_0[1]; //Z ->  Y

			useek(dstPositions,-12,SEEK_CUR);
			uwrite((char*)&f3_1[0],12,1,dstPositions);
		}

		//fixes the normals
		for(int i = 0; i < vertCount; i++)
		{
			uread(&f3_0[0],12,1,dstNormals);

			f3_1[0] = 0 - f3_0[0]; //X -> -X
			f3_1[1] = 0 - f3_0[2]; //Y -> -Z
			f3_1[2] =     f3_0[1]; //Z ->  Y

			useek(dstNormals,-12,SEEK_CUR);
			uwrite((char*)&f3_1[0],12,1,dstNormals);
		}

		useek(dstPositions,0,SEEK_SET);
		useek(dstBoneWeights,0,SEEK_SET);
		useek(dstBoneIndices,0,SEEK_SET);
		useek(dstNormals,0,SEEK_SET);
		useek(dstNormalWs,0,SEEK_SET);
		useek(dstTangents,0,SEEK_SET);
		useek(dstBitangents,0,SEEK_SET);
		useek(dstUVs,0,SEEK_SET);
		useek(dstColors,0,SEEK_SET);

		//can't close these, breaks python/numpy
		/*uclose(dstPositions);
		uclose(dstBoneWeights);
		uclose(dstBoneIndices);
		uclose(dstNormals);
		uclose(dstNormalWs);
		uclose(dstUVs);
		uclose(dstTangents);
		uclose(dstBitangents);
		uclose(dstColors);*/

		if ((vd->positions[0] != vd->positions[1]) && (vd->positions[1] != vd->positions[2]) && (vd->positions[0] != vd->positions[2]))
			this->meshes[meshIndex].vertexData = vd;
	};

	void FLVER2::getVertexDataOrdered(int meshIndex, int uvCount, int colorCount, int tanCount, int vertCount)
	{
		//this->meshes[meshIndex].vertexData;

		/*vertCount = 0;

		for(int mvbi = 0; mvbi < this->meshes[meshIndex].header.vertexBufferCount; mvbi++)
		{
			int vbi = this->meshes[meshIndex].vertexBufferIndices[mvbi];

			vertCount += this->vertexBuffers[vbi].header.vertexCount;
		}

		uint64_t lowest_flags = LLONG_MAX;
		int fsi = 0;

		for(int mfsi = 0; mfsi < this->meshes[meshIndex].header.facesetCount; mfsi++)
		{
			int fsindex = this->meshes[meshIndex].facesetIndices[mfsi];
			if(this->facesets[fsindex].header.flags < lowest_flags)
			{
				fsi = fsindex;
				lowest_flags = this->facesets[fsi].header.flags;
			}
		}

		int triCount = this->facesets[fsi].triCount;
		//printf("internal tri count: %i\n",triCount);

		int opos_size = triCount * 3 * sizeof(float);
		int obnw_size = triCount * 4 * sizeof(float);
		int obni_size = triCount * 4 * sizeof(short);
		int onrm_size = triCount * 3 * sizeof(float);
		int onrw_size = triCount * 1 * sizeof(  int);
		int otan_size = triCount * 4 * sizeof(float) * tanCount;
		int obtn_size = triCount * 4 * sizeof(float);
		int ouvs_size = triCount * 2 * sizeof(float) * uvCount;
		int oclr_size = triCount * 4 * sizeof(float) * colorCount;

		//printf("opos size %i\n",opos_size);

		//ordered vertex data
		cfr::VertexData* ovd = (cfr::VertexData*)malloc(sizeof(cfr::VertexData));
		ovd->positions    = (char*)malloc(opos_size);
		ovd->bone_weights = (char*)malloc(obnw_size);
		ovd->bone_indices = (char*)malloc(obni_size);
		ovd->normals      = (char*)malloc(onrm_size);
		ovd->normalws     = (char*)malloc(onrw_size);
		ovd->tangents     = (char*)malloc(otan_size);
		ovd->bitangents   = (char*)malloc(obtn_size);
		ovd->uvs          = (char*)malloc(ouvs_size);
		ovd->colors       = (char*)malloc(oclr_size);

		UMEM* dstPositions   = uopenMem(&ovd->positions[0],    opos_size);
		UMEM* dstBoneWeights = uopenMem(&ovd->bone_weights[0], obnw_size);
		UMEM* dstBoneIndices = uopenMem(&ovd->bone_indices[0], obni_size);
		UMEM* dstNormals     = uopenMem(&ovd->normals[0],      onrm_size);
		UMEM* dstNormalWs    = uopenMem(&ovd->normalws[0],     onrw_size);
		UMEM* dstTangents    = uopenMem(&ovd->tangents[0],     otan_size);
		UMEM* dstBitangents  = uopenMem(&ovd->bitangents[0],   obtn_size);
		UMEM* dstUVs         = uopenMem(&ovd->uvs[0],          ouvs_size);
		UMEM* dstColors      = uopenMem(&ovd->colors[0],       oclr_size);

		int pos_size = vertCount * 3 * sizeof(float);
		int bnw_size = vertCount * 4 * sizeof(float);
		int bni_size = vertCount * 4 * sizeof(short);
		int nrm_size = vertCount * 3 * sizeof(float);
		int nrw_size = vertCount * 1 * sizeof(  int);
		int tan_size = vertCount * 4 * sizeof(float) * tanCount;
		int btn_size = vertCount * 4 * sizeof(float);
		int uvs_size = vertCount * 2 * sizeof(float) * uvCount;
		int clr_size = vertCount * 4 * sizeof(float) * colorCount;

		//printf("pos size %i\n",pos_size);

		//open up umem handles on the data outputs
		UMEM* srcPositions   = uopenMem(&this->meshes[meshIndex].vertexData->positions[0],    pos_size);
		UMEM* srcBoneWeights = uopenMem(&this->meshes[meshIndex].vertexData->bone_weights[0], bnw_size);
		UMEM* srcBoneIndices = uopenMem(&this->meshes[meshIndex].vertexData->bone_indices[0], bni_size);
		UMEM* srcNormals     = uopenMem(&this->meshes[meshIndex].vertexData->normals[0],      nrm_size);
		UMEM* srcNormalWs    = uopenMem(&this->meshes[meshIndex].vertexData->normalws[0],     nrw_size);
		UMEM* srcTangents    = uopenMem(&this->meshes[meshIndex].vertexData->tangents[0],     tan_size);
		UMEM* srcBitangents  = uopenMem(&this->meshes[meshIndex].vertexData->bitangents[0],   btn_size);
		UMEM* srcUVs         = uopenMem(&this->meshes[meshIndex].vertexData->uvs[0],          uvs_size);
		UMEM* srcColors      = uopenMem(&this->meshes[meshIndex].vertexData->colors[0],       clr_size);

		//printf("srcpos pos %i\n",srcPositions->mem->position);

		//printf("vertcountinternal: %i\n",vertCount);

		//an absolutely massive buffer
		float b[32]; //128 bytes should be enough?
		//TODO: malloc this with highest sized element

		//rewrite the vertex data to be in the same order as faceset wants
		for(int ti = 0; ti < triCount; ti++)
		{
			useek(srcPositions,12*this->facesets[fsi].triList[ti],SEEK_SET);
			uread(&b[0],12,1,srcPositions);
			uwrite((char*)&b[0],12,1,dstPositions);

			useek(srcBoneWeights,16*this->facesets[fsi].triList[ti],SEEK_SET);
			uread(&b[0],16,1,srcBoneWeights);
			uwrite((char*)&b[0],16,1,dstBoneWeights);

			useek(srcBoneIndices,8*this->facesets[fsi].triList[ti],SEEK_SET);
			uread(&b[0],8,1,srcBoneIndices);
			uwrite((char*)&b[0],8,1,dstBoneIndices);

			useek(srcNormals,12*this->facesets[fsi].triList[ti],SEEK_SET);
			uread(&b[0],12,1,srcNormals);
			uwrite((char*)&b[0],12,1,dstNormals);

			useek(srcNormalWs,4*this->facesets[fsi].triList[ti],SEEK_SET);
			uread(&b[0],4,1,srcNormalWs);
			uwrite((char*)&b[0],4,1,dstNormalWs);

			if(tanCount > 0)
			{
				useek(srcTangents,16*this->facesets[fsi].triList[ti]*tanCount,SEEK_SET);
				uread(&b[0],16,tanCount,srcTangents);
				uwrite((char*)&b[0],16,tanCount,dstTangents);
			}

			useek(srcBitangents,16*this->facesets[fsi].triList[ti],SEEK_SET);
			uread(&b[0],16,1,srcBitangents);
			uwrite((char*)&b[0],16,1,dstBitangents);

			if(uvCount > 0)
			{
				useek(srcUVs,8*this->facesets[fsi].triList[ti]*uvCount,SEEK_SET);
				uread(&b[0],8,uvCount,srcUVs);
				uwrite((char*)&b[0],8,uvCount,dstUVs);
			}

			if(colorCount > 0)
			{
				useek(srcColors,16*this->facesets[fsi].triList[ti]*colorCount,SEEK_SET);
				uread(&b[0],16,colorCount,srcColors);
				uwrite((char*)&b[0],16,colorCount,dstColors);
			}
		}

		uclose(srcPositions);
		uclose(srcBoneWeights);
		uclose(srcBoneIndices);
		uclose(srcNormals);
		uclose(srcNormalWs);
		uclose(srcUVs);
		uclose(srcTangents);
		uclose(srcBitangents);
		uclose(srcColors);

		this->meshes[meshIndex].vertexDataOrdered = ovd;
	};

	void vertMap(float* array, int size, int count, int* faceset, int triCount)
	{
		std::map<int,std::tuple<float,float,float>> vertMap;

		for(int tri = 0; tri < triCount; tri++)
		{
			std::vector<std::tuple<float,float,float>> verts;

			for(int i = 0; i < 3; i++)
			{
				int indexKey = faceset[tri+i];
				printf("indexKey:%i\n",indexKey);
				std::tuple<float,float,float> vert;

				if(vertMap.count(indexKey) == 0)
				{
					float f1 = array[indexKey*3+0];
					printf("f1:%f\n",f1);
					float f2 = array[indexKey*3+1];
					float f3 = array[indexKey*3+2];
					vert = std::make_tuple(f1,f2,f3);

					vertMap[indexKey] = vert;
				}
				else
				{
					printf("uhh\n");
					vert = vertMap.at(indexKey);
				}

				verts.push_back(vert);
			}

			for(int i = 0; i < verts.size(); i++)
			{
				printf("1: %8.2f\t",std::get<0>(verts[i]));
				printf("2: %8.2f\t",std::get<1>(verts[i]));
				printf("3: %8.2f\n",std::get<2>(verts[i]));
			}
		}*/
	};
};