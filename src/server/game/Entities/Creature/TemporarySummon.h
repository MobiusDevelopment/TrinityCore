/**
 * This file is part of the MobiusCore project.
 * See AUTHORS file for copyright information.
 */

#ifndef SERVERCORE_TEMPSUMMON_H
#define SERVERCORE_TEMPSUMMON_H

#include "Creature.h"

enum PetEntry
{
    // Warlock pets
    PET_IMP             = 416,
    PET_FEL_HUNTER      = 691,
    PET_VOID_WALKER     = 1860,
    PET_SUCCUBUS        = 1863,
    PET_DOOMGUARD       = 18540,
    PET_FELGUARD        = 30146,

    // Death Knight pets
    PET_GHOUL           = 26125,
    PET_ABOMINATION     = 106848,

    // Shaman pet
    PET_SPIRIT_WOLF     = 29264
};

struct SummonPropertiesEntry;

class GAME_API TempSummon : public Creature
{
    public:
        explicit TempSummon(SummonPropertiesEntry const* properties, Unit* owner, bool isWorldObject);
        virtual ~TempSummon() { }
        void Update(uint32 time) override;
        virtual void InitStats(uint32 lifetime);
        virtual void InitSummon();
        void UpdateObjectVisibilityOnCreate() override;
        virtual void UnSummon(uint32 msTime = 0);
        void RemoveFromWorld() override;
        void SetTempSummonType(TempSummonType type);
        void SaveToDB(uint32 /*mapid*/, std::vector<Difficulty> const& /*spawnDifficulties*/) override { }
        Unit* GetSummoner() const;
        Creature* GetSummonerCreatureBase() const;
        ObjectGuid GetSummonerGUID() const { return m_summonerGUID; }
        TempSummonType const& GetSummonType() { return m_type; }
        uint32 GetTimer() const { return m_timer; }

        void SetVisibleBySummonerOnly(bool visibleBySummonerOnly) { m_visibleBySummonerOnly = visibleBySummonerOnly; }
        bool IsVisibleBySummonerOnly() const { return m_visibleBySummonerOnly; }

        const SummonPropertiesEntry* const m_Properties;
    private:
        TempSummonType m_type;
        uint32 m_timer;
        uint32 m_lifetime;
        ObjectGuid m_summonerGUID;
        bool m_visibleBySummonerOnly;
};

class GAME_API Minion : public TempSummon
{
    public:
        Minion(SummonPropertiesEntry const* properties, Unit* owner, bool isWorldObject);
        void InitStats(uint32 duration) override;
        void RemoveFromWorld() override;
        Unit* GetOwner() const { return m_owner; }
        float GetFollowAngle() const override { return m_followAngle; }
        void SetFollowAngle(float angle) { m_followAngle = angle; }

        // Warlock pets
        bool IsPetImp() const { return GetEntry() == PET_IMP; }
        bool IsPetFelhunter() const { return GetEntry() == PET_FEL_HUNTER; }
        bool IsPetVoidwalker() const { return GetEntry() == PET_VOID_WALKER; }
        bool IsPetSuccubus() const { return GetEntry() == PET_SUCCUBUS; }
        bool IsPetDoomguard() const { return GetEntry() == PET_DOOMGUARD; }
        bool IsPetFelguard() const { return GetEntry() == PET_FELGUARD; }

        // Death Knight pets
        bool IsPetGhoul() const { return GetEntry() == PET_GHOUL; } // Ghoul may be guardian or pet
        bool IsPetAbomination() const { return GetEntry() == PET_ABOMINATION; } // Sludge Belcher dk talent

        // Shaman pet
        bool IsSpiritWolf() const { return GetEntry() == PET_SPIRIT_WOLF; } // Spirit wolf from feral spirits

        bool IsGuardianPet() const;
    protected:
        Unit* const m_owner;
        float m_followAngle;
};

class GAME_API Guardian : public Minion
{
    public:
        Guardian(SummonPropertiesEntry const* properties, Unit* owner, bool isWorldObject);
        void InitStats(uint32 duration) override;
        bool InitStatsForLevel(uint8 level);
        void InitSummon() override;

        bool UpdateStats(Stats stat) override;
        bool UpdateAllStats() override;
        void UpdateResistances(uint32 school) override;
        void UpdateArmor() override;
        void UpdateMaxHealth() override;
        void UpdateMaxPower(Powers power) override;
        void UpdateAttackPowerAndDamage(bool ranged = false) override;
        void UpdateDamagePhysical(WeaponAttackType attType) override;

        int32 GetBonusDamage() const { return m_bonusSpellDamage; }
        void SetBonusDamage(int32 damage);
    protected:
        int32   m_bonusSpellDamage;
        float   m_statFromOwner[MAX_STATS];
};

class GAME_API Puppet : public Minion
{
    public:
        Puppet(SummonPropertiesEntry const* properties, Unit* owner);
        void InitStats(uint32 duration) override;
        void InitSummon() override;
        void Update(uint32 time) override;
        void RemoveFromWorld() override;
};

class GAME_API ForcedUnsummonDelayEvent : public BasicEvent
{
public:
    ForcedUnsummonDelayEvent(TempSummon& owner) : BasicEvent(), m_owner(owner) { }
    bool Execute(uint64 e_time, uint32 p_time) override;

private:
    TempSummon& m_owner;
};
#endif
