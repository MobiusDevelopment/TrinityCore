/**
 * This file is part of the MobiusCore project.
 * See AUTHORS file for copyright information.
 */

#include "ObjectMgr.h"
#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "SpellScript.h"
#include "SpellAuraEffects.h"
#include "Player.h"
#include "Vehicle.h"
#include "deadmines.h"

enum Spells
{
    // Glubtok
    SPELL_BLINK                     = 87925,
    SPELL_FISTS_OF_FROST            = 87861,
    SPELL_FISTS_OF_FLAME            = 87859,
    SPELL_TELEPORT                  = 88002,
    SPELL_EMOTE_TALK                = 79506,
    SPELL_EMOTE_ROAR                = 48350,
    SPELL_ARCANE_POWER              = 88009,
    SPELL_ARCANE_FIRE_BEAM          = 88072,
    SPELL_ARCANE_FROST_BEAM         = 88093,
    SPELL_STUN_SELF                 = 88040,
    SPELL_BLOSSOM_TARGETING         = 88140,
    SPELL_FROST_BLOSSOM             = 88169,
    SPELL_FROST_BLOSSOM_VISUAL      = 88165,
    SPELL_FIRE_BLOSSOM              = 88129,
    SPELL_FIRE_BLOSSOM_VISUAL       = 88164,
    SPELL_ARCANE_OVERLOAD           = 88183,
    SPELL_TRANSITION_INVISIBILITY   = 90424,
    SPELL_ARCANE_OVERLOAD_INSTAKILL = 88185,

    // Fire Wall Platter
    SPELL_FIRE_WALL                 = 91398,

    // General Purpose Bunny JMF
    SPELL_ARCANE_OVERLOAD_EXPLOSION = 90520
};

enum Events
{
    EVENT_BLINK = 1,
    EVENT_ELEMENTAL_FISTS,
    EVENT_PHASE_TWO_INTRO_1,
    EVENT_PHASE_TWO_INTRO_2,
    EVENT_ARCANE_POWER,
    EVENT_STUN_SELF,
    EVENT_ANNOUNCE_FIRE_WALL,
    EVENT_FIRE_WALL,
    EVENT_BLOSSOM_TARGETING,
    EVENT_ARCANE_OVERLOAD,
};

enum Phases
{
    PHASE_1 = 1,
    PHASE_2 = 2
};

enum Texts
{
    SAY_AGGRO               = 0,
    SAY_FISTS_OF_FLAME      = 1,
    SAY_FISTS_OF_FROST      = 2,
    SAY_PHASE_TWO_INTRO_1   = 3,
    SAY_PHASE_TWO_INTRO_2   = 4,
    SAY_ARCANE_POWER        = 5,
    SAY_ANNOUNCE_FIRE_WALL  = 6,
    SAY_DEATH               = 7
};

enum Fists
{
    FISTS_OF_FLAME = 0,
    FISTS_OF_FROST = 1
};

enum Data
{
    DATA_CURRENT_BLOSSOM = 1
};

Position const firewallPlatterPos = { -193.4054f, -441.5011f, 54.57029f, 1.833041f };
Position const firewallPlatterSplineEndpoint = { -193.4514f, -441.0169f, 55.70924f };
Position const leftSideDistanceCheck = { -214.413f, -441.664f, 54.547f };

class boss_glubtok : public CreatureScript
{
public:
    boss_glubtok() : CreatureScript("boss_glubtok") { }

    struct boss_glubtokAI : public BossAI
    {
        boss_glubtokAI(Creature* creature) : BossAI(creature, DATA_GLUBTOK)
        {
            Initialize();
        }

        void Initialize()
        {
            _killed = false;
            _allowKill = false;
            _nextBlossomBunny = NPC_FIRE_BLOSSOM_BUNNY;
            _lastFists = FISTS_OF_FLAME;

        }

        void Reset() override
        {
            _Reset();
            Initialize();
            DoSummon(NPC_GLUBTOK_FIREWALL_PLATTER, firewallPlatterSplineEndpoint, 0, TEMPSUMMON_MANUAL_DESPAWN);
            me->SetHover(false);
        }

        void EnterCombat(Unit* /*who*/) override
        {
            _EnterCombat();
            Talk(SAY_AGGRO);
            instance->SendEncounterUnit(ENCOUNTER_FRAME_ENGAGE, me);
            events.SetPhase(PHASE_1);
            events.ScheduleEvent(EVENT_BLINK, Seconds(18), 0, PHASE_1);
        }

        void JustDied(Unit* /*killer*/) override
        {
            _JustDied();
            instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);
        }

