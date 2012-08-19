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
SDName: Silvermoon_City
SD%Complete: 0
SDComment: Placeholder
SDCategory: Silvermoon City
EndScriptData */

/* ContentData
Lor'themar Theron
EndContentData */

#include "precompiled.h"

/*#######
# boss_lorthemar_theron
#######*/

enum
{
    SPELL_ARCANE_SHOCK              = 59715,
    SPELL_CLEAVE_LORTHEMAR          = 15284,
    SPELL_MANA_BURN                 = 33385,
    SPELL_MASS_CHARM                = 33384,
};

struct MANGOS_DLL_DECL boss_lorthemar_theronAI : public ScriptedAI
{
    boss_lorthemar_theronAI(Creature* pCreature) : ScriptedAI(pCreature) {Reset();}

    uint32 m_uiArcaneShockTimer;
    uint32 m_uiCleaveTimer;
    uint32 m_uiManaBurnTimer;
    uint32 m_uiMassCharmTimer;

    void Reset()
    {
        m_uiArcaneShockTimer    = 9000;
        m_uiCleaveTimer         = 5000;
        m_uiManaBurnTimer       = 12000;
        m_uiMassCharmTimer      = 16000;
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_uiArcaneShockTimer < uiDiff)
        {
            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                DoCast(pTarget, SPELL_ARCANE_SHOCK);
            m_uiArcaneShockTimer = urand(9000, 11000);
        }
        else
            m_uiArcaneShockTimer -= uiDiff;

        if (m_uiCleaveTimer < uiDiff)
        {
            DoCast(m_creature->getVictim(), SPELL_CLEAVE_LORTHEMAR);
            m_uiCleaveTimer = urand(5000, 7000);
        }
        else
            m_uiCleaveTimer -= uiDiff;

        if (m_uiManaBurnTimer < uiDiff)
        {
            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                DoCast(pTarget, SPELL_MANA_BURN);
            m_uiManaBurnTimer = urand(12000, 15000);
        }
        else
            m_uiManaBurnTimer -= uiDiff;

        if (m_uiMassCharmTimer < uiDiff)
        {
            DoCast(m_creature, SPELL_MASS_CHARM);
            m_uiMassCharmTimer = urand(16000, 20000);
        }
        else
            m_uiMassCharmTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_lorthemar_theron(Creature* pCreature)
{
    return new boss_lorthemar_theronAI(pCreature);
}

void AddSC_silvermoon_city()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "boss_lorthemar_theron";
    pNewScript->GetAI = &GetAI_boss_lorthemar_theron;
    pNewScript->RegisterSelf();
}
