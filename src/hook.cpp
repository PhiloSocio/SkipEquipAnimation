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
bool CheckIsValidBoundObject(RE::TESForm* a_lHandObject, RE::TESForm* a_rHandObject)
{
    bool isValid = false;
#ifdef WORK_WITH_MAGIC_OBJECTS
    if ((a_lHandObject && a_lHandObject->IsMagicItem()) || (a_rHandObject && a_rHandObject->IsMagicItem()))
        isValid = true;
#endif
    if (a_lHandObject) {
        if (a_lHandObject->IsArmor())
            isValid = true;
        else if (auto lHandWeapon = a_lHandObject->As<RE::TESObjectWEAP>(); lHandWeapon)//  && (uint32_t)lHandWeapon->GetWeaponType() <= 6u)
            isValid = true;
    }
    if (a_rHandObject) {
        if (a_rHandObject->IsArmor())
            isValid = true;
        else if (auto rHandWeapon = a_rHandObject->As<RE::TESObjectWEAP>(); rHandWeapon)// && (uint32_t)rHandWeapon->GetWeaponType() <= 6u)
            isValid = true;
    }
    return isValid;
}
void SendEquipEvents(RE::Actor* a_this, RE::TESForm* a_lHandObject, RE::TESForm* a_rHandObject)
{
    if (auto animationEventTracker = AnimationEventTracker::GetSingleton(); animationEventTracker) {
        animationEventTracker->SendAnimationEvent(a_this, "weaponDraw");
#ifdef WORK_WITH_MAGIC_OBJECTS
        if ((a_lHandObject && a_lHandObject->IsMagicItem()) || (a_rHandObject && a_rHandObject->IsMagicItem()))
            animationEventTracker->SendAnimationEvent(a_this, "MagicEquip_Out");
#endif

        if ((a_lHandObject && (a_lHandObject->IsWeapon() || a_lHandObject->IsArmor())) || (a_rHandObject && (a_rHandObject->IsWeapon() || a_rHandObject->IsArmor()))) {
            animationEventTracker->SendAnimationEvent(a_this, "WeapEquip_Out");
            animationEventTracker->SendAnimationEvent(a_this, "WeapEquip_OutMoving");
        }
    }
}
bool EquipHook::SkipAnim(RE::PlayerCharacter* a_this, bool a_playAnim)
{
    bool skipAnim = !a_playAnim;
    if (a_this && a_this->AsActorState() && a_this->AsActorState()->IsWeaponDrawn()) {
        auto rHandObj = a_this->GetEquippedObject(false);
        auto lHandObj = a_this->GetEquippedObject(true);

        if (CheckIsValidBoundObject(lHandObj, rHandObj)) {
            int delay = 300;
            a_this->GetGraphVariableBool("SkipEquipAnimation", skipAnim);
            a_this->GetGraphVariableInt("LoadBoundObjectDelay", delay);
            if (delay < (int)(*g_deltaTimeRealTime)) delay = (int)(*g_deltaTimeRealTime);   // the loading process will start next frame.
            std::jthread delayedEquipThread(
                [a_this, skipAnim, delay, lHandObj, rHandObj] {
                    if (skipAnim) {
                        spdlog::debug("equip anim skipped");
                        std::this_thread::sleep_for(std::chrono::milliseconds(delay));
                        SendEquipEvents(a_this, lHandObj, rHandObj);
                        spdlog::debug("weapon 3d model called");
                    }
                }
            );
            delayedEquipThread.detach();
        }
    } return skipAnim;
}