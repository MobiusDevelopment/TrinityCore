/**
 * This file is part of the MobiusCore project.
 * See AUTHORS file for copyright information.
 */

#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "zulaman.h"

enum Says
{
    SAY_AGGRO               = 0,
    SAY_PLAYER_KILL         = 1,
    SAY_MELEE               = 2,
    SAY_SPLIT               = 3,
    SAY_COMBINE             = 4,
    SAY_DEATH               = 5
};

enum Spells
{
};

enum Events
{
};

class boss_halazzi : public CreatureScript
{
    public:

        boss_halazzi() : CreatureScript("boss_halazzi") { }

        struct boss_halazziAI : public BossAI
        {
            boss_halazziAI(Creature* creature) : BossAI(creature, DATA_HALAZZI) { }

            void Reset() override
            {
                _Reset();
            }

            void EnterCombat(Unit* /*who*/) override
            {
                Talk(SAY_AGGRO);
                _EnterCombat();
            }

            void JustDied(Unit* /*killer*/) override
            {
                Talk(SAY_DEATH);
                _JustDied();
            }

            void KilledUnit(Unit* victim) override
            {
                if (victim->GetTypeId() == TYPEID_PLAYER)
                    Talk(SAY_PLAYER_KILL);
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                events.Update(diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;
                /*
                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        default:
                            break;
                    }
                }
                */

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetZulAmanAI<boss_halazziAI>(creature);
        }
};

void AddSC_boss_halazzi()
{
    new boss_halazzi();
}
