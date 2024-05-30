#include <filesystem>
#include <assert.h>
#include "TimeActReader.h"
#include "../../framework.h"
#include "../../extern.h"

AnimData::AnimData()
{
	this->m_reference = 0;
	this->m_bVar18 = 0;
	this->m_fps = 30;
	this->m_bVar1A = 0;
	this->m_bVar1B = 0;
	this->m_lenght = 0;
	this->m_pad[0] = 0;
	this->m_pad[1] = 0;
}

AnimData::AnimData(ifstream* tae)
{
	MemReader::Read(tae, &this->m_reference);
	MemReader::Read(tae, &this->m_nextOffsetOffset);
	MemReader::Read(tae, &this->m_nextOffset);
	MemReader::Read(tae, &this->m_bVar18);
	MemReader::Read(tae, &this->m_fps);
	MemReader::Read(tae, &this->m_bVar1A);
	MemReader::Read(tae, &this->m_bVar1B);
	MemReader::Read(tae, &this->m_lenght);
	MemReader::ReadArray<UINT64>(tae, this->m_pad, 2);
}

AnimData::~AnimData()
{
	this->m_reference = 0;
	this->m_bVar18 = 0;
	this->m_fps = 30;
	this->m_bVar1A = 0;
	this->m_bVar1B = 0;
	this->m_lenght = 0;
	this->m_pad[0] = 0;
	this->m_pad[1] = 0;
}

void AnimData::GenerateBinary(ofstream* tae)
{
	MemReader::Write(tae, this->m_reference);
	MemReader::Write(tae, this->m_nextOffsetOffset);
	MemReader::Write(tae, this->m_nextOffset);
	MemReader::Write(tae, this->m_bVar18);
	MemReader::Write(tae, this->m_fps);
	MemReader::Write(tae, this->m_bVar1A);
	MemReader::Write(tae, this->m_bVar1B);
	MemReader::Write(tae, this->m_lenght);
	MemReader::WriteArray<UINT64>(tae, this->m_pad, 2);
}

TimeActData::TimeActData() 
{
	this->m_eventCount = 0;
	this->m_eventGroupCount = 0;
	this->m_timeCount = 0;

	this->m_animData = new AnimData;
}

TimeActData::TimeActData(int lenght)
{
	this->m_eventCount = 0;
	this->m_eventGroupCount = 0;
	this->m_timeCount = 0;

	this->m_animData = new AnimData;
	this->m_animData->m_lenght = lenght;
}

TimeActData::TimeActData(ifstream* tae)
{
	MemReader::Read(tae, &this->m_eventOffset);
	MemReader::Read(tae, &this->m_eventGroupOffset);
	MemReader::Read(tae, &this->m_timesOffset);
	MemReader::Read(tae, &this->m_animDataOffset);

	MemReader::Read(tae, &this->m_eventCount);
	MemReader::Read(tae, &this->m_eventGroupCount);
	MemReader::Read(tae, &this->m_timeCount);

	if (m_eventOffset)
	{
		tae->seekg(m_eventOffset);

		this->m_events.reserve(this->m_eventCount);

		for (size_t i = 0; i < this->m_eventCount; i++)
			this->m_events.push_back(TimeActEvent(tae));
	}

	if (m_eventGroupOffset)
	{
		streampos bak = tae->tellg();

		tae->seekg(m_eventGroupOffset);

		this->m_groups.reserve(this->m_eventGroupCount);

		for (size_t i = 0; i < this->m_eventGroupCount; i++)
		{
			this->m_groups.push_back(EventGroup(tae, this->m_eventOffset));

			this->m_groups[i].m_event.reserve(this->m_groups[i].m_count);

			for (size_t j = 0; j < this->m_groups[i].m_count; j++)
			{
				UINT64 bak = tae->tellg();

				this->m_groups[i].m_event.push_back(&this->m_events[this->m_groups[i].m_eventIndex[j]]);

				tae->seekg(this->m_groups[i].m_event.back()->m_eventData->m_argsOffset);
				this->m_groups[i].m_event.back()->m_eventData->m_args->GetData(tae, this->m_groups[i].m_groupData->m_eventType, this->m_groups[i].m_event.back()->m_eventData->m_id);

				tae->seekg(bak);
			}
		}
	}

	if (m_animDataOffset)
	{
		streampos bak = tae->tellg();

		tae->seekg(m_animDataOffset);

		this->m_animData = new AnimData(tae);
	}
}

