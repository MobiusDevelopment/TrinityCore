/**
 * This file is part of the MobiusCore project.
 * See AUTHORS file for copyright information.
 */

/* ScriptData
SDName: Instance_The_Eye
SD%Complete: 100
SDComment:
SDCategory: Tempest Keep, The Eye
EndScriptData */

#include "ScriptMgr.h"
#include "Creature.h"
#include "InstanceScript.h"
#include "the_eye.h"

/* The Eye encounters:
0 - Kael'thas event
1 - Al' ar event
2 - Solarian Event
3 - Void Reaver event
*/

DoorData const doorData[] =
{
    { GO_ARCANE_DOOR_LEFT,  DATA_KAELTHAS, DOOR_TYPE_ROOM/*, BOUNDARY_SW  */ },
    { GO_ARCANE_DOOR_RIGHT, DATA_KAELTHAS, DOOR_TYPE_ROOM/*, BOUNDARY_SE  */ },
    {                    0,             0, DOOR_TYPE_ROOM } // END
};

ObjectData const gameObjectData[] =
{
    { GO_KAEL_STATUE_RIGHT,      DATA_KAEL_STATUE_RIGHT     },
    { GO_KAEL_STATUE_LEFT,       DATA_KAEL_STATUE_LEFT      },
    { GO_TEMPEST_BRIDDGE_WINDOW, DATA_TEMPEST_BRIDGE_WINDOW },
    {                         0, 0                          } // END
};

class instance_the_eye : public InstanceMapScript
{
    public:
        instance_the_eye() : InstanceMapScript(TheEyeScriptName, 550) { }

        struct instance_the_eye_InstanceMapScript : public InstanceScript
        {
            instance_the_eye_InstanceMapScript(InstanceMap* map) : InstanceScript(map)
            {
                SetHeaders(DataHeader);
                SetBossNumber(EncounterCount);
                LoadDoorData(doorData);
                LoadObjectData(nullptr, gameObjectData);
            }

            ObjectGuid ThaladredTheDarkener;
            ObjectGuid LordSanguinar;
            ObjectGuid GrandAstromancerCapernian;
            ObjectGuid MasterEngineerTelonicus;
            ObjectGuid Kaelthas;
            ObjectGuid Astromancer;
            ObjectGuid Alar;

            void OnCreatureCreate(Creature* creature) override
            {
                switch (creature->GetEntry())
                {
                    case NPC_SANGUINAR:
                        LordSanguinar = creature->GetGUID();
                        break;
                    case NPC_CAPERNIAN:
                        GrandAstromancerCapernian = creature->GetGUID();
                        break;
                    case NPC_TELONICUS:
                        MasterEngineerTelonicus = creature->GetGUID();
                        break;
                    case NPC_THALADRED:
                        ThaladredTheDarkener = creature->GetGUID();
                        break;
                    case NPC_KAELTHAS:
                        Kaelthas = creature->GetGUID();
                        break;
                    case NPC_HIGH_ASTROMANCER_SOLARIAN:
                        Astromancer = creature->GetGUID();
                        break;
                    case NPC_ALAR:
                        Alar = creature->GetGUID();
                        break;
                    default:
                        break;
                }
            }

            ObjectGuid GetGuidData(uint32 identifier) const override
            {
                switch (identifier)
                {
                case DATA_THALADREDTHEDARKENER:         return ThaladredTheDarkener;
                case DATA_LORDSANGUINAR:                return LordSanguinar;
                case DATA_GRANDASTROMANCERCAPERNIAN:    return GrandAstromancerCapernian;
                case DATA_MASTERENGINEERTELONICUS:      return MasterEngineerTelonicus;
                case DATA_KAELTHAS:                     return Kaelthas;
                case DATA_HIGH_ASTROMANCER_SOLARIAN:    return Astromancer;
                case DATA_ALAR:                         return Alar;
                }
                return ObjectGuid::Empty;
            }
        };

        InstanceScript* GetInstanceScript(InstanceMap* map) const override
        {
            return new instance_the_eye_InstanceMapScript(map);
        }
};
void AddSC_instance_the_eye()
{
    new instance_the_eye;
}
