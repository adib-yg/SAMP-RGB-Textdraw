// -*- coding: utf-8 -*-
/* 
	Legal:
		Version: MPL 1.1
		
		The contents of this file are subject to the Mozilla Public License Version 
		1.1 the "License"; you may not use this file except in compliance with 
		the License. You may obtain a copy of the License at 
		http://www.mozilla.org/MPL/
		
		Software distributed under the License is distributed on an "AS IS" basis,
		WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
		for the specific language governing rights and limitations under the
		License.

		The Original Code is the RGB Textdraw SA:MP Include.
		
		The Initial Developer of the Original Code is Adib "adib_yg".
		Portions created by the Initial Developer are Copyright (c) 2023
		the Initial Developer. All Rights Reserved.

	Contributor(s):
		-

	Special Thanks to:
		Y_Less for YSI-Includes.

	-----------------------------------------------------------------------------

	***
		This include file (omp_rgb_textdraw.inc) is only compatible with OPEN.MP

	Author:
		Adib

	Contact:
		Telegram:
			@adib_yg
		
		Discord:
			@adib_yg

	Created:
		27 Junuary 2023

	Updated:
		8 September 2023

	Version:
		1.0

	Github Repository:
		https://github.com/adib-yg/SAMP-RGB-Textdraw

	-----------------------------------------------------------------------------

	Functions:

		Global Textdraw:

			native AddRGBToTextDraw(Text:text, bool: color = true, bool: bgcolor = false, bool: boxcolor = false);
			native RemoveRGBFromTextDraw(Text:text);
			native SetRGBTextDrawInfo(Text:text, bool: color, bool: bgcolor, bool: boxcolor);
			native RGBTextDrawShowForPlayer(playerid, Text:text);
			native RGBTextDrawHideForPlayer(playerid, Text:text);

		Player Textdraw:

			native AddRGBToPlayerTextDraw(playerid, PlayerText:text, bool: color = true, bool: bgcolor = false, bool: boxcolor = false);
			native RemoveRGBFromPlayerTextDraw(playerid, PlayerText:text);
			native SetRGBPlayerTextDrawInfo(playerid, PlayerText:text, bool: color, bool: bgcolor, bool: boxcolor);
			native RGBPlayerTextDrawShow(playerid, PlayerText:text);
			native RGBPlayerTextDrawHide(playerid, PlayerText:text);

	-----------------------------------------------------------------------------

	Use:

		#include <YSI_Data\y_iterate> or #include <foreach>

		#include <rgb_textdraw>

		new Text: MyTextDraw_1;

		new PlayerText: MyPlayerTextDraw_1[MAX_PLAYERS];

		public OnGameModeInit() 
		{
			MyTextDraw_1 = TextDrawCreate(1.600000, 448.062896, "box");
		    TextDrawLetterSize(MyTextDraw_1, 0.000000, 0.095000);
		    TextDrawTextSize(MyTextDraw_1, 639.300170, 0.000000);
		    TextDrawAlignment(MyTextDraw_1, TEXT_DRAW_ALIGN:1);
		    TextDrawColour(MyTextDraw_1, -1);
		    TextDrawUseBox(MyTextDraw_1, true);
		    TextDrawBoxColour(MyTextDraw_1, 255);
		    TextDrawSetShadow(MyTextDraw_1, 0);
		    TextDrawBackgroundColour(MyTextDraw_1, 255);
		    TextDrawFont(MyTextDraw_1, TEXT_DRAW_FONT:1);
		    TextDrawSetProportional(MyTextDraw_1, true);

			AddRGBToTextDraw(MyTextDraw_1, .color = false, .bgcolor = false, .boxcolor = true); // We added RGB for boxcolor only

		}

		public OnPlayerConnect(playerid) {
			
			RGBTextDrawShowForPlayer(playerid, MyTextDraw_1); // Showing RGB Textdraw For Player

			// Below Is Example Of Use RGB PlayerTextDraw
			MyPlayerTextDraw_1[playerid] = CreatePlayerTextDraw(playerid, 570.396240, 132.500198, "Hello World!");
		    PlayerTextDrawLetterSize(playerid, MyPlayerTextDraw_1[playerid], 0.534999, 1.634999);
		    PlayerTextDrawTextSize(playerid, MyPlayerTextDraw_1[playerid], 0.000000, 133.000000);
		    PlayerTextDrawAlignment(playerid, MyPlayerTextDraw_1[playerid], TEXT_DRAW_ALIGN:2);
		    PlayerTextDrawColour(playerid, MyPlayerTextDraw_1[playerid], -1);
		    PlayerTextDrawSetShadow(playerid, MyPlayerTextDraw_1[playerid], 8);
		    PlayerTextDrawSetOutline(playerid, MyPlayerTextDraw_1[playerid], 1);
		    PlayerTextDrawBackgroundColour(playerid, MyPlayerTextDraw_1[playerid], 255);
		    PlayerTextDrawFont(playerid, MyPlayerTextDraw_1[playerid], TEXT_DRAW_FONT:0);
		    PlayerTextDrawSetProportional(playerid, MyPlayerTextDraw_1[playerid], true);
		    
		    AddRGBToPlayerTextDraw(playerid, MyPlayerTextDraw_1[playerid], .color = true);
		    
		    RGBPlayerTextDrawShow(playerid, MyPlayerTextDraw_1[playerid]);
		}

	-----------------------------------------------------------------------------

	*** 
		Please #include <YSI_Data\y_iterate> before using rgb_textdraw include

	***
		Change 'RGB_TDS_TIMER_INTERVAL' to change the rgb timer time

	-----------------------------------------------------------------------------
*/

