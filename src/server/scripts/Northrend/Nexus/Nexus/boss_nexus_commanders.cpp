/**
 * This file is part of the MobiusCore project.
 * See AUTHORS file for copyright information.
 */

#include "ScriptMgr.h"
#include "nexus.h"
#include "ScriptedCreature.h"

enum Spells
{
    SPELL_BATTLE_SHOUT           = 31403,
    SPELL_CHARGE                 = 60067,
    SPELL_FRIGHTENING_SHOUT      = 19134,
    SPELL_WHIRLWIND              = 38618,
    SPELL_FROZEN_PRISON          = 47543
};

enum Yells
{
    SAY_AGGRO                    = 0,
    SAY_KILL                     = 1,
    SAY_DEATH                    = 2
};

enum Events
{
    EVENT_CHARGE_COMMANDER      = 1,
    EVENT_WHIRLWIND,
    EVENT_FRIGHTENING_SHOUT
};

class boss_nexus_commanders : public CreatureScript
{
    public:
        boss_nexus_commanders() : CreatureScript("boss_nexus_commanders") { }

        struct boss_nexus_commandersAI : public BossAI
        {
            boss_nexus_commandersAI(Creature* creature) : BossAI(creature, DATA_COMMANDER) { }

            void EnterCombat(Unit* /*who*/) override
            {
                _EnterCombat();
                Talk(SAY_AGGRO);
                me->RemoveAurasDueToSpell(SPELL_FROZEN_PRISON);
                DoCast(me, SPELL_BATTLE_SHOUT);

                events.ScheduleEvent(EVENT_CHARGE_COMMANDER, urand(3000, 4000));
                events.ScheduleEvent(EVENT_WHIRLWIND, urand(6000, 8000));
                events.ScheduleEvent(EVENT_FRIGHTENING_SHOUT, urand(13000, 15000));
            }

            void ExecuteEvent(uint32 eventId) override
            {
                switch (eventId)
                {
                    case EVENT_CHARGE_COMMANDER:
                        if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 100.0f, true))
                            DoCast(target, SPELL_CHARGE);
                        events.ScheduleEvent(EVENT_CHARGE_COMMANDER, urand(11000, 15000));
                        break;
                    case EVENT_WHIRLWIND:
                        DoCast(me, SPELL_WHIRLWIND);
                        events.ScheduleEvent(EVENT_WHIRLWIND, urand(19500, 25000));
                        break;
                    case EVENT_FRIGHTENING_SHOUT:
                        DoCastAOE(SPELL_FRIGHTENING_SHOUT);
                        events.ScheduleEvent(EVENT_FRIGHTENING_SHOUT, urand(45000, 55000));
                        break;
                    default:
                        break;
                }
            }

            void JustDied(Unit* /*killer*/) override
            {
                _JustDied();
                Talk(SAY_DEATH);
            }

            void KilledUnit(Unit* who) override
            {
                if (who->GetTypeId() == TYPEID_PLAYER)
                    Talk(SAY_KILL);
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetNexusAI<boss_nexus_commandersAI>(creature);
        }
};

void AddSC_boss_nexus_commanders()
{
    new boss_nexus_commanders();
}
