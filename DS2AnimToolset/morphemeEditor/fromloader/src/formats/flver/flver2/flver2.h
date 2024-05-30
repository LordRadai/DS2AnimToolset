#pragma once

#include <stdexcept>
#include <map>

#include "../../stdafx.h"
//welcome to FLVER-town

//standard fromsoft model format from DSPTDE onwards
//as usual, built using SoulsTemplates! :)
//to help shorten this for now, it won't support non souls games

namespace cfr {
	struct VertexData 
	{
		float* positions    ;
		float* bone_weights ;
		short* bone_indices ;
		float* normals      ;
		int*   normalws     ;
		float* uvs          ;
		float* tangents     ;
		float* bitangents   ;
		float* colors       ;
	};

	enum VertexSemantic
	{
		//FLVER2_VERTEX_SEMANTIC
		FVS_POSITION     =  0,
		FVS_BONE_WEIGHTS =  1,
		FVS_BONE_INDICES =  2,
		FVS_NORMAL       =  3,
		FVS_UV           =  5,
		FVS_TANGENT      =  6,
		FVS_BITANGENT    =  7,
		FVS_VERTEX_COLOR = 10,
	};

	/*enum VertexType
	{
		FVT_VEC2               = 0x01, //two floats
		FVT_VEC3               = 0x02, //three floats
		FVT_VEC4               = 0x03, //four floats
		FVT_UNK0               = 0x10, //unknown
		FVT_4_BYTE_0           = 0x11, //four bytes
		FVT_SHORT2             = 0x12, //two shorts
		FVT_4_BYTE_1           = 0x13, //four bytes
		FVT_UV                 = 0x15, //two shorts
		FVT_UV_PAIR            = 0x16, //four shorts
		FVT_SHORT_BONE_INDICES = 0x18, //four shorts
		FVT_4_SHORT_0          = 0x1A, //four shorts
		FVT_UNK1               = 0x2E, //unknown
		FVT_UNK2               = 0x2F, //unknown
		FVT_UNK3               = 0xF0, //unknown
	};*/

	/*enum VertexType
	{
		FVT_VEC2               = 0x01, //two floats
		FVT_VEC3               = 0x02, //three floats
		FVT_VEC4               = 0x03, //four floats
		FVT_4_SBYTE            = 0x10, //(four sbytes),(three sbytes div by 127, one ubyte),(two sshorts)
		FVT_4_UBYTE            = 0x11, //four ubytes
		FVT_NORMAL             = 0x12, //ubyte, three sbytes each div by 127, converted to floats
		FVT_4_BYTE_1           = 0x13, //four bytes
		FVT_UV                 = 0x15, //two shorts
		FVT_UV_PAIR            = 0x16, //four shorts
		FVT_SHORT_BONE_INDICES = 0x18, //four shorts
		FVT_4_SHORT_0          = 0x1A, //four shorts
		FVT_UNK1               = 0x2E, //unknown
		FVT_UNK2               = 0x2F, //unknown
		FVT_UNK3               = 0xF0, //unknown
	};*/

	enum VertexType
	{
		FVT_FLOAT2				= 0x01, //two floats
		FVT_FLOAT3				= 0x02, //three floats
		FVT_FLOAT4				= 0x03, //four floats
		FVT_BYTE4A				= 0x10, //(four sbytes),(three sbytes div by 127, one ubyte),(two sshorts)
		FVT_BYTE4B				= 0x11, //four ubytes
		FVT_SHORT2_FLOAT2		= 0x12, //ubyte, three sbytes each div by 127, converted to floats
		FVT_BYTE4C				= 0x13, //four bytes
		FVT_UV					= 0x15, //two shorts
		FVT_UV_PAIR				= 0x16, //four shorts
		FVT_SHORT_BONE_INDICES	= 0x18, //four shorts
		FVT_SHORT4_FLOAT4A		= 0x1A, //four shorts
		FVT_SHORT4_FLOAT4B		= 0x2E, //unknown
		FVT_BYTE4E				= 0x2F, //unknown
		FVT_EDGE				= 0xF0, //unknown
	};
		
