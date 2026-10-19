#define MAX_WORKSHOP 100
#define MAX_WORKSHOP_EMPLOYEE 9
#define MAX_WORKSHOP_INT 100000

enum E_WORKSHOP
{
    wID,
    wName[24],
    wOwner[MAX_PLAYER_NAME + 1],
    wOwnerID,
    wComp,
    wMat,
    wMoney,
    Text3D:wText,
    wPickup,
    Float:wX,
    Float:wY,
    Float:wZ,
    wStatus,
    wPrice
};
new wsData[MAX_WORKSHOP][E_WORKSHOP],
    wsEmploy[MAX_WORKSHOP][9][MAX_PLAYER_NAME + 1],
    Iterator:Workshop<MAX_WORKSHOP>;

new CarInfo[MAX_PLAYERS][cInfo];

GetTuningName(componentid)
{
	new Name[20];
	switch(componentid)
	{
		case 1000..1003,1014..1016,1023,1049,1050,1058,1060,1138,1139,1146,1147,1158,1162..1164: Name = "cSpoiler";
		case 1004,1005,1011,1012: Name = "cHood";
		case 1007,1017,1026,1027,1030,1031,1036,1039..1042,1047,1048,1051,1052,1056,1057,1062,1063,1069,1072,1090,1093..1095,1099,1101,1102,1106..1108,1119..1122,1124,1133,1134,1137: Name = "cSideskirt";
		case 1008..1010: Name = "cNitro";
		case 1013,1024: Name = "cLamps";
		case 1018..1022,1028,1029,1034,1037,1043..1046,1059,1064.1066,1089,1092,1105,1113,1114,1126,1127,1129,1132,1135,1136: Name = "cExhaust";
		case 1025,1073..1085,1096..1098: Name = "cWheels";
		case 1006,1032,1033,1035,1038,1053..1055,1061,1067,1068,1088,1091,1103,1128,1130,1131: Name = "cRoof";
		case 1086: Name = "cStereo";
		case 1087: Name = "cHydraqlic";
		case 1117,1152,1153,1155,1157,1160,1165,1166,1169..1174,1176,1179,1181,1182,1185,1188..1191: Name = "cBumper1";
		case 1140,1141,1148..1151,1154,1156,1159,1161,1167,1168,1175,1177,1178,1180,1183,1184,1186,1187,1192,1193: Name = "cBumper2";
	}
	return Name;
}

Workshop_Refresh(id)
{
    if(id > -1)
    {
        if(IsValidDynamic3DTextLabel(wsData[id][wText]))
            DestroyDynamic3DTextLabel(wsData[id][wText]), wsData[id][wText] = Text3D: INVALID_3DTEXT_ID;

        if(IsValidDynamicPickup(wsData[id][wPickup]))
            DestroyDynamicPickup(wsData[id][wPickup]), wsData[id][wPickup] = -1;

        new str[316], stats[64];
        if(wsData[id][wStatus] == 1)
        {
            stats = "{7fff00}OPEN{ffffff}";
        }
        else
        {
            stats = "{ff0000}CLOSED{ffffff}";
        }

        format(str, sizeof str,"[Workshop ID:%d]\n{ffffff}Workshop Price: {7fff00}%s{ffffff}\n{ffffff}Type '/buy' to buy this Workshop", id, FormatMoney(wsData[id][wPrice]));

        if(strcmp(wsData[id][wOwner], "-", true))
            format(str, sizeof str,"[Workshop ID:%d]\n{ffffff}Workshop Name: {15D4ED}%s\n{ffffff}Workshop Owner: {FFFF00}%s\n{ffffff}STATUS: %s\n {ff0000}/wsmenu{ffffff} menu workshop\n{ff0000}/lockworkshop{ffffff} membuka/mentutup WorkShop", id, wsData[id][wName], wsData[id][wOwner], stats);

        wsData[id][wText] = CreateDynamic3DTextLabel(str, -1, wsData[id][wX], wsData[id][wY], wsData[id][wZ]+0.5, 8.0, INVALID_PLAYER_ID, INVALID_VEHICLE_ID, 1, -1, -1, -1, 8.0);
        wsData[id][wPickup] = CreateDynamicPickup(1239, 23, wsData[id][wX], wsData[id][wY], wsData[id][wZ]+0.2, -1);
    }
}

