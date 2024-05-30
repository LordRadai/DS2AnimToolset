#include "flver2.h"

//this method isn't that friendly to performance, but its reasonably clean.

namespace cfr
{
	FLVER2::Header::Header(){};

	FLVER2::Header::Header(UMEM* src)
	{
		uread(this,sizeof(FLVER2::Header),1,src);

#ifdef VALIDATE_ALL
		if(strncmp(this->magic,"FLVER\0",6) != 0)
			throw std::runtime_error("Failed to validate FLVER2!\n");
#endif
	};

	FLVER2::Dummy::Dummy(UMEM* src)
	{
		uread(this,sizeof(FLVER2::Dummy),1,src);

#ifdef VALIDATE_ALL
		assertMsg(this->unk38,"FLVER2::Dummy.unk38 is not 0!\n");
		assertMsg(this->unk3C,"FLVER2::Dummy.unk38 is not 0!\n");
#endif
	};

	FLVER2::Material::Material(UMEM* src, FLVER2* parent)
	{
		uread(&this->header,sizeof(Header),1,src);
		this->gxItems = (GxItem*)malloc(sizeof(GxItem));

		this->name = getUniversalString(this->header.nameOffset,parent->header.unicode,src,&this->nameLength);

		this->mtdName = getUniversalString(this->header.mtdOffset,parent->header.unicode,src,&this->mtdNameLength);

		if(this->header.gxOffset == 0)
			gxIndex = -1;

		if(this->header.gxOffset != 0)
		{
			int gxCount = 0;
			long startpos = utell(src); //come back after enumeration
			useek(src,this->header.gxOffset,SEEK_CUR);
			
			//dry run to get count
			if(parent->header.version <= 0x20010)
			{
				//read one 
				int gxCount = 1;
			}
			else
			{
				//read until terminator value found
				int value = 0;
				while(true)
				{
					uread(&value,sizeof(int32_t),1,src);
					if(value != INT_MAX && value != -1)
					{
						//read it off into the void
						GxItem(src,parent);
						gxCount++;
					}
					else
					{
						break;
					}
				}
			}

			this->gxItemCount = gxCount;
			this->gxItems = (GxItem*)malloc(sizeof(GxItem) * gxCount);

			useek(src,this->header.gxOffset, SEEK_SET);
			for(int i = 0; i < gxCount; i++)
				this->gxItems[i] = GxItem(src,parent);

			int32_t id; //assert int maxvalue
			uread(&id,sizeof(int32_t),1,src);
			int32_t unk100; //assert 100
			uread(&unk100,sizeof(int32_t),1,src);
			int32_t termLen;
			uread(&termLen,sizeof(int32_t),1,src);
			char* terminator = (char*)malloc(termLen - 0xC);
			uread(&terminator[0],termLen - 0xC,1,src);
			//assert all chars in terminator are 0x00
			
			useek(src,startpos,SEEK_SET);
		}
	};

	FLVER2::GxItem::GxItem(UMEM* src, FLVER2* parent)
	{
		//depending on parent header version, this might be a string?
		uread(&this->id,sizeof(int32_t),1,src);
		
		uread(&this->unk04,sizeof(int32_t),1,src);
		uread(&this->length,sizeof(int32_t),1,src);

		if(this->length > 12)
			uread(&this->data[0],this->length-12,1,src);
	};

	FLVER2::Bone::Bone(UMEM* src, FLVER2* parent)
	{
		uread(&this->translation,sizeof(cfr_vec3),1,src);
		uread(&this->nameOffset,sizeof(uint32_t),1,src);

		bool unicode = false;
		if(parent->header.unicode == 1)
			unicode = true;

		this->name = getUniversalString(this->nameOffset,unicode,src,&this->nameLength);

		uread(&this->rot,sizeof(cfr_vec3),1,src);
		
		uread(&this->parentIndex,sizeof(int16_t),1,src);
		uread(&this->childIndex,sizeof(int16_t),1,src);

		uread(&this->scale,sizeof(cfr_vec3),1,src);

		uread(&this->nextSiblingIndex,sizeof(int16_t),1,src);
		uread(&this->previousSiblingIndex,sizeof(int16_t),1,src);
		
		uread(&this->boundingBoxMin,sizeof(cfr_vec3),1,src);

		uread(&this->unk3C,sizeof(int32_t),1,src);

		uread(&this->boundingBoxMax,sizeof(cfr_vec3),1,src);

		useek(src,52,SEEK_CUR); //skip empty junk padding
	};

