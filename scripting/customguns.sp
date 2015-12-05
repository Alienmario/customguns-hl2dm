// Include libraries
#include <sourcemod>
#include <sdktools>
#include <sdkhooks>
#include <dhooks>

#include <customguns/smartdm>
#include <customguns/activity_list>
#include <customguns/drawingtools>
#include <customguns/const>
#include <customguns/stocks>
#include <customguns/settings>

// Include plugin parts
#include <customguns/globals>
#include <customguns/hooks>
#include <customguns/throwable>
#include <customguns/helpers>
#include <customguns/menu>
#include <customguns/addons_scope>

public Plugin myinfo = 
{
	name = "Custom guns", 
	author = "Alienmario", 
	description = "Custom guns plugin for HL2DM", 
	version = "1.1"
};

public APLRes AskPluginLoad2(Handle myself, bool late, char[] error, err_max)
{
	RegPluginLibrary("customguns");
	
	CreateNative("CG_IsClientHoldingCustomGun", Native_IsClientHoldingCustomGun);
	CreateNative("CG_GiveGun", Native_GiveGun);
	CreateNative("CG_ClearInventory", Native_ClearInventory);
	CreateNative("CG_SpawnGun", Native_SpawnGun);
	CreateNative("CG_PlayActivity", Native_PlayActivity);
	CreateNative("CG_PlayPrimaryAttack", Native_PlayPrimaryAttack);
	CreateNative("CG_PlaySecondaryAttack", Native_PlaySecondaryAttack);
	CreateNative("CG_SetPlayerAnimation", Native_SetPlayerAnimation);
	CreateNative("CG_GetShootPosition", Native_GetShootPosition);
	
	return APLRes_Success;
}

public Native_GiveGun(Handle plugin, numParams)
{
	int client = GetNativeCell(1);
	char classname[32];
	GetNativeString(2, classname, sizeof(classname));
	return addToInventory(client, classname, _, GetNativeCell(3));
}

public Native_ClearInventory(Handle plugin, numParams)
{
	int client = GetNativeCell(1);
	clearInventory(client, true);
}

public Native_SpawnGun(Handle plugin, numParams)
{
	char classname[32];
	float origin[3];
	GetNativeString(1, classname, sizeof(classname));
	GetNativeArray(2, origin, 3);
	return spawnGun(getIndex(classname), origin);
}

public Native_IsClientHoldingCustomGun(Handle plugin, numParams)
{
	int client = GetNativeCell(1);
	return (gunEnt[client] != -1);
}

public Native_SetPlayerAnimation(Handle plugin, numParams)
{
	SDKCall(CALL_SetAnimation, GetNativeCell(1), GetNativeCell(2));
}

public Native_GetShootPosition(Handle plugin, numParams)
{
	int client = GetNativeCell(1);
	float pos[3]; getShootPosition(client, pos);
	float forwardOffset = GetNativeCell(3);
	float rightOffset = GetNativeCell(4);
	float upOffset = GetNativeCell(5);
	
	float eyeAngles[3], fwd[3], right[3], up[3];
	GetClientEyeAngles(client, eyeAngles);
	GetAngleVectors(eyeAngles, fwd, right, up);
	pos[0] += fwd[0] * forwardOffset + right[0] * rightOffset +  up[0] * upOffset;
	pos[1] += fwd[1] * forwardOffset + right[1] * rightOffset +  up[1] * upOffset;
	pos[2] += fwd[2] * forwardOffset + right[2] * rightOffset +  up[2] * upOffset;
	
	SetNativeArray(2, pos, sizeof(pos));
}

public int Native_PlayActivity(Handle plugin, numParams)
{
	int weapon = GetNativeCell(1);
	Activity activity = GetNativeCell(2);
	SDKCall(CALL_SendWeaponAnim, weapon, activity);
	float seqDuration = GetEntPropFloat(weapon, Prop_Send, "m_flTimeWeaponIdle") - GetGameTime();
	return view_as<int>(seqDuration);
}

