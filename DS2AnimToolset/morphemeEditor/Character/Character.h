#pragma once
#include "MorphemeSystem/MorphemeCharacter/MorphemeCharacter.h"
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
	static Character* createFromNmb(std::vector<std::wstring>& fileList, const char* filename);
	static Character* createFromTimeAct(const char* filename);

	CharacterModelCtrl* getCharacterModelCtrl() const { return this->m_characterModelCtrl; }
	MorphemeCharacter* getMorphemeCharacter() const { return this->m_morphemeCharacter; }
	MorphemeCharacterDef* getMorphemeCharacterDef() const { return this->m_morphemeCharacter->getCharacterDef(); }
	MR::Network* getMorphemeNetwork() const { return this->m_morphemeCharacter->getNetwork(); }
	MR::AnimRigDef* getRig(int idx) const { return this->getMorphemeCharacterDef()->getNetworkDef()->getRig(idx); }
	int getCharacterId() const { return this->m_chrId; }
	std::wstring getCharacterName() const { return this->m_characterName; }
	Vector3 getPosition() const { return this->m_position; }

	TimeAct::TaeExport::TimeActExportXML* getTimeAct() const { return this->m_timeAct; }

	void destroy();
	void update(float dt);
	void draw(RenderManager* renderManager);

	void loadTimeAct(const char* filename);
	void loadPartsFaceGenBnd(std::wstring root, FgPartType type, int id, bool female);
	void loadWeaponBnd(std::wstring root, PartType type, int id, bool shield);
	void loadPartsBnd(std::wstring root, PartType type, int id, bool female);

private:
	Character();
	~Character();

	int m_chrId;
	std::wstring m_characterName;
	MorphemeCharacter* m_morphemeCharacter;
	CharacterModelCtrl* m_characterModelCtrl;
	Vector3 m_position;
	TimeAct::TaeExport::TimeActExportXML* m_timeAct;
};