	FLVER2::Mesh::Mesh(UMEM* src, FLVER2* parent)
	{
		uread(&this->header,sizeof(Header),1,src);

#ifdef VALIDATE_ALL
		assertMsg(this->Header.unk01,"FLVER2::Mesh.Header.unk01 is not 0!\n");
		assertMsg(this->Header.unk02,"FLVER2::Mesh.Header.unk02 is not 0!\n");
		assertMsg(this->Header.unk03,"FLVER2::Mesh.Header.unk03 is not 0!\n");

		assertMsg(this->Header.unk08,"FLVER2::Mesh.Header.unk08 is not 0!\n");
		assertMsg(this->Header.unk0C,"FLVER2::Mesh.Header.unk0C is not 0!\n");
#endif

		long pos = utell(src);

		if(this->header.boundingBoxOffset != 0)
		{
			useek(src,this->header.boundingBoxOffset,SEEK_SET);
			uread(&this->boundingBoxMin,sizeof(cfr_vec3),2,src);

			if(parent->header.version >= 0x2001A)
				uread(&this->boundingBoxUnk,sizeof(cfr_vec3),1,src);
		}

		if(this->header.boneCount > 0)
		{
			useek(src,this->header.boneIndicesOffset,SEEK_SET);
			this->boneIndices = (int32_t*)malloc(sizeof(int32_t) * this->header.boneCount);
			uread(&this->boneIndices[0],sizeof(int32_t),this->header.boneCount,src);
		}

		useek(src,this->header.facesetIndicesOffset,SEEK_SET);
		this->facesetIndices = (int32_t*)malloc(sizeof(int32_t) * this->header.facesetCount);
		uread(&facesetIndices[0],sizeof(int32_t),this->header.facesetCount,src);

		useek(src,this->header.vertexBufferIndicesOffset,SEEK_SET);
		this->vertexBufferIndices = (int32_t*)malloc(sizeof(int32_t) * this->header.vertexBufferCount);
		uread(&vertexBufferIndices[0],sizeof(int32_t),this->header.vertexBufferCount,src);

		this->parent = parent;

		useek(src,pos,SEEK_SET);
	};

	FLVER2::Member::Member(UMEM* src, long startOffset)
	{
		uread(&this->header,sizeof(Header),1,src);

#ifdef VALIDATE_ALL
		assertMsg(this->Header.unk08,"FLVER2::Member.Header.unk08 is not 0!\n");
		assertMsg(this->Header.unk0C,"FLVER2::Member.Header.unk0C is not 0!\n");

		assertMsg(this->Header.unk20,"FLVER2::Member.Header.unk20 is not 0!\n");
		assertMsg(this->Header.unk24,"FLVER2::Member.Header.unk24 is not 0!\n");
		assertMsg(this->Header.unk28,"FLVER2::Member.Header.unk28 is not 0!\n");
		assertMsg(this->Header.unk2C,"FLVER2::Member.Header.unk2C is not 0!\n");

		assertMsg(this->Header.unk3C == -1,"FLVER2::Member.Header.unk3C is not -1!\n");
#endif

		long pos = utell(src);

		useek(src,startOffset+this->header.dataOffset,SEEK_SET);

		char* tmp = (char*)malloc(sizeof(this->header.dataLength));
		uread(tmp,this->header.dataLength,1,src);
		this->data = uopenMem(tmp,this->header.dataLength);

		useek(src,pos,SEEK_SET);
	};

	FLVER2::EdgeIndices::EdgeIndices(UMEM* src)
	{
		long pos = utell(src);

		uread(&this->header,sizeof(Header),1,src);

#ifdef VALIDATE_ALL
		assertMsg(this->Header.unk08,"FLVER2::EdgeIndices.Header.unk08 is not 0!\n");
#endif

		for(int i = this->header.memberCount; i > 0; i--)
		{
			this->members.push_back(new Member(src,pos));
		}
	};

	FLVER2::Faceset::Faceset(UMEM* src, FLVER2* parent)
	{
		uread(&this->header,sizeof(Faceset::Header),1,src);

		int indexSize = 0;
		if(parent->header.version >= 0x20005)
			uread(&this->vertInfo,sizeof(vertInfo),1,src);

		if(this->vertInfo.vertexIndexSize == 0)
			this->vertInfo.vertexIndexSize = parent->header.vertexIndexSize;
		
		this->vertexSize = this->vertInfo.vertexIndexSize;

		/*if(parent->header.vertexIndexSize == 0)
			this->vertexSize = this->vertInfo.vertexIndexSize;
		else
			this->vertexSize = parent->header.vertexIndexSize;*/
		
		long pos = utell(src);
		useek(src,parent->header.dataOffset + this->header.vertexIndicesOffset,SEEK_SET);

		if(this->vertInfo.vertexIndexSize == 8)
		{
			printf("Edge indices are not supported!\n");
			throw std::runtime_error("FLVER2::Faceset.VertexSize not implemented (8)!\n");
		}
		else if(this->vertInfo.vertexIndexSize == 16)
		{
			this->vertexIndicesShort = (uint16_t*)malloc(sizeof(uint16_t) * this->header.vertexIndexCount);
			uread(&this->vertexIndicesShort[0],sizeof(uint16_t),this->header.vertexIndexCount,src);
		}
		else if(this->vertexSize == 32)
		{
			//TODO: validate this
			this->vertexIndicesInt = (int32_t*)malloc(sizeof(int32_t) * this->header.vertexIndexCount);
			uread(&this->vertexIndicesInt[0],sizeof(int32_t),this->header.vertexIndexCount,src);
		}

		useek(src,pos,SEEK_SET);
	};

