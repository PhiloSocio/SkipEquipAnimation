#include "log.h"
#include "hook.h"
#include "event.h"

void MessageHandler(SKSE::MessagingInterface::Message* a_msg)
{
    switch (a_msg->type) {
    case SKSE::MessagingInterface::kDataLoaded:
        EquipHook::Hook();
        break;
    case SKSE::MessagingInterface::kPostLoad:
        break;
    case SKSE::MessagingInterface::kPreLoadGame:
    case SKSE::MessagingInterface::kPostLoadGame:
    case SKSE::MessagingInterface::kNewGame:
        if (auto animationEventTracker = AnimationEventTracker::GetSingleton(); animationEventTracker)
            animationEventTracker->Register();
        break;
    }
}
SKSEPluginLoad(const SKSE::LoadInterface *skse) {

    SetupLog();

    auto* plugin  = SKSE::PluginDeclaration::GetSingleton();
    spdlog::info("{} v{} is loading...", plugin->GetName(), plugin->GetVersion());

    SKSE::Init(skse);

    auto messaging = SKSE::GetMessagingInterface();
    if (!messaging->RegisterListener("SKSE", MessageHandler)) {
        return false;
    }

    spdlog::info("{} by {} has finished loading. Support for more mods! {}", plugin->GetName(), plugin->GetAuthor(), plugin->GetSupportEmail());

    return true;
<<<<<<< HEAD
}
=======
}

SKSEPluginInfo(
    .Version = REL::Version{ 1, 0, 4, 0 },
    .Name = "SkipEquipAnimation"sv,
    .Author = "AnArchos"sv,
    .SupportEmail = "patreon.com/AnArchos"sv,
    .StructCompatibility = SKSE::StructCompatibility::Independent,
    .RuntimeCompatibility = SKSE::VersionIndependence::AddressLibrary,
    .MinimumSKSEVersion = REL::Version{ 2, 0, 0, 2 }
)
>>>>>>> 2de7a0e96de1cb822a1946aa1bf94b443dc7c510
