#if defined _INC_SAMP_LANG_
	#endinput
#endif 
#define _INC_SAMP_LANG_	



#include <a_samp>
#include <pawnplus>

forward OnPlayerSampLangChange(playerid, newlang, oldlang);


//
//	Defines & Variables
//


#include <YSI_Coding\y_hooks>


#if !defined isnull
	#define isnull(%0) ((%0[(%0[0])=='\1'])=='\0')
#endif 


#define _Lang_IsValid(%0)		(-1 < (%0) < sampLangCount)	


#if !defined MAX_SAMP_LANGS
	#define MAX_SAMP_LANGS 			2
#endif


#if !defined MAX_SAMP_LANG_NAME
	#define MAX_SAMP_LANG_NAME 		16
#endif


#if !defined MAX_SAMP_LANG_CODE
	#define MAX_SAMP_LANG_CODE 		2
#endif


#if !defined MAX_SAMP_LANG_STRING
	#define MAX_SAMP_LANG_STRING 	144
#endif


#if !defined MAX_SAMP_LANG_VAR
	#define MAX_SAMP_LANG_VAR 		16
#endif


#define INVALID_SAMP_LANG 			-1



static sampLangDefault;
static sampLangCount;
static sampLangName[MAX_SAMP_LANGS][MAX_SAMP_LANG_NAME];
static sampLangCode[MAX_SAMP_LANGS][MAX_SAMP_LANG_CODE];
static Map:sampLanguages[MAX_SAMP_LANGS];

static playerSampLang[MAX_PLAYERS];


//
//	Callbacks
//


public OnPlayerConnect(playerid)
{
	playerSampLang[playerid] = sampLangDefault;
	
	#if defined SampLang_OnPlayerConnect
		return SampLang_OnPlayerConnect(playerid);
	#else
		return 1;
	#endif
}


public OnPlayerDisconnect(playerid, reason)
{
	playerSampLang[playerid] = sampLangDefault;

	#if defined SampLang_OnPlayerDisconnect
		return SampLang_OnPlayerDisconnect(playerid, reason);
	#else
		return 1;
	#endif
}


//
//	Functions
//


