#pragma once
#include <string>
#include "../Attribute.h"

namespace NodeEditor
{
	class AnimationTakeAttribute : public Attribute
	{
		std::string m_filename;
		std::string m_takeName;
		std::string m_syncTrack;
	public:
		AnimationTakeAttribute(Entity* owner, const std::string& name) : Attribute(owner, name, "animationTake"), m_filename(""), m_takeName(""), m_syncTrack("") {};

		virtual ~AnimationTakeAttribute() override {};
		virtual bool editorGUI() override;
		virtual void setValue(const std::vector<std::any>& values) override;

		const std::string& getFilename() const { return m_filename; }
		const std::string& getTakeName() const { return m_takeName; }
		const std::string& getSyncTrack() const { return m_syncTrack; }
	};
}