// -----------------------------------------------------------------------------

#if defined _rgbtextdraw_included
	#endinput
#endif

#define _rgbtextdraw_included
#define _rgbtextdraw_version 10 // 1.0

// -----------------------------------------------------------------------------

#if !defined _INC_y_iterate
	#if !defined Iter_Add // if  #include <foreach>  is not included also
		#error y_iterate is missing! Add #include <YSI_Data\y_iterate> before using rgb_textdraw include
	#endif
#endif

// -----------------------------------------------------------------------------

#if !defined RGB_TDS_TIMER_INTERVAL
	#define RGB_TDS_TIMER_INTERVAL 800 // milliseconds
#endif

const bool: _RGB_TD_USE_HOOKED_FUNCS_ = true;

// -----------------------------------------------------------------------------

static 
	red = 255, 
	blue, 
	green,
	final_color_hex,

	timer_id = -1,

	bool: initialise_multi_dimensional,

	Iterator: RGB_TDs<Text:2048>,
	bool: RGB_TextDrawColor[MAX_TEXT_DRAWS],
	bool: RGB_TextDrawBGColor[MAX_TEXT_DRAWS],
	bool: RGB_TextDrawBoxColor[MAX_TEXT_DRAWS],
	Iterator: RGB_TDs_Show[MAX_TEXT_DRAWS]<MAX_PLAYERS>, // multi-dimensional

	Iterator: RGB_PlayerTDs[MAX_PLAYERS]<PlayerText:256>, // multi-dimensional
	bool: RGB_PlayerTextDrawColor[MAX_PLAYERS][MAX_PLAYER_TEXT_DRAWS],
	bool: RGB_PlayerTextDrawBGColor[MAX_PLAYERS][MAX_PLAYER_TEXT_DRAWS],
	bool: RGB_PlayerTextDrawBoxColor[MAX_PLAYERS][MAX_PLAYER_TEXT_DRAWS],
	bool: RGB_PlayerTextDrawShow[MAX_PLAYERS][MAX_PLAYER_TEXT_DRAWS],
	count_player_rgb_textdraw
;

// -----------------------------------------------------------------------------

forward RGBTextdrawsTimer();

// -----------------------------------------------------------------------------

/* Global RGB Text Draws*/

