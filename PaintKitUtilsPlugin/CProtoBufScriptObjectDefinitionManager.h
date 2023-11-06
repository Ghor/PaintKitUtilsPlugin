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
	static const int m_DefMapsCount = protoDefTypeMax + 1;
	CUtlMap<unsigned short, IProtoBufScriptObjectDefinition*> m_DefMaps[m_DefMapsCount];
	
	CUtlMap<unsigned short, IProtoBufScriptObjectFactory*> m_Factories;
	CUtlMap<unsigned short, void*> m_Unknown2;
	int m_Unknown3;
	
public:
	const CUtlMap<unsigned short, IProtoBufScriptObjectDefinition*>& GetDefMap(ProtoDefTypes defIndex) const
	{
		return m_DefMaps[defIndex];
	}

	template<ProtoDefTypes I>
	const CUtlMap<unsigned short, CTypedProtoBufScriptObjectDefinition<typename ProtoDefEnumToType<I>::type, I>*>& GetDefMap() const
	{
		using MsgType_t = typename ProtoDefEnumToType<I>::type;
		using DefType_t = CTypedProtoBufScriptObjectDefinition<MsgType_t, I>;
		using MapType_t = CUtlMap<unsigned short, DefType_t*>;

		

		return *(const MapType_t*)(&m_DefMaps[I]);
	}

	template<typename T>
	const __declspec(noinline) CUtlMap<unsigned short, T*>& GetDefMap() const
	{
		using MsgType_t = typename T::TMsg_t;
		const int index = ProtoDefTypeToEnum<MsgType_t>::enumVal;
		static_assert(index >= 0, "index is out of bounds!");
		static_assert(index < m_DefMapsCount, "index is out of bounds!");
		const CUtlMap<unsigned short, IProtoBufScriptObjectDefinition*>* map = &m_DefMaps[index];
		return *(const CUtlMap<unsigned short, T*>*)(map);
	}

	const CPaintKitDefinition* FindPaintKitDefinition(int defIndex) const;
	const CPaintKitDefinition* FindPaintKitDefinition(const char* name) const;

	const CPaintKitItemDefinition* FindPaintKitItemDefinition(int defIndex) const;
	const CPaintKitItemDefinition* FindPaintKitItemDefinition(const char* name) const;
};

CProtoBufScriptObjectDefinitionManager* ProtoBufScriptDefinitionManager();
