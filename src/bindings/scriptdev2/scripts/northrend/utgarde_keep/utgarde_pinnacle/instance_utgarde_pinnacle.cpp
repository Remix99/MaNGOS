/* Copyright (C) 2006 - 2012 ScriptDev2 <http://www.scriptdev2.com/>
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
SDName: instance_pinnacle
SD%Complete: 25%
SDComment:
SDCategory: Utgarde Pinnacle
EndScriptData */

#include "precompiled.h"
#include "utgarde_pinnacle.h"

instance_pinnacle::instance_pinnacle(Map* pMap) : ScriptedInstance(pMap)
{
    Initialize();
}

void instance_pinnacle::Initialize()
{
    memset(&m_auiEncounter, 0, sizeof(m_auiEncounter));
    for (uint8 i = 0; i < MAX_SPECIAL_ACHIEV_CRITS; ++i)
        m_abAchievCriteria[i] = false;

    m_uiMoveTimer = 0;
    m_uiGordokSubBossCount = 0;
    m_uiStep = 1;
    m_bNeedMovement = false;
}

void instance_pinnacle::OnObjectCreate(GameObject* pGo)
{
    switch(pGo->GetEntry())
    {
        case GO_DOOR_SKADI:
            if (m_auiEncounter[TYPE_SKADI] == DONE)
                pGo->SetGoState(GO_STATE_ACTIVE);
            break;
        case GO_DOOR_AFTER_YMIRON:
            break;
        default:
            return;
    }
    m_mGoEntryGuidStore[pGo->GetEntry()] = pGo->GetObjectGuid();
}

void instance_pinnacle::OnCreatureCreate(Creature* pCreature)
{
    switch(pCreature->GetEntry())
    {
        case NPC_GRAUF:
        case NPC_SKADI:
        case NPC_YMIRON:
        case NPC_FURBOLG:
        case NPC_WORGEN:
        case NPC_JORMUNGAR:
        case NPC_RHINO:
            m_mNpcEntryGuidStore[pCreature->GetEntry()] = pCreature->GetObjectGuid();
            break;
        case NPC_FLAME_BRAZIER:
            m_lFlameBraziersList.push_back(pCreature->GetObjectGuid());
            break;
        case NPC_FLAME_BREATH_TRIGGER:
        {
            if (pCreature->GetPositionY() < -512.0f)
                m_lFlameBreathTriggerLeft.push_back(pCreature->GetObjectGuid());
            else
                m_lFlameBreathTriggerRight.push_back(pCreature->GetObjectGuid());
            break;
        }
    }
}
void instance_pinnacle::SetData(uint32 uiType, uint32 uiData)
{
    switch (uiType)
    {
        case TYPE_SVALA:
            m_auiEncounter[uiType] = uiData;
            break;
        case TYPE_GORTOK:
            m_auiEncounter[uiType] = uiData;
            if (uiData == FAIL)
            {
                if (Creature* pFurbolg = GetSingleCreatureFromStorage(NPC_FURBOLG))
                {
                    pFurbolg->Respawn();
                    pFurbolg->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                }
                if (Creature* pWorg = GetSingleCreatureFromStorage(NPC_WORGEN))
                {
                    pWorg->Respawn();
                    pWorg->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                }
                if (Creature* pJormungar = GetSingleCreatureFromStorage(NPC_JORMUNGAR))
                {
                    pJormungar->Respawn();
                    pJormungar->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                }
                if (Creature* pRhino = GetSingleCreatureFromStorage(NPC_RHINO))
                {
                    pRhino->Respawn();
                    pRhino->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                }
            }
            break;
        case TYPE_SKADI:
            if (uiData == IN_PROGRESS)
            {
                DoStartTimedAchievement(ACHIEVEMENT_CRITERIA_TYPE_KILL_CREATURE, ACHIEV_START_SKADY_ID);
            }
            if (uiData == DONE)
                DoUseDoorOrButton(GO_DOOR_SKADI);

            m_auiEncounter[uiType] = uiData;
            break;
        case TYPE_YMIRON:
            m_auiEncounter[uiType] = uiData;
            if (uiData == DONE)
            {
                DoUseDoorOrButton(GO_DOOR_AFTER_YMIRON);
            }
            break;
        default:
            error_log("SD2: Instance Pinnacle: SetData = %u for type %u does not exist/not implemented.", uiType, uiData);
            return;
    }

    // Saving also SPECIAL for this instance
    if (uiData == DONE || uiData == SPECIAL)
    {
        OUT_SAVE_INST_DATA;

        std::ostringstream saveStream;
        saveStream << m_auiEncounter[0] << " " << m_auiEncounter[1] << " " << m_auiEncounter[2] << " " << m_auiEncounter[3];

        m_strInstData = saveStream.str();

        SaveToDB();
        OUT_SAVE_INST_DATA_COMPLETE;
    }
}