stock AddRGBToTextDraw(Text:text, bool: color = true, bool: bgcolor = false, bool: boxcolor = false) {

	if (Text: text == INVALID_TEXT_DRAW)
		return print("[ERROR] AddRGBToTextDraw(...) Invalid Textdraw ID.");

	if (Iter_Contains(RGB_TDs, Text: text))
		return print("[ERROR] AddRGBToTextDraw(...) Couldn't Add RGB To Textdraw Because This Textdraw Is Already Have RGB.");

	if (!color && !bgcolor && !boxcolor)
		return print("[ERROR] AddRGBToTextDraw(...) Couldn't Add RGB To Textdraw Because 'color', 'bgcolor' and 'boxcolor' is FALSE.");

	if(timer_id == -1) {
		// Start timer on init first rgb textdraw
		timer_id = SetTimer("RGBTextdrawsTimer", RGB_TDS_TIMER_INTERVAL, true);
	}

	new 
		ret = Iter_Add(RGB_TDs, Text: text)
	;

	RGB_TextDrawColor[Text: text] = color;
	RGB_TextDrawBGColor[Text: text] = bgcolor;
	RGB_TextDrawBoxColor[Text: text] = boxcolor;
	return ret;
}

stock RemoveRGBFromTextDraw(Text:text) {

	if (Text: text == INVALID_TEXT_DRAW)
		return print("[ERROR] RemoveRGBFromTextDraw(...) Invalid Textdraw ID.");

	if (!Iter_Contains(RGB_TDs, Text: text))
		return print("[ERROR] RemoveRGBFromTextDraw(...) Couldn't Remove RGB From Textdraw Because This Textdraw Is Not Have RGB.");

	new 
		ret = Iter_Remove(RGB_TDs, Text: text)
	;

	if(timer_id != -1) {
		if(Iter_Count(RGB_TDs) == 0 && count_player_rgb_textdraw == 0) {
			// Stop timer if there is no rgb textdraws
			KillTimer(timer_id);
			timer_id = -1;
		}
	}

	return ret;
}

stock SetRGBTextDrawInfo(Text:text, bool: color = true, bool: bgcolor = false, bool: boxcolor = false) {

	if (Text: text == INVALID_TEXT_DRAW)
		return print("[ERROR] SetRGBTextDrawInfo(...) Invalid Textdraw ID.");

	if (!color && !bgcolor && !boxcolor)
		return print("[ERROR] SetRGBTextDrawInfo(...) Couldn't Set RGB Textdraw Info Because 'color', 'bgcolor' and 'boxcolor' is FALSE.");

	RGB_TextDrawColor[Text: text] = color;
	RGB_TextDrawBGColor[Text: text] = bgcolor;
	RGB_TextDrawBoxColor[Text: text] = boxcolor;

	return 1;
}

stock RGBTextDrawShowForPlayer(playerid, Text:text) {

	if (!Iter_Contains(RGB_TDs, Text: text))
		return TextDrawShowForPlayer(playerid, text);

	if (!(0 <= playerid < MAX_PLAYERS))
		return print("[ERROR] RGBTextDrawShowForPlayer(...) Invalid Player ID.");

	if (Text: text == INVALID_TEXT_DRAW)
		return print("[ERROR] RGBTextDrawShowForPlayer(...) Invalid Textdraw ID.");

	if (Iter_Contains(RGB_TDs_Show[Text: text], playerid))
		return 0;

	new 
		ret = Iter_Add(RGB_TDs_Show[Text: text], playerid)
	;

	return ret;
}

stock RGBTextDrawHideForPlayer(playerid, Text:text) {

	if (!Iter_Contains(RGB_TDs, Text: text))
		return TextDrawHideForPlayer(playerid, text);

	if (!(0 <= playerid < MAX_PLAYERS))
		return print("[ERROR] RGBTextDrawHideForPlayer(...) Invalid Player ID.");

	if (Text: text == INVALID_TEXT_DRAW)
		return print("[ERROR] RGBTextDrawHideForPlayer(...) Invalid Textdraw ID.");

	if (!Iter_Contains(RGB_TDs_Show[Text: text], playerid))
		return 0;

	new 
		ret = Iter_Remove(RGB_TDs_Show[Text: text], playerid)
	;

	TextDrawHideForPlayer(playerid, text);

	return ret;
}