TimeActData::~TimeActData()
{
}

void TimeActData::GenerateBinary(ofstream* tae)
{
	MemReader::Write(tae, this->m_eventOffset);
	MemReader::Write(tae, this->m_eventGroupOffset);
	MemReader::Write(tae, this->m_timesOffset);
	MemReader::Write(tae, this->m_animDataOffset);
	MemReader::Write(tae, this->m_eventCount);
	MemReader::Write(tae, this->m_eventGroupCount);
	MemReader::Write(tae, this->m_timeCount);

	MemReader::Pad(tae, 0, 4);

	UINT64 bak = tae->tellp();

	tae->seekp(m_animDataOffset);
	this->m_animData->GenerateBinary(tae);

	tae->seekp(m_timesOffset);

	for (int i = 0; i < this->m_eventCount; i++)
	{
		MemReader::Write(tae, this->m_events[i].m_start);
		MemReader::Write(tae, this->m_events[i].m_end);
	}

	tae->seekp(m_eventOffset);

	for (int i = 0; i < this->m_eventCount; i++)
		this->m_events[i].GenerateBinary(tae);

	tae->seekp(m_eventGroupOffset);

	for (int i = 0; i < this->m_eventGroupCount; i++)
		this->m_groups[i].GenerateBinary(tae);

	tae->seekp(bak);
}

UINT64 TimeActData::GetEventGroupPtr(int index)
{
	return this->m_eventOffset + index * 0x18;
}

TimeAct::TimeAct() {}

TimeAct::TimeAct(int id, float lenght)
{
	this->m_id = id;

	int frameCount = lenght * 30;
	this->m_taeData = new TimeActData(frameCount);
}

TimeAct::TimeAct(ifstream* tae)
{
	MemReader::Read(tae, &this->m_id);
	MemReader::Read(tae, &this->m_taeOffset);

	streampos bak = tae->tellg();
	tae->seekg(this->m_taeOffset);
	this->m_taeData = new TimeActData(tae);
	tae->seekg(bak);
}

TimeAct::~TimeAct()
{
}

void TimeAct::GenerateBinary(ofstream* tae)
{
	MemReader::Write(tae, this->m_id);
	MemReader::Write(tae, this->m_taeOffset);

	UINT64 bak = tae->tellp();

	tae->seekp(m_taeOffset);
	this->m_taeData->GenerateBinary(tae);

	tae->seekp(bak);
}

float TimeAct::CalculatePlaybackPosFromMorphemeEventTrack(float eventStart, float eventDuration, float eventPlaybackPos)
{
	if (this == nullptr)
		return 0.f;

	if (eventPlaybackPos < eventStart)
		return 0.f;

	float taeDuration = (float)this->m_taeData->m_animData->m_lenght / (float)this->m_taeData->m_animData->m_fps;
	float playSpeed = taeDuration / eventDuration;

	return playSpeed * (eventPlaybackPos - eventStart);
}

TimeActGroup::TimeActGroup() {}

TimeActGroup::TimeActGroup(int startTae, int endTae)
{
	this->m_taeStart = startTae;
	this->m_taeEnd = endTae;
}

TimeActGroup::TimeActGroup(ifstream* tae) {}

TimeActLookupTable::TimeActLookupTable() {}

TimeActLookupTable::TimeActLookupTable(ifstream* tae) {}

void TimeActLookupTable::AddGroup(int tae_id)
{
	bool found = false;

	if (this->m_groups.size() == 0)
	{
		this->m_groupCount++;
		this->m_groups.push_back(TimeActGroup(tae_id, tae_id));

		return;
	}

	//Look at the last inserted TAE
	if ((tae_id - this->m_groups.back().m_taeEnd) == 1)
	{
		this->m_groups.back().m_taeEnd = tae_id;
		found = true;
	}

	//If it's not the last, go look at the previous TAE groups created. This should never get executed, TAE list should be ordered by increasing TAE ID
	if (!found)
	{
		for (size_t i = 0; i < this->m_groupCount; i++)
		{
			if ((tae_id - this->m_groups[i].m_taeEnd) == 1)
			{
				this->m_groups[i].m_taeEnd = tae_id;
				found = true;
				break;
			}
		}

		if (!found)
		{
			this->m_groupCount++;
			this->m_groups.push_back(TimeActGroup(tae_id, tae_id));
		}
	}
}

