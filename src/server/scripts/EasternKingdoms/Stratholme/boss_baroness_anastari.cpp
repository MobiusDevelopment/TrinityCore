/**
 * This file is part of the MobiusCore project.
 * See AUTHORS file for copyright information.
 */

/* ScriptData
SDName: Boss_Baroness_Anastari
SD%Complete: 90
SDComment: MC disabled
SDCategory: Stratholme
EndScriptData */

#include "ScriptMgr.h"
#include "InstanceScript.h"
#include "ScriptedCreature.h"
#include "stratholme.h"

enum Spells
{
    SPELL_BANSHEEWAIL       = 16565,
    SPELL_BANSHEECURSE      = 16867,
    SPELL_SILENCE           = 18327
    //SPELL_POSSESS           = 17244
};

class boss_baroness_anastari : public CreatureScript
{
public:
    boss_baroness_anastari() : CreatureScript("boss_baroness_anastari") { }

    CreatureAI* GetAI(Creature* creature) const override
    {
        return GetStratholmeAI<boss_baroness_anastariAI>(creature);
    }

    struct boss_baroness_anastariAI : public ScriptedAI
    {
        boss_baroness_anastariAI(Creature* creature) : ScriptedAI(creature)
        {
            Initialize();
            instance = me->GetInstanceScript();
        }

        void Initialize()
        {
            BansheeWail_Timer = 1000;
            BansheeCurse_Timer = 11000;
            Silence_Timer = 13000;
            //Possess_Timer = 35000;
        }

        InstanceScript* instance;

        uint32 BansheeWail_Timer;
        uint32 BansheeCurse_Timer;
        uint32 Silence_Timer;
        //uint32 Possess_Timer;

        void Reset() override
        {
            Initialize();
        }

        void EnterCombat(Unit* /*who*/) override
        {
        }

        void JustDied(Unit* /*killer*/) override
        {
            instance->SetData(TYPE_BARONESS, IN_PROGRESS);
        }

        void UpdateAI(uint32 diff) override
        {
            if (!UpdateVictim())
                return;

            //BansheeWail
            if (BansheeWail_Timer <= diff)
            {
                if (rand32() % 100 < 95)
                    DoCastVictim(SPELL_BANSHEEWAIL);
                //4 seconds until we should cast this again
                BansheeWail_Timer = 4000;
            } else BansheeWail_Timer -= diff;

            //BansheeCurse
            if (BansheeCurse_Timer <= diff)
            {
                if (rand32() % 100 < 75)
                    DoCastVictim(SPELL_BANSHEECURSE);
                //18 seconds until we should cast this again
                BansheeCurse_Timer = 18000;
            } else BansheeCurse_Timer -= diff;

            //Silence
            if (Silence_Timer <= diff)
            {
                if (rand32() % 100 < 80)
                    DoCastVictim(SPELL_SILENCE);
                //13 seconds until we should cast this again
                Silence_Timer = 13000;
            } else Silence_Timer -= diff;

            DoMeleeAttackIfReady();
        }
    };

};

void AddSC_boss_baroness_anastari()
{
    new boss_baroness_anastari();
}
