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
#include "precompiled.h"
#include "OutdoorPvP/OutdoorPvPWG.h"

/*######
## go_wg_def_portal
######*/

static float SpawnLocation[10][3]=
{
    { 5153.41f, 2901.35f, 409.191f },
    { 5268.70f, 2666.42f, 409.099f },
    { 5197.05f, 2944.81f, 409.191f },
    { 5196.67f, 2737.34f, 409.189f },
    { 5314.58f, 3055.85f, 408.862f },
    { 5391.28f, 2828.09f, 418.675f },
    { 5153.93f, 2781.67f, 409.246f },
    { 5311.44f, 2618.93f, 409.092f },
    { 5269.21f, 3013.84f, 408.77f },
    { 5401.63f, 2853.67f, 418.675f },
};

static float TeleLocation[10][3]=
{
    { 5157.1f, 2912.0f, 413.494f },
    { 5270.11f, 2655.62f, 413.403f },
    { 5187.48f, 2942.62f, 413.495f },
    { 5187.25f, 2739.45f, 413.492f },
    { 5304.47f, 3054.62f, 412.147f },
    { 5401.77f, 2853.72f, 418.896f },
    { 5155.76f, 2771.45f, 413.493f },
    { 5302.64f, 2623.39f, 413.403f },
    { 5271.58f, 3023.12f, 412.147f },
    { 5391.28f, 2828.09f, 418.675f },
};

bool GOHello_go_wg_teleport(Player* pPlayer, GameObject* pGo)
{
    for (uint32 i=0; i < 10; ++i)
    {
        if (fabs(pGo->GetDistance2d(SpawnLocation[i][0], SpawnLocation[i][1])) < M_NULL_F)
           pPlayer->TeleportTo(pPlayer->GetMapId(),TeleLocation[i][0],TeleLocation[i][1],TeleLocation[i][2],0);
    }

    return false;
}

/*######
## npc_wg_demolisher_engineer
######*/

#define GOSSIP_HELLO_DEMO1  "Build catapult."
#define GOSSIP_HELLO_DEMO2  "Build demolisher."
#define GOSSIP_HELLO_DEMO3  "Build siege engine."
#define GOSSIP_HELLO_DEMO4  "I cannot build more!"

#define SPELL_CORPORAL     33280
#define SPELL_LIEUTENANT   55629

#define NPC_CATAPULT        27881
#define NPC_DEMOLISHER      28094
#define NPC_SIEGE_ENGINE_A  28312
#define NPC_SIEGE_ENGINE_H  32627

/*
    WS_VEHICLE_COUNT_H                  = 3490,
    WS_VEHICLE_COUNT_MAX_H              = 3491,
    WS_VEHICLE_COUNT_A                  = 3680,
    WS_VEHICLE_COUNT_MAX_A              = 3681,
*/

bool GossipHello_npc_wg_demolisher_engineer(Player* pPlayer, Creature* pCreature)
{
    uint32 team     = 0;
    uint32 count    = 0;
    uint32 maxCount = 0;

    if (pCreature->GetEntry() == 30499)
    {
        team = ALLIANCE;
        count = pCreature->GetWorldState(WS_VEHICLE_COUNT_A);
        maxCount = pCreature->GetWorldState(WS_VEHICLE_COUNT_MAX_A);
    }
    else if(pCreature->GetEntry() == 30400)
    {
        team = HORDE;
        count = pCreature->GetWorldState(WS_VEHICLE_COUNT_H);
        maxCount = pCreature->GetWorldState(WS_VEHICLE_COUNT_MAX_H);
    }

    if (maxCount > count)
    {
        if (pPlayer->GetSpellAuraHolder(SPELL_CORPORAL))
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_HELLO_DEMO1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF);
        else if (pPlayer->GetSpellAuraHolder(SPELL_LIEUTENANT))
        {
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_HELLO_DEMO1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF);
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_HELLO_DEMO2, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_HELLO_DEMO3, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
        }
    }

    pPlayer->SEND_GOSSIP_MENU(pPlayer->GetGossipTextId(pCreature), pCreature->GetObjectGuid());

    return true;
}

enum BuildSpellsWG
{
    SPELL_BUILD_CATAPULT   = 56664,
    SPELL_BUILD_DEMOLISHER = 56659,
    SPELL_BUILD_SIEGE_A    = 56662,
    SPELL_BUILD_SIEGE_H    = 61409,
};

// FIXME - Need make over DB
bool GossipSelect_npc_wg_demolisher_engineer(Player* pPlayer, Creature* pCreature, uint32 uiSender, uint32 uiAction)
{
    switch(uiAction)
    {
        case GOSSIP_ACTION_INFO_DEF:
            pPlayer->CastSpell(pPlayer, SPELL_BUILD_CATAPULT, true);
            pPlayer->CLOSE_GOSSIP_MENU();
            break;
        case GOSSIP_ACTION_INFO_DEF + 1:
            pPlayer->CastSpell(pPlayer, SPELL_BUILD_DEMOLISHER, true);
            pPlayer->CLOSE_GOSSIP_MENU();
            break;
        case GOSSIP_ACTION_INFO_DEF + 2:
            if(pCreature->GetEntry() == 30499)
                pPlayer->CastSpell(pPlayer, SPELL_BUILD_SIEGE_A, true);
            else if(pCreature->GetEntry() == 30400)
                pPlayer->CastSpell(pPlayer, SPELL_BUILD_SIEGE_H, true);
            pPlayer->CLOSE_GOSSIP_MENU();
            break;
        default:
            break;
    }

    return true;
}

void AddSC_wintergrasp()
{
    Script *pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "go_wg_teleport";
    pNewScript->pGOUse = &GOHello_go_wg_teleport;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_wg_demolisher_engineer";
    pNewScript->pGossipHello = &GossipHello_npc_wg_demolisher_engineer;
    pNewScript->pGossipSelect = &GossipSelect_npc_wg_demolisher_engineer;
    pNewScript->RegisterSelf();
}