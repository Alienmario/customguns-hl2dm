#if defined _CustomGuns_Throwable_included
	#endinput
#endif
#define _CustomGuns_Throwable_included

Handle gunThrowModel;
Handle gunThrowForce;
Handle gunThrowPullback;
Handle gunThrowForceAttack;
Handle gunThrowTrailType;
Handle gunThrowExplType;
Handle gunThrowRadius;
Handle gunThrowDamage;
Handle gunThrowTimer;
Handle gunThrowGravity;
Handle gunThrowFriction;
Handle gunThrowAngVelScale;
//Handle gunThrowLoopSound;
//Handle gunThrowDetSound;

Handle gunThrowMoloGibs;
Handle gunThrowMoloGibsLife;
Handle gunThrowMoloGibsBurnout;
Handle gunThrowMoloGibsVelocity;
Handle gunThrowMoloGibsMaxVariation;

void Throwable_OnPluginStart(){
	
	gunThrowModel = CreateArray(PLATFORM_MAX_PATH);
	gunThrowForce = CreateArray();
	gunThrowPullback = CreateArray();
	gunThrowForceAttack = CreateArray();
	gunThrowTrailType = CreateArray();
	gunThrowExplType = CreateArray();
	gunThrowRadius = CreateArray();
	gunThrowDamage = CreateArray();
	gunThrowTimer = CreateArray();
	gunThrowGravity = CreateArray();
	gunThrowFriction = CreateArray();
	gunThrowAngVelScale = CreateArray();
	//gunThrowLoopSound = CreateArray(128);
	//gunThrowDetSound = CreateArray(128);
	
	gunThrowMoloGibs = CreateArray();
	gunThrowMoloGibsLife = CreateArray();
	gunThrowMoloGibsBurnout = CreateArray();
	gunThrowMoloGibsVelocity = CreateArray();
	gunThrowMoloGibsMaxVariation = CreateArray();
}

void Throwable_OnMapStart(){
	PrecacheSound(MOLOTOV_EXPLODE, true);
	PrecacheSound(MOLOTOV_BURN_LOOP, true);
}

// Throwable weapon variables (copied from weapon_frag)
// Since player only has one custom weapon at a time, these can be assigned per-player (I hope)
int m_AttackPaused[MAXPLAYERS+1];
bool m_bDrawbackFinished [MAXPLAYERS+1];
bool m_bRedraw [MAXPLAYERS+1];

void throwableReset(int client){
	m_AttackPaused[client] = GRENADE_PAUSED_NO;
	m_bDrawbackFinished[client] = false;
	m_bRedraw[client] = false;
}

void decrementAmmo(int weapon, int client){
	SDKCall(CALL_RemoveAmmo, client, 1, GetEntProp(weapon, Prop_Send, "m_iPrimaryAmmoType"));
}

void reload(int weapon, int index, int client){

	// if limited ammo weapon and has no ammo, dont reload
	if(!GetArrayCell(gunThrowForceAttack, index) && !SDKCall(CALL_HasPrimaryAmmo, weapon)){
		return;
	}
	
	float curtime = GetGameTime();
	if(curtime >= GetEntPropFloat(weapon, Prop_Send, "m_flNextPrimaryAttack")){
		SDKCall(CALL_SendWeaponAnim, weapon, ACT_VM_DRAW);
		
		m_bRedraw[client] = false;
		
		float seqDuration = GetEntPropFloat(weapon, Prop_Send, "m_flTimeWeaponIdle") - curtime;
		SetEntPropFloat(weapon, Prop_Send, "m_flNextPrimaryAttack", curtime + seqDuration);
		SetEntPropFloat(weapon, Prop_Send, "m_flNextSecondaryAttack",  curtime + seqDuration);
		SetEntPropFloat(weapon, Prop_Send, "m_flTimeWeaponIdle",  curtime + seqDuration);
	}
}

bool supportsPullback(int index){
	return GetArrayCell(gunThrowPullback, index) > 0;
}

