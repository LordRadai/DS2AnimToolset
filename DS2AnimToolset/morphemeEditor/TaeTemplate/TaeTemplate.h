#include <fstream>
#include <vector>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

class TaeTemplate
{
public:
	struct Group
	{
		struct Event
		{
			struct Arg
			{
				std::string m_type;
				std::string m_name;

				Arg(std::string name, std::string type);
			};

			std::string m_name;
			int m_id;
			std::vector<Arg> m_args;

			Event(int id, std::string name);
		};

		std::string m_name;
		int m_id;
		std::vector<Event> m_events;

		Group(int id, std::string name);
	};

	std::vector<Group> m_groups;
	nlohmann::json m_file;

	TaeTemplate();
	TaeTemplate(std::ifstream& file);

	std::string GetGroupName(int id);
	std::string GetEventName(int group_id, int event_id);
	void from_json();
};