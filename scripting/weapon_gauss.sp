#include <sourcemod>
#include <sdktools>
#include <sdkhooks>
#include <customguns>

#pragma newdecls required


#define CLASSNAME "weapon_gauss"

#define GAUSS_BEAM_SPRITE "sprites/laserbeam.vmt"

#define AMMO_COST_PRIMARY 1
#define COOLDOWN_PRIMARY 0.25
#define COOLDOWN_SECONDARY 0.5

#define SPREAD 0.00873 // -> VECTOR_CONE_1DEGREES
#define PLAYER_HIT_FORCE 10.0

#define CHARGELOOP_PITCH_START 50
#define CHARGELOOP_PITCH_END 250

#define GAUSS_CHARGE_TIME 0.3 // was 0.2
#define MAX_GAUSS_CHARGE_TIME 3.0
#define DANGER_GAUSS_CHARGE_TIME 10.0

ConVar sk_plr_dmg_gauss;
ConVar sk_plr_max_dmg_gauss;
ConVar sk_plr_push_scale_gauss;

int sprite;

float m_flNextChargeTime[MAXPLAYERS+1];
float m_flChargeTransitionTime[MAXPLAYERS+1];
float m_flChargeStartTime[MAXPLAYERS+1];
bool m_bCharging[MAXPLAYERS+1];
bool m_bChargeIndicated[MAXPLAYERS+1];

bool teamplay;

public void OnPluginStart(){
	sk_plr_dmg_gauss = CreateConVar("sk_plr_dmg_gauss", "30", "Sets the damage that an uncharged shot from the player will deal.");
	sk_plr_max_dmg_gauss = CreateConVar("sk_plr_max_dmg_gauss", "200", "Sets the damage a fully charged shot from the player can deal.");
	sk_plr_push_scale_gauss = CreateConVar("sk_plr_push_scale_gauss", "3.0", "Sets the scale of secondary attack recoil force.");
}

public void OnMapStart()
{
	teamplay = GetConVarBool(FindConVar("mp_teamplay"));
}

public void OnClientPutInServer(int client){
	resetVars(client);
}

void resetVars(int client){
	m_flNextChargeTime[client] = 0.0;
	m_flChargeTransitionTime[client] = 0.0;
	m_flChargeStartTime[client] = 0.0;
	m_bCharging[client] = false;
	m_bChargeIndicated[client] = false;
}

public void OnConfigsExecuted(){
	sprite = PrecacheModel(GAUSS_BEAM_SPRITE);
}

public void CG_OnPrimaryAttack(int client, int weapon){
	char sWeapon[32];
	GetEntityClassname(weapon, sWeapon, sizeof(sWeapon));
	
	if(StrEqual(sWeapon, CLASSNAME)){
		if(m_bCharging[client]){
			return;
		}
		//CG_SetPlayerAnimation(client, PLAYER_ATTACK1);
		CG_PlayPrimaryAttack(weapon);
		CG_SetNextPrimaryAttack(weapon, GetGameTime() + COOLDOWN_PRIMARY);
		CG_SetNextSecondaryAttack(weapon, GetGameTime() + COOLDOWN_SECONDARY);
		CG_RemovePlayerAmmo(client, weapon, AMMO_COST_PRIMARY);
		EmitGameSoundToAll("Weapon_Gauss.Single", weapon);
		
		PrimaryFire(client, weapon);
	}
}

public void CG_OnSecondaryAttack(int client, int weapon){
	char sWeapon[32];
	GetEntityClassname(weapon, sWeapon, sizeof(sWeapon));
	
	if(StrEqual(sWeapon, CLASSNAME)){
	
		if(getClientPrimaryAmmoForWeapon(client, weapon) <= 0){
			return;
		}

		if (!m_bCharging[client])
		{
			//Start looping animation
			m_flChargeTransitionTime[client] = GetGameTime() + CG_PlayActivity(weapon, ACT_VM_PULLBACK_LOW) - 0.1;
			m_flChargeStartTime[client] = GetGameTime();
			m_bCharging[client] = true;
			m_bChargeIndicated[client] = false;
		}

		IncreaseCharge(client, weapon);
	}
}

