#if defined _CustomGuns_Stocks_included
	#endinput
#endif
#define _CustomGuns_Stocks_included


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
	SetEntProp(gun, Prop_Send, "m_iClip1", clip<0? 0:clip);
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

/**
 * @return client's ammo count of type
 */
stock int getClientAmmoOfType(int client, int ammoType){
	if(MAX_AMMO_SLOTS < ammoType < 0){
		ThrowError("Ammo type out of range");
	}
	int ammoOffset = FindDataMapInfo(client, "m_iAmmo");
	int offset = ammoOffset + (ammoType * 4);
	return GetEntData(client, offset);
}

stock int getClientPrimaryAmmoForWeapon(int client, int weapon){
	return getClientAmmoOfType(client, GetEntProp(weapon, Prop_Data, "m_iPrimaryAmmoType"));
}

stock int getClientSecondaryAmmoForWeapon(int client, int weapon){
	return getClientAmmoOfType(client, GetEntProp(weapon, Prop_Data, "m_iSecondaryAmmoType"));
}

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

stock void itemPickup(int client){
	EmitGameSoundToAll("HL2Player.PickupWeapon", client);
	Handle hPickup = StartMessageOne("ItemPickup", client);
	BfWriteString(hPickup, "item_longjump");
	EndMessage();
}

stock void physExplosion(const float origin[3], float magnitude = 60.0, bool noDmg = false){
	//SF_NO_DAMAGE 1
	//SF_PUSH_PLAYERS 2
	//SF_PUSH_RADIALLY 4
	//SF_TEST_LOS 8
	//SF_DISORIENT_PLAYER 16
	int ent = CreateEntityByName("env_physexplosion");
	if(ent != -1){
		DispatchKeyValueFloat(ent, "magnitude", magnitude);
		DispatchKeyValue(ent, "radius", "0");
		DispatchKeyValue(ent, "inner_radius", "0");
		DispatchKeyValue(ent, "spawnflags", noDmg? "11" : "10");
		DispatchKeyValueVector(ent, "origin", origin);
		DispatchSpawn(ent);
		AcceptEntityInput(ent, "Explode");
		AcceptEntityInput(ent, "Kill");
	}
}

stock void CreatePointHurt(const float origin[3], float dmg, float dmgRadius, int dmgType){
	int ent = CreateEntityByName("point_hurt");
	if(ent != -1){
		DispatchKeyValueVector(ent, "origin", origin);
		DispatchKeyValueFloat(ent, "Damage", dmg);
		SetEntProp(ent, Prop_Data, "m_bitsDamageType", dmgType);
		DispatchSpawn(ent);
		SetEntPropFloat(ent, Prop_Data, "m_flRadius", dmgRadius);
		AcceptEntityInput(ent, "Hurt");
		AcceptEntityInput(ent, "Kill");
	}
}

#define FFADE_IN			0x0001		// Just here so we don't pass 0 into the function
#define FFADE_OUT			0x0002		// Fade out (not in)
#define FFADE_MODULATE		0x0004		// Modulate (don't blend)
#define FFADE_STAYOUT		0x0008		// ignores the duration, stays faded out until new ScreenFade message received
#define FFADE_PURGE			0x0010		// Purges all other fades, replacing them with this one

stock bool UTIL_ScreenFade(int client, int duration, int mode, int holdtime, int r=0, int g=0, int b=0, int a=255)
{
	Handle userMessage = StartMessageOne("Fade", client, USERMSG_RELIABLE);
	
	if (!userMessage) {
		return false;
	}

	BfWriteShort(userMessage,	duration);	// Fade duration
	BfWriteShort(userMessage,	holdtime);	// Fade hold time
	BfWriteShort(userMessage,	mode);		// What to do
	BfWriteByte(userMessage,	r);			// Color R
	BfWriteByte(userMessage,	g);			// Color G
	BfWriteByte(userMessage,	b);			// Color B
	BfWriteByte(userMessage,	a);			// Color Alpha

	EndMessage();
	
	return true;
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
/* 	TE_Start("Sprite Spray");
	TE_WriteVector("m_vecOrigin", origin);
	TE_WriteVector("m_vecDirection", vecDir);
	TE_WriteNum("m_nModelIndex", modelIndex);
	TE_WriteFloat("m_fNoise", life);
	TE_WriteNum("m_nSpeed", random);
	TE_WriteNum("m_nCount", count); */
}

