#pragma once
#include "../Attribute.h"

namespace mcd
{
	class AnimationTakeAttribute : public Attribute
	{
		std::unique_ptr<db::StringAttribute> m_file;
		std::unique_ptr<db::StringAttribute> m_take;
		std::unique_ptr<db::StringAttribute> m_format;
		std::unique_ptr<db::StringAttribute> m_syncTrack;

	public:
		AnimationTakeAttribute(db::CompositeAttribute* parent, const std::string& name);

		std::string getFile() const { return m_file->getValue(); }
		void setFile(const std::string& file) { m_file->setValue(file); }

		std::string getTake() const { return m_take->getValue(); }
		void setTake(const std::string& take) { m_take->setValue(take); }

		std::string getFormat() const { return m_format->getValue(); }
		void setFormat(const std::string& format) { m_format->setValue(format); }

		std::string getSyncTrack() const { return m_syncTrack->getValue(); }
		void setSyncTrack(const std::string& syncTrack) { m_syncTrack->setValue(syncTrack); }
	};
}