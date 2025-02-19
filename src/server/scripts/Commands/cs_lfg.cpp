/**
 * This file is part of the MobiusCore project.
 * See AUTHORS file for copyright information.
 */

#include "ScriptMgr.h"
#include "CharacterCache.h"
#include "Chat.h"
#include "DatabaseEnv.h"
#include "Group.h"
#include "GroupMgr.h"
#include "Language.h"
#include "LFGMgr.h"
#include "ObjectAccessor.h"
#include "ObjectMgr.h"
#include "Player.h"
#include "RBAC.h"

void GetPlayerInfo(ChatHandler* handler, Player* player)
{
    if (!player)
        return;

    ObjectGuid guid = player->GetGUID();
    lfg::LfgDungeonSet dungeons = sLFGMgr->GetSelectedDungeons(guid);

    std::string const& state = lfg::GetStateString(sLFGMgr->GetState(guid));
    handler->PSendSysMessage(LANG_LFG_PLAYER_INFO, player->GetName().c_str(),
        state.c_str(), uint8(dungeons.size()), lfg::ConcatenateDungeons(dungeons).c_str(),
        lfg::GetRolesString(sLFGMgr->GetRoles(guid)).c_str());
}

class lfg_commandscript : public CommandScript
{
public:
    lfg_commandscript() : CommandScript("lfg_commandscript") { }

    std::vector<ChatCommand> GetCommands() const override
    {
        static std::vector<ChatCommand> lfgCommandTable =
        {
            {  "player", rbac::RBAC_PERM_COMMAND_LFG_PLAYER,  false, &HandleLfgPlayerInfoCommand, "" },
            {   "group", rbac::RBAC_PERM_COMMAND_LFG_GROUP,   false, &HandleLfgGroupInfoCommand,  "" },
            {   "queue", rbac::RBAC_PERM_COMMAND_LFG_QUEUE,   true,  &HandleLfgQueueInfoCommand,  "" },
            {   "clean", rbac::RBAC_PERM_COMMAND_LFG_CLEAN,   true,  &HandleLfgCleanCommand,      "" },
            { "options", rbac::RBAC_PERM_COMMAND_LFG_OPTIONS, true,  &HandleLfgOptionsCommand,    "" },
        };

        static std::vector<ChatCommand> commandTable =
        {
            { "lfg", rbac::RBAC_PERM_COMMAND_LFG, true, NULL, "", lfgCommandTable },
        };
        return commandTable;
    }

    static bool HandleLfgPlayerInfoCommand(ChatHandler* handler, char const* args)
    {
        Player* target = NULL;
        std::string playerName;
        if (!handler->extractPlayerTarget((char*)args, &target, NULL, &playerName))
            return false;

        GetPlayerInfo(handler, target);
        return true;
    }

    static bool HandleLfgGroupInfoCommand(ChatHandler* handler, char const* args)
    {
        Player* playerTarget;
        ObjectGuid guidTarget;
        std::string nameTarget;

        ObjectGuid parseGUID = ObjectGuid::Create<HighGuid::Player>(uint64(atoull(args)));

        if (sCharacterCache->GetCharacterNameByGuid(parseGUID, nameTarget))
        {
            playerTarget = ObjectAccessor::FindPlayer(parseGUID);
            guidTarget = parseGUID;
        }
        else if (!handler->extractPlayerTarget((char*)args, &playerTarget, &guidTarget, &nameTarget))
            return false;

        Group* groupTarget = NULL;

        if (playerTarget)
            groupTarget = playerTarget->GetGroup();
        else
        {
            CharacterDatabasePreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_SEL_GROUP_MEMBER);
            stmt->setUInt64(0, guidTarget.GetCounter());
            PreparedQueryResult resultGroup = CharacterDatabase.Query(stmt);
            if (resultGroup)
                groupTarget = sGroupMgr->GetGroupByDbStoreId((*resultGroup)[0].GetUInt32());
        }
        if (!groupTarget)
        {
            handler->PSendSysMessage(LANG_LFG_NOT_IN_GROUP, nameTarget.c_str());
            handler->SetSentErrorMessage(true);
            return false;
        }

        ObjectGuid guid = groupTarget->GetGUID();
        std::string const& state = lfg::GetStateString(sLFGMgr->GetState(guid));
        handler->PSendSysMessage(LANG_LFG_GROUP_INFO, groupTarget->isLFGGroup(),
            state.c_str(), sLFGMgr->GetDungeon(guid));

        Group::MemberSlotList const& members = groupTarget->GetMemberSlots();

        for (Group::MemberSlotList::const_iterator itr = members.begin(); itr != members.end(); ++itr)
        {
            Group::MemberSlot const& slot = *itr;
            Player* p = ObjectAccessor::FindPlayer((*itr).guid);
            if (p)
                GetPlayerInfo(handler, p);
            else
                handler->PSendSysMessage("%s is offline.", slot.name.c_str());
        }

        return true;
    }

    static bool HandleLfgOptionsCommand(ChatHandler* handler, char const* args)
    {
        int32 options = -1;
        if (char* str = strtok((char*)args, " "))
        {
            int32 tmp = atoi(str);
            if (tmp > -1)
                options = tmp;
        }

        if (options != -1)
        {
            sLFGMgr->SetOptions(options);
            handler->PSendSysMessage(LANG_LFG_OPTIONS_CHANGED);
        }
        handler->PSendSysMessage(LANG_LFG_OPTIONS, sLFGMgr->GetOptions());
        return true;
    }

    static bool HandleLfgQueueInfoCommand(ChatHandler* handler, char const* args)
    {
        handler->SendSysMessage(sLFGMgr->DumpQueueInfo(*args != '\0').c_str(), true);
        return true;
    }

    static bool HandleLfgCleanCommand(ChatHandler* handler, char const* /*args*/)
    {
        handler->PSendSysMessage(LANG_LFG_CLEAN);
        sLFGMgr->Clean();
        return true;
    }
};

void AddSC_lfg_commandscript()
{
    new lfg_commandscript();
}
