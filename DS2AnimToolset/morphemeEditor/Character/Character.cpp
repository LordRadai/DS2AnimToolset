#include "Character.h"
#include "framework.h"
#include "extern.h"
#include "MorphemeEditorApp/MorphemeEditorApp.h"

namespace
{
    int getChrIdFromNmbFileName(std::wstring name)
    {
        std::wstring chrIdStr;
        int chrId = -1;

        int lastCPos = name.find_last_of(L"\\");

        if (name.substr(lastCPos + 1, 1).compare(L"c") != 0)
            return -1;

        chrIdStr = name.substr(lastCPos + 2, 4);

        chrId = stoi(chrIdStr);

        return chrId;
    }

    inline std::wstring generateCharacterName(int chrId)
    {
        wchar_t name[256];
        swprintf_s(name, L"c%04d", chrId);

        return std::wstring(name);
    }

    std::wstring findGamePath(std::wstring current_path)
    {
        std::filesystem::path gamepath = current_path;

        do
        {
            std::wstring parent_path = gamepath.parent_path();
            gamepath = parent_path;

            int lastDirPos = parent_path.find_last_of(L"\\");

            std::wstring folder = parent_path.substr(lastDirPos, parent_path.length());

            if (folder.compare(L"\\") == 0)
                return L"";

            if (folder.compare(L"\\Game") == 0)
                return gamepath;

        } while (true);

        return L"";
    }

    std::vector<std::wstring> getTaeFileListFromChrId(std::wstring tae_path, int chrId)
    {
        wchar_t chrIdStr[256];
        swprintf_s(chrIdStr, L"%04d", chrId);

        std::vector<std::wstring> files;

        g_appLog->debugMessage(MsgLevel_Debug, "TimeAct belonging to c%04d:\n", chrId);

        for (const auto& entry : std::filesystem::directory_iterator(tae_path))
        {
            if (entry.path().extension().compare(".tae") == 0)
            {
                std::wstring filename = entry.path().filename();
                std::wstring filenameChrId = filename.substr(1, 4);

                if (filenameChrId.compare(chrIdStr) == 0)
                {
                    g_appLog->debugMessage(MsgLevel_Debug, "\t%ws\n", filename.c_str());
                    files.push_back(entry.path());
                }
            }
        }

        if (files.size() == 0)
            g_appLog->alertMessage(MsgLevel_Debug, "Could not find any TimeAct files belonging to c%d in %ws\n", chrId, tae_path);

        return files;
    }

    std::wstring getModelNameFromChrId(std::wstring model_path, std::wstring m_chrId)
    {
        for (const auto& entry : std::filesystem::directory_iterator(model_path))
        {
            if (entry.path().extension().compare(".bnd") == 0)
            {
                std::wstring filename = entry.path().filename();
                std::wstring file_chr_id_str = filename.substr(1, 4);

                if (file_chr_id_str.compare(m_chrId) == 0)
                    return entry.path();
            }
        }

        return L"";
    }

    std::wstring getModelNameFromObjId(std::wstring model_path, std::wstring obj_id)
    {
        for (const auto& entry : std::filesystem::directory_iterator(model_path))
        {
            std::wstring filename = entry.path().filename();
            std::wstring file_chr_id_str = filename.substr(0, 8);

            if ((file_chr_id_str.compare(obj_id) == 0) && (entry.path().extension() == ".bnd"))
                return entry.path();
        }

        return L"";
    }

    void drawFlverModel(FlverModel* model, RenderManager* renderManager)
    {
        if (model == nullptr)
            return;

        model->draw(renderManager);
    }

    std::string extractTimeActFilePrefix(std::string filename)
    {
        std::string name = std::filesystem::path(filename).filename().string();

        if (name.at(0) == 'o')
            return filename;

        size_t underscorePos = filename.find_last_of('_');

        if (underscorePos != std::string::npos)
            return filename.substr(0, underscorePos);

        return "";
    }

    std::string extractTimeActFileSuffix(std::string filename) 
    {
        std::string name = std::filesystem::path(filename).filename().string();

        if (name.at(0) == 'o')
            return "";

        size_t underscorePos = filename.find('_');
        size_t dotPos = filename.find('.');

        if ((underscorePos != std::string::npos) && (dotPos != std::string::npos) && (dotPos > underscorePos))
            return filename.substr(underscorePos + 1, dotPos - underscorePos - 1);

        return "";
    }

