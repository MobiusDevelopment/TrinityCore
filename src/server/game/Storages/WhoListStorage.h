/**
 * This file is part of the MobiusCore project.
 * See AUTHORS file for copyright information.
 */

#ifndef _WHOLISTSTORAGE_H
#define _WHOLISTSTORAGE_H

#include "Common.h"
#include "ObjectGuid.h"

class WhoListPlayerInfo
{
public:
    WhoListPlayerInfo(ObjectGuid guid, uint32 team, AccountTypes security, uint8 level, uint8 clss, uint8 race, uint32 zoneid, uint8 gender, bool visible, bool gamemaster, std::wstring const& widePlayerName,
        std::wstring const& wideGuildName, std::string const& playerName, std::string const& guildName, ObjectGuid guildguid) :
        _guid(guid), _team(team), _security(security), _level(level), _class(clss), _race(race), _zoneid(zoneid), _gender(gender), _visible(visible),
        _gamemaster(gamemaster), _widePlayerName(widePlayerName), _wideGuildName(wideGuildName), _playerName(playerName), _guildName(guildName), _guildguid(guildguid) {}

    ObjectGuid GetGuid() const { return _guid; }
    uint32 GetTeam() const { return _team; }
    AccountTypes GetSecurity() const { return _security; }
    uint8 GetLevel() const { return _level; }
    uint8 GetClass() const { return _class; }
    uint8 GetRace() const { return _race; }
    uint32 GetZoneId() const { return _zoneid; }
    uint8 GetGender() const { return _gender; }
    bool IsVisible() const { return _visible; }
    bool IsGameMaster() const { return _gamemaster; }
    std::wstring const& GetWidePlayerName() const { return _widePlayerName; }
    std::wstring const& GetWideGuildName() const { return _wideGuildName; }
    std::string const& GetPlayerName() const { return _playerName; }
    std::string const& GetGuildName() const { return _guildName; }
    ObjectGuid GetGuildGuid() const { return _guildguid; }

private:
    ObjectGuid _guid;
    uint32 _team;
    AccountTypes _security;
    uint8 _level;
    uint8 _class;
    uint8 _race;
    uint32 _zoneid;
    uint8 _gender;
    bool _visible;
    bool _gamemaster;
    std::wstring _widePlayerName;
    std::wstring _wideGuildName;
    std::string _playerName;
    std::string _guildName;
    ObjectGuid _guildguid;
};

typedef std::vector<WhoListPlayerInfo> WhoListInfoVector;

class GAME_API WhoListStorageMgr
{
private:
    WhoListStorageMgr() { };
    ~WhoListStorageMgr() { };

public:
    static WhoListStorageMgr* instance();

    void Update();
    WhoListInfoVector const& GetWhoList() const { return _whoListStorage; }

protected:
    WhoListInfoVector _whoListStorage;
};

#define sWhoListStorageMgr WhoListStorageMgr::instance()

#endif // _WHOLISTSTORAGE_H