uint32 instance_pinnacle::GetData(uint32 uiType)
{
    if (uiType < MAX_ENCOUNTER)
        return m_auiEncounter[uiType];

    return 0;
}

void instance_pinnacle::OnCreatureDeath(Creature * pCreature)
{
    if (pCreature->GetEntry() == NPC_SCOURGE_HULK)
    {
        if (pCreature->HasAura(SPELL_AURA_RITUAL_STRIKE))
        {
            m_abAchievCriteria[TYPE_ACHIEV_THE_INCREDIBLE_HULK] = true;
        }
    }
    else if (IsCreatureGortokSubboss(pCreature->GetEntry()))
    {
        DoOrbAction();
    }
    else if (pCreature->GetEntry() == NPC_GORTOK)
    {
        SetData(TYPE_GORTOK, DONE); // orb despawn with next update
    }
}

bool instance_pinnacle::CheckAchievementCriteriaMeet(uint32 criteria_id, Player const* source, Unit const* target, uint32 miscvalue1)
{
    switch(criteria_id)
    {
        case ACHIEV_CRIT_KINGS_BANE:
        {
            return m_abAchievCriteria[TYPE_ACHIEV_KINGS_BANE];
        }
        case ACHIEV_CRIT_THE_INCREDIBLE_HULK:
        {
            return m_abAchievCriteria[TYPE_ACHIEV_THE_INCREDIBLE_HULK];
        }
        case ACHIEV_CRIT_MY_GIRL_LOVES_SKADI_ALL_THE_TIME:
        {
            return m_abAchievCriteria[TYPE_ACHIEV_MY_GIRL_LOVES_SKADI_ALL_THE_TIME];
        }
    }
    return false;
}

void instance_pinnacle::SetSpecialAchievementCriteria(uint32 uiType, bool bIsMet)
{
    if (uiType < MAX_SPECIAL_ACHIEV_CRITS)
        m_abAchievCriteria[uiType] = bIsMet;
}

void instance_pinnacle::Load(const char* chrIn)
{
    if (!chrIn)
    {
        OUT_LOAD_INST_DATA_FAIL;
        return;
    }

    OUT_LOAD_INST_DATA(chrIn);

    std::istringstream loadStream(chrIn);
    loadStream >> m_auiEncounter[0] >> m_auiEncounter[1] >> m_auiEncounter[2] >> m_auiEncounter[3];

    for(uint8 i = 0; i < MAX_ENCOUNTER; ++i)
    {
        if (m_auiEncounter[i] == IN_PROGRESS)
            m_auiEncounter[i] = NOT_STARTED;
    }

    OUT_LOAD_INST_DATA_COMPLETE;
}


void instance_pinnacle::DoProcessCallFlamesEvent()
{
    for (GuidList::const_iterator itr = m_lFlameBraziersList.begin(); itr != m_lFlameBraziersList.end(); ++itr)
    {
        if (Creature* pFlame = instance->GetCreature(*itr))
            pFlame->CastSpell(pFlame, SPELL_BALL_OF_FLAME, true);
    }
}

void instance_pinnacle::DoMakeFreezingCloud()
{
    if (urand(0, 1))
    {
        for (GuidList::const_iterator itr = m_lFlameBreathTriggerLeft.begin(); itr != m_lFlameBreathTriggerLeft.end(); ++itr)
        {
            if (Creature* pFlame = instance->GetCreature(*itr))
                pFlame->CastSpell(pFlame, SPELL_FREEZING_CLOUD, true);
        }
    }
    else
    {
        for (GuidList::const_iterator itr = m_lFlameBreathTriggerRight.begin(); itr != m_lFlameBreathTriggerRight.end(); ++itr)
        {
            if (Creature* pFlame = instance->GetCreature(*itr))
                pFlame->CastSpell(pFlame, SPELL_FREEZING_CLOUD, true);
        }
    }
}

