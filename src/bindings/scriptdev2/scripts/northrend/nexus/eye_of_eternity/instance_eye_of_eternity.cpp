/* Copyright (C) 2006 - 2011 ScriptDev2 <https://scriptdev2.svn.sourceforge.net/>
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

/* ScriptData
 SDName: Instance_Eye_of_Eternity
 SD%Complete: 90%
 SDComment:
 SDAuthor: Tassadar
 SDCategory: Nexus, Eye of Eternity
 EndScriptData */

#include "precompiled.h"
#include "eye_of_eternity.h"
#include "Vehicle.h"

instance_eye_of_eternity::instance_eye_of_eternity(Map* pMap) :
        ScriptedInstance(pMap)
{
    difficulty = pMap->GetDifficulty();
    Initialize();
}

void instance_eye_of_eternity::Initialize()
{
    memset(&m_auiEncounter, 0, sizeof(m_auiEncounter));
}

void instance_eye_of_eternity::OnCreatureCreate(Creature* pCreature)
{
    switch (pCreature->GetEntry())
    {
        case NPC_MALYGOS:
            pCreature->SetActiveObjectState(true);
            /* no break */
        case NPC_SURGE_OF_POWER:
            m_mNpcEntryGuidStore[pCreature->GetEntry()] = pCreature->GetObjectGuid();
            break;
        case NPC_POWER_SPARK:
            m_lPowerSparkList.push_back(pCreature->GetObjectGuid());
            break;
        case NPC_ARCANE_OVERLOAD:
            m_lArcaneOverload.push_back(pCreature->GetObjectGuid());
            pCreature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
            pCreature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_OOC_NOT_ATTACKABLE | UNIT_FLAG_PASSIVE);
            break;
        case NPC_NEXUS_LORD:
            m_lNexusLord.push_back(pCreature->GetObjectGuid());
            break;
        case NPC_SCION_OF_ETERNITY:
            m_lScionOfEternity.push_back(pCreature->GetObjectGuid());
            break;
        case NPC_HOVER_DISK_MELEE:
        case NPC_HOVER_DISK_CASTER:
            m_lHoverDisk.push_back(pCreature->GetObjectGuid());
            break;
        case NPC_STATIC_FIELD:
            m_lStaticField.push_back(pCreature->GetObjectGuid());
            break;
        case NPC_SPARK_PORTAL:
            m_lSparkPortal.push_back(pCreature->GetObjectGuid());
            break;
        case NPC_VORTEX:
            m_lVortex.push_back(pCreature->GetObjectGuid());
            break;
        default:
            break;
    }
}

void instance_eye_of_eternity::OnObjectCreate(GameObject* pGo)
{
    switch (pGo->GetEntry())
    {
        case GO_PLATFORM:
        case GO_EXIT_PORTAL:
        case GO_FOCUSING_IRIS:
        case GO_FOCUSING_IRIS_H:
        case GO_ALEXSTRASZAS_GIFT:
        case GO_ALEXSTRASZAS_GIFT_H:
        case GO_HEART_OF_MAGIC:
        case GO_HEART_OF_MAGIC_H:
            m_mGoEntryGuidStore[pGo->GetEntry()] = pGo->GetObjectGuid();
            break;
    }
}

bool instance_eye_of_eternity::IsEncounterInProgress() const
{
    for (uint8 i = 0; i < MAX_ENCOUNTER; ++i)
        if (m_auiEncounter[i] == IN_PROGRESS)
            return true;

    return false;
}

