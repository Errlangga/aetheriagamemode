#include <a_samp>

new Text:TextTime, Text:TextDate;
forward settime(playerid);

public OnGameModeInit()
{
		SetTimer("settime",1000,true);

 		TextDate = TextDrawCreate(551.5, 12.33,"--");
		TextDrawBackgroundColor(TextDate, 255);
		TextDrawFont(TextDate, 3);
		TextDrawLetterSize(TextDate, 0.270833, 1.150000);
		TextDrawSetOutline(TextDate, 1);
		TextDrawAlignment(TextDate, 3);
		TextDrawSetProportional(TextDate, 1);
		TextDrawSetShadow(TextDate, 1);
		TextDrawColor(TextDate,0xFFFFFFFF);

		TextTime = TextDrawCreate(547.5, 23.33,"--");
		TextDrawLetterSize(TextTime,0.270833, 1.150000);
		TextDrawFont(TextTime , 3);
		TextDrawTextSize(TextTime, 796.500000, 847.000000);
		TextDrawSetOutline(TextTime , 1);
		TextDrawAlignment(TextTime, 3);
	    TextDrawSetProportional(TextTime , 1);
		TextDrawBackgroundColor(TextTime, 255);
		TextDrawSetShadow(TextTime, 1);
		TextDrawColor(TextTime,0xFFFFFFFF);
        return 1;
}
public OnPlayerConnect(playerid)
{
	for( new w = 0; w < 1; w++)
	{
		TextDrawShowForPlayer(playerid,TextDate);
		TextDrawShowForPlayer(playerid,TextTime);
	}
	return 1;
}
/*==============COMMAND==================*/
/*CMD:hidetd(playerid, params[]) // hide textdraw
{
        TextDrawHideForPlayer(playerid, TextTime), TextDrawHideForPlayer(playerid, TextDate);
        return 1;
}
CMD:showtd(playerid, params[]) // show textdraw
{
        TextDrawShowForPlayer(playerid, TextTime), TextDrawShowForPlayer(playerid, TextDate);
        return 1;
}*/
/*================END OF COMMAND==============*/
public settime(playerid)
{
        new string[256],year,month,day,hours,minutes,seconds;
        getdate(year, month, day), gettime(hours, minutes, seconds);
        format(string, sizeof string, "%d/%s%d/%s%d", day, ((month < 10) ? ("0") : ("")), month, (year < 10) ? ("0") : (""), year);
        TextDrawSetString(TextDate, string);
        format(string, sizeof string, "%s%d:%s%d:%s%d", (hours < 10) ? ("0") : (""), hours, (minutes < 10) ? ("0") : (""), minutes, (seconds < 10) ? ("0") : (""), seconds);
        TextDrawSetString(TextTime, string);
}
