#if defined _CustomGuns_Menu_included
	#endinput
#endif
#define _CustomGuns_Menu_included

#define SND_OPEN       "npc/roller/remote_yes.wav"  // "npc/roller/remote_yes.wav" // "ui/buttonclickrelease.wav" // "common/null.wav"
#define SND_CLOSE_OK   "common/wpn_hudoff.wav"      // "ui/buttonclickrelease.wav"
#define SND_CLOSE_CANC "Player.DenyWeaponSelection"
#define SND_SELECT     "Player.WeaponSelectionMoveSlot"  // mouseover
public void WheelModeChanged(ConVar convar, const char[] oldValue, const char[] newValue)
{
	for (int i = 1; i <= MaxClients; i++)
	{
		if (IsClientInGame(i) && !IsFakeClient(i))
		{
			generateWheel(i);
		}
	}
}

generateWheel(int client)
{
	Handle sourceArray;
	if (GetConVarBool(customguns_static_wheel))
	{
		sourceArray = gunClassNames;
	}
	else {
		sourceArray = inventory[client];
	}
	int size = GetArraySize(sourceArray);
	if (size == GetArraySize(inventoryWheel[client]))
	{
		return;  // already correct calculations for current wheel size
	}

	Style s = GetClientStyle(client);
	float piece = 360.0 / size;
	float pos[3];
	ClearArray(inventoryWheel[client]);
	if (size == 1)
	{
		pos[1] = s.GetFl(MENU_DISTANCE);
		PushArrayArray(inventoryWheel[client], pos)
	}
	else {
		for (int i = 0; i < size; i++)
		{
			pos[0] = Cosine(DegToRad(i * piece)) * s.GetFl(MENU_RADIUS);
			pos[1] = s.GetFl(MENU_DISTANCE);
			pos[2] = Sine(DegToRad(i * piece)) * s.GetFl(MENU_RADIUS);
			PushArrayArray(inventoryWheel[client], pos)
		}
	}
}

drawMenu(int client)
{
	float eyePos[3], fwdVector[3], rightVector[3], upVector[3];
	GetClientEyePosition(client, eyePos);
	GetAngleVectors(startEyeAngles[client], fwdVector, rightVector, upVector);

	Style s = GetClientStyle(client);
	int selected = getInventoryAimIndex(client);
	bool shouldDrawText = GetGameTime() >= nextDrawText[client];
	bool staticWheel = GetConVarBool(customguns_static_wheel);

	int size = GetArraySize(inventory[client]);
	float beamEndWidth = 360.0 / (staticWheel ? GetArraySize(gunClassNames) : size) / 2.0 * s.GetFl(MENU_BG_END_WIDTH_SCALE);
	if (beamEndWidth > 25.0) beamEndWidth = 25.0;
	float beamStartWidth = beamEndWidth / 4 * s.GetFl(MENU_BG_START_WIDTH_SCALE);

	float center[3];
	center = fwdVector;
	ScaleVector(center, s.GetFl(MENU_DISTANCE));
	AddVectors(eyePos, center, center);

	static int lastSelected[MAXPLAYERS + 1];
	if (selected != lastSelected[client])
	{
		EmitGameSoundToClient(client, SND_SELECT);
		lastSelected[client] = selected;
	}

	// DRAW CENTER SPRITE
	if (s.Get(MENU_CENTER_MODEL_ENABLED))
	{
		TE_SetupGlowSprite(center, s.Get(MENU_CENTER_MODEL_ID), 0.1, selected == -1 ? s.GetFl(MENU_CENTER_MODEL_SIZE_SELECTED) : s.GetFl(MENU_CENTER_MODEL_SIZE), 255);
		TE_SendToClient(client);
	}

	// DRAW MODELS
	for (int i = 0; i < size; i++)
	{
		float localPos[3];
		float gunPos[3];
		float animScale = GetArrayCell(inventoryAnimScale[client], i);
		bool selectable = canSelectItem(client, i);
		int thisGunIndex = GetArrayCell(inventory[client], i);

		GetArrayArray(inventoryWheel[client], staticWheel ? thisGunIndex : i, localPos);

		if (i == selected && selectable)
		{
			if (animScale < 1.0)
			{
				float speed = 1.02 - animScale;
				if (speed > 1.0) speed = 1.0;
				animScale += 0.15 * speed;
			}
		}
		else {
			if (animScale > 0.0)
			{
				float speed = animScale - 0.1;
				if (speed < 0.0) speed = 0.0;
				animScale -= 0.1 * speed;

				if (speed < 0.01) animScale = 0.0;
			}
		}
		if (!shouldDrawText || selected == -1)
		{
			float scale = 0.9 - animScale * 0.4;      // scale inwards the circle
			float fwdScale = 1.0 - animScale * 0.25;  // scale towards the player
			SetArrayCell(inventoryAnimScale[client], i, animScale);

			gunPos[0] = eyePos[0] + rightVector[0] * localPos[0] * scale + fwdVector[0] * localPos[1] * fwdScale + upVector[0] * localPos[2] * scale;
			gunPos[1] = eyePos[1] + rightVector[1] * localPos[0] * scale + fwdVector[1] * localPos[1] * fwdScale + upVector[1] * localPos[2] * scale;
			gunPos[2] = eyePos[2] + rightVector[2] * localPos[0] * scale + fwdVector[2] * localPos[1] * fwdScale + upVector[2] * localPos[2] * scale;

			TE_SetupGlowSprite(gunPos, GetArrayCell(gunModelIndexes, thisGunIndex), 0.1, 0.2, selectable ? 255 : 35);
			TE_SendToClient(client);

			// draw BG beams
			float beamStartPos[3], beamEndPos[3];
			beamEndPos[0] = eyePos[0] + rightVector[0] * localPos[0] + fwdVector[0] * localPos[1] * fwdScale + upVector[0] * localPos[2];
			beamEndPos[1] = eyePos[1] + rightVector[1] * localPos[0] + fwdVector[1] * localPos[1] * fwdScale + upVector[1] * localPos[2];
			beamEndPos[2] = eyePos[2] + rightVector[2] * localPos[0] + fwdVector[2] * localPos[1] * fwdScale + upVector[2] * localPos[2];

			MoveVector(center, beamEndPos, fwdScale * s.GetFl(MENU_BG_CENTER_START_SCALE_SELECTED_MULTIPLIER) + s.GetFl(MENU_BG_CENTER_START_SCALE_ADD), beamStartPos);

			int color[4];
			s.GetArray(i == selected ? MENU_BG_COLOR_SELECT : (selectable ? MENU_BG_COLOR : MENU_BG_COLOR_UNSELECTABLE), color, sizeof(color));
			TE_SetupBeamPoints(beamStartPos, beamEndPos, s.Get(MENU_BG_MODEL_ID), -1, 0, 30, 0.1, beamStartWidth, beamEndWidth, 0, 0.0, color, s.Get(MENU_BG_SPEED));
			TE_SendToClient(client);
		}

		// draw text
		else if (selected == i && shouldDrawText) {
			float minText[3];
			minText = fwdVector;
			float maxText[3];

			char buffer[32];
			GetArrayString(gunNames, GetArrayCell(inventory[client], selected), buffer, sizeof(buffer));

			ScaleVector(minText, s.GetFl(MENU_DISTANCE) + s.GetFl(TEXT_DISTANCE_OFFSET));
			AddVectors(eyePos, minText, minText);

			maxText = minText;

			float textRightVector[3];
			textRightVector = rightVector;
			ScaleVector(textRightVector, -40.0);
			AddVectors(minText, textRightVector, minText);
			ScaleVector(textRightVector, -1.0);
			AddVectors(maxText, textRightVector, maxText);

			DrawText(client, minText, maxText, s.Get(MODEL_TEXT_ID), buffer, 0.20, DRAWTEXT_ALIGN_CENTER, _, DRAWTEXT_WIDTH_DEFAULT * 1.5, DRAWTEXT_HEIGHT_DEFAULT * 1.5, DRAWTEXT_SIZE_DEFAULT, 50.0 * (1 - animScale));
		}
	}

	if (shouldDrawText)
	{
		nextDrawText[client] = GetGameTime() + 0.1;
	}
}