public void CG_ItemPostFrame(int client, int weapon){
	char sWeapon[32];
	GetEntityClassname(weapon, sWeapon, sizeof(sWeapon));
	
	if(StrEqual(sWeapon, CLASSNAME)){
		if(m_bCharging[client])
		{
			if (GetEntProp(client, Prop_Data, "m_afButtonReleased") & IN_ATTACK2)
			{
				ChargedFire(client, weapon);
			}
		}
	}
}

public void CG_OnHolster(int client, int weapon, int switchingTo){
	char sWeapon[32];
	GetEntityClassname(weapon, sWeapon, sizeof(sWeapon));
	
	if(StrEqual(sWeapon, CLASSNAME)){
		StopChargeSound(client, weapon);
		resetVars(client);
	}
}

void PrimaryFire(int client, int weapon){
	float angles[3], startPos[3], endPos[3], vecDir[3], traceNormal[3], vecFwd[3], vecUp[3], vecRight[3];
	CG_GetShootPosition(client, startPos);
	GetClientEyeAngles(client, angles);
	GetAngleVectors(angles, vecFwd, vecRight, vecUp);
	
	float x, y, z;
	//Gassian spread
	do {
		x = GetRandomFloat(-0.5,0.5) + GetRandomFloat(-0.5,0.5);
		y = GetRandomFloat(-0.5,0.5) + GetRandomFloat(-0.5,0.5);
		z = x*x+y*y;
	} while (z > 1);
 
 	vecDir[0] = vecFwd[0] + x * SPREAD * vecRight[0] + y * SPREAD * vecUp[0];
	vecDir[1] = vecFwd[1] + x * SPREAD * vecRight[1] + y * SPREAD * vecUp[1];
	vecDir[2] = vecFwd[2] + x * SPREAD * vecRight[2] + y * SPREAD * vecUp[2];
	
	GetVectorAngles(vecDir, angles);
	
	TR_TraceRayFilter(startPos, angles, MASK_SHOT, RayType_Infinite, TraceEntityFilter, client);
	TR_GetEndPosition(endPos);
	TR_GetPlaneNormal(null, traceNormal);
	int entityHit = TR_GetEntityIndex();

	physExplosion(endPos, 20.0, true);
	
	if(entityHit == 0) { // hit world
		
		DrawBeam( startPos, endPos, 1.6, weapon);

		TE_SetupGaussExplosion(endPos, 0, traceNormal);
		TE_SendToAll();
		
		CG_RadiusDamage(client, client, sk_plr_dmg_gauss.FloatValue, DMG_SHOCK, weapon, endPos, 30.0, client);
		UTIL_ImpactTrace(startPos, DMG_SHOCK, "ImpactGauss");
		
		float hitAngle = -GetVectorDotProduct(traceNormal, vecDir);
		if ( hitAngle < 0.5 )
		{
			float vReflection[3];
			vReflection[0] = 2.0 * traceNormal[0] * hitAngle + vecDir[0];
			vReflection[1] = 2.0 * traceNormal[1] * hitAngle + vecDir[1];
			vReflection[2] = 2.0 * traceNormal[2] * hitAngle + vecDir[2];
			GetVectorAngles(vReflection, angles);
			
			startPos = endPos;
			
			TR_TraceRayFilter(startPos, angles, MASK_SHOT, RayType_Infinite, TraceEntityFilter, client);
			TR_GetEndPosition(endPos);
			entityHit = TR_GetEntityIndex();
			
			if (entityHit > 0)
			{
 				if(IsPlayer(entityHit)){
					if(!teamplay || GetClientTeam(entityHit) != GetClientTeam(client)){
						float dmgForce[3];
						NormalizeVector(vReflection, dmgForce);
						ScaleVector(dmgForce, PLAYER_HIT_FORCE);
						SDKHooks_TakeDamage(entityHit, client, client, sk_plr_dmg_gauss.FloatValue, DMG_SHOCK, weapon, dmgForce, endPos);
					}
				} else {
					//float entityPos[3];
					//GetEntPropVector(entityHit, Prop_Send, "m_vecOrigin", entityPos);
					//CreatePointHurt(entityPos, sk_plr_dmg_gauss.FloatValue, 0.0, DMG_SHOCK|DMG_CRUSH|DMG_PREVENT_PHYSICS_FORCE);
					CG_RadiusDamage(client, client, sk_plr_dmg_gauss.FloatValue, DMG_SHOCK, weapon, endPos, 20.0, client);
				}
			}
			DrawBeam(startPos, endPos, 0.4);
		}
	}
	else if (entityHit != -1)
	{
		if(IsPlayer(entityHit)){
			if(!teamplay || GetClientTeam(entityHit) != GetClientTeam(client)){
				float dmgForce[3];
				NormalizeVector(vecDir, dmgForce);
				ScaleVector(dmgForce, PLAYER_HIT_FORCE);
				SDKHooks_TakeDamage(entityHit, client, client, sk_plr_dmg_gauss.FloatValue, DMG_SHOCK, weapon, dmgForce, endPos);
			}
		} else {
			//float entityPos[3];
			//GetEntPropVector(entityHit, Prop_Send, "m_vecOrigin", entityPos);
			//CreatePointHurt(entityPos, sk_plr_dmg_gauss.FloatValue, 0.0, DMG_SHOCK|DMG_CRUSH|DMG_PREVENT_PHYSICS_FORCE);
			CG_RadiusDamage(client, client, sk_plr_dmg_gauss.FloatValue, DMG_SHOCK, weapon, endPos, 30.0, client);
		}
		
		DrawBeam(startPos, endPos, 1.6, weapon);
		
		TE_SetupGaussExplosion(endPos, 0, traceNormal);
		TE_SendToAll();
		
		UTIL_ImpactTrace(startPos, DMG_SHOCK, "ImpactGauss");
	}
	
	float viewPunch[3];
	viewPunch[0] = GetRandomFloat( -0.5, -0.2 );
	viewPunch[1] = GetRandomFloat( -0.5,  0.5 );
	Tools_ViewPunch(client, viewPunch);

	// Register a muzzleflash for the AI
	SetEntPropFloat(client, Prop_Data, "m_flFlashTime", GetGameTime() + 0.5);
}

