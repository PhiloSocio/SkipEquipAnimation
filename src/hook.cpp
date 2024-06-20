#include "hook.h"
#include "event.h"
#include "util.h"
using namespace Util;

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
bool CheckIsValidBoundObject(const RE::TESForm* a_object)
{
#ifdef WORK_WITH_MAGIC_OBJECTS
    if ((a_object && a_object->IsMagicItem()))
        return = true;
#endif
    return {//  && (uint32_t)a_object->As<RE::TESObjectWEAP>()->GetWeaponType() <= 6u)
        a_object && (a_object->As<RE::TESObjectWEAP>() || a_object->As<RE::TESObjectARMO>())
    };
}
void SendEquipEvents(RE::Actor* a_this, RE::TESForm* a_lHandObject, RE::TESForm* a_rHandObject)
{
    if (auto animationEventTracker = AnimationEventTracker::GetSingleton()) {
        animationEventTracker->SendAnimationEvent(a_this, "weaponDraw");
#ifdef WORK_WITH_MAGIC_OBJECTS
        if ((a_lHandObject && a_lHandObject->IsMagicItem()) || (a_rHandObject && a_rHandObject->IsMagicItem()))
            animationEventTracker->SendAnimationEvent(a_this, "MagicEquip_Out");
#endif

        if ((a_lHandObject && (a_lHandObject->IsWeapon() || a_lHandObject->IsArmor()))
         || (a_rHandObject && (a_rHandObject->IsWeapon() || a_rHandObject->IsArmor()))) {
            animationEventTracker->SendAnimationEvent(a_this, "WeapEquip_Out");
            animationEventTracker->SendAnimationEvent(a_this, "WeapEquip_OutMoving");
        }
    }
}
bool EquipHook::SkipAnim(RE::PlayerCharacter* a_this, bool a_playAnim)
{
    _skipAnim = !a_playAnim;
    if (AnimationEventTracker::GetSingleton()->Register() && a_this && a_this->AsActorState() && a_this->AsActorState()->IsWeaponDrawn()) {
        auto rHandObj = a_this->GetEquippedObject(false);
        auto lHandObj = a_this->GetEquippedObject(true);

        if (CheckIsValidBoundObject(lHandObj) || CheckIsValidBoundObject(rHandObj)) {
            int delay = 300;
            bool skip3D = false;
            a_this->GetGraphVariableBool("SkipEquipAnimation", _skipAnim);
            a_this->GetGraphVariableInt("LoadBoundObjectDelay", delay);
            a_this->GetGraphVariableBool("Skip3DLoading", skip3D);
            if (delay < (int)(*g_deltaTimeRealTime)) delay = (int)(*g_deltaTimeRealTime);   // the loading process will start next frame.
            if (!skip3D && _skipAnim) {
                std::jthread delayedEquipThread([=]() {
                        if (_skipAnim) {
                            spdlog::debug("equip anim skipped");
                            std::this_thread::sleep_for(std::chrono::milliseconds(delay));
                            SendEquipEvents(a_this, lHandObj, rHandObj);
                            spdlog::debug("weapon 3d model called");
                        }
                    }
                );
                delayedEquipThread.detach();
            }
        }
    } return _skipAnim;
}