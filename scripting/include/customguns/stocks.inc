#if defined _CustomGuns_Stocks_included
	#endinput
#endif
#define _CustomGuns_Stocks_included

stock int getClientInProximity(int client, float radius, bool sameTeam = false, bool onlyWounded = false){
	int closest = -1;
	float minDistance = radius;
	for(int i=1; i<=MaxClients; i++){
		if(i!=client && IsClientInGame(i) && IsPlayerAlive(i) && IsClientFacing(client, i, 50.0)){
			if(sameTeam && GetClientTeam(client)!=GetClientTeam(i)){
				continue;
			}
			if(onlyWounded && GetClientHealth(i)>=GetEntProp(i, Prop_Data, "m_iMaxHealth")){
				continue;
			}
			float distance = getClientDistance(client, i);
			if(distance < minDistance){
				minDistance = distance;
				closest = i;
			}
		}
	}
	return closest;
}

stock float getClientDistance(int client1, int client2){
	float clientOrigin1[3], clientOrigin2[3];
	GetClientAbsOrigin(client1, clientOrigin1);
	GetClientAbsOrigin(client2, clientOrigin2);
	return GetVectorDistance(clientOrigin1, clientOrigin2);
}

stock bool IsClientFacing (int client, int client2, float maxAngle=50.0){
	float clientOrigin[3], entOrigin[3];
	float eyeAngles[3], directAngles[3];
	
	GetClientEyePosition(client, clientOrigin);
	GetClientEyePosition(client2, entOrigin);
	
	// Get the vector from player to the entity
	MakeVectorFromPoints(clientOrigin, entOrigin, directAngles); 
	
	GetVectorAngles(directAngles, directAngles);
	
	GetClientEyeAngles(client, eyeAngles);

	if(GetDifferenceBetweenAngles(eyeAngles, directAngles)>maxAngle){
		return false;
	}
	return true;
}

stock Float:GetDifferenceBetweenAngles(float fA[3], float fB[3])
{
    float fFwdA[3]; GetAngleVectors(fA, fFwdA, NULL_VECTOR, NULL_VECTOR);
    float fFwdB[3]; GetAngleVectors(fB, fFwdB, NULL_VECTOR, NULL_VECTOR);
    return RadToDeg(ArcCosine(fFwdA[0] * fFwdB[0] + fFwdA[1] * fFwdB[1] + fFwdA[2] * fFwdB[2]));
}

/* weapon means: set only if this weapon is active */
stock void setWeaponAmmo(int client, int clip, char[] weapon = ""){
	int gun = GetEntPropEnt(client, Prop_Data, "m_hActiveWeapon");
	if(gun==-1) return;
	if(!StrEqual(weapon, "")){
		char activeWeapon[32];
		GetEdictClassname(gun, activeWeapon, sizeof(activeWeapon)); 
		if(!StrEqual(weapon, activeWeapon, false)){
			return;
		}
	}
	SetEntProp(gun, Prop_Send, "m_iClip1", clip);
}

/* weapon means: add only if this weapon is active */
stock void addWeaponAmmo(int client, int clip, char[] weapon = ""){
	int gun = GetEntPropEnt(client, Prop_Data, "m_hActiveWeapon");
	if(gun==-1) return;
	if(!StrEqual(weapon, "")){
		char activeWeapon[32];
		GetEdictClassname(gun, activeWeapon, sizeof(activeWeapon)); 
		if(!StrEqual(weapon, activeWeapon, false)){
			return;
		}
	}
	SetEntProp(gun, Prop_Send, "m_iClip1", GetEntProp(gun, Prop_send, "m_iClip1")+clip);
}

stock int getWeaponAmmo(int client){
	return GetEntProp(GetEntPropEnt(client, Prop_Data, "m_hActiveWeapon"), Prop_send, "m_iClip1");
}

/* Plays a sequence on player's viewmodel */
stock void vmSeq(int client, int sequence, float duration){

	#define EF_PARITY_BITS	3
	#define EF_PARITY_MASK  ((1<<EF_PARITY_BITS)-1) 

 	int vm = GetEntPropEnt(client, Prop_Send, "m_hViewModel");
	SetEntProp(vm, Prop_Send, "m_nSequence", sequence);
	//SetEntProp(vm, Prop_Send, "m_nAnimationParity", (GetEntProp(vm, Prop_Send, "m_nAnimationParity")+1)  & ( (1<<EF_PARITY_BITS) - 1 ));
	SetEntPropFloat(vm, Prop_Data, "m_flCycle", 0.0);
	SetEntPropFloat(vm, Prop_Data, "m_flAnimTime", GetGameTime());
	
	SetEntPropFloat(GetEntPropEnt(client, Prop_Data, "m_hActiveWeapon"), Prop_Data, "m_flTimeWeaponIdle", GetGameTime() + duration);
}

