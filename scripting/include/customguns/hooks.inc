#if defined _CustomGuns_Hooks_included
	#endinput
#endif
#define _CustomGuns_Hooks_included

public Action OnWeaponSwitch(client, weapon)
{
	if (isCustom(weapon))
	{
		swapVM(client, false);
		CreateTimer(0.1, vmSwapDelay, EntIndexToEntRef(weapon), TIMER_FLAG_NO_MAPCHANGE);
	}
	else {
		swapVM(client, true);
	}

	return Plugin_Continue;
}

public OnWeaponSwitchPost(client, weapon)
{
	if (isCustom(weapon))
	{
		throwableReset(client);
		gunEnt[client] = EntIndexToEntRef(weapon);
	}
	else {
		gunEnt[client] = -1;
	}

	// this is here because equipPost fires after switchPost, but we may need to give binded weapon now!
	char sWeapon[32];
	GetEdictClassname(weapon, sWeapon, sizeof(sWeapon));
	int size = GetArraySize(gunClassNames);
	if (GetConVarBool(customguns_autogive))
	{
		for (int i = 0; i < size; i++)
		{
			char masterWeapon[32];
			GetArrayString(gunGiveMasterWeapon, i, masterWeapon, sizeof(masterWeapon));

			if (StrEqual(masterWeapon, sWeapon))
			{
				addToInventoryIndex(client, i);
			}
		}
	}

	// equip binded weapon
	for (int i = 0; i < size; i++)
	{
		char bindWeapon[32];
		GetArrayString(gunBind, i, bindWeapon, sizeof(bindWeapon));

		if (StrEqual(sWeapon, bindWeapon))
		{
			if (hasInInventory(client, i))
			{
				int lastWeapon = GetEntPropEnt(client, Prop_Data, "m_hLastWeapon");

				giveCustomGun(client, i, true);

				SetEntPropEnt(client, Prop_Data, "m_hLastWeapon", lastWeapon);
				ChangeEdictState(client, FindDataMapInfo(client, "m_hLastWeapon"));
			}
		}
	}
}

public OnWeaponEquipPost(client, weapon)
{
	if (GetConVarBool(customguns_autogive))
	{
		char sWeapon[32];
		GetEdictClassname(weapon, sWeapon, sizeof(sWeapon));
		int size = GetArraySize(gunClassNames);
		for (int i = 0; i < size; i++)
		{
			char masterWeapon[32];
			GetArrayString(gunGiveMasterWeapon, i, masterWeapon, sizeof(masterWeapon));

			if (StrEqual(masterWeapon, sWeapon))
			{
				addToInventoryIndex(client, i);
			}
		}
	}
}

public Action vmSwapDelay(Handle timer, any data)
{
	if (IsValidEntity(data))
		SDKCall(CALL_SendWeaponAnim, data, ACT_VM_DRAW);
	return Plugin_Handled;
}

// int CBaseCombatWeapon::GetDefaultClip1( void )
// Kind of a hack: If we're giving a weapon to player, override its default ammo with our saved ammo from inventory if any. Otherwise, do nothing.
public MRESReturn GetDefaultClip1(int weapon, Handle hReturn)
{
	if (IsPlayer(CLIENT_BEING_EQUIPPED))
	{
		int gunIndex = getIndexEnt(weapon);
		int ammo = getInventoryAmmo(CLIENT_BEING_EQUIPPED, gunIndex);
		if (ammo != -1)
		{
			DHookSetReturn(hReturn, ammo);
			return MRES_Supercede;
		}
	}
	return MRES_Ignored;
}

// float CBaseCombatWeapon::GetFireRate( void )
public MRESReturn GetFireRate(int weapon, Handle hReturn)
{
	DHookSetReturn(hReturn, GetArrayCell(gunRof, getIndexEnt(weapon)));
	return MRES_Supercede;
}