Workshop_Save(id)
{
    new query[2248];
    format(query, sizeof query,"UPDATE workshop SET owner='%s', name='%s', component=%d, material=%d, money=%d, posx='%f', posy='%f', posz='%f', status=%d, price=%d",
        wsData[id][wOwner],
        wsData[id][wName],
        wsData[id][wComp],
        wsData[id][wMat],
        wsData[id][wMoney],
        wsData[id][wX],
        wsData[id][wY],
        wsData[id][wZ],
        wsData[id][wStatus],
        wsData[id][wPrice]);
    for(new z = 0; z < MAX_WORKSHOP_EMPLOYEE; z++)
    {
        format(query, sizeof query,"%s, employe%d='%s'", query, z, wsEmploy[id][z]);
    }
    format(query, sizeof query,"%s WHERE id = %d", query, id);
    return mysql_tquery(mMysql, query);
}

Workshop_Reset(id)
{
    format(wsData[id][wOwner], MAX_PLAYER_NAME, "-");
    format(wsEmploy[id][0], MAX_PLAYER_NAME, "-");
    format(wsEmploy[id][1], MAX_PLAYER_NAME, "-");
    format(wsEmploy[id][2], MAX_PLAYER_NAME, "-");
    wsData[id][wComp] = 0;
    wsData[id][wMat] = 0;
    wsData[id][wMoney] = 0;
    for(new z = 0; z < MAX_WORKSHOP_EMPLOYEE; z++)
    {
        format(wsEmploy[id][z], MAX_PLAYER_NAME, "-");
    }
    Workshop_Refresh(id);
}

epublic: LoadWorkshop()
{
    static wid;

	new rows = cache_num_rows(), owner[128], name[128];
 	if(rows)
  	{
		for(new i; i < rows; i++)
		{
			cache_get_value_name_int(i, "id", wid);
			cache_get_value_name(i, "owner", owner);
			format(wsData[wid][wOwner], 128, owner);
			cache_get_value_name(i, "name", name);
			format(wsData[wid][wName], 128, name);
			cache_get_value_name_int(i, "price", wsData[wid][wPrice]);
			cache_get_value_name_float(i, "posx", wsData[wid][wX]);
			cache_get_value_name_float(i, "posy", wsData[wid][wY]);
			cache_get_value_name_float(i, "posz", wsData[wid][wZ]);
			cache_get_value_name_int(i, "component", wsData[wid][wComp]);
			cache_get_value_name_int(i, "material", wsData[wid][wMat]);
            cache_get_value_name_int(i, "money", wsData[wid][wMoney]);
            for(new z = 0; z < MAX_WORKSHOP_EMPLOYEE; z++)
            {
                new str[64];
                format(str, sizeof str,"employe%d", z);
                cache_get_value_name(i, str, wsEmploy[wid][z]);
            }
			Workshop_Refresh(wid);
			Iter_Add(Workshop, wid);
		}
		printf("[WORKSHOP] Number of Loaded: %d.", rows);
	}
}

GetAnyWorkshop(playerid)
{
	new tmpcount;
	foreach(new id : Workshop)
	{
     	if (IsWorkshopOwner(playerid, id))
   		{
   		    tmpcount++;
		}
	}
	return tmpcount;
}

CMD:wsmenu(playerid, params[])
{
    foreach(new i : Workshop) if (IsPlayerInRangeOfPoint(playerid, 5, wsData[i][wX], wsData[i][wY], wsData[i][wZ]))
    {
        if(!IsWorkshopOwner(playerid, i) && !IsWorkshopEmploye(playerid, i)) return ShowError(playerid, "Kamu bukan pengurus Workshop ini.");
        ShowWorkshopMenu(playerid, i);
    }
	return 1;
}

