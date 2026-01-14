#include "AnimationTakeAttribute.h"


namespace mcd
{
	AnimationTakeAttribute::AnimationTakeAttribute(db::Attribute* parent, const std::string& name) : Attribute(parent, "AnimationTakeAttribute", name),
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
}
