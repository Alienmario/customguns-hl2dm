#if defined _CustomGuns_DrawingTools_included
	#endinput
#endif
#define _CustomGuns_DrawingTools_included

DrawModel(int client, const float origin[3], int modelIndex, float life, int brightness){
	TE_Start("GlowSprite");
	TE_WriteVector("m_vecOrigin", origin);
	TE_WriteNum("m_nModelIndex", modelIndex);
	TE_WriteFloat("m_fScale", 0.2);
	TE_WriteFloat("m_fLife", life);
	TE_WriteNum("m_nBrightness", brightness);
	TE_SendToClient(client);
}

#define DrawText_LETTER_WIDTH 2.0
#define DrawText_LETTER_HEIGHT 3.0
#define DrawText_LETTER_SIZE 1.2
#define DrawText_LETTER_SPACING 0.25

int DrawText_DrawingTo;
int DrawText_Model;
int DrawText_Color[4];
float DrawText_HeightScale;
float DrawText_BoldnessScale;
float DrawText_Life;

stock DrawText(int client, float min[3], float max[3], int model, const char[] text, float life = 0.1, int color[4] = { 255, 0, 0, 255 }, bool alignRight = false, float scaleWidth = 1.0, float scaleHeight = 1.0, float scaleBoldness = 2.0){
	float distance = GetVectorDistance(min, max);
	float unit = DrawText_LETTER_WIDTH*scaleWidth+DrawText_LETTER_SPACING*scaleWidth;
	int maxLetters = RoundToFloor(distance / unit);
	if(strlen(text) < maxLetters){
		maxLetters = strlen(text);
		//distance = ((maxLetters*DrawText_LETTER_WIDTH*scaleWidth)+((maxLetters-1)*DrawText_LETTER_SPACING*scaleWidth));
	}
	
/* 	if(center){
		//get distance from last letter to max location and add half of that as the start offset
		float buffer[3];
		MoveVector(max, min, (maxLetters*DrawText_LETTER_WIDTH*scaleWidth + (maxLetters-1)*DrawText_LETTER_SPACING*scaleWidth) / distance, buffer);
		float distanceEnd = GetVectorDistance(buffer, max);
		MoveVector(min, max, (distanceEnd / 2.0) / distance, min);
		DrawText(client, min, max, model, text, life, color, false, scaleWidth, scaleHeight);
	} */

	DrawText_DrawingTo = client;
	DrawText_HeightScale = scaleHeight;
	DrawText_BoldnessScale = scaleBoldness;
	DrawText_Model = model;
	DrawText_Life = life;
	DrawText_Color = color;
	
	float letterMin[3], letterMax[3];
	for(int i = 0; i < maxLetters; i++){
		//compute min and max world location for the letter
		if(alignRight){
			MoveVector(min, max, (((maxLetters-1-i)*DrawText_LETTER_WIDTH*scaleWidth + (maxLetters-1-i)*DrawText_LETTER_SPACING*scaleWidth) / distance), letterMax);
			MoveVector(min, max, (((maxLetters-i)*DrawText_LETTER_WIDTH*scaleWidth + (maxLetters-i-1)*DrawText_LETTER_SPACING*scaleWidth) / distance), letterMin);
		} else {
			MoveVector(max, min, ((i*DrawText_LETTER_WIDTH*scaleWidth + i*DrawText_LETTER_SPACING*scaleWidth) / distance), letterMin);
			MoveVector(max, min, (((i+1)*DrawText_LETTER_WIDTH*scaleWidth + i*DrawText_LETTER_SPACING*scaleWidth) / distance), letterMax);
		}
		switch (CharToUpper(text[i])){
			case 'A':
			{
				DrawLetterLines(letterMin, letterMax, Float:{0.0,0.0}, Float:{0.5,1.0});
				DrawLetterLines(letterMin, letterMax, Float:{0.5,1.0}, Float:{1.0,0.0});
				DrawLetterLines(letterMin, letterMax, Float:{0.25,0.5}, Float:{0.75,0.5});
			}
			case 'B':
			{
				DrawLetterLines(letterMin, letterMax, Float:{0.0,0.0}, Float:{0.0,1.0});
				
				DrawLetterLines(letterMin, letterMax, Float:{0.0,1.0}, Float:{0.95,1.0});
				DrawLetterLines(letterMin, letterMax, Float:{0.0,0.5}, Float:{0.95,0.5});
				DrawLetterLines(letterMin, letterMax, Float:{0.0,0.0}, Float:{0.95,0.0});
				
				DrawLetterLines(letterMin, letterMax, Float:{1.0,0.95}, Float:{1.0,0.55});
				DrawLetterLines(letterMin, letterMax, Float:{1.0,0.05}, Float:{1.0,0.45});
			}
			case 'C':
			{
				DrawLetterLines(letterMin, letterMax, Float:{0.0,0.0}, Float:{0.0,1.0});
				DrawLetterLines(letterMin, letterMax, Float:{0.0,1.0}, Float:{1.0,1.0});
				DrawLetterLines(letterMin, letterMax, Float:{0.0,0.0}, Float:{1.0,0.0});
			}
			case 'D':
			{
				DrawLetterLines(letterMin, letterMax, Float:{0.0,0.0}, Float:{0.0,1.0});
				
				DrawLetterLines(letterMin, letterMax, Float:{0.0,1.0}, Float:{0.95,1.0});
				DrawLetterLines(letterMin, letterMax, Float:{0.0,0.0}, Float:{0.95,0.0});
				
				DrawLetterLines(letterMin, letterMax, Float:{1.0,0.95}, Float:{1.0,0.05});
			}
			case 'E':
			{
				DrawLetterLines(letterMin, letterMax, Float:{0.0,0.0}, Float:{0.0,1.0});
				DrawLetterLines(letterMin, letterMax, Float:{0.0,1.0}, Float:{1.0,1.0});
				DrawLetterLines(letterMin, letterMax, Float:{0.0,0.0}, Float:{1.0,0.0});
				DrawLetterLines(letterMin, letterMax, Float:{0.0,0.5}, Float:{1.0,0.5});
			}
			case 'F':
			{
				DrawLetterLines(letterMin, letterMax, Float:{0.0,0.0}, Float:{0.0,1.0});
				DrawLetterLines(letterMin, letterMax, Float:{0.0,1.0}, Float:{1.0,1.0});
				DrawLetterLines(letterMin, letterMax, Float:{0.0,0.5}, Float:{1.0,0.5});
			}
			case 'G':
			{
				DrawLetterLines(letterMin, letterMax, Float:{0.0,0.0}, Float:{0.0,1.0});
				DrawLetterLines(letterMin, letterMax, Float:{0.0,1.0}, Float:{0.95,1.0});
				DrawLetterLines(letterMin, letterMax, Float:{0.0,0.0}, Float:{1.0,0.0});
				DrawLetterLines(letterMin, letterMax, Float:{1.0,0.0}, Float:{1.0,0.5});
				DrawLetterLines(letterMin, letterMax, Float:{1.0,0.5}, Float:{0.75,0.5});
			}
			case 'H':
			{
				DrawLetterLines(letterMin, letterMax, Float:{0.0,0.0}, Float:{0.0,1.0});
				DrawLetterLines(letterMin, letterMax, Float:{1.0,0.0}, Float:{1.0,1.0});
				DrawLetterLines(letterMin, letterMax, Float:{0.0,0.5}, Float:{1.0,0.5});
			}
			case 'I':
			{
				DrawLetterLines(letterMin, letterMax, Float:{0.5,0.0}, Float:{0.5,1.0});
			}
			case 'J':
			{
				DrawLetterLines(letterMin, letterMax, Float:{1.0,1.0}, Float:{1.0,0.05});
				DrawLetterLines(letterMin, letterMax, Float:{0.95,0.0}, Float:{0.05,0.0});
				DrawLetterLines(letterMin, letterMax, Float:{0.0,0.05}, Float:{0.0,0.5});
			}	
			case 'K':
			{
				DrawLetterLines(letterMin, letterMax, Float:{0.0,0.0}, Float:{0.0,1.0});
				DrawLetterLines(letterMin, letterMax, Float:{0.0,0.5}, Float:{1.0,1.0});
				DrawLetterLines(letterMin, letterMax, Float:{0.0,0.5}, Float:{1.0,0.0});
			}	
			case 'L':
			{
				DrawLetterLines(letterMin, letterMax, Float:{0.0,1.0}, Float:{0.0,0.0});
				DrawLetterLines(letterMin, letterMax, Float:{0.0,0.0}, Float:{1.0,0.0});
			}		
			case 'M':
			{
				DrawLetterLines(letterMin, letterMax, Float:{0.0,0.0}, Float:{0.0,1.0});
				DrawLetterLines(letterMin, letterMax, Float:{1.0,0.0}, Float:{1.0,1.0});
				DrawLetterLines(letterMin, letterMax, Float:{0.0,1.0}, Float:{0.5,0.5});
				DrawLetterLines(letterMin, letterMax, Float:{1.0,1.0}, Float:{0.5,0.5});
			}	
			case 'N':
			{
				DrawLetterLines(letterMin, letterMax, Float:{0.0,0.0}, Float:{0.0,1.0});
				DrawLetterLines(letterMin, letterMax, Float:{1.0,0.0}, Float:{1.0,1.0});
				DrawLetterLines(letterMin, letterMax, Float:{0.0,1.0}, Float:{1.0,0.0});
			}	
			case 'O':
			{
				DrawLetterLines(letterMin, letterMax, Float:{0.0,0.0}, Float:{1.0,0.0});
				DrawLetterLines(letterMin, letterMax, Float:{1.0,0.0}, Float:{1.0,1.0});
				DrawLetterLines(letterMin, letterMax, Float:{1.0,1.0}, Float:{0.0,1.0});
				DrawLetterLines(letterMin, letterMax, Float:{0.0,1.0}, Float:{0.0,0.0});
			}
			case 'P':
			{
				DrawLetterLines(letterMin, letterMax, Float:{0.0,0.0}, Float:{0.0,1.0});
				DrawLetterLines(letterMin, letterMax, Float:{0.0,1.0}, Float:{1.0,1.0});
				DrawLetterLines(letterMin, letterMax, Float:{0.0,0.5}, Float:{1.0,0.5});
				DrawLetterLines(letterMin, letterMax, Float:{1.0,1.0}, Float:{1.0,0.5});
			}
			case 'Q':
			{
				DrawLetterLines(letterMin, letterMax, Float:{0.0,0.0}, Float:{1.0,0.0});
				DrawLetterLines(letterMin, letterMax, Float:{1.0,0.0}, Float:{1.0,1.0});
				DrawLetterLines(letterMin, letterMax, Float:{1.0,1.0}, Float:{0.0,1.0});
				DrawLetterLines(letterMin, letterMax, Float:{0.0,1.0}, Float:{0.0,0.0});
				DrawLetterLines(letterMin, letterMax, Float:{1.0,0.0}, Float:{0.8,0.2});
			}
			case 'R':
			{
				DrawLetterLines(letterMin, letterMax, Float:{0.0,0.0}, Float:{0.0,1.0});
				DrawLetterLines(letterMin, letterMax, Float:{0.0,1.0}, Float:{1.0,1.0});
				DrawLetterLines(letterMin, letterMax, Float:{0.0,0.5}, Float:{1.0,0.5});
				DrawLetterLines(letterMin, letterMax, Float:{1.0,1.0}, Float:{1.0,0.5});
				DrawLetterLines(letterMin, letterMax, Float:{0.0,0.5}, Float:{1.0,0.0});
			}
			case 'S':
			{
				DrawLetterLines(letterMin, letterMax, Float:{1.0,1.0}, Float:{0.0,1.0});
				DrawLetterLines(letterMin, letterMax, Float:{0.0,1.0}, Float:{0.0,0.5});
				DrawLetterLines(letterMin, letterMax, Float:{0.0,0.5}, Float:{1.0,0.5});
				DrawLetterLines(letterMin, letterMax, Float:{1.0,0.5}, Float:{1.0,0.0});
				DrawLetterLines(letterMin, letterMax, Float:{0.0,0.0}, Float:{1.0,0.0});
			}
			case 'T':
			{
				DrawLetterLines(letterMin, letterMax, Float:{0.5,0.0}, Float:{0.5,1.0});
				DrawLetterLines(letterMin, letterMax, Float:{0.0,1.0}, Float:{1.0,1.0});
			}
			case 'U':
			{
				DrawLetterLines(letterMin, letterMax, Float:{0.0,0.0}, Float:{1.0,0.0});
				DrawLetterLines(letterMin, letterMax, Float:{1.0,0.0}, Float:{1.0,1.0});
				DrawLetterLines(letterMin, letterMax, Float:{0.0,1.0}, Float:{0.0,0.0});
			}
			case 'V':
			{
				DrawLetterLines(letterMin, letterMax, Float:{0.5,0.0}, Float:{1.0,1.0});
				DrawLetterLines(letterMin, letterMax, Float:{0.5,0.0}, Float:{0.0,1.0});
			}
			case 'W':
			{
				DrawLetterLines(letterMin, letterMax, Float:{0.0,1.0}, Float:{0.25,0.0});
				DrawLetterLines(letterMin, letterMax, Float:{0.25,0.0}, Float:{0.5,0.8});
				DrawLetterLines(letterMin, letterMax, Float:{0.5,0.8}, Float:{0.75,0.0});
				DrawLetterLines(letterMin, letterMax, Float:{0.75,0.0}, Float:{1.0,1.0});
			}
			case 'X':
			{
				DrawLetterLines(letterMin, letterMax, Float:{0.0,0.0}, Float:{1.0,1.0});
				DrawLetterLines(letterMin, letterMax, Float:{0.0,1.0}, Float:{1.0,0.0});
			}
			case 'Y':
			{
				DrawLetterLines(letterMin, letterMax, Float:{0.5,0.0}, Float:{0.5,0.5});
				DrawLetterLines(letterMin, letterMax, Float:{0.5,0.5}, Float:{0.0,1.0});
				DrawLetterLines(letterMin, letterMax, Float:{0.5,0.5}, Float:{1.0,1.0});
			}
			case 'Z':
			{
				DrawLetterLines(letterMin, letterMax, Float:{0.0,1.0}, Float:{1.0,1.0});
				DrawLetterLines(letterMin, letterMax, Float:{1.0,1.0}, Float:{0.0,0.0});
				DrawLetterLines(letterMin, letterMax, Float:{0.0,0.0}, Float:{1.0,0.0});
			}
			case '-':
			{
				DrawLetterLines(letterMin, letterMax, Float:{0.15,0.5}, Float:{0.85,0.5});
			}
			case ':':
			{
				DrawLetterLines(letterMin, letterMax, Float:{0.5,0.8}, Float:{0.5,0.7});
				DrawLetterLines(letterMin, letterMax, Float:{0.5,0.2}, Float:{0.5,0.3});
			}
			case '/':
			{
				DrawLetterLines(letterMin, letterMax, Float:{0.2,0.0}, Float:{0.8,1.0});
			}
			case '1':
			{
				DrawLetterLines(letterMin, letterMax, Float:{0.25,0.5}, Float:{0.75,1.0});
				DrawLetterLines(letterMin, letterMax, Float:{0.75,1.0}, Float:{0.75,0.0});
			}	
			case '2':
			{
				DrawLetterLines(letterMin, letterMax, Float:{0.0,1.0}, Float:{1.0,1.0});
				DrawLetterLines(letterMin, letterMax, Float:{1.0,1.0}, Float:{1.0,0.5});
				DrawLetterLines(letterMin, letterMax, Float:{1.0,0.5}, Float:{0.0,0.5});
				DrawLetterLines(letterMin, letterMax, Float:{0.0,0.5}, Float:{0.0,0.0});
				DrawLetterLines(letterMin, letterMax, Float:{0.0,0.0}, Float:{1.0,0.0});
			}	
			case '3':
			{
				DrawLetterLines(letterMin, letterMax, Float:{0.0,1.0}, Float:{1.0,1.0});
				DrawLetterLines(letterMin, letterMax, Float:{0.0,0.5}, Float:{1.0,0.5});
				DrawLetterLines(letterMin, letterMax, Float:{0.0,0.0}, Float:{1.0,0.0});
				DrawLetterLines(letterMin, letterMax, Float:{1.0,1.0}, Float:{1.0,0.0});
			}
			case '4':
			{
				DrawLetterLines(letterMin, letterMax, Float:{0.9,1.0}, Float:{0.0,0.5});
				DrawLetterLines(letterMin, letterMax, Float:{0.0,0.5}, Float:{1.0,0.5});
				DrawLetterLines(letterMin, letterMax, Float:{0.9,1.0}, Float:{0.9,0.0});
			}		
			case '5':
			{
				DrawLetterLines(letterMin, letterMax, Float:{1.0,1.0}, Float:{0.0,1.0});
				DrawLetterLines(letterMin, letterMax, Float:{0.0,1.0}, Float:{0.0,0.5});
				DrawLetterLines(letterMin, letterMax, Float:{0.0,0.5}, Float:{1.0,0.5});
				DrawLetterLines(letterMin, letterMax, Float:{1.0,0.5}, Float:{1.0,0.0});
				DrawLetterLines(letterMin, letterMax, Float:{0.0,0.0}, Float:{1.0,0.0});
				DrawLetterLines(letterMin, letterMax, Float:{0.0,0.0}, Float:{0.0,0.25});
			}
			case '6':
			{
				DrawLetterLines(letterMin, letterMax, Float:{0.0,0.0}, Float:{0.0,1.0});
				DrawLetterLines(letterMin, letterMax, Float:{0.0,1.0}, Float:{0.95,1.0});
				DrawLetterLines(letterMin, letterMax, Float:{0.0,0.0}, Float:{1.0,0.0});
				DrawLetterLines(letterMin, letterMax, Float:{1.0,0.0}, Float:{1.0,0.5});
				DrawLetterLines(letterMin, letterMax, Float:{1.0,0.5}, Float:{0.0,0.5});
			}
			case '7':
			{
				DrawLetterLines(letterMin, letterMax, Float:{0.0,1.0}, Float:{1.0,1.0});
				DrawLetterLines(letterMin, letterMax, Float:{1.0,1.0}, Float:{0.0,0.0});
			}	
			case '8':
			{
				DrawLetterLines(letterMin, letterMax, Float:{0.0,0.0}, Float:{1.0,0.0});
				DrawLetterLines(letterMin, letterMax, Float:{1.0,0.0}, Float:{1.0,1.0});
				DrawLetterLines(letterMin, letterMax, Float:{1.0,1.0}, Float:{0.0,1.0});
				DrawLetterLines(letterMin, letterMax, Float:{0.0,1.0}, Float:{0.0,0.0});
				DrawLetterLines(letterMin, letterMax, Float:{0.0,0.5}, Float:{1.0,0.5});
			}
			case '9':
			{
				DrawLetterLines(letterMin, letterMax, Float:{0.0,1.0}, Float:{0.0,0.5});
				DrawLetterLines(letterMin, letterMax, Float:{0.0,1.0}, Float:{1.0,1.0});
				DrawLetterLines(letterMin, letterMax, Float:{0.0,0.5}, Float:{1.0,0.5});
				DrawLetterLines(letterMin, letterMax, Float:{1.0,1.0}, Float:{1.0,0.0});
				DrawLetterLines(letterMin, letterMax, Float:{1.0,0.0}, Float:{0.0,0.0});
			}
			case '0':
			{
				DrawLetterLines(letterMin, letterMax, Float:{0.0,0.0}, Float:{1.0,0.0});
				DrawLetterLines(letterMin, letterMax, Float:{1.0,0.0}, Float:{1.0,1.0});
				DrawLetterLines(letterMin, letterMax, Float:{1.0,1.0}, Float:{0.0,1.0});
				DrawLetterLines(letterMin, letterMax, Float:{0.0,1.0}, Float:{0.0,0.0});
			}
		}
	}
	return maxLetters;
}