MRESReturn Throwable_Holster(int client){
	throwableReset(client);
	return MRES_Ignored;
}

MRESReturn Throwable_PrimaryAttack(int weapon, int index, int client) {
	if ( m_bRedraw[client] )
		return MRES_Supercede; 
			
	Call_StartForward(PrimaryAttackForward);
	Call_PushCell(client);
	Call_PushCell(weapon);
	Call_Finish();
		
	SetEntPropFloat(weapon, Prop_Send, "m_flTimeWeaponIdle", FLT_IDKWHATSMAX);
	SetEntPropFloat(weapon, Prop_Send, "m_flNextPrimaryAttack", FLT_IDKWHATSMAX);
	
	switch (GetArrayCell(gunThrowPullback, index)){
		case 1: {
			m_AttackPaused[client] = GRENADE_PAUSED_PRIMARY;
			SDKCall(CALL_SendWeaponAnim, weapon, ACT_VM_PULLBACK_HIGH);
		}
		case 2: {
			m_AttackPaused[client] = GRENADE_PAUSED_PRIMARY;
			SDKCall(CALL_SendWeaponAnim, weapon, ACT_VM_HAULBACK);
		}
		case -1: {
			SDKCall(CALL_SendWeaponAnim, weapon, ACT_VM_PRIMARYATTACK);
		}
		default: {
			SDKCall(CALL_SendWeaponAnim, weapon, ACT_VM_THROW);
		}
	}
/* 	if(supportsPullback(index)){

		
	} else {
		
	} */
/* 	if ( !SDKCall(CALL_HasPrimaryAmmo, weapon) )
	{
		//pPlayer->SwitchToNextBestWeapon( this );
	}	 */
	return MRES_Supercede;
}


MRESReturn Throwable_ItemPostFrame(int weapon, int index, int client) {
	if(supportsPullback(index)){
		if( m_bDrawbackFinished[client] )
		{
			switch( m_AttackPaused[client] )
			{
				case GRENADE_PAUSED_PRIMARY: {
					if( !(GetClientButtons(client) & IN_ATTACK) )
					{
						SDKCall(CALL_SendWeaponAnim, weapon, ACT_VM_THROW);
						m_bDrawbackFinished[client] = false;
					}
				}
			}
			/* 
				case GRENADE_PAUSED_SECONDARY:
					if( !(pOwner->m_nButtons & IN_ATTACK2) )
					{
						//See if we're ducking
						if ( pOwner->m_nButtons & IN_DUCK )
						{
							//Send the weapon animation
							SendWeaponAnim( ACT_VM_SECONDARYATTACK );
						}
						else
						{
							//Send the weapon animation
							SendWeaponAnim( ACT_VM_HAULBACK );
						}

						m_bDrawbackFinished = false;
					}
					break;

				default:
					break; */
		} else {
			if(GetArrayCell(gunThrowForceAttack, index))
			{
				if ( ( GetClientButtons(client) & IN_ATTACK ) && GetGameTime() > GetEntPropFloat(weapon, Prop_Send, "m_flNextPrimaryAttack") )
				{
					Throwable_PrimaryAttack(weapon, index, client);
				}
				else if ( ( GetClientButtons(client) & IN_ATTACK2 ) && GetGameTime() > GetEntPropFloat(weapon, Prop_Send, "m_flNextSecondaryAttack") )
				{
					SecondaryAttack(weapon);
				}
			}
		}
	}
	
/* 	if( !oldDrawbackFinished && GetArrayCell(gunThrowForceAttack, index))
	{
		if ( ( GetClientButtons(client) & IN_ATTACK ) && GetGameTime() > GetEntPropFloat(weapon, Prop_Send, "m_flNextPrimaryAttack") )
		{
			SDKCall(CALL_PrimaryAttack, weapon);
		}
		else if ( ( GetClientButtons(client) & IN_ATTACK2 ) && GetGameTime() > GetEntPropFloat(weapon, Prop_Send, "m_flNextSecondaryAttack") )
		{
			//SecondaryAttack();
		}
	} */
	
	if ( m_bRedraw[client] )
	{
		reload(weapon, index, client);
	}
	return MRES_Handled;
}

