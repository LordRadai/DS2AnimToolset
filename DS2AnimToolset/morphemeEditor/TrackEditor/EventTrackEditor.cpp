#include "EventTrackEditor.h"
#include "framework.h"
#include "extern.h"
#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
#include <imgui/imstb_rectpack.h>

namespace TrackEditor
{
	EventTrackEditor* EventTrackEditor::create(int options, TimeCodeFormat timeCodeFormat)
	{
		EventTrackEditor* editor = new EventTrackEditor;

		editor->m_fps = 1000;
		editor->m_editorOptions = (TrackEditorOptions)options;
		editor->m_timeCodeFormat = timeCodeFormat;
		editor->m_eventLabelAlignment = kCenter;

		editor->loadColorsFromXML("Data\\res\\color\\EventTrackEditor.xml");

		return editor;
	}


	void EventTrackEditor::update(float dt)
	{
		TrackEditorBase::update(dt);

		if (this->m_reload)
			this->reload();
	}

	void EventTrackEditor::reset()
	{
		TrackEditorBase::reset();
	}

	void EventTrackEditor::destroy()
	{
		this->clearTracks();

		delete this->eventBuffer;
		delete this->trackBuffer;

		delete this;
	}

	std::string EventTrackEditor::getTrackName(int idx)
	{
		return this->m_tracks[idx]->name;
	}

	void EventTrackEditor::setEventTrack(AnimObject* resource)
	{
		this->clearTracks();

		if (resource == nullptr || resource->getTakeList() == nullptr)
			return;

		ME::TakeExport* take = resource->getTakeList()->getTake(0);
		this->m_source = resource;

		float duration = take->getCachedTakeSecondsDuration();

		this->m_edited = false;
		this->m_frameMin = 0;
		this->m_frameMax = RMath::timeToFrame(duration, this->m_fps);

		this->m_firstFrame = RMath::timeToFrame(take->getClipStart() * duration, this->m_fps);
		this->m_lastFrame = RMath::timeToFrame(take->getClipEnd() * duration, this->m_fps);

		this->m_selectedTrack = -1;
		this->m_selectedEvent = -1;

		for (size_t i = 0; i < take->getNumEventTracks(); i++)
		{
			ME::EventTrackExport* eventTrack = take->getEventTrack(i);
			ME::EventTrackExport::EventTrackType type = eventTrack->getEventTrackType();

			int groupId = eventTrack->getUserData();
			int numEvents = eventTrack->getNumEvents();

			Track* track = nullptr;

			switch (type)
			{
			case ME::EventTrackExport::EVENT_TRACK_TYPE_DISCRETE:
				track = Track::create(eventTrack->getEventTrackChannelID(), groupId, eventTrack->getName(), true);

				for (size_t j = 0; j < numEvents; j++)
				{
					ME::DiscreteEventExport* event = static_cast<ME::DiscreteEventTrackExportXML*>(eventTrack)->getEvent(j);

					track->events.push_back(Event::create(RMath::timeToFrame(duration * event->getNormalisedTime(), this->m_fps), RMath::timeToFrame(duration * event->getNormalisedTime(), this->m_fps), event->getUserData()));
				}
				break;
			case ME::EventTrackExport::EVENT_TRACK_TYPE_DURATION:
				track = Track::create(eventTrack->getEventTrackChannelID(), groupId, eventTrack->getName(), false);

				for (size_t j = 0; j < numEvents; j++)
				{
					ME::DurationEventExport* event = static_cast<ME::DurationEventTrackExportXML*>(eventTrack)->getEvent(j);

					track->events.push_back(Event::create(RMath::timeToFrame(duration * event->getNormalisedStartTime(), this->m_fps), RMath::timeToFrame(duration * (event->getNormalisedDuration() + event->getNormalisedStartTime()), this->m_fps), event->getUserData()));
				}
				break;
			default:
				throw("Invalid EventTrack type\n");
				break;
			}

			this->m_tracks.push_back(track);
		}
	}

	std::string EventTrackEditor::getEventLabel(int trackIdx, int eventIdx, bool arguments)
	{
		return std::to_string(this->m_tracks[trackIdx]->events[eventIdx]->userData);
	}

	void EventTrackEditor::addTrack(int userData, std::string name, bool discrete)
	{
		this->m_tracks.push_back(Track::create(this->m_tracks.size(), userData, name, discrete));

		this->saveTracks();

		this->m_selectedTrack = -1;
		this->m_selectedEvent = -1;
		this->m_reload = true;
	}

	void EventTrackEditor::deleteTrack(int idx)
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

	void EventTrackEditor::addEvent(int trackIdx, float startTime, float endTime, int userData)
	{
		this->m_tracks[trackIdx]->events.push_back(Event::create(RMath::timeToFrame(startTime, this->m_fps), RMath::timeToFrame(endTime, this->m_fps), userData));
	
		this->saveTracks();

		this->m_selectedTrack = -1;
		this->m_selectedEvent = -1;
		this->m_reload = true;
	}

	void EventTrackEditor::deleteEvent(int trackIdx, int eventIdx)
	{
		this->m_tracks[trackIdx]->events.erase(this->m_tracks[trackIdx]->events.begin() + eventIdx);
	
		this->saveTracks();

		this->m_selectedTrack = -1;
		this->m_selectedEvent = -1;
		this->m_reload = true;
	}