void ChargedFire(int client, int weapon){
	//bool penetrated = false;
 
	EmitGameSoundToAll("Weapon_Gauss.Single", weapon);
	
	//CG_SetPlayerAnimation(client, PLAYER_ATTACK1);
	CG_PlayActivity(weapon, ACT_VM_SECONDARYATTACK);
	StopChargeSound(client, weapon);

	m_bCharging[client] = false;
	m_bChargeIndicated[client] = false;

	float curtime = GetGameTime();
	CG_SetNextPrimaryAttack(weapon, curtime + COOLDOWN_PRIMARY);
	CG_SetNextSecondaryAttack(weapon, curtime + COOLDOWN_SECONDARY);

	//Shoot a shot straight out
	float angles[3], startPos[3], endPos[3], traceNormal[3], vecFwd[3];
	CG_GetShootPosition(client, startPos);
	GetClientEyeAngles(client, angles);
	GetAngleVectors(angles, vecFwd, NULL_VECTOR, NULL_VECTOR);
	
	TR_TraceRayFilter(startPos, angles, MASK_SHOT, RayType_Infinite, TraceEntityFilter, client);
	TR_GetEndPosition(endPos);
	TR_GetPlaneNormal(null, traceNormal);
	
	//Find how much damage to do
	float flChargeAmount = ( curtime - m_flChargeStartTime[client] ) / MAX_GAUSS_CHARGE_TIME;

	//Clamp this
	if ( flChargeAmount > 1.0 ){
		flChargeAmount = 1.0;
	}

	//Determine the damage amount
	float flDamage = sk_plr_dmg_gauss.FloatValue + ( ( sk_plr_max_dmg_gauss.FloatValue - sk_plr_dmg_gauss.FloatValue ) * flChargeAmount );

	//if ( entityHit == 0 )
	//{
	//	TE_SetupGaussExplosion(endPos, 0, traceNormal);
	//	TE_SendToAll();
	
		//Try wall penetration
 		//UTIL_ImpactTrace( &tr, GetAmmoDef()->DamageType(m_iPrimaryAmmoType), "ImpactGauss" );
		//UTIL_DecalTrace( &tr, "RedGlowFade" );

/* 		float testPos[3];
		testPos[0] = endPos[0] + vecFwd[0] * 48.0;
		testPos[1] = endPos[1] + vecFwd[1] * 48.0;
		testPos[2] = endPos[2] + vecFwd[2] * 48.0;
		TR_TraceRayFilter(testPos, endPos, MASK_SHOT, RayType_EndPoint, TraceEntityFilter, client); */
		
		/*UTIL_TraceLine( testPos, tr.endpos, MASK_SHOT, pOwner, COLLISION_GROUP_NONE, &tr );   
		if ( tr.allsolid == false ){
				UTIL_DecalTrace( &tr, "RedGlowFade" );
				penetrated = true;
		} */
		
		/* if(TR_DidHit() && TR_GetFraction() != 0.0){ //tr.allsolid == false (?)
			//UTIL_DecalTrace( &tr, "RedGlowFade" );
			penetrated = true;
		} */
	//}
	//else if ( entityHit != -1 )
	//{
		//Do direct damage to anything in our path
/* 		if(IsPlayer(entityHit)){
			float dmgForce[3];
			dmgForce = vecFwd;
			ScaleVector(dmgForce, PLAYER_HIT_FORCE);
			SDKHooks_TakeDamage(entityHit, client, client, flDamage, DMG_SHOCK, weapon, dmgForce, endPos);
			CG_RadiusDamage(client, client, 0.0, DMG_SHOCK, weapon, endPos, 50.0, -1, -1);
		} else {
			//float entityPos[3];
			//GetEntPropVector(entityHit, Prop_Send, "m_vecOrigin", entityPos);
			//CreatePointHurt(entityPos, flDamage, 0.0, DMG_SHOCK|DMG_CRUSH|DMG_PREVENT_PHYSICS_FORCE);
			CG_RadiusDamage(client, client, flDamage, DMG_SHOCK, weapon, endPos, 50.0, -1, -1);
		} */
	//	CG_RadiusDamage(client, client, flDamage, DMG_SHOCK, weapon, endPos, 200.0, -1, -1);
	//}

	//UTIL_ImpactTrace( &tr, GetAmmoDef()->DamageType(m_iPrimaryAmmoType), "ImpactGauss" );
	UTIL_ImpactTrace(startPos, DMG_SHOCK, "ImpactGauss");
	
	float viewPunch[3];
	viewPunch[0] = GetRandomFloat( -4.0, -8.0 );
	viewPunch[1] = GetRandomFloat( -0.25,  0.25 );
	Tools_ViewPunch(client, viewPunch);

	DrawBeam( startPos, endPos, 9.6, weapon );

	//Recoil push
	float scale = sk_plr_push_scale_gauss.FloatValue;
	ScaleVector(vecFwd, -(flDamage * scale));
	vecFwd[2] += 30.0 * scale;
	TeleportEntity(client, NULL_VECTOR, NULL_VECTOR, vecFwd);

	TE_SetupGaussExplosion(endPos, 0, traceNormal);
	TE_SendToAll();
	
	//RadiusDamage( CTakeDamageInfo( this, this, flDamage, DMG_SHOCK ), tr.endpos, 200.0f, CLASS_NONE, NULL );
	CG_RadiusDamage(client, client, flDamage, DMG_SHOCK, weapon, endPos, 200.0, client);

	// Register a muzzleflash for the AI
	SetEntPropFloat(client, Prop_Data, "m_flFlashTime", curtime + 0.5);
}

