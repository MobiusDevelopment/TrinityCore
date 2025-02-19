/**
 * This file is part of the MobiusCore project.
 * See AUTHORS file for copyright information.
 */

#include "ScriptMgr.h"
#include "blackrock_spire.h"
#include "ScriptedCreature.h"

enum Spells
{
    SPELL_REND                      = 16509,
    SPELL_STRIKE                    = 15580,
    SPELL_INTIMIDATING_ROAR         = 16508
};

enum Says
{
    SAY_SUMMON                      = 0,
    SAY_AGGRO                       = 1,
};

enum Events
{
    EVENT_REND                      = 1,
    EVENT_STRIKE                    = 2,
    EVENT_INTIMIDATING_ROAR         = 3
};

class boss_urok_doomhowl : public CreatureScript
{
public:
    boss_urok_doomhowl() : CreatureScript("boss_urok_doomhowl") { }

    struct boss_urok_doomhowlAI : public BossAI
    {
        boss_urok_doomhowlAI(Creature* creature) : BossAI(creature, DATA_UROK_DOOMHOWL) { }

        void Reset() override
        {
            _Reset();
        }

        void EnterCombat(Unit* /*who*/) override
        {
            _EnterCombat();
            events.ScheduleEvent(SPELL_REND, urand(17000,20000));
            events.ScheduleEvent(SPELL_STRIKE, urand(10000,12000));
            Talk(SAY_AGGRO);
        }

        void JustDied(Unit* /*killer*/) override
        {
            _JustDied();
        }

        void UpdateAI(uint32 diff) override
        {
            if (!UpdateVictim())
                return;

            events.Update(diff);

            if (me->HasUnitState(UNIT_STATE_CASTING))
                return;

            while (uint32 eventId = events.ExecuteEvent())
            {
                switch (eventId)
                {
                    case SPELL_REND:
                        DoCastVictim(SPELL_REND);
                        events.ScheduleEvent(SPELL_REND, urand(8000,10000));
                        break;
                    case SPELL_STRIKE:
                        DoCastVictim(SPELL_STRIKE);
                        events.ScheduleEvent(SPELL_STRIKE, urand(8000,10000));
                        break;
                    default:
                        break;
                }

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;
            }
            DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return GetBlackrockSpireAI<boss_urok_doomhowlAI>(creature);
    }
};

void AddSC_boss_urok_doomhowl()
{
    new boss_urok_doomhowl();
}
