/**
 * This file is part of the MobiusCore project.
 * See AUTHORS file for copyright information.
 */

/*
 * Ordered alphabetically using scriptname.
 * Scriptnames of files in this file should be prefixed with "npc_pet_mag_".
 */

#include "ScriptMgr.h"
#include "CellImpl.h"
#include "CombatAI.h"
#include "GridNotifiersImpl.h"
#include "MotionMaster.h"
#include "Pet.h"
#include "PetAI.h"
#include "Player.h"
#include "ScriptedCreature.h"

enum MageSpells
{
    SPELL_MAGE_CLONE_ME                 = 45204,
    SPELL_MAGE_MASTERS_THREAT_LIST      = 58838,
    SPELL_MAGE_FROST_BOLT               = 59638,
    SPELL_MAGE_FIRE_BLAST               = 59637
};

enum MirrorImageTimers
{
    TIMER_MIRROR_IMAGE_INIT             = 0,
    TIMER_MIRROR_IMAGE_FROST_BOLT       = 4000,
    TIMER_MIRROR_IMAGE_FIRE_BLAST       = 6000
};

class npc_pet_mage_mirror_image : public CreatureScript
{
    public:
        npc_pet_mage_mirror_image() : CreatureScript("npc_pet_mage_mirror_image") { }

        struct npc_pet_mage_mirror_imageAI : CasterAI
        {
            npc_pet_mage_mirror_imageAI(Creature* creature) : CasterAI(creature) { }

            void Init()
            {
                Unit* owner = me->GetCharmerOrOwner();

                std::list<Unit*> targets;
                Server::AnyUnfriendlyUnitInObjectRangeCheck u_check(me, me, 30.0f);
                Server::UnitListSearcher<Server::AnyUnfriendlyUnitInObjectRangeCheck> searcher(me, targets, u_check);
                Cell::VisitAllObjects(me, searcher, 40.0f);

                Unit* highestThreatUnit = nullptr;
                float highestThreat = 0.0f;
                Unit* nearestPlayer = nullptr;
                for (std::list<Unit*>::const_iterator iter = targets.begin(); iter != targets.end(); ++iter)
                {
                    // Consider only units without CC
                    if (!(*iter)->HasBreakableByDamageCrowdControlAura((*iter)))
                    {
                        // Take first found unit
                        if (!highestThreatUnit && (*iter)->GetTypeId() != TYPEID_PLAYER)
                        {
                            highestThreatUnit = (*iter);
                            continue;
                        }
                        if (!nearestPlayer && ((*iter)->GetTypeId() == TYPEID_PLAYER))
                        {
                            nearestPlayer = (*iter);
                            continue;
                        }
                        // else compare best fit unit with current unit
                        ThreatContainer::StorageType triggers = (*iter)->getThreatManager().getThreatList();
                        for (ThreatContainer::StorageType::const_iterator trig_citr = triggers.begin(); trig_citr != triggers.end(); ++trig_citr)
                        {
                            // Try to find threat referenced to owner
                            if ((*trig_citr)->getTarget() == owner)
                            {
                                // Check if best fit hostile unit hs lower threat than this current unit
                                if (highestThreat < (*trig_citr)->getThreat())
                                {
                                    // If so, update best fit unit
                                    highestThreat = (*trig_citr)->getThreat();
                                    highestThreatUnit = (*iter);
                                    break;
                                }
                            }
                        }
                        // In case no unit with threat was found so far, always check for nearest unit (only for players)
                        if ((*iter)->GetTypeId() == TYPEID_PLAYER)
                        {
                            // If this player is closer than the previous one, update it
                            if (me->GetDistance((*iter)->GetPosition()) < me->GetDistance(nearestPlayer->GetPosition()))
                                nearestPlayer = (*iter);
                        }
                    }
                }
                // Prioritize units with threat referenced to owner
                if (highestThreat > 0.0f && highestThreatUnit)
                        me->Attack(highestThreatUnit, false);
                // If there is no such target, try to attack nearest hostile unit if such exists
                else if (nearestPlayer)
                        me->Attack(nearestPlayer, false);
            }