ShowWorkshopMenu(playerid, id)
{
    PlayerInfo[playerid][pMenuType] = 0;
    PlayerInfo[playerid][pInWs] = id;

    new str[256], vstr[64];
    format(vstr, sizeof vstr,"Workshop (%s) Menu", wsData[id][wName]);
    format(str, sizeof str,"Set Workshop Name\nEmploye Menu\nComponent\t(%d/%d)\nMaterial\t(%d/%d)\nMoney\t({7fff00}%s{ffffff})",
        wsData[id][wComp],
        MAX_WORKSHOP_INT,
        wsData[id][wMat],
        MAX_WORKSHOP_INT,
        FormatMoney(wsData[id][wMoney]));
    ShowPlayerDialog(playerid, WS_MENU, DIALOG_STYLE_LIST, vstr, str, "Select", "Cancel");
    return 1;
}

CMD:createworkshop(playerid, params[])
{
	new query[512];
	new wid = Iter_Free(Workshop);
	if(wid == -1) return ShowError(playerid, "You cant create more workshop!");
	new price;
	if(sscanf(params, "d", price)) return ShowInfo(playerid, "/createworkshop [price]");
	new totalcash[25];
	format(totalcash, sizeof totalcash,"%d00",price);
	price = strval(totalcash);
	format(wsData[wid][wOwner], MAX_PLAYER_NAME, "-");
    format(wsEmploy[wid][0], MAX_PLAYER_NAME, "-");
    format(wsEmploy[wid][1], MAX_PLAYER_NAME, "-");
    format(wsEmploy[wid][2], MAX_PLAYER_NAME, "-");
    format(wsData[wid][wName], 24, "-");
	GetPlayerPos(playerid, wsData[wid][wX], wsData[wid][wY], wsData[wid][wZ]);
	wsData[wid][wPrice] = price;
	wsData[wid][wStatus] = 0;

    Workshop_Refresh(wid);
	Iter_Add(Workshop, wid);

	mysql_format(mMysql, query, sizeof(query), "INSERT INTO workshop SET id=%d, owner='%s', price=%d, posx='%f', posy='%f', posz='%f', name='%s'", wid, wsData[wid][wOwner], wsData[wid][wPrice], wsData[wid][wX], wsData[wid][wY], wsData[wid][wZ], wsData[wid][wName]);
	mysql_tquery(mMysql, query, "OnWorkshopCreated", "i", wid);
    Info(playerid, "Created Workshop ID:%d", wid);
	return 1;
}
IsWorkshopEmploye(playerid, id)
{
    if(pData[playerid][pWSid] == id) return 1;
    return 0;
}

IsWorkshopOwner(playerid, id)
{
    if(!strcmp(wsData[id][wOwner], PlayerInfo[playerid][pName], true))
        return 1;

    return 0;
}

epublic: OnWorkshopCreated(wid)
{
	Workshop_Save(wid);
    Workshop_Refresh(wid);
	return 1;
}

CMD:gotoworkshop(playerid, params[])
{
	new wid;
	if(sscanf(params, "d", wid))
		return ShowInfo(playerid, "/gotoworkshop [id]");
	if(!Iter_Contains(Workshop, wid)) return ShowError(playerid, "The Workshop you specified ID of doesn't exist.");
	SetPlayerPos(playerid, wsData[wid][wX], wsData[wid][wY], wsData[wid][wZ]);
    SetPlayerInterior(playerid, 0);
    SetPlayerVirtualWorld(playerid, 0);
	Info(playerid, "You has teleport to Workshop id %d", wid);
	return 1;
}

