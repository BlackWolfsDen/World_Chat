/*
a simple chat system with an adjustable prefix. default`.chat`.
with adjustable color layout and adjustable channel name.
made by slp13at420 of EmuDevs.com
*/

#include "Chat.h"
#include "Config.h"
#include "ChannelMgr.h"
#include "Player.h"
#include "ScriptMgr.h"
#include "World.h"

std::string wcc_channel_name = "World";
uint32 wcc_channel_id;
uint64 wcc_delay;
uint8 wcc_type;

struct WorldChannelChatElements
{
	uint64 time;
	std::string last_msg;
};

std::unordered_map<uint32, WorldChannelChatElements>WorldChannelChat;

std::string wcc_TeamIcon[2] =
{
	"|TInterface\\icons\\Inv_Misc_Tournaments_banner_Human.png:13|t",
	"|TInterface\\icons\\Inv_Misc_Tournaments_banner_Orc.png:13|t"
};

std::string wcc_GM_ICON = "|TINTERFACE/CHATFRAME/UI-CHATICON-BLIZZ:13:13:0:-1|t";

std::string wcc_GM_RANK[6] =
{
	"Player",
	"GM1",
	"GM2",
	"Lead GM",
	"Admin",
}; // if you have less/more ranks then -/+ as necessary. edit rank names as necessary.

std::string wcc_ClassIcon[11] =
{
	"|TInterface\\icons\\INV_Sword_27.png:13|t",
	"|TInterface\\icons\\INV_Hammer_01.png:13|t",
	"|TInterface\\icons\\INV_Weapon_Bow_07.png:13|t",
	"|TInterface\\icons\\INV_ThrowingKnife_04.png:13|t",
	"|TInterface\\icons\\INV_Staff_30.png:13|t",
	"|TInterface\\icons\\Spell_Deathknight_ClassIcon.png:13|t",
	"|TInterface\\icons\\inv_jewelry_talisman_04.png:13|t",
	"|TInterface\\icons\\INV_Staff_30.png:13|t",
	"|TInterface\\icons\\INV_Staff_30.png:13|t",
	"",
	"|TInterface\\icons\\Ability_Druid_Maul.png:13|t",
};

std::string wcc_allyblue = "|cff3399FF";
std::string wcc_hordered = "|cffCC0000";
std::string wcc_white = "|cffFFFFFF";
std::string wcc_green = "|cff00cc00";
std::string wcc_red = "|cffFF0000";
std::string wcc_blue = "|cff6666FF";
std::string wcc_black = "|cff000000";
std::string wcc_grey = "|cff808080";

std::string wcc_TeamColor[2] = { wcc_allyblue, wcc_hordered };

class WORLD_CHANNEL_CHAT_Load_Conf : public WorldScript
{
public:
	WORLD_CHANNEL_CHAT_Load_Conf() : WorldScript("WORLD_CHANNEL_CHAT_Load_Conf"){ };

	virtual void OnConfigLoad(bool /*reload*/)
	{
		wcc_channel_id = sConfigMgr->GetIntDefault("WORLDCHANNELCHAT.CHANNEL", 1);
		wcc_delay = sConfigMgr->GetIntDefault("WORLDCHANNELCHAT.DELAY", 5);
		wcc_type = sConfigMgr->GetIntDefault("WORLDCHANNELCHAT.TYPE", 1);
	}
};

void SendWorldChatChannelMessage(std::string msg, uint8 team_id)
{
	SessionMap sessions = sWorld->GetAllSessions();

	for (SessionMap::iterator itr = sessions.begin(); itr != sessions.end(); ++itr)
	{
		if (!itr->second)
			continue;

		Player *player = itr->second->GetPlayer();

		if (wcc_type == 0 || ((wcc_type == 1) && (player->GetTeamId() == team_id))) 
		{ 
			ChatHandler(player->GetSession()).PSendSysMessage(msg.c_str()); 
		}
	}
}

class WORLD_CHANNEL_CHAT : public PlayerScript
{
public:
	WORLD_CHANNEL_CHAT() : PlayerScript("WORLD_CHANNEL_CHAT") { }

	void OnLogin(Player* player, bool /*firstLogin*/) override
	{
		ChatHandler(player->GetSession()).PSendSysMessage("type `/%u` to access the World Chat channel.", wcc_channel_id);
	}

	virtual void OnChat(Player* player, uint32 /*type*/, uint32 lang, std::string& msg, Channel* channel)
	{
		if (lang != LANG_ADDON)
		{
			if (channel->GetChannelId() == wcc_channel_id)
			{
				if ((msg != "") && (lang != LANG_ADDON) && (msg != "Away") && (player->CanSpeak() == true))
				{
					uint64 current_time = sWorld->GetGameTime();
					uint32 guid = player->GetGUID();

					if (!WorldChannelChat[guid].time)
					{
						WorldChannelChat[guid].time = (current_time - wcc_delay);
						WorldChannelChat[guid].last_msg = "";
					}

					if (player->IsGameMaster()) // here we will set the gm's stored values so they clear the checks.
					{
						WorldChannelChat[guid].time = current_time - wcc_delay;
						WorldChannelChat[guid].last_msg = "";
					}

					if ((current_time - WorldChannelChat[guid].time) < wcc_delay)
					{
						ChatHandler(player->GetSession()).PSendSysMessage("%sSpam timer triggered.", wcc_red);
					}
					else
					{
						if (WorldChannelChat[guid].last_msg == msg)
						{
							ChatHandler(player->GetSession()).PSendSysMessage("%sSpam message detected.", wcc_red);
						}
						else
						{
							// here you can apply different colors
							std::string wcc_channelcolor = wcc_grey;
							std::string wcc_gm_rankcolor = wcc_blue;
							std::string wcc_msgcolor = wcc_green;

							auto gm_rank = player->GetSession()->GetSecurity();
							std::string pName = player->GetName();
							uint8 team_id = player->GetTeamId();

							std::string name = "|Hplayer:" + pName + "|h" + pName;

							std::string WCCMSG = "";

							WCCMSG = "[" + wcc_channelcolor + wcc_channel_name + "|r]";
							WCCMSG = WCCMSG + "[" + wcc_TeamIcon[player->GetTeamId()] + "]";

								if (player->IsGameMaster())
								{

									WCCMSG = WCCMSG + "[" + wcc_GM_ICON + "]";
									WCCMSG = WCCMSG + "[" + wcc_gm_rankcolor + wcc_GM_RANK[gm_rank] + "|r]";
								}

							WCCMSG = WCCMSG + "[" + wcc_ClassIcon[player->getClass() - 1] + "]";
							WCCMSG = WCCMSG + "[" + wcc_TeamColor[player->GetTeamId()] + name + "|r]";
							WCCMSG = WCCMSG + ":" + wcc_msgcolor + msg;

							WorldChannelChat[guid].time = current_time;
							WorldChannelChat[guid].last_msg = msg;

							SendWorldChatChannelMessage(WCCMSG, team_id);
						}
					}
				}
				msg = -1;
			}
		}
	}
};

void AddSC_WorldChannelChat()
{
	new WORLD_CHANNEL_CHAT_Load_Conf;
	new WORLD_CHANNEL_CHAT;
}
