#pragma once

using EventChecker = RE::BSEventNotifyControl;

class EquipHook
{
public:
    static void Hook()
    {
        REL::Relocation<std::uintptr_t> PlayerCharacterVtbl{ RE::VTABLE_PlayerCharacter[0] };
    //    REL::Relocation<std::uintptr_t> NonPlayerCharacterVtbl{ RE::VTABLE_Actor[0] };

        _OnEquipItemPC      = PlayerCharacterVtbl.write_vfunc(0xB2, OnEquipItemPC);
    //    _OnEquipItemNPC     = NonPlayerCharacterVtbl.write_vfunc(0xB2, OnEquipItemNPC);       //  maybe later
    }
private:
    static void OnEquipItemPC(RE::PlayerCharacter* a_this, bool a_playAnim);
//    static void OnEquipItemNPC(RE::Actor* a_this, bool a_playAnim);

    static bool SkipAnim(RE::PlayerCharacter* a_this, bool a_playAnim);

    static inline REL::Relocation<decltype(OnEquipItemPC)>  _OnEquipItemPC;
//    static inline REL::Relocation<decltype(OnEquipItemNPC)> _OnEquipItemNPC;

    static inline bool _skipAnim;
};