/*
a simple chat system with an adjustable prefix. default`.chat`.
with adjustable color layout and adjustable channel name.
made by slp13at420 of EmuDevs.com
*/
#include "Chat.h"
#include "Language.h"
#include "ObjectMgr.h"
#include "Player.h"
#include "RBAC.h"
#include "ScriptMgr.h"
#include <unordered_map>
#include "World.h"

uint32 delay = 3; // spam check timer : in seconds 1 = 1 second.

std::string wc_world_chat_command = "chat"; // Globaly adjusts the command map prefix.
std::string wc_channel_name = "World";

struct ChatElements
{
	uint8 chat;
	uint64 time;
	std::string last_msg;
};

std::unordered_map<uint32, ChatElements>Chat;

std::string wc_TeamIcon[2] =
{
	"|TInterface\\icons\\Inv_Misc_Tournaments_banner_Human.png:13|t",
	"|TInterface\\icons\\Inv_Misc_Tournaments_banner_Orc.png:13|t"
};

std::string wc_GM_ICON = "|TINTERFACE/CHATFRAME/UI-CHATICON-BLIZZ:13:13:0:-1|t";

std::string wc_GM_RANK[6] =
{
	"Player",
	"GM1",
	"GM2",
	"GM3",
	"Lead GM",
	"Admin",
}; // if you have less/more ranks then -/+ as necessary. edit rank names as necessary.

std::string wc_ClassIcon[11] =
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
}; // Class Icons

std::string wc_allyblue = "|cff3399FF";
std::string wc_hordered = "|cffCC0000";
std::string wc_white = "|cffFFFFFF";
std::string wc_green = "|cff00cc00";
std::string wc_red = "|cffFF0000";
std::string wc_blue = "|cff6666FF";
std::string wc_black = "|cff000000";
std::string wc_grey = "|cff808080";

std::string wc_TeamColor[2] = { wc_allyblue, wc_hordered };

class WORLD_CHAT_COMMAND : public CommandScript
{
public:
	WORLD_CHAT_COMMAND() : CommandScript("WORLD_CHAT_COMMAND") { }

	std::vector<ChatCommand> GetCommands() const
	{
		static std::vector<ChatCommand> WorldChatCommandChangeTable =
		{
			{ "on", rbac::RBAC_IN_GRANTED_LIST, true, &HandleOnCommand, "changes `/say` channel to world chat access channel for player." },
			{ "off", rbac::RBAC_IN_GRANTED_LIST, true, &HandleOffCommand, "resets `/say` to normal local `/say` channel for player." },
		};

		static std::vector<ChatCommand> commandTable =
		{
			{ wc_world_chat_command.c_str(), rbac::RBAC_IN_GRANTED_LIST, true, NULL, "World Chat commands.`", WorldChatCommandChangeTable },
		};
		return commandTable;
	}

	static bool HandleOnCommand(ChatHandler* handler, const char* msg)
	{
		Player* player = handler->GetSession()->GetPlayer();
		uint32 guid = player->GetGUID();

		Chat[guid].chat = 1;

		ChatHandler(player->GetSession()).PSendSysMessage("%sWorld Chat on.", wc_green);

		return true;
	};

	static bool HandleOffCommand(ChatHandler* handler, const char* msg)
	{
		Player* player = handler->GetSession()->GetPlayer();
		uint32 guid = player->GetGUID();

		Chat[guid].chat = 0;

		ChatHandler(player->GetSession()).PSendSysMessage("%sWorld Chat off.", wc_green);

		return true;
	};
};

class Player_WorldChat_Chat_Commanded : public PlayerScript
{
public: Player_WorldChat_Chat_Commanded() : PlayerScript("Player_WorldChat_Chat_Commanded"){ }

		virtual void OnChat(Player* player, uint32 type, uint32 lang, std::string& msg)
		{
			uint64 current_time = sWorld->GetGameTime();
			uint32 guid = player->GetGUID();

			if (!Chat[guid].chat) {Chat[guid].chat = 0;};

			if (!Chat[guid].time)
			{ 
				Chat[guid].time = current_time - delay;
				Chat[guid].last_msg = ""; 
			};

			if (player->IsGameMaster()) // here we will set the gm's stored values so they clear the checks.
			{
				Chat[guid].time = current_time - delay;
				Chat[guid].last_msg = "";
			}

			if ((msg != "") && (lang != LANG_ADDON) && (msg != "Away") && (player->CanSpeak() == true) && (Chat[guid].chat == 1))
			{

				if ((current_time < (Chat[guid].time + delay)) || (Chat[guid].last_msg == msg))
				{
					ChatHandler(player->GetSession()).PSendSysMessage("%sSpam detect triggered.", wc_red);
				}
				else
				{
					// here you can apply different colors
					std::string wc_channelcolor = wc_grey;
					std::string wc_gm_rankcolor = wc_blue;
					std::string wc_msgcolor = wc_green;

					Chat[guid].last_msg = msg;
					Chat[guid].time = current_time;

					auto gm_rank = player->GetSession()->GetSecurity();
					std::string pName = player->GetName();

					std::string name = "|Hplayer:" + pName + "|h" + pName;

					std::string WCMSG = "";

					WCMSG += "[" + wc_channelcolor + wc_channel_name + "|r]";
					WCMSG += "[" + wc_TeamIcon[player->GetTeamId()] + "]";

					if (player->IsGameMaster())
					{

						WCMSG += "[" + wc_GM_ICON + "]";
						WCMSG += "[" + wc_gm_rankcolor + wc_GM_RANK[gm_rank] + "|r]";
					}

					WCMSG += "[" + wc_ClassIcon[player->getClass() - 1] + "]";
					WCMSG += "[" + wc_TeamColor[player->GetTeamId()] + name + "|r]";
					WCMSG += ":" + wc_msgcolor + msg;

					sWorld->SendGlobalText(WCMSG.c_str(), NULL);

					msg = -1;
				}
			}
		}
};

void AddSC_WorldChat_Commanded()
{
	new WORLD_CHAT_COMMAND;
	new Player_WorldChat_Chat_Commanded;
}
