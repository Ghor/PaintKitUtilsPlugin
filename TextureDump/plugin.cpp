//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//
//===========================================================================//

#include <stdio.h>

//#define GAME_DLL
#ifdef GAME_DLL
#include "cbase.h"
#endif

#include <stdio.h>
#include "interface.h"
#include "filesystem.h"
#include "engine/iserverplugin.h"
#include "eiface.h"
#include "igameevents.h"
#include "convar.h"
#include "engine/IEngineTrace.h"
#include "tier2/tier2.h"
#include "game/server/pluginvariant.h"
#include "game/server/iplayerinfo.h"
#include "game/server/ientityinfo.h"
#include "game/server/igameinfo.h"

#include "iclient.h"

#include "materialsystem/imaterialsystem.h"
#include "materialsystem/itexture.h"

#include "TextureUtils.h"
#include "CompositorQueue.h"
#include "CProtoBufScriptObjectDefinitionManager.h"
#include "CPaintKitDefinition.h"
#include "CPaintKitItemDefinition.h"

#include <typeinfo>

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

// Interfaces from the engine
IVEngineServer* engine = NULL; // helper functions (messaging clients, loading content, making entities, running commands, etc)
IEngineTrace* enginetrace = NULL;

// useful helper func
#ifndef GAME_DLL
inline bool FStrEq(const char* sz1, const char* sz2)
{
	return(Q_stricmp(sz1, sz2) == 0);
}
#endif
//---------------------------------------------------------------------------------
// Purpose: a sample 3rd party plugin class
//---------------------------------------------------------------------------------
class CTextureDumpPlugin : public IServerPluginCallbacks
{
public:
	CTextureDumpPlugin();
	~CTextureDumpPlugin();

	// IServerPluginCallbacks methods
	virtual bool			Load(CreateInterfaceFn interfaceFactory, CreateInterfaceFn gameServerFactory);
	virtual void			Unload(void);
	virtual void			Pause(void);
	virtual void			UnPause(void);
	virtual const char* GetPluginDescription(void);
	virtual void			LevelInit(char const* pMapName);
	virtual void			ServerActivate(edict_t* pEdictList, int edictCount, int clientMax);
	virtual void			GameFrame(bool simulating);
	virtual void			LevelShutdown(void);
	virtual void			ClientActive(edict_t* pEntity);
	virtual void			ClientDisconnect(edict_t* pEntity);
	virtual void			ClientPutInServer(edict_t* pEntity, char const* playername);
	virtual void			SetCommandClient(int index);
	virtual void			ClientSettingsChanged(edict_t* pEdict);
	virtual PLUGIN_RESULT	ClientConnect(bool* bAllowConnect, edict_t* pEntity, const char* pszName, const char* pszAddress, char* reject, int maxrejectlen);
	virtual PLUGIN_RESULT	ClientCommand(edict_t* pEntity, const CCommand& args);
	virtual PLUGIN_RESULT	NetworkIDValidated(const char* pszUserName, const char* pszNetworkID);
	virtual void			OnQueryCvarValueFinished(QueryCvarCookie_t iCookie, edict_t* pPlayerEntity, EQueryCvarValueStatus eStatus, const char* pCvarName, const char* pCvarValue);
	virtual void			OnEdictAllocated(edict_t* edict);
	virtual void			OnEdictFreed(const edict_t* edict);

	virtual int GetCommandIndex() { return m_iClientCommandIndex; }
private:
	int m_iClientCommandIndex;
};


// 
// The plugin is a static singleton that is exported as an interface
//
CTextureDumpPlugin g_TextureDumpPlugin;
EXPOSE_SINGLE_INTERFACE_GLOBALVAR(CTextureDumpPlugin, IServerPluginCallbacks, INTERFACEVERSION_ISERVERPLUGINCALLBACKS, g_TextureDumpPlugin);

CompositorQueue* g_pCompositorQueue = NULL;

//---------------------------------------------------------------------------------
// Purpose: constructor/destructor
//---------------------------------------------------------------------------------
CTextureDumpPlugin::CTextureDumpPlugin()
{
	m_iClientCommandIndex = 0;
}

CTextureDumpPlugin::~CTextureDumpPlugin()
{
}