// void CBaseCombatWeapon::AddViewKick( void )
public MRESReturn AddViewKick(int weapon)
{
	int client = GetOwner(weapon);
	if (IsPlayer(client))
	{
		int index = getIndexEnt(weapon);
		float maxTime = GetArrayCell(gunViewKickTime, index);

		Tools_AddViewKick(client,
		                  GetArrayCell(gunViewKickScale, index),
		                  GetArrayCell(gunViewKickAngle, index),
		                  maxTime < 0.0 ? 1.0 : GetEntPropFloat(weapon, Prop_Data, "m_fFireDuration"),
		                  maxTime < 0.0 ? 1.0 : maxTime);
	}
	return MRES_Ignored;
}

// void CHL2MP_Player::FireBullets ( const FireBulletsInfo_t &info )
/* "FireBulletsInfo_t":
    0 int m_iShots;
    4 Vector m_vecSrc;
    16 Vector m_vecDirShooting;
    28 Vector m_vecSpread;
    40 float m_flDistance;
    44 int m_iAmmoType;
    48 int m_iTracerFreq;
    52 float m_flDamage;
    56 int m_iPlayerDamage;
    60 int m_nFlags;
    64 float m_flDamageForceScale;
    68 CBaseEntity *m_pAttacker;
    72 CBaseEntity *m_pAdditionalIgnoreEnt;
    76 bool m_bPrimaryAttack;
*/
public MRESReturn FireBullets(int client, Handle hParams)
{
	// check if client is firing custom gun
	if (selectedGunIndex[client] != -1 && gunEnt[client] != -1)
	{
		int damage = GetArrayCell(gunDmg, selectedGunIndex[client]);
		DHookSetParamObjectPtrVar(hParams, 1, 52, ObjectValueType_Float, float(damage));
		DHookSetParamObjectPtrVar(hParams, 1, 56, ObjectValueType_Int, damage);

		float spread = GetArrayCell(gunSpread, selectedGunIndex[client]);
		DHookSetParamObjectPtrVar(hParams, 1, 28, ObjectValueType_Float, spread);
		DHookSetParamObjectPtrVar(hParams, 1, 32, ObjectValueType_Float, spread);
		DHookSetParamObjectPtrVar(hParams, 1, 36, ObjectValueType_Float, spread);

		float origin[3];
		DHookGetParamObjectPtrVarVector(hParams, 1, 4, ObjectValueType_Vector, origin);
		float vecDir[3];
		DHookGetParamObjectPtrVarVector(hParams, 1, 16, ObjectValueType_Vector, vecDir);
		int ammoID = DHookGetParamObjectPtrVar(hParams, 1, 44, ObjectValueType_Int);
		int shots = DHookGetParamObjectPtrVar(hParams, 1, 0, ObjectValueType_Int);

		TE_SetupShotgunShot(origin, vecDir, ammoID, GetURandomInt() & 255, shots, client, spread, true, GetArrayCell(gunFireVisible, selectedGunIndex[client]));
		TE_SendToClient(client);
	}
	return MRES_Ignored;
}

// Activity CBaseCombatCharacter::Weapon_TranslateActivity( Activity baseAct, bool *pRequired )
// This is a fix to allow player model animations/poses for unknown guns
public MRESReturn TranslateActivity(int client, Handle hReturn, Handle hParams)
{
	if (gunEnt[client] != -1 && selectedGunIndex[client] != -1)
	{
		char animPrefix[32];
		GetArrayString(gunAnimPrefix, selectedGunIndex[client], animPrefix, sizeof(animPrefix));
		Activity resolved = resolveAnimFor(animPrefix, DHookGetParam(hParams, 1));
		if (resolved != ACT_INVALID)
		{
			DHookSetReturn(hReturn, resolved);
			return MRES_Supercede;
		}
	}
	return MRES_Ignored;
}

/*
struct animevent_t
{
    int				event;
    const char		*options;
    float			cycle;
    float			eventtime;
    int				type;
    CBaseAnimating	*pSource;
};
*/
// void CBaseCombatWeapon::Operator_HandleAnimEvent( animevent_t *pEvent, CBaseCombatCharacter *pOperator )
public MRESReturn Operator_HandleAnimEvent(int weapon, Handle hParams)
{
	int client = DHookGetParam(hParams, 2);
	if (IsPlayer(client))
	{
		int event = DHookGetParamObjectPtrVar(hParams, 1, 0, ObjectValueType_Int);
		return Throwable_Operator_HandleAnimEvent(weapon, client, event);
	}
	return MRES_Ignored;
}