MRESReturn Throwable_Operator_HandleAnimEvent(int weapon, int client, int event) {
	bool bThrewGrenade;
	switch( event )
	{
		case EVENT_WEAPON_SEQUENCE_FINISHED: {
			m_bDrawbackFinished[client] = true;
		}
		case EVENT_WEAPON_THROW: {
			ThrowThrowable( client, weapon );
			bThrewGrenade = true;
		}
/* 		case EVENT_WEAPON_THROW2:
			RollGrenade( pOwner );
			DecrementAmmo( pOwner );
			fThrewGrenade = true;
			break;

		case EVENT_WEAPON_THROW3:
			LobGrenade( pOwner );
			DecrementAmmo( pOwner );
			fThrewGrenade = true;
			break;
 */
		default: {
			return MRES_Handled;
		}
	}
	
	if(bThrewGrenade){
		decrementAmmo(weapon, client);
		
		#define RETHROW_DELAY	0.5
		SetEntPropFloat(weapon, Prop_Send, "m_flNextPrimaryAttack", GetGameTime() + RETHROW_DELAY);
		SetEntPropFloat(weapon, Prop_Send, "m_flNextSecondaryAttack", GetGameTime() + RETHROW_DELAY);
		SetEntPropFloat(weapon, Prop_Send, "m_flTimeWeaponIdle", FLT_IDKWHATSMAX);
	}
	return MRES_Supercede;
}

void ThrowThrowable(int client, int weapon) {
	int index = getIndexEnt(weapon);
	char buffer[PLATFORM_MAX_PATH];
	
	float origin[3], eyeAngles[3], fwd[3];
	GetClientEyePosition(client, origin);
	GetClientEyeAngles(client, eyeAngles);
	GetAngleVectors(eyeAngles, fwd, NULL_VECTOR, NULL_VECTOR);
	ScaleVector(fwd, GetArrayCell(gunThrowForce, index));
	
	switch(GetArrayCell(gunThrowExplType, index)){
		case EXPL_BUGBAIT: {
			int ent = CreateEntityByName("npc_grenade_bugbait");
			if (ent != -1) {
				DispatchSpawn(ent);
				
				GetArrayString(gunThrowModel, index, buffer, sizeof(buffer));
				SetEntityModel(ent, buffer);
				
				SetEntPropEnt(ent, Prop_Send, "m_hOwnerEntity", client);
				SetEntPropEnt(ent, Prop_Send, "m_hThrower", client);
				SetEntProp(ent, Prop_Send, "m_iTeamNum", GetClientTeam(client));
				SetEntPropEnt(ent, Prop_Send, "m_hEffectEntity", weapon); // lets store the weapon here for now
				
				TeleportEntity(ent, origin, NULL_VECTOR, fwd);
				
				createTrail(ent, index, origin);
			}
		}
		default: {
			float timer = GetArrayCell(gunThrowTimer, index);
			bool explOnTouch = timer < 0.0;
			
			int ent = CreateEntityByName(explOnTouch? "npc_contactgrenade" : "npc_handgrenade");
			if (ent != -1) {
				DHookEntity(DHOOK_Explode, true, ent);
				
				if(!explOnTouch){
					SetEntPropFloat(ent, Prop_Data, "m_flDetonateTime", GetGameTime() + timer);
				}
				DispatchSpawn(ent);
				
				GetArrayString(gunThrowModel, index, buffer, sizeof(buffer));
				SetEntityModel(ent, buffer);
				SetMinMaxSize(ent, Float:{-6.0, -6.0, -2.0}, Float:{6.0, 6.0, 2.0});
				
				SetEntPropFloat(ent, Prop_Send, "m_flDamage", GetArrayCell(gunThrowDamage, index));
				SetEntPropFloat(ent, Prop_Send, "m_DmgRadius", GetArrayCell(gunThrowRadius, index));
				SetTakeDamage(ent, explOnTouch? DAMAGE_YES : DAMAGE_NO);
				SetEntProp(ent, Prop_Data, "m_iHealth", explOnTouch? 1 : 100);
				SetEntityGravity(ent, GetArrayCell(gunThrowGravity, index));
				SetEntPropFloat(ent, Prop_Data, "m_flFriction", GetArrayCell(gunThrowFriction, index));
				
				SetEntPropEnt(ent, Prop_Send, "m_hOwnerEntity", client);
				SetEntPropEnt(ent, Prop_Send, "m_hThrower", client);
				SetEntProp(ent, Prop_Send, "m_iTeamNum", GetClientTeam(client));
				SetEntPropEnt(ent, Prop_Send, "m_hEffectEntity", weapon); // lets store the weapon here for now

				TeleportEntity(ent, origin, NULL_VECTOR, fwd);
				
				float angVel[3];
				float velScale = GetArrayCell(gunThrowAngVelScale, index);
				angVel[0] = GetRandomFloat( -100.0*velScale, -500.0*velScale );
				angVel[1] = GetRandomFloat( -100.0*velScale, -500.0*velScale );
				angVel[2] = GetRandomFloat( -100.0*velScale, -500.0*velScale );
				SetEntPropVector(ent, Prop_Data, "m_vecAngVelocity", angVel);
				
				createTrail(ent, index, origin);
			}
		}
	}
	
	m_bRedraw[client] = true;
	SDKCall(CALL_WeaponSound, weapon, SINGLE, 0.0);
	SDKCall(CALL_SetAnimation, client, PLAYER_ATTACK1);
}

