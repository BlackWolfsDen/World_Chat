/*
a simple chat system with an adjustable prefix. default`.chat`.
with adjustable color layout and adjustable channel name.
made by slp13at420 of EmuDevs.com

use `.chat hello world`.
*/

#include "Chat.h"
#include "Player.h"
#include "ScriptMgr.h"
#include "World.h"

std::string wc1_world_chat_command = "chat";
std::string wc1_channel_name = "World";

std::string wc1_TeamIcon[2] =
{
	"|TInterface\\icons\\Inv_Misc_Tournaments_banner_Human.png:13|t",
	"|TInterface\\icons\\Inv_Misc_Tournaments_banner_Orc.png:13|t"
};

std::string wc1_GM_ICON = "|TINTERFACE/CHATFRAME/UI-CHATICON-BLIZZ:13:13:0:-1|t";

std::string wc1_GM_RANK[6] =
{
	"Player",
	"GM1",
	"GM2",
	"GM3",
	"Lead GM",
	"Admin",
}; // if you have less/more ranks then -/+ as necessary. edit rank names as necessary.

std::string wc1_ClassIcon[11] =
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

std::string wc1_allyblue = "|cff3399FF";
std::string wc1_hordered = "|cffCC0000";
std::string wc1_white = "|cffFFFFFF";
std::string wc1_green = "|cff00cc00";
std::string wc1_red = "|cffFF0000";
std::string wc1_blue = "|cff6666FF";
std::string wc1_black = "|cff000000";
std::string wc1_grey = "|cff808080";

std::string wc1_TeamColor[2] = { wc1_allyblue, wc1_hordered };

class Player_WorldChat : public PlayerScript
{
public:
	Player_WorldChat() : PlayerScript("Player_WorldChat") { }

	void OnLogin(Player* player, bool /*firstLogin*/) override
	{
		ChatHandler(player->GetSession()).PSendSysMessage("type `.%s` for World Chat commands.", wc1_world_chat_command.c_str());
	}
};

class WORLD_CHAT : public CommandScript
{
public:
	WORLD_CHAT() : CommandScript("WORLD_CHAT") { }

	std::vector<ChatCommand> GetCommands() const
	{
		static std::vector<ChatCommand> commandTable =
		{
			{ wc1_world_chat_command.c_str(), rbac::RBAC_IN_GRANTED_LIST, true, &HandleWorldChatCommand, "World Chat prefix. `.chat hello world`" },
		};
		return commandTable;
	}

	static bool HandleWorldChatCommand(ChatHandler* handler, const char* msg)
	{
		// here you can apply different colors
		std::string wc1_channelcolor = wc1_grey;
		std::string wc1_gm_rankcolor = wc1_blue;
		std::string wc1_msgcolor = wc1_green;

		WorldSession* session = handler->GetSession();
		Player* player = session->GetPlayer();
		auto gm_rank = session->GetSecurity();
		std::string pName = player->GetName();

		std::string name = "|Hplayer:" + pName + "|h" + pName;

		std::string WCMSG = "";

		WCMSG = "[" + wc1_channelcolor + wc1_channel_name + "|r]";
		WCMSG = WCMSG + "[" + wc1_TeamIcon[player->GetTeamId()] + "]";

		if (player->IsGameMaster())
		{

			WCMSG = WCMSG + "[" + wc1_GM_ICON + "]";
			WCMSG = WCMSG + "[" + wc1_gm_rankcolor + wc1_GM_RANK[gm_rank] + "|r]";
		}

		WCMSG = WCMSG + "[" + wc1_ClassIcon[player->getClass() - 1] + "]";
		WCMSG = WCMSG + "[" + wc1_TeamColor[player->GetTeamId()] + name + "|r]";
		WCMSG = WCMSG + ":" + wc1_msgcolor + msg;

		sWorld->SendGlobalText(WCMSG.c_str(), NULL);

		return true;
	}
};

void AddSC_WorldChat()
{
	new Player_WorldChat;
	new WORLD_CHAT;
}