stock Lang_Add(const code[], const name[])
{
	if(isnull(code) || isnull(name))
	{
		print("Samp-Language: Invalid language code or name");
		return INVALID_SAMP_LANG;
	}

	if(sampLangCount + 1 > MAX_SAMP_LANGS)
	{
		print("Samp-Language: Languages reached your limit of "#MAX_SAMP_LANGS"");
		return INVALID_SAMP_LANG;
	}

	for(new i = sampLangCount; i > -1; --i) {
		if(!strcmp(sampLangCode[i], code)) {
			printf("Samp-Language: code '%s' already defined.", code);
			return INVALID_SAMP_LANG;
		}

		else if(!strcmp(sampLangName[i], name)) {
			printf("Samp-Language: language name '%s' already defined.", name);
			return INVALID_SAMP_LANG;
		}		
	}

	Lang_strcpy(sampLangCode[sampLangCount], code, MAX_SAMP_LANG_CODE);
	Lang_strcpy(sampLangName[sampLangCount], name, MAX_SAMP_LANG_NAME);

	sampLanguages[sampLangCount] = map_new();
	return sampLangCount++;
}


stock Lang_AddText(langid, const var[], const string[], var_size = sizeof(var))
{
	if(_Lang_IsValid(langid))
	{
		if(map_valid(sampLanguages[langid]))
		{
			if(map_has_arr_key(sampLanguages[langid], var, var_size))
			{
				printf("Samp-lang: [Lang_AddText] var name '%s' duplicated.", var);
				return;
			}
			map_arr_add_str(sampLanguages[langid], var, string, var_size);	
		}
	}
}


stock Lang_IsValid(langid)
	return _Lang_IsValid(langid);


stock Lang_SetDefaultLang(langid)
{
	if(_Lang_IsValid(langid))
	{
		sampLangDefault = langid;
	}
}


stock Lang_SetPlayerLang(playerid, langid)
{
	if(-1 < playerid < MAX_PLAYERS && _Lang_IsValid(langid))
	{
		new tmp = playerSampLang[playerid];
		playerSampLang[playerid] = langid;

		if(tmp != langid)
			CallRemoteFunction("OnPlayerSampLangChange", "ddd", playerid, langid, tmp);
	}
}


stock Lang_GetPlayerLang(playerid)
{
	if(-1 < playerid < MAX_PLAYERS)
		return playerSampLang[playerid];

	return sampLangDefault;	
}


stock Lang_GetText(langid, var[], string[], const string_size = sizeof(string), const var_size = sizeof(var))
{
	if(_Lang_IsValid(langid))
	{
		if(map_has_arr_key(sampLanguages[langid], var, var_size))
		{
			static message[MAX_SAMP_LANG_STRING];
			map_arr_get_arr(sampLanguages[langid], var, message, sizeof(message), var_size);

			string[0] = EOS;
			strcat(string, message, string_size);
		}
	}
}


stock Lang_Send(playerid, color, var[], GLOBAL_TAG_TYPES:...)
{
	if(-1 < playerid < MAX_PLAYERS)
	{
		static string[MAX_SAMP_LANG_STRING];
		Lang_GetText(playerSampLang[playerid], var, string, .var_size = strlen(var) + 1);
		format(string, sizeof string, string, ___(2));
		SendClientMessage(playerid, color, string);
		return 1;
	}

	return 0;
}


stock Lang_SendAll(color, var[], GLOBAL_TAG_TYPES:...)
{
	static string[MAX_SAMP_LANG_STRING];


	return 1;
}


stock SampLang_ShowPlayerDialog(playerid, dialogid, style, const caption[], const string[], const button1[], const button2[])
{
	static 
		var[MAX_SAMP_LANG_VAR],
		str_caption[MAX_SAMP_LANG_STRING],
		str_button1[MAX_SAMP_LANG_STRING],
		str_button2[MAX_SAMP_LANG_STRING],
		str_info[MAX_SAMP_LANG_STRING * 25],
		size;


	if(Lang_IsLanguageVar(caption, var, size))
		Lang_GetText(playerSampLang[playerid], var, str_caption, .var_size = size);
	else 
		Lang_strcpy(str_caption, caption);


	if(Lang_IsLanguageVar(string, var, size))
		Lang_GetText(playerSampLang[playerid], var, str_info, .var_size = size);
	else 
		Lang_strcpy(str_info, string);		


	if(Lang_IsLanguageVar(button1, var, size))
		Lang_GetText(playerSampLang[playerid], var, str_button1, .var_size = size);
	else 	
		Lang_strcpy(str_button1, button1);


	if(!isnull(button2))
	{	
		if(Lang_IsLanguageVar(button2, var, size))
			Lang_GetText(playerSampLang[playerid], var, str_button2, .var_size = size);
		else 	
			Lang_strcpy(str_button2, button2);					
	}
	else 
	{
		str_button2[0] = EOS;
	}

	return ShowPlayerDialog(playerid, dialogid, style, str_caption, str_info, str_button1, str_button2);
}


//
//	Internal
//


static stock Lang_IsLanguageVar(const string[], key[MAX_SAMP_LANG_VAR], &size)
{

	new idx = strfind(string, "$");
	if(idx == -1)
		return 0;


	strcat((key[0] = EOS, key), string);
	strmid(key, key, idx + 1, sizeof key, sizeof key);
	size = strlen(key) + 1;
	return 1;
}

static stock Lang_strcpy(result[], const source[], const size = sizeof(result))
{
	result[0] = 0;
	return strcat(result, source, size);
}


//
//	Hooks
//


#if defined _ALS_OnPlayerConnect
	#undef OnPlayerConnect
#else
	#define _ALS_OnPlayerConnect
#endif

#define OnPlayerConnect SampLang_OnPlayerConnect
#if defined SampLang_OnPlayerConnect
	forward SampLang_OnPlayerConnect(playerid);
#endif


#if defined _ALS_OnPlayerDisconnect
	#undef OnPlayerDisconnect
#else
	#define _ALS_OnPlayerDisconnect
#endif

#define OnPlayerDisconnect SampLang_OnPlayerDisconnect
#if defined SampLang_OnPlayerDisconnect
	forward SampLang_OnPlayerDisconnect(playerid, reason);
#endif


#if defined _ALS_ShowPlayerDialog
	#undef ShowPlayerDialog
#else
	#define _ALS_ShowPlayerDialog
#endif

#define ShowPlayerDialog SampLang_ShowPlayerDialog	


#undef _Lang_IsValid	