	class FLVER2 : public File
	{
		public:
		//forward declare everything
		class Header;
		class Dummy;
		class GxItem;
		class Material;
		class Bone;
		class Mesh;
		class Member;
		class EdgeIndices;
		class Faceset;
		class VertexBuffer;
		class LayoutMember;
		class BufferLayout;
		class Texture;
		class VertexBoneWeights;
		class VertexBoneIndices;
		//class VertexColor;
		struct VertexColor;

		class Header
		{
			public:
			char magic[6]; //this was working with size 4 somehow. it shouldn't
			char endian[2];
			uint32_t version = 0;

			uint32_t dataOffset = 0;
			uint32_t dataLength = 0;

			int32_t dummyCount = 0;
			int32_t materialCount = 0;
			int32_t boneCount = 0;
			int32_t meshCount = 0;
			int32_t vertexBufferCount = 0;

			cfr_vec3 boundingBoxMin = {0.0f,0.0f,0.0f};
			cfr_vec3 boundingBoxMax = {0.0f,0.0f,0.0f};

			int32_t trueFaceCount  = 0; //not incl shadow meshes or degen faces
			int32_t totalFaceCount = 0;

			int8_t vertexIndexSize = 0;
			int8_t unicode = 0;

			int8_t unk4A = 0;
			int8_t unk4B = 0;

			int32_t primitiveRestartConstant = 0; //guess, needs confirmation
			int32_t facesetCount = 0;
			int32_t bufferLayoutCount = 0;
			int32_t textureCount = 0;

			int8_t unk5C = 0;
			int8_t unk5D = 0;
			int8_t unk5E = 0; //assert(0)
			int8_t unk5F = 0; //assert(0)

			int32_t unk60 = 0; //assert(0)
			int32_t unk64 = 0; //assert(0)
			int32_t unk68 = 0;
			int32_t unk6C = 0; //assert(0)
			int32_t unk70 = 0; //assert(0)
			int32_t unk74 = 0; //assert(0)
			int32_t unk78 = 0; //assert(0)
			int32_t unk7C = 0; //assert(0)

			Header();
			Header(UMEM* src);
		};

		class Dummy
		{
			public:
			cfr_vec3 position;

			uint8_t color[4];

			cfr_vec3 forward = {0.0f,0.0f,0.0f};

			int16_t referenceID;
			int16_t dummyBoneIndex;

			cfr_vec3 upward = {0.0f,0.0f,0.0f};

			int16_t attachBoneIndex;
			char unk2E;
			char useUpwardVector;

			int32_t unk30;
			int32_t unk34;
			int32_t unk38; //assert(0)
			int32_t unk3C; //assert(0)

			Dummy(UMEM* src);
		};

		class GxItem //length INCLUDES this struct!
		{
			public:
			int32_t id;
			int32_t unk04; //maybe assert(100)? if header.version < 0x20010
			int32_t length;

			char* data;

			GxItem(UMEM* src, FLVER2* parent);
		};

		class Material //unclear on how safe this is '_>', specifically gxOffset
		{
			public:
			struct Header
			{
				uint32_t nameOffset = 0;
				uint32_t mtdOffset = 0;

				int32_t textureCount = 0;
				int32_t textureIndex = 0;

				uint32_t flags = 0;
				uint32_t gxOffset = 0;

				int32_t unk18 = 0;
				int32_t unk1C = 0; //assert(0)
			};

			Header header;
			wchar_t* name = nullptr;
			int nameLength = 0;

			wchar_t* mtdName = nullptr;
			int mtdNameLength = 0;

			int gxIndex;
			//std::vector<GxItem*> gxItems;
			GxItem* gxItems;
			int gxItemCount;
			Material(UMEM* src, FLVER2* parent);
			void print();
		};

		class Bone
		{
			public:
			cfr_vec3 translation = {0.0f,0.0f,0.0f};

			uint32_t nameOffset = 0;
			int32_t nameLength = 0;
			wchar_t* name = nullptr;

			cfr_vec3 rot = {0.0f,0.0f,0.0f};

			int16_t parentIndex = 0;
			int16_t childIndex = 0;