// -----------------------------------------------------------------------------

/* Player RGB Text Draws*/

stock AddRGBToPlayerTextDraw(playerid, PlayerText:text, bool: color = true, bool: bgcolor = false, bool: boxcolor = false) {

	if (!(0 <= playerid < MAX_PLAYERS))
		return print("[ERROR] AddRGBToPlayerTextDraw(...) Invalid Player ID.");

	if (PlayerText: text == INVALID_PLAYER_TEXT_DRAW)
		return print("[ERROR] AddRGBToPlayerTextDraw(...) Invalid Textdraw ID.");

	if (Iter_Contains(RGB_PlayerTDs[playerid], PlayerText: text))
		return print("[ERROR] AddRGBToPlayerTextDraw(...) Couldn't Add RGB To Textdraw Because This Textdraw Is Already Have RGB.");

	if (!color && !bgcolor && !boxcolor)
		return print("[ERROR] AddRGBToPlayerTextDraw(...) Couldn't Add RGB To Textdraw Because 'color', 'bgcolor' and 'boxcolor' is FALSE.");

	if(timer_id == -1) {
		// Start timer on init first rgb textdraw
		timer_id = SetTimer("RGBTextdrawsTimer", RGB_TDS_TIMER_INTERVAL, true);
	}

	new 
		ret = Iter_Add(RGB_PlayerTDs[playerid], PlayerText: text)
	;

	count_player_rgb_textdraw++;

	RGB_PlayerTextDrawColor[playerid][PlayerText: text] = color;
	RGB_PlayerTextDrawBGColor[playerid][PlayerText: text] = bgcolor;
	RGB_PlayerTextDrawBoxColor[playerid][PlayerText: text] = boxcolor;
	return ret;
}

stock RemoveRGBFromPlayerTextDraw(playerid, PlayerText:text) {

	if (!(0 <= playerid < MAX_PLAYERS))
		return print("[ERROR] RemoveRGBFromPlayerTextDraw(...) Invalid Player ID.");

	if (PlayerText: text == INVALID_PLAYER_TEXT_DRAW)
		return print("[ERROR] RemoveRGBFromPlayerTextDraw(...) Invalid Textdraw ID.");

	if (!Iter_Contains(RGB_PlayerTDs[playerid], PlayerText: text))
		return print("[ERROR] RemoveRGBFromPlayerTextDraw(...) Couldn't Remove RGB From Textdraw Because This Textdraw Is Not Have RGB.");

	new 
		ret = Iter_Remove(RGB_PlayerTDs[playerid], PlayerText: text)
	;

	count_player_rgb_textdraw--;

	if(timer_id != -1) {
		if(Iter_Count(RGB_TDs) == 0 && count_player_rgb_textdraw == 0) {
			// Stop timer if there is no rgb textdraws
			KillTimer(timer_id);
			timer_id = -1;
		}
	}

	return ret;
}

stock SetRGBPlayerTextDrawInfo(playerid, PlayerText:text, bool: color = true, bool: bgcolor = false, bool: boxcolor = false) {

	if (!(0 <= playerid < MAX_PLAYERS))
		return print("[ERROR] SetRGBPlayerTextDrawInfo(...) Invalid Player ID.");

	if (PlayerText: text == INVALID_PLAYER_TEXT_DRAW)
		return print("[ERROR] SetRGBPlayerTextDrawInfo(...) Invalid Textdraw ID.");

	if (!color && !bgcolor && !boxcolor)
		return print("[ERROR] SetRGBPlayerTextDrawInfo(...) Couldn't Set RGB Textdraw Info Because 'color', 'bgcolor' and 'boxcolor' is FALSE.");

	RGB_PlayerTextDrawColor[playerid][PlayerText: text] = color;
	RGB_PlayerTextDrawBGColor[playerid][PlayerText: text] = bgcolor;
	RGB_PlayerTextDrawBoxColor[playerid][PlayerText: text] = boxcolor;

	return 1;
}

