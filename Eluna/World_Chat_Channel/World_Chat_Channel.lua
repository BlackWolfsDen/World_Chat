
-- by slp13at420 of EmuDevs.com
-- simple world chat WITHOUT the '#chat' command...WTF you say?
-- just change to /1 channel . you can change the channel id below.
-- and chat away.
-- names are clickable for whispers and sub menu.

local channel_name = "World Chat";
local channel_id = 1;
local duration = 5; -- in seconds.
local WorldChannelChat = {};

local Colors =  { -- colors for names and misc
	[0] = "|cff3399FF", -- blue for alliance name
	[1] = "|cffFF0000", -- red for horde name
	[3] = "|cff000000", -- black for [channel name]
	[4] = "|cff00cc00", -- green for "message"
	[5] = "|cff3399ff", -- good responce
	[6] = "|cffFF0000", -- bad responce
		};
	
local function ChatSystem(event, player, msg, Type, lang, channel)

local id = player:GetGUIDLow();

	if not(WorldChannelChat[id])then
	
		WorldChannelChat[id] = {
						time = GetGameTime()-duration,
						last_message = "",
		};
	end

	if(channel == channel_id)then
	
		if(lang ~= -1)then
		
			if(msg ~= "")then
				
				if(msg ~= "Away")then
					
					local time = GetGameTime();
					
					if(msg ~= WorldChannelChat[id].last_message)then
					
						if((time-WorldChannelChat[id].time >= duration))then
						
							local t = table.concat{"[", Colors[3], channel_name, "|r]", "[", Colors[player:GetTeam()],"|Hplayer:", player:GetName(),  "|h", player:GetName(), "|h", "|r]:", Colors[4], msg, "|r"};
							SendWorldMessage(t);
							WorldChannelChat[id].time = time;
							WorldChannelChat[id].last_message = msg;
							
							return false;
						else
					    	player:SendBroadcastMessage(Colors[6].."World chat spam timer triggered.|r")
							return false;
						end				
					else
				    	player:SendBroadcastMessage(Colors[6].."World chat spam detected.|r")
						return false;
					end				
				end
			end
		end
	end
end
	
RegisterPlayerEvent(22, ChatSystem)

print("Grumbo'z World Chat Channel loaded.")
