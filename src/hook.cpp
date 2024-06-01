#include "hook.h"
#include "event.h"

void EquipHook::OnEquipItemPC(RE::PlayerCharacter* a_this, bool a_playAnim)
{
    _OnEquipItemPC(a_this, !SkipAnim(a_this, a_playAnim));
}
/*
void EquipHook::OnEquipItemNPC(RE::Actor* a_this, bool a_playAnim)
{
    _OnEquipItemNPC(a_this, !SkipAnim(a_this, a_playAnim));
}
*/
bool EquipHook::SkipAnim(RE::PlayerCharacter* a_this, bool a_playAnim)
{
    bool skipAnim = !a_playAnim;
    if (a_this) {
        int delay = 300;
        a_this->GetGraphVariableBool("SkipEquipAnimation", skipAnim);
        a_this->GetGraphVariableInt("LoadBoundObjectDelay", delay);
        if (delay < (int)(*g_deltaTimeRealTime * 2.f)) delay = (int)(*g_deltaTimeRealTime * 2.f);   // the loading process will start at leas two frames later for safety
        std::jthread delayedEquipThread(
            [a_this, skipAnim, delay] {
                if (skipAnim) {
                    spdlog::debug("equip anim skipped");
                    std::this_thread::sleep_for(std::chrono::milliseconds(delay));
                    if (auto animationEventTracker = AnimationEventTracker::GetSingleton(); animationEventTracker) {
                        animationEventTracker->SendAnimationEvent(a_this, "weaponDraw");
                        animationEventTracker->SendAnimationEvent(a_this, "WeapEquip_Out");
                        animationEventTracker->SendAnimationEvent(a_this, "WeapEquip_OutMoving");
                        spdlog::debug("weapon 3d model called");
                    }
                }
            }
        );
        delayedEquipThread.detach();
    } return skipAnim;
}