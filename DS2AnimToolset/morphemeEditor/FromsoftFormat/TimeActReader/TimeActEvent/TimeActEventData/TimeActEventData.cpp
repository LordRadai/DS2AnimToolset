#include "TimeActEventData.h"
#include "../../../Utils/MemReader/MemReader.h"
#include "../../../../Application/Application.h"
#include "../../../../TaeTemplate/TaeTemplate.h"

void TimeActEventData::GetData(ifstream* tae, int group_id, int event_id)
{
	for (int group_idx = 0; group_idx < g_taeTemplate.m_groups.size(); group_idx++)
	{
		if (g_taeTemplate.m_groups[group_idx].m_id == group_id)
		{
			for (int event_idx = 0; event_idx < g_taeTemplate.m_groups[group_idx].m_events.size(); event_idx++)
			{
				if (g_taeTemplate.m_groups[group_idx].m_events[event_idx].m_id == event_id)
				{
					for (int arg_idx = 0; arg_idx < g_taeTemplate.m_groups[group_idx].m_events[event_idx].m_args.size(); arg_idx++)
					{
						this->m_args.push_back(Argument());
						this->m_args.back().m_type = g_taeTemplate.m_groups[group_idx].m_events[event_idx].m_args[arg_idx].m_type;
						this->m_args.back().m_name = g_taeTemplate.m_groups[group_idx].m_events[event_idx].m_args[arg_idx].m_name;

						std::string arg_type = this->m_args[arg_idx].m_type;

						if (arg_type.compare("s8") == 0)
						{
							this->m_size += 1;

							MemReader::Read(tae, &this->m_args.back().m_s8);
						}
						else if (arg_type.compare("u8") == 0)
						{
							this->m_size += 1;

							MemReader::Read(tae, &this->m_args.back().m_u8);
						}
						else if (arg_type.compare("s16") == 0)
						{
							this->m_size += 2;

							MemReader::Read(tae, &this->m_args.back().m_s16);
						}
						else if (arg_type.compare("u16") == 0)
						{
							this->m_size += 2;

							MemReader::Read(tae, &this->m_args.back().m_u16);
						}
						else if (arg_type.compare("s32") == 0)
						{
							this->m_size += 4;

							MemReader::Read(tae, &this->m_args.back().m_s32);
						}
						else if (arg_type.compare("u32") == 0)
						{
							this->m_size += 4;

							MemReader::Read(tae, &this->m_args.back().m_u32);
						}
						else if (arg_type.compare("f32") == 0)
						{
							this->m_size += 4;

							MemReader::Read(tae, &this->m_args.back().m_f32);
						}
						else if (arg_type.compare("s64") == 0)
						{
							this->m_size += 8;

							MemReader::Read(tae, &this->m_args.back().m_s64);
						}
						else if (arg_type.compare("u64") == 0)
						{
							this->m_size += 8;

							MemReader::Read(tae, &this->m_args.back().m_u64);
						}
					}

					break;
				}
			}

			break;
		}
	}
}

void TimeActEventData::GenerateBinary(ofstream* tae)
{
	for (size_t i = 0; i < this->m_args.size(); i++)
	{
		std::string arg_type = this->m_args[i].m_type;

		if (arg_type.compare("s8") == 0)
			MemReader::Write(tae, this->m_args[i].m_s8);
		else if (arg_type.compare("u8") == 0)
			MemReader::Write(tae, this->m_args[i].m_u8);
		else if (arg_type.compare("s16") == 0)
			MemReader::Write(tae, this->m_args[i].m_s16);
		else if (arg_type.compare("u16") == 0)
			MemReader::Write(tae, this->m_args[i].m_u16);
		else if (arg_type.compare("s32") == 0)
			MemReader::Write(tae, this->m_args[i].m_s32);
		else if (arg_type.compare("u32") == 0)
			MemReader::Write(tae, this->m_args[i].m_u32);
		else if (arg_type.compare("f32") == 0)
			MemReader::Write(tae, this->m_args[i].m_f32);
		else if (arg_type.compare("s64") == 0)
			MemReader::Write(tae, this->m_args[i].m_s64);
		else if (arg_type.compare("u64") == 0)
			MemReader::Write(tae, this->m_args[i].m_u64);
	}
}