void TimeActLookupTable::GenerateBinary(ofstream* tae)
{
	MemReader::Write(tae, this->m_groupCount);

	MemReader::Pad(tae, 0, 4);

	UINT64 offset = tae->tellp();
	offset += 0x8;

	MemReader::Write(tae, offset);

	for (int i = 0; i < this->m_groupCount; i++)
	{
		MemReader::Write(tae, this->m_groups[i].m_taeStart);
		MemReader::Write(tae, this->m_groups[i].m_taeEnd);
		MemReader::Write(tae, this->m_groups[i].m_offset);
	}
}

Header::Header() {}

Header::Header(ifstream* tae)
{
	MemReader::ReadArray(tae, this->m_magic, 4);
	MemReader::Read(tae, &this->m_bigEndian);
	MemReader::Read(tae, &this->m_bVar5);
	MemReader::Read(tae, &this->m_bVar6);
	MemReader::Read(tae, &this->m_is64Bit); assert(this->m_is64Bit == 255 || this->m_is64Bit == 0);

	if (this->m_is64Bit == 0)
		throw("32 bit TAE not supported yet\n");

	MemReader::Read(tae, &this->m_version);
	MemReader::Read(tae, &this->m_fileSize);
	MemReader::Read(tae, &this->m_flagsOffset);
	MemReader::Read(tae, &this->m_isReadable);
	MemReader::Read(tae, &this->m_fileDataOffset);
	MemReader::Read(tae, &this->m_unkFileDataOffset);
	MemReader::Read(tae, &this->m_iVar30);
	MemReader::Read(tae, &this->m_iVar38);
	MemReader::ReadArray(tae, this->m_flags, 16);
	MemReader::Read(tae, &this->m_fileID);
	MemReader::Read(tae, &this->m_taeCount);
	MemReader::Read(tae, &this->m_taeOffset);
	MemReader::Read(tae, &this->m_taeLookupTableOffset);
	MemReader::Read(tae, &this->m_iVar68);
	MemReader::Read(tae, &this->m_taeCount2);
	MemReader::Read(tae, &this->m_taeDataOffset);
	MemReader::Read(tae, &this->m_iVar80);
	MemReader::Read(tae, &this->m_pVar88);
	MemReader::Read(tae, &this->m_fileId2);
	MemReader::Read(tae, &this->m_fileId3);
	MemReader::Read(tae, &this->m_pVar98);
	MemReader::Read(tae, &this->m_pVarA0);
	MemReader::Read(tae, &this->m_pVarA8);
	MemReader::Read(tae, &this->m_skeletonNameOffset);
	MemReader::Read(tae, &this->m_sibNameOffset);

	if (this->m_sibNameOffset != this->m_taeOffset)
		MemReader::ReadArray(tae, this->m_sibName, this->m_taeOffset - this->m_sibNameOffset);
}

