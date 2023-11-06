#include "convar.h"
#include "materialsystem/itexture.h"
#include "materialsystem/imaterialsystem.h"
#include "TextureUtils.h"
#include "CompositorQueue.h"
#include "CProtoBufScriptObjectDefinitionManager.h"
#include "KeyValues.h"
#include "CPaintKitDefinition.h"
#include "CPaintKitItemDefinition.h"
#include "CmdArgOneOf.h" // For concommand args.
#include "filesystem.h" // For g_pFullFileSystem
#include <stdexcept>

bool GetPaintKitDefFromName(const char* inputStr, const CPaintKitDefinition** result)
{
	*result = ProtoBufScriptDefinitionManager()->FindPaintKitDefinition(inputStr);
	if (*result == NULL)
	{
		CUtlString msg;
		msg.Format("Could not find paintkit with header.name==%s", inputStr);
		throw std::runtime_error(msg);
	}
	return true;
}

bool GetPaintKitDefFromIndex(const char* inputStr, const CPaintKitDefinition** result)
{
	int defIndex = atoi(inputStr);
	*result = ProtoBufScriptDefinitionManager()->FindPaintKitDefinition(defIndex);

	if (*result == NULL)
	{
		CUtlString msg;
		msg.Format("Could not find paintkit with header.defindex==%s", inputStr);
		throw std::runtime_error(msg);
	}
	return true;
}

CmdArgOneOf<const CPaintKitDefinition*> paintKitDefArgOneOf({
	{
		"-paintKitDefIndex",
		GetPaintKitDefFromIndex
	},
	{
		"-paintKitDefName",
		GetPaintKitDefFromName
	}
});

bool GetPaintKitItemDefFromName(const char* inputStr, const CPaintKitItemDefinition** result)
{
	*result = ProtoBufScriptDefinitionManager()->FindPaintKitItemDefinition(inputStr);
	if (*result == NULL)
	{
		CUtlString msg;
		msg.Format("Could not find paintkit item with header.name==%s", inputStr);
		throw std::runtime_error(msg);
	}
	return true;
}

bool GetPaintKitItemDefFromIndex(const char* inputStr, const CPaintKitItemDefinition** result)
{
	int defIndex = atoi(inputStr);
	*result = ProtoBufScriptDefinitionManager()->FindPaintKitItemDefinition(defIndex);
	if (*result == NULL)
	{
		CUtlString msg;
		msg.Format("Could not find paintkit item with header.defindex==%d", defIndex);
		throw std::runtime_error(msg);
	}
	return true;
}

bool GetPaintKitItemFromItemIndex(const char* inputStr, const CPaintKitItemDefinition** result)
{
	*result = NULL;

	int itemIndex = atoi(inputStr);

	const auto& map = ProtoBufScriptDefinitionManager()->GetDefMap<CPaintKitItemDefinition>();
	for (auto i = map.FirstInorder(); map.IsValidIndex(i); i = map.NextInorder(i))
	{
		const auto& current = map.Element(i);
		if (current->GetMessage()->item_definition_index() == itemIndex)
		{
			*result = current;
			return true;
		}
	}

	CUtlString msg;
	msg.Format("Could not find paintkit item with item_def_index==%d", itemIndex);
	throw std::runtime_error(msg);
}

CmdArgOneOf<const CPaintKitItemDefinition*> paintKitItemDefArgOneOf({
	{
		"-paintKitItemDefIndex",
		GetPaintKitItemDefFromIndex
	},
	{
		"-paintKitItemDefName",
		GetPaintKitItemDefFromName
	},
	{
		"-paintKitItemDefItemIndex",
		GetPaintKitItemFromItemIndex
	}
});

