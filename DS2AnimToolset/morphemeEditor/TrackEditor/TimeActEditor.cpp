#include "TimeActEditor.h"
#include "extern.h"
#include "framework.h"
#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
#include <imgui/imstb_rectpack.h>

namespace TrackEditor
{
	namespace
	{
		void setArgumentValue(TimeAct::TaeExport::TimeActArgumentExportXML* dst, TimeAct::TaeExport::TimeActArgumentExportXML* src)
		{
			TimeAct::DataType type = src->getType();

			switch (type)
			{
			case TimeAct::kBool:
				break;
			case TimeAct::kByte:
				dst->setAsByte(src->getAsByte());
				break;
			case TimeAct::kUByte:
				dst->setAsUByte(src->getAsUByte());
				break;
			case TimeAct::kShort:
				dst->setAsShort(src->getAsShort());
				break;
			case TimeAct::kUShort:
				dst->setAsShort(src->getAsUShort());
				break;
			case TimeAct::kInt:
				dst->setAsInt(src->getAsInt());
				break;
			case TimeAct::kUInt:
				dst->setAsInt(src->getAsUInt());
				break;
			case TimeAct::kFloat:
				dst->setAsFloat(src->getAsFloat());
				break;
			case TimeAct::kInt64:
				dst->setAsInt64(src->getAsInt64());
				break;
			case TimeAct::kUInt64:
				dst->setAsUInt64(src->getAsUInt64());
				break;
			default:
				break;
			}
		}