// When player touches a weapon
// bool CBasePlayer::BumpWeapon( CBaseCombatWeapon *pWeapon )
public MRESReturn BumpWeapon(int client, Handle hReturn, Handle hParams)
{
	int gun = DHookGetParam(hParams, 1);
	int gunIndex = getIndexEnt(gun);

	// Is it custom?
	if (gunIndex != -1)
	{
		// Can this be added to player's inventory?
		if (addToInventoryIndex(client, gunIndex))
		{
			// Player put the gun in his inventory

			SDKCall(CALL_CheckRespawn, gun);

			AcceptEntityInput(gun, "kill");
			DHookSetReturn(hReturn, 1);
			return MRES_Supercede;
		}
		else if (pickAmmoFromWeapon(client, gun)) {
			// Grabbed the ammo instead
			// CHL2MP_Player behavior : If we have room for the ammo, then "take" the weapon too.

			SDKCall(CALL_CheckRespawn, gun);

			AcceptEntityInput(gun, "kill");
			DHookSetReturn(hReturn, 1);
			return MRES_Supercede;

			/*
			// CBasePlayer behavior
			// check for ammo left

			if(SDKCall(CALL_HasPrimaryAmmo, gun)){
			    // still has some, leave it lying
			    DHookSetReturn(hReturn, 0);
			    return MRES_Supercede;
			}

			// weapon extenguished, remove it completely
			AcceptEntityInput(gun, "kill");
			DHookSetReturn(hReturn, 1);
			return MRES_Supercede;

			*/
		}

		DHookSetReturn(hReturn, 0);
		return MRES_Supercede;
	}

	return MRES_Ignored;
}

// bool CBaseCombatWeapon::ReloadOrSwitchWeapons( void )
// This is where we give new weapon because we run out of ammo
public MRESReturn ReloadOrSwitchWeapons(int weapon, Handle hReturn)
{
	int owner = GetOwner(weapon);
	int prim = GetEntProp(weapon, Prop_Send, "m_iPrimaryAmmoType");
	int sec = GetEntProp(weapon, Prop_Send, "m_iSecondaryAmmoType");

	if (prim == -1 && sec == -1)
	{
		// is melee/other weapon
		return MRES_Ignored;
	}

	int index = getIndexEnt(weapon);
	updateWeaponInventoryAmmo(weapon, index, owner);

	int buttons = GetClientButtons(owner);
	if (/* GetArrayCell(gunAmmoTypes, selectedGunIndex[owner]) == -1 && */ !(buttons & IN_ATTACK) && !(buttons & IN_ATTACK2)
	    && !(SDKCall(CALL_HasSecondaryAmmo, weapon) || SDKCall(CALL_HasPrimaryAmmo, weapon)))
	{
		// first get rid of bound weapon if any
		char classname[32];
		GetArrayString(gunBind, index, classname, sizeof(classname));

		if (!StrEqual(classname, ""))
		{
			int offset = FindDataMapInfo(owner, "m_hMyWeapons") - 4;
			for (new i = 0; i < MAX_WEAPONS; i++)
			{
				offset += 4;

				int weapon2 = GetEntDataEnt2(owner, offset);
				if (weapon2 != -1)
				{
					char classname2[32];
					GetEdictClassname(weapon2, classname2, sizeof(classname2));
					if (StrEqual(classname, classname2))
					{
						SetEntProp(weapon2, Prop_Send, "m_iClip1", 0);
						SetEntProp(weapon2, Prop_Send, "m_iClip2", 0);
					}
				}
			}
		}

		// give new weapon
		giveCustomGun(owner, selectBestIndex(owner));
	}
	return MRES_Ignored;
}

// bool CBaseCombatWeapon::Reload( void )
public MRESReturn Reload(int weapon, Handle hReturn)
{
	if (DHookGetReturn(hReturn))
	{
		// fix reload sound not playing
		SDKCall(CALL_WeaponSound, weapon, RELOAD, 0.0);

		int client = GetOwner(weapon);
		if (IsPlayer(client))
		{
			lastFired[client] = 0.0;
		}
	}
	return MRES_Ignored;
}

