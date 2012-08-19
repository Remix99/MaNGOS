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
SDName: Boss_Malygos
SD%Complete: 98%
SDComment:
Some Vehicle-Core problems with need improvments:
    Hover Disks are unsummon, if player leave it
    Dragon spell handling. Healing other player is a little bit strange
Possible improvements:
    Make a entry in vehicle_accessory for hover disk 30234 and 30248 with passengers Nexus Lord and Scion of Eternity
Other things:
    Remove spell_dbc hack for spell 56046
SDAuthor: Reamer + Tasssadar + PSZ + some corrections from MaxXx2021 and /dev/rsa
SDCategory: Nexus, Eye of Eternity
EndScriptData */

#include "precompiled.h"
#include "eye_of_eternity.h"
#include "Vehicle.h"

enum
{
    // ******************************** SPELLS ******************************** //
    SPELL_BERSERK                   = 64238,

    //////////////// PHASE 0 ////////////////
    SPELL_PORTAL_BEAM               = 56046,
    SPELL_PORTAL_OPEN               = 61236, // not used
    SPELL_PORTAL_CLOSE              = 55949, // not used

    //////////////// PHASE 1 ////////////////
    SPELL_ARCANE_BREATH             = 56272,
    SPELL_ARCANE_BREATH_H           = 60072,
    SPELL_ARCANE_STORM              = 61693,
    SPELL_ARCANE_STORM_H            = 61694,
    SPELL_POWER_SPARK_MAIN_SUMMON   = 56140, // limit to one target in spell.cpp and spell_script_target with target 30118
    SPELL_POWER_SPARK_SUMMON        = 56142,
    SPELL_POWER_SPARK               = 56152, // if spark reach malygos then buff him
    SPELL_POWER_SPARK_PLAYERS       = 55852, // if spark is killed - buff players in area
    SPELL_POWER_SPARK_VISUAL        = 55845,

    SPELL_VORTEX_SPAWN              = 59670, // visual spawn effect for vortex
    SPELL_VORTEX                    = 56105,
    SPELL_VORTEX_CHANNEL            = 56237, // Malygos Channel Effect

    //////////////// PHASE 2 ////////////////
    SPELL_ARCANE_BOMB_SUMMON        = 56429, // target with spell_script_target-table to trigger npc 22517
    SPELL_ARCANE_BOMB_MISSILE       = 56430,
    SPELL_ARCANE_BOMB_DAMAGE        = 56431, // cast by arcane overload
    SPELL_ARCANE_OVERLOAD           = 56432, // cast this on arcane overload NPCs
    SPELL_SURGE_OF_POWER_BREATH     = 56505, // omfg, they say deep breath, but its this!

    //NPCs spells
    SPELL_ARCANE_SHOCK              = 57058,
    SPELL_ARCANE_SHOCK_H            = 60073,
    SPELL_HASTE                     = 57060,
    SPELL_ARCANE_BARRAGE            = 50804, // hack, right spell is 56397

    //////////////// PHASE 3 ////////////////
    SPELL_STATIC_FIELD_MISSILE      = 57430, // TODO: maybe limit to 1 in spell.cpp
    SPELL_STATIC_FIELD              = 57428,
    SPELL_SURGE_OF_POWER            = 57407, // this is on one target
    SPELL_SURGE_OF_POWER_H          = 60936, // this is on unlimited tagets, must limit it in mangos
    SPELL_ARCANE_PULSE              = 57432,

    SPELL_ALEXSTRASZAS_GIFT_BEAM    = 61028,
    SPELL_ALEXSTRASZAS_GIFT_VISUAL  = 61023,

    SPELL_CALL_SKYTALON             = 58846,
    SPELL_RIDE_RED_DRAGON_BUDDY     = 56071,

    SAY_INTRO1                      = -1616000,
    SAY_INTRO2                      = -1616001,
    SAY_INTRO3                      = -1616002,
    SAY_INTRO4                      = -1616003,
    SAY_INTRO5                      = -1616004,
    SAY_INTRO_PHASE3                = -1616018,
    SAY_AGGRO1                      = -1616005,
    SAY_AGGRO2                      = -1616013,
    SAY_AGGRO3                      = -1616019,
    SAY_VORTEX                      = -1616006,
    SAY_POWER_SPARK                 = -1616035,
    SAY_POWER_SPARK_BUFF            = -1616007,
    SAY_KILL1_1                     = -1616008,
    SAY_KILL1_2                     = -1616009,
    SAY_KILL1_3                     = -1616010,
    SAY_KILL2_1                     = -1616020,
    SAY_KILL2_2                     = -1616021,
    SAY_KILL2_3                     = -1616022,
    SAY_KILL3_1                     = -1616023,
    SAY_KILL3_2                     = -1616024,
    SAY_KILL3_3                     = -1616025,
    SAY_END_PHASE1                  = -1616012,
    SAY_END_PHASE2                  = -1616017,
    SAY_ARCANE_PULSE                = -1616014,
    SAY_ARCANE_PULSE_WARN           = -1616015,
    SAY_ARCANE_OVERLOAD             = -1616016,
    SAY_SURGE_OF_POWER              = -1616026,
    SAY_CAST_SPELL1                 = -1616027,
    SAY_CAST_SPELL2                 = -1616028,
    SAY_CAST_SPELL3                 = -1616029,
    SAY_DEATH                       = -1616030,
    // Alexstrasza
    SAY_OUTRO1                      = -1616031,
    SAY_OUTRO2                      = -1616032,
    SAY_OUTRO3                      = -1616033,
    SAY_OUTRO4                      = -1616034,

    PLATFORM_MIN_X                  = 722,
    PLATFORM_MAX_X                  = 768,
    PLATFORM_MIN_Y                  = 1290,
    PLATFORM_MAX_Y                  = 1339,

    NEXUS_LORD_COUNT                = 2,
    NEXUS_LORD_COUNT_H              = 4,
    SCION_OF_ETERNITY_COUNT         = 4,
    SCION_OF_ETERNITY_COUNT_H       = 8,

    POINT_ID_WAYPOINT               = 1,
    POINT_ID_LAND                   = 2,
    POINT_ID_VORTEX_AIR             = 3,
    POINT_ID_PHASE_2_WP             = 4,
    POINT_ID_DEEP_BREATH            = 5,

    //hacks
    SPELL_FLIGHT                    = 59553,
    MODEL_ID_INVISIBLE              = 11686
};

