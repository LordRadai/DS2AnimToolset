#include "TimeActEventData.h"
#include "RCore.h"
#include "extern.h"
#include "RCore.h"
#include "../../TaeTemplate/TaeTemplate.h"
#define ARGS_BUFFER_SIZE 32

namespace TimeAct
{
	Argument* Argument::create(DataType type, std::string name)
	{
		Argument* arg = new Argument;

		arg->m_type = type;
		arg->m_name = name;

		switch (type)
		{
		case kBool:
			arg->m_data = new bool;
			arg->setValue(false);
			break;
		case kByte:
			arg->m_data = new BYTE;
			arg->setValue((int8_t)0);
			break;
		case kUByte:
			arg->m_data = new BYTE;
			arg->setValue((uint8_t)0);
			break;
		case kShort:
			arg->m_data = new SHORT;
			arg->setValue((int16_t)0);
			break;
		case kUShort:
			arg->m_data = new USHORT;
			arg->setValue((uint16_t)0);
			break;
		case kInt:
			arg->m_data = new INT;
			arg->setValue((int32_t)0);
			break;
		case kUInt:
			arg->m_data = new UINT;
			arg->setValue((uint32_t)0);
			break;
		case kFloat:
			arg->m_data = new FLOAT;
			arg->setValue(0.f);
			break;
		case kInt64:
			arg->m_data = new INT64;
			arg->setValue((int64_t)0);
			break;
		case kUInt64:
			arg->m_data = new UINT64;
			arg->setValue((uint64_t)0);
			break;
		default:
			throw("Invalid argument type");
			break;
		}

		return arg;
	}

	Argument* Argument::createFromResource(DataType type, std::string name, void* buffer)
	{
		Argument* arg = new Argument;

		arg->m_type = type;
		arg->m_name = name;

		switch (type)
		{
		case kBool:
			arg->m_data = new bool;
			memcpy(arg->m_data, buffer, sizeof(bool));
			break;
		case kByte:
			arg->m_data = new BYTE;
			memcpy(arg->m_data, buffer, sizeof(BYTE));
			break;
		case kUByte:
			arg->m_data = new BYTE;
			memcpy(arg->m_data, buffer, sizeof(BYTE));
			break;
		case kShort:
			arg->m_data = new SHORT;
			memcpy(arg->m_data, buffer, sizeof(SHORT));
			break;
		case kUShort:
			arg->m_data = new USHORT;
			memcpy(arg->m_data, buffer, sizeof(USHORT));
			break;
		case kInt:
			arg->m_data = new INT;
			memcpy(arg->m_data, buffer, sizeof(INT));
			break;
		case kUInt:
			arg->m_data = new UINT;
			memcpy(arg->m_data, buffer, sizeof(UINT));
			break;
		case kFloat:
			arg->m_data = new FLOAT;
			memcpy(arg->m_data, buffer, sizeof(FLOAT));
			break;
		case kInt64:
			arg->m_data = new INT64;
			memcpy(arg->m_data, buffer, sizeof(INT64));
			break;
		case kUInt64:
			arg->m_data = new UINT64;
			memcpy(arg->m_data, buffer, sizeof(UINT64));
			break;
		default:
			throw("Invalid argument type");
			break;
		}

		return arg;
	}

	void Argument::destroy()
	{
		if (this->m_data)
			delete this->m_data;
	}

	int Argument::generateBinary(RFile* file)
	{
		switch (this->m_type)
		{
		case kBool:
			file->write(getValue<bool>());
			break;
		case kByte:
			file->write(getValue<INT8>());
			break;
		case kUByte:
			file->write(getValue<UINT8>());
			break;
		case kShort:
			file->write(getValue<INT16>());
			break;
		case kUShort:
			file->write(getValue<UINT16>());
			break;
		case kInt:
			file->write(getValue<INT32>());
			break;
		case kUInt:
			file->write(getValue<UINT32>());
			break;
		case kFloat:
			file->write(getValue<FLOAT>());
			break;
		case kInt64:
			file->write(getValue<INT64>());
			break;
		case kUInt64:
			file->write(getValue<UINT64>());
			break;
		default:
			throw("Invalid data type");
			break;
		}

		return file->tell();
	}