stock void setViewmodelVisible(int client, bool visible){
	setVisible ( GetEntPropEnt(client, Prop_Send, "m_hViewModel"), visible );
}

stock void setVisible(int ent, bool visible){
	#define EF_NODRAW 32
	int effects = GetEntProp(ent, Prop_Send, "m_fEffects"); 
	if(!visible) {
		effects |= EF_NODRAW; 
	} else	{
		effects &= ~EF_NODRAW;
	}
	SetEntProp(ent, Prop_Send, "m_fEffects", effects); 
}

stock void setCrosshairVisible(int client, bool visible){
	#define	HIDEHUD_CROSSHAIR			( 1<<8 )
	
	int hudflags = GetEntProp(client, Prop_Send, "m_iHideHUD");
	if(!visible) {
		hudflags |= HIDEHUD_CROSSHAIR;
	} else {
		hudflags &= ~HIDEHUD_CROSSHAIR;
	}
	SetEntProp(client, Prop_Send, "m_iHideHUD", hudflags);
}

stock void fov(int client, int to, float time, int from = -1){
	SetEntProp(client, Prop_Send, "m_iFOVStart", from<1? GetEntProp(client, Prop_Send, "m_iFOV"):from);
	SetEntPropFloat(client, Prop_Send, "m_flFOVTime", GetGameTime());
	SetEntProp(client, Prop_Send, "m_iFOV", to<1? GetEntProp(client, Prop_Send, "m_iDefaultFOV"):to);
	SetEntPropFloat(client, Prop_Send, "m_flFOVRate", time);
}

stock void itemPickup(int client/*,  char[] itemname */){
	EmitGameSoundToAll("HL2Player.PickupWeapon", client);
	///Handle hPickup = StartMessageOne("ItemPickup", client);
	//BfWriteString(hPickup, itemname);
	//EndMessage();
}

//
// Recreation of CHL2MPMachineGun::DoMachineGunKick( CBasePlayer *pPlayer, float dampEasy, float maxVerticleKickAngle, float fireDurationTime, float slideLimitTime )
// https://github.com/ValveSoftware/source-sdk-2013/blob/55ed12f8d1eb6887d348be03aee5573d44177ffb/mp/src/game/shared/hl2mp/weapon_hl2mpbase_machinegun.cpp#L135
//
stock void Tools_AddViewKick(int client, float scale, float maxVerticleKickAngle, float fireDurationTime, float slideLimitTime){	
	#define	KICK_MIN_X			0.2		//Degrees
	#define	KICK_MIN_Y			0.2		//Degrees
	#define	KICK_MIN_Z			0.1		//Degrees

	//Find how far into our accuracy degradation we are
	float vecScratch[3];
	
	float duration	= ( fireDurationTime > slideLimitTime ) ? slideLimitTime : fireDurationTime;
	float kickPerc = duration / slideLimitTime;
	
	// do this to get a hard discontinuity, clear out anything under 10 degrees punch
	//Tools_ViewPunchReset(client, 10.0);
	
	//Apply this to the view angles as well
	vecScratch[0] = -( KICK_MIN_X + ( maxVerticleKickAngle * kickPerc ) );
	vecScratch[1] = -( KICK_MIN_Y + ( maxVerticleKickAngle * kickPerc ) ) / 3;
	vecScratch[2] = KICK_MIN_Z + ( maxVerticleKickAngle * kickPerc ) / 8;

	//Wibble left and right
	if (GetURandomInt()%2)
		vecScratch[1] *= -1;
	
	//Wobble up and down
	if (GetURandomInt()%2)
		vecScratch[2] *= -1;
	
	//Clip this to our desired min/max
	float m_vecPunchAngle[3]; GetEntPropVector(client, Prop_Send, "m_vecPunchAngle", m_vecPunchAngle);
	UTIL_ClipPunchAngleOffset( vecScratch, m_vecPunchAngle, Float:{24.0, 3.0, 1.0} );
	
	//Add it to the view punch
	// NOTE: 0.5 is just tuned to match the old effect before the punch became simulated
	ScaleVector( vecScratch, 0.5 * scale);
	Tools_ViewPunch (client, vecScratch );
}

