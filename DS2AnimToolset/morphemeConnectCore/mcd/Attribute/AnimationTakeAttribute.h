#pragma once
#include "Attribute.h"

namespace mcd
{
	class AnimationTakeAttribute : public Attribute
	{
		std::unique_ptr<db::StringAttribute> m_file;
		std::unique_ptr<db::StringAttribute> m_take;
		std::unique_ptr<db::StringAttribute> m_format;
		std::unique_ptr<db::StringAttribute> m_syncTrack;

	public:
		AnimationTakeAttribute(db::Attribute* parent, const std::string& name) : Attribute(parent, "AnimationTakeAttribute", name),
			m_file(std::make_unique<db::StringAttribute>(this, "File", "")),
			m_take(std::make_unique<db::StringAttribute>(this, "Take", "")),
			m_format(std::make_unique<db::StringAttribute>(this, "Format", "nsa")),
			m_syncTrack(std::make_unique<db::StringAttribute>(this, "SyncTrack", ""))
		{
			addAttribute(m_file.get());
			addAttribute(m_take.get());
			addAttribute(m_format.get());
			addAttribute(m_syncTrack.get());
		};

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