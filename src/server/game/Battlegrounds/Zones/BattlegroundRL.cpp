/**
 * This file is part of the MobiusCore project.
 * See AUTHORS file for copyright information.
 */

#include "BattlegroundRL.h"
#include "Log.h"
#include "Player.h"
#include "WorldStatePackets.h"

BattlegroundRL::BattlegroundRL()
{
    BgObjects.resize(BG_RL_OBJECT_MAX);
}

void BattlegroundRL::StartingEventCloseDoors()
{
    for (uint32 i = BG_RL_OBJECT_DOOR_1; i <= BG_RL_OBJECT_DOOR_2; ++i)
        SpawnBGObject(i, RESPAWN_IMMEDIATELY);
}

void BattlegroundRL::StartingEventOpenDoors()
{
    for (uint32 i = BG_RL_OBJECT_DOOR_1; i <= BG_RL_OBJECT_DOOR_2; ++i)
        DoorOpen(i);

    for (uint32 i = BG_RL_OBJECT_BUFF_1; i <= BG_RL_OBJECT_BUFF_2; ++i)
        SpawnBGObject(i, 60);
}

void BattlegroundRL::HandleAreaTrigger(Player* player, uint32 trigger, bool entered)
{
    if (GetStatus() != STATUS_IN_PROGRESS)
        return;

    switch (trigger)
    {
        case 4696:                                          // buff trigger?
        case 4697:                                          // buff trigger?
            break;
        default:
            Battleground::HandleAreaTrigger(player, trigger, entered);
            break;
    }
}

void BattlegroundRL::FillInitialWorldStates(WorldPackets::WorldState::InitWorldStates& packet)
{
    packet.Worldstates.emplace_back(0xbba, 1);
    Arena::FillInitialWorldStates(packet);
}

bool BattlegroundRL::SetupBattleground()
{
    // gates
    if (!AddObject(BG_RL_OBJECT_DOOR_1, BG_RL_OBJECT_TYPE_DOOR_1, 1293.561f, 1601.938f, 31.60557f, -1.457349f, 0, 0, -0.6658813f, 0.7460576f, RESPAWN_IMMEDIATELY)
        || !AddObject(BG_RL_OBJECT_DOOR_2, BG_RL_OBJECT_TYPE_DOOR_2, 1278.648f, 1730.557f, 31.60557f, 1.684245f, 0, 0, 0.7460582f, 0.6658807f, RESPAWN_IMMEDIATELY)
    // buffs
        || !AddObject(BG_RL_OBJECT_BUFF_1, BG_RL_OBJECT_TYPE_BUFF_1, 1328.719971f, 1632.719971f, 36.730400f, -1.448624f, 0, 0, 0.6626201f, -0.7489557f, 120)
        || !AddObject(BG_RL_OBJECT_BUFF_2, BG_RL_OBJECT_TYPE_BUFF_2, 1243.300049f, 1699.170044f, 34.872601f, -0.06981307f, 0, 0, 0.03489945f, -0.9993908f, 120))
    {
        LOG_ERROR("sql.sql", "BatteGroundRL: Failed to spawn some object!");
        return false;
    }

    return true;
}
