/**
 * This file is part of the MobiusCore project.
 * See AUTHORS file for copyright information.
 */

/*
Blasted_Lands
Quest support: 3628.
*/

#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "ScriptedGossip.h"
#include "SpellScript.h"
#include "Player.h"
#include "Group.h"

enum DeathlyUsher
{
    SPELL_TELEPORT_SINGLE               = 12885,
    SPELL_TELEPORT_SINGLE_IN_GROUP      = 13142,
    SPELL_TELEPORT_GROUP                = 27686
};

/*#####
# spell_razelikh_teleport_group
#####*/

class spell_razelikh_teleport_group : public SpellScriptLoader
{
    public: spell_razelikh_teleport_group() : SpellScriptLoader("spell_razelikh_teleport_group") { }

        class spell_razelikh_teleport_group_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_razelikh_teleport_group_SpellScript);

            bool Validate(SpellInfo const* /*spell*/) override
            {
                return ValidateSpellInfo({ SPELL_TELEPORT_SINGLE, SPELL_TELEPORT_SINGLE_IN_GROUP });
            }

            void HandleScriptEffect(SpellEffIndex /* effIndex */)
            {
                if (Player* player = GetHitPlayer())
                {
                    if (Group* group = player->GetGroup())
                    {
                        for (GroupReference* itr = group->GetFirstMember(); itr != NULL; itr = itr->next())
                            if (Player* member = itr->GetSource())
                                if (member->IsWithinDistInMap(player, 20.0f) && !member->isDead())
                                    member->CastSpell(member, SPELL_TELEPORT_SINGLE_IN_GROUP, true);
                    }
                    else
                        player->CastSpell(player, SPELL_TELEPORT_SINGLE, true);
                }
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_razelikh_teleport_group_SpellScript::HandleScriptEffect, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_razelikh_teleport_group_SpellScript();
        }
};

void AddSC_blasted_lands()
{
    new spell_razelikh_teleport_group();
}