enum MalygosPhase
{
    PHASE_INTRO                     = 0,
        SUBPHASE_INTRO              = 0,
    PHASE_FLOOR                     = 1,
        SUBPHASE_NOT_VORTEX         = 10,
        SUBPHASE_VORTEX             = 11,
    PHASE_ADDS                      = 2,
        SUBPHASE_NOT_DEEP_BREATH    = 20, // maybe better name SUBPHASE_NOT_SURGE_OF_POWER
        SUBPHASE_FLY                = 21,
        SUBPHASE_DEEP_BREATH        = 22, // maybe better name SUBPHASE_SURGE_OF_POWER
    PHASE_DRAGONS                   = 3,
        SUBPHASE_NOT_SURGE_OF_POWER = 30,
        SUBPHASE_DESTROY_PLATFORM_1 = 31,
        SUBPHASE_DESTROY_PLATFORM_2 = 32,
        SUBPHASE_DESTROY_PLATFORM_3 = 33,
        SUBPHASE_DESTROY_PLATFORM_4 = 34,
        SUBPHASE_SURGE_OF_POWER     = 35,

};

struct LocationsXY
{
    float x, y;
};

static LocationsXY WPs[]=
{
    {810.0f, 1248.0f},
    {810.0f, 1358.0f},
    {700.0f, 1358.0f},
    {700.0f, 1248.0f},
};

#define CENTER_X                754.50f
#define CENTER_Y                1301.76f
#define FLOOR_Z                 266.17f
#define AIR_Z                   296.17f

/*######
## boss_malygos
######*/

