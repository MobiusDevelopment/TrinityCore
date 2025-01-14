/**
 * This file is part of the MobiusCore project.
 * See AUTHORS file for copyright information.
 */

/* ScriptData
SDName: Uldaman
SD%Complete: 100
SDComment: Quest support: 2240, 2278 + 1 trash mob.
SDCategory: Uldaman
EndScriptData */

/* ContentData
npc_jadespine_basilisk
go_keystone_chamber
at_map_chamber
EndContentData */

#include "ScriptMgr.h"
#include "GameObject.h"
#include "InstanceScript.h"
#include "Player.h"
#include "ScriptedCreature.h"
#include "uldaman.h"

/*######
## npc_jadespine_basilisk
######*/

enum Spells
{
    SPELL_CRYSTALLINE_SLUMBER   = 3636,
};

class npc_jadespine_basilisk : public CreatureScript
{
    public:

        npc_jadespine_basilisk()
            : CreatureScript("npc_jadespine_basilisk")
        {
        }

        struct npc_jadespine_basiliskAI : public ScriptedAI
        {
            npc_jadespine_basiliskAI(Creature* creature) : ScriptedAI(creature)
            {
                Initialize();
            }

            void Initialize()
            {
                uiCslumberTimer = 2000;
            }

            uint32 uiCslumberTimer;

            void Reset() override
            {
                Initialize();
            }

            void EnterCombat(Unit* /*who*/) override
            {
            }

            void UpdateAI(uint32 uiDiff) override
            {
                //Return since we have no target
                if (!UpdateVictim())
                    return;

                //uiCslumberTimer
                if (uiCslumberTimer <= uiDiff)
                {
                    //Cast
                    DoCastVictim(SPELL_CRYSTALLINE_SLUMBER, true);

                    //Stop attacking target thast asleep and pick new target
                    uiCslumberTimer = 28000;

                    Unit* target = SelectTarget(SELECT_TARGET_TOPAGGRO, 0);

                    if (!target || target == me->GetVictim())
                        target = SelectTarget(SELECT_TARGET_RANDOM, 0, 100, true);

                    if (target)
                        me->TauntApply(target);

                } else uiCslumberTimer -= uiDiff;

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_jadespine_basiliskAI(creature);
        }
};

/*######
## go_keystone_chamber
######*/

class go_keystone_chamber : public GameObjectScript
{
public:
    go_keystone_chamber() : GameObjectScript("go_keystone_chamber") { }

    bool OnGossipHello(Player* /*player*/, GameObject* go) override
    {
        if (InstanceScript* instance = go->GetInstanceScript())
            instance->SetData(DATA_IRONAYA_SEAL, IN_PROGRESS); //door animation and save state.

        return false;
    }
};

/*######
## at_map_chamber
######*/

enum MapChamber
{
    QUEST_HIDDEN_CHAMBER = 2240
};

class AreaTrigger_at_map_chamber : public AreaTriggerScript
{
    public:

        AreaTrigger_at_map_chamber()
            : AreaTriggerScript("at_map_chamber")
        {
        }

        bool OnTrigger(Player* player, AreaTriggerEntry const* /*areaTrigger*/, bool /*entered*/) override
        {
            if (player->GetQuestStatus(QUEST_HIDDEN_CHAMBER) == QUEST_STATUS_INCOMPLETE)
                player->AreaExploredOrEventHappens(QUEST_HIDDEN_CHAMBER);

            return true;
        }
};

void AddSC_uldaman()
{
    new npc_jadespine_basilisk();
    new go_keystone_chamber();
    new AreaTrigger_at_map_chamber();
}
