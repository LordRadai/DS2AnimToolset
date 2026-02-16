#pragma once
#include "CharacterMotionCtrl/CharacterMotionCtrl.h"
#include "CharacterModelCtrl/CharacterModelCtrl.h"
#include "FromSoftware/TimeAct/TaeExport/TaeExport.h"
#include "RCore.h"

enum TimeActSlot
{
	kTimeActPl,
	kTimeActSfx,
	kTimeActSnd,
	kNumTimeActSlots
};

class Character
{
public:
	static Character* createFromMorphemeBundle(std::vector<std::wstring>& fileList, const char* filename, bool doSimulateNetwork);
	static Character* createFromTimeAct(const char* filename);

	CharacterModelCtrl* getCharacterModelCtrl() const { return this->m_characterModelCtrl; }
	CharacterMotionCtrlAnimPreview* getCharacterMotionCtrl() const { return this->m_characterMotionCtrl; }

	int getCharacterId() const { return this->m_chrId; }
	std::wstring getCharacterName() const { return this->m_characterName; }
	Matrix getPosition() const { return this->m_position; }
	void setPosition(const Matrix& position) { this->m_position = position; }

	bool getEnableRootMotion() const { return this->m_enableRootMotion; }
	void setEnableRootMotion(bool enable) { this->m_enableRootMotion = enable; }

	TimeAct::TaeExport::TimeActExportXML* getTimeAct() const { return this->m_timeAct; }

	void destroy();
	void update(float dt);
	void draw(RenderManager* renderManager);

	void loadTimeAct(const char* filename);
	void loadPartsFaceGenBnd(std::wstring root, FgPartType type, int id, bool female);
	void loadWeaponBnd(std::wstring root, PartType type, int id, bool shield);
	void loadPartsBnd(std::wstring root, PartType type, int id, bool female);

private:
	Character() {}
	~Character() {}

	int m_chrId = -1;
	std::wstring m_characterName = L"";
	CharacterMotionCtrlAnimPreview* m_characterMotionCtrl = nullptr;
	CharacterModelCtrl* m_characterModelCtrl = nullptr;
	Matrix m_position = Matrix::Identity;
	TimeAct::TaeExport::TimeActExportXML* m_timeAct = nullptr;
	bool m_enableRootMotion = true;
};