struct MANGOS_DLL_DECL boss_malygosAI : public ScriptedAI
{
    boss_malygosAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (instance_eye_of_eternity*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    instance_eye_of_eternity* m_pInstance;
    bool m_bIsRegularMode;

    MalygosPhase m_Phase;
    MalygosPhase m_SubPhase;

    uint8 m_uiSpeechCount;
    uint32 m_uiSpeechTimer;

    uint32 m_uiTimer;
    uint32 m_uiEnrageTimer;
    uint32 m_uiVortexTimer;
    uint32 m_uiVortexStep;
    uint32 m_uiArcaneBreathTimer;
    uint32 m_uiPowerSparkTimer;
    uint32 m_uiDeepBreathTimer;
    uint32 m_uiShellTimer;
    uint32 m_uiArcaneStormTimer;
    uint32 m_uiStaticFieldTimer;
    uint32 m_uiArcanePulseTimer;
    uint32 m_uiSurgeOfPowerTimer;
    uint32 m_uiCheckTimer;
    uint32 m_uiMovingSteps;

    uint8 m_uiWP;
    uint8 m_uiAddDieCounter;

    bool m_bReadyForWPMove;

    void Reset()
    {
        m_uiMovingSteps = 0;
        m_bReadyForWPMove = true;
        m_Phase = PHASE_INTRO;
        m_SubPhase = SUBPHASE_INTRO;
        m_uiTimer = 0;
        m_uiEnrageTimer = 600000;
        m_uiArcaneBreathTimer = urand(13000, 16000);
        m_uiArcaneStormTimer = urand(10000, 15000);
        m_uiVortexTimer = 60000;
        m_uiVortexStep = 0;
        m_uiPowerSparkTimer = urand(20000, 30000);
        m_uiDeepBreathTimer = 65000;
        m_uiShellTimer = 0;
        m_uiStaticFieldTimer = 10000;
        m_uiArcanePulseTimer = 1000;
        m_uiSurgeOfPowerTimer = 8000;
        m_uiCheckTimer = 1000;

        m_uiWP = 0;
        m_uiAddDieCounter = 0;
        m_uiSpeechCount = 0;
        m_uiSpeechTimer = 15000;
        m_creature->SetSpeedRate(MOVE_RUN, 2.85714f);
        m_creature->SetSpeedRate(MOVE_WALK, 6.0f);
        m_creature->SetSpeedRate(MOVE_FLIGHT, 2.0f);
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        MakeBossFlight();
        m_creature->GetMotionMaster()->Clear();
        if (m_pInstance)
            m_pInstance->SetData(TYPE_MALYGOS, NOT_STARTED);
    }

    void MakeBossFlight()
    {
        m_creature->SetByteValue(UNIT_FIELD_BYTES_1, 3, UNIT_BYTE1_FLAG_ALWAYS_STAND | UNIT_BYTE1_FLAG_HOVER);
        m_creature->SetLevitate(true);
    }

    void MakeBossWalk()
    {
        m_creature->SetByteValue(UNIT_FIELD_BYTES_1, 3, 0);
        m_creature->SetLevitate(false);
    }

    void Aggro(Unit* pWho)
    {
        DoScriptText(SAY_AGGRO1, m_creature);
        if (m_pInstance)
            m_pInstance->SetData(TYPE_MALYGOS, IN_PROGRESS);
    }

    void JustDied(Unit* pKiller)
    {
        DoScriptText(SAY_DEATH, m_creature);
        m_creature->SummonCreature(NPC_ALEXSTRASZA, CENTER_X+20.0f, CENTER_Y+20.0f, AIR_Z, 0, TEMPSUMMON_DEAD_DESPAWN, 0);
        m_creature->GetMap()->CreatureRelocation(m_creature, m_creature->GetPositionX(), m_creature->GetPositionY(), FLOOR_Z-500.0f, 0);
        m_creature->MonsterMoveWithSpeed(m_creature->GetPositionX(), m_creature->GetPositionY(), FLOOR_Z-400.0f, 26);
    }

    void KilledUnit(Unit* pVictim)
    {
        uint8 uiText = 0;
        switch (m_Phase)
        {
            case PHASE_FLOOR: uiText = urand(0, 2); break;
            case PHASE_ADDS: uiText = urand(3, 5); break;
            case PHASE_DRAGONS: uiText = urand(6, 8); break;
            default:
                break;
        }
        switch (uiText)
        {
            case 0: DoScriptText(SAY_KILL1_1, m_creature); break;
            case 1: DoScriptText(SAY_KILL1_2, m_creature); break;
            case 2: DoScriptText(SAY_KILL1_3, m_creature); break;
            case 3: DoScriptText(SAY_KILL2_1, m_creature); break;
            case 4: DoScriptText(SAY_KILL2_2, m_creature); break;
            case 5: DoScriptText(SAY_KILL2_3, m_creature); break;
            case 6: DoScriptText(SAY_KILL3_1, m_creature); break;
            case 7: DoScriptText(SAY_KILL3_2, m_creature); break;
            case 8: DoScriptText(SAY_KILL3_3, m_creature); break;
            default:
                break;
        }
    }

    void SpellHit(Unit* pCaster, const SpellEntry* pSpell)
    {
        switch (pSpell->Id)
        {
            case SPELL_POWER_SPARK:
                DoScriptText(SAY_POWER_SPARK_BUFF, m_creature);
                break;
            default:
                break;
        }
    }

    void SpellHitTarget(Unit* pTarget, const SpellEntry* pSpell)
    {
        switch (pSpell->Id)
        {
            case SPELL_ARCANE_BOMB_MISSILE:
            {
                if (pTarget->GetEntry() == NPC_ARCANE_OVERLOAD)
                {
                    pTarget->CastSpell(pTarget, SPELL_ARCANE_BOMB_DAMAGE, false);
                    pTarget->CastSpell(pTarget, SPELL_ARCANE_OVERLOAD, false);
                    ((Creature*)pTarget)->ForcedDespawn(45000);
                }
                break;
            }
            case SPELL_VORTEX:
            {
                m_pInstance->HandleRiderOfVortex(pTarget);
                break;
            }
            default:
                break;
        }
    }

    void JustSummoned(Creature* pSummoned)
    {
        switch (pSummoned->GetEntry())
        {
            case NPC_STATIC_FIELD:
            {
                pSummoned->CastSpell(m_creature, SPELL_STATIC_FIELD, false, 0, 0, m_creature->GetObjectGuid());
                pSummoned->ForcedDespawn(30000);
                pSummoned->SetDisplayId(MODEL_ID_INVISIBLE);
                break;
            }
            case NPC_NEXUS_LORD:
            {
                if (Creature* pDisk = pSummoned->SummonCreature(NPC_HOVER_DISK_MELEE, pSummoned->GetPositionX(), pSummoned->GetPositionY(), pSummoned->GetPositionZ(), 0, TEMPSUMMON_CORPSE_DESPAWN, 0))
                {
                    pDisk->SetLevitate(true);
                    pDisk->CastSpell(pDisk, SPELL_FLIGHT, true);
                    if (VehicleKit* pDiskVehicle = pDisk->GetVehicleKit())
                        pSummoned->EnterVehicle(pDiskVehicle, 0);
                }
                pSummoned->SetInCombatWithZone();
                break;
            }
            case NPC_SCION_OF_ETERNITY:
            {
                if (Creature* pDisk = pSummoned->SummonCreature(NPC_HOVER_DISK_CASTER, pSummoned->GetPositionX(), pSummoned->GetPositionY(), pSummoned->GetPositionZ(), 0, TEMPSUMMON_CORPSE_DESPAWN, 0))
                {
                    pDisk->SetLevitate(true);
                    pDisk->CastSpell(pDisk, SPELL_FLIGHT, true);
                    if (VehicleKit* pDiskVehicle = pDisk->GetVehicleKit())
                        pSummoned->EnterVehicle(pDiskVehicle, 0);
                }
                pSummoned->SetInCombatWithZone();
                break;
            }
            case NPC_POWER_SPARK:
            {
                // maybe some functions
                break;
            }
            case NPC_ARCANE_OVERLOAD:
            {
                DoCast(pSummoned, SPELL_ARCANE_BOMB_MISSILE, true);
                break;
            }
            default:
                break;
        }
    }

    void SummonedCreatureJustDied(Creature* pSummoned)
    {
        switch (pSummoned->GetEntry())
        {
            case NPC_NEXUS_LORD:
            case NPC_SCION_OF_ETERNITY:
            {
                ++m_uiAddDieCounter;
                break;
            }
            default:
                break;
        }
    }

    void MovementInform(uint32 uiType, uint32 uiPointId)
    {
        if (uiType != POINT_MOTION_TYPE)
            return;

        switch(uiPointId)
        {
            case POINT_ID_WAYPOINT:
            {
                if (Creature* pSparkPortal = GetClosestCreatureWithEntry(m_creature, NPC_SPARK_PORTAL, 100.0f))
                {
                    m_creature->SetFacingToObject(pSparkPortal);
                    DoCast(pSparkPortal, SPELL_PORTAL_BEAM, true);
                }
                m_bReadyForWPMove = true;
                m_uiTimer = 15000;
                break;
            }
            case POINT_ID_LAND:
            {
                MakeBossWalk();
                m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                m_Phase = PHASE_FLOOR;
                m_SubPhase = SUBPHASE_NOT_VORTEX;
                m_creature->SetInCombatWithZone();
                if (Unit* pTarget = m_creature->getVictim())
                    m_creature->GetMotionMaster()->MoveChase(pTarget);
                break;
            }
            case POINT_ID_VORTEX_AIR:
            {
                m_pInstance->ActivateVisualOfVortex();
                DoCast(m_creature, SPELL_VORTEX);
                DoCast(m_creature, SPELL_VORTEX_CHANNEL);
                m_uiVortexTimer = 10 * IN_MILLISECONDS;
                m_uiVortexStep = 1;
                break;
            }
            case POINT_ID_PHASE_2_WP:
            {
                if (m_Phase == PHASE_ADDS)
                {
                    m_bReadyForWPMove = true;
                    m_uiTimer = 0;
                    if (m_SubPhase == SUBPHASE_FLY)
                    {
                        DoScriptText(SAY_AGGRO2, m_creature);
                        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                        m_uiShellTimer = urand(2000, 3000);
                        m_SubPhase = SUBPHASE_NOT_DEEP_BREATH;
                        for (uint8 i = 0; i < (m_bIsRegularMode ? NEXUS_LORD_COUNT : NEXUS_LORD_COUNT_H); ++i)
                            m_creature->SummonCreature(NPC_NEXUS_LORD, urand(PLATFORM_MIN_X, PLATFORM_MAX_X), urand(PLATFORM_MIN_Y, PLATFORM_MAX_Y), FLOOR_Z+10.0f+urand(0, 15), 0, TEMPSUMMON_CORPSE_DESPAWN, 0);
                        for (uint8 i = 0; i < (m_bIsRegularMode ? SCION_OF_ETERNITY_COUNT : SCION_OF_ETERNITY_COUNT_H); ++i)
                            m_creature->SummonCreature(NPC_SCION_OF_ETERNITY, urand(PLATFORM_MIN_X, PLATFORM_MAX_X), urand(PLATFORM_MIN_Y, PLATFORM_MAX_Y), FLOOR_Z+10.0f+urand(0, 15), 0, TEMPSUMMON_CORPSE_DESPAWN, 0);
                        m_uiTimer = 5000;
                    }
                }
                break;
            }
            case POINT_ID_DEEP_BREATH:
            {
                if (m_Phase == PHASE_ADDS)
                {
                    DoScriptText(SAY_ARCANE_PULSE, m_creature);
                    DoScriptText(SAY_ARCANE_PULSE_WARN, m_creature);
                    DoCast(m_creature, SPELL_SURGE_OF_POWER_BREATH);
                    m_uiShellTimer = urand(2000, 4000);
                    m_SubPhase = SUBPHASE_NOT_DEEP_BREATH;
                    m_bReadyForWPMove = true;
                    m_uiTimer = 10000;
                }
                break;
            }
            default:
                break;
        }
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (m_Phase != PHASE_INTRO)
        {
            if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
                return;

            if (m_uiEnrageTimer <= uiDiff)
            {
                SetCombatMovement(true);
                DoCast(m_creature, SPELL_BERSERK, true);
                m_uiEnrageTimer = 600000;
                m_creature->GetMotionMaster()->Clear(false, true);
                m_creature->GetMotionMaster()->MoveChase(m_creature->getVictim());
            }
            else
                m_uiEnrageTimer -= uiDiff;
        }

        switch (m_Phase)
        {
            case PHASE_INTRO:
                if (m_creature->GetMotionMaster()->GetCurrentMovementGeneratorType() != POINT_MOTION_TYPE)
                {
                    if (m_bReadyForWPMove)
                    {
                        if (m_uiTimer <= uiDiff)
                        {
                            if (m_pInstance->GetData(TYPE_MALYGOS) == IN_PROGRESS) // variable set with gameobject focusing iris Script
                            {
                                float fAng = m_creature->GetAngle(CENTER_X, CENTER_Y);
                                m_creature->GetMotionMaster()->MovePoint(POINT_ID_LAND, CENTER_X-40.76f*cos(fAng), CENTER_Y-40.76f*sin(fAng), FLOOR_Z+7.0f);
                            }
                            else
                            {
                                m_creature->GetMotionMaster()->MovePoint(POINT_ID_WAYPOINT, WPs[m_uiWP].x, WPs[m_uiWP].y, AIR_Z);
                            }
                            m_bReadyForWPMove = false;
                            ++m_uiWP;
                            if (m_uiWP == 4)
                                m_uiWP = 0;
                        }
                        else
                            m_uiTimer -= uiDiff;
                    }

                    if (m_uiSpeechTimer <= uiDiff)
                    {
                        switch(m_uiSpeechCount)
                        {
                            case 0:
                            {
                                DoScriptText(SAY_INTRO1, m_creature);
                                m_uiSpeechTimer = 22000;
                                break;
                            }
                            case 1:
                            {
                                DoScriptText(SAY_INTRO2, m_creature);
                                m_uiSpeechTimer = 24000;
                                break;
                            }
                            case 2:
                            {
                                DoScriptText(SAY_INTRO3, m_creature);
                                m_uiSpeechTimer = 26000;
                                break;
                            }
                            case 3:
                            {
                                DoScriptText(SAY_INTRO4, m_creature);
                                m_uiSpeechTimer = 23000;
                                break;
                            }
                            case 4:
                            {
                                DoScriptText(SAY_INTRO5, m_creature);
                                m_uiSpeechTimer = 120000;
                                break;
                            }
                        }
                        ++m_uiSpeechCount;
                        if (m_uiSpeechCount == 5)
                            m_uiSpeechCount = 0;
                    }
                    else
                        m_uiSpeechTimer -= uiDiff;
                }
                break;
            case PHASE_FLOOR:
            {
                switch (m_SubPhase)
                {
                    case SUBPHASE_VORTEX:
                    {
                        if (m_uiVortexTimer < uiDiff)
                        {
                            switch (m_uiVortexStep)
                            {
                                case 0:             // fly - should never appear, but maybe a fly point never reached
                                    m_creature->MonsterSay("Fly time is to short", LANG_UNIVERSAL);
                                    m_uiVortexTimer = 500;
                                    break;
                                case 1:             // remove Aura
                                    m_creature->RemoveAurasDueToSpell(SPELL_VORTEX_CHANNEL);
                                    m_uiVortexTimer = 500;
                                    break;
                                case 2:             // land
                                    MakeBossWalk();
                                    m_uiVortexTimer = 500;
                                    break;
                                case 3:             // end
                                    m_SubPhase = SUBPHASE_NOT_VORTEX;
                                    m_creature->GetMotionMaster()->Clear();
                                    SetCombatMovement(true);
                                    if (Unit* pTarget = m_creature->getVictim())
                                        m_creature->GetMotionMaster()->MoveChase(pTarget);
                                    m_uiVortexTimer = 50 * IN_MILLISECONDS;
                                    m_uiPowerSparkTimer = urand(2000, 5000);
                                    break;
                                default:
                                    m_creature->MonsterSay("Unknow VortexStep", LANG_UNIVERSAL);
                                    break;

                            }
                            ++m_uiVortexStep;
                        }
                        else
                            m_uiVortexTimer -= uiDiff;

                        break;
                    }
                    case SUBPHASE_NOT_VORTEX:
                    {
                        if (m_uiArcaneBreathTimer <= uiDiff)
                        {
                            if (DoCastSpellIfCan(m_creature, m_bIsRegularMode ? SPELL_ARCANE_BREATH : SPELL_ARCANE_BREATH_H) == CAST_OK)
                                m_uiArcaneBreathTimer = urand(13000, 16000);
                        }
                        else
                            m_uiArcaneBreathTimer -= uiDiff;

                        if (m_uiArcaneStormTimer <= uiDiff)
                        {
                            if (DoCastSpellIfCan(m_creature, m_bIsRegularMode ? SPELL_ARCANE_STORM : SPELL_ARCANE_STORM_H) == CAST_OK)
                                m_uiArcaneStormTimer = urand(10000, 15000);
                        }
                        else
                            m_uiArcaneStormTimer -= uiDiff;

                        if (m_uiPowerSparkTimer <= uiDiff)
                        {
                            if (DoCastSpellIfCan(m_creature, SPELL_POWER_SPARK_MAIN_SUMMON) == CAST_OK)
                            {
                                m_uiPowerSparkTimer = urand(20000, 30000);
                                DoScriptText(SAY_POWER_SPARK, m_creature);
                            }
                        }
                        else
                            m_uiPowerSparkTimer -= uiDiff;

                        if (m_uiVortexTimer <= uiDiff)
                        {
                            MakeBossFlight();
                            m_creature->GetMotionMaster()->Clear();
                            SetCombatMovement(false);
                            m_creature->GetMotionMaster()->MovePoint(POINT_ID_VORTEX_AIR, CENTER_X, CENTER_Y, AIR_Z);
                            m_SubPhase = SUBPHASE_VORTEX;
                            m_uiVortexStep = 0;
                            m_uiVortexTimer = 5000;
                            DoScriptText(SAY_VORTEX, m_creature);
                            return;
                        }
                        else
                            m_uiVortexTimer -= uiDiff;

                        // switch to next Phase (Add Phase)
                        if (m_creature->GetHealthPercent() <= 50.0f)
                        {
                            DoScriptText(SAY_END_PHASE1, m_creature);
                            m_Phase = PHASE_ADDS;
                            m_SubPhase = SUBPHASE_FLY ;
                            MakeBossFlight();
                            m_creature->GetMotionMaster()->Clear();
                            SetCombatMovement(false);
                            m_creature->GetMotionMaster()->MovePoint(0, CENTER_X, CENTER_Y, AIR_Z);
                            m_bReadyForWPMove = false;
                            m_uiTimer = 23000;
                            return;
                        }

                        DoMeleeAttackIfReady();
                        break;
                    }
                    default:
                        m_creature->MonsterSay("Unknown SubPhase in Floor Phase", LANG_UNIVERSAL);
                }
                break;
            }
            case PHASE_ADDS:
            {
                switch (m_SubPhase)
                {
                    case SUBPHASE_FLY :
                    {
                        if (m_uiTimer <= uiDiff)
                        {
                            m_uiWP = urand(0, 3);
                            m_creature->GetMotionMaster()->MovePoint(POINT_ID_PHASE_2_WP, WPs[m_uiWP].x, WPs[m_uiWP].y, AIR_Z);
                            m_uiTimer = 15000;
                        }
                        else
                            m_uiTimer -= uiDiff;
                        return;
                    }
                    case SUBPHASE_DEEP_BREATH:
                    {
                        break;
                    }
                    case SUBPHASE_NOT_DEEP_BREATH:
                    {
                        if (m_uiShellTimer <= uiDiff)
                        {
                            if (DoCastSpellIfCan(m_creature, SPELL_ARCANE_BOMB_SUMMON) == CAST_OK)
                            {
                                if (!urand(0, 3))
                                    DoScriptText(SAY_ARCANE_OVERLOAD, m_creature);
                                m_uiShellTimer = urand(15000, 17000);
                                m_bReadyForWPMove = true;
                                m_uiTimer = 2000;
                            }
                        }
                        else
                            m_uiShellTimer -= uiDiff;

                        if (m_uiDeepBreathTimer <= uiDiff)
                        {
                            m_creature->GetMotionMaster()->MovePoint(POINT_ID_DEEP_BREATH, CENTER_X, CENTER_Y, AIR_Z+20.0f);
                            m_bReadyForWPMove = false;
                            m_uiTimer = 20000;
                            m_uiDeepBreathTimer = 63000;
                            m_SubPhase = SUBPHASE_DEEP_BREATH;
                        }
                        else
                            m_uiDeepBreathTimer -= uiDiff;

                        break;
                    }
                    default:
                        break;
                }

                if (m_uiCheckTimer <= uiDiff)
                {
                    uint8 uiAddAmount  = m_bIsRegularMode ? (NEXUS_LORD_COUNT + SCION_OF_ETERNITY_COUNT): (NEXUS_LORD_COUNT_H + SCION_OF_ETERNITY_COUNT_H);
                    if (m_uiAddDieCounter == uiAddAmount)
                    {
                        // for save work
                        //if (!m_pInstance->IsAnyAddAtLife())
                        //{
                            m_Phase = PHASE_DRAGONS;
                            m_SubPhase = SUBPHASE_DESTROY_PLATFORM_1;
                            DoScriptText(SAY_END_PHASE2, m_creature);
                            m_pInstance->DestroyVisualOfVortex(false);
                            m_uiTimer = 6000;
                        //}
                    }

                    Map* pMap = m_creature->GetMap();
                    if (pMap)
                    {
                        Map::PlayerList const &lPlayers = pMap->GetPlayers();
                        for (Map::PlayerList::const_iterator itr = lPlayers.begin(); itr != lPlayers.end(); ++itr)
                        {
                            Player* pPlayer = itr->getSource();
                            if (pPlayer->GetVehicle() && m_Phase == PHASE_ADDS)
                                pPlayer->ApplySpellImmune(0, IMMUNITY_SCHOOL, SPELL_SCHOOL_MASK_ARCANE, true);
                            else
                                pPlayer->ApplySpellImmune(0, IMMUNITY_SCHOOL, SPELL_SCHOOL_MASK_ARCANE, false);
                        }
                    }

                    if (m_Phase == PHASE_DRAGONS)
                        return;

                    m_uiCheckTimer = 1000;
                }
                else
                    m_uiCheckTimer -= uiDiff;

                if (m_bReadyForWPMove)
                {
                    if (m_uiTimer <= uiDiff)
                    {
                        m_creature->GetMotionMaster()->Clear();
                        if (m_SubPhase == SUBPHASE_DEEP_BREATH)
                        {
                            m_uiWP = urand(0, 3);
                            m_creature->GetMotionMaster()->MovePoint(POINT_ID_PHASE_2_WP, WPs[m_uiWP].x, WPs[m_uiWP].y, AIR_Z);
                        }
                        else
                        {
                            float fAng = M_PI/4 + m_creature->GetAngle(CENTER_X, CENTER_Y);
                            m_creature->GetMotionMaster()->MovePoint(POINT_ID_PHASE_2_WP, CENTER_X-80.0f*cos(fAng), CENTER_Y-80.0f*sin(fAng), AIR_Z);
                        }
                        m_SubPhase = SUBPHASE_NOT_DEEP_BREATH;
                        m_bReadyForWPMove = false;
                    }
                    else
                        m_uiTimer -= uiDiff;
                }
                break;
            }
            case PHASE_DRAGONS:
            {
                switch (m_SubPhase)
                {
                    case SUBPHASE_DESTROY_PLATFORM_1:
                    {
                        if (m_uiTimer <= uiDiff)
                        {
                            m_pInstance->DespawnCreatures(NPC_ARCANE_OVERLOAD);
                            m_pInstance->DespawnCreatures(NPC_HOVER_DISK_CASTER);
                            DoCast(m_creature, SPELL_CALL_SKYTALON, true);
                            m_pInstance->DestroyVisualOfVortex(true);
                            m_creature->SetInCombatWithZone();
                            m_SubPhase = SUBPHASE_DESTROY_PLATFORM_2;
                            m_uiTimer = 3000;
                        }
                        else
                            m_uiTimer -= uiDiff;

                        return;
                    }
                    case SUBPHASE_DESTROY_PLATFORM_2:
                    {
                        if (m_uiTimer <= uiDiff)
                        {
                            if (m_pInstance)
                                if (GameObject* pPlatform = m_pInstance->GetSingleGameObjectFromStorage(GO_PLATFORM))
                                    pPlatform->SetFlag(GAMEOBJECT_FLAGS, GO_FLAG_DESTROYED);

                            m_creature->GetMotionMaster()->MovePoint(0, CENTER_X, CENTER_Y, FLOOR_Z);
                            DoScriptText(SAY_INTRO_PHASE3, m_creature);

                            m_SubPhase = SUBPHASE_DESTROY_PLATFORM_3;
                            m_uiTimer = 3000;
                        }
                        else
                            m_uiTimer -= uiDiff;

                        return;
                    }
                    case SUBPHASE_DESTROY_PLATFORM_3:
                    {
                        if (m_uiTimer<= uiDiff)
                        {
                            m_SubPhase = SUBPHASE_DESTROY_PLATFORM_4;
                            m_uiTimer = 12000;
                        }
                        else
                            m_uiTimer -= uiDiff;

                        return;
                    }
                    case SUBPHASE_DESTROY_PLATFORM_4:
                    {
                        if (m_uiTimer <= uiDiff)
                        {
                            m_SubPhase = SUBPHASE_NOT_SURGE_OF_POWER;
                            m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                            DoScriptText(SAY_AGGRO3, m_creature);
                            m_uiArcaneStormTimer = 6000;
                        }
                        else
                            m_uiTimer -= uiDiff;

                        return;
                    }
                    case  SUBPHASE_SURGE_OF_POWER:
                    {
                        if (m_uiTimer <= uiDiff)
                            m_SubPhase = SUBPHASE_NOT_SURGE_OF_POWER;
                        else
                            m_uiTimer -= uiDiff;
                        return;
                    }
                    case SUBPHASE_NOT_SURGE_OF_POWER:
                    {
                        if (m_uiArcanePulseTimer <= uiDiff)
                        {
                            DoCast(m_creature, SPELL_ARCANE_PULSE);
                            m_uiArcanePulseTimer = 1000;
                        }
                        else
                            m_uiArcanePulseTimer -= uiDiff;

                        if (m_uiArcaneStormTimer <= uiDiff)
                        {
                            DoCast(m_creature, m_bIsRegularMode ? SPELL_ARCANE_STORM : SPELL_ARCANE_STORM_H);
                            m_uiArcaneStormTimer = urand(6000, 10000);
                        }
                        else
                            m_uiArcaneStormTimer -= uiDiff;

                        if (m_uiStaticFieldTimer <= uiDiff)
                        {
                            for (uint8 i = 0; i<=50; ++i)
                            {
                                if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                                    if (pTarget->GetEntry() == NPC_WYRMREST_SKYTALON)
                                    {
                                        switch (urand(0, 5))
                                        {
                                            case 0: DoScriptText(SAY_CAST_SPELL1, m_creature); break;
                                            case 1: DoScriptText(SAY_CAST_SPELL2, m_creature); break;
                                            case 2: DoScriptText(SAY_CAST_SPELL3, m_creature); break;
                                        }
                                        DoCast(pTarget, SPELL_STATIC_FIELD_MISSILE);
                                        break;
                                    }
                            }

                            m_uiStaticFieldTimer = urand(10000, 16000);
                        }
                        else
                            m_uiStaticFieldTimer -= uiDiff;

                        if (m_uiSurgeOfPowerTimer <= uiDiff)
                        {
                            for (uint8 i = 0; i<=50; ++i)
                            {
                                if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                                    if (pTarget->GetEntry() == NPC_WYRMREST_SKYTALON)
                                    {
                                        m_SubPhase = SUBPHASE_SURGE_OF_POWER;
                                        m_uiTimer = 6500;
                                        if (urand(0, 1))
                                            DoScriptText(SAY_SURGE_OF_POWER, m_creature);
                                        DoCast(pTarget, m_bIsRegularMode ? SPELL_SURGE_OF_POWER : SPELL_SURGE_OF_POWER_H);
                                        break;
                                    }
                            }

                            m_uiSurgeOfPowerTimer = urand(5000, 15000);
                        }
                        else
                            m_uiSurgeOfPowerTimer -= uiDiff;
                        break;
                    }
                    default:
                        m_creature->MonsterSay("Unknown SubPhase in MainPhase Dragon", LANG_UNIVERSAL);
                        break;
                }
                break;
            }
            default:
                m_creature->MonsterSay("Unknown MainPhase", LANG_UNIVERSAL);
                break;
        }
    }
};

/*######
## npc_power_spark
######*/
struct MANGOS_DLL_DECL npc_power_sparkAI : public ScriptedAI
{
    npc_power_sparkAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (instance_eye_of_eternity*)pCreature->GetInstanceData();
        Reset();
    }