/**
	Draws one line of a letter indicated by start and end vectors and world min/max position to client
 */
stock DrawLetterLines(float min[3], float max[3], float lineStartNorm[2], float lineEndNorm[2]){
	float lineStart[3], lineEnd[3];
	MoveVector(min, max, lineStartNorm[0], lineStart);
	MoveVector(min, max, lineEndNorm[0], lineEnd);
	lineStart[2] += lineStartNorm[1]*DrawText_LETTER_HEIGHT*DrawText_HeightScale;
	lineEnd[2] += lineEndNorm[1]*DrawText_LETTER_HEIGHT*DrawText_HeightScale;
	TE_SetupBeamPoints(lineStart, lineEnd, DrawText_Model, -1, 0, 30, DrawText_Life, DrawText_LETTER_SIZE*DrawText_BoldnessScale, DrawText_LETTER_SIZE*DrawText_BoldnessScale, 2, 0.0, DrawText_Color, 0);
	TE_SendToClient(DrawText_DrawingTo);
}

stock MoveVector(const Float:start[3], const Float:end[3], Float:scale, Float:output[3])
{
	SubtractVectors(end,start,output);
	ScaleVector(output,scale);
	AddVectors(start,output,output);
}

/**
 * @param client
 * @param bottomCorner	One bottom corner of the box.
 * @param upperCorner	One upper corner of the box.
 * @param modelIndex	Precached model index.
 * @param haloIndex		Precached model index.
 * @param startFrame	Initital frame to render.
 * @param frameRate		Beam frame rate.
 * @param life			Time duration of the beam.
 * @param width			Initial beam width.
 * @param endWidth		Final beam width.
 * @param fadeLength	Beam fade time duration.
 * @param amplitude		Beam amplitude.
 * @param color			Color array (r, g, b, a).
 * @param speed			Speed of the beam.
 * @noreturn
 */