	bool EventTrackEditor::addTrackGUI()
	{
		ImGui::PopStyleColor();

		if (ImGui::BeginPopup("addTrack"))
		{
			ImGui::Text("Add Track");
			ImGui::Separator();

			ImGui::InputText("Name", this->trackBuffer->name, 256);
			ImGui::InputInt("User Data", &this->trackBuffer->userData);

			const char* types[] = { "Duration", "Discrete" };
			const char* previewVal = types[this->trackBuffer->discrete];

			if (ImGui::BeginCombo("Type", previewVal))
			{
				for (size_t i = 0; i < 2; i++)
				{
					ImGui::Selectable(types[i]);

					if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(0))
						this->trackBuffer->discrete = i;
				}

				ImGui::EndCombo();
			}

			if (ImGui::Button("Add Track") || RInput::isKeyStateChanged(VK_RETURN))
			{
				this->addTrack(this->trackBuffer->userData, this->trackBuffer->name, this->trackBuffer->discrete);

				ImGui::CloseCurrentPopup();
			}

			ImGui::EndPopup();
		}

		ImGui::PushStyleColor(ImGuiCol_FrameBg, 0);

		return false;
	}

	bool EventTrackEditor::deleteTrackGUI()
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

	bool EventTrackEditor::addEventGUI()
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

			ImGui::InputInt("Event ID", &this->eventBuffer->userData);

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

	bool EventTrackEditor::deleteEventGUI()
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

	bool EventTrackEditor::renameTrackGUI()
	{
		bool closePopup = false;

		ImGui::PopStyleColor();

		if (ImGui::BeginPopup("renameTrack"))
		{
			std::string header = this->getTrackName(this->m_selectedTrack);

			ImGui::Text(header.c_str());
			ImGui::Separator();

			ImGui::InputText("Name", this->m_tracks[this->m_selectedTrack]->name, 256);
			ImGui::InputInt("User Data", &this->m_tracks[this->m_selectedTrack]->userData);

			if (RInput::isKeyStateChanged(VK_RETURN))
				ImGui::CloseCurrentPopup();

			ImGui::EndPopup();
		}

		ImGui::PushStyleColor(ImGuiCol_FrameBg, 0);

		return false;
	}

	void EventTrackEditor::reload()
	{
		this->m_reload = false;

		this->setEventTrack(this->getSource());
	}

	bool EventTrackEditor::saveTracks()
	{
		AnimObject* source = this->getSource();
		
		if (source == nullptr)
			return false;

		const float duration = RMath::frameToTime(this->m_frameMax, this->m_fps);

		ME::TakeExportXML* takeXMLOld = static_cast<ME::TakeExportXML*>(source->getTakeList()->getTake(0));

		const std::wstring dest = RString::toWide(source->getTakeList()->getDestFilename());
		const std::wstring takeName = RString::toWide(takeXMLOld->getName());
		const float takeFps = takeXMLOld->getCachedTakeFPS();
		const bool takeLoop = takeXMLOld->getLoop();

		delete takeXMLOld;

		const float clipStart = RMath::frameToTime(this->m_firstFrame, this->m_fps) / duration;
		const float clipEnd = RMath::frameToTime(this->m_lastFrame, this->m_fps) / duration;

		ME::ExportFactoryXML factory;
		ME::TakeListXML* takeList = (ME::TakeListXML*)factory.createTakeList(dest.c_str(), dest.c_str());
		ME::TakeExportXML* take = (ME::TakeExportXML*)takeList->createTake(takeName.c_str(), duration, takeFps, takeLoop, clipStart, clipEnd);

		for (size_t i = 0; i < this->m_tracks.size(); i++)
		{
			Track* track = this->m_tracks[i];

			GUID gidReference;
			CoCreateGuid(&gidReference);

			if (track->discrete)
			{
				ME::DiscreteEventTrackExportXML* trackXML = static_cast<ME::DiscreteEventTrackExportXML*>(take->createEventTrack(ME::EventTrackExport::EVENT_TRACK_TYPE_DISCRETE, RString::guidToString(gidReference).c_str(), RString::toWide(track->name).c_str(), track->channelId, track->userData));
			
				for (size_t j = 0; j < track->events.size(); j++)
				{
					const Event* event = track->events[j];

					const float normStartTime = RMath::frameToTime(event->frameStart, this->m_fps) / duration;

					trackXML->createEvent(j, normStartTime, event->userData);
				}
			}
			else
			{
				ME::DurationEventTrackExportXML* trackXML = static_cast<ME::DurationEventTrackExportXML*>(take->createEventTrack(ME::EventTrackExport::EVENT_TRACK_TYPE_DURATION, RString::guidToString(gidReference).c_str(), RString::toWide(track->name).c_str(), track->channelId, track->userData));

				for (size_t j = 0; j < track->events.size(); j++)
				{
					const Event* event = track->events[j];

					const float normStartTime = RMath::frameToTime(event->frameStart, this->m_fps) / duration;
					const float normDuration = RMath::frameToTime(event->frameEnd - event->frameStart, this->m_fps) / duration;

					trackXML->createEvent(j, normStartTime, normDuration, event->userData);
				}
			}
		}

		source->setTakeList(takeList);

		this->m_reload = true;

		return true;
	}
}