stock TE_SetupBeamEntPoint(startEntity, endEntity, const Float:start[3], const Float:end[3], ModelIndex, HaloIndex, StartFrame, FrameRate, Float:Life, 
				Float:Width, Float:EndWidth, FadeLength, Float:Amplitude, const Color[4], Speed, flags)
{
	TE_Start("BeamEntPoint");
	TE_WriteNum("m_nModelIndex", ModelIndex);
	TE_WriteNum("m_nHaloIndex", HaloIndex);
	TE_WriteNum("m_nStartFrame", StartFrame);
	TE_WriteNum("m_nFrameRate", FrameRate);
	TE_WriteFloat("m_fLife", Life);
	TE_WriteFloat("m_fWidth", Width);
	TE_WriteFloat("m_fEndWidth", EndWidth);
	TE_WriteNum("m_nFadeLength", FadeLength);
	TE_WriteFloat("m_fAmplitude", Amplitude);
	TE_WriteNum("m_nSpeed", Speed);
	TE_WriteNum("r", Color[0]);
	TE_WriteNum("g", Color[1]);
	TE_WriteNum("b", Color[2]);
	TE_WriteNum("a", Color[3]);
	TE_WriteNum("m_nFlags", flags);
	TE_WriteEncodedEnt("m_nStartEntity", startEntity);
	TE_WriteEncodedEnt("m_nEndEntity", endEntity);
	TE_WriteVector("m_vecStartPoint", start);
	TE_WriteVector("m_vecEndPoint", end);
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

stock bool IsSolid(int entity){
	return SolidType_t:GetEntProp(entity, Prop_Data, "m_nSolidType", 1) != SOLID_NONE
	&& !(SolidFlags_t:GetEntProp(entity, Prop_Data, "m_usSolidFlags", 2) & FSOLID_NOT_SOLID == FSOLID_NOT_SOLID)
}

stock void SetMinMaxSize(int entity, const float vecMins[3], const float vecMaxs[3])
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

stock void VectorMin(const float vec1[3], const float vec2[3], float out[3]){
	out[0] = vec1[0] < vec2[0]? vec1[0] : vec2[0];
	out[1] = vec1[1] < vec2[1]? vec1[1] : vec2[1];
	out[2] = vec1[2] < vec2[2]? vec1[2] : vec2[2];
}

stock void VectorMax(const float vec1[3], const float vec2[3], float out[3]){
	out[0] = vec1[0] > vec2[0]? vec1[0] : vec2[0];
	out[1] = vec1[1] > vec2[1]? vec1[1] : vec2[1];
	out[2] = vec1[2] > vec2[2]? vec1[2] : vec2[2];
}

// Purpose : Creates both an decal and any associated impact effects (such
//			 as flecks) for the given iDamageType and the trace's end position
//
// Note: uses current global trace result
stock void UTIL_ImpactTrace(const float start[3], int iDamageType, const char[] pCustomImpactName = "Impact"){
	if(TR_GetEntityIndex() == -1 || TR_GetFraction() == 1.0){ //+check sky
		return;
	}
	float origin[3]; TR_GetEndPosition(origin);
	
	TE_SetupEffectDispatch(origin, start, NULL_VECTOR, NULL_VECTOR, 0, 0.0, 1.0, 0, 0,
		getEffectDispatchStringTableIndex(pCustomImpactName), 0, iDamageType, TR_GetHitGroup(), TR_GetEntityIndex(), 0, 0.0, false,
		NULL_VECTOR, NULL_VECTOR, false, 0, NULL_VECTOR);
	TE_SendToAll();
}

stock void UTIL_Tracer( const float vecStart[3], const float vecEnd[3], int iEntIndex, int iAttachment, float flVelocity, bool bWhiz, const char[] pCustomTracerName = "Tracer", int iParticleID = 0 )
{
	int flags, m_nAttachmentIndex;
	if(bWhiz)
	{
		flags |= TRACER_FLAG_WHIZ;
	}
	if ( iAttachment != TRACER_DONT_USE_ATTACHMENT )
	{
		flags |= TRACER_FLAG_USEATTACHMENT;
		m_nAttachmentIndex = iAttachment;
	}
	TE_SetupEffectDispatch(vecEnd, vecStart, NULL_VECTOR, NULL_VECTOR, flags, 0.0, flVelocity, m_nAttachmentIndex, 0,
		getEffectDispatchStringTableIndex(pCustomTracerName), 0, 0, iParticleID, iEntIndex, 0, 0.0, false,
		NULL_VECTOR, NULL_VECTOR, false, 0, NULL_VECTOR);
	TE_SendToAll();
}

//#define MAX_EFFECT_FLAG_BITS 8
//#define CUSTOM_COLOR_CP1		9
//#define CUSTOM_COLOR_CP2		10
stock TE_SetupEffectDispatch(const float origin[3], const float start[3], const float angles[3], const float normal[3],
	int flags, float magnitude, float scale, int attachmentIndex, int surfaceProp, int effectName, int material, int damageType,
	int hitbox, int entindex, int color, float radius, bool customColors, const float customColor1[3], const float customColor2[3],
	bool controlPoint1, int cp1ParticleAttachment, const float cp1Offset[3])
{
	TE_Start("EffectDispatch");
	TE_WriteFloat("m_vOrigin[0]", origin[0]);
	TE_WriteFloat("m_vOrigin[1]", origin[1]);
	TE_WriteFloat("m_vOrigin[2]", origin[2]);
	TE_WriteFloat("m_vStart[0]", start[0]);
	TE_WriteFloat("m_vStart[1]", start[1]);
	TE_WriteFloat("m_vStart[2]", start[2]);
	TE_WriteVector("m_vAngles", angles);
	TE_WriteVector("m_vNormal", normal);
	TE_WriteNum("m_fFlags", flags);
	TE_WriteFloat("m_flMagnitude", magnitude);
	TE_WriteFloat("m_flScale", scale);
	TE_WriteNum("m_nAttachmentIndex", attachmentIndex);
	TE_WriteNum("m_nSurfaceProp", surfaceProp);
	TE_WriteNum("m_iEffectName", effectName);
	TE_WriteNum("m_nMaterial", material);
	TE_WriteNum("m_nDamageType", damageType);
	TE_WriteNum("m_nHitBox", hitbox);
	TE_WriteNum("entindex", entindex);
	TE_WriteNum("m_nColor", color);
	TE_WriteFloat("m_flRadius", radius);
	TE_WriteNum("m_bCustomColors", customColors);
	TE_WriteVector("m_CustomColors.m_vecColor1", customColor1);
	TE_WriteVector("m_CustomColors.m_vecColor2", customColor2);
	TE_WriteNum("m_bControlPoint1", controlPoint1);
	TE_WriteNum("m_ControlPoint1.m_eParticleAttachment", cp1ParticleAttachment);
	TE_WriteFloat("m_ControlPoint1.m_vecOffset[0]", cp1Offset[0]);
	TE_WriteFloat("m_ControlPoint1.m_vecOffset[1]", cp1Offset[1]);
	TE_WriteFloat("m_ControlPoint1.m_vecOffset[2]", cp1Offset[2]);
}

stock int getEffectDispatchStringTableIndex(const char[] effectName){
	static int table = INVALID_STRING_TABLE;
	if(table == INVALID_STRING_TABLE){
		table = FindStringTable("EffectDispatch");
	}
	int index;
	if( (index = FindStringIndex(table, effectName)) != INVALID_STRING_INDEX)
		return index;
	AddToStringTable(table, effectName);
	return FindStringIndex(table, effectName)
}


/* float UTIL_Approach( float target, float value, float speed ){
	float delta = target - value;

	if ( delta > speed )
		value += speed;
	else if ( delta < -speed )
		value -= speed;
	else 
		value = target;

	return value;
}

float UTIL_AngleMod(float a){
	a = (360.0/65536) * (RoundToFloor(a*(65536.0/360.0)) & 65535);
	return a;
}
 */

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