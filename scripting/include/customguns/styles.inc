#if defined _CustomGuns_Styles_included
	#endinput
#endif
#define _CustomGuns_Styles_included

enum
{
	MENU_DISTANCE = 0,
	MENU_RADIUS,
	SELECT_THRESHOLD,
	
	MENU_CENTER_MODEL_ENABLED,
	MENU_CENTER_MODEL,
	MENU_CENTER_MODEL_SIZE,
	MENU_CENTER_MODEL_SIZE_SELECTED,
	MENU_BG_MODEL,
	MENU_BG_SPEED,
	MENU_BG_COLOR,
	MENU_BG_COLOR_SELECT,
	MENU_BG_COLOR_UNSELECTABLE,
	MENU_BG_START_WIDTH_SCALE,
	MENU_BG_END_WIDTH_SCALE,
	MENU_BG_DISTANCE_OFFSET,
	
	MENU_BG_CENTER_START_SCALE_ADD,
	MENU_BG_CENTER_START_SCALE_SELECTED_MULTIPLIER,

	MODEL_TEXT,
	TEXT_DISTANCE_OFFSET,
	
	MENU_BG_MODEL_ID,
	MENU_CENTER_MODEL_ID,
	MODEL_TEXT_ID,
	
	STYLE_NAME,
	StyleProps
}

methodmap Style < ArrayList
{
	public Style(){
		return view_as<Style>(new ArrayList(128, StyleProps));
	}
	
	/* Quickly re-tags to float */
	public float GetFl(int propertyID){
		return this.Get(propertyID);
	}
}

ArrayList Styles;
void loadStyles(){
	KeyValues kv = new KeyValues("kv");
	Styles = new ArrayList();
	
	char buffer[PLATFORM_MAX_PATH];
	BuildPath(Path_SM, buffer, sizeof(buffer), "configs/customguns_styles.txt");
	if(!kv.ImportFromFile(buffer) || !kv.GotoFirstSubKey()){
		LogError("Failed to read Styles: %s", buffer);
		Styles.Push(defaultStyle());
	}
	else
	{
		do
		{
			Style s = new Style();
			int color[4];
			if(kv.GetSectionName(buffer, sizeof(buffer)))
				s.SetString(STYLE_NAME, buffer);
			else
				s.SetString(STYLE_NAME, "Unnamed");
			
			s.Set(MENU_DISTANCE, kv.GetFloat("MENU_DISTANCE"));
			s.Set(MENU_RADIUS, kv.GetFloat("MENU_RADIUS"));
			s.Set(SELECT_THRESHOLD, kv.GetFloat("SELECT_THRESHOLD"));
			s.Set(MENU_CENTER_MODEL_ENABLED, kv.GetNum("MENU_CENTER_MODEL_ENABLED"));
			kv.GetString("MENU_CENTER_MODEL", buffer, sizeof(buffer));
			s.SetString(MENU_CENTER_MODEL, buffer);
			s.Set(MENU_CENTER_MODEL_SIZE, kv.GetFloat("MENU_CENTER_MODEL_SIZE"));
			s.Set(MENU_CENTER_MODEL_SIZE_SELECTED, kv.GetFloat("MENU_CENTER_MODEL_SIZE_SELECTED"));
			kv.GetString("MENU_BG_MODEL", buffer, sizeof(buffer));
			s.SetString(MENU_BG_MODEL, buffer);
			s.Set(MENU_BG_SPEED, kv.GetNum("MENU_BG_SPEED"));
			kv.GetColor4("MENU_BG_COLOR", color);
			s.SetArray(MENU_BG_COLOR, color);
			kv.GetColor4("MENU_BG_COLOR_SELECT", color);
			s.SetArray(MENU_BG_COLOR_SELECT, color);
			kv.GetColor4("MENU_BG_COLOR_UNSELECTABLE", color);
			s.SetArray(MENU_BG_COLOR_UNSELECTABLE, color);
			s.Set(MENU_BG_START_WIDTH_SCALE, kv.GetFloat("MENU_BG_START_WIDTH_SCALE"));
			s.Set(MENU_BG_END_WIDTH_SCALE, kv.GetFloat("MENU_BG_END_WIDTH_SCALE"));
			s.Set(MENU_BG_DISTANCE_OFFSET, kv.GetFloat("MENU_BG_DISTANCE_OFFSET"));
			s.Set(MENU_BG_CENTER_START_SCALE_ADD, kv.GetFloat("MENU_BG_CENTER_START_SCALE_ADD"));
			s.Set(MENU_BG_CENTER_START_SCALE_SELECTED_MULTIPLIER, kv.GetFloat("MENU_BG_CENTER_START_SCALE_SELECTED_MULTIPLIER"));
			kv.GetString("MODEL_TEXT", buffer, sizeof(buffer));
			s.SetString(MODEL_TEXT, buffer);
			s.Set(TEXT_DISTANCE_OFFSET, kv.GetFloat("TEXT_DISTANCE_OFFSET"));
			Styles.Push(s);
		} while (kv.GotoNextKey());
	}
	delete kv;
}

