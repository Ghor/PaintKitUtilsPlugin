#include "CProtoBufScriptObjectDefinitionManager.h"
#include "CPaintKitDefinition.h"
#include "CPaintKitItemDefinition.h"
#include "SigScan.h"

const CPaintKitDefinition* CProtoBufScriptObjectDefinitionManager::FindPaintKitDefinition(int defIndex) const
{
    auto& map = this->GetDefMap(ProtoDefTypes::DEF_TYPE_PAINTKIT_DEFINITION);
    for (int i = 0; i < map.Count(); ++i)
    {
        if (map.Key(i) == defIndex)
        {
            return reinterpret_cast<const CPaintKitDefinition*>(map.Element(i));
        }
    }
    return NULL;
}

const CPaintKitDefinition* CProtoBufScriptObjectDefinitionManager::FindPaintKitDefinition(const char* name) const
{
    auto& map = GetDefMap<ProtoDefTypes::DEF_TYPE_PAINTKIT_DEFINITION>();
    for (int i = 0; i < map.Count(); ++i)
    {
        auto* current = reinterpret_cast<const CPaintKitDefinition*>(map.Element(i));
        auto* currentMessage = current->GetMessage();
        
        const CMsgProtoDefHeader& header = currentMessage->header();
        if (!header.has_name())
        {
            continue;
        }

        if (stricmp(header.name().c_str(), name) == 0)
        {
            return current;
        }
    }
    return NULL;
}

const CPaintKitItemDefinition* CProtoBufScriptObjectDefinitionManager::FindPaintKitItemDefinition(int defIndex) const
{
    auto& map = this->GetDefMap(ProtoDefTypes::DEF_TYPE_PAINTKIT_ITEM_DEFINITION);
    for (int i = 0; i < map.Count(); ++i)
    {
        if (map.Key(i) == defIndex)
        {
            return reinterpret_cast<CPaintKitItemDefinition*>(map.Element(i));
        }
    }
    return NULL;
}

const CPaintKitItemDefinition* CProtoBufScriptObjectDefinitionManager::FindPaintKitItemDefinition(const char* name) const
{
    auto& map = this->GetDefMap<ProtoDefTypes::DEF_TYPE_PAINTKIT_ITEM_DEFINITION>();
    for (int i = 0; i < map.Count(); ++i)
    {
        auto* current = reinterpret_cast<const CPaintKitItemDefinition*>(map.Element(i));
        auto* currentMessage = current->GetMessage();
        const CMsgProtoDefHeader& header = currentMessage->header();

        if (stricmp(header.name().c_str(), name) == 0)
        {
            return current;
        }
    }
    return NULL;
}



// OffsetFunc<CProtoBufScriptObjectDefinitionManager* (__cdecl*)()> ProtoBufScriptDefinitionManager_Impl("client.dll", 0x03258d0);
SigScannedFunc<CProtoBufScriptObjectDefinitionManager* (__cdecl*)()> ProtoBufScriptDefinitionManager_Impl(
    "client.dll",
    std::make_unique<CompoundPattern>(std::initializer_list<Pattern*>{
        new BasicPattern("\xA1"),
        new WildcardPattern(4),
        new BasicPattern("\x85\xC0\x75\x2A\x68\xB4\x01\x00\x00\xE8"),
        new WildcardPattern(4)
    })
);

CProtoBufScriptObjectDefinitionManager* ProtoBufScriptDefinitionManager()
{
    auto func = ProtoBufScriptDefinitionManager_Impl.GetFunc();
    if (func)
    {
        return func();
    }
    return NULL;
}
