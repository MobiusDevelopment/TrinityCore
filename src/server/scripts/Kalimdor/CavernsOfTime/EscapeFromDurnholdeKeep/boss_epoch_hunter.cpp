/**
 * This file is part of the MobiusCore project.
 * See AUTHORS file for copyright information.
 */

/* ScriptData
SDName: Boss_Epoch_Hunter
SD%Complete: 60
SDComment: Missing spawns pre-event, missing speech to be coordinated with rest of escort event.
SDCategory: Caverns of Time, Old Hillsbrad Foothills
EndScriptData */

#include "ScriptMgr.h"
#include "InstanceScript.h"
#include "old_hillsbrad.h"
#include "ScriptedCreature.h"

/*###################
# boss_epoch_hunter #
####################*/

enum EpochHunter
{
    SAY_ENTER                   = 0,
    SAY_AGGRO                   = 1,
    SAY_SLAY                    = 2,
    SAY_BREATH                  = 3,
    SAY_DEATH                   = 4,

    SPELL_SAND_BREATH           = 31914,
    SPELL_IMPENDING_DEATH       = 31916,
    SPELL_MAGIC_DISRUPTION_AURA = 33834,
    SPELL_WING_BUFFET           = 31475
};

class boss_epoch_hunter : public CreatureScript
{
public:
    boss_epoch_hunter() : CreatureScript("boss_epoch_hunter") { }

    CreatureAI* GetAI(Creature* creature) const override
    {
        return GetOldHillsbradAI<boss_epoch_hunterAI>(creature);
    }

    struct boss_epoch_hunterAI : public ScriptedAI
    {
        boss_epoch_hunterAI(Creature* creature) : ScriptedAI(creature)
        {
            Initialize();
            instance = creature->GetInstanceScript();
        }

        void Initialize()
        {
            SandBreath_Timer = urand(8000, 16000);
            ImpendingDeath_Timer = urand(25000, 30000);
            WingBuffet_Timer = 35000;
            Mda_Timer = 40000;
        }

        InstanceScript* instance;

        uint32 SandBreath_Timer;
        uint32 ImpendingDeath_Timer;
        uint32 WingBuffet_Timer;
        uint32 Mda_Timer;

        void Reset() override
        {
            Initialize();
        }

        void EnterCombat(Unit* /*who*/) override
        {
            Talk(SAY_AGGRO);
        }

        void KilledUnit(Unit* /*victim*/) override
        {
            Talk(SAY_SLAY);
        }

        void JustDied(Unit* /*killer*/) override
        {
            Talk(SAY_DEATH);

            if (instance->GetData(TYPE_THRALL_EVENT) == IN_PROGRESS)
                instance->SetData(TYPE_THRALL_PART4, DONE);
        }

        void UpdateAI(uint32 diff) override
        {
            //Return since we have no target
            if (!UpdateVictim())
                return;

            //Sand Breath
            if (SandBreath_Timer <= diff)
            {
                if (me->IsNonMeleeSpellCast(false))
                    me->InterruptNonMeleeSpells(false);

                DoCastVictim(SPELL_SAND_BREATH);

                Talk(SAY_BREATH);

                SandBreath_Timer = urand(10000, 20000);
            } else SandBreath_Timer -= diff;

            if (ImpendingDeath_Timer <= diff)
            {
                DoCastVictim(SPELL_IMPENDING_DEATH);
                ImpendingDeath_Timer = 25000 + rand32() % 5000;
            } else ImpendingDeath_Timer -= diff;

            if (WingBuffet_Timer <= diff)
            {
                if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0))
                    DoCast(target, SPELL_WING_BUFFET);
                WingBuffet_Timer = 25000 + rand32() % 10000;
            } else WingBuffet_Timer -= diff;

            if (Mda_Timer <= diff)
            {
                DoCast(me, SPELL_MAGIC_DISRUPTION_AURA);
                Mda_Timer = 15000;
            } else Mda_Timer -= diff;

            DoMeleeAttackIfReady();
        }
    };

};

void AddSC_boss_epoch_hunter()
{
    new boss_epoch_hunter();
}