	FLVER2::VertexBuffer::VertexBuffer(UMEM* src, FLVER2* parent)
	{
		uread(&this->header,sizeof(VertexBuffer::Header),1,src);
		long dataSize = this->header.vertexCount*this->header.vertexSize;

#ifdef VALIDATE_ALL
		assertMsg(this->Header.unk10,"FLVER2::EdgeIndices.Header.unk10 is not 0!\n");
		assertMsg(this->Header.unk14,"FLVER2::EdgeIndices.Header.unk14 is not 0!\n");
#endif

		if(this->header.unk10 == 0 && this->header.unk14 == 0)
		{
			long pos = utell(src);

			this->verts = (char*)malloc(dataSize);
			useek(src,this->header.bufferOffset + parent->header.dataOffset,SEEK_SET);
			uread(this->verts,dataSize,1,src);
			this->data = uopenMem(this->verts,dataSize);

			useek(src,pos,SEEK_SET);
		}
		else
		{
			printf("Failed to verify unk data!\n");
			throw std::runtime_error("Failed to verify unk data!\n");
		}
	};

	FLVER2::LayoutMember::LayoutMember(UMEM* src)
	{
		uread(this,sizeof(LayoutMember),1,src);

#ifdef VALIDATE_ALL
		//assertMsg(this->header.unk04,"FLVER2::LayoutMember.unk00 not from 0 to 2!\n");
#endif
	};

	FLVER2::BufferLayout::BufferLayout(UMEM* src)
	{
		uread(&this->header,sizeof(Header),1,src);

#ifdef VALIDATE_ALL
		assertMsg(this->header.unk04,"FLVER2::BufferLayout.header.unk04 not 0!\n");
		assertMsg(this->header.unk08,"FLVER2::BufferLayout.header.unk04 not 0!\n");
#endif

		long pos = utell(src);
		useek(src,this->header.membersOffset,SEEK_SET);

		this->members = (FLVER2::LayoutMember*)malloc(sizeof(FLVER2::LayoutMember) * this->header.memberCount);

		for(int i = 0; i < this->header.memberCount; i++)
		{
			this->members[i] = LayoutMember(src);
		}

		useek(src,pos,SEEK_SET);
	};

	FLVER2::Texture::Texture(UMEM* src, FLVER2* parent)
	{
		uread(&this->pathOffset,sizeof(uint32_t),1,src);
		this->path = getUniversalString(this->pathOffset,parent->header.unicode,src,&this->pathLength);

		uread(&this->typeOffset,sizeof(uint32_t),1,src);
		this->type = getUniversalString(this->typeOffset,parent->header.unicode,src,&this->typeLength);

		uread(&this->scale,sizeof(cfr_vec2),1,src);

		uread(&this->unk10,sizeof(char),1,src);
		uread(&this->unk11,sizeof(char),1,src);
		uread(&this->unk12,sizeof(short),1,src);
		uread(&this->unk14,sizeof(float),1,src);
		uread(&this->unk18,sizeof(float),1,src);
		uread(&this->unk1C,sizeof(float),1,src);

#ifdef VALIDATE_ALL
		assertMsg(this->unk12,"FLVER2::Texture.unk12 not 0!\n");
#endif
	};

	//TODO: VALIDATE THESE
	FLVER2::VertexBoneWeights::VertexBoneWeights(UMEM* src)
	{
		uread(this,sizeof(VertexBoneWeights),1,src);

#ifdef VALIDATE_ALL
		assertMsg((this->length == 4),"FLVER2::VertexBoneWeights.length not 4!\n");
#endif
	};

	FLVER2::VertexBoneIndices::VertexBoneIndices(UMEM* src)
	{
		uread(this,sizeof(VertexBoneIndices),1,src);

#ifdef VALIDATE_ALL
		assertMsg((this->length == 4),"FLVER2::VertexBoneIndices.length not 4!\n");
#endif
	};