stock RGBPlayerTextDrawShow(playerid, PlayerText:text) {

	if (0 <= playerid < MAX_PLAYERS) {
		if (!Iter_Contains(RGB_PlayerTDs[playerid], PlayerText: text))
			return PlayerTextDrawShow(playerid, text);
	}
	
	if (!(0 <= playerid < MAX_PLAYERS))
		return print("[ERROR] RGBPlayerTextDrawShow(...) Invalid Player ID.");

	if (PlayerText: text == INVALID_PLAYER_TEXT_DRAW)
		return print("[ERROR] RGBPlayerTextDrawShow(...) Invalid Textdraw ID.");

	return RGB_PlayerTextDrawShow[playerid][PlayerText: text] = true;
}

stock RGBPlayerTextDrawHide(playerid, PlayerText:text) {

	if (0 <= playerid < MAX_PLAYERS) {
		if (!Iter_Contains(RGB_PlayerTDs[playerid], PlayerText: text))
			return PlayerTextDrawHide(playerid, text);
	}

	if (!(0 <= playerid < MAX_PLAYERS))
		return print("[ERROR] RGBPlayerTextDrawHide(...) Invalid Player ID.");

	if (PlayerText: text == INVALID_PLAYER_TEXT_DRAW)
		return print("[ERROR] RGBPlayerTextDrawHide(...) Invalid Textdraw ID.");

	RGB_PlayerTextDrawShow[playerid][PlayerText: text] = false;

	return PlayerTextDrawHide(playerid, text);
}

// -----------------------------------------------------------------------------

public RGBTextdrawsTimer()
{
    if(red == 255 && blue == 0 && green != 255) {
    	green += 15;
    }
    
    if(red != 0 && blue == 0 && green == 255) {
    	red -= 15;
    }
    
    if(red == 0 && blue != 255 && green == 255) {
    	blue += 15;
    }
    
    if(red == 0 && blue == 255 && green != 0) {
    	green -= 15;
    }
    
    if(red != 255 && blue == 255 && green == 0) {
    	red += 15;
    }
    
    if(red == 255 && green == 0 && blue != 0) {
    	blue -= 15;
    }

	
	final_color_hex = red << 24 | green << 16 | blue << 8 | 255;

	/* Global RGB Textdraws */
	if(Iter_Count(RGB_TDs) > 0) {
		foreach(new Text: text : RGB_TDs) {
	 		if(RGB_TextDrawColor[Text: text])
	 			TextDrawColour(Text: text, final_color_hex);

	 		if(RGB_TextDrawBGColor[Text: text])
	 			TextDrawBackgroundColour(Text: text, final_color_hex);

	 		if(RGB_TextDrawBoxColor[Text: text])
	 			TextDrawBoxColour(Text: text, final_color_hex);

	 		foreach(new i : RGB_TDs_Show[Text: text]) 
	 			TextDrawShowForPlayer(i, Text: text);
	 	}
	}

	/* Player RGB Textdraws */
	if(count_player_rgb_textdraw > 0) {
 		foreach(new i : Player) {
	 		if(Iter_Count(RGB_PlayerTDs[i]) > 0) {
		 		foreach(new PlayerText: text : RGB_PlayerTDs[i]) {

			 		if(RGB_PlayerTextDrawColor[i][PlayerText: text])
			 			PlayerTextDrawColour(i, PlayerText: text, final_color_hex);

			 		if(RGB_PlayerTextDrawBGColor[i][PlayerText: text])
			 			PlayerTextDrawBackgroundColour(i, PlayerText: text, final_color_hex);

			 		if(RGB_PlayerTextDrawBoxColor[i][PlayerText: text])
			 			PlayerTextDrawBoxColour(i, PlayerText: text, final_color_hex);

		 			if(RGB_PlayerTextDrawShow[i][PlayerText: text])
		 				PlayerTextDrawShow(i, PlayerText: text);
		 		}
	 		}
	 	}
	}
}

// -----------------------------------------------------------------------------

// hooks