void Header::GenerateBinary(ofstream* tae)
{
	BYTE magic[4] = {0x54, 0x41, 0x45, 0x20};
	MemReader::WriteArray(tae, magic, 4);
	MemReader::Write(tae, this->m_bigEndian);
	MemReader::Write(tae, this->m_bVar5);
	MemReader::Write(tae, this->m_bVar6);
	MemReader::Write(tae, this->m_is64Bit);
	MemReader::Write(tae, this->m_version);

	MemReader::Write(tae, this->m_fileSize);

	MemReader::Write(tae, this->m_flagsOffset);
	MemReader::Write(tae, this->m_isReadable);
	MemReader::Write(tae, this->m_fileDataOffset);
	MemReader::Write(tae, this->m_unkFileDataOffset);
	MemReader::Write(tae, this->m_iVar30);
	MemReader::Write(tae, this->m_iVar38);

	MemReader::WriteArray(tae, this->m_flags, 16);
	MemReader::Write(tae, this->m_fileID);
	MemReader::Write(tae, this->m_taeCount);
	MemReader::Write(tae, this->m_taeOffset);
	MemReader::Write(tae, this->m_taeLookupTableOffset);
	MemReader::Write(tae, this->m_iVar68);
	MemReader::Write(tae, this->m_taeCount2);
	MemReader::Write(tae, this->m_taeDataOffset);

	MemReader::Write(tae, this->m_iVar80);
	MemReader::Write(tae, this->m_pVar88);
	MemReader::Write(tae, this->m_fileId2);
	MemReader::Write(tae, this->m_fileId3);
	MemReader::Write(tae, this->m_pVar98);
	MemReader::Write(tae, this->m_pVarA0);
	MemReader::Write(tae, this->m_pVarA8);
	MemReader::Write(tae, this->m_skeletonNameOffset);
	MemReader::Write(tae, this->m_sibNameOffset);

	if (this->m_sibNameOffset != this->m_taeOffset)
		MemReader::WriteArray(tae, this->m_sibName, this->m_taeOffset - this->m_sibNameOffset);
}

TimeActReader::TimeActReader()
{
}

TimeActReader::TimeActReader(PWSTR filePath)
{
	this->m_filePath = filePath;

	std::filesystem::path path(filePath);

	this->m_fileName = path.filename().c_str();

	ifstream tae;

	tae.open(this->m_filePath, ios::binary);

	try
	{
		this->m_header = Header(&tae);
	}
	catch (const char* error)
	{
		ShowCursor(true);
		MessageBoxA(NULL, error, "TimeActReader.cpp", MB_ICONERROR);

		this->m_init = false;
		return;
	}

	this->m_fileSize = this->m_header.m_fileSize;

	this->m_tae.reserve(this->m_header.m_taeCount);

	tae.seekg(this->m_header.m_taeOffset);

	for (int i = 0; i < this->m_header.m_taeCount; i++)
		this->m_tae.push_back(TimeAct(&tae));

	this->m_lookupTable.m_groupCount = 0;
	this->m_lookupTable.m_groups.clear();

	tae.close();

	this->m_init = true;
}

TimeActReader::~TimeActReader()
{
}