public int Native_PlayPrimaryAttack(Handle plugin, numParams)
{
	int weapon = GetNativeCell(1);
	SDKCall(CALL_SendWeaponAnim, weapon, ACT_VM_PRIMARYATTACK);
	float curtime = GetGameTime();
	float seqDuration = GetEntPropFloat(weapon, Prop_Send, "m_flTimeWeaponIdle") - curtime;
	SetEntPropFloat(weapon, Prop_Send, "m_flNextPrimaryAttack", curtime + seqDuration);
	SetEntPropFloat(weapon, Prop_Send, "m_flNextSecondaryAttack", curtime + seqDuration);
	return view_as<int>(seqDuration);
}

public int Native_PlaySecondaryAttack(Handle plugin, numParams)
{
	int weapon = GetNativeCell(1);
	SDKCall(CALL_SendWeaponAnim, weapon, ACT_VM_SECONDARYATTACK);
	float curtime = GetGameTime();
	float seqDuration = GetEntPropFloat(weapon, Prop_Send, "m_flTimeWeaponIdle") - curtime;
	SetEntPropFloat(weapon, Prop_Send, "m_flNextPrimaryAttack", curtime + seqDuration);
	SetEntPropFloat(weapon, Prop_Send, "m_flNextSecondaryAttack", curtime + seqDuration);
	return view_as<int>(seqDuration);
}

