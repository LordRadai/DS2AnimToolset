#include "PlayerModelPreset.h"

PlayerModelPreset* PlayerModelPreset::loadFromFile(std::string path)
{
	PlayerModelPreset* preset = new PlayerModelPreset;
	preset->m_data = RINI::open(path);

	return preset;
}

bool PlayerModelPreset::isFemale()
{
	if (this->m_data)
		return this->m_data->getBool("Gender", "is_female", false);

	return false;
}

void PlayerModelPreset::setFemale(bool female)
{
	if (this->m_data)
		this->m_data->setBool("Gender", "is_female", female);
}

int PlayerModelPreset::getHeadId()
{
	if (this->m_data)
		return this->m_data->getInt("Armor", "head", -1);

	return -1;
}

int PlayerModelPreset::getFaceId()
{
	if (this->m_data)
		return this->m_data->getInt("Armor", "face", -1);

	return -1;
}

int PlayerModelPreset::getBodyId()
{
	if (this->m_data)
		return this->m_data->getInt("Armor", "body", -1);

	return -1;
}

int PlayerModelPreset::getArmId()
{
	if (this->m_data)
		return this->m_data->getInt("Armor", "arm", -1);

	return -1;
}

int PlayerModelPreset::getLegId()
{
	if (this->m_data)
		return this->m_data->getInt("Armor", "leg", -1);

	return -1;
}

void PlayerModelPreset::setHeadId(int id)
{
	if (this->m_data)
		this->m_data->setInt("Armor", "head", id);
}

void PlayerModelPreset::setFaceId(int id)
{
	if (this->m_data)
		this->m_data->setInt("Armor", "face", id);
}

void PlayerModelPreset::setBodyId(int id)
{
	if (this->m_data)
		this->m_data->setInt("Armor", "body", id);
}

void PlayerModelPreset::setArmId(int id)
{
	if (this->m_data)
		this->m_data->setInt("Armor", "arm", id);
}

void PlayerModelPreset::setLegId(int id)
{
	if (this->m_data)
		this->m_data->setInt("Armor", "leg", id);
}

int PlayerModelPreset::getFgFaceId()
{
	if (this->m_data)
		return this->m_data->getInt("FaceGen", "head", -1);

	return -1;
}

int PlayerModelPreset::getFgHeadId()
{
	if (this->m_data)
		return this->m_data->getInt("FaceGen", "head", -1);

	return -1;
}

int PlayerModelPreset::getFgEyeId()
{
	if (this->m_data)
		return this->m_data->getInt("FaceGen", "eye", -1);

	return -1;
}

int PlayerModelPreset::getFgEyeBrowsId()
{
	if (this->m_data)
		return this->m_data->getInt("FaceGen", "eye_brows", -1);

	return -1;
}

int PlayerModelPreset::getFgBeardId()
{
	if (this->m_data)
		return this->m_data->getInt("FaceGen", "beard", -1);

	return -1;
}

int PlayerModelPreset::getFgHairId()
{
	if (this->m_data)
		return this->m_data->getInt("FaceGen", "hair", -1);

	return -1;
}

void PlayerModelPreset::setFgFaceId(int id)
{
	if (this->m_data)
		this->m_data->setInt("FaceGen", "face", id);
}

void PlayerModelPreset::setFgHeadId(int id)
{
	if (this->m_data)
		this->m_data->setInt("FaceGen", "head", id);
}

void PlayerModelPreset::setFgEyeId(int id)
{
	if (this->m_data)
		this->m_data->setInt("FaceGen", "eye", id);
}

void PlayerModelPreset::setFgEyeBrowsId(int id)
{
	if (this->m_data)
		this->m_data->setInt("FaceGen", "eye_brows", id);
}

void PlayerModelPreset::setFgBeardId(int id)
{
	if (this->m_data)
		this->m_data->setInt("FaceGen", "beard", id);
}

void PlayerModelPreset::setFgHairId(int id)
{
	if (this->m_data)
		this->m_data->setInt("FaceGen", "hair", id);
}

int PlayerModelPreset::getRightHandEquipId()
{
	if (this->m_data)
		return this->m_data->getInt("Right", "id", -1);

	return -1;
}

bool PlayerModelPreset::isRightHandEquipShield()
{
	if (this->m_data)
		return this->m_data->getBool("Right", "is_shield", false);

	return false;
}

int PlayerModelPreset::getLeftHandEquipId()
{
	if (this->m_data)
		return this->m_data->getInt("Left", "id", -1);

	return -1;
}

bool PlayerModelPreset::isLeftHandEquipShield()
{
	if (this->m_data)
		return this->m_data->getBool("Left", "is_shield", false);

	return false;
}

void PlayerModelPreset::setRightHandEquipId(int id, bool shield)
{
	if (this->m_data)
	{
		this->m_data->setInt("Right", "id", id);
		this->m_data->setBool("Right", "is_shield", shield);
	}
}

void PlayerModelPreset::setLeftHandEquipId(int id, bool shield)
{
	if (this->m_data)
	{
		this->m_data->setInt("Left", "id", id);
		this->m_data->setBool("Left", "is_shield", shield);
	}
}

void PlayerModelPreset::save()
{
	if (this->m_data)
		this->m_data->write();
}