/**
 * This file is part of the MobiusCore project.
 * See AUTHORS file for copyright information.
 */

#ifndef DEF_THEBLACKMORASS_H
#define DEF_THEBLACKMORASS_H

#include "CreatureAIImpl.h"

#define TBMScriptName "instance_the_black_morass"
#define DataHeader "TBM"

uint32 const EncounterCount             = 2;

enum TBMDataTypes
{
    TYPE_MEDIVH                         = 1,
    TYPE_RIFT                           = 2,
    TYPE_AEONUS                         = 3,
    TYPE_TEMPORUS                       = 4,
    TYPE_CRONO_LORD_DEJA                = 5,
    DATA_MEDIVH                         = 10,
    DATA_PORTAL_COUNT                   = 11,
    DATA_SHIELD                         = 12
};

enum TBMWorldStateIds
{
    WORLD_STATE_BM                      = 2541,
    WORLD_STATE_BM_SHIELD               = 2540,
    WORLD_STATE_BM_RIFT                 = 2784
};

enum TBMQuestIds
{
    QUEST_OPENING_PORTAL                = 10297,
    QUEST_MASTER_TOUCH                  = 9836
};

enum TBMCreatureIds
{
    NPC_MEDIVH                          = 15608,
    NPC_TIME_RIFT                       = 17838,
    NPC_TIME_KEEPER                     = 17918,
    NPC_RIFT_KEEPER                     = 21104,
    NPC_RIFT_LORD                       = 17839,
    NPC_CRONO_LORD_DEJA                 = 17879,
    NPC_TEMPORUS                        = 17880,
    NPC_AEONUS                          = 17881,
    NPC_INFINITE_ASSASIN                = 17835,
    NPC_INFINITE_WHELP                  = 21818,
    NPC_INFINITE_CRONOMANCER            = 17892,
    NPC_INFINITE_EXECUTIONER            = 18994,
    NPC_INFINITE_VANQUISHER             = 18995
};

template<typename AI>
inline AI* GetBlackMorassAI(Creature* creature)
{
    return GetInstanceAI<AI>(creature, TBMScriptName);
}

#endif
