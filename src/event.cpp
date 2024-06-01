#include "event.h"
/**/
bool AnimationEventTracker::Register()
{
    const auto pc = RE::PlayerCharacter::GetSingleton();

    bool bSinked = false;
    RE::BSAnimationGraphManagerPtr graphManager;

    if (pc) pc->GetAnimationGraphManager(graphManager);
    else spdlog::debug("Can't found Player Character");

    if (graphManager) {
        for (auto& animationGraph : graphManager->graphs) {
            if (bSinked) {
                break;
            }
            eventSource = animationGraph->GetEventSource<RE::BSAnimationGraphEvent>();
            for (auto& sink : eventSource->sinks) {
                if (sink == AnimationEventTracker::GetSingleton()) {
                    bSinked = true;
                    break;
                }
            }
        }
    }

    if (!bSinked) {
        spdlog::info("Failed to register {}", typeid(RE::BSAnimationGraphEvent).name());
    }
    return bSinked;
}

void AnimationEventTracker::SendAnimationEvent(RE::Actor* a_this, const RE::BSFixedString a_tag, const RE::BSFixedString a_payload)
{
    if (a_this) {
        RE::BSAnimationGraphEvent event = {a_tag, a_this, a_payload};
        a_this->ProcessEvent(&event, eventSource);
    }
}