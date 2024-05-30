#include "../Utils/MemReader/MemReader.h"
#include <SimpleMath.h>

class FlverHeader
{
public:
	char m_magic[4];
	char m_endian[2];
	UINT m_version;
	UINT m_dataOffset;
	UINT m_dataSize;
	UINT m_dummyCount;
	UINT m_materialCount;
	UINT m_boneCount;
	UINT m_meshCount;
	UINT m_vertexBufferCount;
	DirectX::SimpleMath::Vector3 m_bbMin;
	DirectX::SimpleMath::Vector3 m_bbMax;
	UINT m_trueFaceCount;
	UINT m_totalFaceCount;
	BYTE m_vertexIndexSize;
	BYTE m_unicode;
	BYTE m_bVar4A;
	BYTE m_bVar4B;
	UINT m_primitiveRestartCount;
	UINT m_faceSetCount;
	UINT m_bufferLayoutCount;
	UINT m_textureCount;
	BYTE m_bVar5C;
	BYTE m_bVar5D;
	BYTE m_bVar5E;
	BYTE m_bVar5F;
	UINT m_iVar60;
	UINT m_iVar64;
	UINT m_iVar68;

	FlverHeader();
	FlverHeader(ifstream* flver);
	~FlverHeader();
};

class Dummy
{
public:
	DirectX::SimpleMath::Vector3 m_position;
	BYTE m_color[4];
	DirectX::SimpleMath::Vector3 m_forward;
	short m_referenceId;
	short m_dummyBoneIndex;
	DirectX::SimpleMath::Vector3 m_up;
	short m_attackBoneIndex;
	BYTE m_bVar2E;
	bool m_useUpVector;
	INT m_iVar30;
	INT m_iVar34;

	Dummy();
	Dummy(ifstream* flver);
	~Dummy();
};

class Bone
{
public:
	DirectX::SimpleMath::Vector3 m_position;
	std::wstring m_name;
	DirectX::SimpleMath::Vector3 m_rotation;
	short m_parentIdx;
	short m_childIdx;
	DirectX::SimpleMath::Vector3 m_scale;
	short m_nextSiblingIndex;
	short m_previousSiblingIndex;
	DirectX::SimpleMath::Vector3 m_bbMin;
	int m_pad;
	DirectX::SimpleMath::Vector3 m_bbMax;

	Bone();
	Bone(ifstream* flver);
	~Bone();
};

class Mesh
{
public:
	bool m_dynamic;
	int m_materialIdx;
	int m_defaultBoneIdx;
	int m_boneCount;
	UINT m_bbOffset;
	int* m_boneIndices;
	int m_faceSetIndicesCount;
	int* m_faceSetIndices;
	int m_vertexBufferCount;
	int* m_vertexBufferIndices;

	Mesh();
	Mesh(ifstream* flver);
	~Mesh();
};

class FaceSet
{
public:
	UINT m_flags;
	bool m_triangleStrip;
	bool m_cullBackFaces;
	BYTE m_bVar6;
	BYTE m_bVar7;
	int m_vertexIndexCount;
	USHORT* m_vertexIndex;
	int m_vertexIndicesLenght;
	int m_vertexIndexSize;

	FaceSet();
	FaceSet(ifstream* flver);
	~FaceSet();
};

class VertexBuffer
{
public:
	int m_bufferIndex;
	int m_layoutIndex;
	int m_vertexSize;
	int m_vertexCount;
	int m_iVar10;
	int m_iVar14;
	UINT m_verticesLenght;
	DirectX::SimpleMath::Vector3* m_position;

	VertexBuffer();
	VertexBuffer(ifstream* flver);
	~VertexBuffer();
};

class BufferLayout
{
public:
	BufferLayout();
	~BufferLayout();
};

class FlverReader 
{
public:
	PWSTR m_filePath;
	UINT64 m_fileSize;
	PWSTR m_outFilePath;
	UINT64 m_outFileSize;
	bool m_init = false;

	FlverReader();
	FlverReader(PWSTR pszFilePath);
	~FlverReader();
};