    void writeArgumentValue(const TimeAct::Argument* arg, TimeAct::TaeExport::TimeActArgumentExportXML* argExport)
    {
        switch (arg->getType())
        {
        case TimeAct::kBool:
            argExport->setAsBool(arg->getValue<bool>());
            break;
        case TimeAct::kByte:
            argExport->setAsByte(arg->getValue<INT8>());
            break;
        case TimeAct::kUByte:
            argExport->setAsUByte(arg->getValue<UINT8>());
            break;
        case TimeAct::kShort:
            argExport->setAsShort(arg->getValue<INT16>());
            break;
        case TimeAct::kUShort:
            argExport->setAsUShort(arg->getValue<UINT16>());
            break;
        case TimeAct::kInt:
            argExport->setAsInt(arg->getValue<INT32>());
            break;
        case TimeAct::kUInt:
            argExport->setAsInt(arg->getValue<UINT32>());
            break;
        case TimeAct::kFloat:
            argExport->setAsFloat(arg->getValue<float>());
            break;
        case TimeAct::kInt64:
            argExport->setAsInt64(arg->getValue<INT64>());
            break;
        case TimeAct::kUInt64:
            argExport->setAsUInt64(arg->getValue<UINT64>());
            break;
        }
    }

    void addTimeActEventGroupsToXML(TimeAct::TimeActTrack* track, TimeAct::TaeExport::TimeActTrackExportXML* trackExport)
    {
        for (size_t groupIdx = 0; groupIdx < track->getNumEventGroups(); groupIdx++)
        {
            TimeAct::EventGroup* eventGroup = track->getEventGroup(groupIdx);

            TimeAct::TaeExport::TimeActGroupExportXML* groupExport = trackExport->addEventGroup(eventGroup->getGroupTypeID());

            for (size_t eventIdx = 0; eventIdx < eventGroup->getNumEvents(); eventIdx++)
            {
                TimeAct::TimeActEvent* event = eventGroup->getEvent(eventIdx);

                TimeAct::TaeExport::TimeActEventExportXML* eventExport = groupExport->addEvent(event->getStartTime(), event->getEndTime(), event->getEventValue());

                for (size_t argIdx = 0; argIdx < event->getNumArguments(); argIdx++)
                {
                    TimeAct::Argument* arg = event->getArgument(argIdx);

                    TimeAct::TaeExport::TimeActArgumentExportXML* argExport = eventExport->addArgument(arg->getName());

                    writeArgumentValue(arg, argExport);
                }
            }
        }
    }