//---------------------------------------------------------------------------------
// Purpose: called when the plugin is loaded, load the interface we need from the engine
//---------------------------------------------------------------------------------
bool CTextureDumpPlugin::Load(CreateInterfaceFn interfaceFactory, CreateInterfaceFn gameServerFactory)
{
	ConnectTier1Libraries(&interfaceFactory, 1);
	ConnectTier2Libraries(&interfaceFactory, 1);

	engine = (IVEngineServer*)interfaceFactory(INTERFACEVERSION_VENGINESERVER, NULL);
	enginetrace = (IEngineTrace*)interfaceFactory(INTERFACEVERSION_ENGINETRACE_SERVER, NULL);

	// get the interfaces we want to use
	if (!(engine && enginetrace && materials))
	{
		return false; // we require all these interface to function
	}

	ConVar_Register(0);

	g_pCompositorQueue = new CompositorQueue();

	return true;
}

//---------------------------------------------------------------------------------
// Purpose: called when the plugin is unloaded (turned off)
//---------------------------------------------------------------------------------
void CTextureDumpPlugin::Unload(void)
{
	delete g_pCompositorQueue;
	g_pCompositorQueue = NULL;

	ConVar_Unregister();
	DisconnectTier2Libraries();
	DisconnectTier1Libraries();
}

//---------------------------------------------------------------------------------
// Purpose: called when the plugin is paused (i.e should stop running but isn't unloaded)
//---------------------------------------------------------------------------------
void CTextureDumpPlugin::Pause(void)
{
}

//---------------------------------------------------------------------------------
// Purpose: called when the plugin is unpaused (i.e should start executing again)
//---------------------------------------------------------------------------------
void CTextureDumpPlugin::UnPause(void)
{
}

//---------------------------------------------------------------------------------
// Purpose: the name of this plugin, returned in "plugin_print" command
//---------------------------------------------------------------------------------
const char* CTextureDumpPlugin::GetPluginDescription(void)
{
	return "TextureDump Plugin V0, Jeffrey \"Ghor\" Hunt";
}

//---------------------------------------------------------------------------------
// Purpose: called on level start
//---------------------------------------------------------------------------------
void CTextureDumpPlugin::LevelInit(char const* pMapName)
{
}

//---------------------------------------------------------------------------------
// Purpose: called on level start, when the server is ready to accept client connections
//		edictCount is the number of entities in the level, clientMax is the max client count
//---------------------------------------------------------------------------------
void CTextureDumpPlugin::ServerActivate(edict_t* pEdictList, int edictCount, int clientMax)
{
}

//---------------------------------------------------------------------------------
// Purpose: called once per server frame, do recurring work here (like checking for timeouts)
//---------------------------------------------------------------------------------
void CTextureDumpPlugin::GameFrame(bool simulating)
{
	if (g_pCompositorQueue)
	{
		g_pCompositorQueue->Update();
	}
}

//---------------------------------------------------------------------------------
// Purpose: called on level end (as the server is shutting down or going to a new map)
//---------------------------------------------------------------------------------
void CTextureDumpPlugin::LevelShutdown(void) // !!!!this can get called multiple times per map change
{
}

//---------------------------------------------------------------------------------
// Purpose: called when a client spawns into a server (i.e as they begin to play)
//---------------------------------------------------------------------------------
void CTextureDumpPlugin::ClientActive(edict_t* pEntity)
{
}

//---------------------------------------------------------------------------------
// Purpose: called when a client leaves a server (or is timed out)
//---------------------------------------------------------------------------------
void CTextureDumpPlugin::ClientDisconnect(edict_t* pEntity)
{
}

//---------------------------------------------------------------------------------
// Purpose: called on 
//---------------------------------------------------------------------------------
void CTextureDumpPlugin::ClientPutInServer(edict_t* pEntity, char const* playername)
{
}

//---------------------------------------------------------------------------------
// Purpose: called on level start
//---------------------------------------------------------------------------------
void CTextureDumpPlugin::SetCommandClient(int index)
{
	m_iClientCommandIndex = index;
}

void ClientPrint(edict_t* pEdict, char* format, ...)
{
	va_list		argptr;
	static char		string[1024];

	va_start(argptr, format);
	Q_vsnprintf(string, sizeof(string), format, argptr);
	va_end(argptr);

	engine->ClientPrintf(pEdict, string);
}
//---------------------------------------------------------------------------------
// Purpose: called on level start
//---------------------------------------------------------------------------------
void CTextureDumpPlugin::ClientSettingsChanged(edict_t* pEdict)
{
}