void createTrail(int throwable, int index, float origin[3]){
	int ent;
	switch(GetArrayCell(gunThrowTrailType, index)){
		case TRAIL_SMOKE: {
			ent = CreateEntityByName("env_smoketrail");
			if(ent == -1) return;
			
 			float startColor[3] = {0.2, 0.2, 0.2};
			float endColor[3] = {0.0, 0.0, 0.0};
			SetEntPropFloat(ent, Prop_Send, "m_SpawnRate", 48.0);
			SetEntPropFloat(ent, Prop_Send, "m_ParticleLifetime", 1.0);
			SetEntPropVector(ent, Prop_Send, "m_StartColor", startColor);
			SetEntPropVector(ent, Prop_Send, "m_EndColor", endColor);
			SetEntPropFloat(ent, Prop_Send, "m_StartSize", 8.0);
			SetEntPropFloat(ent, Prop_Send, "m_EndSize", 32.0);
			SetEntPropFloat(ent, Prop_Send, "m_SpawnRadius", 4.0);
			SetEntPropFloat(ent, Prop_Send, "m_MinSpeed", 8.0);
			SetEntPropFloat(ent, Prop_Send, "m_MaxSpeed", 16.0);
			SetEntPropFloat(ent, Prop_Send, "m_Opacity", 0.25);

			DispatchKeyValue(ent, "emittime", "10");
		}
		case TRAIL_SPORE: {
			ent = CreateEntityByName("env_sporeexplosion");
			if(ent == -1) return;
		}
		case TRAIL_FIRE: {
			ent = CreateEntityByName("env_fire_trail");
			if(ent == -1) return;
		}
		default: return;
	}
	ActivateEntity(ent);
	DispatchSpawn(ent);
	TeleportEntity(ent, origin, NULL_VECTOR, NULL_VECTOR);						
	SetVariantString("!activator");
	AcceptEntityInput(ent, "SetParent", throwable);
	AcceptEntityInput(ent, "Enable");
}
/* 
public void OnEntityDestroyed(int ent){
	int size = GetArraySize(grenades);
	for(int i = 0; i<size; i++){
		if(EntRefToEntIndex(GetArrayCell(grenades, i)) == ent){
			RemoveFromArray(grenades, i);
			throwableDestroyed(ent);
			return;
		}
	}
} */