    TimeAct::TimeAct* mergeTimeActFiles(TimeAct::TimeAct* taePl, TimeAct::TimeAct* taeSfx, TimeAct::TimeAct* taeSnd)
    {
        if (taePl == nullptr)
            return nullptr;

        TimeAct::TimeAct* mergedFile = TimeAct::TimeAct::create(taePl->getFileId(), taePl->getSibFileId(), taePl->getSkeletonFileId(), taePl->getSibName(), taePl->getSkeletonName(), g_taeTemplate);

        for (size_t trackIdx = 0; trackIdx < taePl->getNumTimeActTracks(); trackIdx++)
        {
            TimeAct::TimeActTrack* track = taePl->getTimeActTrack(trackIdx);

            TimeAct::TimeActTrack* mergedTrack = mergedFile->addTimeActTrack(track->getId(), track->getLenght(), track->getFps());

            for (size_t groupIdx = 0; groupIdx < track->getNumEventGroups(); groupIdx++)
            {
                TimeAct::EventGroup* group = track->getEventGroup(groupIdx);

                TimeAct::EventGroup* mergedGroup = mergedTrack->addGroup(group->getGroupTypeID());

                for (size_t eventIdx = 0; eventIdx < group->getNumEvents(); eventIdx++)
                {
                    TimeAct::TimeActEvent* event = group->getEvent(eventIdx);

                    TimeAct::TimeActEvent* mergedEvent = mergedGroup->addEvent(event->getStartTime(), event->getEndTime(), event->getEventValue(), g_taeTemplate);

                    for (size_t argIdx = 0; argIdx < event->getNumArguments(); argIdx++)
                    {
                        if (event->getNumArguments() != mergedEvent->getNumArguments())
                            g_appLog->panicMessage("Arg count mismath\n");

                        TimeAct::Argument* arg = event->getArgument(argIdx);

                        TimeAct::Argument* mergedArg = mergedEvent->getArgument(argIdx);

                        TimeAct::DataType type = arg->getType();

                        switch (type)
                        {
                        case TimeAct::kBool:
                            mergedArg->setValue(arg->getValue<bool>());
                            break;
                        case TimeAct::kByte:
                            mergedArg->setValue(arg->getValue<INT8>());
                            break;
                        case TimeAct::kUByte:
                            mergedArg->setValue(arg->getValue<UINT8>());
                            break;
                        case TimeAct::kShort:
                            mergedArg->setValue(arg->getValue<INT16>());
                            break;
                        case TimeAct::kUShort:
                            mergedArg->setValue(arg->getValue<UINT16>());
                            break;
                        case TimeAct::kInt:
                            mergedArg->setValue(arg->getValue<INT>());
                            break;
                        case TimeAct::kUInt:
                            mergedArg->setValue(arg->getValue<UINT>());
                            break;
                        case TimeAct::kFloat:
                            mergedArg->setValue(arg->getValue<float>());
                            break;
                        case TimeAct::kInt64:
                            mergedArg->setValue(arg->getValue<INT64>());
                            break;
                        case TimeAct::kUInt64:
                            mergedArg->setValue(arg->getValue<UINT64>());
                            break;
                        default:
                            break;
                        }
                    }
                }
            }
        }

        if (taeSfx != nullptr)
        {
            for (size_t trackIdx = 0; trackIdx < taeSfx->getNumTimeActTracks(); trackIdx++)
            {
                TimeAct::TimeActTrack* track = taeSfx->getTimeActTrack(trackIdx);

                TimeAct::TimeActTrack* mergedTrack = mergedFile->addTimeActTrack(track->getId(), track->getLenght(), track->getFps());

                for (size_t groupIdx = 0; groupIdx < track->getNumEventGroups(); groupIdx++)
                {
                    TimeAct::EventGroup* group = track->getEventGroup(groupIdx);

                    TimeAct::EventGroup* mergedGroup = mergedTrack->addGroup(group->getGroupTypeID());

                    for (size_t eventIdx = 0; eventIdx < group->getNumEvents(); eventIdx++)
                    {
                        TimeAct::TimeActEvent* event = group->getEvent(eventIdx);

                        TimeAct::TimeActEvent* mergedEvent = mergedGroup->addEvent(event->getStartTime(), event->getEndTime(), event->getEventValue(), g_taeTemplate);

                        for (size_t argIdx = 0; argIdx < event->getNumArguments(); argIdx++)
                        {
                            if (event->getNumArguments() != mergedEvent->getNumArguments())
                                g_appLog->panicMessage("Arg count mismath\n");

                            TimeAct::Argument* arg = event->getArgument(argIdx);

                            TimeAct::Argument* mergedArg = mergedEvent->getArgument(argIdx);

                            TimeAct::DataType type = arg->getType();

                            switch (type)
                            {
                            case TimeAct::kBool:
                                mergedArg->setValue(arg->getValue<bool>());
                                break;
                            case TimeAct::kByte:
                                mergedArg->setValue(arg->getValue<INT8>());
                                break;
                            case TimeAct::kUByte:
                                mergedArg->setValue(arg->getValue<UINT8>());
                                break;
                            case TimeAct::kShort:
                                mergedArg->setValue(arg->getValue<INT16>());
                                break;
                            case TimeAct::kUShort:
                                mergedArg->setValue(arg->getValue<UINT16>());
                                break;
                            case TimeAct::kInt:
                                mergedArg->setValue(arg->getValue<INT>());
                                break;
                            case TimeAct::kUInt:
                                mergedArg->setValue(arg->getValue<UINT>());
                                break;
                            case TimeAct::kFloat:
                                mergedArg->setValue(arg->getValue<float>());
                                break;
                            case TimeAct::kInt64:
                                mergedArg->setValue(arg->getValue<INT64>());
                                break;
                            case TimeAct::kUInt64:
                                mergedArg->setValue(arg->getValue<UINT64>());
                                break;
                            default:
                                break;
                            }
                        }
                    }
                }
            }
        }

        if (taeSnd != nullptr)
        {
            for (size_t trackIdx = 0; trackIdx < taeSnd->getNumTimeActTracks(); trackIdx++)
            {
                TimeAct::TimeActTrack* track = taeSnd->getTimeActTrack(trackIdx);

                TimeAct::TimeActTrack* mergedTrack = mergedFile->addTimeActTrack(track->getId(), track->getLenght(), track->getFps());

                for (size_t groupIdx = 0; groupIdx < track->getNumEventGroups(); groupIdx++)
                {
                    TimeAct::EventGroup* group = track->getEventGroup(groupIdx);

                    TimeAct::EventGroup* mergedGroup = mergedTrack->addGroup(group->getGroupTypeID());

                    for (size_t eventIdx = 0; eventIdx < group->getNumEvents(); eventIdx++)
                    {
                        TimeAct::TimeActEvent* event = group->getEvent(eventIdx);

                        TimeAct::TimeActEvent* mergedEvent = mergedGroup->addEvent(event->getStartTime(), event->getEndTime(), event->getEventValue(), g_taeTemplate);

                        for (size_t argIdx = 0; argIdx < event->getNumArguments(); argIdx++)
                        {
                            if (event->getNumArguments() != mergedEvent->getNumArguments())
                                g_appLog->panicMessage("Arg count mismath\n");

                            TimeAct::Argument* arg = event->getArgument(argIdx);

                            TimeAct::Argument* mergedArg = mergedEvent->getArgument(argIdx);

                            TimeAct::DataType type = arg->getType();

                            switch (type)
                            {
                            case TimeAct::kBool:
                                mergedArg->setValue(arg->getValue<bool>());
                                break;
                            case TimeAct::kByte:
                                mergedArg->setValue(arg->getValue<INT8>());
                                break;
                            case TimeAct::kUByte:
                                mergedArg->setValue(arg->getValue<UINT8>());
                                break;
                            case TimeAct::kShort:
                                mergedArg->setValue(arg->getValue<INT16>());
                                break;
                            case TimeAct::kUShort:
                                mergedArg->setValue(arg->getValue<UINT16>());
                                break;
                            case TimeAct::kInt:
                                mergedArg->setValue(arg->getValue<INT>());
                                break;
                            case TimeAct::kUInt:
                                mergedArg->setValue(arg->getValue<UINT>());
                                break;
                            case TimeAct::kFloat:
                                mergedArg->setValue(arg->getValue<float>());
                                break;
                            case TimeAct::kInt64:
                                mergedArg->setValue(arg->getValue<INT64>());
                                break;
                            case TimeAct::kUInt64:
                                mergedArg->setValue(arg->getValue<UINT64>());
                                break;
                            default:
                                break;
                            }
                        }
                    }
                }
            }
        }

        return mergedFile;
    }

