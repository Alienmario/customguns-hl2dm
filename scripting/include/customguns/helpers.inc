#if defined _CustomGuns_Helpers_included
	#endinput
#endif
#define _CustomGuns_Helpers_included


loadConfig() {
	DirectoryListing dir = OpenDirectory("scripts/", true);
	if (dir) {
		char buffer[PLATFORM_MAX_PATH];
		char filename[PLATFORM_MAX_PATH];
		FileType type;
		while (dir.GetNext(filename, sizeof(filename), type)) {
			if (type == FileType_File) {
				if (StrContains(filename, "weapon_", false) == 0 && StrEqual(filename[strlen(filename) - 3], "txt", false)) {
					
					Format(buffer, sizeof(buffer), "scripts/%s", filename);
					Handle kv = CreateKeyValues("customguns");
					if (!FileToKeyValues(kv, buffer)) {
						delete kv;
						LogError("Failed to parse file: %s", buffer);
						continue;
					}
					
					char animPrefix[32];
					KvGetString(kv, "anim_prefix", animPrefix, sizeof(animPrefix), "");
					if (KvGotoFirstSubKey(kv)) {
						if (KvJumpToKeyFix(kv, "CustomGunsPluginData")) {
							PushArrayString(gunAnimPrefix, animPrefix);
							Format(filename, strlen(filename) - 3, filename);
							PushArrayString(gunClassNames, filename);
							KvGetString(kv, "name", buffer, 32, "");
							PushArrayString(gunNames, buffer);
							KvGetString(kv, "model", buffer, sizeof(buffer), "");
							PushArrayString(gunModels, buffer);
							PushArrayCell(gunSkin, KvGetNum(kv, "skin", 0));
							PushArrayCell(gunGive, KvGetNum(kv, "give_on_spawn", 1));
							KvGetString(kv, "give_with_weapon", buffer, 32, "");
							PushArrayString(gunGiveMasterWeapon, buffer);
							KvGetString(kv, "bind_to_weapon", buffer, 32, "");
							PushArrayString(gunBind, buffer);
							PushArrayCell(gunPersistent, KvGetNum(kv, "persistent", 0));
							KvGetString(kv, "admin_level", buffer, sizeof(buffer), "-1");
							PushArrayCell(gunAdminLevel, StrEqual(buffer, "-1")? -1 : ReadFlagString(buffer));
							
							PushArrayCell(gunScopeFov, KvGetNum(kv, "scope_fov", -1));
							KvGetString(kv, "scope_overlay", buffer, sizeof(buffer), "");
							PushArrayString(gunScopeOverlay, buffer);
							KvGetString(kv, "scope_sound_on", buffer, sizeof(buffer), "");
							PushArrayString(gunScopeSoundOn, buffer);
							KvGetString(kv, "scope_sound_off", buffer, sizeof(buffer), "");
							PushArrayString(gunScopeSoundOff, buffer);
							
							KvGetString(kv, "weapon_type", buffer, sizeof(buffer), "");
							
							bool shouldGoBack = false;
							
							if (StrEqual(buffer, "throwable", false))
							{
								shouldGoBack = KvGotoFirstSubKey(kv);
								if(!KvJumpToKeyFix(kv, "throwable_settings")){
									LogError("Weapon type throwable but throwable_settings block not found for %s", filename);
								}
								PushArrayCell(gunType, GunType_Throwable);
								
							}
							else if (StrEqual(buffer, "custom", false))
							{
								PushArrayCell(gunType, GunType_Custom);
							}
							else
							{
								shouldGoBack = KvGotoFirstSubKey(kv);
								if(!KvJumpToKeyFix(kv, "bullet_settings")){
									if (StrEqual(buffer, "bullet", false)) LogError("Weapon type bullet but bullet_settings block not found for %s", filename);
								}
								PushArrayCell(gunType, GunType_Bullet);
							}
							
							// Load bullet weapon
							{
								PushArrayCell(gunDmg, KvGetNum(kv, "damage", 0));
								PushArrayCell(gunFireVisible, KvGetNum(kv, "bullet_fire_visible", 1));
								PushArrayCell(gunFireLoopFix, KvGetNum(kv, "fire_sound_is_loop", 0));
								PushArrayCell(gunFireLoopLength, KvGetFloat(kv, "fire_sound_loop_length", 0.0));
								
								PushArrayCell(gunSpread, KvGetFloat(kv, "spread", 0.0));
								PushArrayCell(gunRof, KvGetFloat(kv, "fire_rate", 0.075));
								
								PushArrayCell(gunDelay, KvGetFloat(kv, "fire_delay", 0.0));
								PushArrayCell(gunDelaySequence, KvGetNum(kv, "fire_delay_sequence", 0));
								PushArrayCell(gunDelayFireCooldown, KvGetFloat(kv, "fire_delay_cooldown", 0.0));
								
								PushArrayCell(gunViewKickScale, KvGetFloat(kv, "view_kick_scale", 1.0));
								PushArrayCell(gunViewKickAngle, KvGetFloat(kv, "view_kick_max_angle", 1.0));
								PushArrayCell(gunViewKickTime, KvGetFloat(kv, "view_kick_max_time", 2.0));
							}
							
							// Load throwable weapon
							{
								KvGetString(kv, "throwable_model", buffer, sizeof(buffer), "");
								PushArrayString(gunThrowModel, buffer);
								
								PushArrayCell(gunThrowForce, KvGetFloat(kv, "throw_force", 800.0));
								PushArrayCell(gunThrowPullback, KvGetNum(kv, "supports_pullback", 0));
								PushArrayCell(gunThrowForceAttack, KvGetNum(kv, "unlimited_ammo_fixup", 0));
								PushArrayCell(gunThrowTrailType, KvGetNum(kv, "trail_type", 0));
								PushArrayCell(gunThrowExplType, KvGetNum(kv, "explosion_type", 0));
								PushArrayCell(gunThrowRadius, KvGetFloat(kv, "explosion_radius", 150.0));
								PushArrayCell(gunThrowDamage, KvGetFloat(kv, "explosion_damage", 100.0));
								PushArrayCell(gunThrowTimer, KvGetFloat(kv, "timer", -1.0));
								PushArrayCell(gunThrowGravity, KvGetFloat(kv, "throwable_gravity", 1.0));
								PushArrayCell(gunThrowFriction, KvGetFloat(kv, "throwable_friction", 1.0));
								PushArrayCell(gunThrowAngVelScale, KvGetFloat(kv, "angvel_scale", 1.0));
								
								// Molotov settings
								PushArrayCell(gunThrowMoloGibs, KvGetNum(kv, "molotov_gibs", 10));
								PushArrayCell(gunThrowMoloGibsLife, KvGetFloat(kv, "molotov_gibs_life", 17.0));
								PushArrayCell(gunThrowMoloGibsBurnout, KvGetFloat(kv, "molotov_gibs_burnout", 15.0));
								PushArrayCell(gunThrowMoloGibsVelocity, KvGetFloat(kv, "molotov_gibs_velocity", 150.0));
								PushArrayCell(gunThrowMoloGibsMaxVariation, KvGetFloat(kv, "molotov_gibs_velocity_max_var", 4.0));
							}
							
							if(shouldGoBack) KvGoBack(kv);
							
							if (KvGotoFirstSubKey(kv)){
								if (KvJumpToKeyFix(kv, "download")) {
									if (KvGotoFirstSubKey(kv, false)) {
										do {
											KvGetString(kv, NULL_STRING, buffer, sizeof(buffer));
											PushArrayString(gunDownloads, buffer);
										} while (KvGotoNextKey(kv, false));
										KvGoBack(kv);
									}
								}
							}
						}
					}
					delete kv;
				}
			}
		}
		delete dir;
	} else {
		SetFailState("Failed to find weapon scripts");
	}
}

