#include "CPaintKitDefinition.h"
#include "SigScan.h"

namespace {
	SigScannedFunc<KeyValues* (__thiscall*)(const CPaintKitDefinition* paintKit, unsigned short itemIndex, int wear)> GenerateStageDescKeyValues_Impl(
		"client.dll",
		"GenerateStageDescKeyValues",
		std::make_unique<BasicPattern>("\x55\x8B\xEC\x83\xEC\x28\x66\x8B\x45\x08\x53\x8B\x5D\x0C\x56\x57\x8B\xF9\x66\x89\x45\xF8\x66\x89\x45\xEC\x8D\x45\xEC\x50\x89\x5D\xF0\x8D\x8F\xF4\x01\x00\x00")
	);

	// OffsetFunc<KeyValues* (__thiscall*)(CPaintKitDefinition* paintKit, unsigned short itemIndex, int wear)> GenerateStageDescKeyValues_ImplExpected("client.dll", 0x0030AD30);
}


const KeyValues* CPaintKitDefinition::GetStageDescKeyValues(unsigned int itemDefinitionIndex, int wear) const
{
	auto func = GenerateStageDescKeyValues_Impl.GetFunc();
	if (func)
	{
		return func(this, itemDefinitionIndex, wear);
	}
	else
	{
		Warning("Couldn't find " __FUNCTION__ " implementation!");
	}
	return NULL;
}
