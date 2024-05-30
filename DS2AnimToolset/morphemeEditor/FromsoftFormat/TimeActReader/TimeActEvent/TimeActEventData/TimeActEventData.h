#include <fstream>
#include <Windows.h>
#include <nlohmann/json.hpp>
#pragma once

using namespace std;
using json = nlohmann::json;

struct Argument
{
	std::string m_name;
	std::string m_type;

	CHAR m_s8;
	UCHAR m_u8;
	SHORT m_s16;
	USHORT m_u16;
	INT m_s32;
	UINT m_u32;
	FLOAT m_f32;
	INT64 m_s64;
	UINT64 m_u64;
};

class TimeActEventData
{
public:
	int m_size;
	std::vector<Argument> m_args;

	TimeActEventData() {}
	void GetData(ifstream* tae, int group_id, int event_id);
	void GenerateBinary(ofstream* tae);
	std::string GetArgumentsAsString();
	void ImGuiEdit();
	void CreateArguments(int id);
};