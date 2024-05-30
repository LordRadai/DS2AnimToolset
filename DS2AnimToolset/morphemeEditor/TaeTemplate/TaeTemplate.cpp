#include "TaeTemplate.h"
#include "../framework.h"
#include "../extern.h"

TaeTemplate::Group::Event::Arg::Arg(std::string name, std::string type)
{
	this->m_name = name;
	this->m_type = type;
}

TaeTemplate::Group::Event::Event(int id, std::string name)
{
	this->m_id = id;
	this->m_name = name;
}

TaeTemplate::Group::Group(int id, std::string name)
{
	this->m_id = id;
	this->m_name = name;
}

TaeTemplate::TaeTemplate() 
{
	this->m_file = nullptr;
}

TaeTemplate::TaeTemplate(std::ifstream& file)
{
	this->m_file = json::parse(file);

	this->from_json();
}

std::string TaeTemplate::GetGroupName(int id)
{
	for (int i = 0; i < this->m_groups.size(); i++)
	{
		if (this->m_groups[i].m_id == id)
			return this->m_groups[i].m_name;
	}

	return std::to_string(id);
}

std::string TaeTemplate::GetEventName(int group_id, int event_id)
{
	for (int i = 0; i < this->m_groups.size(); i++)
	{
		if (this->m_groups[i].m_id == group_id)
		{
			for (int j = 0; j < this->m_groups[i].m_events.size(); j++)
			{
				if (this->m_groups[i].m_events[j].m_id == event_id)
					return this->m_groups[i].m_events[j].m_name;
			}

			return this->m_groups[i].m_name + "_" + std::to_string(event_id);
		}
	}

	return std::to_string(group_id) + "_" + std::to_string(event_id);
}

void TaeTemplate::from_json()
{
	try
	{
		json groups = m_file.at("groups");

		for (json::iterator it_group = groups.begin(); it_group != groups.end(); it_group++)
		{
			this->m_groups.push_back(Group((*it_group).at("id").get<int>(), (*it_group).at("name").get<std::string>()));

			json events = (*it_group).at("events");

			for (json::iterator it_event = events.begin(); it_event != events.end(); it_event++)
			{
				this->m_groups.back().m_events.push_back(Group::Event((*it_event).at("id").get<int>(), (*it_event).at("name").get<std::string>()));

				json arguments = (*it_event).at("arguments");

				for (json::iterator it_argument = arguments.begin(); it_argument != arguments.end(); it_argument++)
					this->m_groups.back().m_events.back().m_args.push_back(Group::Event::Arg((*it_argument).at("name").get<std::string>(), (*it_argument).at("type").get<std::string>()));
			}
		}
	}
	catch (const std::exception& exc)
	{
		RDebug::SystemAlert(g_logLevel, MsgLevel_Error, "TaeTemplate.cpp", exc.what());
		return;
	}
}