        void EnterEvadeMode(EvadeReason /*why*/) override
        {
            _EnterEvadeMode();
            summons.DespawnAll();
            instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);
            _DespawnAtEvade();
        }

        void JustSummoned(Creature* summon) override
        {
            switch (summon->GetEntry())
            {
                case NPC_GLUBTOK_FIREWALL_PLATTER:
                {
                    summon->SetSpeed(MOVE_RUN, 0.2f);
                    float distance = summon->GetPosition().GetExactDist2d(firewallPlatterPos);
                    summon->GetMotionMaster()->MoveCirclePath(firewallPlatterPos.GetPositionX(), firewallPlatterPos.GetPositionY(),
                        firewallPlatterSplineEndpoint.GetPositionZ(), distance, true, 11);
                    break;
                }
                case NPC_FIREWALL_PLATTER_1A:
                case NPC_FIREWALL_PLATTER_1B:
                case NPC_FIREWALL_PLATTER_1C:
                case NPC_FIREWALL_PLATTER_2A:
                case NPC_FIREWALL_PLATTER_2B:
                case NPC_FIREWALL_PLATTER_2C:
                    _firewallDummyGUIDList.insert(summon->GetGUID());
                    break;
                default:
                    break;
            }
            summons.Summon(summon);
        }

        void SpellHitTarget(Unit* target, SpellInfo const* spell) override
        {
            if (spell->Id == SPELL_BLOSSOM_TARGETING)
            {
                DoCast(target, target->GetEntry() == NPC_FIRE_BLOSSOM_BUNNY ? SPELL_FIRE_BLOSSOM : SPELL_FROST_BLOSSOM, true);
                target->CastSpell(target, target->GetEntry() == NPC_FIRE_BLOSSOM_BUNNY ? SPELL_FIRE_BLOSSOM_VISUAL : SPELL_FROST_BLOSSOM_VISUAL, true);
            }
        }

        void DamageTaken(Unit* /*attacker*/, uint32& damage) override
        {
            if (damage >= me->GetHealth() && !_allowKill)
            {
                damage = me->GetHealth() - 1;
                if (!_killed)
                {
                    _killed = true;
                    Talk(SAY_DEATH);
                    events.CancelEvent(EVENT_BLOSSOM_TARGETING);
                    DoCastSelf(SPELL_ARCANE_OVERLOAD, true);
                    events.ScheduleEvent(EVENT_ARCANE_OVERLOAD, Seconds(6));
                    // We really need this here because there are more of those triggers in the instance...
                    std::list<Creature*> units;
                    GetCreatureListWithEntryInGrid(units, me, NPC_GENERAL_PURPOSE_BUNNY_L2, 30.0f);
                    if (!units.empty())
                    {
                        for (auto itr = units.begin(); itr != units.end(); ++itr)
                        {
                            if ((*itr)->GetHomePosition().GetExactDist2d(leftSideDistanceCheck) <= 20.0f)
                                (*itr)->CastSpell((*itr), SPELL_ARCANE_FROST_BEAM);
                            else
                                (*itr)->CastSpell((*itr), SPELL_ARCANE_FIRE_BEAM);
                        }
                    }
                }
            }
        }

        uint32 GetData(uint32 type) const override
        {
            if (type == DATA_CURRENT_BLOSSOM)
                return _nextBlossomBunny;
            return 0;
        }

        void UpdateAI(uint32 diff) override
        {
            if (!UpdateVictim())
                return;

            if (me->GetHealthPct() <= 50.0f && !events.IsInPhase(PHASE_2))
            {
                events.SetPhase(PHASE_2);
                me->SetReactState(REACT_PASSIVE);
                me->AttackStop();
                me->CastStop();
                DoCastSelf(SPELL_TELEPORT, true);
                me->StopMoving();
                me->NearTeleportTo(me->GetHomePosition());
                events.ScheduleEvent(EVENT_PHASE_TWO_INTRO_1, Seconds(3) + Milliseconds(600));
            }

            events.Update(diff);

            if (me->HasUnitState(UNIT_STATE_CASTING))
                return;

            while (uint32 eventId = events.ExecuteEvent())
            {
                switch (eventId)
                {
                    case EVENT_BLINK:
                        if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 50.0f, true, 0))
                        {
                            DoCast(target, SPELL_BLINK);
                            if (IsHeroic())
                                me->getThreatManager().resetAllAggro();

                            events.ScheduleEvent(EVENT_ELEMENTAL_FISTS, Milliseconds(800), 0, PHASE_1);
                            events.Repeat(Seconds(13) + Milliseconds(300));
                        }
                        break;
                    case EVENT_ELEMENTAL_FISTS:
                        switch (_lastFists)
                        {
                            case FISTS_OF_FLAME:
                                Talk(SAY_FISTS_OF_FROST);
                                DoCastSelf(SPELL_FISTS_OF_FROST);
                                _lastFists = FISTS_OF_FROST;
                                break;
                            case FISTS_OF_FROST:
                                Talk(SAY_FISTS_OF_FLAME);
                                DoCastSelf(SPELL_FISTS_OF_FLAME);
                                _lastFists = FISTS_OF_FLAME;
                                break;
                            default:
                                break;
                        }
                        break;
                    case EVENT_PHASE_TWO_INTRO_1:
                        Talk(SAY_PHASE_TWO_INTRO_1);
                        DoCastSelf(SPELL_EMOTE_TALK, true);
                        events.ScheduleEvent(EVENT_PHASE_TWO_INTRO_2, Seconds(2) + Milliseconds(400));
                        break;
                    case EVENT_PHASE_TWO_INTRO_2:
                        Talk(SAY_PHASE_TWO_INTRO_2);
                        DoCastSelf(SPELL_EMOTE_ROAR, true);
                        events.ScheduleEvent(EVENT_ARCANE_POWER, Seconds(2) + Milliseconds(400));
                        break;
                    case EVENT_ARCANE_POWER:
                    {
                        Talk(SAY_ARCANE_POWER);
                        DoCastSelf(SPELL_ARCANE_POWER);
                        me->SetHover(true);

                        // We really need this here because there are more of those triggers in other rooms...
                        std::list<Creature*> units;
                        GetCreatureListWithEntryInGrid(units, me, NPC_GENERAL_PURPOSE_BUNNY_L2, 30.0f);
                        if (!units.empty())
                        {
                            for (auto itr = units.begin(); itr != units.end(); ++itr)
                            {
                                if ((*itr)->GetHomePosition().GetExactDist2d(leftSideDistanceCheck) <= 20.0f)
                                    (*itr)->CastSpell((*itr), SPELL_ARCANE_FROST_BEAM);
                                else
                                    (*itr)->CastSpell((*itr), SPELL_ARCANE_FIRE_BEAM);
                            }
                        }
                        events.ScheduleEvent(EVENT_STUN_SELF, Seconds(2) + Milliseconds(500));
                        events.ScheduleEvent(EVENT_BLOSSOM_TARGETING, Seconds(6));
                        break;
                    }
                    case EVENT_STUN_SELF:
                        DoCastSelf(SPELL_STUN_SELF, true);
                        if (IsHeroic())
                            events.ScheduleEvent(EVENT_ANNOUNCE_FIRE_WALL, Seconds(1) + Milliseconds(500));
                        break;
                    case EVENT_ANNOUNCE_FIRE_WALL:
                        Talk(SAY_ANNOUNCE_FIRE_WALL);
                        events.ScheduleEvent(EVENT_FIRE_WALL, Seconds(1) + Milliseconds(400));
                        break;
                    case EVENT_FIRE_WALL:
                        for (auto itr = _firewallDummyGUIDList.begin(); itr != _firewallDummyGUIDList.end(); itr++)
                            if (Creature* firewallDummy = ObjectAccessor::GetCreature(*me, (*itr)))
                                firewallDummy->CastSpell(firewallDummy, SPELL_FIRE_WALL);
                        break;
                    case EVENT_ARCANE_OVERLOAD:
                        me->SetHover(false);
                        _allowKill = true;
                        DoCastSelf(SPELL_TRANSITION_INVISIBILITY, true);
                        DoCastSelf(SPELL_ARCANE_OVERLOAD_INSTAKILL, true);
                        if (Creature* bunny = me->FindNearestCreature(NPC_GENERAL_PURPOSE_DUMMY_JMF, 5.0f, true))
                            bunny->CastSpell(bunny, SPELL_ARCANE_OVERLOAD_EXPLOSION);
                        break;
                    case EVENT_BLOSSOM_TARGETING:
                        if (_nextBlossomBunny == NPC_FIRE_BLOSSOM_BUNNY)
                            _nextBlossomBunny = NPC_FROST_BLOSSOM_BUNNY;
                        else
                            _nextBlossomBunny = NPC_FIRE_BLOSSOM_BUNNY;

                        DoCastAOE(SPELL_BLOSSOM_TARGETING, true);
                        events.Repeat(Seconds(2) + Milliseconds(400));
                        break;
                    default:
                        break;
                }
            }

            DoMeleeAttackIfReady();
        }
    private:
        bool _killed;
        bool _allowKill;
        uint8 _lastFists;
        uint32 _nextBlossomBunny;
        GuidSet _firewallDummyGUIDList;
    };

    CreatureAI* GetAI(Creature *creature) const override
    {
        return GetDeadminesAI<boss_glubtokAI>(creature);
    }
};

class CreatureEntryCheck
{
    public:
        CreatureEntryCheck(uint32 entry) : _entry(entry) { }

        bool operator()(WorldObject* object)
        {
            return (object->GetEntry() != _entry);
        }
    private:
        uint32 _entry;
};

class spell_glubtok_blossom_targeting : public SpellScriptLoader
{
    public:
        spell_glubtok_blossom_targeting() : SpellScriptLoader("spell_glubtok_blossom_targeting") { }

        class spell_glubtok_blossom_targeting_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_glubtok_blossom_targeting_SpellScript);

            void FilterTargets(std::list<WorldObject*>& targets)
            {
                if (targets.empty())
                    return;

                if (Unit* caster = GetCaster())
                    if (Creature* creature = caster->ToCreature())
                        if (creature->IsAIEnabled)
                            targets.remove_if(CreatureEntryCheck(creature->GetAI()->GetData(DATA_CURRENT_BLOSSOM)));

                if (targets.empty())
                    return;

                Server::Containers::RandomResize(targets, 1);
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_glubtok_blossom_targeting_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENTRY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_glubtok_blossom_targeting_SpellScript();
        }
};

void AddSC_boss_glubtok()
{
    new boss_glubtok();
    new spell_glubtok_blossom_targeting();
}