// void CBaseGrenade::Explode( trace_t *pTrace, int bitsDamageType )
public MRESReturn Explode(int throwable, Handle hReturn, Handle hParams) {
	int weapon = GetEntPropEnt(throwable, Prop_Send, "m_hEffectEntity");
	if(IsValidEntity(weapon)){
		int index = getIndexEnt(weapon);
		if(index != -1) {
			int client = GetEntPropEnt(throwable, Prop_Send, "m_hOwnerEntity");
			
			// Pull a bit out of the wall
			float origin[3], originMin[3], originMax[3];
			float vecMins[3], vecMaxs[3], normal[3];
			GetEntPropVector(throwable, Prop_Data, "m_vecOrigin", origin);
			GetEntPropVector(throwable, Prop_Send, "m_vecMins", vecMins);
			GetEntPropVector(throwable, Prop_Send, "m_vecMaxs", vecMaxs);
			originMin = origin;
			originMax = origin;
			AddVectors(origin, vecMins, originMin);
			AddVectors(origin, vecMaxs, originMax);
			TR_TraceHullFilter(originMin, originMax, vecMins, vecMaxs, MASK_SOLID, TraceFilter, throwable);
			if(TR_DidHit()){
 				TR_GetPlaneNormal(INVALID_HANDLE, normal);
				ScaleVector(normal, 20.0);
				AddVectors(origin, normal, origin);
			}
			TR_TraceHullFilter(originMax, originMin, vecMins, vecMaxs, MASK_SOLID, TraceFilter, throwable);
			if(TR_DidHit()){
 				TR_GetPlaneNormal(INVALID_HANDLE, normal);
				ScaleVector(normal, 20.0);
				AddVectors(origin, normal, origin);
			}

			switch(GetArrayCell(gunThrowExplType, index)){
				case EXPL_NONE: {
					//AcceptEntityInput(throwable, "kill");
					//return MRES_Supercede;
				}
				case EXPL_MOLOTOV: {
					createMolotovEffect(index, client, origin);
				}
			}
		}
	}
	return MRES_Ignored;
}

public bool TraceFilter(entity, mask, any data) {
	if(entity==data)
		return false;
	return true;
}