public OnPluginStart()
{
	/***************************/
	/********** HOOKS **********/
	/***************************/
	
	Handle gamedata = LoadGameConfigFile("customguns");
	
	if (!gamedata)
	{
		SetFailState("Failed to find gamedata 'customguns'");
	}
	
	int offset;
	
	{
		// void CBaseGrenade::Explode( CGameTrace *pTrace, int bitsDamageType ) // (trace_t)
		offset = GameConfGetOffset(gamedata, "Explode");
		DHOOK_Explode = DHookCreate(offset, HookType_Entity, ReturnType_Void, ThisPointer_CBaseEntity, Explode);
		DHookAddParam(DHOOK_Explode, HookParamType_ObjectPtr, -1);
		DHookAddParam(DHOOK_Explode, HookParamType_Int);
		
		// void CHL2MP_Player::FireBullets ( const FireBulletsInfo_t &info )
		offset = GameConfGetOffset(gamedata, "FireBullets");
		DHOOK_FireBullets = DHookCreate(offset, HookType_Entity, ReturnType_Void, ThisPointer_CBaseEntity, FireBullets);
		DHookAddParam(DHOOK_FireBullets, HookParamType_ObjectPtr, -1, DHookPass_ByVal);
		
		// Activity CBaseCombatCharacter::Weapon_TranslateActivity( Activity baseAct, bool *pRequired )
		offset = GameConfGetOffset(gamedata, "Weapon_TranslateActivity");
		DHOOK_TranslateActivity = DHookCreate(offset, HookType_Entity, ReturnType_Int, ThisPointer_CBaseEntity, TranslateActivity);
		DHookAddParam(DHOOK_TranslateActivity, HookParamType_Int);
		DHookAddParam(DHOOK_TranslateActivity, HookParamType_Bool);

		// void CBaseCombatWeapon::Operator_HandleAnimEvent( animevent_t *pEvent, CBaseCombatCharacter *pOperator )
		offset = GameConfGetOffset(gamedata, "Operator_HandleAnimEvent");
		DHOOK_Operator_HandleAnimEvent = DHookCreate(offset, HookType_Entity, ReturnType_Void, ThisPointer_CBaseEntity, Operator_HandleAnimEvent);
		DHookAddParam(DHOOK_Operator_HandleAnimEvent, HookParamType_ObjectPtr, -1);
		DHookAddParam(DHOOK_Operator_HandleAnimEvent, HookParamType_CBaseEntity);

		// float CBaseCombatWeapon::GetFireRate( void )
		offset = GameConfGetOffset(gamedata, "GetFireRate");
		DHOOK_GetFireRate = DHookCreate(offset, HookType_Entity, ReturnType_Float, ThisPointer_CBaseEntity, GetFireRate);
		
		// void CBaseCombatWeapon::AddViewKick( void )
		offset = GameConfGetOffset(gamedata, "AddViewKick");
		DHOOK_AddViewKick = DHookCreate(offset, HookType_Entity, ReturnType_Void, ThisPointer_CBaseEntity, AddViewKick);
		
		// bool CBaseCombatWeapon::ReloadOrSwitchWeapons( void )
		offset = GameConfGetOffset(gamedata, "ReloadOrSwitchWeapons");
		DHOOK_ReloadOrSwitchWeapons = DHookCreate(offset, HookType_Entity, ReturnType_Bool, ThisPointer_CBaseEntity, ReloadOrSwitchWeapons);
		
		// bool CBasePlayer::BumpWeapon( CBaseCombatWeapon *pWeapon )
		offset = GameConfGetOffset(gamedata, "BumpWeapon");
		DHOOK_BumpWeapon = DHookCreate(offset, HookType_Entity, ReturnType_Bool, ThisPointer_CBaseEntity, BumpWeapon);
		DHookAddParam(DHOOK_BumpWeapon, HookParamType_CBaseEntity);
		
		// bool CBaseCombatWeapon::Reload( void )
		offset = GameConfGetOffset(gamedata, "Reload");
		DHOOK_Reload = DHookCreate(offset, HookType_Entity, ReturnType_Bool, ThisPointer_CBaseEntity, Reload);
		
		// void CBaseCombatWeapon::ItemPostFrame( void )
		offset = GameConfGetOffset(gamedata, "ItemPostFrame");
		DHOOK_ItemPostFrame = DHookCreate(offset, HookType_Entity, ReturnType_Void, ThisPointer_CBaseEntity, ItemPostFrame);
		DHOOK_ItemPostFramePost = DHookCreate(offset, HookType_Entity, ReturnType_Void, ThisPointer_CBaseEntity, ItemPostFramePost);
		
		// void CBaseCombatWeapon::PrimaryAttack( void )
		offset = GameConfGetOffset(gamedata, "PrimaryAttack");
		DHOOK_PrimaryAttack = DHookCreate(offset, HookType_Entity, ReturnType_Void, ThisPointer_CBaseEntity, PrimaryAttack);
			
		// void CBaseCombatWeapon::SecondaryAttack( void )
		offset = GameConfGetOffset(gamedata, "SecondaryAttack");
		DHOOK_SecondaryAttack = DHookCreate(offset, HookType_Entity, ReturnType_Void, ThisPointer_CBaseEntity, SecondaryAttack);
		
		// bool CBaseCombatWeapon::Holster( CBaseCombatWeapon *pSwitchingTo )
		offset = GameConfGetOffset(gamedata, "Holster");
		DHOOK_Holster = DHookCreate(offset, HookType_Entity, ReturnType_Bool, ThisPointer_CBaseEntity, Holster);
		DHookAddParam(DHOOK_Holster, HookParamType_CBaseEntity);
		
		// void CBaseCombatWeapon::Drop( const Vector &vecVelocity )
		offset = GameConfGetOffset(gamedata, "Drop");
		DHOOK_Drop = DHookCreate(offset, HookType_Entity, ReturnType_Void, ThisPointer_CBaseEntity, Drop);
		DHookAddParam(DHOOK_Drop, HookParamType_VectorPtr, -1, DHookPass_ByRef);

		// void CBaseCombatWeapon:WeaponSound( WeaponSound_t sound_type, float soundtime = 0.0f )
		offset = GameConfGetOffset(gamedata, "WeaponSound");
		DHOOK_WeaponSound = DHookCreate(offset, HookType_Entity, ReturnType_Void, ThisPointer_CBaseEntity, WeaponSound);
		DHookAddParam(DHOOK_WeaponSound, HookParamType_Int);
		DHookAddParam(DHOOK_WeaponSound, HookParamType_Float);

		// int CBaseCombatWeapon::GetDefaultClip1( void )
		offset = GameConfGetOffset(gamedata, "GetDefaultClip1");
		DHOOK_GetDefaultClip1 = DHookCreate(offset, HookType_Entity, ReturnType_Int, ThisPointer_CBaseEntity, GetDefaultClip1);
	}
	
	/***************************/
	/********** CALLS **********/
	/***************************/
	
	{
		// bool CBaseCombatWeapon::SendWeaponAnim( int iActivity )
		StartPrepSDKCall(SDKCall_Entity);
		PrepSDKCall_SetFromConf(gamedata, SDKConf_Virtual, "SendWeaponAnim");
		PrepSDKCall_AddParameter(SDKType_PlainOldData, SDKPass_Plain);
		PrepSDKCall_SetReturnInfo(SDKType_Bool, SDKPass_Plain);
		CALL_SendWeaponAnim = EndPrepSDKCall();

		// void CBaseCombatWeapon::SendViewModelAnim( int nSequence )
		StartPrepSDKCall(SDKCall_Entity);
		PrepSDKCall_SetFromConf(gamedata, SDKConf_Virtual, "SendViewModelAnim");
		PrepSDKCall_AddParameter(SDKType_PlainOldData, SDKPass_Plain);
		CALL_SendViewModelAnim = EndPrepSDKCall();
		
		// bool CBaseCombatWeapon::HasPrimaryAmmo( void )
		StartPrepSDKCall(SDKCall_Entity);
		PrepSDKCall_SetFromConf(gamedata, SDKConf_Virtual, "HasPrimaryAmmo");
		PrepSDKCall_SetReturnInfo(SDKType_Bool, SDKPass_Plain);
		CALL_HasPrimaryAmmo = EndPrepSDKCall();
		
		// bool CBaseCombatWeapon::HasSecondaryAmmo( void )
		StartPrepSDKCall(SDKCall_Entity);
		PrepSDKCall_SetFromConf(gamedata, SDKConf_Virtual, "HasSecondaryAmmo");
		PrepSDKCall_SetReturnInfo(SDKType_Bool, SDKPass_Plain);
		CALL_HasSecondaryAmmo = EndPrepSDKCall();
		
		// bool CBaseCombatWeapon::UsesClipsForAmmo1( void )
		StartPrepSDKCall(SDKCall_Entity);
		PrepSDKCall_SetFromConf(gamedata, SDKConf_Virtual, "UsesClipsForAmmo1");
		PrepSDKCall_SetReturnInfo(SDKType_Bool, SDKPass_Plain);
		CALL_UsesClipsForAmmo1 = EndPrepSDKCall();
		
		// void CBaseCombatWeapon:WeaponSound( WeaponSound_t sound_type, float soundtime = 0.0f )
		StartPrepSDKCall(SDKCall_Entity);
		PrepSDKCall_SetFromConf(gamedata, SDKConf_Virtual, "WeaponSound");
		PrepSDKCall_AddParameter(SDKType_PlainOldData, SDKPass_Plain);
		PrepSDKCall_AddParameter(SDKType_Float, SDKPass_Plain);
		CALL_WeaponSound = EndPrepSDKCall();
		
		// void CBaseCombatWeapon::StopWeaponSound( WeaponSound_t sound_type )
		StartPrepSDKCall(SDKCall_Entity);
		PrepSDKCall_SetFromConf(gamedata, SDKConf_Virtual, "StopWeaponSound");
		PrepSDKCall_AddParameter(SDKType_PlainOldData, SDKPass_Plain);
		CALL_StopWeaponSound = EndPrepSDKCall();
		
		// void CBaseCombatWeapon::CheckRespawn( void )
		StartPrepSDKCall(SDKCall_Entity);
		PrepSDKCall_SetFromConf(gamedata, SDKConf_Virtual, "CheckRespawn");
		CALL_CheckRespawn = EndPrepSDKCall();
		
		// bool CHL2MP_Player::Weapon_Switch( CBaseCombatWeapon *pWeapon, int viewmodelindex = 0)
		StartPrepSDKCall(SDKCall_Player);
		PrepSDKCall_SetFromConf(gamedata, SDKConf_Virtual, "Weapon_Switch");
		PrepSDKCall_AddParameter(SDKType_CBaseEntity, SDKPass_Pointer);
		PrepSDKCall_AddParameter(SDKType_PlainOldData, SDKPass_Plain);
		CALL_Weapon_Switch = EndPrepSDKCall();
		
		// int CHL2_Player::GiveAmmo( int nCount, int nAmmoIndex, bool bSuppressSound)
		StartPrepSDKCall(SDKCall_Player);
		PrepSDKCall_SetFromConf(gamedata, SDKConf_Virtual, "GiveAmmo");
		PrepSDKCall_AddParameter(SDKType_PlainOldData, SDKPass_Plain);
		PrepSDKCall_AddParameter(SDKType_PlainOldData, SDKPass_Plain);
		PrepSDKCall_AddParameter(SDKType_Bool, SDKPass_Plain);
		PrepSDKCall_SetReturnInfo(SDKType_PlainOldData, SDKPass_Plain);
		CALL_GiveAmmo = EndPrepSDKCall();
		
		// int CBaseCombatCharacter::GetAmmoCount( int iAmmoIndex )
		StartPrepSDKCall(SDKCall_Player);
		PrepSDKCall_SetFromConf(gamedata, SDKConf_Virtual, "GetAmmoCount");
		PrepSDKCall_AddParameter(SDKType_PlainOldData, SDKPass_Plain);
		PrepSDKCall_SetReturnInfo(SDKType_PlainOldData, SDKPass_Plain);
		CALL_GetAmmoCount = EndPrepSDKCall();
		
		// void CBaseCombatCharacter::RemoveAmmo( int iCount, int iAmmoIndex )
		StartPrepSDKCall(SDKCall_Player);
		PrepSDKCall_SetFromConf(gamedata, SDKConf_Virtual, "RemoveAmmo");
		PrepSDKCall_AddParameter(SDKType_PlainOldData, SDKPass_Plain);
		PrepSDKCall_AddParameter(SDKType_PlainOldData, SDKPass_Plain);
		CALL_RemoveAmmo = EndPrepSDKCall();
		
		// void CHL2MP_Player::SetAnimation( PLAYER_ANIM playerAnim )
		StartPrepSDKCall(SDKCall_Player);
		PrepSDKCall_SetFromConf(gamedata, SDKConf_Virtual, "SetAnimation");
		PrepSDKCall_AddParameter(SDKType_PlainOldData, SDKPass_Plain);
		CALL_SetAnimation = EndPrepSDKCall();
			
		// Vector CBaseCombatCharacter::Weapon_ShootPosition( )
		StartPrepSDKCall(SDKCall_Player);
		PrepSDKCall_SetFromConf(gamedata, SDKConf_Virtual, "Weapon_ShootPosition");
		PrepSDKCall_SetReturnInfo(SDKType_Vector, SDKPass_ByValue);
		CALL_ShootPosition = EndPrepSDKCall();
	}
	
	CloseHandle(gamedata);
	
	/***************************/
	/********** SETUP **********/
	/***************************/
	
	gunClassNames = CreateArray(32);
	gunNames = CreateArray(32);
	gunModels = CreateArray(PLATFORM_MAX_PATH);
	gunType = CreateArray();
	gunDmg = CreateArray();
	gunAnimPrefix = CreateArray(32);
	gunType = CreateArray();
	gunSpread = CreateArray();
	gunRof = CreateArray();
	gunDelay = CreateArray();
	gunDelaySequence = CreateArray();
	gunDelayFireCooldown = CreateArray();
	gunViewKickScale = CreateArray();
	gunViewKickAngle = CreateArray();
	gunViewKickTime = CreateArray();
	gunModelIndexes = CreateArray();
	gunDownloads = CreateArray(PLATFORM_MAX_PATH);
	gunAdminLevel = CreateArray();
	gunPersistent = CreateArray();
	gunGive = CreateArray();
	gunGiveMasterWeapon = CreateArray(32);
	gunBind = CreateArray(32);
	gunFireLoopFix = CreateArray();
	gunFireLoopLength = CreateArray();
	gunFireVisible = CreateArray();
	gunScopeFov = CreateArray();
	gunScopeOverlay = CreateArray(PLATFORM_MAX_PATH);
	gunScopeSoundOn = CreateArray(PLATFORM_MAX_PATH);
	gunScopeSoundOff = CreateArray(PLATFORM_MAX_PATH);
	
	Throwable_OnPluginStart();
	loadConfig();
	
	LoadTranslations("common.phrases");
	HookEvent("player_spawn", OnSpawn);
	
	customguns_default = CreateConVar("customguns_default", "weapon_hands", "The preferred custom weapon that players should spawn with", FCVAR_PLUGIN);
	customguns_global_switcher = CreateConVar("customguns_global_switcher", "1", "Enables fast switching from any weapon by holding reload button. If 0, players can switch only when holding a custom weapon.", FCVAR_PLUGIN, true, 0.0, true, 1.0);
	customguns_autogive = CreateConVar("customguns_autogive", "1", "Globally enables/disables auto-giving of all custom weapons", FCVAR_PLUGIN, true, 0.0, true, 1.0);

	PrimaryAttackForward = CreateGlobalForward("CG_OnPrimaryAttack", ET_Ignore, Param_Cell, Param_Cell);
	SecondaryAttackForward = CreateGlobalForward("CG_OnSecondaryAttack", ET_Ignore, Param_Cell, Param_Cell);
	
	RegAdminCmd("sm_customgun", CustomGun, ADMFLAG_ROOT, "Spawns a custom gun by classname");
	RegAdminCmd("sm_customguns", CustomGun, ADMFLAG_ROOT, "Spawns a custom gun by classname");
	RegAdminCmd("sm_seqtest", SeqTest, ADMFLAG_ROOT, "Viewmodel sequence test");
	
	if(GetConVarBool(customguns_autogive)){
		for (int i = 1; i <= MaxClients; i++) {
			if (IsClientInGame(i)) {
				OnClientPutInServer(i);
				if(!IsFakeClient(i) && IsPlayerAlive(i)){
					addSpawnWeapons(i);
					giveCustomGun(i);
				}
			}
		}
	}
}

