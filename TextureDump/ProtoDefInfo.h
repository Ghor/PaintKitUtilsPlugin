/*
* Metadata for ProtoDef types.
*/

#pragma once
#include "tf_proto_def_messages.pb.h"

namespace ProtoDefInfo
{
	template<ProtoDefTypes I> struct ProtoDefEnumToType {};
	template<typename T> struct ProtoDefTypeToEnum {};

#pragma push_macro("DEF_PROTODEF_METADATA")
#define DEF_PROTODEF_METADATA(I, T)\
template<> struct ProtoDefEnumToType<I> { using type = T; };\
template<> struct ProtoDefTypeToEnum<T> { static const int enumVal = I; };

	DEF_PROTODEF_METADATA(ProtoDefTypes::DEF_TYPE_QUEST_MAP_NODE, CMsgQuestMapNodeDef)
	DEF_PROTODEF_METADATA(ProtoDefTypes::DEF_TYPE_QUEST_THEME, CMsgQuestTheme)
	DEF_PROTODEF_METADATA(ProtoDefTypes::DEF_TYPE_QUEST_MAP_REGION, CMsgQuestMapRegionDef)
	DEF_PROTODEF_METADATA(ProtoDefTypes::DEF_TYPE_QUEST, CMsgQuestDef)
	DEF_PROTODEF_METADATA(ProtoDefTypes::DEF_TYPE_QUEST_OBJECTIVE, CMsgQuestObjectiveDef)
	DEF_PROTODEF_METADATA(ProtoDefTypes::DEF_TYPE_PAINTKIT_VARIABLES, CMsgPaintKit_Variables)
	DEF_PROTODEF_METADATA(ProtoDefTypes::DEF_TYPE_PAINTKIT_OPERATION, CMsgPaintKit_Operation)
	DEF_PROTODEF_METADATA(ProtoDefTypes::DEF_TYPE_PAINTKIT_ITEM_DEFINITION, CMsgPaintKit_ItemDefinition)
	DEF_PROTODEF_METADATA(ProtoDefTypes::DEF_TYPE_PAINTKIT_DEFINITION, CMsgPaintKit_Definition)
	DEF_PROTODEF_METADATA(ProtoDefTypes::DEF_TYPE_HEADER_ONLY, CMsgHeaderOnly)
	DEF_PROTODEF_METADATA(ProtoDefTypes::DEF_TYPE_QUEST_MAP_STORE_ITEM, CMsgQuestMapStoreItem)
	DEF_PROTODEF_METADATA(ProtoDefTypes::DEF_TYPE_QUEST_MAP_STAR_TYPE, CMsgQuestMapStarType)

#pragma pop_macro("DEF_PROTODEF_METADATA")

	// An array of the values in ProtoDefTypes. This is useful for being able to do iteration over the enum and get its size. It just needs to be updated if the enum ever changes.
	static constexpr const ProtoDefTypes protoDefTypes[] = {
		ProtoDefTypes::DEF_TYPE_QUEST_MAP_NODE,
		ProtoDefTypes::DEF_TYPE_QUEST_THEME,
		ProtoDefTypes::DEF_TYPE_QUEST_MAP_REGION,
		ProtoDefTypes::DEF_TYPE_QUEST,
		ProtoDefTypes::DEF_TYPE_QUEST_OBJECTIVE,
		ProtoDefTypes::DEF_TYPE_PAINTKIT_VARIABLES,
		ProtoDefTypes::DEF_TYPE_PAINTKIT_OPERATION,
		ProtoDefTypes::DEF_TYPE_PAINTKIT_ITEM_DEFINITION,
		ProtoDefTypes::DEF_TYPE_PAINTKIT_DEFINITION,
		ProtoDefTypes::DEF_TYPE_HEADER_ONLY,
		ProtoDefTypes::DEF_TYPE_QUEST_MAP_STORE_ITEM,
		ProtoDefTypes::DEF_TYPE_QUEST_MAP_STAR_TYPE,
	};

	constexpr int protoDefTypeCount = std::size(protoDefTypes);
	constexpr int protoDefTypeMax = protoDefTypes[std::size(protoDefTypes) - 1];
}