    TimeAct::TaeExport::TimeActExportXML* createTimeActXML(std::string name, TimeAct::TimeAct* taePl, TimeAct::TimeAct* taeSfx, TimeAct::TimeAct* taeSnd)
    {
        TimeAct::TimeAct* mergedTae = mergeTimeActFiles(taePl, taeSfx, taeSnd);

        if (mergedTae == nullptr)
            return nullptr;

        TimeAct::TaeExport::TimeActExportXML* taeExport = TimeAct::TaeExport::TimeActExportXML::create("", name, mergedTae->getFileId());
        taeExport->setSibFile(taePl->getSibFileId(), RString::toNarrow(taePl->getSibName()));
        taeExport->setSkeletonFile(taePl->getSkeletonFileId(), RString::toNarrow(taePl->getSkeletonName()));

        TimeAct::TaeExport::TimeActTrackListExportXML* trackListExport = taeExport->createTrackList();

        for (size_t trackIdx = 0; trackIdx < mergedTae->getNumTimeActTracks(); trackIdx++)
        {
            TimeAct::TimeActTrack* track = mergedTae->getTimeActTrack(trackIdx);

            TimeAct::TaeExport::TimeActTrackExportXML* trackExport = trackListExport->addTrack(track->getId(), track->getFps(), track->getLenght());

            addTimeActEventGroupsToXML(track, trackExport);
        }

        mergedTae->destroy();

        return taeExport;
    }
}