void precacheStyles(){
	for (int i = 0; i < Styles.Length; i++){
		Style s = Styles.Get(i);
		char buffer[128];
		s.GetString(MENU_BG_MODEL, buffer, sizeof(buffer));
		s.Set(MENU_BG_MODEL_ID, PrecacheModel(buffer, true));
		
		s.GetString(MODEL_TEXT, buffer, sizeof(buffer));
		s.Set(MODEL_TEXT_ID, PrecacheModel(buffer, true));
		
		if(s.Get(MENU_CENTER_MODEL_ENABLED)){
			s.GetString(MENU_CENTER_MODEL, buffer, sizeof(buffer));
			s.Set(MENU_CENTER_MODEL_ID, PrecacheModel(buffer, true));
		}
	}
}

Style defaultStyle(){
	Style style = new Style();
	style.SetString(STYLE_NAME, "Default");
	style.Set(MENU_DISTANCE, 60.0);
	style.Set(MENU_RADIUS, 40.0);
	style.Set(SELECT_THRESHOLD, 0.86);
	style.Set(MENU_CENTER_MODEL_ENABLED, 0);
	style.SetString(MENU_CENTER_MODEL, "");
	style.Set(MENU_CENTER_MODEL_SIZE, 0.1);
	style.Set(MENU_CENTER_MODEL_SIZE_SELECTED, 0.2);
	style.SetString(MENU_BG_MODEL, "materials/effects/laserplane.vmt");
	style.Set(MENU_BG_SPEED, 8);
	style.SetArray(MENU_BG_COLOR, {20, 150, 255, 255});
	style.SetArray(MENU_BG_COLOR_SELECT, {150, 255, 255, 255});
	style.SetArray(MENU_BG_COLOR_UNSELECTABLE, {255, 150, 50, 50});
	style.Set(MENU_BG_START_WIDTH_SCALE, 0.5);
	style.Set(MENU_BG_END_WIDTH_SCALE, 0.6);
	style.Set(MENU_BG_DISTANCE_OFFSET, -25.0);
	style.Set(MENU_BG_CENTER_START_SCALE_ADD, 0.25);
	style.Set(MENU_BG_CENTER_START_SCALE_SELECTED_MULTIPLIER, 0.0);
	style.SetString(MODEL_TEXT, "materials/sprites/lgtning_noz.vmt");
	style.Set(TEXT_DISTANCE_OFFSET, -25.0);
	return style;
}

Style GetClientStyle(int client){
	return Styles.Get(menuStyle[client]);
}

void ReloadStyle(int client){
	char buffer[128], buffer2[128];
	GetClientCookie(client, cookie_menu_style, buffer, sizeof(buffer));
	for (int i = 0; i < Styles.Length; i++){
		Style s = Styles.Get(i);
		s.GetString(STYLE_NAME, buffer2, sizeof(buffer2));
		if(StrEqual(buffer, buffer2, false)){
			menuStyle[client] = i;
			return;
		}
	}
}

public Action ShowStyleMenu(int client, int args){
	Menu menu = new Menu(StyleMenuHandler);
	menu.SetTitle("Choose your style");
	
	char buffer[128];
	for (int i = 0; i < Styles.Length; i++){
		Style s = Styles.Get(i);
		s.GetString(STYLE_NAME, buffer, sizeof(buffer));
		menu.AddItem(buffer, buffer);
	}
	menu.Display(client, 60);
	return Plugin_Handled;
}

public int StyleMenuHandler(Menu menu, MenuAction action, int param1, int param2)
{
	if (action == MenuAction_Select)
	{
		char info[128];
		bool found = menu.GetItem(param2, info, sizeof(info));
		if(found && AreClientCookiesCached(param1))
		{
			SetClientCookie(param1, cookie_menu_style, info);
			ReloadStyle(param1);
		}
	}
	else if (action == MenuAction_End)
	{
		delete menu;
	}
}