void createMolotovEffect(int index, int client, float origin[3]){
	static int thrown = 0;
	int particles = CreateEntityByName("info_particle_system");
	if(particles != -1) {
		DispatchKeyValue(particles, "effect_name", "env_fire_small_coverage_smoke");
		
		int gibs = GetArrayCell(gunThrowMoloGibs, index);
		float life = GetArrayCell(gunThrowMoloGibsLife, index);
		float burnout = GetArrayCell(gunThrowMoloGibsBurnout, index);
		float velocity = GetArrayCell(gunThrowMoloGibsVelocity, index);
		float variation = GetArrayCell(gunThrowMoloGibsMaxVariation, index);
		
		for (new i = 0; i < gibs; i++){
			int gib = CreateEntityByName("prop_physics");
			if(gib != -1) {
				char glass[64], name[32], cpoint[9];
				Format(name, sizeof(name), "customguns_throw_molo_gib%d", ++thrown);
				Format(glass, sizeof(glass), "models/props_junk/garbage_glassbottle003a_chunk0%d.mdl", GetRandomInt(1, 3));
				Format(cpoint, sizeof(cpoint), "cpoint%d", i+1);
				DispatchKeyValue(particles, cpoint, name);

				float direction[3];
				direction[0] = ((GetURandomFloat()-0.5)* 2 * velocity) * (GetURandomFloat() * variation);
				direction[1] = ((GetURandomFloat()-0.5)* 2 * velocity) * (GetURandomFloat() * variation);
				direction[2] = ((GetURandomFloat()-0.5)* 2 * velocity) * (GetURandomFloat() * variation);
 				
				float angVel[3];
				angVel[0] = GetRandomFloat( -100.0, -600.0 );
				angVel[1] = GetRandomFloat( -100.0, -600.0 );
				angVel[2] = GetRandomFloat( -100.0, -600.0 );
				
				DispatchKeyValue(gib, "targetname", name);
 				DispatchKeyValue(gib, "model", glass);

				DispatchSpawn(gib);
				disablePickup(gib);
				
				// Make this non solid to player but still trigger touch
				
				SetEntPropEnt(gib, Prop_Data, "m_hOwnerEntity", client);
				SetEntProp(gib, Prop_Send, "m_usSolidFlags", FSOLID_TRIGGER);
				SetEntProp(gib, Prop_Data, "m_CollisionGroup", COLLISION_GROUP_DEBRIS_TRIGGER);
				
				// Give it better radius for burning things
				
				SetEntProp(gib, Prop_Send, "m_nSolidType", SOLID_BBOX, 1);
				SetMinMaxSize(gib, Float:{-50.0, -50.0, -50.0}, Float:{50.0, 50.0, 50.0});
				
				TeleportEntity(gib, origin, NULL_VECTOR, direction);
				SetEntPropVector(gib, Prop_Data, "m_vecAbsVelocity", Float:{0.0,0.0,0.0});
				SetEntPropVector(gib, Prop_Data, "m_vecAngVelocity", angVel);
				SDKHook(gib, SDKHook_TouchPost, BurnTouch);
				CreateTimer(life, makeDisappear, EntIndexToEntRef(gib), TIMER_FLAG_NO_MAPCHANGE);
				CreateTimer(burnout+1.0, stopBurnTouch, EntIndexToEntRef(gib), TIMER_FLAG_NO_MAPCHANGE);
			}
		}

		DispatchSpawn(particles);
		ActivateEntity(particles);
		TeleportEntity(particles, origin, NULL_VECTOR, NULL_VECTOR);
		
		int entref = EntIndexToEntRef(particles);
		CreateTimer(0.2, startFire, entref, TIMER_FLAG_NO_MAPCHANGE);
		CreateTimer(burnout+0.2, stopBurningSound, entref, TIMER_FLAG_NO_MAPCHANGE);
		CreateTimer(burnout+0.2, delayedKill, entref, TIMER_FLAG_NO_MAPCHANGE);
	}
}

public BurnTouch (int ent, int other){
	int owner = GetEntPropEnt(ent, Prop_Data, "m_hOwnerEntity");
 	if(IsPlayer(other)){
		if(IsPlayer(owner) && GetClientTeam(owner) == GetClientTeam(other)){
			return;
		}
		SDKHooks_TakeDamage(other, ent, owner, MOLOTOV_BURN_DMG_PL, DMG_BURN);
	} else {
		SDKHooks_TakeDamage(other, ent, owner, MOLOTOV_BURN_DMG, DMG_BURN);
	}
}

public Action startFire(Handle timer, any entref){
	if(IsValidEntity(entref)){
		EmitSoundToAll(MOLOTOV_EXPLODE, entref, _, SNDLEVEL_SCREAMING);
		EmitSoundToAll(MOLOTOV_BURN_LOOP, entref, SNDCHAN_BODY, SNDLEVEL_TRAFFIC);
		
		AcceptEntityInput(entref, "start");
	}
}

public Action stopBurningSound(Handle timer, any entref){
	if(IsValidEntity(entref)){
		EmitSoundToAll("common/null.wav", entref, SNDCHAN_BODY, 0);
	}
}
public Action makeDisappear(Handle timer, any entref){
	if(IsValidEntity(entref)){
		SetEntityRenderFx(entref, RENDERFX_FADE_FAST);
		CreateTimer(2.0, delayedKill, entref);
	}
}
public Action stopBurnTouch(Handle timer, any entref){
	if(IsValidEntity(entref)){
		SDKUnhook(entref, SDKHook_TouchPost, BurnTouch);
	}
}