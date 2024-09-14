#include "Event.h"
#include "imgui/imgui.h"
#include "imgui_custom/imgui_custom_widget.h"

namespace TrackEditor
{
	namespace
	{
		template<typename T> int getArgEnumValueIndex(TimeAct::TaeTemplate::Group::Event::Arg argTemplate, T value)
		{
			for (size_t i = 0; i < argTemplate.values.size(); i++)
			{
				if (argTemplate.values[i].value == value)
					return i;
			}
		}
	}

	Argument* Argument::create(TimeAct::DataType type, std::string name)
	{
		Argument* arg = new Argument;
		arg->type = type;
		arg->name = name;

		switch (type)
		{
		case TimeAct::kBool:
			arg->m_data = new bool;
			arg->setValue(false);
			break;
		case TimeAct::kByte:
			arg->m_data = new BYTE;
			arg->setValue((int8_t)0);
			break;
		case TimeAct::kUByte:
			arg->m_data = new BYTE;
			arg->setValue((uint8_t)0);
			break;
		case TimeAct::kShort:
			arg->m_data = new SHORT;
			arg->setValue((int16_t)0);
			break;
		case TimeAct::kUShort:
			arg->m_data = new USHORT;
			arg->setValue((uint16_t)0);
			break;
		case TimeAct::kInt:
			arg->m_data = new INT;
			arg->setValue((int32_t)0);
			break;
		case TimeAct::kUInt:
			arg->m_data = new UINT;
			arg->setValue((uint32_t)0);
			break;
		case TimeAct::kFloat:
			arg->m_data = new FLOAT;
			arg->setValue(0.f);
			break;
		case TimeAct::kInt64:
			arg->m_data = new INT64;
			arg->setValue((int64_t)0);
			break;
		case TimeAct::kUInt64:
			arg->m_data = new UINT64;
			arg->setValue((uint64_t)0);
			break;
		default:
			throw("Invalid argument type");
			break;
		}

		return arg;
	}
	
	void Argument::destroy()
	{
		delete this->m_data;

		delete this;
	}

	std::string Argument::getAsString()
	{
		char buffer[256];
		bool value = false;

		switch (this->type)
		{
		case TimeAct::kBool:
			sprintf_s(buffer, "%d", getValue<bool>());
			break;
		case TimeAct::kByte:
			sprintf_s(buffer, "%d", getValue<INT8>());
			break;
		case TimeAct::kUByte:
			sprintf_s(buffer, "%d", getValue<UINT8>());
			break;
		case TimeAct::kShort:
			sprintf_s(buffer, "%d", getValue<INT16>());
			break;
		case TimeAct::kUShort:
			sprintf_s(buffer, "%d", getValue<UINT16>());
			break;
		case TimeAct::kInt:
			sprintf_s(buffer, "%d", getValue<INT32>());
			break;
		case TimeAct::kUInt:
			sprintf_s(buffer, "%d", getValue<UINT32>());
			break;
		case TimeAct::kFloat:
			sprintf_s(buffer, "%.3f", getValue<FLOAT>());
			break;
		case TimeAct::kInt64:
			sprintf_s(buffer, "%d", getValue<INT64>());
			break;
		case TimeAct::kUInt64:
			sprintf_s(buffer, "%d", getValue<UINT64>());
			break;
		default:
			throw("Invalid data type");
			break;
		}

		return std::string(buffer);
	}

	Event* Event::create(int start, int end, int userData)
	{
		Event* event = new Event;

		event->frameStart = start;
		event->frameEnd = end;
		event->userData = userData;

		return event;
	}

	Argument* Event::addArgument(TimeAct::DataType type, std::string name)
	{
		Argument* arg = Argument::create(type, name);

		this->arguments.push_back(arg);

		return arg;
	}

	bool Event::isEventActive(int currentFrame)
	{
		if ((currentFrame >= this->frameStart) && currentFrame <= (this->frameEnd))
			return true;

		return false;
	}

	void Event::destroy()
	{
		for (size_t i = 0; i < this->arguments.size(); i++)
			this->arguments[i]->destroy();

		this->arguments.clear();

		delete this;
	}

	std::string Event::getArgumentsString(int groupId, TimeAct::TaeTemplate* taeTemplate)
	{
		if (this->arguments.size() == 0)
			return "";

		std::string str = "(";

		for (size_t i = 0; i < this->arguments.size(); i++)
		{
			TimeAct::TaeTemplate::Group::Event::Arg argTemplate = taeTemplate->getEvent(groupId, this->userData)->args[i];

			if (!argTemplate.hidden)
			{
				str += this->arguments[i]->getAsString();

				if (i < this->arguments.size() - 1)
					str += ", ";
			}
		}

		str += ")";

		return str;
	}

