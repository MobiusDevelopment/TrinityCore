/**
 * This file is part of the MobiusCore project.
 * See AUTHORS file for copyright information.
 */

#include "ScriptMgr.h"
#include "Creature.h"
#include "GameObject.h"
#include "InstanceScript.h"
#include "utgarde_keep.h"

DoorData const doorData[] =
{
    { GO_GIANT_PORTCULLIS_1,    DATA_INGVAR,    DOOR_TYPE_PASSAGE },
    { GO_GIANT_PORTCULLIS_2,    DATA_INGVAR,    DOOR_TYPE_PASSAGE },
    { 0,                        0,              DOOR_TYPE_ROOM } // END
};

MinionData const minionData[] =
{
    { NPC_SKARVALD,     DATA_SKARVALD_DALRONN },
    { NPC_DALRONN,      DATA_SKARVALD_DALRONN },
    { 0,                0                     } // END
};

class instance_utgarde_keep : public InstanceMapScript
{
    public:
        instance_utgarde_keep() : InstanceMapScript(UKScriptName, 574) { }

        struct instance_utgarde_keep_InstanceMapScript : public InstanceScript
        {
            instance_utgarde_keep_InstanceMapScript(InstanceMap* map) : InstanceScript(map)
            {
                SetHeaders(DataHeader);
                SetBossNumber(EncounterCount);
                LoadDoorData(doorData);
                LoadMinionData(minionData);
            }

            void OnCreatureCreate(Creature* creature) override
            {
                switch (creature->GetEntry())
                {
                    case NPC_PRINCE_KELESETH:
                        PrinceKelesethGUID = creature->GetGUID();
                        break;
                    case NPC_SKARVALD:
                        SkarvaldGUID = creature->GetGUID();
                        AddMinion(creature, true);
                        break;
                    case NPC_DALRONN:
                        DalronnGUID = creature->GetGUID();
                        AddMinion(creature, true);
                        break;
                    case NPC_INGVAR:
                        IngvarGUID = creature->GetGUID();
                        break;
                    default:
                        break;
                }
            }

            void OnCreatureRemove(Creature* creature) override
            {
                switch (creature->GetEntry())
                {
                    case NPC_SKARVALD:
                    case NPC_DALRONN:
                        AddMinion(creature, false);
                        break;
                    default:
                        break;
                }
            }

            void OnGameObjectCreate(GameObject* go) override
            {
                switch (go->GetEntry())
                {
                    case GO_BELLOW_1:
                        Forges[0].BellowGUID = go->GetGUID();
                        HandleGameObject(ObjectGuid::Empty, Forges[0].Event != NOT_STARTED, go);
                        break;
                    case GO_BELLOW_2:
                        Forges[1].BellowGUID = go->GetGUID();
                        HandleGameObject(ObjectGuid::Empty, Forges[1].Event != NOT_STARTED, go);
                        break;
                    case GO_BELLOW_3:
                        Forges[2].BellowGUID = go->GetGUID();
                        HandleGameObject(ObjectGuid::Empty, Forges[2].Event != NOT_STARTED, go);
                        break;
                    case GO_FORGEFIRE_1:
                        Forges[0].FireGUID = go->GetGUID();
                        HandleGameObject(ObjectGuid::Empty, Forges[0].Event != NOT_STARTED, go);
                        break;
                    case GO_FORGEFIRE_2:
                        Forges[1].FireGUID = go->GetGUID();
                        HandleGameObject(ObjectGuid::Empty, Forges[1].Event != NOT_STARTED, go);
                        break;
                    case GO_FORGEFIRE_3:
                        Forges[2].FireGUID = go->GetGUID();
                        HandleGameObject(ObjectGuid::Empty, Forges[2].Event != NOT_STARTED, go);
                        break;
                    case GO_GLOWING_ANVIL_1:
                        Forges[0].AnvilGUID = go->GetGUID();
                        HandleGameObject(ObjectGuid::Empty, Forges[0].Event != NOT_STARTED, go);
                        break;
                    case GO_GLOWING_ANVIL_2:
                        Forges[1].AnvilGUID = go->GetGUID();
                        HandleGameObject(ObjectGuid::Empty, Forges[1].Event != NOT_STARTED, go);
                        break;
                    case GO_GLOWING_ANVIL_3:
                        Forges[2].AnvilGUID = go->GetGUID();
                        HandleGameObject(ObjectGuid::Empty, Forges[2].Event != NOT_STARTED, go);
                        break;
                    case GO_GIANT_PORTCULLIS_1:
                    case GO_GIANT_PORTCULLIS_2:
                        AddDoor(go, true);
                        break;
                    default:
                        break;
                }
            }

            void OnGameObjectRemove(GameObject* go) override
            {
                switch (go->GetEntry())
                {
                    case GO_GIANT_PORTCULLIS_1:
                    case GO_GIANT_PORTCULLIS_2:
                        AddDoor(go, false);
                        break;
                    default:
                        break;
                }
            }

            ObjectGuid GetGuidData(uint32 type) const override
            {
                switch (type)
                {
                    case DATA_PRINCE_KELESETH:
                        return PrinceKelesethGUID;
                    case DATA_SKARVALD:
                        return SkarvaldGUID;
                    case DATA_DALRONN:
                        return DalronnGUID;
                    case DATA_INGVAR:
                        return IngvarGUID;
                    default:
                        break;
                }

                return ObjectGuid::Empty;
            }

            void SetData(uint32 type, uint32 data) override
            {
                switch (type)
                {
                    case DATA_FORGE_1:
                    case DATA_FORGE_2:
                    case DATA_FORGE_3:
                    {
                        uint8 i = type - DATA_FORGE_1;
                        HandleGameObject(Forges[i].AnvilGUID, data != NOT_STARTED);
                        HandleGameObject(Forges[i].BellowGUID, data != NOT_STARTED);
                        HandleGameObject(Forges[i].FireGUID, data != NOT_STARTED);
                        Forges[i].Event = data;

                        if (data == DONE)
                            SaveToDB();
                        break;
                    }
                    default:
                        break;
                }
            }

            void WriteSaveDataMore(std::ostringstream& data) override
            {
                for (uint8 i = 0; i < 3; ++i)
                    data << Forges[i].Event << ' ';
            }

            void ReadSaveDataMore(std::istringstream& data) override
            {
                for (uint8 i = 0; i < 3; ++i)
                    data >> Forges[i].Event;
            }

        protected:
            ForgeInfo Forges[3];

            ObjectGuid PrinceKelesethGUID;
            ObjectGuid SkarvaldGUID;
            ObjectGuid DalronnGUID;
            ObjectGuid IngvarGUID;
        };

        InstanceScript* GetInstanceScript(InstanceMap* map) const override
        {
           return new instance_utgarde_keep_InstanceMapScript(map);
        }
};

void AddSC_instance_utgarde_keep()
{
    new instance_utgarde_keep();
}