void instance_eye_of_eternity::SetData(uint32 uiType, uint32 uiData)
{
    switch (uiType)
    {
        case TYPE_MALYGOS:
        {
            switch (uiData)
            {
                case FAIL:
                case NOT_STARTED:
                {
                    if (GameObject* pFocusingIris = GetSingleGameObjectFromStorage(difficulty == RAID_DIFFICULTY_10MAN_NORMAL ? GO_FOCUSING_IRIS : GO_FOCUSING_IRIS_H))
                    {
                        pFocusingIris->SetGoState(GO_STATE_READY);
                        pFocusingIris->SetPhaseMask(1, true);
                    }
                    if (GameObject* pExitPortal = GetSingleGameObjectFromStorage(GO_EXIT_PORTAL))
                        pExitPortal->SetPhaseMask(1, true);
                    if (GameObject* pPlatform = GetSingleGameObjectFromStorage(GO_PLATFORM))
                        pPlatform->RemoveFlag(GAMEOBJECT_FLAGS, GO_FLAG_DESTROYED);
                    DespawnCreatures(NPC_POWER_SPARK);
                    DespawnCreatures(NPC_ARCANE_OVERLOAD);
                    DespawnCreatures(NPC_NEXUS_LORD);
                    DespawnCreatures(NPC_SCION_OF_ETERNITY);
                    DespawnCreatures(NPC_HOVER_DISK_CASTER);
                    DespawnCreatures(NPC_STATIC_FIELD);
                    break;
                }
                case IN_PROGRESS:
                {
                    if (GameObject* pFocusingIris = GetSingleGameObjectFromStorage(difficulty == RAID_DIFFICULTY_10MAN_NORMAL ? GO_FOCUSING_IRIS : GO_FOCUSING_IRIS_H))
                        pFocusingIris->SetPhaseMask(2, true);
                    if (GameObject* pExitPortal = GetSingleGameObjectFromStorage(GO_EXIT_PORTAL))
                        pExitPortal->SetPhaseMask(2, true);
                    break;
                }
                case DONE:
                {
                    if (GameObject* pExitPortal = GetSingleGameObjectFromStorage(GO_EXIT_PORTAL))
                        pExitPortal->SetPhaseMask(1, true);
                    DoRespawnGameObject(difficulty == RAID_DIFFICULTY_10MAN_NORMAL ? GO_ALEXSTRASZAS_GIFT : GO_ALEXSTRASZAS_GIFT_H, HOUR * IN_MILLISECONDS);
                    DoRespawnGameObject(difficulty == RAID_DIFFICULTY_10MAN_NORMAL ? GO_HEART_OF_MAGIC : GO_HEART_OF_MAGIC_H, HOUR * IN_MILLISECONDS);
                    break;
                }
                default:
                    break;
            }
            m_auiEncounter[0] = uiData;
            break;
        }
        default:
            break;
    }
    OUT_SAVE_INST_DATA;
    std::ostringstream saveStream;
    saveStream << m_auiEncounter[TYPE_MALYGOS];

    strInstData = saveStream.str();
    SaveToDB();
    OUT_SAVE_INST_DATA_COMPLETE;

}

void instance_eye_of_eternity::Load(const char* chrIn)
{
    if (!chrIn)
    {
        OUT_LOAD_INST_DATA_FAIL;
        return;
    }

    OUT_LOAD_INST_DATA(chrIn);

    std::istringstream loadStream(chrIn);
    loadStream >> m_auiEncounter[0];

    for (uint8 i = 0; i < MAX_ENCOUNTER; ++i)
    {
        if (m_auiEncounter[i] == IN_PROGRESS)
            m_auiEncounter[i] = NOT_STARTED;
    }

    OUT_LOAD_INST_DATA_COMPLETE;
}

uint32 instance_eye_of_eternity::GetData(uint32 uiType)
{
    switch (uiType)
    {
        case TYPE_MALYGOS:
            return m_auiEncounter[TYPE_MALYGOS];
    }
    return 0;
}