//---------------------------------------------------------------------------------
// Purpose: called when a client joins a server
//---------------------------------------------------------------------------------
PLUGIN_RESULT CTextureDumpPlugin::ClientConnect(bool* bAllowConnect, edict_t* pEntity, const char* pszName, const char* pszAddress, char* reject, int maxrejectlen)
{
	return PLUGIN_CONTINUE;
}

//---------------------------------------------------------------------------------
// Purpose: called when a client types in a command (only a subset of commands however, not CON_COMMAND's)
//---------------------------------------------------------------------------------
PLUGIN_RESULT CTextureDumpPlugin::ClientCommand(edict_t* pEntity, const CCommand& args)
{
	return PLUGIN_CONTINUE;
}

//---------------------------------------------------------------------------------
// Purpose: called when a client is authenticated
//---------------------------------------------------------------------------------
PLUGIN_RESULT CTextureDumpPlugin::NetworkIDValidated(const char* pszUserName, const char* pszNetworkID)
{
	return PLUGIN_CONTINUE;
}

//---------------------------------------------------------------------------------
// Purpose: called when a cvar value query is finished
//---------------------------------------------------------------------------------
void CTextureDumpPlugin::OnQueryCvarValueFinished(QueryCvarCookie_t iCookie, edict_t* pPlayerEntity, EQueryCvarValueStatus eStatus, const char* pCvarName, const char* pCvarValue)
{
}
void CTextureDumpPlugin::OnEdictAllocated(edict_t* edict)
{
}
void CTextureDumpPlugin::OnEdictFreed(const edict_t* edict)
{
}

//---------------------------------------------------------------------------------
// Purpose: an example of how to implement a new command
//---------------------------------------------------------------------------------

CON_COMMAND(texture_dump, "Dumps the contents of the named texture to disk.")
{
	if (args.ArgC() < 2)
	{
		Warning("texture_dump <texture_name> [texture_group_name]\n");
	}
	else
	{
		const char* texture_name = args.Arg(1);
		const char* texture_group_name = (args.ArgC() >= 2) ? args.Arg(2) : NULL;

		ITexture* texture = NULL;
		if (texture_group_name == NULL)
		{
			int num_found = 0;
			FindTextureAllGroups(texture_name, &num_found, &texture);
			if (num_found > 0)
			{
				Warning("Texture appears in more than one group.\n");
			}
		}
		else
		{
			texture = materials->FindTexture(texture_name, texture_group_name);
		}

		if ((texture == NULL) || (texture->IsError()))
		{
			Warning("could not find specified texture.\n");
		}
		else
		{
			Msg("Found texture. animation_frames=%d\n", texture->GetNumAnimationFrames());

			size_t output_file_name_buffer_len;
			char* output_file_name_buffer;
			{
				const char* prefix = "dump/";
				const char* suffix = ".tga";

				const char* last_slash = strrchr(texture_name, '/');
				const char* last_backslash = strrchr(texture_name, '\\');

				const char* filename_start = Max(last_slash, last_backslash);

				if (filename_start != NULL)
				{
					++filename_start; // Advance over the slash.
				}
				else
				{
					filename_start = texture_name; // Just use the whole string.
				}

				output_file_name_buffer_len = strlen(prefix) + strlen(filename_start) + strlen(suffix) + 1;
				output_file_name_buffer = new char[output_file_name_buffer_len];
				snprintf(output_file_name_buffer, output_file_name_buffer_len, "%s%s%s", prefix, filename_start, suffix);
			}

			try {
				// Unfortunately, we can't just use ITexture::SaveToFile, since it seems to fail for DXT5 textures (which are most textures in the game!)
				SaveTextureToDisk(texture, output_file_name_buffer);
			}
			catch (std::exception e)
			{
				Msg("Encountered exception: %s\n", e.what());
			}
			delete[] output_file_name_buffer;
			Msg("Command complete.\n");
		}
	}
}