void TimeActReader::AdjustOffsets()
{
	this->CreateTaeGroups();

	this->m_header.m_taeLookupTableOffset = this->m_header.m_taeOffset + 0x10 * this->m_header.m_taeCount;

	this->m_lookupTable.m_groupOffset = this->m_header.m_taeLookupTableOffset + 0x10;

	this->m_header.m_taeDataOffset = this->m_lookupTable.m_groupOffset + this->m_lookupTable.m_groupCount * 0x10;

	for (int i = 0; i < this->m_tae.size(); i++)
		this->m_tae[i].m_taeOffset = this->m_header.m_taeDataOffset + 0x30 * i;

	UINT64 dataBase = this->m_header.m_taeDataOffset + this->m_tae.size() * 0x30;

	if ((this->m_header.m_taeCount != this->m_header.m_taeCount2) || this->m_header.m_taeCount != this->m_tae.size())
		RDebug::SystemPanic("TimeActReader.cpp", "TimeAct count specified in the header does not match the actual amount (taeCount=%d, taeSize=%d)\n", this->m_header.m_taeCount, this->m_tae.size());

	UINT64 nextUnkDataOffset = dataBase;
	for (int i = 0; i < this->m_tae.size(); i++)
	{
		this->m_tae[i].m_taeData->m_animDataOffset = nextUnkDataOffset;

		this->m_tae[i].m_taeData->m_animData->m_nextOffsetOffset = this->m_tae[i].m_taeData->m_animDataOffset + 0x10;
		this->m_tae[i].m_taeData->m_animData->m_nextOffset = this->m_tae[i].m_taeData->m_animData->m_nextOffsetOffset + 0x20;

		UINT64 biggestOffset = this->m_tae[i].m_taeData->m_animDataOffset + 0x30;

		this->m_tae[i].m_taeData->m_timeCount = 2 * this->m_tae[i].m_taeData->m_eventCount;

		if (this->m_tae[i].m_taeData->m_timeCount > 0)
		{
			this->m_tae[i].m_taeData->m_timesOffset = this->m_tae[i].m_taeData->m_animDataOffset + 0x30;
			biggestOffset = this->m_tae[i].m_taeData->m_timesOffset;
		}
		else
			this->m_tae[i].m_taeData->m_timesOffset = 0;

		if (this->m_tae[i].m_taeData->m_eventCount > 0)
		{
			this->m_tae[i].m_taeData->m_timeCount = 2 * this->m_tae[i].m_taeData->m_eventCount;
			this->m_tae[i].m_taeData->m_eventOffset = this->m_tae[i].m_taeData->m_timesOffset + this->m_tae[i].m_taeData->m_timeCount * 0x4;

			biggestOffset = this->m_tae[i].m_taeData->m_eventOffset;
		}
		else
			this->m_tae[i].m_taeData->m_eventOffset = 0;

		UINT64 oldSize = 0;
		UINT64 oldDataOffset = biggestOffset + this->m_tae[i].m_taeData->m_eventCount * 0x18;
		for (int j = 0; j < this->m_tae[i].m_taeData->m_eventCount; j++)
		{
			this->m_tae[i].m_taeData->m_events[j].m_startOffset = this->m_tae[i].m_taeData->m_timesOffset + j * 0x8;
			this->m_tae[i].m_taeData->m_events[j].m_endOffset = this->m_tae[i].m_taeData->m_timesOffset + j * 0x8 + 0x4;
			this->m_tae[i].m_taeData->m_events[j].m_eventDataOffset = oldDataOffset + oldSize;
		
			this->m_tae[i].m_taeData->m_events[j].m_eventData->m_argsOffset = this->m_tae[i].m_taeData->m_events[j].m_eventDataOffset + 0x10;

			oldDataOffset = this->m_tae[i].m_taeData->m_events[j].m_eventDataOffset;
			oldSize = this->m_tae[i].m_taeData->m_events[j].GetArgumentsSize();
		}

		if (this->m_tae[i].m_taeData->m_eventGroupCount > 0)
			this->m_tae[i].m_taeData->m_eventGroupOffset = oldDataOffset + oldSize;
		else
			this->m_tae[i].m_taeData->m_eventGroupOffset = 0;

		UINT64 groupTotalSize = 0;
		for (int j = 0; j < this->m_tae[i].m_taeData->m_eventGroupCount; j++)
		{
			groupTotalSize += this->m_tae[i].m_taeData->m_groups[j].m_count * 0x8;

			int remainder = groupTotalSize % 16;
			if (remainder != 0)
				groupTotalSize += 16 - remainder;
		}

		UINT64 groupDataBase = this->m_tae[i].m_taeData->m_eventGroupOffset + this->m_tae[i].m_taeData->m_eventGroupCount * 0x20 + 0x40;
		UINT64 oldGroupSize = groupDataBase;
		for (int j = 0; j < this->m_tae[i].m_taeData->m_eventGroupCount; j++)
		{
			this->m_tae[i].m_taeData->m_groups[j].m_groupDataOffset = oldGroupSize;
			this->m_tae[i].m_taeData->m_groups[j].m_groupData->m_offset = this->m_tae[i].m_taeData->m_groups[j].m_groupDataOffset + 0x10;

			this->m_tae[i].m_taeData->m_groups[j].m_eventsOffset = this->m_tae[i].m_taeData->m_groups[j].m_groupDataOffset + 0x20;

			for (int k = 0; k < this->m_tae[i].m_taeData->m_groups[j].m_count; k++)
				this->m_tae[i].m_taeData->m_groups[j].m_eventOffset[k] = this->m_tae[i].m_taeData->GetEventGroupPtr(this->m_tae[i].m_taeData->m_groups[j].m_eventIndex[k]);
			
			UINT64 size = this->m_tae[i].m_taeData->m_groups[j].m_count * 0x8;

			int remainder = size % 16;
			if (remainder != 0)
				size += 16 - remainder;

			oldGroupSize = size + this->m_tae[i].m_taeData->m_groups[j].m_eventsOffset;

			biggestOffset = oldGroupSize;
		}

		nextUnkDataOffset = biggestOffset;
	}

	for (int i = 0; i < this->m_lookupTable.m_groupCount; i++)
	{
		for (int j = 0; j < this->m_tae.size(); j++)
		{
			if (this->m_tae[j].m_id == this->m_lookupTable.m_groups[i].m_taeStart)
			{
				this->m_lookupTable.m_groups[i].m_offset = this->m_header.m_taeOffset + j * 0x10;
				break;
			}
		}
	}

	this->m_header.m_fileSize = this->m_tae.back().m_taeData->m_animData->m_nextOffset;
}