public OnPluginEnd(){
	for (int i = 1; i <= MaxClients; i++) 
		if (IsClientInGame(i))
			removeCustomWeapon(i)
}

public Action SeqTest(int client, int args) {
	char seq[32]; GetCmdArgString(seq, 32);
	vmSeq(client, StringToInt(seq), 4.0);
	return Plugin_Handled;
}

public Action CustomGun(int client, int args) {
	if (args == 1 || args == 2) {
		char classname[32]; GetCmdArg(1, classname, 32);
		if (args == 1) {
			if(client == 0){
				ReplyToCommand(client, "Sorry, you need to be in-game to spawn guns at your origin");
				return Plugin_Handled;
			}
			float origin[3];
			GetClientAbsOrigin(client, origin);
			origin[2] += 25.0;
			if (spawnGun(getIndex(classname), origin) == -1) {
				ReplyToCommand(client, "Unable to spawn %s", classname);
			} else {
				ReplyToCommand(client, "You have spawned %s", classname);
			}
		} else if (args == 2) {
			char arg2[32]; GetCmdArg(2, arg2, 32);
			int target;
			if ((target = FindTarget(client, arg2, true, false)) == -1) {
				return Plugin_Handled;
			}
			if (!IsPlayerAlive(target)) {
				ReplyToCommand(client, "But he's dead!");
			}
			else if (!addToInventory(target, classname, true, true)) {
				ReplyToCommand(client, "Cannot give %s to %N. Either unknown weapon or target has it already.", classname, target);
			}
			else {
				ReplyToCommand(client, "You have given %s to %N", classname, target);
				PrintToChat(target, "Admin %N gave you %s", client, classname);
			}
		}
	} else {
		ReplyToCommand(client, "Usage: !customgun <classname> [player]");
		ReplyToCommand(client, "\nListing classnames:");
		for (int i = 0; i < GetArraySize(gunClassNames); i++) {
			char sWeapon[32];
			GetArrayString(gunClassNames, i, sWeapon, sizeof(sWeapon));
			ReplyToCommand(client, sWeapon);
		}
	}
	return Plugin_Handled;
}

