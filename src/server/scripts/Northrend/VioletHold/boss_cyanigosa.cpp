/**
 * This file is part of the MobiusCore project.
 * See AUTHORS file for copyright information.
 */

#include "ScriptMgr.h"
#include "InstanceScript.h"
#include "SpellScript.h"
#include "ScriptedCreature.h"
#include "violet_hold.h"

enum Spells
{
    SPELL_SUMMON_PLAYER                         = 21150,
    SPELL_ARCANE_VACUUM                         = 58694,
    SPELL_BLIZZARD                              = 58693,
    SPELL_MANA_DESTRUCTION                      = 59374,
    SPELL_TAIL_SWEEP                            = 58690,
    SPELL_UNCONTROLLABLE_ENERGY                 = 58688,
    SPELL_TRANSFORM                             = 58668
};

enum Yells
{
    SAY_AGGRO                                   = 0,
    SAY_SLAY                                    = 1,
    SAY_DEATH                                   = 2,
    SAY_SPAWN                                   = 3,
    SAY_DISRUPTION                              = 4,
    SAY_BREATH_ATTACK                           = 5,
    SAY_SPECIAL_ATTACK                          = 6
};

class boss_cyanigosa : public CreatureScript
{
    public:
        boss_cyanigosa() : CreatureScript("boss_cyanigosa") { }

        struct boss_cyanigosaAI : public BossAI
        {
            boss_cyanigosaAI(Creature* creature) : BossAI(creature, DATA_CYANIGOSA) { }

            void EnterCombat(Unit* who) override
            {
                BossAI::EnterCombat(who);
                Talk(SAY_AGGRO);
            }

            void KilledUnit(Unit* victim) override
            {
                if (victim->GetTypeId() == TYPEID_PLAYER)
                    Talk(SAY_SLAY);
            }

            void JustDied(Unit* killer) override
            {
                BossAI::JustDied(killer);
                Talk(SAY_DEATH);
            }

            void MoveInLineOfSight(Unit* /*who*/) override { }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                scheduler.Update(diff,
                    std::bind(&BossAI::DoMeleeAttackIfReady, this));
            }

            void ScheduleTasks() override
            {
                scheduler.Schedule(Seconds(10), [this](TaskContext task)
                {
                    DoCastAOE(SPELL_ARCANE_VACUUM);
                    task.Repeat();
                });

                scheduler.Schedule(Seconds(15), [this](TaskContext task)
                {
                    if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 45.0f, true))
                        DoCast(target, SPELL_BLIZZARD);
                    task.Repeat();
                });

                scheduler.Schedule(Seconds(20), [this](TaskContext task)
                {
                    DoCastVictim(SPELL_TAIL_SWEEP);
                    task.Repeat();
                });

                scheduler.Schedule(Seconds(25), [this](TaskContext task)
                {
                    DoCastVictim(SPELL_UNCONTROLLABLE_ENERGY);
                    task.Repeat();
                });

                if (IsHeroic())
                {
                    scheduler.Schedule(Seconds(30), [this](TaskContext task)
                    {
                        if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 50.0f, true))
                            DoCast(target, SPELL_MANA_DESTRUCTION);
                        task.Repeat();
                    });
                }
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetVioletHoldAI<boss_cyanigosaAI>(creature);
        }
};

class achievement_defenseless : public AchievementCriteriaScript
{
    public:
        achievement_defenseless() : AchievementCriteriaScript("achievement_defenseless") { }

        bool OnCheck(Player* /*player*/, Unit* target) override
        {
            if (!target)
                return false;

            InstanceScript* instance = target->GetInstanceScript();
            if (!instance)
                return false;

            return instance->GetData(DATA_DEFENSELESS) != 0;
        }
};

class spell_cyanigosa_arcane_vacuum : public SpellScriptLoader
{
    public:
        spell_cyanigosa_arcane_vacuum() : SpellScriptLoader("spell_cyanigosa_arcane_vacuum") { }

        class spell_cyanigosa_arcane_vacuum_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_cyanigosa_arcane_vacuum_SpellScript);

            bool Validate(SpellInfo const* /*spellInfo*/) override
            {
                return ValidateSpellInfo({ SPELL_SUMMON_PLAYER });
            }

            void HandleScript(SpellEffIndex /*effIndex*/)
            {
                GetCaster()->CastSpell(GetHitUnit(), SPELL_SUMMON_PLAYER, true);
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_cyanigosa_arcane_vacuum_SpellScript::HandleScript, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_cyanigosa_arcane_vacuum_SpellScript();
        }
};

void AddSC_boss_cyanigosa()
{
    new boss_cyanigosa();
    new achievement_defenseless();
    new spell_cyanigosa_arcane_vacuum();
}