void IncreaseCharge(int client, int weapon)
{
	float curtime = GetGameTime();
	
	// Send charge-sound pitch updates to client
	
	float flChargeAmount = ( curtime - m_flChargeStartTime[client] ) / MAX_GAUSS_CHARGE_TIME;
	if ( flChargeAmount <= 1.0 ){
		int channel; int soundLevel; float volume; int oldpitch; char sample[PLATFORM_MAX_PATH];
		GetGameSoundParams("Weapon_Gauss.ChargeLoop", channel, soundLevel, volume, oldpitch, sample, sizeof(sample));
		int newPitch = CHARGELOOP_PITCH_START + RoundToFloor((CHARGELOOP_PITCH_END - CHARGELOOP_PITCH_START) * flChargeAmount);
		EmitSoundToAll(sample, weapon, channel, soundLevel, SND_CHANGEPITCH|SND_CHANGEVOL, volume, newPitch);
	}
	
	if(curtime >= m_flChargeTransitionTime[client]){
		CG_PlayActivity(weapon, ACT_VM_PULLBACK);
		m_flChargeTransitionTime[client] = FLT_IDKWHATSMAX;
	}
	
	if ( m_flNextChargeTime[client] > curtime )
		return;

	//Check our charge time
	if ( ( curtime - m_flChargeStartTime[client] ) > MAX_GAUSS_CHARGE_TIME )
	{
		//Notify the player they're at maximum charge
		if ( m_bChargeIndicated[client] == false )
		{
			EmitGameSoundToAll("Weapon_Gauss.Charged", weapon);
			m_bChargeIndicated[client] = true;
		}

		if ( ( curtime - m_flChargeStartTime[client] ) > DANGER_GAUSS_CHARGE_TIME )
		{
			//Damage the player
			EmitGameSoundToAll("Weapon_Gauss.OverCharged", weapon);
		   
			// Add DMG_CRUSH because we don't want any physics force
			SDKHooks_TakeDamage(client, weapon, weapon, 25.0, DMG_SHOCK | DMG_CRUSH);
			
			// Fade done by TakeDamage already
			//Client_ScreenFade(client, 200, FFADE_IN, 200, 255, 128, 0, 128);
			
			m_flNextChargeTime[client] = curtime + GetRandomFloat( 0.5, 2.5 );
		}
		
		return;
	}
	
	//Decrement power
	CG_RemovePlayerAmmo(client, weapon, 1);

	//Make sure we can draw power
	if ( getClientPrimaryAmmoForWeapon(client, weapon) <= 0 )
	{
		ChargedFire(client, weapon);
		return;
	}

	m_flNextChargeTime[client] = curtime + GAUSS_CHARGE_TIME;
}