Character::Character()
{
    this->m_characterModelCtrl = new CharacterModelCtrl();
}

Character::~Character()
{
}

Character* Character::createFromNmb(std::vector<std::wstring>& fileList, const char* filename)
{
    Character* character = new Character();

    MorphemeCharacterDef* characterDef = MorphemeSystem::createCharacterDef(filename, true);
    characterDef->loadAnimations();

    std::wstring animFolder = std::filesystem::path(filename).parent_path().c_str();
    int animCount = characterDef->getAnimFileLookUp()->getNumAnims();

    for (int i = 0; i < animCount; i++)
    {
        std::wstring animFileName = RString::toWide(characterDef->getAnimFileLookUp()->getFilename(i));
        std::wstring animFilePath = animFolder + L"\\" + animFileName;

        characterDef->addAnimation(RString::toNarrow(animFilePath.c_str()).c_str());
    }

    characterDef->sortAnimations();

    if (!characterDef)
        throw("Failed to create MorphemeCharacterDef instance (%s)", filename);

    character->m_morphemeCharacter = MorphemeCharacter::create(characterDef);

    if (!character->m_morphemeCharacter)
        throw("Failed to create MorphemeCharacter instance (%s)", filename);

    character->m_chrId = getChrIdFromNmbFileName(RString::toWide(filename));
    character->m_characterName = generateCharacterName(character->m_chrId);

    std::wstring gamePath = findGamePath(RString::toWide(filename));

    if (gamePath != L"")
    {
        std::wstring modelFolder = gamePath + L"\\model";
        std::wstring timeActFolder = gamePath + L"\\timeact";
        std::wstring chrFolder = modelFolder + L"\\chr\\";

        wchar_t modelName[256];
        swprintf_s(modelName, L"%ws\%ws.bnd", chrFolder.c_str(), character->m_characterName.c_str());

        character->m_characterModelCtrl->setModel(FlverModel::createFromBnd(modelName, characterDef->getNetworkDef()->getRig(0)));
        
        fileList = getTaeFileListFromChrId(timeActFolder + L"\\chr\\", character->m_chrId);

        ImGui::OpenPopup("Select TimeAct File");

        if (character->m_chrId == 1)
        {
            PlayerModelPreset* preset = MorphemeEditorApp::getInstance()->getPlayerModelPreset();

            std::wstring partsFolder = modelFolder + L"\\parts";

            bool female = preset->isFemale();

            character->loadPartsFaceGenBnd(partsFolder, kFgHair, preset->getFgHairId(), female);
            character->loadPartsFaceGenBnd(partsFolder, kFgFace, preset->getFgFaceId(), female);
            character->loadPartsFaceGenBnd(partsFolder, kFgHead, preset->getFgHeadId(), female);
            character->loadPartsFaceGenBnd(partsFolder, kFgEyeBrows, preset->getFgEyeBrowsId(), female);
            character->loadPartsFaceGenBnd(partsFolder, kFgEyes, preset->getFgEyeId(), female);
            character->loadPartsFaceGenBnd(partsFolder, kFgBeard, preset->getFgBeardId(), female);

            character->loadPartsBnd(partsFolder, kPartsHead, preset->getHeadId(), female);
            character->loadPartsBnd(partsFolder, kPartsBody, preset->getBodyId(), female);
            character->loadPartsBnd(partsFolder, kPartsArm, preset->getArmId(), female);
            character->loadPartsBnd(partsFolder, kPartsLeg, preset->getLegId(), female);

            character->loadWeaponBnd(partsFolder, kPartsWeaponLeft, preset->getLeftHandEquipId(), preset->isLeftHandEquipShield());
            character->loadWeaponBnd(partsFolder, kPartsWeaponRight, preset->getRightHandEquipId(), preset->isRightHandEquipShield());
        }
    }

    return character;
}

