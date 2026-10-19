

// * Settings * //
#define 	MAX_NOTIFY 		(10) // max show notofication
#define 	MAX_NT_STRING 	(320)
#define 	MAX_NT_TITLE 	(64)
#define 	SECONDS_NT 		(5)

enum ntInfo {ntTitle[MAX_NT_TITLE], ntMessage[MAX_NT_STRING], ntLines }
static notifyInfo[MAX_PLAYERS][MAX_NOTIFY][ntInfo],
PlayerText:TweetNotif[MAX_PLAYERS][MAX_NOTIFY * 8],
notifyIndex[MAX_PLAYERS];

#define notify::%0(%1) forward %0(%1); public %0(%1)
#define InternalNotification_show InternalNotification_Show
#define notification.	InternalNotification_

static const TDTextCaracterWidth[] = {
	0,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,
	12,12,12,12,12,12,12,13,13,28,28,28,28,8,17,17,30,28,28,12,9,21,28,14,28,28,
	28,28,28,28,28,28,13,13,30,30,30,30,10,25,23,21,24,22,20,24,24,17,20,22,20,
	30,27,27,26,26,24,23,24,31,23,31,24,23,21,28,33,33,14,28,10,11,12,9,11,10,
	10,12,12,7,7,13,5,18,12,10,12,11,10,12,8,13,13,18,17,13,12,30,30,37,35,37,
	25,25,25,25,33,21,24,24,24,24,17,17,17,17,27,27,27,27,31,31,31,31,11,11,11,
	11,11,20,9,10,10,10,10,7,7,7,7,10,10,10,10,13,13,13,13,27,12,30
};


// Get width for char //
stock getSizeMessage(const string:message[], maxWidth=500) {
	new size = 0, lines=1, i=-1, lastPoint = 0;

	while(message[++i]) {
		size += TDTextCaracterWidth[message[i]];

		switch(message[i]) {
			case ' ': 
				lastPoint = i;

			default: 
				if(size >= maxWidth) 
					++lines, 
					size -= maxWidth, 
					size += i - lastPoint;
		}
	}
	return lines;
}