CMD:editworkshop(playerid, params[])
{
    static
        wid,
        type[24],
        string[128];

    if(sscanf(params, "ds[24]S()[128]", wid, type, string))
    {
        ShowInfo(playerid, "/editworkshop [id] [name]");
        SendClientMessage(playerid, COLOR_YELLOW, "[NAMES]:{FFFFFF} location, status, owner, price, money, comp, mat");
        return 1;
    }
    if((wid < 0 || wid >= MAX_WORKSHOP))
        return ShowError(playerid, "You have specified an invalid ID.");
	if(!Iter_Contains(Workshop, wid)) return ShowError(playerid, "The Workshop you specified ID of doesn't exist.");

    if(!strcmp(type, "location", true))
    {
		GetPlayerPos(playerid, wsData[wid][wX], wsData[wid][wY], wsData[wid][wZ]);
        Workshop_Save(wid);
		Workshop_Refresh(wid);
    }
    else if(!strcmp(type, "status", true))
    {
        new locked;

        if(sscanf(string, "d", locked))
            return ShowInfo(playerid, "/editWorkshop [id] [locked] [0/1]");

        if(locked < 0 || locked > 1)
            return ShowError(playerid, "You must specify at least 0 or 1.");

        wsData[wid][wStatus] = locked;
        Workshop_Save(wid);
		Workshop_Refresh(wid);
    }
    else if(!strcmp(type, "price", true))
    {
        new price;

        if(sscanf(string, "d", price))
            return ShowInfo(playerid, "/editWorkshop [id] [Price] [Amount]");

        wsData[wid][wPrice] = price;

        Workshop_Save(wid);
		Workshop_Refresh(wid);
    }
	else if(!strcmp(type, "money", true))
    {
        new money;

        if(sscanf(string, "d", money))
            return ShowInfo(playerid, "/editWorkshop [id] [money] [Ammount]");

        wsData[wid][wMoney] = money;
        Workshop_Save(wid);
		Workshop_Refresh(wid);
    }
	else if(!strcmp(type, "comp", true))
    {
        new amount;

        if(sscanf(string, "d", amount))
            return ShowInfo(playerid, "/editWorkshop [id] [Comp] [Ammount]");

        wsData[wid][wComp] = amount;
        Workshop_Save(wid);
		Workshop_Refresh(wid);
    }
    else if(!strcmp(type, "mat", true))
    {
        new amount;

        if(sscanf(string, "d", amount))
            return ShowInfo(playerid, "/editWorkshop [id] [mat] [Ammount]");

        wsData[wid][wMat] = amount;
        Workshop_Save(wid);
		Workshop_Refresh(wid);
    }
    else if(!strcmp(type, "owner", true))
    {
        new owners[MAX_PLAYER_NAME];

        if(sscanf(string, "s[24]", owners))
            return ShowInfo(playerid, "/editWorkshop [id] [owner] [player name] (use '-' to no owner)");

        format(wsData[wid][wOwner], MAX_PLAYER_NAME, owners);

        Workshop_Save(wid);
		Workshop_Refresh(wid);
    }
    else if(!strcmp(type, "reset", true))
    {
        Workshop_Reset(wid);
		Workshop_Save(wid);
		Workshop_Refresh(wid);
    }
	else if(!strcmp(type, "delete", true))
    {
		Workshop_Reset(wid);

		DestroyDynamic3DTextLabel(wsData[wid][wText]);
        DestroyDynamicPickup(wsData[wid][wPickup]);

		wsData[wid][wX] = 0;
		wsData[wid][wY] = 0;
		wsData[wid][wZ] = 0;
		wsData[wid][wPrice] = 0;
		wsData[wid][wText] = Text3D: INVALID_3DTEXT_ID;
		wsData[wid][wPickup] = -1;

		Iter_Remove(Workshop, wid);
		new query[128];
		mysql_format(mMysql, query, sizeof(query), "DELETE FROM Workshop WHERE ID=%d", wid);
		mysql_tquery(mMysql, query);
	}
    return 1;
}

CMD:lockworkshop(playerid, params[])
{
	foreach(new wid : Workshop)
	{
		if(IsPlayerInRangeOfPoint(playerid, 3.5, wsData[wid][wX], wsData[wid][wY], wsData[wid][wZ]))
		{
			if(!IsWorkshopOwner(playerid, wid) && !IsWorkshopEmploye(playerid, wid)) return ShowError(playerid, "Kamu bukan pengurus Workshop ini.");
			if(!wsData[wid][wStatus])
			{
				wsData[wid][wStatus] = 1;

				ShowInfo(playerid, "Workshop anda berhasil ~g~Dibuka!");
				PlayerPlaySound(playerid, 1145, 0.0, 0.0, 0.0);
			}
			else
			{
				wsData[wid][wStatus] = 0;

				ShowInfo(playerid,"Workshop anda berhasil ~r~Ditutup");
				PlayerPlaySound(playerid, 1145, 0.0, 0.0, 0.0);
			}
            Workshop_Refresh(wid);
            Workshop_Save(wid);
		}
	}
	return 1;
}

