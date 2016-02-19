#if defined _CustomGuns_Settings_included
	#endinput
#endif
#define _CustomGuns_Settings_included

#define SND_OPEN "npc/roller/remote_yes.wav"
#define SND_CLOSE_OK "common/wpn_hudoff.wav" // "ui/buttonclickrelease.wav"
#define SND_CLOSE_CANC "Player.DenyWeaponSelection"
#define SND_SELECT "Player.WeaponSelectionMoveSlot" //mouseover

#define MOLOTOV_BURN_DMG 2.0
#define MOLOTOV_BURN_DMG_PL 0.4

#define MOLOTOV_EXPLODE "ambient/fire/mtov_flame2.wav"
#define MOLOTOV_BURN_LOOP "ambient/fire/fire_small_loop2.wav"

//#define BOUNCY_STYLE2

#if defined BOUNCY_STYLE2

	#define MENU_DISTANCE 60.0
	#define MENU_RADIUS 40.0
	#define SELECT_THRESHOLD 0.86

	#define MENU_CENTER_MODEL_ENABLED 0
	#define MENU_CENTER_MODEL "materials/.vmt"
	#define MENU_CENTER_MODEL_SIZE 0.1
	#define MENU_CENTER_MODEL_SIZE_SELECTED 0.2

	// background beams drawing settings
	#define MENU_BG_MODEL "materials/effects/laserplane.vmt"
	#define MENU_BG_SPEED 8
	#define MENU_BG_COLOR {20, 150, 255, 255}
	#define MENU_BG_COLOR_SELECT {150, 255, 255, 255}
	#define MENU_BG_COLOR_UNSELECTABLE {255, 150, 50, 50}
	#define MENU_BG_START_WIDTH_SCALE 0.5
	#define MENU_BG_END_WIDTH_SCALE 0.6
	#define MENU_BG_DISTANCE_OFFSET -25.0
	// from center (0) to menu radius (1), where the beams start
	#define MENU_BG_CENTER_START_SCALE_ADD 0.25
	#define MENU_BG_CENTER_START_SCALE_SELECTED_MULTIPLIER 0.0

	#define MODEL_TEXT "materials/sprites/lgtning_noz.vmt"
	#define TEXT_DISTANCE_OFFSET -25.0
	
#elseif defined BOUNCY_STYLE

	#define MENU_DISTANCE 60.0
	#define MENU_RADIUS 45.0
	#define SELECT_THRESHOLD 0.86

	#define MENU_CENTER_MODEL_ENABLED 0
	#define MENU_CENTER_MODEL "materials/.vmt"
	#define MENU_CENTER_MODEL_SIZE 0.1
	#define MENU_CENTER_MODEL_SIZE_SELECTED 0.2

	// background beams drawing settings
	#define MENU_BG_MODEL "materials/sprites/blueshaft1.vmt"
	#define MENU_BG_SPEED 9
	#define MENU_BG_COLOR {255, 255, 255, 20}
	#define MENU_BG_COLOR_SELECT {255, 125, 50, 200}
	#define MENU_BG_COLOR_UNSELECTABLE {80, 5, 5, 70}
	#define MENU_BG_START_WIDTH_SCALE 1.0
	#define MENU_BG_END_WIDTH_SCALE 0.9
	#define MENU_BG_DISTANCE_OFFSET -25.0
	// from center (0) to menu radius (1), where the beams start
	#define MENU_BG_CENTER_START_SCALE_ADD 0.25
	#define MENU_BG_CENTER_START_SCALE_SELECTED_MULTIPLIER 0.0

	#define MODEL_TEXT "materials/sprites/lgtning_noz.vmt"
	#define TEXT_DISTANCE_OFFSET -25.0

#else

	#define MENU_DISTANCE 60.0
	#define MENU_RADIUS 45.0
	#define SELECT_THRESHOLD 0.84

	#define MENU_CENTER_MODEL_ENABLED 0
	#define MENU_CENTER_MODEL "materials/vgui/ui_logo.vmt"
	#define MENU_CENTER_MODEL_SIZE 0.1
	#define MENU_CENTER_MODEL_SIZE_SELECTED 0.2

	// background beams drawing settings
	#define MENU_BG_MODEL "materials/sprites/reticle2.vmt"
	#define MENU_BG_SPEED 0
	#define MENU_BG_COLOR {200, 100, 255, 255}
	#define MENU_BG_COLOR_SELECT {255, 255, 255, 255}
	#define MENU_BG_COLOR_UNSELECTABLE {150, 255, 255, 200}
	#define MENU_BG_START_WIDTH_SCALE 0.7
	#define MENU_BG_END_WIDTH_SCALE 0.7
	#define MENU_BG_DISTANCE_OFFSET -25.0
	// from center (0) to outer circle (1), where the beams start
	#define MENU_BG_CENTER_START_SCALE_ADD -1.8
	#define MENU_BG_CENTER_START_SCALE_SELECTED_MULTIPLIER 2.0

	#define MODEL_TEXT "materials/sprites/lgtning_noz.vmt"
	#define TEXT_DISTANCE_OFFSET -25.0
#endif