bool KvJumpToKeyFix(Handle kv, const char[] key) {
	char section[255];
	do {
		if (KvGetSectionName(kv, section, sizeof(section))) {
			if (StrEqual(section, key))return true;
		}
	} while (KvGotoNextKey(kv));
	return false;
} 

void removeCustomWeapon(int client){
	//Loop through all client weapons and remove ours
	int offset = FindDataMapOffs(client, "m_hMyWeapons") - 4;
	for (new i = 0; i < MAX_WEAPONS; i++) {
		offset += 4;
		
		int weapon = GetEntDataEnt2(client, offset);
		if (isCustom(weapon)) {
			RemovePlayerItem(client, weapon);
			
			// Let the engine kill the weapon later, instead of a timer, in case of plugin unload (killing it now can cause crashes)
			SetVariantString("OnUser4 !self:kill::0.1:-1")
			AcceptEntityInput(weapon, "addoutput");
			AcceptEntityInput(weapon, "FireUser4");
		}
	}
}

bool hasCustomWeapon(int client){
	int offset = FindDataMapOffs(client, "m_hMyWeapons") - 4;
	for (new i = 0; i < MAX_WEAPONS; i++) {
		offset += 4;
		
		int weapon = GetEntDataEnt2(client, offset);
		if (isCustom(weapon)) {
			return true;
		}
	}
	return false;
}