//  CBasePlayer::ViewPunchReset( float tolerance )
stock void Tools_ViewPunchReset(int client, float tolerance){
	if ( tolerance != 0 )
	{
		tolerance *= tolerance;	// square
		float m_vecPunchAngleVel[3]; GetEntPropVector(client, Prop_Send, "m_vecPunchAngleVel", m_vecPunchAngleVel);
		float m_vecPunchAngle[3]; GetEntPropVector(client, Prop_Send, "m_vecPunchAngle", m_vecPunchAngle);
		float check = GetVectorLength(m_vecPunchAngleVel, true) + GetVectorLength(m_vecPunchAngleVel, true);
		if ( check > tolerance )
			return;
	}
	SetEntPropVector(client, Prop_Send, "m_vecPunchAngle", Float:{0.0, 0.0, 0.0});
	SetEntPropVector(client, Prop_Send, "m_vecPunchAngleVel", Float:{0.0, 0.0, 0.0});
}

//void UTIL_ClipPunchAngleOffset( QAngle &in, const QAngle &punch, const QAngle &clip )
stock void UTIL_ClipPunchAngleOffset( float inAngle[3], const float punch[3], const float clip[3] )
{
	float final[3]; AddVectors( inAngle, punch, final );

	//Clip each component
	for ( int i = 0; i < 3; i++ )
	{
		if ( final[i] > clip[i] )
		{
			final[i] = clip[i];
		}
		else if ( final[i] < -clip[i] )
		{
			final[i] = -clip[i];
		}

		//Return the result
		inAngle[i] = final[i] - punch[i];
	}
}

//	void CBasePlayer::ViewPunch( const QAngle &angleOffset )
stock void Tools_ViewPunch(int client, float angleOffset[3]){
	float m_vecPunchAngleVel[3]; GetEntPropVector(client, Prop_Send, "m_vecPunchAngleVel", m_vecPunchAngleVel);
	m_vecPunchAngleVel[0] += angleOffset[0] * 20;
	m_vecPunchAngleVel[1] += angleOffset[1] * 20;
	m_vecPunchAngleVel[2] += angleOffset[2] * 20;
	SetEntPropVector(client, Prop_Send, "m_vecPunchAngleVel", m_vecPunchAngleVel);
}

stock void TE_SetupShotgunShot(float[3] origin, float[3] vecDir, int ammoID, int seed, int shots, int player, float spread, bool doImpacts = true, bool doTracers = true){
	TE_Start("Shotgun Shot");
	TE_WriteVector("m_vecOrigin", origin);
	TE_WriteVector("m_vecDir", vecDir);
	TE_WriteNum("m_iAmmoID", ammoID);
	TE_WriteNum("m_iSeed", seed);
	TE_WriteNum("m_iShots", shots);
	TE_WriteNum("m_iPlayer", player);
	TE_WriteFloat("m_flSpread", spread);
	TE_WriteNum("m_bDoImpacts", doImpacts);
	TE_WriteNum("m_bDoTracers", doTracers);
}

stock void TE_SetupSpriteSpray(float[3] origin, float[3] vecDir, float[3] size, int modelIndex, int random, int count, float life, int flags){
	TE_Start("breakmodel");
	TE_WriteVector("m_vecOrigin", origin);
	TE_WriteFloat("m_angRotation[0]", 0.0);
	TE_WriteFloat("m_angRotation[1]", 0.0);
	TE_WriteFloat("m_angRotation[2]", 0.0);
	TE_WriteVector("m_vecSize", size);
	TE_WriteVector("m_vecVelocity", vecDir);
	TE_WriteNum("m_nModelIndex", modelIndex);
	TE_WriteNum("m_nRandomization", random);
	TE_WriteNum("m_nCount", count);
	TE_WriteFloat("m_fTime", life);
	TE_WriteNum("m_nFlags", flags);
}

