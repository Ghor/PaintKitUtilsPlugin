#pragma once

#include "platform.h" // Prerequisite of "game/shared/igamesystem.h"
#include "game/shared/igamesystem.h" // For CAutoGameSystemPerFrame
#include "utlmap.h"
#include "tf_proto_def_messages.pb.h"
#include "IProtoBufScriptObjectDefinition.h"
#include "CTypedProtoBufScriptObjectDefinition.h"
#include "ProtoDefInfo.h"

using namespace ProtoDefInfo;

class CPaintKitDefinition;
class CPaintKitItemDefinition;
class IProtoBufScriptObjectFactory;

// "new" allocates 0x1b4 (436) bytes for this type.
class CProtoBufScriptObjectDefinitionManager : public CAutoGameSystemPerFrame
{
private:
	CUtlMap<unsigned short, IProtoBufScriptObjectDefinition*> m_DefMaps[protoDefTypeMax + 1];
	
	CUtlMap<unsigned short, IProtoBufScriptObjectFactory*> m_Factories;
	CUtlMap<unsigned short, void*> m_Unknown2;
	int m_Unknown3;
	
public:
	const CUtlMap<unsigned short, IProtoBufScriptObjectDefinition*>& GetDefMap(ProtoDefTypes defIndex) const
	{
		return m_DefMaps[defIndex];
	}

	template<ProtoDefTypes I>
	const auto& GetDefMap() const
	{
		using MsgType_t = typename ProtoDefEnumToType<I>::type;
		using DefType_t = CTypedProtoBufScriptObjectDefinition<MsgType_t, I>;
		using MapType_t = CUtlMap<unsigned short, DefType_t*>;
		return *(MapType_t*)(&m_DefMaps[I]);
	}

	const CPaintKitDefinition* FindPaintKitDefinition(int defIndex) const;
	const CPaintKitDefinition* FindPaintKitDefinition(const char* name) const;

	const CPaintKitItemDefinition* FindPaintKitItemDefinition(int defIndex) const;
	const CPaintKitItemDefinition* FindPaintKitItemDefinition(const char* name) const;
};

CProtoBufScriptObjectDefinitionManager* ProtoBufScriptDefinitionManager();