// void CBaseCombatWeapon::ItemPostFrame( void )
// Handles spinup anim (fire delay) on appropriate guns by delaying next attack
public MRESReturn ItemPostFrame(int weapon)
{
	int client = GetOwner(weapon);
	if (IsPlayer(client))
	{
		int index = getIndexEnt(weapon);
		GunType type = getType(index);

		Call_StartForward(ItemPostFrameForward);
		Call_PushCell(client);
		Call_PushCell(weapon);
		Call_Finish();

		if (type == GunType_Throwable)
		{
			return Throwable_ItemPostFrame(weapon, index, client);
		}
		else if (type == GunType_Bullet)
		{
			int buttons = GetClientButtons(client);
			float curtime = GetGameTime();

			float delay = GetArrayCell(gunDelay, index);
			if (delay > 0.0)
			{
				static bool inSpinUp[MAXPLAYERS + 1];

				float delayFireCooldown = GetArrayCell(gunDelayFireCooldown, index);
				int delaySequence = GetArrayCell(gunDelaySequence, index);

				if (buttons & IN_ATTACK
				    && curtime >= lastFired[client] + delayFireCooldown
				    && GetEntPropFloat(weapon, Prop_Data, "m_fFireDuration") <= 0.0
				    && SDKCall(CALL_HasPrimaryAmmo, weapon))
				{
					if (delaySequence != -1) SDKCall(CALL_SendViewModelAnim, weapon, delaySequence);
					SetEntPropFloat(weapon, Prop_Send, "m_flNextPrimaryAttack", curtime + delay);
					SDKCall(CALL_WeaponSound, weapon, SPECIAL1, 0.0);
					inSpinUp[client] = true;
				}
				else if (inSpinUp[client])
				{
					if (!(buttons & IN_ATTACK) || GetEntPropFloat(weapon, Prop_Data, "m_fFireDuration") <= 0.0)
					{  // spinup cancelled by player (unpressed button)
						SDKCall(CALL_StopWeaponSound, weapon, SPECIAL1);
						SDKCall(CALL_SendWeaponAnim, weapon, ACT_VM_IDLE);
						inSpinUp[client] = false;
					}
					else if (curtime >= GetEntPropFloat(weapon, Prop_Data, "m_flNextPrimaryAttack")) {  // spinup transitioning to firing
						inSpinUp[client] = false;
					}
				}
			}

			// semi-auto handling
			if (GetArrayCell(gunFireType, index) == 1)
			{
				if (!(buttons & IN_ATTACK))
				{
					SetEntPropFloat(weapon, Prop_Send, "m_flNextPrimaryAttack", curtime - 0.1);
				}
			}
		}
		// Custom weapon
		else if (!GetArrayCell(gunCustomKeepAmmo, index)) {
			if ((GetClientButtons(client) & IN_ATTACK) && GetGameTime() > GetEntPropFloat(weapon, Prop_Send, "m_flNextPrimaryAttack"))
			{
				Call_StartForward(PrimaryAttackForward);
				Call_PushCell(client);
				Call_PushCell(weapon);
				Call_Finish();
			}
			else if ((GetClientButtons(client) & IN_ATTACK2) && GetGameTime() > GetEntPropFloat(weapon, Prop_Send, "m_flNextSecondaryAttack"))
			{
				Call_StartForward(SecondaryAttackForward);
				Call_PushCell(client);
				Call_PushCell(weapon);
				Call_Finish();
			}
		}
	}
	return MRES_Ignored;
}

// void CBaseCombatWeapon::ItemPostFrame( void )
// Loop sound handler
public MRESReturn ItemPostFramePost(int weapon)
{
	int client = GetOwner(weapon);
	if (IsPlayer(client))
	{
		if (firing[client] && GetEntPropFloat(weapon, Prop_Data, "m_fFireDuration") == 0.0)
		{
			CreateTimer(0.0, stopSound, EntIndexToEntRef(weapon));
			firing[client] = false;
			nextFireSound[client] = 0.0;
		}
	}
	return MRES_Ignored;
}