stock TE_SetupBeamEntPoints(startEntity, endEntity, const Float:start[3], const Float:end[3], ModelIndex, HaloIndex, StartFrame, FrameRate, Float:Life, 
				Float:Width, Float:EndWidth, FadeLength, Float:Amplitude, const Color[4], Speed, flags)
{
	TE_Start("BeamEntPoint");
	TE_WriteEncodedEnt("m_nStartEntity", startEntity);
	TE_WriteEncodedEnt("m_nEndEntity", endEntity);
	TE_WriteVector("m_vecStartPoint", start);
	TE_WriteVector("m_vecEndPoint", end);
	TE_WriteNum("m_nModelIndex", ModelIndex);
	TE_WriteNum("m_nHaloIndex", HaloIndex);
	TE_WriteNum("m_nStartFrame", StartFrame);
	TE_WriteNum("m_nFrameRate", FrameRate);
	TE_WriteFloat("m_fLife", Life);
	TE_WriteFloat("m_fWidth", Width);
	TE_WriteFloat("m_fEndWidth", EndWidth);
	TE_WriteFloat("m_fAmplitude", Amplitude);
	TE_WriteNum("r", Color[0]);
	TE_WriteNum("g", Color[1]);
	TE_WriteNum("b", Color[2]);
	TE_WriteNum("a", Color[3]);
	TE_WriteNum("m_nSpeed", Speed);
	TE_WriteNum("m_nFadeLength", FadeLength);
	TE_WriteNum("m_nFlags", flags);
}

stock TE_SetupBeamEnts(startEntity, endEntity, ModelIndex, HaloIndex, StartFrame, FrameRate, Float:Life, 
				Float:Width, Float:EndWidth, FadeLength, Float:Amplitude, const Color[4], Speed, flags)
{
	TE_Start("BeamEnts");
	TE_WriteEncodedEnt("m_nStartEntity", startEntity);
	TE_WriteEncodedEnt("m_nEndEntity", endEntity);
	TE_WriteNum("m_nModelIndex", ModelIndex);
	TE_WriteNum("m_nHaloIndex", HaloIndex);
	TE_WriteNum("m_nStartFrame", StartFrame);
	TE_WriteNum("m_nFrameRate", FrameRate);
	TE_WriteFloat("m_fLife", Life);
	TE_WriteFloat("m_fWidth", Width);
	TE_WriteFloat("m_fEndWidth", EndWidth);
	TE_WriteFloat("m_fAmplitude", Amplitude);
	TE_WriteNum("r", Color[0]);
	TE_WriteNum("g", Color[1]);
	TE_WriteNum("b", Color[2]);
	TE_WriteNum("a", Color[3]);
	TE_WriteNum("m_nSpeed", Speed);
	TE_WriteNum("m_nFadeLength", FadeLength);
	TE_WriteNum("m_nFlags", flags);
}

stock bool IsPlayer(int number){
	return MaxClients>=number>0
}

stock int GetOwner(int ent){
	return GetEntPropEnt(ent, Prop_Data, "m_hOwner")
}

#define		DAMAGE_NO			0
#define 	DAMAGE_EVENTS_ONLY	1		// Call damage functions, but don't modify health
#define		DAMAGE_YES			2
#define		DAMAGE_AIM			3
stock void SetTakeDamage(entity, value)
{
	SetEntProp(entity, Prop_Data, "m_takedamage", value, 1);
}

stock void disablePickup(int entity){
	#define	EFL_NO_PHYSCANNON_INTERACTION 1<<30 // Physcannon can't pick these up or punt them
	
	SetEntProp(entity, Prop_Data, "m_iEFlags", GetEntProp(entity, Prop_Data, "m_iEFlags")|EFL_NO_PHYSCANNON_INTERACTION);
}

stock void SetMinMaxSize(int entity, float vecMins[3], float vecMaxs[3])
{
	for (int i=0; i<3; i++) {

		if (vecMins[i] > vecMaxs[i]) {
			ThrowError("Error: mins[%d] > maxs[%d] of entity %d", i, i, EntRefToEntIndex(entity));
		}
	}
	
	float m_vecMins[3], m_vecMaxs[3];
	GetEntPropVector(entity, Prop_Send, "m_vecMins", m_vecMins);
	GetEntPropVector(entity, Prop_Send, "m_vecMaxs", m_vecMaxs);

	if(GetVectorDistance(m_vecMins, vecMins, true) == 0.0 && GetVectorDistance(m_vecMaxs, vecMaxs, true) == 0.0){
		return;
	}

	SetEntPropVector(entity, Prop_Send, "m_vecMins", vecMins);
	SetEntPropVector(entity, Prop_Send, "m_vecMaxs", vecMaxs);
	
	float vecSize[3];
	SubtractVectors(vecMaxs, vecMins, vecSize);
	SetEntPropFloat(entity, Prop_Data, "m_flRadius", GetVectorLength(vecSize) * 0.5);
	
	MarkSurrBoundsDirty(entity);
}