public OnMapStart() {
	modelText = PrecacheModel(MODEL_TEXT, true);
	//modelBorder = PrecacheModel(MODEL_BORDER, true);
	PrecacheSound(SND_OPEN, true);
	PrecacheSound(SND_CLOSE_OK, true);
	PrecacheScriptSound(SND_CLOSE_CANC);
	PrecacheScriptSound(SND_SELECT);
	Throwable_OnMapStart();
	
	ClearArray(gunModelIndexes);
	char buffer[PLATFORM_MAX_PATH];
	for (int i = 0; i < GetArraySize(gunModels); i++) {
		GetArrayString(gunModels, i, buffer, sizeof(buffer));
		PushArrayCell(gunModelIndexes, PrecacheModel(buffer, true));
	}
	for (int i = 0; i < GetArraySize(gunDownloads); i++) {
		GetArrayString(gunDownloads, i, buffer, sizeof(buffer));
		Downloader_AddFileToDownloadsTable(buffer);
	}
}

public OnClientPutInServer(int client) {
	if (!IsFakeClient(client)) {
		inventory[client] = CreateArray();
		inventoryWheel[client] = CreateArray(3);
		inventoryAnimScale[client] = CreateArray();
		inventoryAmmo[client] = CreateArray();
		inventoryAmmoType[client] = CreateArray();
		
 		SDKHook(client, SDKHook_WeaponSwitch, OnWeaponSwitch);
		SDKHook(client, SDKHook_WeaponSwitchPost, OnWeaponSwitchPost);
		SDKHook(client, SDKHook_WeaponEquipPost, OnWeaponEquipPost);
 		DHookEntity(DHOOK_FireBullets, false, client);
 		DHookEntity(DHOOK_FireBullets, true, client);
		DHookEntity(DHOOK_TranslateActivity, false, client);
		DHookEntity(DHOOK_BumpWeapon, false, client);
		
		ScopeInit(client);
	}
}

