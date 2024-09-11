#pragma once
#include <fstream>
#include <vector>
#include "TaeTemplateXML/TaeTemplateXML.h"

namespace TimeAct
{
	class TaeTemplate
	{
	public:
		struct Group
		{
			struct Event
			{
				struct Arg
				{
					struct ArgEnumValue
					{
						std::string name;
						int value;

						ArgEnumValue(std::string name, int value)
						{
							this->name = name;
							this->value = value;
						}
					};

					DataType type;
					std::string name;
					bool hidden;

					std::vector< ArgEnumValue> values;

					Arg(std::string name, DataType type, bool hidden)
					{
						this->name = name;
						this->type = type;
						this->hidden = hidden;
					}
				};

				std::string name;
				int id;
				std::vector<Arg> args;

				Event(int id, std::string name)
				{
					this->name = name;
					this->id = id;
				}
			};

			std::string name;
			int id;
			std::vector<Event> events;

			Group(int id, std::string name)
			{
				this->id = id;
				this->name = name;
			}
		};

		static TaeTemplate* load(std::wstring filepath);

		std::vector<Group> groups;
		TaeTemplateXML* sourceXML = nullptr;

		TaeTemplate::Group* getGroup(int id);
		TaeTemplate::Group::Event* getEvent(int groupId, int id);

		std::string getGroupName(int id);
		std::string getEventName(int group_id, int event_id);

	private:
		TaeTemplate() {}
	};
}