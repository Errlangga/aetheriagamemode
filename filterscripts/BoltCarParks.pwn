
#define FILTERSCRIPT

#include <a_samp>
#include <zcmd> 
#include <sscanf2>
#include <YSI\y_ini>

#define MAX_CAR_PARKS	30
#define GREEN 			0x21DD00FF

enum data 
{
	Float:exteriorPos[3], 
	exteriorVW, 
	Price, 
	cpIconID, 
	cpIcon, 
	Text3D:cpLabel, 
};
new carPark[MAX_CAR_PARKS][data]; 

#if defined FILTERSCRIPT

public OnFilterScriptInit()
{
	print("\n--------------------------------------");
	print(" Bolt Dynamic Car Parks by Chenko");
	print("--------------------------------------\n");
	
	for(new i = 1; i < MAX_CAR_PARKS; i++)
	{
		LoadCarPark(i); 
	}
	
	//Car park mapping
	CreateObject(10010, 57.44, 1812.52, -93.95,   0.00, 0.00, 0.00);
	CreateObject(3037, 23.84, 1845.51, -91.42,   0.00, 0.00, 90.00);
	CreateObject(3037, 34.06, 1845.53, -91.42,   0.00, 0.00, 90.00);
	CreateObject(3037, 23.84, 1845.53, -87.03,   0.00, 0.00, 90.00);
	CreateObject(3037, 34.06, 1845.54, -87.03,   0.00, 0.00, 90.00);
	
	return 1;
}

public OnPlayerConnect(playerid)
{
	SetPVarInt(playerid, "inCP", -1);
	return 1;
}

public OnFilterScriptExit()
{
	return 1;
}

#endif 

stock saveCarPark(idx)
{
	new fileName[64];
	format(fileName, sizeof(fileName), "CarParks/CarPark_%d.ini", idx);
	
	new INI:file = INI_Open(fileName); 
	INI_WriteFloat(file, "ExteriorX", carPark[idx][exteriorPos][0]);
	INI_WriteFloat(file, "ExteriorY", carPark[idx][exteriorPos][1]);
	INI_WriteFloat(file, "ExteriorZ", carPark[idx][exteriorPos][2]);
	INI_WriteInt(file, "ExteriorVW", carPark[idx][exteriorVW]); 
	INI_WriteInt(file, "IconType", carPark[idx][cpIcon]); 
	INI_WriteInt(file, "Price", carPark[idx][Price]);
	INI_Close(file);
	printf("[BoltCarParks] Car park %d saved", idx); 
	return 1;
}

stock LoadCarPark(id)
{
	new file[64]; 
	format(file, sizeof(file), "CarParks/CarPark_%d.ini", id); 
	if(fexist(file))
		INI_ParseFile(file, "loadCarPark", .bExtra = true, .extra = id, .bPassTag = true);
	
	new string[128]; 
	if(carPark[id][Price] > 0)
		format(string, sizeof(string), "Car Park %d\nPrice per hour: %d\nType /entercarpark to enter.", id, carPark[id][Price]); 
	else format(string, sizeof(string), "Car Park %d\nFree Parking\nType /entercarpark to enter.", id, carPark[id][Price]); 
	
	carPark[id][cpLabel] = Create3DTextLabel(string, GREEN, carPark[id][exteriorPos][0], carPark[id][exteriorPos][1], carPark[id][exteriorPos][2], 50, carPark[id][exteriorVW]); 
	carPark[id][cpIconID] = CreatePickup(carPark[id][cpIcon], 1, carPark[id][exteriorPos][0], carPark[id][exteriorPos][1], carPark[id][exteriorPos][2], -1); 
	Create3DTextLabel("Type /exitcarpark to exit", GREEN, 24.0939, 1842.4647, -92.8708, 50, 1500 + id); 
	printf("[BoltCarParks] Car park %d loaded.", id);
	return 1;
}

forward loadCarPark(idx, tag[], name[], value[]);
public loadCarPark(idx, tag[], name[], value[])
{
	INI_Float("ExteriorX", carPark[idx][exteriorPos][0]);
	INI_Float("ExteriorY", carPark[idx][exteriorPos][1]);
	INI_Float("ExteriorZ", carPark[idx][exteriorPos][2]); 
	INI_Int("ExteriorVW", carPark[idx][exteriorVW]); 
	INI_Int("IconType", carPark[idx][cpIcon]);
	INI_Int("Price", carPark[idx][Price]);
	return 1;
}

