#include <sourcemod>
#include <sdktools>
#include <sdkhooks>
#include <customguns>

#define FIRE_FORCE 4500.0
#define MASS_SCALE 8.0 // more mass = more damage
#define DELETE_AFTER 15.0

#define SECONDARY_ATTACK_LENGTH 1.035644

#define MODEL "models/projectiles/serioussam/cannonball.mdl"
//#define MODEL "models/props_junk/watermelon01.mdl"

#define SOUND_FIRE "weapons/serioussam/cannon/fire.wav"
#define SOUND_PREPARE "weapons/serioussam/cannon/prepare.wav"


float startSecondary[MAXPLAYERS+1];

public OnConfigsExecuted(){
	PrecacheModel(MODEL, true);
	PrecacheSound(SOUND_FIRE, true);
	PrecacheSound(SOUND_PREPARE, true);
}

public void CG_OnPrimaryAttack(int client, int weapon){
	char cls[32];
	GetEntityClassname(weapon, cls, sizeof(cls));
	
	if(StrEqual(cls, "weapon_cannon")){
		fireCannon(client, 0.25);
		CG_PlayPrimaryAttack(weapon);
	}
}

public void CG_OnSecondaryAttack(int client, int weapon){
	char cls[32];
	GetEntityClassname(weapon, cls, sizeof(cls));
	
	if(StrEqual(cls, "weapon_cannon")){
		startSecondary[client] = GetGameTime();
		EmitSoundToAll(SOUND_PREPARE, client, SNDCHAN_WEAPON);
		
		CG_PlayActivity(weapon, ACT_VM_PRIMARYATTACK_1);
		
		// delay attacks indefinitely until this attack has finished!
		CG_SetNextPrimaryAttack(weapon, FLT_IDKWHATSMAX);
		CG_SetNextSecondaryAttack(weapon, FLT_IDKWHATSMAX);
	}
}

public Action OnPlayerRunCmd(client, &buttons){
	if(startSecondary[client] > 0.0){
		char cls[32];
		GetClientWeapon(client, cls, sizeof(cls));
		if(StrEqual(cls, "weapon_cannon")){
			int weapon = GetEntPropEnt(client, Prop_Data, "m_hActiveWeapon");
			
			if(!(buttons & IN_ATTACK2))
			{
				// launch early
				float force = (GetGameTime() - startSecondary[client]) / SECONDARY_ATTACK_LENGTH;
				if(force > 1.0) force = 1.0;
				if(force < 0.25) force = 0.25;
				
				fireCannon(client, force);
			}
			else if (GetGameTime() - startSecondary[client] > SECONDARY_ATTACK_LENGTH)
			{
				// launch at full force
				fireCannon(client, 1.0);
				buttons &= ~IN_ATTACK2;
				buttons &= ~IN_ATTACK;
			} else return;
			
			/* Return to idle and reset everything! */
			CG_PlayActivity(weapon, ACT_VM_IDLE);
			CG_Cooldown(weapon, 1.3);
			startSecondary[client] = 0.0;
		} else {
			startSecondary[client] = 0.0;
			StopSound(client, SNDCHAN_WEAPON, SOUND_PREPARE);
		}
	}
}

stock void fireCannon(int client, float forceScale = 1.0){
	CG_SetPlayerAnimation(client, PLAYER_ATTACK1);
	EmitSoundToAll(SOUND_FIRE, client, SNDCHAN_WEAPON);
	
	int ent = CreateEntityByName("prop_physics_override");
	SetEntityModel(ent, MODEL);
	DispatchKeyValue(ent, "Damagetype", "1");
	DispatchKeyValue(ent, "ExplodeDamage", "50");
	DispatchKeyValue(ent, "ExplodeRadius", "300");
	DispatchKeyValueFloat(ent, "massScale", MASS_SCALE);
	DispatchSpawn(ent);

	SetEntPropEnt(ent, Prop_Data, "m_hOwnerEntity", client);
	
	CreateTimer(DELETE_AFTER, explode, EntIndexToEntRef(ent), TIMER_FLAG_NO_MAPCHANGE);
	
	float fwd[3], force[3], pos[3], ang[3];
	GetClientEyePosition(client, pos);
	GetClientEyeAngles(client, ang);
	GetAngleVectors(ang, fwd, NULL_VECTOR, NULL_VECTOR);
	ScaleVector(fwd, FIRE_FORCE * forceScale);
	force = fwd;
	
	GetAngleVectors(ang, fwd, NULL_VECTOR, NULL_VECTOR);
	ScaleVector(fwd, 7.0);
	AddVectors(fwd, pos, pos);
	
	TeleportEntity(ent, pos, Float:{90.0,0.0,0.0}, force);

	SetEntPropVector(ent, Prop_Data, "m_vecAbsVelocity", Float:{0.0,0.0,0.0}); //trampoline fix
}

public Action explode(Handle timer, any data){
	if(IsValidEntity(data)){
		AcceptEntityInput(data, "Kill");
	}
}