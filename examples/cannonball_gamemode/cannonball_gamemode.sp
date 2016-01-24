#include <sourcemod>
#include <sdktools>
#include <sdkhooks>
#include <customguns>

public Plugin myinfo = 
{
	name = "Cannon ball gamemode", 
	author = "Alienmario", 
	description = "Cannon ball gamemode", 
	version = "1.0"
};

bool enabled;
Handle autogive;

public OnPluginStart(){
	HookEvent("player_spawn", OnSpawn);
	RegAdminCmd("sm_cannon", ToggleMode, ADMFLAG_ROOT, "Toggle cannon ball gamemode");
}

public OnAllPluginsLoaded(){
	autogive = FindConVar("customguns_autogive");
	if(!autogive){
		SetFailState("Unable to find customguns_autogive convar. Is customguns plugin running?");
	}
}

public OnMapEnd(){
	if(enabled) ToggleMode(0, 0);
}

public OnClientPutInServer(int client){
	SDKHook(client, SDKHook_WeaponCanUsePost, WeaponBlock);
}

public Action ToggleMode(int client, int args){
	static bool autoGiveRevertVal;
	if(enabled)
	{
		// revert autogive cvar
		SetConVarBool(autogive, autoGiveRevertVal);
		enabled = false;
		PrintToChatAll("[Cannonball mode disabled]");
	} 
	else
	{
		autoGiveRevertVal = GetConVarBool(autogive);
		SetConVarBool(autogive, false);
		enabled = true;
		PrintToChatAll("[Cannonball mode enabled]");
		
		for(int i = 1; i<=MaxClients; i++){
			if(IsClientInGame(i) && !IsFakeClient(i) && IsPlayerAlive(i)){
				removeWeapons(i);
				CG_ClearInventory(i);
				CG_GiveGun(i, "weapon_cannon", true);
			}
		}
	}
	return Plugin_Handled;
}

public OnSpawn(Handle event, const char[] name, bool dontBroadcast)
{
	if(enabled) CreateTimer(0.1, equipClient, GetEventInt(event, "userid"), TIMER_FLAG_NO_MAPCHANGE);
}

public Action equipClient(Handle timer, any userid){
	int client = GetClientOfUserId(userid);
	if(client > 0 && !IsFakeClient(client) && IsPlayerAlive(client)){
		removeWeapons(client);
		CG_GiveGun(client, "weapon_cannon", true);
	}
}

void removeWeapons(int client){
	int offset = FindDataMapOffs(client, "m_hMyWeapons") - 4;
	for (new i = 0; i < MAX_WEAPONS; i++) {
		offset += 4;
		
		int weapon = GetEntDataEnt2(client, offset);
		if(weapon != -1){
			RemovePlayerItem(client, weapon);
			AcceptEntityInput(weapon, "kill");
		}
	}
}

public WeaponBlock(client, weapon)
{
	if(enabled){
		char sWeapon[32];
		GetEdictClassname(weapon, sWeapon, sizeof(sWeapon));
		if(!StrEqual(sWeapon, "weapon_cannon")){
			RemovePlayerItem(client, weapon);
			AcceptEntityInput(weapon, "kill");
		}
	}
}