            bool IsInThreatList(Unit* target)
            {
                Unit* owner = me->GetCharmerOrOwner();

                std::list<Unit*> targets;
                Server::AnyUnfriendlyUnitInObjectRangeCheck u_check(me, me, 30.0f);
                Server::UnitListSearcher<Server::AnyUnfriendlyUnitInObjectRangeCheck> searcher(me, targets, u_check);
                Cell::VisitAllObjects(me, searcher, 40.0f);

                for (std::list<Unit*>::const_iterator iter = targets.begin(); iter != targets.end(); ++iter)
                {
                    if ((*iter) == target)
                    {
                        // Consider only units without CC
                        if (!(*iter)->HasBreakableByDamageCrowdControlAura((*iter)))
                        {
                            ThreatContainer::StorageType triggers = (*iter)->getThreatManager().getThreatList();
                            for (ThreatContainer::StorageType::const_iterator trig_citr = triggers.begin(); trig_citr != triggers.end(); ++trig_citr)
                            {
                                // Try to find threat referenced to owner
                                if ((*trig_citr)->getTarget() == owner)
                                    return true;
                            }
                        }
                    }
                }
                return false;
            }

            void InitializeAI() override
            {
                CasterAI::InitializeAI();
                Unit* owner = me->GetOwner();
                if (!owner)
                    return;

                // here mirror image casts on summoner spell (not present in client dbc) 49866
                // here should be auras (not present in client dbc): 35657, 35658, 35659, 35660 selfcast by mirror images (stats related?)
                // Clone Me!
                owner->CastSpell(me, SPELL_MAGE_CLONE_ME, false);
            }

            void EnterCombat(Unit* who) override
            {
                if (me->GetVictim() && !me->GetVictim()->HasBreakableByDamageCrowdControlAura(me))
                {
                    me->CastSpell(who, SPELL_MAGE_FIRE_BLAST, false);
                    events.ScheduleEvent(SPELL_MAGE_FROST_BOLT, TIMER_MIRROR_IMAGE_INIT);
                    events.ScheduleEvent(SPELL_MAGE_FIRE_BLAST, TIMER_MIRROR_IMAGE_FIRE_BLAST);
                }
                else
                    EnterEvadeMode(EVADE_REASON_OTHER);
            }

            void Reset() override
            {
                events.Reset();
            }

            void UpdateAI(uint32 diff) override
            {
                Unit* owner = me->GetCharmerOrOwner();
                if (!owner)
                    return;

                Unit* target = owner->getAttackerForHelper();

                events.Update(diff);

                // prevent CC interrupts by images
                if (me->GetVictim() && me->EnsureVictim()->HasBreakableByDamageCrowdControlAura(me))
                {
                    me->InterruptNonMeleeSpells(false);
                    return;
                }

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                // assign target if image doesnt have any or the target is not actual
                if (!target || me->GetVictim() != target)
                {
                    Unit* ownerTarget = nullptr;
                    if (Player* ownerPlayer = me->GetCharmerOrOwner()->ToPlayer())
                        ownerTarget = ownerPlayer->GetSelectedUnit();

                    // recognize which victim will be choosen
                    if (ownerTarget && ownerTarget->GetTypeId() == TYPEID_PLAYER)
                    {
                        if (!ownerTarget->HasBreakableByDamageCrowdControlAura(ownerTarget))
                            me->Attack(ownerTarget, false);
                    }
                    else if (ownerTarget && (ownerTarget->GetTypeId() != TYPEID_PLAYER) && IsInThreatList(ownerTarget))
                    {
                        if (!ownerTarget->HasBreakableByDamageCrowdControlAura(ownerTarget))
                            me->Attack(ownerTarget, false);
                    }
                    else
                        Init();
                }

                if (uint32 spellId = events.ExecuteEvent())
                {
                    if (spellId == SPELL_MAGE_FROST_BOLT)
                    {
                        events.ScheduleEvent(SPELL_MAGE_FROST_BOLT, TIMER_MIRROR_IMAGE_FROST_BOLT);
                        DoCastVictim(spellId);
                    }
                    else if (spellId == SPELL_MAGE_FIRE_BLAST)
                    {
                        DoCastVictim(spellId);
                        events.ScheduleEvent(SPELL_MAGE_FIRE_BLAST, TIMER_MIRROR_IMAGE_FIRE_BLAST);
                    }
                }
            }

            // Do not reload Creature templates on evade mode enter - prevent visual lost
            void EnterEvadeMode(EvadeReason /*why*/) override
            {
                if (me->IsInEvadeMode() || !me->IsAlive())
                    return;

                Unit* owner = me->GetCharmerOrOwner();

                me->CombatStop(true);
                if (owner && !me->HasUnitState(UNIT_STATE_FOLLOW))
                {
                    me->GetMotionMaster()->Clear(false);
                    me->GetMotionMaster()->MoveFollow(owner, PET_FOLLOW_DIST, me->GetFollowAngle(), MOTION_SLOT_ACTIVE);
                }
                Init();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_pet_mage_mirror_imageAI(creature);
        }
};

void AddSC_mage_pet_scripts()
{
    new npc_pet_mage_mirror_image();
}