void instance_eye_of_eternity::DespawnCreatures(uint32 uiEntry)
{
    switch (uiEntry)
    {
        case NPC_POWER_SPARK:
        {
            for (GuidList::iterator iter = m_lPowerSparkList.begin(); iter != m_lPowerSparkList.end(); ++iter)
            {
                if (Creature* pSpark = instance->GetCreature(*iter))
                    pSpark->ForcedDespawn();
            }
            m_lPowerSparkList.clear();
            break;
        }
        case NPC_ARCANE_OVERLOAD:
        {
            for (GuidList::iterator iter = m_lArcaneOverload.begin(); iter != m_lArcaneOverload.end(); ++iter)
            {
                if (Creature* pArcaneOverload = instance->GetCreature(*iter))
                    pArcaneOverload->ForcedDespawn();
            }
            m_lArcaneOverload.clear();
            break;
        }
        case NPC_NEXUS_LORD:
        {
            for (GuidList::iterator iter = m_lNexusLord.begin(); iter != m_lNexusLord.end(); ++iter)
            {
                if (Creature* pNexusLord = instance->GetCreature(*iter))
                    pNexusLord->ForcedDespawn();
            }
            m_lNexusLord.clear();
            break;
        }
        case NPC_SCION_OF_ETERNITY:
        {
            for (GuidList::iterator iter = m_lScionOfEternity.begin(); iter != m_lScionOfEternity.end(); ++iter)
            {
                if (Creature* pScionOfEternity = instance->GetCreature(*iter))
                    pScionOfEternity->ForcedDespawn();
            }
            m_lScionOfEternity.clear();
            break;
        }
        case NPC_HOVER_DISK_MELEE:
        case NPC_HOVER_DISK_CASTER:
        {
            for (GuidList::iterator iter = m_lHoverDisk.begin(); iter != m_lHoverDisk.end(); ++iter)
            {
                if (Creature* pHoverDisk = instance->GetCreature(*iter))
                    pHoverDisk->ForcedDespawn();
            }
            m_lHoverDisk.clear();
            break;
        }
        case NPC_STATIC_FIELD:
        {
            for (GuidList::iterator iter = m_lStaticField.begin(); iter != m_lStaticField.end(); ++iter)
            {
                if (Creature* pStaticField = instance->GetCreature(*iter))
                    pStaticField->ForcedDespawn();
            }
            m_lStaticField.clear();
            break;
        }
        default:
            break;
    }
}

void instance_eye_of_eternity::ActivateVisualOfVortex()
{
    for (GuidList::const_iterator iter = m_lVortex.begin(); iter != m_lVortex.end(); ++ iter)
    {
        if (Creature* pVortex = instance->GetCreature(*iter))
        {
            pVortex->CastSpell(pVortex, SPELL_VORTEX_VISUAL, false);
        }
    }
}

void instance_eye_of_eternity::DestroyVisualOfVortex(bool boom)
{
    for (GuidList::const_iterator iter = m_lVortex.begin(); iter != m_lVortex.end(); ++ iter)
    {
        if (Creature* pVortex = instance->GetCreature(*iter))
        {
            pVortex->CastSpell(pVortex, (boom ? SPELL_DESTROY_PLATFORM_BOOM : SPELL_DESTROY_PLATFORM_PRE), false);
        }
    }
}

void instance_eye_of_eternity::HandleRiderOfVortex(Unit* pTarget)
{
    for (GuidList::const_iterator iter = m_lVortex.begin(); iter != m_lVortex.end(); ++iter)
    {
        if (Creature* pVortex = instance->GetCreature(*iter))
        {
            if (VehicleKit* pVehicle = pVortex->GetVehicleKit())
            {
                if (pVehicle->GetNextEmptySeatWithFlag(0) != -1)
                {
                    pTarget->CastSpell(pTarget, SPELL_VORTEX_RIDE_AURA, true);
                    return;
                }
            }
        }
    }
}

//TODO: better count down
bool instance_eye_of_eternity::IsAnyAddAtLife()
{
    for (GuidList::const_iterator iter = m_lNexusLord.begin(); iter != m_lNexusLord.end(); ++iter)
    {
        if (Creature* pNexusLord = instance->GetCreature(*iter))
        {
            if (pNexusLord->isAlive())
                return true;
        }
    }
    for (GuidList::const_iterator iter = m_lScionOfEternity.begin(); iter != m_lScionOfEternity.end(); ++iter)
    {
        if (Creature* pScionOfEternity = instance->GetCreature(*iter))
        {
            if (pScionOfEternity->isAlive())
                return true;
        }
    }
    return false;
}

InstanceData* GetInstanceData_instance_eye_of_eternity(Map* pMap)
{
    return new instance_eye_of_eternity(pMap);
}

void AddSC_instance_eye_of_eternity()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "instance_eye_of_eternity";
    pNewScript->GetInstanceData = &GetInstanceData_instance_eye_of_eternity;
    pNewScript->RegisterSelf();
}