			cfr_vec3 scale = {0.0f,0.0f,0.0f};

			int16_t nextSiblingIndex = 0;
			int16_t previousSiblingIndex = 0;

			cfr_vec3 boundingBoxMin = {0.0f,0.0f,0.0f};
			
			int32_t unk3C = 0;

			cfr_vec3 boundingBoxMax = {0.0f,0.0f,0.0f};
			
			// 13 * 4 bytes of space
			//char emptyJunk[52]; //potentially needed for spacing :/

			Bone(UMEM* src, FLVER2* parent);
			
			void print();
		};

		class Mesh
		{
			public:
			struct Header
			{
				int8_t dynamic; // 0 or 1
		
				int8_t unk01; //assert(0)
				int8_t unk02; //assert(0)
				int8_t unk03; //assert(0)
				
				int32_t materialIndex;
				
				int32_t unk08; //assert(0)
				int32_t unk0C; //assert(0)
				
				int32_t defaultBoneIndex;
				int32_t boneCount;

				uint32_t boundingBoxOffset = 0;
				uint32_t boneIndicesOffset = 0;
				
				int32_t facesetCount;
				uint32_t facesetIndicesOffset;

				int32_t vertexBufferCount;
				uint32_t vertexBufferIndicesOffset; //loc of the vertbuffind array
			};
			
			Mesh::Header header;

			//only if boundingBoxOffset != 0
			cfr_vec3 boundingBoxMin = {0.0f,0.0f,0.0f};
			cfr_vec3 boundingBoxMax = {0.0f,0.0f,0.0f};

			//only if header.version >= 0x2001A
			cfr_vec3 boundingBoxUnk = {0.0f,0.0f,0.0f};

			int32_t* boneIndices; //size of boneCount
			int32_t* facesetIndices; //size of faceSetCount
			Faceset* facesets; //pointers to the actual facesets
			int32_t* vertexBufferIndices; //size of vertexBufferCount
			FLVER2* parent;
			VertexData* vertexData = nullptr;
			VertexData* vertexDataOrdered = nullptr;

			Mesh(UMEM* src, FLVER2* parent);

			//ret binary vert data in specd format vec of ints to select type and order
			char* writeVertexData();

			//std::vector<float> getVertPositions();
			std::vector<float> getNormals();
			
		};

		struct Member
		{
			struct Header
			{
				int32_t dataLength;
				int32_t dataOffset;

				int32_t unk08; //assert(0)
				int32_t unk0C; //assert(0)
				int16_t unk10;
				int16_t unk12;

				int16_t baseIndex;

				int16_t unk16;
				int32_t unk18;
				int32_t unk1C;
				int32_t unk20; //assert(0)
				int32_t unk24; //assert(0)
				int32_t unk28; //assert(0)
				int32_t unk2C; //assert(0)

				//edgeGeomSpuConfigInfo? not sure what this means
				int16_t unk30;
				int16_t unk32;
				int16_t unk34;
				int16_t unk36;
				int16_t unk38;

				int16_t indexCount;

				int32_t unk3C; //assert(-1)
			};

			Header header;
			UMEM* data; //size of dataLength

			Member(UMEM* src, long startOffset);
		};

		class EdgeIndices
		{
			public:
			struct Header
			{
				int16_t memberCount = 0;
				int16_t unk02 = 0;
				int32_t unk04 = 0;
				int32_t unk08 = 0; //assert(0)
				int32_t unk0C = 0;
			};

			Header header;
			std::vector<Member*> members; //size of memberCount

			EdgeIndices(UMEM* src);
		};

		class Faceset
		{
			public:
			struct Header
			{
				uint32_t flags = 0;
				uint8_t triangleStrip = 0;
				uint8_t cullBackFaces = 0;
				/*char unk06;
				char unk07;*/
				short unk06 = 0;
				int32_t vertexIndexCount = 0; //number of indices
				uint32_t vertexIndicesOffset = 0;
			};

			//only if header.version >= 0x20009
			struct VertInfo
			{
				int32_t vertexIndicesLength = 0; //i don't know
				int32_t unk14 = 0; //assert(0)
				int32_t vertexIndexSize = 0; //byte size of indices
				int32_t unk1C = 0; //assert(0)
			};
			