void addSpawnWeapons(client) {
	int size = GetArraySize(gunClassNames);
	for (int i = 0; i < size; i++) {
		if (GetArrayCell(gunGive, i)) {
			addToInventoryIndex(client, i, false);
		}
	}
	
	generateWheel(client);
}

stock bool addToInventory(int client, const char[] weapon, bool refreshWheel = true, bool ignoreAdminLevel = false) {
	return addToInventoryIndex(client, getIndex(weapon), refreshWheel, ignoreAdminLevel);
}

stock bool addToInventoryIndex(int client, int weaponIndex, bool refreshWheel = true, bool ignoreAdminLevel = false) {
	if (weaponIndex != -1) {
		if (IsPlayerAlive(client) && canHaveGun(client, weaponIndex, ignoreAdminLevel)) {
			PushArrayCell(inventory[client], weaponIndex);
			PushArrayCell(inventoryAnimScale[client], 0.0);
			PushArrayCell(inventoryAmmo[client], -1);
			PushArrayCell(inventoryAmmoType[client], -1);
			itemPickup(client);
			
			if (refreshWheel) {
				generateWheel(client);
			}
			
			if (preferedGunIndex[client] == weaponIndex) {
				giveCustomGun(client, preferedGunIndex[client], true);
			} else if (refreshWheel) {
				giveCustomGun(client, selectedGunIndex[client]==-1? weaponIndex : -1);
			}
			return true;
		}
	}
	return false;
}

void clearInventory(int client, bool clearPersistent = false){
	if(inventory[client]){
		for (int i = 0; i < GetArraySize(inventory[client]); i++) {
			int gunIndex = GetArrayCell(inventory[client], i);
			if (GetArrayCell(gunPersistent, gunIndex)) {
				if(!clearPersistent) continue;
			}
			if(gunIndex == selectedGunIndex[client]){
				selectedGunIndex[client] = -1;
			}
			RemoveFromArray(inventory[client], i);
			RemoveFromArray(inventoryAnimScale[client], i);
			RemoveFromArray(inventoryAmmo[client], i);
			RemoveFromArray(inventoryAmmoType[client], i);
			i--;
		}
		generateWheel(client);
	}
}

int selectBestIndex(int client){
	if(preferedGunIndex[client] != -1 && canSelectInInventory(client, preferedGunIndex[client])){
		return preferedGunIndex[client];
	}
	
	int defaultIndex = getDefaultIndex();
	int index = -1;
	if(inventory[client]){
		int size = GetArraySize(inventory[client]);
		for (int i = 0; i < size; i++) {
			if(canSelectItem(client, i)){
				int gunIndex = GetArrayCell(inventory[client], i);
				if(defaultIndex == gunIndex){
					return gunIndex;
				}
				index = gunIndex;
			}
		}
	}
	return index;
}

void setInventoryAmmo(int client, int weaponIndex, int ammo, int ammoType) {
	int index = getInventoryIndex(client, weaponIndex);
	if (index != -1) {
		SetArrayCell(inventoryAmmo[client], index, ammo);
		SetArrayCell(inventoryAmmoType[client], index, ammoType);
	}
}

