#include "FlverReader.h"

FlverHeader::FlverHeader()
{
}

FlverHeader::FlverHeader(ifstream* flver)
{
	MemReader::ReadArray(flver, this->m_magic, 6);
	MemReader::ReadArray(flver, this->m_endian, 2);
	MemReader::Read(flver, &this->m_version);
	MemReader::Read(flver, &this->m_dataOffset);
	MemReader::Read(flver, &this->m_dataSize);
	MemReader::Read(flver, &this->m_dummyCount);
	MemReader::Read(flver, &this->m_materialCount);
	MemReader::Read(flver, &this->m_boneCount);
	MemReader::Read(flver, &this->m_meshCount);
	MemReader::Read(flver, &this->m_vertexBufferCount);
	MemReader::ReadArray(flver, &this->m_bbMin, 3);
	MemReader::ReadArray(flver, &this->m_bbMax, 3);
	MemReader::Read(flver, &this->m_trueFaceCount);
	MemReader::Read(flver, &this->m_totalFaceCount);
	MemReader::Read(flver, &this->m_vertexIndexSize);
	MemReader::Read(flver, &this->m_unicode);
	MemReader::Read(flver, &this->m_bVar4A);
	MemReader::Read(flver, &this->m_bVar4B);
	MemReader::Read(flver, &this->m_primitiveRestartCount);
	MemReader::Read(flver, &this->m_faceSetCount);
	MemReader::Read(flver, &this->m_bufferLayoutCount);
	MemReader::Read(flver, &this->m_textureCount);
	MemReader::Read(flver, &this->m_bVar5C);
	MemReader::Read(flver, &this->m_bVar5D);
	MemReader::Read(flver, &this->m_bVar5E);
	MemReader::Read(flver, &this->m_bVar5F);
	MemReader::Read(flver, &this->m_iVar60);
	MemReader::Read(flver, &this->m_iVar64);
	MemReader::Read(flver, &this->m_iVar68);
}

FlverHeader::~FlverHeader()
{
}

Dummy::Dummy()
{
}

Dummy::Dummy(ifstream* flver)
{
	MemReader::ReadArray(flver, &this->m_position, 3);
	MemReader::ReadArray(flver, this->m_color, 4);
	MemReader::ReadArray(flver, &this->m_forward, 3);
	MemReader::Read(flver, (WORD*)&this->m_referenceId);
	MemReader::Read(flver, (WORD*)&this->m_dummyBoneIndex);
	MemReader::ReadArray(flver, &this->m_up, 3);
	MemReader::Read(flver, (WORD*)&this->m_attackBoneIndex);
	MemReader::Read(flver, &this->m_bVar2E);
	MemReader::Read(flver, &this->m_useUpVector);
	MemReader::Read(flver, &this->m_iVar30);
	MemReader::Read(flver, &this->m_iVar34);
}

Dummy::~Dummy()
{
}

Bone::Bone()
{
}

Bone::Bone(ifstream* flver)
{
	UINT64 pos_bak;
	MemReader::ReadArray(flver, &this->m_position, 3);

	pos_bak = flver->tellg();
	UINT nameOffset;

	MemReader::Read(flver, &nameOffset);
	wchar_t name[50];

	flver->seekg(nameOffset);
	MemReader::ReadArray(flver, (WORD*)name, 50);

	flver->seekg(pos_bak + 0x4);

	MemReader::ReadArray(flver, &this->m_rotation, 3);
	MemReader::Read(flver, (WORD*)&this->m_parentIdx);
	MemReader::Read(flver, (WORD*)&this->m_childIdx);
	MemReader::ReadArray(flver, &this->m_scale, 3);
	MemReader::Read(flver, (WORD*)&this->m_nextSiblingIndex);
	MemReader::Read(flver, (WORD*)&this->m_previousSiblingIndex);
	MemReader::ReadArray(flver, &this->m_bbMin, 3);
	MemReader::Read(flver, &this->m_pad);
	MemReader::ReadArray(flver, &this->m_bbMax, 3);
}

Bone::~Bone()
{
}

Mesh::Mesh()
{
}

Mesh::Mesh(ifstream* flver)
{

}

Mesh::~Mesh()
{
}

FaceSet::FaceSet()
{
}

FaceSet::FaceSet(ifstream* flver)
{

}

FaceSet::~FaceSet()
{
}

VertexBuffer::VertexBuffer()
{
}

VertexBuffer::VertexBuffer(ifstream* flver)
{

}

VertexBuffer::~VertexBuffer()
{
}

BufferLayout::BufferLayout()
{
}

BufferLayout::~BufferLayout()
{
}

FlverReader::FlverReader()
{
}

FlverReader::FlverReader(PWSTR pszFilePath)
{
	this->m_filePath = pszFilePath;

	ifstream flver;

	flver.open(this->m_filePath, ios::binary | ios::ate);
	this->m_fileSize = flver.tellg();
	flver.close();

	flver.open(this->m_filePath, ios::binary);
}

FlverReader::~FlverReader()
{
}