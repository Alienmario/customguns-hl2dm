#if defined _CustomGuns_DrawingTools_included
	#endinput
#endif
#define _CustomGuns_DrawingTools_included

#define DRAWTEXT_LETTER_SPACING 0.25

#define DRAWTEXT_HEIGHT_DEFAULT 3.0
#define DRAWTEXT_WIDTH_DEFAULT 2.0
#define DRAWTEXT_SIZE_DEFAULT 0.4

#define DRAWTEXT_ALIGN_START -1
#define DRAWTEXT_ALIGN_CENTER 0
#define DRAWTEXT_ALIGN_END 1

int DrawText_Client;
int DrawText_Model;
int DrawText_Color[4];
float DrawText_Height;
float DrawText_Size;
float DrawText_Life;
float DrawText_Amplitude;

/**
 * Draws text in the world with beams
 *
 * @param client			client index or -1 for all
 * @param start				world position from where to start
 * @param end				world position where drawing cuts off
 * @param model				material index from PrecacheModel
 *
 * @return					number of letters drawn
 */
stock DrawText
(
	int client,
	const float start[3],
	const float end[3],
	int model,
	const char[] text,
	float life,
	int align = DRAWTEXT_ALIGN_CENTER,
	const int color[4] = { 255, 255, 255, 255 },
	float fontWidth = DRAWTEXT_WIDTH_DEFAULT,
	float fontHeight = DRAWTEXT_HEIGHT_DEFAULT,
	float fontSize = DRAWTEXT_SIZE_DEFAULT,
	float amplitude = 0.0
){
	DrawText_Client = client;
	DrawText_Model = model;
	DrawText_Color = color;
	DrawText_Height = fontHeight;
	DrawText_Size = fontSize;
	DrawText_Life = life;
	DrawText_Amplitude = amplitude;

	float distance = GetVectorDistance(start, end);
	
	// Get letter count that fits in specified space
	int letters = RoundToFloor(distance / (fontWidth + fontWidth*DRAWTEXT_LETTER_SPACING));
	
	float startOffset; // letter offset, amount of free space from start
	
	// clamp letters to text length
	int textlen = strlen(text);
	if(textlen < letters){
		// if centered, set start offset
		if (align == DRAWTEXT_ALIGN_CENTER){
			startOffset = (letters-textlen)/2.0;
		}
		letters = textlen;	
	}
	
	float letterMin[3], letterMax[3];
	for(int i = 0; i < letters; i++){
	
		//compute min and max world location for the letter
		if(align == DRAWTEXT_ALIGN_END){
			MoveVector(end, start, (((letters-1-i)*fontWidth + (letters-1-i)*DRAWTEXT_LETTER_SPACING*fontWidth) / distance), letterMax);
			MoveVector(end, start, (((letters-i)*fontWidth + (letters-i-1)*DRAWTEXT_LETTER_SPACING*fontWidth) / distance), letterMin);
		} else {
			float j = i + startOffset; //applied start offset
			MoveVector(start, end, ((j*fontWidth + j*DRAWTEXT_LETTER_SPACING*fontWidth) / distance), letterMin);
			MoveVector(start, end, (((j+1)*fontWidth + j*DRAWTEXT_LETTER_SPACING*fontWidth) / distance), letterMax);
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
	return letters;
}

/**
	Draws one line of a letter indicated by start and end vectors and world min/max position to client
 */
stock DrawLetterLines(const float min[3], const float max[3], const float lineStartNorm[2], const float lineEndNorm[2]){
	float lineStart[3], lineEnd[3];
	MoveVector(min, max, lineStartNorm[0], lineStart);
	MoveVector(min, max, lineEndNorm[0], lineEnd);
	lineStart[2] += lineStartNorm[1] * DrawText_Height;
	lineEnd[2] += lineEndNorm[1] * DrawText_Height;
	
	TE_SetupBeamPoints(lineStart, lineEnd, DrawText_Model, -1, 0, 30, DrawText_Life, DrawText_Size, DrawText_Size, 0, DrawText_Amplitude, DrawText_Color, 0);
	
	if(DrawText_Client <= 0){
		TE_SendToAll();
	} else {
		TE_SendToClient(DrawText_Client);
	}
}

stock MoveVector(const float start[3], const float end[3], float scale, float output[3])
{
	SubtractVectors(end, start, output);
	ScaleVector(output, scale);
	AddVectors(start, output, output);
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