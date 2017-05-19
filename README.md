# customguns-hl2dm
Sourcemod plugin that adds custom weapons to Half life 2: Deathmatch

Requirements
---------
[Metamod](http://www.metamodsource.net/) and [Sourcemod](http://www.sourcemod.net)  
[DHooks](https://forums.alliedmods.net/showthread.php?t=180114) 

Getting started
---------
Each custom weapon consists of:
* Weapon script
* Models & materials & other content like sounds (But you can use stock game content)
* Controlling plugin (Optional for more advanced weapons)

Weapon scripts are based on [Valve's weapon script](https://developer.valvesoftware.com/wiki/Weapon_script) files. It is recommended that you understand how all this works first. This plugin will read these files and look for a "CustomGunsPluginData" block that defines the custom weapon's properties and behavior. The properties read are mostly self explainatory and documented within the scripts of included examples.  
It's also possible to code weapons in their seperate plugins with the use of provided natives; however, script files are required for all weapons.

Original fields used (for authoring custom weapons)
* "viewmodel"
* "playermodel"
* "anim_prefix"
* "primary_ammo"
* "secondary_ammo"
* "clip_size"
* "default_clip"
* "clip2_size"
* "default_clip2"
* "autoswitchto"
* "autoswitchfrom"
* rumble
* SoundData { ... }

Installing the plugin
---------
Copy configs, gamedata and plugins folders into hl2mp/addons/sourcemod

Weapon types & Examples
---------
* Check out the [Bubbleblower example weapon](examples/weapon_bubbleblower) for custom scripted weapons (also the [natives](scripting/include/customguns.inc)).   
* Check out the [Molotov example weapon](examples/weapon_molotov) for throwable weapons.   
* Check out the [OICW example weapon](examples/weapon_oicw) for bullet firing weapons.   
* There is also the default ["Hands" weapon example](examples/weapon_hands) that can help accessing other weapons. 

Installing weapons
---------
Using the custom folder is the preferred way of organizing, although dropping the weapon content under hl2mp can work as well and offers better compatibility with FastDL systems and such.
```
hl2mp
└───custom
    └───Custom_Weapons
        ├───materials
        ├───models
        ├───scripts
        │   │───unused
        │   │───weapon_sampleblaster420.txt
        │   │───game_sounds_manifest.txt
        │   └───game_sounds_weapons_custom.txt
        └───sound
```

Weapon scripts (the txt files) go in the scripts folder. You can also easily disable weapons by moving them to the scripts/unused folder. 
[game_sounds_manifest.txt](examples/game_sounds_manifest.txt) and [game_sounds_weapons_custom.txt](examples/game_sounds_weapons_custom.txt) both go in the scripts folder. The latter file is where additional weapon sounds are added.

Other content (models, sounds and materials) goes in its respective directory.

If the weapon has its own plugin, like the [bubbleblower example](examples/weapon_bubbleblower), copy it into sourcemod's plugins folder.  

Once in-game, hold the reload button or bind +attack3 to activate the weapon switcher. Use the command sm_customguns to list and spawn weapons.

Convars & Commands
---------
CVARS:  
```
customguns_default | The preferred custom weapon that players should spawn with (weapon_hands by default)
customguns_global_switcher | Enables fast switching from any weapon by holding reload button. If 0, players can switch only when holding a custom weapon.
customguns_autogive | Globally enables/disables auto-giving of all custom weapons
customguns_order_alphabetically | If enabled, orders weapons by name in the menu, rather than the order they were picked up. Only applies to dynamic wheel mode
customguns_static_wheel | Enables stationary item placement in the radial menu (1) versus dynamic placement and resizing (0)
hl2dm_customguns_version | Plugin version
```
Commands:
```
sm_customgun or sm_customguns | Spawns a custom gun by classname or gives it to a player if specified. Also lists available classnames.
sm_gunmenu | Lets the players pick their weapon menu skin. Styles can be customized in configs/customguns_styles.txt
sm_seqtest <sequence id> | Viewmodel sequence test
```

Special thanks
---------
Sourcemod (http://www.sourcemod.net)  
DHooks by Dr!fter (https://forums.alliedmods.net/showthread.php?t=180114)  
This thread (https://forums.alliedmods.net/showthread.php?p=1876312)   
[FT]Xen0morph  
Henky‼  