stock void MarkSurrBoundsDirty(int entity){
	#define EFL_DIRTY_SURR_COLLISION_BOUNDS 1<<14
	
	SetEntProp(entity, Prop_Data, "m_iEFlags", GetEntProp(entity, Prop_Data, "m_iEFlags")|EFL_DIRTY_SURR_COLLISION_BOUNDS);
}

stock void swapVM(int client, bool predicted){
	int oldVM = GetEntPropEnt(client, Prop_Send, "m_hViewModel");
	char newClassname[32];
	newClassname = predicted? "predicted_viewmodel" : "viewmodel";
	
	if(oldVM != -1){
		char sOld[32];	GetEntityClassname(oldVM, sOld, sizeof(sOld));
		if(StrEqual(sOld, newClassname)){
			return;
		}
		AcceptEntityInput(oldVM, "kill");
	}
	
	int vm = CreateEntityByName(newClassname);
	
	float vec[3]; GetClientAbsOrigin(client, vec);
	TeleportEntity(vm, vec, NULL_VECTOR, NULL_VECTOR);
	
	SetEntPropEnt(vm, Prop_Send, "m_hOwner", client);
	SetEntProp(vm, Prop_Send, "m_nViewModelIndex", 0);
	DispatchSpawn(vm);
	
	SetEntPropEnt(client, Prop_Send, "m_hViewModel", vm);
	//ChangeEdictState(client, FindSendPropInfo("CHL2MP_Player", "m_hViewModel"));
}


/* This breaks thirdperson models and last weapon switching */
/* 
stock void removeWeaponFromSlot(int client, int weapon){
	int offset = FindDataMapOffs(client, "m_hMyWeapons") -4;
	for (new i=0; i < MAX_WEAPONS; i++) {
		offset += 4;

		if(GetEntDataEnt2(client, offset) == weapon){
			SetEntDataEnt2(client, offset, -1, true);
		}
	}
}
*/

/* 
#define VM_INDEX 0
void createVM(int client){

	int vm = CreateEntityByName("viewmodel");
	
	float vec[3]; GetClientAbsOrigin(client, vec);
	TeleportEntity(vm, vec, NULL_VECTOR, NULL_VECTOR);
	
	SetEntPropEnt(vm, Prop_Data, "m_hOwner", client);
	SetEntProp(vm, Prop_Data, "m_nViewModelIndex", VM_INDEX);
	DispatchSpawn(vm);
	
	//int offs = FindSendPropInfo("CHL2MP_Player", "m_hViewModel");
	int offset = FindDataMapOffs(client, "m_hViewModel");
	SetEntDataEnt2(client, offset+4, vm, true);
	//SetEntPropEnt(client, Prop_Send, "m_hViewModel", vm);
}
*/

/*
void followEntity(int ent, int parent){
	SetVariantString("!activator");
	AcceptEntityInput(ent, "setparent", parent);
	SetEntityMoveType(ent, MOVETYPE_NONE);
	
	int solidFlags = GetEntProp(ent, Prop_Data, "m_usSolidFlags", 2);
	SetEntProp(ent, Prop_Data, "m_usSolidFlags", solidFlags|0x0004); //FSOLID_NOT_SOLID
	TeleportEntity(ent, Float:{0.0,0.0,0.0}, Float:{0.0,0.0,0.0}, NULL_VECTOR);
}
*/

 
/* 	
	int offset_ammo = FindDataMapOffs(client, "m_iAmmo");
	for(int i=0; i<MAX_AMMO_SLOTS; i++){
		int offset = offset_ammo + (i * 4);
		SetEntData(client, offset, 50, 4, true);
	}
*/
	
/*
	TE_Start("Sprite Spray");
	TE_WriteVector("m_vecOrigin", gunPos);
	TE_WriteVector("m_vecDirection", Float:{0.0,0.0,0.0});
	TE_WriteNum("m_nModelIndex", GetArrayCell(gunModelIndexes, GetArrayCell(inventory[client], i)));
	TE_WriteFloat("m_fNoise", 500.0);
	TE_WriteNum("m_nSpeed", 200);
	TE_WriteNum("m_nCount", 2);
	TE_SendToClient(client);
*/