void StopChargeSound(int client, int weapon){
	if(m_bCharging[client]){
		int channel; int soundLevel; float volume; int oldpitch; char sample[PLATFORM_MAX_PATH];
		GetGameSoundParams("Weapon_Gauss.ChargeLoop", channel, soundLevel, volume, oldpitch, sample, sizeof(sample));
		StopSound(weapon, channel, sample);
	}
}

public bool TraceEntityFilter(int entity, int mask, any data){
	if (entity == data)
		return false;
	return true;
}

void TE_SetupGaussExplosion(const float vecOrigin[3], int type, float direction[3]){	
 	TE_Start("GaussExplosion");
	TE_WriteFloat("m_vecOrigin[0]", vecOrigin[0]);
	TE_WriteFloat("m_vecOrigin[1]", vecOrigin[1]);
	TE_WriteFloat("m_vecOrigin[2]", vecOrigin[2]);
	TE_WriteNum("m_nType", type);
	TE_WriteVector("m_vecDirection", direction);
}

void DrawBeam(const float startPos[3], const float endPos[3], float width, int startEntity = -1){
	//UTIL_Tracer( startPos, endPos, 0, TRACER_DONT_USE_ATTACHMENT, 6500.0, false, "GaussTracer" );
	int beam = CreateEntityByName("beam");
	if(beam != -1){

		if(startEntity != -1){
			Beam_PointEntInit(beam, endPos, startEntity);
			SetEntPropFloat(beam, Prop_Data, "m_fWidth", width / 4.0);
			SetEntPropFloat(beam, Prop_Data, "m_fEndWidth", width);
		} else {
			Beam_PointPointInit(beam, startPos, endPos);
			SetEntPropFloat(beam, Prop_Data, "m_fWidth", width);
			SetEntPropFloat(beam, Prop_Data, "m_fEndWidth", width / 4.0);
		}
		
		SetEntityRenderColor(beam, 255, 145 +GetRandomInt(-16, 16), 0, 255);
		DispatchKeyValue(beam, "model", GAUSS_BEAM_SPRITE);
		SetEntProp(beam, Prop_Data, "m_nModelIndex", sprite);
			
		SetVariantString("OnUser1 !self:kill::0.1:-1")
		AcceptEntityInput(beam, "addoutput");
		AcceptEntityInput(beam, "FireUser1");
		
		DispatchSpawn(beam);
		ActivateEntity(beam);
	}
	
	//Draw electric bolts along shaft
	for ( int i = 0; i < 3; i++ )
	{
		beam = CreateEntityByName("beam");
		if(beam != -1){
			if(startEntity != -1){
				Beam_PointEntInit(beam, endPos, startEntity);
			} else {
				Beam_PointPointInit(beam, startPos, endPos);
			}
			
			SetEntPropFloat(beam, Prop_Data, "m_fAmplitude", 1.6 * i);
			
			SetEntPropFloat(beam, Prop_Data, "m_fWidth", width/2.0 + i);
			SetEntPropFloat(beam, Prop_Data, "m_fEndWidth", 0.1);
			
			SetEntityRenderColor(beam, 255, 255, 150 + GetRandomInt(0, 64));
			DispatchKeyValue(beam, "model", GAUSS_BEAM_SPRITE);
			SetEntProp(beam, Prop_Data, "m_nModelIndex", sprite);
				
			SetVariantString("OnUser1 !self:kill::0.1:-1")
			AcceptEntityInput(beam, "addoutput");
			AcceptEntityInput(beam, "FireUser1");
			
			DispatchSpawn(beam);
			ActivateEntity(beam);
		}
	}
}