//main function for show notification
stock InternalNotification_Show(const playerid, const string:title[], const string:message[])
{
	for(new x=-1;++x<notifyIndex[playerid];) {
		for(new i=-1;++i<8;) PlayerTextDrawDestroy(playerid, TweetNotif[playerid][(x*8) + i]);
		notifyInfo[playerid][notifyIndex[playerid]-x] = notifyInfo[playerid][(notifyIndex[playerid]-x)-1];
	}

	strmid(notifyInfo[playerid][0][ntTitle], title, 0, MAX_NT_TITLE);
	strmid(notifyInfo[playerid][0][ntMessage], message, 0, MAX_NT_STRING);
	notifyInfo[playerid][0][ntLines] = getSizeMessage(message);

	++notifyIndex[playerid];

	new Float:static_x=0.0;
	for(new x=-1;++x<notifyIndex[playerid];) {
		createNotifyTD(playerid, x, x * 8, static_x);
		static_x+=(notifyInfo[playerid][x][ntLines] * 7.5) + 20.0;
	}

	SetTimerEx(#destroy_notify, SECONDS_NT * 700, false, #i, playerid);
	return 1;
}

// when the notification stops //
notify::destroy_notify(playerid) {
	if(!notifyIndex[playerid]) return 1;
	--notifyIndex[playerid];
	for(new i=-1;++i<8;) PlayerTextDrawDestroy(playerid, TweetNotif[playerid][(notifyIndex[playerid]*8) + i]);
	return 1;
}

// display notification //
stock createNotifyTD(const playerid, index, i, const Float:static_x) {
	new lines = notifyInfo[playerid][index][ntLines],Float:x = (lines * 7.5) + static_x;

	TweetNotif[playerid][i] = CreatePlayerTextDraw(playerid, 80.000000, 284.000000 - x, "_");
	PlayerTextDrawFont(playerid, TweetNotif[playerid][i], 1);
	PlayerTextDrawLetterSize(playerid, TweetNotif[playerid][i], 0.575000, 3.699999);
	PlayerTextDrawTextSize(playerid, TweetNotif[playerid][i], 293.500000, 142.000000);
	PlayerTextDrawSetOutline(playerid, TweetNotif[playerid][i], 1);
	PlayerTextDrawSetShadow(playerid, TweetNotif[playerid][i], 0);
	PlayerTextDrawAlignment(playerid, TweetNotif[playerid][i], 2);
	PlayerTextDrawColor(playerid, TweetNotif[playerid][i], -1);
	PlayerTextDrawBackgroundColor(playerid, TweetNotif[playerid][i], 255);
	PlayerTextDrawBoxColor(playerid, TweetNotif[playerid][i], 66);
	PlayerTextDrawUseBox(playerid, TweetNotif[playerid][i], 1);
	PlayerTextDrawSetProportional(playerid, TweetNotif[playerid][i], 1);
	PlayerTextDrawSetSelectable(playerid, TweetNotif[playerid][i], 0);
	PlayerTextDrawShow(playerid, TweetNotif[playerid][i]);

	TweetNotif[playerid][++i] = CreatePlayerTextDraw(playerid, 80.000000, 281.000000 - x, "_");
	PlayerTextDrawFont(playerid, TweetNotif[playerid][i], 1);
	PlayerTextDrawLetterSize(playerid, TweetNotif[playerid][i], 0.575000, -0.149999);
	PlayerTextDrawTextSize(playerid, TweetNotif[playerid][i], 293.500000, 142.000000);
	PlayerTextDrawSetOutline(playerid, TweetNotif[playerid][i], 1);
	PlayerTextDrawSetShadow(playerid, TweetNotif[playerid][i], 0);
	PlayerTextDrawAlignment(playerid, TweetNotif[playerid][i], 2);
	PlayerTextDrawColor(playerid, TweetNotif[playerid][i], -1);
	PlayerTextDrawBackgroundColor(playerid, TweetNotif[playerid][i], 255);
	PlayerTextDrawBoxColor(playerid, TweetNotif[playerid][i], 1687547271);
	PlayerTextDrawUseBox(playerid, TweetNotif[playerid][i], 1);
	PlayerTextDrawSetProportional(playerid, TweetNotif[playerid][i], 1);
	PlayerTextDrawSetSelectable(playerid, TweetNotif[playerid][i], 0);
	PlayerTextDrawShow(playerid, TweetNotif[playerid][i]);

	TweetNotif[playerid][++i] = CreatePlayerTextDraw(playerid, 8.000000, 287.000000 - x, "ld_beat:chit");
	PlayerTextDrawFont(playerid, TweetNotif[playerid][i], 4);
	PlayerTextDrawLetterSize(playerid, TweetNotif[playerid][i], 0.600000, 2.000000);
	PlayerTextDrawTextSize(playerid, TweetNotif[playerid][i], 26.000000, 26.000000);
	PlayerTextDrawSetOutline(playerid, TweetNotif[playerid][i], 1);
	PlayerTextDrawSetShadow(playerid, TweetNotif[playerid][i], 0);
	PlayerTextDrawAlignment(playerid, TweetNotif[playerid][i], 2);
	PlayerTextDrawColor(playerid, TweetNotif[playerid][i], 1687547391);
	PlayerTextDrawBackgroundColor(playerid, TweetNotif[playerid][i], 255);
	PlayerTextDrawBoxColor(playerid, TweetNotif[playerid][i], 50);
	PlayerTextDrawUseBox(playerid, TweetNotif[playerid][i], 1);
	PlayerTextDrawSetProportional(playerid, TweetNotif[playerid][i], 1);
	PlayerTextDrawSetSelectable(playerid, TweetNotif[playerid][i], 0);
	PlayerTextDrawShow(playerid, TweetNotif[playerid][i]);

	TweetNotif[playerid][++i] = CreatePlayerTextDraw(playerid, 17.000000, 296.000000 - x, "ld_bum:blkdot");
	PlayerTextDrawFont(playerid, TweetNotif[playerid][i], 4);
	PlayerTextDrawLetterSize(playerid, TweetNotif[playerid][i], 0.600000, 2.000000);
	PlayerTextDrawTextSize(playerid, TweetNotif[playerid][i], 9.500000, 7.500000);
	PlayerTextDrawSetOutline(playerid, TweetNotif[playerid][i], 1);
	PlayerTextDrawSetShadow(playerid, TweetNotif[playerid][i], 0);
	PlayerTextDrawAlignment(playerid, TweetNotif[playerid][i], 2);
	PlayerTextDrawColor(playerid, TweetNotif[playerid][i], -1);
	PlayerTextDrawBackgroundColor(playerid, TweetNotif[playerid][i], 255);
	PlayerTextDrawBoxColor(playerid, TweetNotif[playerid][i], 50);
	PlayerTextDrawUseBox(playerid, TweetNotif[playerid][i], 1);
	PlayerTextDrawSetProportional(playerid, TweetNotif[playerid][i], 1);
	PlayerTextDrawSetSelectable(playerid, TweetNotif[playerid][i], 0);
	PlayerTextDrawShow(playerid, TweetNotif[playerid][i]);

	TweetNotif[playerid][++i] = CreatePlayerTextDraw(playerid, 13.000000, 294.000000 - x, "ld_beat:chit");
	PlayerTextDrawFont(playerid, TweetNotif[playerid][i], 4);
	PlayerTextDrawLetterSize(playerid, TweetNotif[playerid][i], 0.600000, 2.000000);
	PlayerTextDrawTextSize(playerid, TweetNotif[playerid][i], 6.500000, 6.500000);
	PlayerTextDrawSetOutline(playerid, TweetNotif[playerid][i], 1);
	PlayerTextDrawSetShadow(playerid, TweetNotif[playerid][i], 0);
	PlayerTextDrawAlignment(playerid, TweetNotif[playerid][i], 2);
	PlayerTextDrawColor(playerid, TweetNotif[playerid][i], -1);
	PlayerTextDrawBackgroundColor(playerid, TweetNotif[playerid][i], 255);
	PlayerTextDrawBoxColor(playerid, TweetNotif[playerid][i], 50);
	PlayerTextDrawUseBox(playerid, TweetNotif[playerid][i], 1);
	PlayerTextDrawSetProportional(playerid, TweetNotif[playerid][i], 1);
	PlayerTextDrawSetSelectable(playerid, TweetNotif[playerid][i], 0);
	PlayerTextDrawShow(playerid, TweetNotif[playerid][i]);

	TweetNotif[playerid][++i] = CreatePlayerTextDraw(playerid, 22.000000, 294.000000 - x, "...");
	PlayerTextDrawFont(playerid, TweetNotif[playerid][i], 1);
	PlayerTextDrawLetterSize(playerid, TweetNotif[playerid][i], 0.179167, 0.950000);
	PlayerTextDrawTextSize(playerid, TweetNotif[playerid][i], 150.500000, 17.000000);
	PlayerTextDrawSetOutline(playerid, TweetNotif[playerid][i], 0);
	PlayerTextDrawSetShadow(playerid, TweetNotif[playerid][i], 0);
	PlayerTextDrawAlignment(playerid, TweetNotif[playerid][i], 2);
	PlayerTextDrawColor(playerid, TweetNotif[playerid][i], 255);
	PlayerTextDrawBackgroundColor(playerid, TweetNotif[playerid][i], -1);
	PlayerTextDrawBoxColor(playerid, TweetNotif[playerid][i], 50);
	PlayerTextDrawUseBox(playerid, TweetNotif[playerid][i], 0);
	PlayerTextDrawSetProportional(playerid, TweetNotif[playerid][i], 1);
	PlayerTextDrawSetSelectable(playerid, TweetNotif[playerid][i], 0);
	PlayerTextDrawShow(playerid, TweetNotif[playerid][i]);

	TweetNotif[playerid][++i] = CreatePlayerTextDraw(playerid, 33.000000, 283.000000 - x, notifyInfo[playerid][index][ntTitle]);
	PlayerTextDrawFont(playerid, TweetNotif[playerid][i], 1);
	PlayerTextDrawLetterSize(playerid, TweetNotif[playerid][i], 0.145833, 1.250000);
	PlayerTextDrawTextSize(playerid, TweetNotif[playerid][i], 150.500000, 17.000000);
	PlayerTextDrawSetOutline(playerid, TweetNotif[playerid][i], 0);
	PlayerTextDrawSetShadow(playerid, TweetNotif[playerid][i], 0);
	PlayerTextDrawAlignment(playerid, TweetNotif[playerid][i], 1);
	PlayerTextDrawColor(playerid, TweetNotif[playerid][i], -1);
	PlayerTextDrawBackgroundColor(playerid, TweetNotif[playerid][i], -1);
	PlayerTextDrawBoxColor(playerid, TweetNotif[playerid][i], 50);
	PlayerTextDrawUseBox(playerid, TweetNotif[playerid][i], 0);
	PlayerTextDrawSetProportional(playerid, TweetNotif[playerid][i], 1);
	PlayerTextDrawSetSelectable(playerid, TweetNotif[playerid][i], 0);
	PlayerTextDrawShow(playerid, TweetNotif[playerid][i]);

	TweetNotif[playerid][++i] = CreatePlayerTextDraw(playerid, 33.000000, 294.000000 - x, notifyInfo[playerid][index][ntMessage]);
	PlayerTextDrawFont(playerid, TweetNotif[playerid][i], 1);
	PlayerTextDrawLetterSize(playerid, TweetNotif[playerid][i], 0.137500, 0.800000);
	PlayerTextDrawTextSize(playerid, TweetNotif[playerid][i], 150.500000, 17.000000);
	PlayerTextDrawSetOutline(playerid, TweetNotif[playerid][i], 0);
	PlayerTextDrawSetShadow(playerid, TweetNotif[playerid][i], 0);
	PlayerTextDrawAlignment(playerid, TweetNotif[playerid][i], 1);
	PlayerTextDrawColor(playerid, TweetNotif[playerid][i], -1094795521);
	PlayerTextDrawBackgroundColor(playerid, TweetNotif[playerid][i], -1);
	PlayerTextDrawBoxColor(playerid, TweetNotif[playerid][i], 50);
	PlayerTextDrawUseBox(playerid, TweetNotif[playerid][i], 0);
	PlayerTextDrawSetProportional(playerid, TweetNotif[playerid][i], 1);
	PlayerTextDrawSetSelectable(playerid, TweetNotif[playerid][i], 0);
	PlayerTextDrawShow(playerid, TweetNotif[playerid][i]);
	return true;
}

public OnPlayerConnect(playerid)
{
	
	notifyIndex[playerid] = 0;

	#if defined NT_OnPlayerConnect
		return NT_OnPlayerConnect(playerid);
	#else
		return 1;
	#endif
}
#if defined _ALS_OnPlayerConnect
	#undef OnPlayerConnect
#else
	#define _ALS_OnPlayerConnect
#endif

#define OnPlayerConnect NT_OnPlayerConnect
#if defined NT_OnPlayerConnect
	forward NT_OnPlayerConnect(playerid);
#endif