const char* paintkitOutputTextureHelp = "Queues up a paintkit texture to be generated and then dumped to disk. Will not produce results unless you are in a map.\n"
"\n"
"Example:\n"
"> paintkit_output_texture -paintKitDefName \"workshop_simple_spirits\" -itemDefName \"scattergun\" -wear 3 -team 0 -seed 1234567890 -width 2048 -height 2048 -outputDir \"dump\" -outputName \"simple_spirits_scattergun_1_0_123456790\".\n"
"\n"
"Parameters:\n"
" > \"-paintKitDefName\" is the name of the paint kit to use (e.g. \"workshop_simple_spirits\" for Simple Spirits). You can use \"paintKitDefIndex\" instead.\n"
" > \"-paintKitDefIndex\" is the index of the paint kit to use (e.g. 290 for Simple Spirits). You can use \"paintKitDefName\" instead.\n"
" > \"-itemDefName\" is the name of the item to use (e.g. \"scattergun\" for the Scattergun). You can use \"itemDefIndex\" instead.\n"
" > \"-itemDefIndex\" is the index of the item to use (e.g. 200 for the Scattergun). You can use \"itemDefName\" instead.\n"
" > \"-wear\" is the wear level of the weapon in the range [1,5], with 1 being Factory New and 5 being Battle Scarred. Optional. Defaults to 1.\n"
" > \"-team\" is the team to use for multi-team paintkits. It should be in the range [0, 1]. Optional. Defaults to 0.\n"
" > \"-seed\" is the 64-bit seed to use. You *can* use 0, but you probably shouldn't. \n"
" > \"-width\" is the width of the texture to be generated. Optional. Defaults to 2048, the largest the game will use.\n"
" > \"-height\" is the height of the texture to be generated. Optional. Defaults to 2048, the largest the game will use.\n"
" > \"-outputDir\" is the directory to save the generated file to. Optional. Defaults to \"dump\".\n"
" > \"-outputName\" is the name to use for the generated file, without a file extension. Optional. Defaults to the engine's internal name for the generated texture.\n"
;
CON_COMMAND(paintkit_output_texture, paintkitOutputTextureHelp)
{
	CompositorQueue::CompositorRequest request;
	request.paintKitDefIndex = args.FindArgInt("-paintKitDefIndex", -1);
	request.paintKitDefName = args.FindArg("-paintKitDefName");
	request.itemDefIndex = args.FindArgInt("-itemDefIndex", -1);
	request.itemDefName = args.FindArg("-itemDefName");
	request.wear = args.FindArgInt("-wear", 1);
	request.team = args.FindArgInt("-team", 0);
	const char* seedStr = args.FindArg("-seed");
	request.seed = seedStr ? atoll(seedStr) : 0;
	request.width = args.FindArgInt("-width", 2048);
	request.height = args.FindArgInt("-height", 2048);
	request.outputDir = args.FindArg("-outputDir");
	request.outputName = args.FindArg("-outputName");


	auto* scriptDefinitionManager = ProtoBufScriptDefinitionManager();
	if (!scriptDefinitionManager)
	{
		Warning("Script definition manager unavailable.\n");
		return;
	}

	if ((request.paintKitDefIndex == -1) && request.paintKitDefName.IsEmpty())
	{
		Warning("Must specify either -paintKitDefIndex or -paintKitDefName.\n");
		return;
	}
	else if ((request.paintKitDefIndex != -1) && !request.paintKitDefName.IsEmpty())
	{
		Warning("Cannot specify both -paintKitDefIndex and -paintKitDefName.\n");
		return;
	}
	else if (request.paintKitDefIndex != -1)
	{
		if (!(request.paintKitDef = ProtoBufScriptDefinitionManager()->FindPaintKitDefinition(request.paintKitDefIndex)))
		{
			Warning("Can't find paintkit with defindex %d.\n", request.paintKitDefIndex);
			return;
		}
	}
	else if (!request.paintKitDefName.IsEmpty())
	{
		if (!(request.paintKitDef = ProtoBufScriptDefinitionManager()->FindPaintKitDefinition(request.paintKitDefName)))
		{
			Warning("Can't find paintkit with name %s.\n", request.paintKitDefName.Get());
			return;
		}
	}

	if ((request.itemDefIndex == -1) && request.itemDefName.IsEmpty())
	{
		Warning("Must specify either -itemDefIndex or -itemDefName.\n");
		return;
	}
	else if ((request.itemDefIndex != -1) && !request.itemDefName.IsEmpty())
	{
		Warning("Cannot specify both -itemDefIndex and -itemDefName.\n");
		return;
	}
	else if (request.itemDefIndex != -1)
	{
		if (!(request.itemDef = ProtoBufScriptDefinitionManager()->FindPaintKitItemDefinition(request.itemDefIndex)))
		{
			Warning("Can't find item with defindex %d.\n", request.itemDefIndex);
			return;
		}
	}
	else if (!request.itemDefName.IsEmpty())
	{
		if (!(request.itemDef = ProtoBufScriptDefinitionManager()->FindPaintKitItemDefinition(request.itemDefName)))
		{
			Warning("Can't find item with name %s.\n", request.itemDefName.Get());
			return;
		}
	}
	
	if ((request.wear < 1) || (request.wear > 5))
	{
		Warning("-wear must be in the range [1, 5]. Clamping.\n");
	}

	if ((request.team < 0) || (request.team > 1))
	{
		Warning("-team must be in the range [0, 1]. Clamping.\n");
	}

	if (seedStr == NULL)
	{
		Warning("Must specify a seed.\n");
		return;
	}

	if (request.width <= 0)
	{
		Warning("width must be positive.\n");
		return;
	}

	if (request.height <= 0)
	{
		Warning("height must be positive.\n");
		return;
	}

	g_pCompositorQueue->EnqueueRequest(request);
}

