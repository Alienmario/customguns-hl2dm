#if defined _CustomGuns_Const_included
	#endinput
#endif
#define _CustomGuns_Const_included

/****************************
 *
 *	 PLUGIN CONSTANTS
 *
 *****************************/

#define FLT_IDKWHATSMAX 99999999999999999999999.0

enum GunType
{
	GunType_Bullet,
	GunType_Custom,
	GunType_Throwable
}

enum trailType
{
	TRAIL_NONE = 0,
	TRAIL_SMOKE = 1,
	TRAIL_FIRE = 2,
	TRAIL_BEAM = 3,
	TRAIL_SPORE = 4
}

enum explodeType
{
	EXPL_NONE = 0,
	EXPL_CLASSIC = 1,
	EXPL_SMOKE = 2,
	EXPL_MOLOTOV = 3,
	EXPL_PULSE = 4,
	EXPL_BUGBAIT = 5
}

// Needed to use various fof weapons as base
enum FofBase
{
	FofBase_Pistol,
	FofBase_Melee,
	FofBase_Large,
	FofBase_Boom,
}

/****************************
 *
 *	 SDK CONSTANTS
 *
 *****************************/

#define MAX_WEAPONS                    48
#define MAX_AMMO_SLOTS                 32

// https://github.com/ValveSoftware/source-sdk-2013/blob/55ed12f8d1eb6887d348be03aee5573d44177ffb/mp/src/game/shared/npcevent.h
// Operator_HandleAnimEvent
#define EVENT_WEAPON_THROW             3005
#define EVENT_WEAPON_THROW2            3013
#define EVENT_WEAPON_THROW3            3016
#define EVENT_WEAPON_SEQUENCE_FINISHED 3900

#define GRENADE_PAUSED_NO              0
#define GRENADE_PAUSED_PRIMARY         1
#define GRENADE_PAUSED_SECONDARY       2

#define TRACER_FLAG_WHIZ               0x0001
#define TRACER_FLAG_USEATTACHMENT      0x0002

#define TRACER_DONT_USE_ATTACHMENT     -1

enum PLAYER_ANIM
{
	PLAYER_IDLE,
	PLAYER_WALK,
	PLAYER_JUMP,
	PLAYER_SUPERJUMP,
	PLAYER_DIE,
	PLAYER_ATTACK1,
	PLAYER_IN_VEHICLE,

	// TF Player animations
	PLAYER_RELOAD,
	PLAYER_START_AIMING,
	PLAYER_LEAVE_AIMING,
};

enum Class_T
{
	CLASS_NONE = 0,
	CLASS_PLAYER,
	CLASS_PLAYER_ALLY,
	CLASS_PLAYER_ALLY_VITAL,
	CLASS_ANTLION,
	CLASS_BARNACLE,
	CLASS_BULLSEYE,
	// CLASS_BULLSQUID,
	CLASS_CITIZEN_PASSIVE,
	CLASS_CITIZEN_REBEL,
	CLASS_COMBINE,
	CLASS_COMBINE_GUNSHIP,
	CLASS_CONSCRIPT,
	CLASS_HEADCRAB,
	// CLASS_HOUNDEYE,
	CLASS_MANHACK,
	CLASS_METROPOLICE,
	CLASS_MILITARY,
	CLASS_SCANNER,
	CLASS_STALKER,
	CLASS_VORTIGAUNT,
	CLASS_ZOMBIE,
	CLASS_PROTOSNIPER,
	CLASS_MISSILE,
	CLASS_FLARE,
	CLASS_EARTH_FAUNA,
	CLASS_HACKED_ROLLERMINE,
	CLASS_COMBINE_HUNTER,

	NUM_AI_CLASSES
};

enum WeaponSound_t
{
	EMPTY,
	SINGLE,
	SINGLE_NPC,
	WPN_DOUBLE,
	DOUBLE_NPC,
	BURST,
	RELOAD,
	RELOAD_NPC,
	MELEE_MISS,
	MELEE_HIT,
	MELEE_HIT_WORLD,
	SPECIAL1,
	SPECIAL2,
	SPECIAL3,
	TAUNT,
	DEPLOY,

	NUM_SHOOT_SOUND_TYPES,
};