public OnClientDisconnect(int client) {
	if (!IsFakeClient(client)) {
		firing[client] = false;
		open[client] = false;
		nextFireSound[client] = 0.0;
		nextDraw[client] = 0.0;
		firstOpen[client] = 0.0;
		delete inventory[client];
		delete inventoryWheel[client];
		delete inventoryAnimScale[client];
		delete inventoryAmmo[client];
		delete inventoryAmmoType[client];
	}
}

public OnSpawn(Handle event, const char[] name, bool dontBroadcast)
{
	int client = GetClientOfUserId(GetEventInt(event, "userid"));
	if (!IsFakeClient(client)) {
		selectedGunIndex[client] = -1;
		ClearArray(inventory[client]);
		ClearArray(inventoryAnimScale[client]);
		ClearArray(inventoryAmmo[client]);
		ClearArray(inventoryAmmoType[client]);
		
		if(GetConVarBool(customguns_autogive)){
			addSpawnWeapons(client);
			giveCustomGun(client);
			// delay against weapon strippers
			CreateTimer(1.0, tGiveCustomGun, GetEventInt(event, "userid"), TIMER_FLAG_NO_MAPCHANGE);
		}
	}
}

public Action tGiveCustomGun(Handle timer, any userid)
{
	int client = GetClientOfUserId(userid);
	if (client > 0 && IsClientInGame(client) && IsPlayerAlive(client)) {
		giveCustomGun(client);
	}
}