		std::vector<Argument*> createArgumentsFromTemplate(int groupId, int eventId, TimeAct::TaeTemplate* taeTemplate)
		{
			std::vector<Argument*> argList;

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
								TimeAct::DataType argType = taeTemplate->groups[groupIdx].events[eventIdx].args[argIdx].type;
								std::string argName = taeTemplate->groups[groupIdx].events[eventIdx].args[argIdx].name;

								argList.push_back(Argument::create(argType, argName));
							}
						}
					}
				}
			}

			return argList;
		}
	
		bool isTrackLessThan(Track* first, Track* second)
		{
			return first->userData < second->userData;
		}

		int getGroupIdx(TimeAct::TaeTemplate* taeTemplate, int groupId)
		{
			for (size_t i = 0; i < taeTemplate->groups.size(); i++)
			{
				if (taeTemplate->groups[i].id == groupId)
					return i;
			}
		}
	}

	TimeActEditor* TimeActEditor::create(int options, TimeCodeFormat timeCodeFormat, TimeAct::TaeTemplate* templateRes)
	{
		TimeActEditor* editor = new TimeActEditor;

		editor->m_fps = 30;
		editor->m_editorOptions = (TrackEditorOptions)options;
		editor->m_timeCodeFormat = timeCodeFormat;
		editor->m_template = templateRes;
		editor->m_eventLabelAlignment = kLeft;

		editor->loadColorsFromXML("Data\\res\\color\\TimeActEditor.xml");

		return editor;
	}

	void TimeActEditor::update(float dt)
	{
		TrackEditorBase::update(dt);

		if (this->m_reload)
			this->reload();
	}

	void TimeActEditor::destroy()
	{
		this->clearTracks();

		delete this->eventBuffer;
		delete this->trackBuffer;

		delete this;
	}

	void TimeActEditor::setTimeAct(TimeAct::TaeExport::TimeActTrackExportXML* resource)
	{
		this->clearTracks();

		if (resource == nullptr)
			return;

		this->m_source = resource;

		this->m_edited = false;
		this->m_frameMin = 0;
		this->m_frameMax = resource->getFrameCount();

		this->m_firstFrame = 0;
		this->m_lastFrame = this->m_frameMax;
		this->m_selectedTrack = -1;
		this->m_selectedEvent = -1;

		for (size_t i = 0; i < resource->getNumEventGroups(); i++)
		{
			TimeAct::TaeExport::TimeActGroupExportXML* eventGroup = resource->getEventGroup(i);
			int groupId = eventGroup->getGroupId();

			Track* track = Track::create(i, groupId, this->m_template->getGroupName(groupId), false);
			
			for (size_t j = 0; j < eventGroup->getNumEvents(); j++)
			{
				TimeAct::TaeExport::TimeActEventExportXML* timeActEvent = eventGroup->getEvent(j);

				Event* event = Event::create(RMath::timeToFrame(timeActEvent->getStartTime(), this->m_fps), RMath::timeToFrame(timeActEvent->getEndTime(), this->m_fps), timeActEvent->getEventId());

				for (size_t k = 0; k < timeActEvent->getArgumentList()->getNumArguments(); k++)
				{
					TimeAct::TaeExport::TimeActArgumentExportXML* argXML = timeActEvent->getArgumentList()->getArgument(k);
					Argument* arg = event->addArgument(argXML->getType(), argXML->getName());

					TimeAct::DataType type = argXML->getType();

					switch (type)
					{
					case TimeAct::kBool:
						arg->setValue(argXML->getAsBool());
						break;
					case TimeAct::kByte:
						arg->setValue(argXML->getAsByte());
						break;
					case TimeAct::kUByte:
						arg->setValue(argXML->getAsUByte());
						break;
					case TimeAct::kShort:
						arg->setValue(argXML->getAsShort());
						break;
					case TimeAct::kUShort:
						arg->setValue(argXML->getAsUShort());
						break;
					case TimeAct::kInt:
						arg->setValue(argXML->getAsInt());
						break;
					case TimeAct::kUInt:
						arg->setValue(argXML->getAsUInt());
						break;
					case TimeAct::kFloat:
						arg->setValue(argXML->getAsFloat());
						break;
					case TimeAct::kInt64:
						arg->setValue(argXML->getAsInt64());
						break;
					case TimeAct::kUInt64:
						arg->setValue(argXML->getAsUInt64());
						break;
					}
				}

				track->events.push_back(event);
			}

			this->m_tracks.push_back(track);
		}
	}

	void TimeActEditor::reset()
	{
		TrackEditorBase::reset();
	}

	std::string TimeActEditor::getTrackName(int idx)
	{
		return this->m_template->getGroupName(this->m_tracks[idx]->userData);
	}

	std::string TimeActEditor::getEventLabel(int trackIdx, int eventIdx, bool arguments)
	{
		std::string eventString = this->m_template->getEventName(this->m_tracks[trackIdx]->userData, this->m_tracks[trackIdx]->events[eventIdx]->userData);

		if (arguments)
			return eventString + this->m_tracks[trackIdx]->events[eventIdx]->getArgumentsString(this->m_tracks[trackIdx]->userData, this->m_template);

		return eventString;
	}

	void TimeActEditor::addTrack(int userData, std::string name, bool discrete)
	{
		this->m_tracks.push_back(Track::create(this->m_tracks.size(), userData, name, false));

		std::sort(this->m_tracks.begin(), this->m_tracks.end(), isTrackLessThan);

		this->saveTracks();

		this->m_selectedTrack = -1;
		this->m_selectedEvent = -1;
		this->m_reload = true;
	}

	void TimeActEditor::deleteTrack(int idx)
	{
		for (size_t i = 0; i < this->m_tracks[idx]->events.size(); i++)
			this->deleteEvent(idx, i);

		this->m_tracks[idx]->destroy();
		this->m_tracks.erase(this->m_tracks.begin() + idx);

		this->saveTracks();

		this->m_selectedTrack = -1;
		this->m_selectedEvent = -1;
		this->m_reload = true;
	}

	void TimeActEditor::addEvent(int trackIdx, float startTime, float endTime, int userData)
	{
		Event* event = Event::create(RMath::timeToFrame(startTime, this->m_fps), RMath::timeToFrame(endTime, this->m_fps), userData);

		event->arguments = createArgumentsFromTemplate(this->m_tracks[trackIdx]->userData, userData, this->m_template);

		this->m_tracks[trackIdx]->events.push_back(event);

		this->saveTracks();

		this->m_selectedTrack = -1;
		this->m_selectedEvent = -1;
		this->m_reload = true;
	}

	void TimeActEditor::deleteEvent(int trackIdx, int eventIdx)
	{
		this->m_tracks[trackIdx]->events.erase(this->m_tracks[trackIdx]->events.begin() + eventIdx);

		this->saveTracks();

		this->m_selectedTrack = -1;
		this->m_selectedEvent = -1;
		this->m_reload = true;
	}

	bool TimeActEditor::addTrackGUI()
	{
		ImGui::PopStyleColor();
		if (ImGui::BeginPopup("addTrack"))
		{
			ImGui::Text("Add Track");
			ImGui::Separator();

			static int selectedGroupIdx = 0;

			if (ImGui::BeginCombo("Group Type", this->m_template->groups[selectedGroupIdx].name.c_str()))
			{
				for (size_t i = 0; i < this->m_template->groups.size(); i++)
				{
					TimeAct::TaeTemplate::Group group = this->m_template->groups[i];

					char label[256];
					sprintf_s(label, "%s [%d]", group.name.c_str(), group.id);

					ImGui::Selectable(label);

					if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(0))
					{
						selectedGroupIdx = i;
						this->trackBuffer->userData = this->m_template->groups[i].id;
					}
				}

				ImGui::EndCombo();
			}

			if (ImGui::Button("Add Track") || RInput::isKeyStateChanged(VK_RETURN))
			{
				this->addTrack(this->trackBuffer->userData, this->m_template->getGroupName(this->trackBuffer->userData), false);

				ImGui::CloseCurrentPopup();
			}

			ImGui::EndPopup();
		}

		ImGui::PushStyleColor(ImGuiCol_FrameBg, 0);

		return false;
	}

	bool TimeActEditor::deleteTrackGUI()
	{
		ImGui::PopStyleColor();

		if (ImGui::BeginPopup("deleteTrack"))
		{
			std::string header = this->getTrackName(this->m_selectedTrack);

			ImGui::Text(header.c_str());
			ImGui::Separator();

			if (ImGui::Button("Delete Track") || RInput::isKeyStateChanged(VK_RETURN))
			{
				this->deleteTrack(this->m_selectedTrack);

				ImGui::CloseCurrentPopup();
			}

			ImGui::EndPopup();
		}

		ImGui::PushStyleColor(ImGuiCol_FrameBg, 0);

		return false;
	}

	bool TimeActEditor::addEventGUI()
	{
		ImGui::PopStyleColor();

		if (ImGui::BeginPopup("addEvent"))
		{
			ImGui::Text(this->getTrackName(this->m_selectedTrack).c_str());
			ImGui::Separator();

			if (this->eventBuffer->frameEnd < this->eventBuffer->frameStart)
				this->eventBuffer->frameEnd = this->eventBuffer->frameStart;

			float startTime = RMath::frameToTime(this->eventBuffer->frameStart, this->m_fps);
			float endTime = RMath::frameToTime(this->eventBuffer->frameEnd, this->m_fps);

			ImGui::InputFloat("Start", &startTime, 1.f / this->m_fps);
			ImGui::InputFloat("End", &endTime, 1.f / this->m_fps);

			this->eventBuffer->frameStart = RMath::timeToFrame(startTime, this->m_fps);
			this->eventBuffer->frameEnd = RMath::timeToFrame(endTime, this->m_fps);

			this->m_currentFrame = this->eventBuffer->frameStart;

			this->notifyListeners();

			int groupIdx = getGroupIdx(this->m_template, this->m_tracks[this->m_selectedTrack]->userData);
			TimeAct::TaeTemplate::Group group = this->m_template->groups[groupIdx];

			static int selectedEventIdx = 0;

			if (ImGui::BeginCombo("Event Type", group.events[selectedEventIdx].name.c_str()))
			{
				for (size_t i = 0; i < group.events.size(); i++)
				{
					TimeAct::TaeTemplate::Group::Event event = group.events[i];

					char label[256];
					sprintf_s(label, "%s [%d]", event.name.c_str(), event.id);

					ImGui::Selectable(label);

					if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(0))
					{
						selectedEventIdx = i;
						this->eventBuffer->userData = this->m_template->groups[groupIdx].events[selectedEventIdx].id;
					}
				}

				ImGui::EndCombo();
			}

			if (ImGui::Button("Add Event") || RInput::isKeyStateChanged(VK_RETURN))
			{
				this->addEvent(this->m_selectedTrack, startTime, endTime, this->eventBuffer->userData);
				
				ImGui::CloseCurrentPopup();
			}

			ImGui::EndPopup();
		}

		ImGui::PushStyleColor(ImGuiCol_FrameBg, 0);

		return false;
	}

	bool TimeActEditor::deleteEventGUI()
	{
		ImGui::PopStyleColor();

		if (ImGui::BeginPopup("deleteEvent"))
		{
			std::string header = this->getTrackName(this->m_selectedTrack) + " [" + std::to_string(this->m_selectedEvent) + "]";

			ImGui::Text(header.c_str());
			ImGui::Separator();

			if (ImGui::Button("Delete Event") || RInput::isKeyStateChanged(VK_RETURN))
			{
				this->deleteEvent(this->m_selectedTrack, this->m_selectedEvent);

				ImGui::CloseCurrentPopup();
			}

			ImGui::EndPopup();
		}

		ImGui::PushStyleColor(ImGuiCol_FrameBg, 0);

		return false;
	}

	bool TimeActEditor::renameTrackGUI()
	{
		ImGui::PopStyleColor();
		ImGui::CloseCurrentPopup();
		ImGui::PushStyleColor(ImGuiCol_FrameBg, 0);

		return false;
	}

	void TimeActEditor::reload()
	{
		this->m_reload = false;

		this->setTimeAct(this->getSource());
	}

	bool TimeActEditor::saveTracks()
	{
		TimeAct::TaeExport::TimeActTrackExportXML* trackSource = this->getSource();
		trackSource->clear();

		for (size_t i = 0; i < this->m_tracks.size(); i++)
		{
			Track* track = this->m_tracks[i];

			TimeAct::TaeExport::TimeActGroupExportXML* eventGroupXML = trackSource->addEventGroup(track->userData);
		
			for (size_t j = 0; j < track->events.size(); j++)
			{
				Event* event = track->events[j];

				TimeAct::TaeExport::TimeActEventExportXML* eventXML = eventGroupXML->addEvent(RMath::frameToTime(event->frameStart, this->m_fps), RMath::frameToTime(event->frameEnd, this->m_fps), event->userData);
				TimeAct::TaeExport::TimeActArgumentListExportXML* argList = eventXML->createArgumentList();

				for (size_t k = 0; k < event->arguments.size(); k++)
				{
					Argument* arg = event->arguments[k];

					TimeAct::TaeExport::TimeActArgumentExportXML* argXML = argList->addArgument(arg->name);

					switch (arg->type)
					{
					case TimeAct::kBool:
						argXML->setAsBool(arg->getValue<bool>());
						break;
					case TimeAct::kByte:
						argXML->setAsByte(arg->getValue<INT8>());
						break;
					case TimeAct::kUByte:
						argXML->setAsUByte(arg->getValue<UINT8>());
						break;
					case TimeAct::kShort:
						argXML->setAsShort(arg->getValue<INT16>());
						break;
					case TimeAct::kUShort:
						argXML->setAsUShort(arg->getValue<UINT16>());
						break;
					case TimeAct::kInt:
						argXML->setAsInt(arg->getValue<INT32>());
						break;
					case TimeAct::kUInt:
						argXML->setAsInt(arg->getValue<UINT32>());
						break;
					case TimeAct::kFloat:
						argXML->setAsFloat(arg->getValue<float>());
						break;
					case TimeAct::kInt64:
						argXML->setAsInt64(arg->getValue<INT64>());
						break;
					case TimeAct::kUInt64:
						argXML->setAsUInt64(arg->getValue<UINT64>());
						break;
					}
				}
			}
		}

		this->reload();

		return true;
	}
}