Character* Character::createFromTimeAct(const char* filename)
{
    Character* character = new Character;

    character->m_characterName = std::filesystem::path(filename).filename().replace_extension("");
    character->loadTimeAct(filename);

    return character;
}

void Character::update(float dt)
{
    if (this->m_characterModelCtrl)
        this->m_characterModelCtrl->update(dt);

    FlverModel* model = this->m_characterModelCtrl->getModel();

    if (model)
        this->m_position = Vector3::Transform(Vector3::Zero, model->getWorldMatrix());

    if (this->m_morphemeCharacter && this->m_morphemeCharacter->getCharacterDef()->simulateNetwork())
        this->m_morphemeCharacter->update(dt);
}

void Character::draw(RenderManager* renderManager)
{
    drawFlverModel(this->m_characterModelCtrl->getModel(), renderManager);
    
    drawFlverModel(this->m_characterModelCtrl->getModelPart(kPartsHead), renderManager);
    drawFlverModel(this->m_characterModelCtrl->getModelPart(kPartsFace), renderManager);
    drawFlverModel(this->m_characterModelCtrl->getModelPart(kPartsBody), renderManager);
    drawFlverModel(this->m_characterModelCtrl->getModelPart(kPartsLeg), renderManager);
    drawFlverModel(this->m_characterModelCtrl->getModelPart(kPartsArm), renderManager);
    drawFlverModel(this->m_characterModelCtrl->getModelPart(kPartsWeaponRight), renderManager);
    drawFlverModel(this->m_characterModelCtrl->getModelPart(kPartsWeaponLeft), renderManager);

    drawFlverModel(this->m_characterModelCtrl->getModelFg(kFgHead), renderManager);
    drawFlverModel(this->m_characterModelCtrl->getModelFg(kFgBeard), renderManager);
    drawFlverModel(this->m_characterModelCtrl->getModelFg(kFgEyeBrows), renderManager);
    drawFlverModel(this->m_characterModelCtrl->getModelFg(kFgEyes), renderManager);
    drawFlverModel(this->m_characterModelCtrl->getModelFg(kFgFace), renderManager);
    drawFlverModel(this->m_characterModelCtrl->getModelFg(kFgHair), renderManager);
}

void Character::destroy()
{
    if (this->m_characterModelCtrl)
        this->m_characterModelCtrl->destroy();

    if (this->m_timeAct)
        this->m_timeAct->destroy();

    delete this;
}

void Character::loadTimeAct(const char* filename)
{
    std::string prefix = extractTimeActFilePrefix(filename);
    std::string suffix = extractTimeActFileSuffix(filename);

    g_appLog->debugMessage(MsgLevel_Info, "Loading TimeAct files:\n", filename);

    if (suffix.compare("") == 0)
    {
        g_appLog->debugMessage(MsgLevel_Info, "\tLoading TimeAct \"%s\"\n", filename);
        TimeAct::TimeAct* timeAct = TimeAct::TimeAct::createFromFile(RString::toWide(filename), g_taeTemplate);

        this->m_timeAct = createTimeActXML(std::filesystem::path(prefix).filename().string(), timeAct, nullptr, nullptr);
    }
    else
    {
        std::string taePl = prefix + "_pl.tae";
        std::string taeSfx = prefix + "_sfx.tae";
        std::string taeSnd = prefix + "_snd.tae";

        g_appLog->debugMessage(MsgLevel_Info, "\tLoading TimeAct \"%s\"\n", taePl.c_str());
        TimeAct::TimeAct* timeActPl = TimeAct::TimeAct::createFromFile(RString::toWide(taePl), g_taeTemplate);
        
        g_appLog->debugMessage(MsgLevel_Info, "\tLoading TimeAct \"%s\"\n", taeSfx.c_str());
        TimeAct::TimeAct* timeActSfx = TimeAct::TimeAct::createFromFile(RString::toWide(taeSfx), g_taeTemplate);
        
        g_appLog->debugMessage(MsgLevel_Info, "\tLoading TimeAct \"%s\"\n", taeSnd.c_str());
        TimeAct::TimeAct* timeActSnd = TimeAct::TimeAct::createFromFile(RString::toWide(taeSnd), g_taeTemplate);

        this->m_timeAct = createTimeActXML(std::filesystem::path(prefix).filename().string(), timeActPl, timeActSfx, timeActSnd);
    }
}

