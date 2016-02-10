#if defined _CustomGuns_Settings_included
	#endinput
#endif
#define _CustomGuns_Settings_included


#define SND_OPEN "npc/roller/remote_yes.wav"
#define SND_CLOSE_OK "common/wpn_hudoff.wav" // "ui/buttonclickrelease.wav"
#define SND_CLOSE_CANC "Player.DenyWeaponSelection"
#define SND_SELECT "Player.WeaponSelectionMoveSlot" //mouseover

#define MENU_DISTANCE 60.0
#define MENU_RADIUS 45.0
#define SELECT_THRESHOLD 0.86


#define MENU_BG_MODEL "materials/sprites/reticle2.vmt"
#define MENU_BG_SPEED 0
#define MENU_BG_COLOR {200, 100, 255, 200}
#define MENU_BG_COLOR_SELECT {255, 255, 255, 255}
#define MENU_BG_COLOR_UNSELECTABLE {150, 200, 200, 255}
#define MENU_BG_START_WIDTH_SCALE 0.7
#define MENU_BG_END_WIDTH_SCALE 0.7
#define MENU_BG_DISTANCE_OFFSET -25.0
#define MENU_BG_DISTANCE MENU_DISTANCE+MENU_BG_DISTANCE_OFFSET

#define MODEL_TEXT "materials/sprites/lgtning_noz.vmt"
#define TEXT_DISTANCE_OFFSET -25.0

#define MODEL_BORDER "materials/particle/particle_ring_refract_01.vmt"
#define MENU_BORDER_SCALE 0.95
#define MENU_BORDER_TRANSPARENCY 255
#define MENU_BORDER_DISTANCE_OFFSET 20.0
#define MENU_BORDER_Z_OFFSET -0.0


#define MOLOTOV_BURN_DMG 2.0
#define MOLOTOV_BURN_DMG_PL 0.4

#define MOLOTOV_EXPLODE "ambient/fire/mtov_flame2.wav"
#define MOLOTOV_BURN_LOOP "ambient/fire/fire_small_loop2.wav"