CON_COMMAND(pku_texture_dump, "Dumps the contents of the named texture to disk.")
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
"> paintkit_output_texture -paintKitDefName \"workshop_simple_spirits\" -itemDefName \"scattergun\" -wear 3 -team 0 -seed 1234567890 -width 2048 -height 2048 -outputDir \"dump\" -outputName \"simple_spirits_scattergun_1_0_123456790\"\n"
"\n"
"Parameters:\n"
" > \"-paintKitDefName\" is the name of the paint kit to use (e.g. \"workshop_simple_spirits\" for Simple Spirits). You can use \"paintKitDefIndex\" instead.\n"
" > \"-paintKitDefIndex\" is the index of the paint kit to use (e.g. 290 for Simple Spirits). You can use \"paintKitDefName\" instead.\n"
" > \"-paintKitItemDefName\" is the name of the item to use (e.g. \"scattergun\" for the Scattergun). You can use \"itemDefIndex\" instead.\n"
" > \"-paintKitItemDefIndex\" is the index of the item to use (e.g. 200 for the Scattergun). You can use \"itemDefName\" instead.\n"
" > \"-wear\" is the wear level of the weapon in the range [1,5], with 1 being Factory New and 5 being Battle Scarred. Optional. Defaults to 1.\n"
" > \"-team\" is the team to use for multi-team paintkits. It should be in the range [0, 1]. Optional. Defaults to 0.\n"
" > \"-seed\" is the 64-bit seed to use. You *can* use 0, but you probably shouldn't. \n"
" > \"-width\" is the width of the texture to be generated. Optional. Defaults to 2048, the largest the game will use.\n"
" > \"-height\" is the height of the texture to be generated. Optional. Defaults to 2048, the largest the game will use.\n"
" > \"-outputDir\" is the directory to save the generated file to. Optional. Defaults to \"dump\".\n"
" > \"-outputNameOverride\" is the name to use for the generated file, without a file extension. Optional. Defaults to the engine's internal name for the generated texture.\n"
;
CON_COMMAND(pku_paintkit_output_texture, paintkitOutputTextureHelp)
{
	if (args.ArgC() == 0)
	{
		Msg(paintkitOutputTextureHelp);
		return;
	}
	try {
		CompositorQueue::CompositorRequest request;
		const CPaintKitDefinition* paintKitDef;
		const CPaintKitItemDefinition* paintKitItemDef;
		int wear;
		{
			paintKitDefArgOneOf.Resolve(args, &paintKitDef, NULL, NULL);
			paintKitItemDefArgOneOf.Resolve(args, &paintKitItemDef, NULL, NULL);

			wear = args.FindArgInt("-wear", 1);
			if ((wear < 1) || (wear > 5))
			{
				Warning("-wear must be in the range [1, 5]. Clamping.\n");
				wear = clamp(wear, 1, 5);
			}

			request.team = args.FindArgInt("-team", 0);
			if ((request.team < 0) || (request.team > 1))
			{
				Warning("-team must be in the range [0, 1]. Clamping.\n");
				request.team = clamp(request.team, 0, 1);
			}

			const char* seedStr = args.FindArg("-seed");
			if (seedStr == NULL)
			{
				throw std::runtime_error("Must specify a seed.");
			}
			request.seed = strtoull(seedStr, NULL, 10);

			request.SetWidth(args.FindArgInt("-width", 2048));
			request.SetHeight(args.FindArgInt("-height", 2048));

			CUtlString outputDir = args.FindArg("-outputDir");
			// Default to "dump" if no output directory was explicitly specified.
			if (outputDir.IsEmpty())
			{
				outputDir.Set("dump");
			}

			CUtlString outputName = args.FindArg("-outputNameOverride");

			char finalItemName[_MAX_PATH];
			V_sprintf_safe(finalItemName, "%d_%d_wear_%02d", paintKitItemDef->GetMessage()->item_definition_index(), paintKitDef->GetMessage()->header().defindex(), wear); // Same format used by the game.
			request.name.Set(finalItemName);

			const KeyValues* stageDescKvs = paintKitDef->GetStageDescKeyValues(paintKitItemDef->GetMessage()->item_definition_index(), wear);
			request.SetStageDesc(stageDescKvs);

			request.onComplete = [outputDir, outputName](const CompositorQueue::CompositorResult& result) mutable {
				
				if (result.resultTexture)
				{
					// Default to the engine's name for the texture if a name override wasn't specified.
					if (outputName.IsEmpty())
					{
						outputName.Set(CUtlString(result.resultTexture->GetName()).UnqualifiedFilename());
					}

					// Enforce the .tga extension. Makes no sense to allow anything else.
					constexpr const char* outputFormatExtension = ".tga";
					if (!outputName.GetExtension().IsEqual_CaseInsensitive(outputFormatExtension))
					{
						outputName.Append(outputFormatExtension);
					}

					// Generate final output path.
					CUtlString outputPath = outputDir;
					outputPath.Append("/");
					outputPath.Append(outputName);

					// Save the texture to disk.
					SaveTextureToDisk(result.resultTexture, outputPath);
				}
				else if (result.exception)
				{
					Msg("Failed to generate texture: %s\n", result.exception->what());
				}
				else
				{
					Msg("Failed to generate texture: Error unknown or queue has been cancelled.\n");
				}
			};
		}

		CompositorQueue::Singleton()->EnqueueRequest(std::move(request));
	}
	catch (std::exception& e)
	{
		Warning("Failed to submit texture compositor request: %s\n", e.what());
	}
}