void instance_pinnacle::OnCreatureEvade(Creature * pCreature)
{
    if (IsCreatureGortokSubboss(pCreature->GetEntry()) || pCreature->GetEntry() == NPC_GORTOK)
    {
        pCreature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
        SetData(TYPE_GORTOK, FAIL); // orb despawn with next update
    }
}

bool instance_pinnacle::IsCreatureGortokSubboss(uint32 entry)
{
    return entry >= NPC_WORGEN && entry <= NPC_RHINO;
}

void instance_pinnacle::Update(uint32 const uiDiff)
{
    if (Creature* pGortokOrb = instance->GetCreature(m_gortokOrb))
    {
        if (GetData(TYPE_GORTOK) != IN_PROGRESS)
        {
            pGortokOrb->ForcedDespawn();
            return;
        }

        if (m_bNeedMovement)
        {
            if (m_uiMoveTimer <= uiDiff)
            {
                switch (m_uiStep)
                {
                    case 1:
                    {
                        pGortokOrb->GetMotionMaster()->MovePoint(0, 238.61f, -460.71f, 109.57f + 4.0f, false);
                        m_uiMoveTimer = 4000;
                        ++m_uiStep;
                        break;
                    }
                    case 2:
                    {
                        pGortokOrb->GetMotionMaster()->MovePoint(1, 279.11f, -452.01f, 109.57f + 2.0f, false);
                        m_uiMoveTimer = 10000;
                        ++m_uiStep;
                        break;
                    }
                    case 3:
                    {
                        DoOrbAction();
                        m_bNeedMovement = false;
                        break;
                    }
                    default:  // should never appear
                    {
                        SetData(TYPE_GORTOK, FAIL);
                        break;
                    }
                }
            }
            else
                m_uiMoveTimer -= uiDiff;
        }
    }
}

void instance_pinnacle::InitializeOrb(Creature* pGortokOrb)
{
    m_gortokOrb = pGortokOrb->GetObjectGuid();
    m_uiMoveTimer = 4000;
    m_bNeedMovement = true;
    m_uiStep = 1;
    m_uiGordokSubBossCount = 0;
    pGortokOrb->SetLevitate(true);
    pGortokOrb->SetDisplayId(16925);
    pGortokOrb->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_NON_ATTACKABLE);
    pGortokOrb->CastSpell(pGortokOrb, SPELL_ORB_VISUAL, true);
}

void instance_pinnacle::DoOrbAction()
{
    if (Creature* pGortokOrb = instance->GetCreature(m_gortokOrb))
    {
        ++m_uiGordokSubBossCount;
        bool nextBoss = m_uiGordokSubBossCount > (instance->IsRegularDifficulty() ? 2 : 4);
        pGortokOrb->CastSpell(pGortokOrb, nextBoss ? SPELL_WAKEUP_GORTOK : SPELL_WAKEUP_SUBBOSS, false);
    }
}

InstanceData* GetInstanceData_instance_pinnacle(Map* pMap)
{
    return new instance_pinnacle(pMap);
}

bool ProcessEventId_event_start_gortok(uint32 uiEventId, Object* pSource, Object* pTarget, bool bIsStart)
{
    if (pSource->isType(TYPEMASK_WORLDOBJECT))
    {
        WorldObject* pObject = (WorldObject*)pSource;
        if (instance_pinnacle* m_pInstance = (instance_pinnacle*)pObject->GetInstanceData())
        {
            if (m_pInstance->GetData(TYPE_GORTOK) == NOT_STARTED || m_pInstance->GetData(TYPE_GORTOK) == FAIL)
            {
                m_pInstance->SetData(TYPE_GORTOK, IN_PROGRESS);
                if (Creature* pGortokOrb = pObject->SummonCreature(NPC_STASIS_CONTROLLER, 238.61f, -460.71f, 109.57f, 0, TEMPSUMMON_DEAD_DESPAWN, 0))
                {
                    m_pInstance->InitializeOrb(pGortokOrb);
                    return true;
                }
            }
        }
    }
    return false;
}

void AddSC_instance_pinnacle()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "instance_pinnacle";
    pNewScript->GetInstanceData = &GetInstanceData_instance_pinnacle;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "event_start_gortok";
    pNewScript->pProcessEventId= &ProcessEventId_event_start_gortok;
    pNewScript->RegisterSelf();
}