std::string TimeActEventData::GetArgumentsAsString()
{
	if (this->m_args.size() == 0)
		return "";

	std::string arg_str = "(";

	for (size_t i = 0; i < this->m_args.size(); i++)
	{
		std::string arg_type = this->m_args[i].m_type;

		if (arg_type.compare("s8") == 0)
			arg_str += std::to_string(this->m_args[i].m_s8);
		else if (arg_type.compare("u8") == 0)
			arg_str += std::to_string(this->m_args[i].m_u8);
		else if (arg_type.compare("s16") == 0)
			arg_str += std::to_string(this->m_args[i].m_s16);
		else if (arg_type.compare("u16") == 0)
			arg_str += std::to_string(this->m_args[i].m_u16);
		else if (arg_type.compare("s32") == 0)
			arg_str += std::to_string(this->m_args[i].m_s32);
		else if (arg_type.compare("u32") == 0)
			arg_str += std::to_string(this->m_args[i].m_u32);
		else if (arg_type.compare("f32") == 0)
			arg_str += RString::FloatToString(this->m_args[i].m_f32);
		else if (arg_type.compare("s64") == 0)
			arg_str += std::to_string(this->m_args[i].m_s64);
		else if (arg_type.compare("u64") == 0)
			arg_str += std::to_string(this->m_args[i].m_u64);

		if (i < this->m_args.size() - 1)
			arg_str += ", ";
	}

	arg_str += ")\n";

	return arg_str;
}

void TimeActEventData::ImGuiEdit()
{
	if (this->m_args.size() == 0)
		return;

	ImGui::SeparatorText("Arguments");

	for (size_t i = 0; i < this->m_args.size(); i++)
	{
		std::string arg_type = this->m_args[i].m_type;

		ImGui::PushID(i);
		if (arg_type.compare("s8") == 0)
			ImGui::InputByte(this->m_args[i].m_name.c_str(), (ImS8*)&this->m_args[i].m_s8, 0, 0, 0);
		else if (arg_type.compare("u8") == 0)
			ImGui::InputUByte(this->m_args[i].m_name.c_str(), (ImU8*)&this->m_args[i].m_u8, 0, 0, 0);
		else if (arg_type.compare("s16") == 0)
			ImGui::InputShort(this->m_args[i].m_name.c_str(), (ImS16*)&this->m_args[i].m_s16, 0, 0, 0);
		else if (arg_type.compare("u16") == 0)
			ImGui::InputUShort(this->m_args[i].m_name.c_str(), (ImU16*)&this->m_args[i].m_u16, 0, 0, 0);
		else if (arg_type.compare("s32") == 0)
			ImGui::InputInt(this->m_args[i].m_name.c_str(), (ImS32*)&this->m_args[i].m_s32, 0, 0, 0);
		else if (arg_type.compare("u32") == 0)
			ImGui::InputUInt(this->m_args[i].m_name.c_str(), (ImU32*)&this->m_args[i].m_u32, 0, 0, 0);
		else if (arg_type.compare("f32") == 0)
			ImGui::InputFloat(this->m_args[i].m_name.c_str(), &this->m_args[i].m_f32, 0, 0, 0);
		else if (arg_type.compare("s64") == 0)
			ImGui::InputInt64(this->m_args[i].m_name.c_str(), (ImS64*)&this->m_args[i].m_s64, 0, 0, 0);
		else if (arg_type.compare("u64") == 0)
			ImGui::InputUInt64(this->m_args[i].m_name.c_str(), (ImU64*)&this->m_args[i].m_u64, 0, 0, 0);
		ImGui::PopID();
	}
}

void TimeActEventData::CreateArguments(int id)
{
	for (int group_idx = 0; group_idx < g_taeTemplate.m_groups.size(); group_idx++)
	{
		for (int event_idx = 0; event_idx < g_taeTemplate.m_groups[group_idx].m_events.size(); event_idx++)
		{
			if (g_taeTemplate.m_groups[group_idx].m_events[event_idx].m_id == id)
			{
				for (int arg_idx = 0; arg_idx < g_taeTemplate.m_groups[group_idx].m_events[event_idx].m_args.size(); arg_idx++)
				{
					this->m_args.push_back(Argument());
					this->m_args.back().m_type = g_taeTemplate.m_groups[group_idx].m_events[event_idx].m_args[arg_idx].m_type;
					this->m_args.back().m_name = g_taeTemplate.m_groups[group_idx].m_events[event_idx].m_args[arg_idx].m_name;

					std::string arg_type = this->m_args[arg_idx].m_type;

					if (arg_type.compare("s8") == 0)
					{
						this->m_size += 1;
					}
					else if (arg_type.compare("u8") == 0)
					{
						this->m_size += 1;
					}
					else if (arg_type.compare("s16") == 0)
					{
						this->m_size += 2;
					}
					else if (arg_type.compare("u16") == 0)
					{
						this->m_size += 2;
					}
					else if (arg_type.compare("s32") == 0)
					{
						this->m_size += 4;
					}
					else if (arg_type.compare("u32") == 0)
					{
						this->m_size += 4;
					}
					else if (arg_type.compare("f32") == 0)
					{
						this->m_size += 4;
					}
					else if (arg_type.compare("s64") == 0)
					{
						this->m_size += 8;
					}
					else if (arg_type.compare("u64") == 0)
					{
						this->m_size += 8;
					}
				}

				return;
			}
		}
	}
}