//Hook: OnFilterScriptInit
public OnFilterScriptInit()
{
	if(!initialise_multi_dimensional) {
		initialise_multi_dimensional = true;
		Iter_Init(RGB_TDs_Show);
		Iter_Init(RGB_PlayerTDs);
	}
	#if defined RGBTD_OnFilterScriptInit
		return RGBTD_OnFilterScriptInit();
	#else
		return 1;
	#endif
}
#if defined _ALS_OnFilterScriptInit
	#undef OnFilterScriptInit
#else
	#define _ALS_OnFilterScriptInit
#endif

#define OnFilterScriptInit RGBTD_OnFilterScriptInit
#if defined RGBTD_OnFilterScriptInit
	forward RGBTD_OnFilterScriptInit();
#endif

//Hook: OnGameModeInit
public OnGameModeInit()
{
	if(!initialise_multi_dimensional) {
		initialise_multi_dimensional = true;
		Iter_Init(RGB_TDs_Show);
		Iter_Init(RGB_PlayerTDs);
	}
	#if defined RGBTD_OnGameModeInit
		return RGBTD_OnGameModeInit();
	#else
		return 1;
	#endif
}
#if defined _ALS_OnGameModeInit
	#undef OnGameModeInit
#else
	#define _ALS_OnGameModeInit
#endif

#define OnGameModeInit RGBTD_OnGameModeInit
#if defined RGBTD_OnGameModeInit
	forward RGBTD_OnGameModeInit();
#endif

//Hook: OnPlayerDisconnect
public OnPlayerDisconnect(playerid, reason)
{
	// Hide RGB Textdraws
	foreach(new Text: text : RGB_TDs) {
		if (Iter_Contains(RGB_TDs_Show[Text: text], playerid)) {
			Iter_Remove(RGB_TDs_Show[Text: text], playerid);
		}
	}

	// Hide Player RGB Textdraws
	if(count_player_rgb_textdraw > 0) {
		if(Iter_Count(RGB_PlayerTDs[playerid]) > 0) {
			foreach(new PlayerText: text : RGB_PlayerTDs[playerid]) {
				RGB_PlayerTextDrawShow[playerid][PlayerText: text] = false;
				
				count_player_rgb_textdraw--;
			}

			// Clear Player Added RGB Textdraws
			Iter_Clear(RGB_PlayerTDs[playerid]);

			if(timer_id != -1) {
				if(Iter_Count(RGB_TDs) == 0 && count_player_rgb_textdraw == 0) {
					// Stop timer if there is no rgb textdraws
					KillTimer(timer_id);
					timer_id = -1;
				}
			}
		}
	}

	#if defined RGBTD_OnPlayerDisconnect
	    return RGBTD_OnPlayerDisconnect(playerid, reason);
	#else
	    return 0;
	#endif
}

#if defined _ALS_OnPlayerDisconnect
	#undef OnPlayerDisconnect
#else
	#define _ALS_OnPlayerDisconnect
#endif

#define OnPlayerDisconnect RGBTD_OnPlayerDisconnect

#if defined RGBTD_OnPlayerDisconnect
	forward RGBTD_OnPlayerDisconnect(playerid, reason);
#endif

// -----------------------------------------------------------------------------

// hooked functions ()

#if _RGB_TD_USE_HOOKED_FUNCS_

	#if defined _ALS_TextDrawHideForPlayer
		#undef TextDrawHideForPlayer
	#else
		#define _ALS_TextDrawHideForPlayer
	#endif

	#if defined _ALS_TextDrawShowForPlayer
		#undef TextDrawShowForPlayer
	#else
		#define _ALS_TextDrawShowForPlayer
	#endif

	#if defined _ALS_PlayerTextDrawHide
		#undef PlayerTextDrawHide
	#else
		#define _ALS_PlayerTextDrawHide
	#endif

	#if defined _ALS_PlayerTextDrawShow
		#undef PlayerTextDrawShow
	#else
		#define _ALS_PlayerTextDrawShow
	#endif

	#define TextDrawHideForPlayer 		RGBTextDrawHideForPlayer
	#define TextDrawShowForPlayer 		RGBTextDrawShowForPlayer
	#define PlayerTextDrawHide 			RGBPlayerTextDrawHide
	#define PlayerTextDrawShow 			RGBPlayerTextDrawShow

#endif