    instance_eye_of_eternity* m_pInstance;
    uint32 m_uiCheckTimer;

    void Reset()
    {
        m_uiCheckTimer = 2000;
        SetCombatMovement(false);
        m_creature->SetLevitate(true);
        m_creature->CastSpell(m_creature, SPELL_POWER_SPARK_VISUAL, false);
        m_creature->SetRespawnDelay(DAY);
    }

    void JustDied(Unit* pKiller)
    {
        DoCast(m_creature, SPELL_POWER_SPARK_PLAYERS, true);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (m_uiCheckTimer <= uiDiff)
        {
            if (m_pInstance)
                if (Creature* pMalygos = m_pInstance->GetSingleCreatureFromStorage(NPC_MALYGOS))
                {
                    m_creature->GetMotionMaster()->Clear();
                    if (m_creature->IsWithinDist(pMalygos, 5.0f, false))
                    {
                        m_creature->CastSpell(pMalygos, SPELL_POWER_SPARK, true);
                        m_creature->ForcedDespawn(100);
                    }
                    else
                    {
                        m_creature->GetMotionMaster()->MovePoint(0, pMalygos->GetPositionX(), pMalygos->GetPositionY(), pMalygos->GetPositionZ(), false);
                    }
                }

            m_uiCheckTimer = 2000;
        }
        else
            m_uiCheckTimer -= uiDiff;
    }
};