enum SolidType_t
{
	SOLID_NONE = 0,      // no solid model
	SOLID_BSP = 1,       // a BSP tree
	SOLID_BBOX = 2,      // an AABB
	SOLID_OBB = 3,       // an OBB (not implemented yet)
	SOLID_OBB_YAW = 4,   // an OBB, constrained so that it can only yaw
	SOLID_CUSTOM = 5,    // Always call into the entity for tests
	SOLID_VPHYSICS = 6,  // solid vphysics object, get vcollide from the model and collide with that
	SOLID_LAST,
};

enum Collision_Group_t
{
	COLLISION_GROUP_NONE = 0,
	COLLISION_GROUP_DEBRIS,              // Collides with nothing but world and static stuff
	COLLISION_GROUP_DEBRIS_TRIGGER,      // Same as debris, but hits triggers
	COLLISION_GROUP_INTERACTIVE_DEBRIS,  // Collides with everything except other interactive debris or debris
	COLLISION_GROUP_INTERACTIVE,         // Collides with everything except interactive debris or debris
	COLLISION_GROUP_PLAYER,
	COLLISION_GROUP_BREAKABLE_GLASS,
	COLLISION_GROUP_VEHICLE,
	COLLISION_GROUP_PLAYER_MOVEMENT,  // For HL2, same as Collision_Group_Player, for
	                                  // TF2, this filters out other players and CBaseObjects
	COLLISION_GROUP_NPC,              // Generic NPC group
	COLLISION_GROUP_IN_VEHICLE,       // for any entity inside a vehicle
	COLLISION_GROUP_WEAPON,           // for any weapons that need collision detection
	COLLISION_GROUP_VEHICLE_CLIP,     // vehicle clip brush to restrict vehicle movement
	COLLISION_GROUP_PROJECTILE,       // Projectiles!
	COLLISION_GROUP_DOOR_BLOCKER,     // Blocks entities not permitted to get near moving doors
	COLLISION_GROUP_PASSABLE_DOOR,    // Doors that the player shouldn't collide with
	COLLISION_GROUP_DISSOLVING,       // Things that are dissolving are in this group
	COLLISION_GROUP_PUSHAWAY,         // Nonsolid on client and server, pushaway in player code

	COLLISION_GROUP_NPC_ACTOR,     // Used so NPCs in scripts ignore the player.
	COLLISION_GROUP_NPC_SCRIPTED,  // USed for NPCs in scripts that should not collide with each other

	LAST_SHARED_COLLISION_GROUP
};

enum
{
	HL2COLLISION_GROUP_PLASMANODE = view_as<int>(LAST_SHARED_COLLISION_GROUP),
	HL2COLLISION_GROUP_SPIT,
	HL2COLLISION_GROUP_HOMING_MISSILE,
	HL2COLLISION_GROUP_COMBINE_BALL,

	HL2COLLISION_GROUP_FIRST_NPC,
	HL2COLLISION_GROUP_HOUNDEYE,
	HL2COLLISION_GROUP_CROW,
	HL2COLLISION_GROUP_HEADCRAB,
	HL2COLLISION_GROUP_STRIDER,
	HL2COLLISION_GROUP_GUNSHIP,
	HL2COLLISION_GROUP_ANTLION,
	HL2COLLISION_GROUP_LAST_NPC,
	HL2COLLISION_GROUP_COMBINE_BALL_NPC,
};

enum SolidFlags_t
{
	FSOLID_CUSTOMRAYTEST = 0x0001,         // Ignore solid type + always call into the entity for ray tests
	FSOLID_CUSTOMBOXTEST = 0x0002,         // Ignore solid type + always call into the entity for swept box tests
	FSOLID_NOT_SOLID = 0x0004,             // Are we currently not solid?
	FSOLID_TRIGGER = 0x0008,               // This is something may be collideable but fires touch functions
	                                       // even when it's not collideable (when the FSOLID_NOT_SOLID flag is set)
	FSOLID_NOT_STANDABLE = 0x0010,         // You can't stand on this
	FSOLID_VOLUME_CONTENTS = 0x0020,       // Contains volumetric contents (like water)
	FSOLID_FORCE_WORLD_ALIGNED = 0x0040,   // Forces the collision rep to be world-aligned even if it's SOLID_BSP or SOLID_VPHYSICS
	FSOLID_USE_TRIGGER_BOUNDS = 0x0080,    // Uses a special trigger bounds separate from the normal OBB
	FSOLID_ROOT_PARENT_ALIGNED = 0x0100,   // Collisions are defined in root parent's local coordinate space
	FSOLID_TRIGGER_TOUCH_DEBRIS = 0x0200,  // This trigger will touch debris objects

	FSOLID_MAX_BITS = 10
};