bool TimeActReader::SaveFile(PWSTR pszOutFilePath)
{
	if (this->m_init == false)
		return false;

	std::filesystem::path path = pszOutFilePath;

	if (std::filesystem::exists(path))
	{
		std::filesystem::path bak_path = path;
		bak_path.replace_extension(L".tae.bak");

		std::filesystem::copy_file(path, bak_path, std::filesystem::copy_options::overwrite_existing);
	}

	this->m_outFilePath = pszOutFilePath;

	ofstream out;

	out.open(this->m_outFilePath, ios::binary);

	this->AdjustOffsets();

	this->m_header.GenerateBinary(&out);
	
	out.seekp(this->m_header.m_taeOffset);

	for (int i = 0; i < this->m_tae.size(); i++)
		this->m_tae[i].GenerateBinary(&out);

	out.seekp(this->m_header.m_taeLookupTableOffset);

	this->m_lookupTable.GenerateBinary(&out);

	out.close();

	return true;
}

void TimeActReader::CreateTaeGroups()
{
	this->m_lookupTable.m_groupCount = 0;
	this->m_lookupTable.m_groups.clear();

	//TODO adjust pointers to event groups
	for (int i = 0; i < this->m_tae.size(); i++)
		this->m_lookupTable.AddGroup(this->m_tae[i].m_id);
}

TimeAct* TimeActReader::TimeActLookup(int id)
{
	//TAE list **MUST** be ordered in crescent order for this to work
	int startIdx = 0;
	int endIdx = this->m_tae.size() - 1;

	while (startIdx <= endIdx)
	{
		int midIdx = (startIdx + endIdx) / 2;

		if (this->m_tae[midIdx].m_id == id)
			return &this->m_tae[midIdx];

		if (this->m_tae[midIdx].m_id > id)
			endIdx = midIdx - 1;
		else
			startIdx = midIdx + 1;
	}

	return NULL;
}

bool TimeActReader::AddTimeAct(int id, float lenght)
{
	int startIdx = 0;
	int endIdx = this->m_tae.size() - 1;
	int insertIdx = -1;

	while (startIdx <= endIdx)
	{
		int midIdx = (startIdx + endIdx) / 2;

		if (this->m_tae[midIdx].m_id == id)
			return false; //Our TAE is already present. Don't add

		if (this->m_tae[midIdx].m_id > id)
			endIdx = midIdx - 1;
		else
		{
			startIdx = midIdx + 1;
			insertIdx = midIdx + 1;
		}
	}

	if (insertIdx > -1)
	{
		this->m_header.m_taeCount++;
		this->m_header.m_taeCount2++;

		this->m_tae.insert(this->m_tae.begin() + insertIdx, TimeAct(id, lenght));
	}
}

bool TimeActReader::DeleteTimeAct(int id)
{
	int startIdx = 0;
	int endIdx = this->m_tae.size() - 1;
	int deleteIdx = -1;

	while (startIdx <= endIdx)
	{
		int midIdx = (startIdx + endIdx) / 2;

		if (this->m_tae[midIdx].m_id == id)
		{
			deleteIdx = midIdx;
			break;
		}

		if (this->m_tae[midIdx].m_id > id)
			endIdx = midIdx - 1;
		else
			startIdx = midIdx + 1;
	}

	if (deleteIdx > -1)
	{
		this->m_header.m_taeCount--;
		this->m_header.m_taeCount2--;

		this->m_tae.erase(this->m_tae.begin() + deleteIdx);

		return true;
	}

	return false;
}