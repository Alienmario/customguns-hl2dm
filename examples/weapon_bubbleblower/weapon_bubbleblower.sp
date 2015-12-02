#include <sourcemod>
#include <sdktools>
#include <customguns>

int model;

public OnMapStart(){
	model = PrecacheModel("effects/bubble.vmt", true);
}

public void CG_OnPrimaryAttack(int client, int weapon){
	char sWeapon[32];
	GetEntityClassname(weapon, sWeapon, sizeof(sWeapon));
	
	if(StrEqual(sWeapon, "weapon_bubbleblower")){
		CG_SetPlayerAnimation(client, PLAYER_ATTACK1);
		CG_PlayPrimaryAttack(weapon);
		CG_Cooldown(weapon, 0.2);
		
		float direction[3], pos[3];
		CG_GetShootPosition(client, pos, 12.0, 6.0, -3.0);
		
		GetClientEyeAngles(client, direction);
		GetAngleVectors(direction, direction, NULL_VECTOR, NULL_VECTOR);
		ScaleVector(direction, GetRandomFloat(600.0, 1000.0));
		
		TE_SetupSpriteSpray(pos, direction, Float:{10.0, 10.0, 10.0}, model, 200, 50, 2.0, 0);
		TE_SendToAll();
	}
}