/*######
## npc_nexus_lord
######*/

struct MANGOS_DLL_DECL npc_nexus_lordAI : public ScriptedAI
{
    npc_nexus_lordAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    bool m_bIsRegularMode;
    uint32 m_uiArcaneShockTimer;
    uint32 m_uiHasteTimer;

    uint32 m_uiMoveTimer;

    void Reset()
    {
        m_uiMoveTimer = 2000;
        SetCombatMovement(false);
        m_uiArcaneShockTimer = urand(8000, 9000);
        m_uiHasteTimer = urand(10000, 12000);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (!m_creature->CanReachWithMeleeAttack(m_creature->getVictim()))
        {
            if (m_uiMoveTimer <=uiDiff)
            {
                if (VehicleKit* pVehicleBase = m_creature->GetVehicle())
                {
                    if (Unit* pDisk = pVehicleBase->GetBase())
                    {
                        pDisk->GetMotionMaster()->Clear();
                        pDisk->GetMotionMaster()->MoveChase(m_creature->getVictim());
                    }
                }
                m_uiMoveTimer = 2000;
            }
            else
                m_uiMoveTimer -= uiDiff;
        }

        if (m_uiArcaneShockTimer <= uiDiff)
        {
            DoCast(m_creature->getVictim(), m_bIsRegularMode ? SPELL_ARCANE_SHOCK : SPELL_ARCANE_SHOCK_H);
            m_uiArcaneShockTimer = urand(8000, 10000);
        }
        else
            m_uiArcaneShockTimer -= uiDiff;

        if (m_uiHasteTimer <= uiDiff)
        {
            DoCast(m_creature, SPELL_HASTE);
            m_uiHasteTimer = urand(25000, 30000);
        }
        else
            m_uiHasteTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

/*######
## npc_scion_of_eternity
######*/

struct MANGOS_DLL_DECL npc_scion_of_eternityAI : public ScriptedAI
{
    npc_scion_of_eternityAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    bool m_bIsRegularMode;
    uint32 m_uiArcaneBarrageTimer;

    uint32 m_uiMoveTimer;

    void Reset()
    {
        m_uiMoveTimer = 2000;
        SetCombatMovement(false);
        m_uiArcaneBarrageTimer = urand(4000, 12000);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_uiArcaneBarrageTimer <= uiDiff)
        {
            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
            {
                if (!pTarget->GetVehicle())
                {
                    int32 uiDmg = m_bIsRegularMode ? urand(14138, 15862) : urand(16965, 19035);
                    m_creature->CastCustomSpell(pTarget, SPELL_ARCANE_BARRAGE, &uiDmg, 0, 0, true);
                    m_uiArcaneBarrageTimer = urand(4000, 12000);
                }
            }
        }
        else
            m_uiArcaneBarrageTimer -= uiDiff;

        if (m_uiMoveTimer < uiDiff)
        {
            if (VehicleKit* pVehicle = m_creature->GetVehicle())
            {
                if (Unit* pDisk = pVehicle->GetBase())
                {
                    pDisk->GetMotionMaster()->Clear();
                    pDisk->GetMotionMaster()->MovePoint(0, urand(PLATFORM_MIN_X, PLATFORM_MAX_X), urand(PLATFORM_MIN_Y, PLATFORM_MAX_Y), FLOOR_Z+10.0f+urand(0, 15), false);
                }
            }
            m_uiMoveTimer = 10000;
        }
        else
            m_uiMoveTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

/*######
## npc_hover_disk
######*/
struct MANGOS_DLL_DECL npc_hover_diskAI : public ScriptedAI
{
    npc_hover_diskAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        Reset();
    }

    void Reset()
    {
        m_creature->CastSpell(m_creature, SPELL_FLIGHT, true);
        m_creature->SetLevitate(true);
    }

    void AttackStart(Unit *pWho)
    {
    }

    void PassengerBoarded(Unit * pWho, int8 seatId, bool apply)
    {
        if (!apply)
        {
            m_creature->GetMotionMaster()->MoveFall();
            m_creature->setFaction(35);
            m_creature->SetByteValue(UNIT_FIELD_BYTES_1, 3, UNIT_BYTE1_FLAG_ALWAYS_STAND | UNIT_BYTE1_FLAG_UNK_2);
            m_creature->CastSpell(m_creature, SPELL_FLIGHT, true);
            m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
        }
    }

    void UpdateAI(const uint32 uiDiff)
    {
    }
};

/*######
## npc_alexstrasza
######*/
struct MANGOS_DLL_DECL npc_alexstraszaAI : public ScriptedAI
{
    npc_alexstraszaAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        Reset();
    }

    ScriptedInstance* m_pInstance;
    uint32 m_uiTimer;
    uint8 m_uiCount;

    void Reset()
    {
        m_uiTimer = 9500;
        m_uiCount = 0;
        m_creature->SetByteValue(UNIT_FIELD_BYTES_1, 3, UNIT_BYTE1_FLAG_ALWAYS_STAND | UNIT_BYTE1_FLAG_UNK_2);
        m_creature->SetLevitate(true);
        if (m_pInstance)
            if (Creature* pMalygos = m_pInstance->GetSingleCreatureFromStorage(NPC_MALYGOS))
                m_creature->SetFacingToObject(pMalygos);
    }

    void SpellHitTarget(Unit* pUnit, const SpellEntry* pSpell)
    {
        if (pUnit->GetEntry() == NPC_ALEXSTRASZAS_GIFT && pSpell->Id == SPELL_ALEXSTRASZAS_GIFT_BEAM)
            pUnit->CastSpell(pUnit, SPELL_ALEXSTRASZAS_GIFT_VISUAL, true);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (m_uiCount == 6)
            return;

        if (m_uiTimer <= uiDiff)
        {
            switch (m_uiCount)
            {
                case 0:
                    if (m_pInstance)
                        if (Creature* pMalygos = m_pInstance->GetSingleCreatureFromStorage(NPC_MALYGOS))
                            pMalygos->RemoveCorpse();
                    DoScriptText(SAY_OUTRO1, m_creature);
                    m_uiTimer = 5000;
                    break;
                case 1:
                    DoScriptText(SAY_OUTRO2, m_creature);
                    m_uiTimer = 3500;
                    break;
                case 2:
                    DoScriptText(SAY_OUTRO3, m_creature);
                    m_uiTimer = 22000;
                    break;
                case 3:
                    DoScriptText(SAY_OUTRO4, m_creature);
                    m_uiTimer = 19500;
                    break;
                case 4:
                    DoCast(m_creature, SPELL_ALEXSTRASZAS_GIFT_BEAM);
                       m_uiTimer = 3000;
                    break;
                case 5:
                    if (m_pInstance)
                        m_pInstance->SetData(TYPE_MALYGOS, DONE);
                    break;
            }
            ++m_uiCount;
        }
        else
            m_uiTimer -= uiDiff;
    }
};

/*######
## go_focusing_iris
######*/
bool GOHello_go_focusing_iris(Player* pPlayer, GameObject* pGo)
{
    if (instance_eye_of_eternity* pInstance = (instance_eye_of_eternity*)pGo->GetInstanceData())
    {
        pInstance->SetData(TYPE_MALYGOS, IN_PROGRESS);
    }
    return false;
}

CreatureAI* GetAI_boss_malygos(Creature* pCreature)
{
    return new boss_malygosAI(pCreature);
}

CreatureAI* GetAI_npc_power_spark(Creature* pCreature)
{
    return new npc_power_sparkAI(pCreature);
}

CreatureAI* GetAI_npc_nexus_lord(Creature* pCreature)
{
    return new npc_nexus_lordAI(pCreature);
}

CreatureAI* GetAI_npc_scion_of_eternity(Creature* pCreature)
{
    return new npc_scion_of_eternityAI(pCreature);
}

CreatureAI* GetAI_npc_hover_disk(Creature* pCreature)
{
    return new npc_hover_diskAI(pCreature);
}

CreatureAI* GetAI_npc_alexstrasza(Creature* pCreature)
{
    return new npc_alexstraszaAI(pCreature);
}

struct MANGOS_DLL_DECL npc_whyrmrest_skytalonAI : public ScriptedAI
{
    npc_whyrmrest_skytalonAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (instance_eye_of_eternity*)pCreature->GetInstanceData();
        Reset();
    }