CMD:createcarpark(playerid, params[])
{
	if(!IsPlayerAdmin(playerid))
		return 1;
		
	new newID = -1, file[64]; 
	
	for(new i = 1; i < MAX_CAR_PARKS; i++) 
	{
		format(file, sizeof(file), "CarParks/CarPark_%d.ini", i);
		if(!fexist(file)) {
			newID = i;
			break;
		}
	}
	
	if(newID == -1)
		return SendClientMessage(playerid, -1, "No more car parks can be created."); 
	
	new iconID, price;
	if(sscanf(params, "dd", iconID, price))
		return SendClientMessage(playerid, -1, "SYNTAX: /createcarpark [icon id] [price per hour]"); 
	
	new Float:pPos[3];
	GetPlayerPos(playerid, pPos[0], pPos[1], pPos[2]);
	
	carPark[newID][exteriorPos][0] = pPos[0];
	carPark[newID][exteriorPos][1] = pPos[1]; 
	carPark[newID][exteriorPos][2] = pPos[2]; 
	carPark[newID][exteriorVW] = GetPlayerVirtualWorld(playerid);
	carPark[newID][cpIcon] = iconID;
	carPark[newID][Price] = price; 
	
	new string[128]; 
	if(carPark[newID][Price] > 0)
		format(string, sizeof(string), "Car Park %d\nCost: %d\nType /entercarpark to enter.", newID, carPark[newID][Price]); 
	else format(string, sizeof(string), "Car Park %d\nFree Parking\nType /entercarpark to enter.", newID, carPark[newID][Price]); 
	
	carPark[newID][cpLabel] = Create3DTextLabel(string, GREEN, carPark[newID][exteriorPos][0], carPark[newID][exteriorPos][1], carPark[newID][exteriorPos][2], 50, carPark[newID][exteriorVW]); 
	carPark[newID][cpIconID] = CreatePickup(carPark[newID][cpIcon], 1, carPark[newID][exteriorPos][0], carPark[newID][exteriorPos][1], carPark[newID][exteriorPos][2], -1); 
	Create3DTextLabel("Type /exitcarpark to exit", GREEN, 24.0939, 1842.4647, -92.8708, 50, 1500 + newID); 
	
	saveCarPark(newID);
	format(string, sizeof(string), "You have created car park ID %d", newID);
	SendClientMessage(playerid, -1, string); 
	printf("[BoltCarParks] Car park %d created.", newID); 
	return 1;
}

CMD:deletecarpark(playerid, params[])
{
	if(!IsPlayerAdmin(playerid))
		return 1;
		
	new cpID; 
	if(sscanf(params, "d", cpID))
		return SendClientMessage(playerid, -1, "SYNTAX: /deletecarpark [car park id]");
		
	new file[64]; 
	format(file, sizeof(file), "CarParks/CarPark_%d.ini", cpID);
	if(!fexist(file))
		return SendClientMessage(playerid, -1, "That car park ID does not exist.");
		
	carPark[cpID][exteriorPos][0] = -1;
	carPark[cpID][exteriorPos][1] = -1;
	carPark[cpID][exteriorPos][2] = -1;
	carPark[cpID][exteriorVW] = -1; 
	carPark[cpID][cpIcon] = -1;
	carPark[cpID][Price] = -1;
	DestroyPickup(carPark[cpID][cpIconID]);
	Delete3DTextLabel(carPark[cpID][cpLabel]); 
	fremove(file);
	format(file, sizeof(file), "You have deleted car park ID %d", cpID);
	SendClientMessage(playerid, -1, file);
	printf("[BoltCarParks] Car park %d deleted.", cpID); 
	return 1;
}
	
stock GetClosestCarPark(playerid)
{
	new Float:dist, id; 
	for(new i = 1; i < MAX_CAR_PARKS; i ++)
	{
		if(id < 1 || dist > GetPlayerDistanceFromPoint(playerid, carPark[i][exteriorPos][0], carPark[i][exteriorPos][1], carPark[i][exteriorPos][2]))
		{
			id = i;
			dist = GetPlayerDistanceFromPoint(playerid, carPark[i][exteriorPos][0], carPark[i][exteriorPos][1], carPark[i][exteriorPos][2]); 
		}
	}
	return id;
}
	
//21.5899,1840.9188,-93.1398	
CMD:entercarpark(playerid, params[])
{
	new id = GetClosestCarPark(playerid); 
	
	if(!IsPlayerInRangeOfPoint(playerid, 10, carPark[id][exteriorPos][0], carPark[id][exteriorPos][1], carPark[id][exteriorPos][2]))
		return SendClientMessage(playerid, -1, "You aren't close enough to any car park!"); 
	
	if(IsPlayerInAnyVehicle(playerid))
	{
		new vehID = GetPlayerVehicleID(playerid);
		SetVehiclePos(vehID, 21.5899,1840.9188,-93.1398);
		SetVehicleVirtualWorld(vehID, 1500 + id); 
	}
	else
	{
		SetPlayerPos(playerid, 21.5899,1840.9188,-93.1398);
	}
	SetPVarInt(playerid, "inCP", id);
	SetPlayerVirtualWorld(playerid, 1500 + id); 
	return 1;
}

CMD:exitcarpark(playerid, params[])
{
	new id = GetPVarInt(playerid, "inCP"); 
	
	if(id == -1)
		return SendClientMessage(playerid, -1, "You are not inside a car park!"); 
	
	if(!IsPlayerInRangeOfPoint(playerid, 10, 21.5899,1840.9188,-93.1398))
		return SendClientMessage(playerid, -1, "You aren't close enough to the exit!"); 
	
	if(IsPlayerInAnyVehicle(playerid))
	{
		if(GetPlayerMoney(playerid) < carPark[id][Price] && carPark[id][Price] > 0)
			return SendClientMessage(playerid, -1, "You can't afford this!"); 
		
		new vehID = GetPlayerVehicleID(playerid);
		SetVehiclePos(vehID, carPark[id][exteriorPos][0], carPark[id][exteriorPos][1], carPark[id][exteriorPos][2]);
		SetVehicleVirtualWorld(vehID, carPark[id][exteriorVW]);
		if(carPark[id][Price] > 0)
			GivePlayerMoney(playerid,  0 - carPark[id][Price]);
	}
	else SetPlayerPos(playerid, carPark[id][exteriorPos][0], carPark[id][exteriorPos][1], carPark[id][exteriorPos][2]);
	SetPlayerVirtualWorld(playerid, carPark[id][exteriorVW]); 
	SetPVarInt(playerid, "inCP", -1);
	return 1;
}