const char* paintkitGenerateKvsHelp = "Generates the KeyValues of a paintkit."
"\n"
"Example:\n"
"> paintkit_generate_kvs -paintKitDefName \"workshop_simple_spirits\" -paintKitItemDefName \"flamethrower\" -wear 3 -outputFilePath \"dump/paintkits/workshop_simple_spirits_w3.vdf\"\n"
"\n"
"Parameters:\n"
" > \"-paintKitDefName\" is the name of the paint kit to use (e.g. \"workshop_simple_spirits\" for Simple Spirits). You can use \"paintKitDefIndex\" instead.\n"
" > \"-paintKitDefIndex\" is the index of the paint kit to use (e.g. 290 for Simple Spirits). You can use \"paintKitDefName\" instead.\n"
" > \"-paintKitItemDefName\" is the name of the item to use (e.g. \"scattergun\" for the Scattergun). You can use \"itemDefIndex\" instead.\n"
" > \"-paintKitItemDefIndex\" is the index of the item to use (e.g. 200 for the Scattergun). You can use \"itemDefName\" instead.\n"
" > \"-wear\" is the wear level of the weapon in the range [1,5], with 1 being Factory New and 5 being Battle Scarred. Optional. Defaults to 1.\n"
" > \"-outputFilePath\" is the path to save the generated file to. Optional. Outputs to console if this option is not provided.\n"
;

CON_COMMAND(pku_paintkit_generate_kvs, paintkitGenerateKvsHelp)
{
	if (args.ArgC() == 0)
	{
		Msg(paintkitGenerateKvsHelp);
		return;
	}
	try
	{
		const CPaintKitDefinition* paintKitDef = NULL;
		const CPaintKitItemDefinition* paintKitItemDef = NULL;

		if (!paintKitDefArgOneOf.Resolve(args, &paintKitDef, NULL, NULL))
		{
			return;
		}

		if (!paintKitItemDefArgOneOf.Resolve(args, &paintKitItemDef, NULL, NULL))
		{
			return;
		}

		int wear = args.FindArgInt("-wear", 1);
		if ((wear < 1) || (wear > 5))
		{
			Warning("-wear must be in the range [1, 5]. Clamping.\n");
			wear = clamp(wear, 1, 5);
		}

		CUtlString outputFilePath = args.FindArg("-outputFilePath");
		if (outputFilePath.IsEmpty())
		{
			Msg("-outputFilePath not specified, outputting to console.\n");
		}

		int item_definition_index = paintKitItemDef->GetMessage()->item_definition_index();

		// Msg("Generating KVs for item_definition_index=%d...\n", item_definition_index);

		KeyValues* stageDesc = const_cast<KeyValues*>(paintKitDef->GetStageDescKeyValues(item_definition_index, wear)); // Const casting this because the functions used (KeyValuesDumpAsDevMsg and SaveToFile) didn't mark the args const, but I don't expect they modify state.

		if (stageDesc)
		{
			if (outputFilePath.IsEmpty())
			{
				// I'm just gonna assume that this function should've marked its arg as const.
				KeyValuesDumpAsDevMsg(stageDesc, 0);
				Msg("Generated KVs.\n");
			}
			else
			{
				// Ensure necessary directories exist.
				g_pFullFileSystem->CreateDirHierarchy(outputFilePath.DirName());
				if (stageDesc->SaveToFile(g_pFullFileSystem, outputFilePath, NULL))
				{
					Msg("Wrote KVs to file \"%s\".\n", outputFilePath.Get());
				}
				else
				{
					CUtlString msg;
					msg.Format("Unable to write to file \"%s\".", outputFilePath.Get());
					throw std::runtime_error(msg);
				}
			}
		}
		else
		{
			Msg("Failed to generate kvs.\n");
		}
	}
	catch (std::exception& e)
	{
		Warning("Failed to generate kvs: %s\n", e.what());
	}
}

CON_COMMAND(pku_paintkit_list, "Lists all paint kits in the game.")
{
	const auto& defMap = ProtoBufScriptDefinitionManager()->GetDefMap<CPaintKitDefinition>();
	for (auto i = defMap.FirstInorder(); defMap.IsValidIndex(i); i = defMap.NextInorder(i))
	{
		// auto key = defMap.Key(i);
		auto protoBufScriptObjectDef = defMap.Element(i);
		auto* msg = protoBufScriptObjectDef->GetMessage();

		auto& header = msg->header();
		Msg("{ defindex=%d name=\"%s\" }\n", header.defindex(), header.name().c_str());
	}
}

CON_COMMAND(pku_paintkit_item_list, "Lists all paintable items in the game.")
{
	const auto& defMap = ProtoBufScriptDefinitionManager()->GetDefMap<CPaintKitItemDefinition>();
	for (auto i = defMap.FirstInorder(); defMap.IsValidIndex(i); i = defMap.NextInorder(i))
	{
		auto key = defMap.Key(i);
		auto protoBufScriptObjectDef = defMap.Element(i);
		auto* msg = protoBufScriptObjectDef->GetMessage();

		auto& header = msg->header();
		Msg("{ defindex=%d name=\"%s\" item_definition_index=%d }\n", header.defindex(), header.name().c_str(), msg->item_definition_index());
	}
}