CMD:buyworkshop(playerid, params[])
{
	foreach(new id : Workshop) if (IsPlayerInRangeOfPoint(playerid, 5, wsData[id][wX], wsData[id][wY], wsData[id][wZ]))
    {
        if (GetAnyWorkshop(playerid) >= 1)
            return ShowError(playerid, "You can only own 1 workshop!");

        if (wsData[id][wOwner] != 0)
            return ShowError(playerid, "This workshop is already owned.");

        if (wsData[id][wPrice] > GetPlayerMoney(playerid))
            return ShowError(playerid, "You don't have enough money!");

        GivePlayerMoneyEx(playerid, -wsData[id][wPrice], "Beli WorkShop");
        GetPlayerName(playerid, wsData[id][wOwner], MAX_PLAYER_NAME);
        wsData[id][wOwnerID] = PlayerInfo[playerid][pID];

        Workshop_Refresh(id);
        Workshop_Save(id);
        new cgs[128];
        format(cgs, sizeof(cgs), "PROPERTY: {FFFFFF}You've successfully buy a workshop for {00FF00}%s", FormatMoney(wsData[id][wPrice]));
        SendClientMessageEx(playerid, -1, cgs);
    }
	return 1;
}
alias:addcomponent("addtuning");
CMD:addcomponent(playerid, params[])
{
	foreach(new wid : Workshop)
	{
		if (IsPlayerInRangeOfPoint(playerid, 5, wsData[wid][wX], wsData[wid][wY], wsData[wid][wZ]))
			return Error(playerid, "Anda tidak di area workshop");

	    if(!IsWorkshopOwner(playerid, wid) && !IsWorkshopEmploye(playerid, wid)) return 1;

	    extract params -> new pembeli, compo; else return SendClientMessage(playerid, -1, "/addcompo [ID Pemain] [ID Component]");

	    if(PlayerInfo[playerid][pComponent] < 35) return SCM(playerid, COLOR_RED, "ERROR: {FFFFFF}Component Anda Tidak Cukup, Butuh 35");
	    if(!(1000 <= compo <= 1193)) return SendClientMessage(playerid, COLOR_RED, "ERROR: {FFFFFFID Component Salah {FFFF00}(1000 - 1193)");

	    if(!ProxDetectorS(5.0, playerid, pembeli))
	        return SendClientMessage(playerid, COLOR_RED, "ERROR: {FFFFFF}Pemainnya Terlalu Jauh");
	    if(!IsPlayerDriver(pembeli))
	        return SendClientMessage(playerid, COLOR_RED, "ERROR: {FFFFFF}Pelanggan Harus Mengemudi");

	    new u_car = GetPlayerUseHomeCar(pembeli);

	    if(u_car == -1)
	        return SCM(playerid, COLOR_RED, "ERROR: {FFFFFF}Pelanggan Harus Menggunakan Mobil Pribadi");

	    SetPlayerUseListitem(pembeli, u_car);

	    new u_car_0 = GetPlayerUseListitem(pembeli);
	    new vehicleid = GetPlayerVehicleID(pembeli);

	    switch(compo)
	    {
	        case 1000..1003,1014..1016,1023,1049,1050,1058,1060,1138,1139,1146,1147,1158,1162..1164: CarInfo[pembeli][cSpoiler][u_car_0] = compo;
	        case 1004,1005,1011,1012: CarInfo[pembeli][cHood][u_car_0] = compo;
	        case 1007,1017,1026,1027,1030,1031,1036,1039..1042,1047,1048,1051,1052,1056,1057,1062,1063,1069,1072,1090,1093..1095,1099,1101,1102,1106..1108,1119..1122,1124,1133,1134,1137: CarInfo[pembeli][cSideskirt][u_car_0] = compo;
	        case 1008..1010: CarInfo[pembeli][cNitro][u_car_0] = compo;
	        case 1013,1024: CarInfo[pembeli][cLamps][u_car_0] = compo;
	        case 1018..1022,1028,1029,1034,1037,1043..1046,1059,1064.1066,1089,1092,1105,1113,1114,1126,1127,1129,1132,1135,1136:CarInfo[pembeli][cExhaust][u_car_0] = compo;
	        case 1025,1073..1085,1096..1098:CarInfo[pembeli][cWheels][u_car_0] = compo;
	        case 1006,1032,1033,1035,1038,1053..1055,1061,1067,1068,1088,1091,1103,1128,1130,1131: CarInfo[pembeli][cRoof][u_car_0] = compo;
	        case 1086: CarInfo[pembeli][cStereo][u_car_0] = compo;
	        case 1087: CarInfo[pembeli][cHydrawlic][u_car_0] = compo;
	        //ashiap
	        case 1117,1152,1153,1155,1157,1160,1165,1166,1169..1174,1176,1179,1181,1182,1185,1188..1191: CarInfo[pembeli][cBumper1][u_car_0] = compo;
	        case 1140,1141,1148..1151,1154,1156,1159,1161,1167,1168,1175,1177,1178,1180,1183,1184,1186,1187,1192,1193: CarInfo[pembeli][cBumper2][u_car_0] = compo;
	        case 1142..1145: CarInfo[pembeli][cVentRight][u_car_0] = compo;

	    }
	    AddVehicleComponent(vehicleid, compo);

	    SaveCars(pembeli);

	    PlayerInfo[playerid][pComponent] -= 35;
	    OnPlayerUpdateAccountsPer(playerid, "pComponent", PlayerInfo[playerid][pComponent]);
	}
    return 1;
}
CMD:paintws(playerid, params[])
{
	foreach(new wid : Workshop)
	{
		if (IsPlayerInRangeOfPoint(playerid, 5, wsData[wid][wX], wsData[wid][wY], wsData[wid][wZ]))
			return Error(playerid, "Anda tidak di area workshop");

		if(!IsWorkshopOwner(playerid, wid) && !IsWorkshopEmploye(playerid, wid)) return 1;

		extract params -> new pembeli, color1, color2; else return SendClientMessage(playerid, -1, "/repaint [id pemain] [color 1] [color 2]");

		if(PlayerInfo[playerid][pComponent] < 28) return SCM(playerid, COLOR_RED, "ERROR: {FFFFFF}Component Anda Tidak Cukup, Butuh 28");
		if(!(0 <= color1 <= 255)) return SendClientMessage(playerid, COLOR_RED, "ERROR: {FFFFFF}ID Warna 0 - 255");
		if(!(0 <= color2 <= 255)) return SendClientMessage(playerid, COLOR_RED, "ERROR: {FFFFFF}ID Warna 0 - 255");

		if(!ProxDetectorS(5.0, playerid, pembeli))
		    return SendClientMessage(playerid, COLOR_RED, "ERROR: {FFFFFF}Pemainnya Terlalu Jauh");
		if(!IsPlayerDriver(pembeli))
		    return SendClientMessage(playerid, COLOR_RED, "ERROR: {FFFFFF}Pelanggan Harus Mengemudi");

		new u_car = GetPlayerUseHomeCar(pembeli);

		if(u_car == -1)
		    return SCM(playerid, COLOR_RED, "ERROR: {FFFFFF}Pelanggan Harus Menggunakan Mobil Pribadi");

		SetPlayerUseListitem(pembeli, u_car);

		new u_car_0 = GetPlayerUseListitem(pembeli);
		new vehicleid = GetPlayerVehicleID(pembeli);

		ChangeVehicleColor(vehicleid, color1, color2);

		CarInfo[pembeli][cColor1][u_car_0] = color1;
		CarInfo[pembeli][cColor2][u_car_0] = color2;

		SaveCars(pembeli);

		PlayerInfo[playerid][pComponent] -= 28;
		OnPlayerUpdateAccountsPer(playerid, "pComponent", PlayerInfo[playerid][pComponent]);
	}
    return 1;
}
CMD:paintjobws(playerid, params[])
{
	foreach(new wid : Workshop)
	{
		if (IsPlayerInRangeOfPoint(playerid, 5, wsData[wid][wX], wsData[wid][wY], wsData[wid][wZ]))
			return Error(playerid, "Anda tidak di area workshop");

		if(!IsWorkshopOwner(playerid, wid) && !IsWorkshopEmploye(playerid, wid)) return 1;
		
	    extract params -> new pembeli, pid; else return SendClientMessage(playerid, -1, "/paintjobws [ID Pemain] [ID Paintjob]");

	    if(PlayerInfo[playerid][pComponent] < 23) return SCM(playerid, COLOR_RED, "ERROR: {FFFFFF}Component Anda Tidak Cukup, Butuh 23");
	    //if(sscanf(params, "ddd",params[0],params[1],params[2],params[3])) return SCM(playerid,COLOR_WHITE,"Masukkan: /changecolor [id pemain][id warna 1][id warna 2][slot]");

	    if(!ProxDetectorS(5.0, playerid, pembeli))
	        return SendClientMessage(playerid, COLOR_RED, "ERROR: {FFFFFF}Pemainnya Terlalu Jauh");
	    
	    if(!IsPlayerDriver(pembeli))
	        return SendClientMessage(playerid, COLOR_RED, "ERROR: {FFFFFF}Pelanggan Harus Mengemudi");

	    new u_car = GetPlayerUseHomeCar(pembeli);

	    if(u_car == -1)
	        return SCM(playerid, COLOR_RED, "ERROR: {FFFFFF}Pelanggan Harus Menggunakan Mobil Pribadi");

	    SetPlayerUseListitem(pembeli, u_car);

	    new u_car_0 = GetPlayerUseListitem(pembeli);
	    new vehicleid = GetPlayerVehicleID(pembeli);

	    CarInfo[pembeli][cPaintJobs][u_car_0] = pid;

	    ChangeVehiclePaintjob(vehicleid, pid);
	    SaveCars(pembeli);

	    PlayerInfo[playerid][pComponent] -= 23;
	    OnPlayerUpdateAccountsPer(playerid, "pComponent", PlayerInfo[playerid][pComponent]);
	}
    return 1;
}