    instance_eye_of_eternity* m_pInstance;
    bool Active;
    bool seated;
    ObjectGuid ownerGUID;
    uint32 StartTimer;
    uint32 SeatTimer;

    void Reset()
    {
        Active = false;
        seated = false;
        ownerGUID = ObjectGuid();
        StartTimer = 500;
        SeatTimer = 2000;
        ownerGUID = m_creature->GetCreatorGuid();
        m_creature->SetByteValue(UNIT_FIELD_BYTES_1, 3, UNIT_BYTE1_FLAG_ALWAYS_STAND | UNIT_BYTE1_FLAG_UNK_2);
        m_creature->SetLevitate(true);
    }

    void AttackStart(Unit *) {}
    void MoveInLineOfSight(Unit*) {}

    /*void JustDied(Unit* killer)
    {
        if (!m_creature || m_creature->GetTypeId() != TYPEID_UNIT)
            return;

        if (ownerGUID.IsEmpty())
            ownerGUID = m_creature->GetCreatorGuid();

        Unit* owner = m_creature->GetMap()->GetUnit(ownerGUID);

        if (!owner || owner->GetTypeId() != TYPEID_PLAYER)
            return;

        owner->RemoveAurasDueToSpell(SPELL_VEHICLE_HARDCODED);
        m_creature->SetCreatorGuid(ObjectGuid());
    }*/

