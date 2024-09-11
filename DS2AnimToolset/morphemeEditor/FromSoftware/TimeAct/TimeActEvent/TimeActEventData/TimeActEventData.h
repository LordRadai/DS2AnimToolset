#pragma once
#include <Windows.h>
#include "RCore.h"
#include "../../TaeTemplate/TaeTemplate.h"

namespace TimeAct
{
    class Argument
    {
    public:
        Argument() {}
        ~Argument() {}

        static Argument* create(DataType type, std::string name);
        static Argument* createFromResource(DataType type, std::string name, void* buffer);
        void destroy();

        DataType getType() const { return this->m_type; }
        std::string getName() const { return this->m_name; }
        void* getDataBuffer() const { return this->m_data; }

        template <typename T> T getValue() const
        {
            if (!m_data)
                throw std::runtime_error("this->m_data is null");

            switch (m_type)
            {
            case kBool:
                return getTypedValue<bool, T>();
            case kByte:
                return getTypedValue<int8_t, T>();
            case kUByte:
                return getTypedValue<uint8_t, T>();
            case kShort:
                return getTypedValue<int16_t, T>();
            case kUShort:
                return getTypedValue<uint16_t, T>();
            case kInt:
                return getTypedValue<int32_t, T>();
            case kUInt:
                return getTypedValue<uint32_t, T>();
            case kFloat:
                return getTypedValue<float, T>();
            case kInt64:
                return getTypedValue<int64_t, T>();
            case kUInt64:
                return getTypedValue<uint64_t, T>();
            default:
                throw std::runtime_error("Invalid type");
            }
        }

        template <typename T> void setValue(T v)
        {
            if (!m_data)
                throw std::runtime_error("this->m_data is null");

            switch (m_type)
            {
            case kBool:
                return setTypedValue<bool, T>(v);
            case kByte:
                return setTypedValue<int8_t, T>(v);
            case kUByte:
                return setTypedValue<uint8_t, T>(v);
            case kShort:
                return setTypedValue<int16_t, T>(v);
            case kUShort:
                return setTypedValue<uint16_t, T>(v);
            case kInt:
                return setTypedValue<int32_t, T>(v);
            case kUInt:
                return setTypedValue<uint32_t, T>(v);
            case kFloat:
                return setTypedValue<float, T>(v);
            case kInt64:
                return setTypedValue<int64_t, T>(v);
            case kUInt64:
                return setTypedValue<uint64_t, T>(v);
            default:
                throw std::runtime_error("Invalid type");
            }
        }

        int generateBinary(RFile* file);

    private:

        template <typename SourceType, typename TargetType>
        TargetType getTypedValue() const
        {
            if (typeid(SourceType) != typeid(TargetType))
                throw std::runtime_error("Type mismatch");

            return *reinterpret_cast<SourceType*>(m_data);
        }

        template <typename SourceType, typename TargetType>
        void setTypedValue(TargetType v)
        {
            if (typeid(SourceType) != typeid(TargetType))
                throw std::runtime_error("Type mismatch");

            *reinterpret_cast<SourceType*>(m_data) = v;
        }

        DataType m_type = kTypeInvalid;
        std::string m_name = "";
        void* m_data = nullptr;
    };

    class TimeActEventData
    {
        friend class EventData;
    public:
        TimeActEventData() {}
        ~TimeActEventData() {}

        static TimeActEventData* create(int groupId, int eventId, TaeTemplate* taeTemplate);
        static TimeActEventData* createFromResource(char* resource, int groupId, int eventId, TaeTemplate* taeTemplate);
        void destroy();

        int getNumArguments() const { return this->m_args.size(); }
        int getSize() const { return this->m_size; }
        Argument* getArgument(int idx) const { return this->m_args[idx]; }

        template <typename T> T getArgumentValue(int idx) const { return this->m_args[idx]->getValue<T>(); }

    protected:
        int getMemoryRequirements();
        int generateBinary(RFile* file);

        int m_size = 0;
        std::vector<Argument*> m_args;
    };
}