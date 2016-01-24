#if defined _CustomGuns_Menu_included
	#endinput
#endif
#define _CustomGuns_Menu_included


generateWheel(int client) {
	ClearArray(inventoryWheel[client]);
	
	int size = GetArraySize(inventory[client]);
	float piece = 360.0 / size;
	float pos[3];
	
	if (size == 1) {
		pos[1] = MENU_DISTANCE;
		PushArrayArray(inventoryWheel[client], pos)
	} else {
		for (int i = 0; i < size; i++) {
			pos[0] = Cosine(DegToRad(i * piece)) * MENU_RADIUS;
			pos[1] = MENU_DISTANCE;
			pos[2] = Sine(DegToRad(i * piece)) * MENU_RADIUS;
			PushArrayArray(inventoryWheel[client], pos)
		}
	}
/* 	
	float wep1Pos[3], wep2Pos[3];
	if (size > 1) {
		GetArrayArray(inventoryWheel[client], 0, wep1Pos); NormalizeVector(wep1Pos, wep1Pos);
		GetArrayArray(inventoryWheel[client], 1, wep2Pos); NormalizeVector(wep2Pos, wep2Pos);
		threshold[client] = GetVectorDotProduct(wep1Pos, wep2Pos);
	} else threshold[client] = 0.0; */
}

drawMenu(int client) {
	float eyePos[3], fwdVector[3], rightVector[3], upVector[3];
	GetClientEyePosition(client, eyePos);
	
	GetAngleVectors(startEyeAngles[client], fwdVector, rightVector, upVector);
	
	int selected = getInventoryAimIndex(client);
	bool shouldDraw = GetGameTime() >= nextDraw[client]; // should draw text
	
	int size = GetArraySize(inventory[client]);
	float beamEndWidth = 360.0/size/2.0;
	if(beamEndWidth > 25.0) beamEndWidth = 25.0;
		
	//DRAW MODELS
	for (int i = 0; i < size; i++) {
		float localPos[3];
		float gunPos[3];
		float animScale = GetArrayCell(inventoryAnimScale[client], i);
		bool selectable = canSelectItem(client, i);
		
		GetArrayArray(inventoryWheel[client], i, localPos);
		
		if (i == selected && selectable) {
			if (animScale < 1.0) {
				if (animScale <= 0.0) {
					//first selected
					EmitGameSoundToClient(client, SND_SELECT);
				}
				
				float speed = 1.02 - animScale;
				if (speed > 1.0)speed = 1.0;
				animScale += 0.05 * speed;
			}
		} else {
			if (animScale > 0.0) {
				float speed = animScale - 0.1;
				if (speed < 0.0)speed = 0.0;
				animScale -= 0.1 * speed;
				
				if (speed < 0.01)animScale = 0.0;
			}
		}
		
		float scale = 0.9 - animScale * 0.5; // scale inwards the circle
		float fwdScale = 1.0 - animScale * 0.25; // scale towards the player
		SetArrayCell(inventoryAnimScale[client], i, animScale);
		
		gunPos[0] = eyePos[0] + rightVector[0] * localPos[0] * scale + fwdVector[0] * localPos[1] * fwdScale + upVector[0] * localPos[2] * scale;
		gunPos[1] = eyePos[1] + rightVector[1] * localPos[0] * scale + fwdVector[1] * localPos[1] * fwdScale + upVector[1] * localPos[2] * scale;
		gunPos[2] = eyePos[2] + rightVector[2] * localPos[0] * scale + fwdVector[2] * localPos[1] * fwdScale + upVector[2] * localPos[2] * scale;
		
		TE_SetupGlowSprite(gunPos, GetArrayCell(gunModelIndexes, GetArrayCell(inventory[client], i)), 0.1, 0.2, selectable ? 255 : 35);
		TE_SendToClient(client);
		
		//draw BG beams
		float center[3], beamStartPos[3], beamEndPos[3];
		center = fwdVector;
		ScaleVector(center, MENU_DISTANCE+MENU_BORDER_DISTANCE_OFFSET);
		AddVectors(eyePos, center, center);
		
		beamEndPos[0] = eyePos[0] + rightVector[0] * localPos[0] + fwdVector[0] * localPos[1] * fwdScale + upVector[0] * localPos[2];
		beamEndPos[1] = eyePos[1] + rightVector[1] * localPos[0] + fwdVector[1] * localPos[1] * fwdScale + upVector[1] * localPos[2];
		beamEndPos[2] = eyePos[2] + rightVector[2] * localPos[0] + fwdVector[2] * localPos[1] * fwdScale + upVector[2] * localPos[2];
		
		MoveVector(center, beamEndPos, 0.25, beamStartPos);
		
		TE_SetupBeamPoints(beamStartPos, beamEndPos, modelBG, -1, 0, 30, 0.1, 0.0, beamEndWidth, 0, 0.0, i==selected? MENU_BG_COLOR_SELECT : (selectable? MENU_BG_COLOR:MENU_BG_COLOR_UNSELECTABLE), 9);
		TE_SendToClient(client);
		
		// draw text
		if (selected == i && shouldDraw) {
			
			float minText[3]; minText = fwdVector;
			float maxText[3];
			
			char buffer[32];
			GetArrayString(gunNames, GetArrayCell(inventory[client], selected), buffer, sizeof(buffer));
			
			ScaleVector(minText, MENU_DISTANCE+TEXT_DISTANCE_OFFSET);
			AddVectors(eyePos, minText, minText);

			maxText = minText;
			
			float textRightVector[3]; textRightVector = rightVector;
			ScaleVector(textRightVector, -40.0);
			AddVectors(minText, textRightVector, minText);
			ScaleVector(textRightVector, -1.0);
			AddVectors(maxText, textRightVector, maxText);
			
			DrawText(client, minText, maxText, modelText, buffer, 0.1, DRAWTEXT_ALIGN_CENTER, _, DRAWTEXT_WIDTH_DEFAULT*1.5, DRAWTEXT_HEIGHT_DEFAULT*1.5, DRAWTEXT_SIZE_DEFAULT, 50.0 * (1-animScale));
		}
	}

	if (shouldDraw) {
		nextDraw[client] = GetGameTime() + 0.05;
	}
}