stock DrawBeamBox2D(
	int client,
	float min[3],
	float max[3],
	int modelIndex,
	int haloIndex,
	int startFrame=0,
	int frameRate=30,
	float life=5.0,
	float width=5.0,
	float endWidth=5.0,
	int fadeLength=2,
	float amplitude=1.0,
	int color[4]={ 136, 153, 227, 255 },
	int speed=0
) {
	float from[3], to[3];
	
	//left beam
	from[0]=min[0]; from[1]=min[1]; from[2]=min[2];
	to[0]=min[0]; to[1]=min[1]; to[2]=max[2];
	TE_SetupBeamPoints(from, to, modelIndex, haloIndex, startFrame, frameRate, life, width, endWidth, fadeLength, amplitude, color, speed);
	TE_SendToClient(client);
	//right beam
	from[0]=max[0]; from[1]=max[1]; from[2]=min[2];
	to[0]=max[0]; to[1]=max[1]; to[2]=max[2];
	TE_SetupBeamPoints(from, to, modelIndex, haloIndex, startFrame, frameRate, life, width, endWidth, fadeLength, amplitude, color, speed);
	TE_SendToClient(client);
	//bottom beam
	from[0]=min[0]; from[1]=min[1]; from[2]=min[2];
	to[0]=max[0]; to[1]=max[1]; to[2]=min[2];
	TE_SetupBeamPoints(from, to, modelIndex, haloIndex, startFrame, frameRate, life, width, endWidth, fadeLength, amplitude, color, speed);
	TE_SendToClient(client);
	//top beam
	from[0]=min[0]; from[1]=min[1]; from[2]=max[2];
	to[0]=max[0]; to[1]=max[1]; to[2]=max[2];
	TE_SetupBeamPoints(from, to, modelIndex, haloIndex, startFrame, frameRate, life, width, endWidth, fadeLength, amplitude, color, speed);
	TE_SendToClient(client);
}