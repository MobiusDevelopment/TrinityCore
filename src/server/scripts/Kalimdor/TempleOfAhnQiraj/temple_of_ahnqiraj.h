/**
 * This file is part of the MobiusCore project.
 * See AUTHORS file for copyright information.
 */

#ifndef DEF_TEMPLE_OF_AHNQIRAJ_H
#define DEF_TEMPLE_OF_AHNQIRAJ_H

#include "CreatureAIImpl.h"

#define AQ40ScriptName "instance_temple_of_ahnqiraj"
#define DataHeader "AQT"

enum AQTDataTypes
{
    DATA_SKERAM             = 1,
    DATA_KRI                = 2,
    DATA_VEM                = 3,
    DATA_VEMISDEAD          = 4,
    DATA_VEM_DEATH          = 5,
    DATA_VEKLOR             = 6,
    DATA_VEKLORISDEAD       = 7,
    DATA_VEKLOR_DEATH       = 8,
    DATA_VEKNILASH          = 9,
    DATA_VEKNILASHISDEAD    = 10,
    DATA_VEKNILASH_DEATH    = 11,
    DATA_BUG_TRIO_DEATH     = 14,
    DATA_CTHUN_PHASE        = 20,
    DATA_VISCIDUS           = 21
};

enum AQTCreatures
{
    BOSS_EYE_OF_CTHUN       = 15589,
    NPC_CTHUN_PORTAL        = 15896,
    NPC_CLAW_TENTACLE       = 15725,
    NPC_EYE_TENTACLE        = 15726,
    NPC_SMALL_PORTAL        = 15904,
    NPC_BODY_OF_CTHUN       = 15809,
    NPC_GIANT_CLAW_TENTACLE = 15728,
    NPC_GIANT_EYE_TENTACLE  = 15334,
    NPC_FLESH_TENTACLE      = 15802,
    NPC_GIANT_PORTAL        = 15910,

    NPC_VISCIDUS            = 15299,
    NPC_GLOB_OF_VISCIDUS    = 15667,

    NPC_SKERAM              = 15263,
    NPC_VEM                 = 15544,
    NPC_KRI                 = 15511,
    NPC_VEKLOR              = 15276,
    NPC_VEKNILASH           = 15275
};

template<typename AI>
inline AI* GetAQ40AI(Creature* creature)
{
    return GetInstanceAI<AI>(creature, AQ40ScriptName);
}

#endif