int getInventoryAmmo(int client, int weaponIndex) {
	int index = getInventoryIndex(client, weaponIndex);
	if (index != -1) {
		return GetArrayCell(inventoryAmmo[client], index);
	}
	return -1;
}

bool pickAmmoFromWeapon(int client, int weapon) {
	int gunIndex = getIndexEnt(weapon);
	if (gunIndex != -1) {
		int index = getInventoryIndex(client, gunIndex);
		if (index != -1) {
			bool usesClipsForAmmo1 = SDKCall(CALL_UsesClipsForAmmo1, weapon);
			
			int primaryCanGive = usesClipsForAmmo1 ? GetEntProp(weapon, Prop_Send, "m_iClip1") : GetEntProp(weapon, Prop_Data, "m_iPrimaryAmmoCount");
			
			int tookPrimary = SDKCall(CALL_GiveAmmo, client, primaryCanGive, GetEntProp(weapon, Prop_Send, "m_iPrimaryAmmoType"), false);
			
			if (tookPrimary > 0) {
				return true;
			}
		}
	}
	return false;
}

bool hasInInventory(int client, int weaponIndex){
	return getInventoryIndex(client, weaponIndex) != -1;
}

bool canSelectInInventory(int client, int weaponIndex){
	int invIndex = getInventoryIndex(client, weaponIndex);
	if(invIndex != -1 && canSelectItem(client, invIndex))
		return true;
	return false;
}

int getInventoryIndex(int client, int weaponIndex) {
	if(inventory[client]){
		int size = GetArraySize(inventory[client]);
		for (int i = 0; i < size; i++) {
			if (GetArrayCell(inventory[client], i) == weaponIndex) {
				return i;
			}
		}
	}
	return -1;
}

bool canSelectItem(int client, int inventoryIndex) {
	int invAmmo = GetArrayCell(inventoryAmmo[client], inventoryIndex);
	int invAmmoType = GetArrayCell(inventoryAmmoType[client], inventoryIndex);
	return invAmmo == -1
		|| invAmmo > 0 // got ammo (cached clips) in inventory
		|| invAmmoType == -1 // melee/other
		|| getType(GetArrayCell(inventory[client], inventoryIndex)) == GunType_Custom // custom-ammotype-guns always selectable
		|| SDKCall(CALL_GetAmmoCount, client, invAmmoType) > 0 // got ammo in stock
}

bool isCustom(int weaponEnt) {
	return getIndexEnt(weaponEnt) != -1;
}

int getIndexEnt(int weaponEnt) {
	if (weaponEnt == -1) return -1;
	char sWeapon[32];
	GetEdictClassname(weaponEnt, sWeapon, sizeof(sWeapon));
	return getIndex(sWeapon);
}

int getIndex(const char[] weapon) {
	return FindStringInArray(gunClassNames, weapon);
}

int getDefaultIndex() {
	char gun[32];
	GetConVarString(customguns_default, gun, sizeof(gun));
	return getIndex(gun);
}

bool canHaveGun(int client, int gunIndex, bool ignoreAdminLevel) {
	if (FindValueInArray(inventory[client], gunIndex) == -1) {
		int clientFlags = GetUserFlagBits(client);
		int gunFlags = GetArrayCell(gunAdminLevel, gunIndex);
		if (ignoreAdminLevel || gunFlags == -1 || clientFlags & ADMFLAG_ROOT || clientFlags & gunFlags) {
			return true;
		}
	}
	return false;
}

GunType getType (int index) {
	return GetArrayCell(gunType, index)
}

void getShootPosition(int client, float pos[3]){
	SDKCall(CALL_ShootPosition, client, pos);
}

public Action stopSound(Handle timer, any data) {
	if (IsValidEntity(data)) {
		SDKCall(CALL_StopWeaponSound, data, SINGLE);
	}
}

public Action deploySound(Handle timer, any data) {
	if (IsValidEntity(data)) {
		SDKCall(CALL_WeaponSound, data, DEPLOY, 0.0);
	}
}

public Action delayedKill(Handle timer, any ent) {
	if (IsValidEntity(ent)) {
		AcceptEntityInput(ent, "kill");
	}
}