CMD:copotws(playerid, params[])
{
	foreach(new wid : Workshop)
	{
		if (IsPlayerInRangeOfPoint(playerid, 5, wsData[wid][wX], wsData[wid][wY], wsData[wid][wZ]))
			return Error(playerid, "Anda tidak di area workshop");

		if(!IsWorkshopOwner(playerid, wid) && !IsWorkshopEmploye(playerid, wid)) return 1;
		
		if(!strlen(params))
			return Usage(playerid, "/copot [id pemain] [nomor component] [biaya]");

		extract params -> new to_player ,componentnumber , nitro_price;
		if(!(1000 <= componentnumber <= 1193)) return Error(playerid, "ID Component 1000 - 1193");

	    if(!IsPlayerConnected(to_player) || !IsPlayerLogged(to_player) || to_player == playerid) return Error(playerid, "Pelanggan Harus didalam mobil");

		if(!IsPlayerDriver(to_player))
			return SendClientMessage(playerid, 0xCECECEFF, "Pelanggan harus mengemudi");

		if(!(1 <= nitro_price <= 100000))
			return Info(playerid, "Biaya mencapai Rp100000");

		if(GetPlayerMoneyEx(to_player) < nitro_price)
			return SendClientMessage(playerid, 0xCECECEFF, "Pelanggan tidak punya banyak uang");

		SendPlayerOffer(playerid, to_player, OFFER_TYPE_COMPONENT, componentnumber, nitro_price);
		SCM(to_player, -1, "Untuk menyetujui, klik Y(/accept) atau untuk penolakan N(/cancel)");
	}
	return 1;
}
CMD:repairws(playerid, params[])
{
	foreach(new wid : Workshop)
	{
		if(!IsWorkshopOwner(playerid, wid) && !IsWorkshopEmploye(playerid, wid)) return 1;

		if(PlayerInfo[playerid][pComponent] < 15) return Error(playerid, "Component Anda Tidak Cukup");
	    if(GetPlayerState(playerid) != 2) return Error(playerid, "Kamu tidak berada di kursi pengemudi!");
	    RepairVehicle(GetPlayerVehicleID(playerid));
	    PlayerInfo[playerid][pComponent] -= 15;
		OnPlayerUpdateAccountsPer(playerid, "pComponent", PlayerInfo[playerid][pComponent]);
	    Info(playerid, "Kendaraan berhasil di perbaiki.");
	    PlayerPlaySound(playerid, 1133, 0.0, 0.0, 0.0);
	}
    return 1;
}