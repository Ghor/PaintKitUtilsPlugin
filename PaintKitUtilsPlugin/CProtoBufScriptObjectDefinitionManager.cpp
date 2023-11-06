#include "CProtoBufScriptObjectDefinitionManager.h"
#include "CPaintKitDefinition.h"
#include "CPaintKitItemDefinition.h"
#include "SigScan.h"

const CPaintKitDefinition* CProtoBufScriptObjectDefinitionManager::FindPaintKitDefinition(int defIndex) const
{
    auto& map = this->GetDefMap<CPaintKitDefinition>();
    for (int i = 0; i < map.Count(); ++i)
    {
        if (map.Key(i) == defIndex)
        {
            return map.Element(i);
        }
    }
    return NULL;
}

const CPaintKitDefinition* CProtoBufScriptObjectDefinitionManager::FindPaintKitDefinition(const char* name) const
{
    auto& map = GetDefMap<CPaintKitDefinition>();
    for (int i = 0; i < map.Count(); ++i)
    {
        auto* current = map.Element(i);
        auto* currentMessage = current->GetMessage();
        if (currentMessage == NULL)
        {
            Warning("Paintkit %d has no message.", map.Key(i));
            continue;
        }
        if (!currentMessage->has_header())
        {
            Warning("Paintkit item %d has no header.", map.Key(i));
            continue;
        }
        
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

const __declspec(noinline) CPaintKitItemDefinition* CProtoBufScriptObjectDefinitionManager::FindPaintKitItemDefinition(const char* name) const
{
    // auto& map = this->GetDefMap<ProtoDefTypes::DEF_TYPE_PAINTKIT_ITEM_DEFINITION>();
    auto& map = this->GetDefMap<CPaintKitItemDefinition>();
    //int count = map.Count();


    // Msg("this=0x%X paintKitMap=0x%X itemMap=0x%X sizeof(map)=0x%X\n", (int) this, (int) &this->GetDefMap<CPaintKitDefinition>(), (int) & map, sizeof(map));
    // return NULL;
    
    for (int i = 0; i < map.Count(); ++i)
    {
        auto* current = map.Element(i);
        auto* currentMessage = current->GetMessage();
        if (currentMessage == NULL)
        {
            Warning("Paintkit item %d has no message.", map.Key(i));
            continue;
        }

        if (!currentMessage->has_header())
        {
            Warning("Paintkit item %d has no header.", map.Key(i));
            continue;
        }
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
    auto& map = this->GetDefMap<CPaintKitItemDefinition>();
    for (int i = 0; i < map.Count(); ++i)
    {
        if (map.Key(i) == defIndex)
        {
            return map.Element(i);
        }
    }
    return NULL;
}



// OffsetFunc<CProtoBufScriptObjectDefinitionManager* (__cdecl*)()> ProtoBufScriptDefinitionManager_Impl("client.dll", 0x03258d0);
SigScannedFunc<CProtoBufScriptObjectDefinitionManager* (__cdecl*)()> ProtoBufScriptDefinitionManager_Impl(
    "client.dll",
    "ProtoBufScriptDefinitionManager",
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
