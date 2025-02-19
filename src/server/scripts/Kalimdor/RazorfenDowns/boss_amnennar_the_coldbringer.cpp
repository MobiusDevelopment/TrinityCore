/**
 * This file is part of the MobiusCore project.
 * See AUTHORS file for copyright information.
 */

#include "ScriptMgr.h"
#include "razorfen_downs.h"
#include "ScriptedCreature.h"

enum Say
{
    SAY_AGGRO               = 0,
    SAY_SUMMON60            = 1,
    SAY_SUMMON30            = 2,
    SAY_HP                  = 3,
    SAY_KILL                = 4
};

enum Spells
{
    SPELL_AMNENNARSWRATH    = 13009,
    SPELL_FROSTBOLT         = 15530,
    SPELL_FROST_NOVA        = 15531,
    SPELL_FROST_SPECTRES    = 12642
};

enum Events
{
    EVENT_AMNENNARSWRATH    = 1,
    EVENT_FROSTBOLT         = 2,
    EVENT_FROST_NOVA        = 3
};

class boss_amnennar_the_coldbringer : public CreatureScript
{
public:
    boss_amnennar_the_coldbringer() : CreatureScript("boss_amnennar_the_coldbringer") { }

    struct boss_amnennar_the_coldbringerAI : public BossAI
    {
        boss_amnennar_the_coldbringerAI(Creature* creature) : BossAI(creature, DATA_AMNENNAR_THE_COLD_BRINGER)
        {
            Initialize();
        }

        void Initialize()
        {
            hp60Spectrals = false;
            hp30Spectrals = false;
            hp50 = false;
        }

        void Reset() override
        {
            _Reset();
            Initialize();
        }

        void EnterCombat(Unit* /*who*/) override
        {
            _EnterCombat();
            events.ScheduleEvent(EVENT_AMNENNARSWRATH, 8000);
            events.ScheduleEvent(EVENT_FROSTBOLT, 1000);
            events.ScheduleEvent(EVENT_FROST_NOVA, urand(10000, 15000));
            Talk(SAY_AGGRO);
        }

        void KilledUnit(Unit* who) override
        {
            if (who->GetTypeId() == TYPEID_PLAYER)
                Talk(SAY_KILL);
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
                    case EVENT_AMNENNARSWRATH:
                        DoCastVictim(SPELL_AMNENNARSWRATH);
                        events.ScheduleEvent(EVENT_AMNENNARSWRATH, 12000);
                        break;
                    case EVENT_FROSTBOLT:
                        DoCastVictim(SPELL_FROSTBOLT);
                        events.ScheduleEvent(EVENT_FROSTBOLT, 8000);
                        break;
                    case EVENT_FROST_NOVA:
                        DoCast(me, SPELL_FROST_NOVA);
                        events.ScheduleEvent(EVENT_FROST_NOVA, 15000);
                        break;
                }

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;
            }

            if (!hp60Spectrals && HealthBelowPct(60))
            {
                Talk(SAY_SUMMON60);
                DoCastVictim(SPELL_FROST_SPECTRES);
                hp60Spectrals = true;
            }

            if (!hp50 && HealthBelowPct(50))
            {
                Talk(SAY_HP);
                hp50 = true;
            }

            if (!hp30Spectrals && HealthBelowPct(30))
            {
                Talk(SAY_SUMMON30);
                DoCastVictim(SPELL_FROST_SPECTRES);
                hp30Spectrals = true;
            }

            DoMeleeAttackIfReady();
        }

    private:
        bool hp60Spectrals;
        bool hp30Spectrals;
        bool hp50;
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return GetRazorfenDownsAI<boss_amnennar_the_coldbringerAI>(creature);
    }
};

void AddSC_boss_amnennar_the_coldbringer()
{
    new boss_amnennar_the_coldbringer();
}