			Faceset::Header header;
			VertInfo vertInfo;
			EdgeIndices* vertexIndicesEdge = nullptr;
			uint16_t* vertexIndicesShort = nullptr; //vertexindexcount
			int32_t* vertexIndicesInt = nullptr; //vertexindexcount
			int* triList = nullptr;
			long triCount = 0;
			int vertexSize = 0; //more accurate

			Faceset(UMEM* src, FLVER2* parent);

			//will add to module to avoid copying data
			//std::vector<int> getTriStrip();

			void triangulate();
		};

		class LayoutMember
		{
			public:
			int32_t unk00 = 0; //0, 1, or 2
			int32_t structOffset = 0; //seems to be used in PTDE?
			uint32_t type = 0;
			uint32_t semantic = 0;
			int32_t index = 0; //this doesn't seem to be used?

			LayoutMember(UMEM* src);
		};

		class BufferLayout
		{
			public:
			struct Header
			{
				int32_t memberCount = 0;

				int32_t unk04 = 0; //assert(0)
				int32_t unk08 = 0; //assert(0)

				uint32_t membersOffset = 0; //file location for the members
			};

			BufferLayout::Header header;
			//std::vector<LayoutMember*> members; //size of memberCount
			LayoutMember* members = nullptr;

			BufferLayout(UMEM* src);

			//prints human readable laytout info
			void print();
		};

		class VertexBuffer
		{
			public:
			struct Header
			{
				int32_t bufferIndex = 0;
				int32_t layoutIndex = 0;
				int32_t vertexSize  = 0;
				int32_t vertexCount = 0;

				int32_t unk10 = 0; //assert(0)
				int32_t unk14 = 0; //assert(0)

				uint32_t verticesLength = 0; //0 in version 20005, non 0 in 20008
				int32_t bufferOffset   = 0; //location in flver file
			};
			
			VertexBuffer::Header header;
			char* verts = nullptr; //actual data that data hooks to
			UMEM* data = nullptr; //vertexCount * vertexSize

			VertexBuffer(UMEM* src, FLVER2* parent);
		};

		class Texture
		{
			public:
			uint32_t pathOffset;
			uint32_t typeOffset;

			cfr_vec2 scale;

			char unk10;
			char unk11;
			int16_t unk12; //assert(0)

			float unk14;
			float unk18;
			float unk1C;

			wchar_t* path;
			int pathLength;

			wchar_t* type;
			int typeLength;

			Texture(UMEM* src, FLVER2* parent);

			void print();
		};

		class VertexBoneWeights
		{
			public:
			float a,b,c,d;
			int32_t length; //always 4

			VertexBoneWeights(UMEM* src);
		};

		class VertexBoneIndices
		{
			public:
			int32_t a,b,c,d;
			int32_t length; //always 4

			VertexBoneIndices(UMEM* src);
		};

		struct VertexColor
		{
			float a,r,g,b;
		};

		Header header;
		Dummy* dummies;
		Material* materials;
		Bone* bones;
		Mesh* meshes;
		Faceset* facesets;
		VertexBuffer* vertexBuffers;
		BufferLayout* bufferLayouts;
		Texture* textures;

		FLVER2(const char* path);
		FLVER2(UMEM* src);

		//returns buffer layout for specific mesh
		std::vector<FLVER2::BufferLayout> getMeshBufferLayouts(int meshIndex);
		void getVertexData(int meshIndex, int* uvCount, int* colorCount, int* tanCount);
		void getVertexDataOrdered(int meshIndex, int uvCount, int colorCount, int tanCount, int vertCount);
	};

	// :(
	void convertVertexType(VertexType type, VertexSemantic semantic, UMEM* src, UMEM* dst, UMEM* dst2, int uvFactor);

	//in helper functions, but also here so gltf can use it too
	void convertVertexType(VertexType t, VertexSemantic s, UMEM* src, UMEM* dst, UMEM* dst2, int uvFactor);

	void exportGLTF(FLVER2* flver);

	void vertMap(float* array, int size, int count, int* faceset, int triCount);
};