/*
** index = -1 >> give physical weapon if client doesn't have it yet
** valid index >> give new weapon of index
*/
stock giveCustomGun(client, int index = -1, bool switchTo = false) {
	if (GetArraySize(gunClassNames) > 0) {
		
		if(index == -1){
			if(!hasCustomWeapon(client)){
				if(selectedGunIndex[client] == -1){
					index = reselectIndex(client);
				} else {
					index = selectedGunIndex[client];
				}
			} else return;
		}
	
		removeCustomWeapon(client);
		
		CLIENT_BEING_EQUIPPED = client;
		
		int ent = spawnGun(index);
		if (ent != -1) {
			selectedGunIndex[client] = index;
			EquipPlayerWeapon(client, ent);
			if (switchTo) {
				SDKCall(CALL_Weapon_Switch, client, ent, 0);
				CreateTimer(0.1, deploySound, EntIndexToEntRef(ent));
			}
		} else {
			selectedGunIndex[client] = -1;
		}
		CLIENT_BEING_EQUIPPED = -1;
	}
}

int spawnGun(int index, const float origin[3] = NULL_VECTOR) {
	if (0 > index || index >= GetArraySize(gunClassNames)) {
		return -1;
	}
	
	// basehl2mpcombatweapon : crashes client (about 0.5s after deployed) or has to be precached on client
	// weapon_hl2mp_base : the same as above, flickers
	// basehlcombatweapon : pretty good, but overshadowing with other weapons at slot 0,0
	// weapon_cubemap : also good, but does not show stock ammo of player (pesky cubemap has -1 clips and no ammotype on client by default)
	int ent = CreateEntityByName("weapon_cubemap");
	if (ent != -1) {
	
 		GunType guntype = GetArrayCell(gunType, index);
		
		//inventory ammo save-load hooks
		DHookEntity(DHOOK_Holster, true, ent);
		DHookEntity(DHOOK_GetDefaultClip1, true, ent);

		DHookEntity(DHOOK_SecondaryAttack, true, ent);
		DHookEntity(DHOOK_Drop, false, ent);
		
		if(guntype == GunType_Bullet)
		{
			DHookEntity(DHOOK_GetFireRate, false, ent);
			DHookEntity(DHOOK_AddViewKick, false, ent);
			DHookEntity(DHOOK_ReloadOrSwitchWeapons, true, ent);
			DHookEntity(DHOOK_Reload, true, ent);		
			DHookEntity(DHOOK_PrimaryAttack, true, ent);
			
			if (GetArrayCell(gunDelay, index) > 0.0){
				DHookEntity(DHOOK_ItemPostFrame, false, ent);
			}
			
			if (GetArrayCell(gunFireLoopFix, index)) {
				DHookEntity(DHOOK_ItemPostFramePost, true, ent);
				DHookEntity(DHOOK_WeaponSound, false, ent);
			}
		}
		else if (guntype == GunType_Throwable)
		{
			DHookEntity(DHOOK_ItemPostFrame, false, ent);
			DHookEntity(DHOOK_PrimaryAttack, false, ent);
			DHookEntity(DHOOK_Operator_HandleAnimEvent, false, ent);
		} else {
			DHookEntity(DHOOK_ItemPostFrame, true, ent);
			DHookEntity(DHOOK_PrimaryAttack, true, ent);
		}
		
		//SetEntProp(ent, Prop_Data, "m_bReloadsSingly", 1);
		
		char weapon[32];
		GetArrayString(gunClassNames, index, weapon, sizeof(weapon));
		DispatchKeyValue(ent, "classname", weapon);
		DispatchSpawn(ent);
		ActivateEntity(ent);
 		
		 // make it selectable
		if(guntype == GunType_Custom){
			SetEntProp(ent, Prop_Send, "m_iPrimaryAmmoType", 12);
			SetEntProp(ent, Prop_Send, "m_iClip2", 1);
		} else if (guntype == GunType_Throwable){
			SetEntProp(ent, Prop_Send, "m_iClip2", 1);
		}
		
		TeleportEntity(ent, origin, NULL_VECTOR, NULL_VECTOR);
	}
	return ent;
}