onMenuOpening(int client) {
	GetClientEyeAngles(client, startEyeAngles[client]);
	EmitSoundToClient(client, SND_OPEN, _, _, _, _, 0.6);
	fov(client, 100, 0.3);
	setViewmodelVisible(client, false);
}

onMenuClosing(int client) {
	fov(client, -1, 0.2);
	setViewmodelVisible(client, true);
	
	int selected = getInventoryAimIndex(client);
	if (selected != -1) {
		preferedGunIndex[client] = GetArrayCell(inventory[client], selected);
		EmitSoundToClient(client, SND_CLOSE_OK);
		giveCustomGun(client, preferedGunIndex[client], true);
	} else EmitGameSoundToClient(client, SND_CLOSE_CANC);
}

int getInventoryAimIndex(int client) {
	float eyePos[3], eyeAngles[3], fwdVector[3];
	GetClientEyePosition(client, eyePos);
	GetClientEyeAngles(client, eyeAngles);
	SubtractVectors(eyeAngles, startEyeAngles[client], eyeAngles);
	GetAngleVectors(eyeAngles, fwdVector, NULL_VECTOR, NULL_VECTOR);
	
	float highest = 0.0;
	int highestIndex = -1;

	int size = GetArraySize(inventory[client]);
	for (int i = 0; i < size; i++) {
		if (!canSelectItem(client, i)) continue;
		
		float localPos[3];
		GetArrayArray(inventoryWheel[client], i, localPos);
		
		//bullfix
		float temp = -localPos[0];
		localPos[0] = localPos[1];
		localPos[1] = temp;
		
		NormalizeVector(localPos, localPos);
		
		float a = GetVectorDotProduct(localPos, fwdVector);
		if (SELECT_THRESHOLD < a > highest) {
			highest = a;
			highestIndex = i;
		}
	}
	return highestIndex;
}