// void CBaseCombatWeapon::PrimaryAttack( void )
public MRESReturn PrimaryAttack(int weapon)
{
	int client = GetOwner(weapon);
	if (IsPlayer(client))
	{
		int index = getIndexEnt(weapon);
		GunType type = getType(index);

		if (type == GunType_Throwable)
		{
			return Throwable_PrimaryAttack(weapon, index, client);
		}
		else if (type == GunType_Bullet) {
			if (GetArrayCell(gunFireLoopFix, index))
			{
				firing[client] = true;
			}
			lastFired[client] = GetGameTime();

			if (GetArrayCell(gunFireType, index) == 1)
			{
				// semi-auto handling
				SetEntPropFloat(weapon, Prop_Send, "m_flNextPrimaryAttack", FLT_IDKWHATSMAX);
			}
		}

		Call_StartForward(PrimaryAttackForward);
		Call_PushCell(client);
		Call_PushCell(weapon);
		Call_Finish();

		if (type == GunType_Custom)
		{
			return MRES_Supercede;
		}
	}
	return MRES_Ignored;
}

// void CBaseCombatWeapon::SecondaryAttack( void )
public MRESReturn SecondaryAttack(int weapon)
{
	int client = GetOwner(weapon);
	if (IsPlayer(client))
	{
		Call_StartForward(SecondaryAttackForward);
		Call_PushCell(client);
		Call_PushCell(weapon);
		Call_Finish();

		int index = getIndexEnt(weapon);
		GunType type = getType(index);

		if (type == GunType_Custom)
		{
			return MRES_Supercede;
		}
	}
	return MRES_Ignored;
}

// void CBaseCombatWeapon::Drop( const Vector &vecVelocity )
public MRESReturn Drop(int weapon)
{
	int client = GetOwner(weapon);
	if (IsPlayer(client))
	{
		clearInventory(client);
	}
	return MRES_Ignored;
}

// bool CBaseCombatWeapon::Holster( CBaseCombatWeapon *pSwitchingTo )
public MRESReturn Holster(int weapon, Handle hReturn, Handle hParams)
{
	// this makes minigun types play spinup animation when switched to while holding fire button. The game doesn't reset this itself for some reason..
	SetEntPropFloat(weapon, Prop_Data, "m_fFireDuration", 0.0);

	int client = GetOwner(weapon);
	if (IsPlayer(client))
	{
		int index = getIndexEnt(weapon);
		GunType type = getType(index);

		updateWeaponInventoryAmmo(weapon, index, client);

		if (type == GunType_Throwable)
		{
			return Throwable_Holster(client);
		}
		else if (type == GunType_Bullet)
		{
			if (firing[client] && GetArrayCell(gunFireLoopFix, index))
			{
				CreateTimer(0.0, stopSound, EntIndexToEntRef(weapon));
				firing[client] = false;
				nextFireSound[client] = 0.0;
			}
		}

		Call_StartForward(HolsterForward);
		Call_PushCell(client);
		Call_PushCell(weapon);
		Call_PushCell(DHookIsNullParam(hParams, 1) ? -1 : DHookGetParam(hParams, 1));
		Call_Finish();
	}

	return MRES_Ignored;
}

// void CBaseCombatWeapon:WeaponSound( WeaponSound_t sound_type, float soundtime = 0.0f )
public MRESReturn WeaponSound(int weapon, Handle hParams)
{
	int client = GetOwner(weapon);
	if (IsPlayer(client) && selectedGunIndex[client] != -1)
	{
		if (firing[client] && DHookGetParam(hParams, 1) == SINGLE)
		{
			if (GetGameTime() >= nextFireSound[client])
			{
				float rof = GetArrayCell(gunRof, selectedGunIndex[client]);
				float length = GetArrayCell(gunFireLoopLength, selectedGunIndex[client]);
				nextFireSound[client] = GetGameTime() + length - 2 * rof;
				return MRES_Ignored;
			}
			return MRES_Supercede;
		}
	}
	return MRES_Ignored;
}