onMenuOpening(int client)
{
	GetClientEyeAngles(client, startEyeAngles[client]);
	EmitSoundToClient(client, SND_OPEN, _, _, _, _, 0.6);
	fov(client, 100, 0.3);
	setViewmodelVisible(client, false);
	// SetEntityFlags(client, GetEntityFlags(client)|FL_ATCONTROLS);
}

onMenuClosing(int client)
{
	fov(client, -1, 0.2);
	setViewmodelVisible(client, true);
	// SetEntityFlags(client, GetEntityFlags(client) & ~FL_ATCONTROLS);

	int selected = getInventoryAimIndex(client);
	if (selected != -1)
	{
		preferedGunIndex[client] = GetArrayCell(inventory[client], selected);
		EmitSoundToClient(client, SND_CLOSE_OK);
		giveCustomGun(client, preferedGunIndex[client], true);
	}
	else EmitGameSoundToClient(client, SND_CLOSE_CANC);
}

int getInventoryAimIndex(int client)
{
	float eyePos[3], eyeAngles[3], fwdVector[3];
	GetClientEyePosition(client, eyePos);
	GetClientEyeAngles(client, eyeAngles);
	SubtractVectors(eyeAngles, startEyeAngles[client], eyeAngles);
	GetAngleVectors(eyeAngles, fwdVector, NULL_VECTOR, NULL_VECTOR);

	float highest = 0.0;
	int highestIndex = -1;
	bool staticWheel = GetConVarBool(customguns_static_wheel);
	int size = GetArraySize(inventory[client]);
	float threshold = GetClientStyle(client).GetFl(SELECT_THRESHOLD);

	for (int i = 0; i < size; i++)
	{
		if (!canSelectItem(client, i)) continue;

		float localPos[3];
		GetArrayArray(inventoryWheel[client], staticWheel ? GetArrayCell(inventory[client], i) : i, localPos);

		// bullfix
		float temp = -localPos[0];
		localPos[0] = localPos[1];
		localPos[1] = temp;

		NormalizeVector(localPos, localPos);

		float a = GetVectorDotProduct(localPos, fwdVector);
		if (threshold<a> highest)
		{
			highest = a;
			highestIndex = i;
		}
	}
	return highestIndex;
}