void Character::loadPartsFaceGenBnd(std::wstring root, FgPartType type, int id, bool female)
{
    std::wstring filepath = root.c_str();
    wchar_t modelName[255] = { 0 };
    int fullId = 0;

    switch (type)
    {
    case kFgFace:
        if (female)
            swprintf_s(modelName, L"\\face\\fg_1001_f.bnd");
        else
            swprintf_s(modelName, L"\\face\\fg_1001_m.bnd");
        break;
    case kFgHead:
        if (female)
            swprintf_s(modelName, L"\\face\\fg_2001_f.bnd");
        else
            swprintf_s(modelName, L"\\face\\fg_2001_m.bnd");
        break;
    case kFgEyes:
        swprintf_s(modelName, L"\\face\\fg_3001_a.bnd");
        break;
    case kFgEyeBrows:
        fullId = 4000 + id;

        swprintf_s(modelName, L"\\face\\fg_%d_m.bnd", fullId);
        break;
    case kFgBeard:
        fullId = 5000 + id;

        if (!female)
            swprintf_s(modelName, L"\\face\\fg_%d_m.bnd", fullId);
        break;
    case kFgHair:
        fullId = 7000 + id;

        if (female)
            swprintf_s(modelName, L"\\face\\fg_%d_f.bnd", fullId);
        else
            swprintf_s(modelName, L"\\face\\fg_%d_m.bnd", fullId);

        break;
    default:
        break;
    }

    filepath += modelName;

    FlverModel* model = FlverModel::createFromBnd(filepath, this->m_morphemeCharacter->getNetwork()->getRig(0));

    this->m_characterModelCtrl->setModelFg(type, model);
}

void Character::loadWeaponBnd(std::wstring root, PartType type, int id, bool shield)
{
    std::wstring filepath = root.c_str();
    wchar_t modelName[255];

    if (shield)
        swprintf_s(modelName, L"\\shield\\sd_%d_m.bnd", id);
    else
        swprintf_s(modelName, L"\\weapon\\wp_%d_m.bnd", id);

    filepath += modelName;

    FlverModel* model = FlverModel::createFromBnd(filepath, this->m_morphemeCharacter->getNetwork()->getRig(0));

    if (model)
        this->m_characterModelCtrl->setModelPart(type, model);
}

void Character::loadPartsBnd(std::wstring root, PartType type, int id, bool female)
{
    std::wstring filepath = root.c_str();
    wchar_t modelName[255] = { 0 };

    switch (type)
    {
    case kPartsHead:
        if (female)
            swprintf_s(modelName, L"\\head\\hd_%d_f.bnd", id);
        else
            swprintf_s(modelName, L"\\head\\hd_%d_m.bnd", id);
        break;
    case kPartsFace:
        if (female)
            swprintf_s(modelName, L"\\face\\fc_%d_f.bnd", id);
        else
            swprintf_s(modelName, L"\\face\\fc_%d_m.bnd", id);
        break;
    case kPartsBody:
        if (female)
            swprintf_s(modelName, L"\\body\\bd_%d_f.bnd", id);
        else
            swprintf_s(modelName, L"\\body\\bd_%d_m.bnd", id);
        break;
    case kPartsArm:
        if (female)
            swprintf_s(modelName, L"\\arm\\am_%d_f.bnd", id);
        else
            swprintf_s(modelName, L"\\arm\\am_%d_m.bnd", id);
        break;
    case kPartsLeg:
        if (female)
            swprintf_s(modelName, L"\\leg\\lg_%d_f.bnd", id);
        else
            swprintf_s(modelName, L"\\leg\\lg_%d_m.bnd", id);
        break;
    default:
        break;
    }

    filepath += modelName;

    FlverModel* model = FlverModel::createFromBnd(filepath, this->m_morphemeCharacter->getNetwork()->getRig(0));

    this->m_characterModelCtrl->setModelPart(type, model);
}