	void Event::argsEditorGUI(int groupId, TimeAct::TaeTemplate* taeTemplate)
	{
		for (size_t i = 0; i < this->arguments.size(); i++)
		{
			TimeAct::TaeTemplate::Group::Event::Arg argTemplate = taeTemplate->getEvent(groupId, this->userData)->args[i];

			if (!argTemplate.hidden)
			{
				Argument* arg = this->arguments[i];

				TimeAct::DataType type = arg->type;
				std::string name = arg->name;

				if (argTemplate.values.size() == 0)
				{
					switch (type)
					{
					case TimeAct::kBool:
						ImGui::Checkbox(name.c_str(), static_cast<bool*>(arg->m_data));
						break;
					case TimeAct::kByte:
						ImGui::InputByte(name.c_str(), static_cast<ImS8*>(arg->m_data));
						break;
					case TimeAct::kUByte:
						ImGui::InputUByte(name.c_str(), static_cast<ImU8*>(arg->m_data));
						break;
					case TimeAct::kShort:
						ImGui::InputShort(name.c_str(), static_cast<ImS16*>(arg->m_data));
						break;
					case TimeAct::kUShort:
						ImGui::InputUShort(name.c_str(), static_cast<ImU16*>(arg->m_data));
						break;
					case TimeAct::kInt:
						ImGui::InputInt(name.c_str(), static_cast<ImS32*>(arg->m_data));
						break;
					case TimeAct::kUInt:
						ImGui::InputUInt(name.c_str(), static_cast<ImU32*>(arg->m_data));
						break;
					case TimeAct::kFloat:
						ImGui::InputFloat(name.c_str(), static_cast<float*>(arg->m_data), 0.001f, 0.01f);
						break;
					case TimeAct::kInt64:
						ImGui::InputInt64(name.c_str(), static_cast<ImS64*>(arg->m_data));
						break;
					case TimeAct::kUInt64:
						ImGui::InputUInt64(name.c_str(), static_cast<ImU64*>(arg->m_data));
						break;
					}
				}
				else
				{
					int idx = 0;

					switch (type)
					{
					case TimeAct::kByte:
						idx = getArgEnumValueIndex<int8_t>(argTemplate, arg->getValue<int8_t>());

						if (ImGui::BeginCombo(name.c_str(), argTemplate.values[idx].name.c_str()))
						{
							for (size_t j = 0; j < argTemplate.values.size(); j++)
							{
								ImGui::Selectable(argTemplate.values[j].name.c_str(), idx == j);

								if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(0))
									arg->setValue<int8_t>(argTemplate.values[j].value);
							}

							ImGui::EndCombo();
						}
						break;
					case TimeAct::kUByte:
						idx = getArgEnumValueIndex<uint8_t>(argTemplate, arg->getValue<uint8_t>());

						if (ImGui::BeginCombo(name.c_str(), argTemplate.values[idx].name.c_str()))
						{
							for (size_t j = 0; j < argTemplate.values.size(); j++)
							{
								ImGui::Selectable(argTemplate.values[j].name.c_str(), idx == j);

								if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(0))
									arg->setValue<uint8_t>(argTemplate.values[j].value);
							}

							ImGui::EndCombo();
						}
						break;
					case TimeAct::kShort:
						idx = getArgEnumValueIndex<int16_t>(argTemplate, arg->getValue<int16_t>());

						if (ImGui::BeginCombo(name.c_str(), argTemplate.values[idx].name.c_str()))
						{
							for (size_t j = 0; j < argTemplate.values.size(); j++)
							{
								ImGui::Selectable(argTemplate.values[j].name.c_str(), idx == j);

								if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(0))
									arg->setValue<int16_t>(argTemplate.values[j].value);
							}

							ImGui::EndCombo();
						}						
						break;
					case TimeAct::kUShort:
						idx = getArgEnumValueIndex<uint16_t>(argTemplate, arg->getValue<uint16_t>());

						if (ImGui::BeginCombo(name.c_str(), argTemplate.values[idx].name.c_str()))
						{
							for (size_t j = 0; j < argTemplate.values.size(); j++)
							{
								ImGui::Selectable(argTemplate.values[j].name.c_str(), idx == j);

								if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(0))
									arg->setValue<uint16_t>(argTemplate.values[j].value);
							}

							ImGui::EndCombo();
						}
						break;
					case TimeAct::kInt:
						idx = getArgEnumValueIndex<int32_t>(argTemplate, arg->getValue<int32_t>());

						if (ImGui::BeginCombo(name.c_str(), argTemplate.values[idx].name.c_str()))
						{
							for (size_t j = 0; j < argTemplate.values.size(); j++)
							{
								ImGui::Selectable(argTemplate.values[j].name.c_str(), idx == j);

								if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(0))
									arg->setValue<int32_t>(argTemplate.values[j].value);
							}

							ImGui::EndCombo();
						}
						break;
					case TimeAct::kUInt:
						idx = getArgEnumValueIndex<uint32_t>(argTemplate, arg->getValue<uint32_t>());

						if (ImGui::BeginCombo(name.c_str(), argTemplate.values[idx].name.c_str()))
						{
							for (size_t j = 0; j < argTemplate.values.size(); j++)
							{
								ImGui::Selectable(argTemplate.values[j].name.c_str(), idx == j);

								if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(0))
									arg->setValue<uint32_t>(argTemplate.values[j].value);
							}

							ImGui::EndCombo();
						}
						break;
					default:
						throw("Invalid argument data type for enumerator");
						break;
					}
				}
			}
		}	
	}
}