CON_COMMAND(paintkit_generate_kvs, "Generates the KeyValues of a paintkit.")
{
	const char* paintKitDefName = args.FindArg("-paintKitDefName");
	const char* itemDefName = args.FindArg("-paintKitItemDefName");
	int wear = args.FindArgInt("-wear", 1);

	paintKitDefName = paintKitDefName ? paintKitDefName : "";
	itemDefName = itemDefName ? itemDefName : "";

	const CPaintKitDefinition* paintKitDef = NULL;
	if (!(paintKitDef = ProtoBufScriptDefinitionManager()->FindPaintKitDefinition(paintKitDefName)))
	{
		Warning("Can't find paintkit with name %s.\n", paintKitDefName);
		return;
	}

	const CPaintKitItemDefinition* itemDef = NULL;
	if (!(itemDef = ProtoBufScriptDefinitionManager()->FindPaintKitItemDefinition(itemDefName)))
	{
		Warning("Can't find item with name %s.\n", itemDefName);
		return;
	}


	auto* message = itemDef->GetMessage();
	
	// Msg("%d\n", message->item_definition_index());

	auto kvs = paintKitDef->GetStageDescKeyValues(message->item_definition_index(), wear);
	if (kvs)
	{
		KeyValuesDumpAsDevMsg(kvs, 1);
		Msg("Generated kvs.\n");
	}
	else
	{
		Msg("Failed to generate kvs.\n");
	}

}

CON_COMMAND(paintkit_list, "Lists all paint kits in the game.")
{
	const auto& defMap = ProtoBufScriptDefinitionManager()->GetDefMap<DEF_TYPE_PAINTKIT_DEFINITION>();
	Msg("defMap.Count()=%d\n", defMap.Count());
	for (auto i = defMap.FirstInorder(); defMap.IsValidIndex(i); i = defMap.NextInorder(i))
	{
		// auto key = defMap.Key(i);
		auto protoBufScriptObjectDef = defMap.Element(i);
		auto* msg = protoBufScriptObjectDef->GetMessage();

		auto& header = msg->header();
		Msg("{ defindex=%d name=\"%s\" }\n", header.defindex(), header.name().c_str());
	}
}

CON_COMMAND(paintkit_item_list, "Lists all paintable items in the game.")
{
	const auto& defMap = ProtoBufScriptDefinitionManager()->GetDefMap<DEF_TYPE_PAINTKIT_ITEM_DEFINITION>();
	Msg("defMap.Count()=%d\n", defMap.Count());
	for (auto i = defMap.FirstInorder(); defMap.IsValidIndex(i); i = defMap.NextInorder(i))
	{
		auto key = defMap.Key(i);
		auto protoBufScriptObjectDef = defMap.Element(i);
		auto* msg = protoBufScriptObjectDef->GetMessage();
		
		auto& header = msg->header();
		Msg("{ defindex=%d name=\"%s\" item_definition_index=%d }\n", header.defindex(), header.name().c_str(), msg->item_definition_index());
	}
}