	void initFLVER2(FLVER2* f, UMEM* src);

	FLVER2::FLVER2(const char* path) : File(path)
	{
		UMEM* src;
		if (src = uopenFile(path,"rb"))
		{
			initFLVER2(this,src);
		}
		else
		{
			throw std::runtime_error("Failed to open file!");
		}
	};

	FLVER2::FLVER2(UMEM* src) : File(src)
	{
		useek(src,0,SEEK_SET);
		initFLVER2(this,src);
	};

	void initFLVER2(FLVER2* f, UMEM* src)
	{
		f->data = src;
		f->header = FLVER2::Header(f->data);

		//printf("bone count: %i\n",f->header.boneCount);
		//printf("faceset count: %i\n",f->header.facesetCount);
		//printf("mesh count: %i\n",f->header.meshCount);
		//printf("vert buff count: %i\n",f->header.vertexBufferCount);

		f->dummies = (FLVER2::Dummy*)malloc(sizeof(FLVER2::Dummy) * f->header.dummyCount);
		f->materials = (FLVER2::Material*)malloc(sizeof(FLVER2::Material) * f->header.materialCount);
		f->bones = (FLVER2::Bone*)malloc(sizeof(FLVER2::Bone) * f->header.boneCount);
		f->meshes = (FLVER2::Mesh*)malloc(sizeof(FLVER2::Mesh) * f->header.meshCount);
		f->facesets = (FLVER2::Faceset*)malloc(sizeof(FLVER2::Faceset) * f->header.facesetCount);
		f->vertexBuffers = (FLVER2::VertexBuffer*)malloc(sizeof(FLVER2::VertexBuffer) * f->header.vertexBufferCount);
		f->bufferLayouts = (FLVER2::BufferLayout*)malloc(sizeof(FLVER2::BufferLayout) * f->header.bufferLayoutCount);
		f->textures = (FLVER2::Texture*)malloc(sizeof(FLVER2::Texture) * f->header.textureCount);

		for(int i = 0; i < f->header.dummyCount; i++)
			f->dummies[i] = FLVER2::Dummy(f->data);
		
		//printf("dummy0: %f\n",f->dummies[0].position.x);

		for(int i = 0; i < f->header.materialCount; i++)
			f->materials[i] = FLVER2::Material(f->data,f);

		//printf("mat %i name: %LS\n",0,f->materials[0].name);
		//f->materials[0].print();

		for(int i = 0; i < f->header.boneCount; i++)
			f->bones[i] = FLVER2::Bone(f->data,f);
	
		//f->bones[22].print();
		
		for(int i = 0; i < f->header.meshCount; i++)
			f->meshes[i] = FLVER2::Mesh(f->data,f);

		//printf("mesh0: %i\n",f->meshes[0].header.facesetCount);

		for(int i = 0; i < f->header.facesetCount; i++)
			f->facesets[i] = FLVER2::Faceset(f->data,f);

		//printf("faceset0: %i\n",f->facesets[0].header.vertexIndexCount);
		
		for(int i = 0; i < f->header.vertexBufferCount; i++)
			f->vertexBuffers[i] = FLVER2::VertexBuffer(f->data,f);

		//printf("vertexbuffer0: %i\n",f->vertexBuffers[0].header.vertexCount);
		
		for(int i = 0; i < f->header.bufferLayoutCount; i++)
			f->bufferLayouts[i] = FLVER2::BufferLayout(f->data);
		
		for(int i = 0; i < f->header.textureCount; i++)
			f->textures[i] = FLVER2::Texture(f->data,f);

		//f->textures[0].print();
		//f->textures[1].print();

		useek(f->data,0,SEEK_SET);
	};

	void FLVER2::Bone::print()
	{
		//setlocale(LC_ALL, "");
		printf("name: %LS\n",this->name);
		printf("\ttranslation: %8.2f %8.2f %8.2f\n",this->translation.x,this->translation.y,this->translation.z);
		printf("\trotation:    %8.2f %8.2f %8.2f\n",this->rot.x,this->rot.y,this->rot.z);
		printf("\tscale:       %8.2f %8.2f %8.2f\n",this->scale.x,this->scale.y,this->scale.z);
	};

	void FLVER2::Material::print()
	{
		printf("name: %LS\n",this->name);
		printf("mtd name: %LS\n",this->mtdName);
		printf("\titem count: %i\n",this->gxItemCount);
	};

	void FLVER2::Texture::print()
	{
		printf("path: %LS\n",this->path);
		printf("type: %LS\n",this->type);
	};
};