	TimeActEventData* TimeActEventData::create(int groupId, int eventId, TaeTemplate* taeTemplate)
	{
		TimeActEventData* eventData = new TimeActEventData;

		for (size_t groupIdx = 0; groupIdx < taeTemplate->groups.size(); groupIdx++)
		{
			if (taeTemplate->groups[groupIdx].id == groupId)
			{
				for (size_t eventIdx = 0; eventIdx < taeTemplate->groups[groupIdx].events.size(); eventIdx++)
				{
					if (taeTemplate->groups[groupIdx].events[eventIdx].id == eventId)
					{
						for (size_t argIdx = 0; argIdx < taeTemplate->groups[groupIdx].events[eventIdx].args.size(); argIdx++)
						{
							DataType argType = taeTemplate->groups[groupIdx].events[eventIdx].args[argIdx].type;
							std::string argName = taeTemplate->groups[groupIdx].events[eventIdx].args[argIdx].name;

							switch (argType)
							{
							case kBool:
								eventData->m_size += sizeof(bool);
								break;
							case kByte:
								eventData->m_size += sizeof(int8_t);
								break;
							case kUByte:
								eventData->m_size += sizeof(uint8_t);
								break;
							case kShort:
								eventData->m_size += sizeof(int16_t);
								break;
							case kUShort:
								eventData->m_size += sizeof(uint16_t);
								break;
							case kInt:
								eventData->m_size += sizeof(int32_t);
								break;
							case kUInt:
								eventData->m_size += sizeof(uint32_t);
								break;
							case kFloat:
								eventData->m_size += sizeof(float);
								break;
							case kInt64:
								eventData->m_size += sizeof(int64_t);
								break;
							case kUInt64:
								eventData->m_size += sizeof(uint64_t);
								break;
							}

							eventData->m_args.push_back(Argument::create(argType, argName));
						}
					}
				}
			}
		}

		return eventData;
	}

	TimeActEventData* TimeActEventData::createFromResource(char* resource, int groupId, int eventId, TaeTemplate* taeTemplate)
	{
		TimeActEventData* eventData = new TimeActEventData;

		for (size_t groupIdx = 0; groupIdx < taeTemplate->groups.size(); groupIdx++)
		{
			if (taeTemplate->groups[groupIdx].id == groupId)
			{
				for (size_t eventIdx = 0; eventIdx < taeTemplate->groups[groupIdx].events.size(); eventIdx++)
				{
					if (taeTemplate->groups[groupIdx].events[eventIdx].id == eventId)
					{
						int offset = 0;
						for (size_t argIdx = 0; argIdx < taeTemplate->groups[groupIdx].events[eventIdx].args.size(); argIdx++)
						{
							DataType argType = taeTemplate->groups[groupIdx].events[eventIdx].args[argIdx].type;
							std::string argName = taeTemplate->groups[groupIdx].events[eventIdx].args[argIdx].name;

							switch (argType)
							{
							case kBool:
								eventData->m_size += sizeof(bool);
								break;
							case kByte:
								eventData->m_size += sizeof(int8_t);
								break;
							case kUByte:
								eventData->m_size += sizeof(uint8_t);
								break;
							case kShort:
								eventData->m_size += sizeof(int16_t);
								break;
							case kUShort:
								eventData->m_size += sizeof(uint16_t);
								break;
							case kInt:
								eventData->m_size += sizeof(int32_t);
								break;
							case kUInt:
								eventData->m_size += sizeof(uint32_t);
								break;
							case kFloat:
								eventData->m_size += sizeof(float);
								break;
							case kInt64:
								eventData->m_size += sizeof(int64_t);
								break;
							case kUInt64:
								eventData->m_size += sizeof(uint64_t);
								break;
							}

							eventData->m_args.push_back(Argument::createFromResource(argType, argName, &resource[offset]));
							offset = eventData->m_size;
						}
					}
				}
			}
		}

		return eventData;
	}

	void TimeActEventData::destroy()
	{
		for (size_t i = 0; i < this->m_args.size(); i++)
		{
			if (this->m_args[i])
			{
				this->m_args[i]->destroy();
				delete this->m_args[i];
			}
		}

		this->m_args.clear();
	}

	int TimeActEventData::getMemoryRequirements()
	{
		return ARGS_BUFFER_SIZE;
	}

	int TimeActEventData::generateBinary(RFile* file)
	{
		for (size_t i = 0; i < this->m_args.size(); i++)
			this->m_args[i]->generateBinary(file);

		if (this->m_size < ARGS_BUFFER_SIZE)
		{
			for (size_t i = 0; i < ARGS_BUFFER_SIZE - this->m_size; i++)
				file->write(0);
		}
		else
			throw("Exceeded maximum TimeActEvent arg buffer size");

		file->alignStream(16);

		return file->tell();
	}
}