    void UpdateAI(const uint32 uiDiff)
    {

        if (ownerGUID.IsEmpty())
        {
            ownerGUID = m_creature->GetCreatorGuid();
            if (Unit* owner = m_creature->GetMap()->GetUnit(ownerGUID))
                m_creature->setFaction(owner->getFaction());
        }

        if (!ownerGUID.IsEmpty() && !seated && Active)
        {
            if (SeatTimer < uiDiff)
            {
                if (Unit* owner = m_creature->GetMap()->GetUnit(ownerGUID))
                {
                    m_creature->setFaction(owner->getFaction());
                    owner->CastSpell(m_creature, SPELL_RIDE_RED_DRAGON_BUDDY, true);
                    if (Creature* pMalygos = m_pInstance->GetSingleCreatureFromStorage(NPC_MALYGOS))
                    {
                        pMalygos->SetInCombatWith(m_creature);
                        pMalygos->AddThreat(m_creature);
                    }
                    seated = true;
                }
            }
            else
                SeatTimer -= uiDiff;
        }

        if (!ownerGUID.IsEmpty() && !Active)
        {
            if (StartTimer < uiDiff)
            {
                if (Unit* owner = m_creature->GetMap()->GetUnit(ownerGUID))
                {
                    float x, y, z;
                    owner->GetClosePoint(x, y, z, owner->GetObjectBoundingRadius(), 1.0f, owner->GetAngle(m_creature));
                    m_creature->GetMotionMaster()->MovePoint(0, x, y, z);
                    Active = true;
                }
            }
            else
                StartTimer -= uiDiff;
        }
    }
};

CreatureAI* GetAI_npc_whyrmrest_skytalon(Creature* pCreature)
{
    return new npc_whyrmrest_skytalonAI(pCreature);
}

void AddSC_boss_malygos()
{
    Script *newscript;

    newscript = new Script;
    newscript->Name = "boss_malygos";
    newscript->GetAI = &GetAI_boss_malygos;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_power_spark";
    newscript->GetAI = &GetAI_npc_power_spark;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_nexus_lord";
    newscript->GetAI = &GetAI_npc_nexus_lord;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_scion_of_eternity";
    newscript->GetAI = &GetAI_npc_scion_of_eternity;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_hover_disk";
    newscript->GetAI = &GetAI_npc_hover_disk;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_whyrmrest_skytalon";
    newscript->GetAI = &GetAI_npc_whyrmrest_skytalon;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_alexstrasza";
    newscript->GetAI = &GetAI_npc_alexstrasza;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "go_focusing_iris";
    newscript->pGOUse = &GOHello_go_focusing_iris;
    newscript->RegisterSelf();
}
