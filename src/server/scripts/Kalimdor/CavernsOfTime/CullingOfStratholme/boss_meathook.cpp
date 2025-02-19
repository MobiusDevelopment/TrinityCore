/**
 * This file is part of the MobiusCore project.
 * See AUTHORS file for copyright information.
 */

#include "ScriptMgr.h"
#include "culling_of_stratholme.h"
#include "ScriptedCreature.h"

enum Spells
{
    SPELL_CONSTRICTING_CHAINS                   = 52696, //Encases the targets in chains, dealing 1800 Physical damage every 1 sec. and stunning the target for 5 sec.
    SPELL_DISEASE_EXPULSION                     = 52666, //Meathook belches out a cloud of disease, dealing 1710 to 1890 Nature damage and interrupting the spell casting of nearby enemy targets for 4 sec.
    SPELL_FRENZY                                = 58841 //Increases the caster's Physical damage by 10% for 30 sec.
};

enum Yells
{
    SAY_AGGRO                                   = 0,
    SAY_SLAY                                    = 1,
    SAY_SPAWN                                   = 2,
    SAY_DEATH                                   = 3
};

enum Events
{
    EVENT_CHAIN                                 = 1,
    EVENT_DISEASE,
    EVENT_FRENZY
};

class boss_meathook : public CreatureScript
{
    public:
        boss_meathook() : CreatureScript("boss_meathook") { }

        struct boss_meathookAI : public BossAI
        {
            boss_meathookAI(Creature* creature) : BossAI(creature, DATA_MEATHOOK)
            {
                Talk(SAY_SPAWN);
            }

            void EnterCombat(Unit* /*who*/) override
            {
                Talk(SAY_AGGRO);
                _EnterCombat();
                events.ScheduleEvent(EVENT_CHAIN, urand(12000, 17000));
                events.ScheduleEvent(EVENT_DISEASE, urand(2000, 4000));
                events.ScheduleEvent(EVENT_FRENZY, urand(21000, 26000));
            }

            void ExecuteEvent(uint32 eventId) override
            {
                switch (eventId)
                {
                    case EVENT_CHAIN:
                        if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 100.0f, true))
                            DoCast(target, SPELL_CONSTRICTING_CHAINS);
                        events.ScheduleEvent(EVENT_CHAIN, urand(2000, 4000));
                        break;
                    case EVENT_DISEASE:
                        DoCastAOE(SPELL_DISEASE_EXPULSION);
                        events.ScheduleEvent(EVENT_DISEASE, urand(1500, 4000));
                        break;
                    case EVENT_FRENZY:
                        DoCast(me, SPELL_FRENZY);
                        events.ScheduleEvent(EVENT_FRENZY, urand(21000, 26000));
                        break;
                    default:
                        break;
                }
            }

            void JustDied(Unit* /*killer*/) override
            {
                Talk(SAY_DEATH);
                _JustDied();
            }

            void KilledUnit(Unit* victim) override
            {
                if (victim->GetTypeId() == TYPEID_PLAYER)
                    Talk(SAY_SLAY);
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetCullingOfStratholmeAI<boss_meathookAI>(creature);
        }
};

void AddSC_boss_meathook()
{
    new boss_meathook();
}
