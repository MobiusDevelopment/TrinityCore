/**
 * This file is part of the MobiusCore project.
 * See AUTHORS file for copyright information.
 */

/*
Name: Boss_Chrono_Lord_Deja
%Complete: 65
Comment: All abilities not implemented
Category: Caverns of Time, The Black Morass
*/

#include "ScriptMgr.h"
#include "InstanceScript.h"
#include "ScriptedCreature.h"
#include "the_black_morass.h"

enum Enums
{
    SAY_ENTER                   = 0,
    SAY_AGGRO                   = 1,
    SAY_BANISH                  = 2,
    SAY_SLAY                    = 3,
    SAY_DEATH                   = 4,

    SPELL_ARCANE_BLAST          = 31457,
    H_SPELL_ARCANE_BLAST        = 38538,
    SPELL_ARCANE_DISCHARGE      = 31472,
    H_SPELL_ARCANE_DISCHARGE    = 38539,
    SPELL_TIME_LAPSE            = 31467,
    SPELL_ATTRACTION            = 38540                       //Not Implemented (Heroic mode)
};

enum Events
{
    EVENT_ARCANE_BLAST          = 1,
    EVENT_TIME_LAPSE            = 2,
    EVENT_ARCANE_DISCHARGE      = 3,
    EVENT_ATTRACTION            = 4
};

class boss_chrono_lord_deja : public CreatureScript
{
public:
    boss_chrono_lord_deja() : CreatureScript("boss_chrono_lord_deja") { }

    struct boss_chrono_lord_dejaAI : public BossAI
    {
        boss_chrono_lord_dejaAI(Creature* creature) : BossAI(creature, TYPE_CRONO_LORD_DEJA) { }

        void Reset() override { }

        void EnterCombat(Unit* /*who*/) override
        {
            events.ScheduleEvent(EVENT_ARCANE_BLAST, urand(18000, 23000));
            events.ScheduleEvent(EVENT_TIME_LAPSE, urand(10000, 15000));
            events.ScheduleEvent(EVENT_ARCANE_DISCHARGE, urand(20000, 30000));
            if (IsHeroic())
                events.ScheduleEvent(EVENT_ATTRACTION, urand(25000, 35000));

            Talk(SAY_AGGRO);
        }

        void MoveInLineOfSight(Unit* who) override

        {
            //Despawn Time Keeper
            if (who->GetTypeId() == TYPEID_UNIT && who->GetEntry() == NPC_TIME_KEEPER)
            {
                if (me->IsWithinDistInMap(who, 20.0f))
                {
                    Talk(SAY_BANISH);
                    me->DealDamage(who, who->GetHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
                }
            }

            ScriptedAI::MoveInLineOfSight(who);
        }

        void KilledUnit(Unit* /*victim*/) override
        {
            Talk(SAY_SLAY);
        }

        void JustDied(Unit* /*killer*/) override
        {
            Talk(SAY_DEATH);

            instance->SetData(TYPE_RIFT, SPECIAL);
        }

        void UpdateAI(uint32 diff) override
        {
            if (!UpdateVictim())
                return;

            if (me->HasUnitState(UNIT_STATE_CASTING))
                return;

            events.Update(diff);

            while (uint32 eventId = events.ExecuteEvent())
            {
                switch (eventId)
                {
                    case EVENT_ARCANE_BLAST:
                        DoCastVictim(SPELL_ARCANE_BLAST);
                        events.ScheduleEvent(EVENT_ARCANE_BLAST, urand(15000, 25000));
                        break;
                    case EVENT_TIME_LAPSE:
                        Talk(SAY_BANISH);
                        DoCast(me, SPELL_TIME_LAPSE);
                        events.ScheduleEvent(EVENT_TIME_LAPSE, urand(15000, 25000));
                        break;
                    case EVENT_ARCANE_DISCHARGE:
                        if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0))
                            DoCast(target, SPELL_ARCANE_DISCHARGE);
                        events.ScheduleEvent(EVENT_ARCANE_DISCHARGE, urand(20000, 30000));
                        break;
                    case EVENT_ATTRACTION: // Only in Heroic
                        DoCast(me, SPELL_ATTRACTION);
                        events.ScheduleEvent(EVENT_ATTRACTION, urand(25000, 35000));
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
        return GetBlackMorassAI<boss_chrono_lord_dejaAI>(creature);
    }
};

void AddSC_boss_chrono_lord_deja()
{
    new boss_chrono_lord_deja();
}