//
// CBeam stuff
//
//

enum 
{
	BEAM_POINTS = 0,
	BEAM_ENTPOINT,
	BEAM_ENTS,
	BEAM_HOSE,
	BEAM_SPLINE,
	BEAM_LASER,
	NUM_BEAM_TYPES
};

void Beam_PointEntInit(int beam, const float start[3], int endEntity){
	SetEntProp(beam, Prop_Send, "m_nBeamType", BEAM_ENTPOINT);
	SetEntProp(beam, Prop_Send, "m_nNumBeamEnts", 2);
	SetEntPropVector(beam, Prop_Send, "m_vecOrigin", start);
	
	//SetEndEntity
	int offset = FindDataMapInfo(beam, "m_hAttachEntity");
	SetEntDataEnt2(beam, offset+4, endEntity);
	
	SetEntPropEnt(beam, Prop_Data, "m_hEndEntity", endEntity);
	
	//SetEndAttachment
	offset = FindDataMapInfo(beam, "m_nAttachIndex");
	SetEntData(beam, offset+4, 1);
} 

void Beam_PointPointInit(int beam, const float start[3], const float end[3]){
	SetEntProp(beam, Prop_Send, "m_nBeamType", BEAM_POINTS);
	SetEntProp(beam, Prop_Send, "m_nNumBeamEnts", 2);
	TeleportEntity(beam, start, NULL_VECTOR, NULL_VECTOR);
	SetEntPropVector(beam, Prop_Send, "m_vecEndPos", end);
}