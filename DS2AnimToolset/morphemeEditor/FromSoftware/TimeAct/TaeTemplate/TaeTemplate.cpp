#include "TaeTemplate.h"
#include "../framework.h"
#include "../extern.h"

namespace TimeAct
{
	TaeTemplate* TaeTemplate::load(std::wstring filepath)
	{
		TaeTemplate* taeTemplate = new TaeTemplate;
		TaeTemplateXML* templateXML = TaeTemplateXML::loadFromFile(filepath);

		taeTemplate->sourceXML = templateXML;

		for (size_t groupIdx = 0; groupIdx < templateXML->getNumEventGroups(); groupIdx++)
		{
			TaeTemplateEventGroupXML* groupTemplateXML = templateXML->getEventGroupTemplate(groupIdx);

			Group group(groupTemplateXML->getId(), groupTemplateXML->getName());

			for (size_t eventIdx = 0; eventIdx < groupTemplateXML->getNumEvents(); eventIdx++)
			{
				TaeTemplateEventXML* eventTemplateXML = groupTemplateXML->getEventTemplate(eventIdx);

				Group::Event event(eventTemplateXML->getId(), eventTemplateXML->getName());

				for (size_t argIdx = 0; argIdx < eventTemplateXML->getNumArguments(); argIdx++)
				{
					TaeTemplateEventArgumentXML* argTemplateXML = eventTemplateXML->getArgumentTemplate(argIdx);

					Group::Event::Arg arg(argTemplateXML->getName(), argTemplateXML->getType(), argTemplateXML->isHidden());

					for (size_t argValueIdx = 0; argValueIdx < argTemplateXML->getNumValues(); argValueIdx++)
					{
						TaeTemplateArgEnumXML* enumValueXML = argTemplateXML->getEnumValue(argValueIdx);

						Group::Event::Arg::ArgEnumValue enumValue(enumValueXML->getName(), enumValueXML->getValue());

						arg.values.push_back(enumValue);
					}

					event.args.push_back(arg);
				}

				group.events.push_back(event);
			}

			taeTemplate->groups.push_back(group);
		}

		return taeTemplate;
	}

	TaeTemplate::Group* TaeTemplate::getGroup(int id)
	{
		for (int i = 0; i < this->groups.size(); i++)
		{
			if (this->groups[i].id == id)
				return &this->groups[i];
		}

		return nullptr;
	}

	TaeTemplate::Group::Event* TaeTemplate::getEvent(int groupId, int id)
	{
		for (int i = 0; i < this->groups.size(); i++)
		{
			if (this->groups[i].id == groupId)
			{
				for (int j = 0; j < this->groups[i].events.size(); j++)
				{
					if (this->groups[i].events[j].id == id)
						return &this->groups[i].events[j];
				}
			}
		}

		return nullptr;
	}

	std::string TaeTemplate::getGroupName(int id)
	{
		for (int i = 0; i < this->groups.size(); i++)
		{
			if (this->groups[i].id == id)
				return this->groups[i].name;
		}

		return std::to_string(id);
	}

	std::string TaeTemplate::getEventName(int group_id, int event_id)
	{
		for (int i = 0; i < this->groups.size(); i++)
		{
			if (this->groups[i].id == group_id)
			{
				for (int j = 0; j < this->groups[i].events.size(); j++)
				{
					if (this->groups[i].events[j].id == event_id)
						return this->groups[i].events[j].name;
				}

				return this->groups[i].name + "_" + std::to_string(event_id);
			}
		}

		return std::to_string(group_id) + "_" + std::to_string(event_id);
	}
}