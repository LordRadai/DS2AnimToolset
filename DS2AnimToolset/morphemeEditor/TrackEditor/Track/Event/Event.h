#pragma once
#include <stdexcept>
#include <vector>
#include <Windows.h>
#include <string>
#include "FromSoftware/TimeAct/TimeAct.h"

namespace TrackEditor
{
	class Argument
	{
		friend class Event;

	public:
		static Argument* create(TimeAct::DataType type, std::string name);
		void destroy();

		std::string getAsString();

		template <typename T> T getValue() const
		{
			if (!m_data)
				throw("this->m_data is null");

			switch (this->type)
			{
			case TimeAct::kBool:
				return getTypedValue<bool, T>();
			case TimeAct::kByte:
				return getTypedValue<int8_t, T>();
			case TimeAct::kUByte:
				return getTypedValue<uint8_t, T>();
			case TimeAct::kShort:
				return getTypedValue<int16_t, T>();
			case TimeAct::kUShort:
				return getTypedValue<uint16_t, T>();
			case TimeAct::kInt:
				return getTypedValue<int32_t, T>();
			case TimeAct::kUInt:
				return getTypedValue<uint32_t, T>();
			case TimeAct::kFloat:
				return getTypedValue<float, T>();
			case TimeAct::kInt64:
				return getTypedValue<int64_t, T>();
			case TimeAct::kUInt64:
				return getTypedValue<uint64_t, T>();
			default:
				throw("Invalid type");
			}
		}

		template <typename T> void setValue(T v)
		{
			if (!m_data)
				throw("this->m_data is null");

			switch (this->type)
			{
			case TimeAct::kBool:
				return setTypedValue<bool, T>(v);
			case TimeAct::kByte:
				return setTypedValue<int8_t, T>(v);
			case TimeAct::kUByte:
				return setTypedValue<uint8_t, T>(v);
			case TimeAct::kShort:
				return setTypedValue<int16_t, T>(v);
			case TimeAct::kUShort:
				return setTypedValue<uint16_t, T>(v);
			case TimeAct::kInt:
				return setTypedValue<int32_t, T>(v);
			case TimeAct::kUInt:
				return setTypedValue<uint32_t, T>(v);
			case TimeAct::kFloat:
				return setTypedValue<float, T>(v);
			case TimeAct::kInt64:
				return setTypedValue<int64_t, T>(v);
			case TimeAct::kUInt64:
				return setTypedValue<uint64_t, T>(v);
			default:
				throw("Invalid type");
			}
		}

		TimeAct::DataType type = TimeAct::kTypeInvalid;
		std::string name = "";

	protected:
		void* m_data = nullptr;

		template <typename SourceType, typename TargetType>
		TargetType getTypedValue() const
		{
			return *reinterpret_cast<SourceType*>(m_data);
		}

		template <typename SourceType, typename TargetType>
		void setTypedValue(TargetType v)
		{
			*reinterpret_cast<SourceType*>(m_data) = v;
		}

		Argument() {}
		~Argument() {}
	};

	class Event
	{
		friend class TimeActEditor;
		friend class TrackEditorBase;
		friend class EventTrackEditor;
		friend class Track;
	public:
		static Event* create(int start, int end, int userData);
		Argument* addArgument(TimeAct::DataType type, std::string name);
		bool isEventActive(int currentFrame);
		void destroy();

		std::string getArgumentsString(int groupId, TimeAct::TaeTemplate* taeTemplate);
		void argsEditorGUI(int groupId, TimeAct::TaeTemplate* taeTemplate);

		int frameStart;
		int frameEnd;
		int userData;
		std::vector<Argument*> arguments;
	protected:
		Event() {}
		~Event() {}
	};
}