public Action OnPlayerRunCmd(client, &buttons, &impulse, float vel[3], float angles[3], &weapon, &subtype, &cmdnum, &tickcount, &seed, mouse[2])
{
	if(!IsFakeClient(client)){
		
		char sWeapon[32];
		GetClientWeapon(client, sWeapon, sizeof(sWeapon));
		int gunIndex = getIndex(sWeapon);
		
		//handle opening/closing menu
		if (!open[client] && IsPlayerAlive(client) && inventory[client] && GetArraySize(inventory[client])>0 && GetEntProp(client, Prop_Send, "m_iTeamNum") > 1) {
			if (buttons & IN_ATTACK3) {
				onMenuOpening(client);
				open[client] = true;
			}
			else if (buttons & IN_RELOAD) {
				if( !(!GetConVarBool(customguns_global_switcher) && gunIndex == -1) ){
					if (!(GetEntProp(client, Prop_Data, "m_nOldButtons") & IN_RELOAD)) {
						firstOpen[client] = GetGameTime();
					}
					else if (GetGameTime() >= firstOpen[client] + 0.25) {
						//if (!StrEqual(sWeapon, "weapon_physcannon")) {
							onMenuOpening(client);
							open[client] = true;
						//}
					}
				}
			}
		}
		//if not holding any button or dead -> close the menu
		else if (open[client] && (!(buttons & IN_ATTACK3) && !(buttons & IN_RELOAD)) || !IsPlayerAlive(client)) {
			if (IsClientInGame(client) && IsPlayerAlive(client)) {
				onMenuClosing(client);
			}
			open[client] = false;
		}
		
		if (open[client]) {
			drawMenu(client);
		}
		
		// check scope
		ScopeThink(client, buttons, gunIndex, open[client]);
	}
}