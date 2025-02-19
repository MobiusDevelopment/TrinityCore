/**
 * This file is part of the MobiusCore project.
 * See AUTHORS file for copyright information.
 */

#ifndef BattlePetMgr_h__
#define BattlePetMgr_h__

#include "BattlePetPackets.h"
#include "DatabaseEnvFwd.h"
#include <unordered_map>

enum BattlePetMisc
{
    MAX_PET_BATTLE_SLOTS            = 3,
    MAX_BATTLE_PETS_PER_SPECIES     = 3,
    BATTLE_PET_CAGE_ITEM_ID         = 82800,
    DEFAULT_SUMMON_BATTLE_PET_SPELL = 118301
};

// 6.2.4
enum FlagsControlType
{
    FLAGS_CONTROL_TYPE_APPLY        = 1,
    FLAGS_CONTROL_TYPE_REMOVE       = 2
};

// 6.2.4
enum BattlePetError
{
    BATTLEPETRESULT_CANT_HAVE_MORE_PETS_OF_THAT_TYPE = 3,
    BATTLEPETRESULT_CANT_HAVE_MORE_PETS              = 4,
    BATTLEPETRESULT_TOO_HIGH_LEVEL_TO_UNCAGE         = 7,

    // TODO: find correct values if possible and needed (also wrong order)
    BATTLEPETRESULT_DUPLICATE_CONVERTED_PET,
    BATTLEPETRESULT_NEED_TO_UNLOCK,
    BATTLEPETRESULT_BAD_PARAM,
    BATTLEPETRESULT_LOCKED_PET_ALREADY_EXISTS,
    BATTLEPETRESULT_OK,
    BATTLEPETRESULT_UNCAPTURABLE,
    BATTLEPETRESULT_CANT_INVALID_CHARACTER_GUID
};

// taken from BattlePetState.db2 - it seems to store some initial values for battle pets
// there are only values used in BattlePetSpeciesState.db2 and BattlePetBreedState.db2
// TODO: expand this enum if needed
enum BattlePetState
{
    STATE_MAX_HEALTH_BONUS          = 2,
    STATE_INTERNAL_INITIAL_LEVEL    = 17,
    STATE_STAT_POWER                = 18,
    STATE_STAT_STAMINA              = 19,
    STATE_STAT_SPEED                = 20,
    STATE_MOD_DAMAGE_DEALT_PERCENT  = 23,
    STATE_GENDER                    = 78, // 1 - male, 2 - female
    STATE_COSMETIC_WATER_BUBBLED    = 85,
    STATE_SPECIAL_IS_COCKROACH      = 93,
    STATE_COSMETIC_FLY_TIER         = 128,
    STATE_COSMETIC_BIGGLESWORTH     = 144,
    STATE_PASSIVE_ELITE             = 153,
    STATE_PASSIVE_BOSS              = 162,
    STATE_COSMETIC_TREASURE_GOBLIN  = 176,
    // these are not in BattlePetState.db2 but are used in BattlePetSpeciesState.db2
    STATE_START_WITH_BUFF           = 183,
    STATE_START_WITH_BUFF_2         = 184,
    //
    STATE_COSMETIC_SPECTRAL_BLUE    = 196
};

enum BattlePetSaveInfo
{
    BATTLE_PET_UNCHANGED = 0,
    BATTLE_PET_CHANGED   = 1,
    BATTLE_PET_NEW       = 2,
    BATTLE_PET_REMOVED   = 3
};

class BattlePetMgr
{
public:
    struct BattlePet
    {
        void CalculateStats();

        WorldPackets::BattlePet::BattlePet PacketInfo;
        BattlePetSaveInfo SaveInfo;
    };

    explicit BattlePetMgr(WorldSession* owner);

    static void Initialize();

    static uint16 RollPetBreed(uint32 species);
    static uint8 GetDefaultPetQuality(uint32 species);

    void LoadFromDB(PreparedQueryResult pets, PreparedQueryResult slots);
    void SaveToDB(LoginDatabaseTransaction& trans);

    BattlePet* GetPet(ObjectGuid guid);
    void AddPet(uint32 species, uint32 creatureId, uint16 breed, uint8 quality, uint16 level = 1);
    void RemovePet(ObjectGuid guid);

    uint8 GetPetCount(uint32 species) const;

    WorldPackets::BattlePet::BattlePetSlot* GetSlot(uint8 slot) { return slot < _slots.size() ? &_slots[slot] : nullptr; }
    void UnlockSlot(uint8 slot);

    WorldSession* GetOwner() const { return _owner; }

    uint16 GetTrapLevel() const { return _trapLevel; }
    uint16 GetMaxPetLevel() const;
    std::vector<WorldPackets::BattlePet::BattlePetSlot> const& GetSlots() const { return _slots; }

    void CageBattlePet(ObjectGuid guid);
    void HealBattlePetsPct(uint8 pct);

    void SummonPet(ObjectGuid guid);
    void DismissPet();

    void SendJournal();
    void SendUpdates(std::vector<std::reference_wrapper<BattlePet>> pets, bool petAdded);
    void SendError(BattlePetError error, uint32 creatureId);

private:
    WorldSession* _owner;
    uint16 _trapLevel = 0;
    std::unordered_map<uint64 /*battlePetGuid*/, BattlePet> _pets;
    std::vector<WorldPackets::BattlePet::BattlePetSlot> _slots;

    static void LoadAvailablePetBreeds();
    static void LoadDefaultPetQualities();

    // hash no longer required in C++14
    static std::unordered_map<uint16 /*BreedID*/, std::unordered_map<BattlePetState /*state*/, int32 /*value*/, std::hash<std::underlying_type<BattlePetState>::type> >> _battlePetBreedStates;
    static std::unordered_map<uint32 /*SpeciesID*/, std::unordered_map<BattlePetState /*state*/, int32 /*value*/, std::hash<std::underlying_type<BattlePetState>::type> >> _battlePetSpeciesStates;
    static std::unordered_map<uint32 /*SpeciesID*/, std::unordered_set<uint8 /*breed*/>> _availableBreedsPerSpecies;
    static std::unordered_map<uint32 /*SpeciesID*/, uint8 /*quality*/> _defaultQualityPerSpecies;
};

#endif // BattlePetMgr_h__
