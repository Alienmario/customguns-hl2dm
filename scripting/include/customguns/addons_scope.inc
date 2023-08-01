#if defined _CustomGuns_Addons_Scope_included
	#endinput
#endif
#define _CustomGuns_Addons_Scope_included

static int zoomingGunIndex[MAXPLAYERS + 1];
static float nextToggle[MAXPLAYERS + 1];

ScopeInit(client)
{
	zoomingGunIndex[client] = -1;
	nextToggle[client] = 0.0;
}

ScopeThink(int client, int buttons, int gunIndex, bool forceOff)
{
	if ((buttons & IN_ZOOM || forceOff) && zooming(client))
	{
		toggleZoom(client, gunIndex);
	}
	else if (!forceOff) {
		if (gunIndex != -1)
		{
			if (GetArrayCell(gunScopeFov, gunIndex) != -1)
			{
				if (GetEntProp(client, Prop_Data, "m_afButtonPressed") & IN_ATTACK2)
				{
					if (!(!zooming(client) && buttons & IN_ZOOM) && GetGameTime() >= nextToggle[client])
						toggleZoom(client, gunIndex);
				}
			}
			else if (zooming(client)) {
				toggleZoom(client, gunIndex);
			}
		}
		else if (zooming(client)) {
			toggleZoom(client, gunIndex);
		}
	}

	if (buttons & IN_ZOOM)
	{
		setViewmodelVisible(client, false);
	}
	else if (!zooming(client) && GetEntProp(client, Prop_Data, "m_afButtonReleased") & IN_ZOOM) {
		setViewmodelVisible(client, true);
	}
}

static void toggleZoom(int client, int gunIndex)
{
	char buffer[PLATFORM_MAX_PATH];
	if (!zooming(client))
	{
		fov(client, GetArrayCell(gunScopeFov, gunIndex), 0.1);
		setViewmodelVisible(client, false);
		setCrosshairVisible(client, false);

		GetArrayString(gunScopeOverlay, gunIndex, buffer, sizeof(buffer));
		Format(buffer, sizeof(buffer), "r_screenoverlay %s", buffer);
		ClientCommand(client, buffer);

		GetArrayString(gunScopeSoundOn, gunIndex, buffer, sizeof(buffer));
		EmitGameSoundToAll(buffer, client);

		zoomingGunIndex[client] = gunIndex;
	}
	else {
		fov(client, 0, 0.2);
		setViewmodelVisible(client, true);
		setCrosshairVisible(client, true);
		ClientCommand(client, "r_screenoverlay -1");

		GetArrayString(gunScopeSoundOff, zoomingGunIndex[client], buffer, sizeof(buffer));
		EmitGameSoundToAll(buffer, client);

		zoomingGunIndex[client] = -1;
	}
	nextToggle[client] = GetGameTime() + 0.2;
}

bool zooming(int client)
{
	return zoomingGunIndex[client] != -1;
}