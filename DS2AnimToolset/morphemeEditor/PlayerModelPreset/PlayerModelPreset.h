#pragma once
#include "RCore.h"

class PlayerModelPreset
{
public:
	static PlayerModelPreset* loadFromFile(std::string path);

	bool isFemale();
	void setFemale(bool female);

	int getHeadId();
	int getFaceId();
	int getBodyId();
	int getArmId();
	int getLegId();

	void setHeadId(int id);
	void setFaceId(int id);
	void setBodyId(int id);
	void setArmId(int id);
	void setLegId(int id);

	int getFgFaceId();
	int getFgHeadId();
	int getFgEyeId();
	int getFgEyeBrowsId();
	int getFgBeardId();
	int getFgHairId();

	void setFgFaceId(int id);
	void setFgHeadId(int id);
	void setFgEyeId(int id);
	void setFgEyeBrowsId(int id);
	void setFgBeardId(int id);
	void setFgHairId(int id);

	int getRightHandEquipId();
	bool isRightHandEquipShield();

	int getLeftHandEquipId();
	bool isLeftHandEquipShield();

	void setRightHandEquipId(int id, bool shield);
	void setLeftHandEquipId(int id, bool shield);

	void save();
private:
	PlayerModelPreset() {}
	~PlayerModelPreset() {}

	RINI* m_data = nullptr;
};