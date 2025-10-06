#include <windows.h>
#include <iostream>
#include <string>
#include <map>


#include "Sigscan.h"
#include <thread>
#include <chrono>


#include "assert/MinHook.h"


#pragma comment(lib, "libMinHook.x64.lib")


#include "utils.h"
#include "Game.h"

#include "HookDX11.h"



_Game* Game = nullptr;





// todo: map seed
//constexpr int FIXED_SEED = 123456;
//
//
//typedef void(__fastcall* Random_Ctor_t)(void* _this, void* seed);
//Random_Ctor_t oRandom_Ctor = nullptr;
//
//int fixedSeed = 2137;  // Twój stały seed
//
//void __fastcall hkIl2CppRandom(void* rcx, void* seed) {
//
//    seed = (void*)(uintptr_t)fixedSeed;  // nadpisanie rdx
//
//    
//    //std::cout << "GENERATED SEED: " << (int)seed << std::endl;
//
//    // working only for mapseed not interactables
//
//    oRandom_Ctor(rcx, seed);
//
//}
//
//using Random_Ctor_Default_t = void(__fastcall*)(void* _this);
//Random_Ctor_Default_t oRandom_Ctor_Default = nullptr;
//
//void __fastcall hkRandom_Ctor_Default(void* _this)
//{    
//    oRandom_Ctor(_this, (void*)fixedSeed);
//}



typedef void(__fastcall* PlayerOnDestroy)(void* rcx, void* rdx);
PlayerOnDestroy oPlayerOnDestroy = nullptr;

void __fastcall hkPlayerOnDestroy(void* rcx, void* rdx) {

    // clear everything

    Game->Chests.list.clear();
    Game->Merchants.list.clear();
    Game->Pickups.list.clear();
    Game->MoaiShrines.list.clear();
    Game->CursedShrines.list.clear();
    Game->ChallengeShrines.list.clear();
    Game->GreedShrines.list.clear();
    Game->MagnetShrines.list.clear();
    Game->Microwaves.list.clear();

    
    oPlayerOnDestroy(rcx, rdx);



}


typedef void(__fastcall* PickupConstr)(void* rcx, void* rdx);
PickupConstr oPickupConstr = nullptr;


void __fastcall hkPickupConstre(void* rcx, void* rdx) {

    oPickupConstr(rcx, rdx);
    //int pickupID = *(int*)((DWORD64)rcx + 0x28);
    //std::cout << "added pickup - id: " << pickupID << std::endl;
    Game->Pickups.list.push_back((Pickup*)rcx);


}

typedef void(__fastcall* BaseInterfaceConstructor_t)(void* rcx, void* rdx);
BaseInterfaceConstructor_t oBaseInterfaceConstructor = nullptr;



std::string GetIl2CppTypeName(void* obj)
{
    if (!obj) return "null";

    HMODULE hGameAssembly = GetModuleHandleA("GameAssembly.dll");

    auto il2cpp_object_get_class = (il2cpp_object_get_class_t)GetProcAddress(hGameAssembly, "il2cpp_object_get_class");
    auto il2cpp_class_get_type = (il2cpp_class_get_type_t)GetProcAddress(hGameAssembly, "il2cpp_class_get_type");
    auto il2cpp_type_get_name = (il2cpp_type_get_name_t)GetProcAddress(hGameAssembly, "il2cpp_type_get_name");

    if (!il2cpp_object_get_class || !il2cpp_class_get_type || !il2cpp_type_get_name)
        return "error";

    void* klass = il2cpp_object_get_class(obj);
    void* type = il2cpp_class_get_type(klass);

    const char* name = il2cpp_type_get_name(type);
    return name ? std::string(name) : "unknown";
}


void __fastcall hkBaseInterfaceConstructor(void* rcx, void* rdx) {

    oBaseInterfaceConstructor(rcx, rdx);

    std::string name = GetIl2CppTypeName(rcx);

    //std::cout << "[IL2CPP] Created object: " << name << std::endl;


    if (name.find("InteractableShrineMoai") != std::string::npos) {
        Game->MoaiShrines.list.push_back((MoaiData*)rcx);
    }
    if (name.find("InteractableShrineMagnet") != std::string::npos) {
        Game->MagnetShrines.list.push_back((MoaiData*)rcx);
    }
    if (name.find("InteractableShrineCursed") != std::string::npos) {
        Game->CursedShrines.list.push_back((MoaiData*)rcx);
    }
    if (name.find("InteractableShrineChallenge") != std::string::npos) {
        Game->ChallengeShrines.list.push_back((MoaiData*)rcx);
    }
    if (name.find("InteractableShrineGreed") != std::string::npos) {
        Game->GreedShrines.list.push_back((GreedData*)rcx);
    }
    if (name.find("InteractableMicrowave") != std::string::npos) {
        Game->Microwaves.list.push_back((MicroWave*)rcx);
    }



    // rewrited
    if (name.find("InteractableChest") != std::string::npos) {
        Game->Chests.list.push_back((InteractableChest*)rcx);
    }
    // InteractableAltarGreed ???

}

typedef void(__fastcall* ShadyGuy_Awake)(void* _this, void* rdx);
ShadyGuy_Awake oShadyGuyAwake;

void __fastcall hkShadyGuyAwake(void* rcx, void* rdx) {

    oShadyGuyAwake(rcx, rdx);

    Game->Merchants.list.push_back((InteractableShadyGuy*)rcx);

}






DWORD WINAPI RenderThread(LPVOID)
{
    HWND hwnd = FindWindowA(NULL, "Megabonk");
    while (!hwnd) {
        Sleep(100);
        hwnd = FindWindowA(NULL, "Megabonk");
    }

    HookDX11(hwnd);
    return 0;
}

// Funkcja wątku w DLL
DWORD WINAPI MainThread(LPVOID) {
    // Tworzymy konsolę
    Game = new _Game();
    // for dev
    //AllocConsole();
    //FILE* f;
    //freopen_s(&f, "CONOUT$", "w", stdout);



    if (MH_Initialize() != MH_OK) {
        std::cout << "MinHook init failed!\n";
    }






    HANDLE dll = 0;
    while (!dll) {
        dll = GetModuleHandleA("GameAssembly.dll");
    }
    //std::cout << "[+] FOUND DLL" << std::endl;
    

    //oRandom_Ctor = (Random_Ctor_t)((DWORD64)dll + 0x145CE20);
    //
    //MH_CreateHook((LPVOID)oRandom_Ctor, &hkIl2CppRandom, (LPVOID*)&oRandom_Ctor);
    //oRandom_Ctor_Default = (Random_Ctor_Default_t)((DWORD64)dll + 0x145CDC0);
    //MH_CreateHook((LPVOID)oRandom_Ctor_Default, &hkRandom_Ctor_Default, (LPVOID*)&oRandom_Ctor_Default);


    SigScan scanner;
     


    const char* pattern = "\x48\x89\x5c\x24\x00\x48\x89\x74\x24\x00\x57\x48\x83\xec\x00\x80\x3d\x00\x00\x00\x00\x00\x48\x8b\xf1\x75\x00\x48\x8d\x0d\x00\x00\x00\x00\xe8\x00\x00\x00\x00\x48\x8d\x0d\x00\x00\x00\x00\xe8\x00\x00\x00\x00\x48\x8d\x0d\x00\x00\x00\x00\xe8\x00\x00\x00\x00\xc6\x05\x00\x00\x00\x00\x00\x48\x8b\x05\x00\x00\x00\x00\x83\xb8\x00\x00\x00\x00\x00\x75\x00\x48\x8b\xc8\xe8\x00\x00\x00\x00\x48\x8b\x05\x00\x00\x00\x00\x48\x8b\x80\x00\x00\x00\x00\x48\x8b\x0d\x00\x00\x00\x00\x48\x8b\x78\x00\xe8\x00\x00\x00\x00\x4c\x8b\x05\x00\x00\x00\x00\x45\x33\xc9\x48\x8b\xd6\x48\x8b\xc8\x48\x8b\xd8\xe8\x00\x00\x00\x00\x45\x33\xc0\x48\x8b\xd3\x48\x8b\xcf\xe8\x00\x00\x00\x00\x4c\x8b\xc0\x33\xd2\x48\x8b\x05\x00\x00\x00\x00\x4d\x85\xc0\x75\x00\x48\x8b\x05\x00\x00\x00\x00\x48\x8b\x88\x00\x00\x00\x00\x48\x89\x51\x00\xeb\x00\x49\x39\x00\x4c\x8b\xca\x4d\x0f\x44\xc8\x4d\x85\xc9\x74\x00\x48\x8b\x05\x00\x00\x00\x00\x48\x8b\x88\x00\x00\x00\x00\x4c\x89\x49\x00\x48\x8b\x05\x00\x00\x00\x00\x49\x39\x00\x49\x0f\x44\xd0\x48\x85\xd2\x74\x00\x48\x8b\x05\x00\x00\x00\x00\x48\x8b\x88\x00\x00\x00\x00\x48\x83\xc1\x00\x48\x8b\x5c\x24\x00\x48\x8b\x74\x24\x00\x48\x83\xc4\x00\x5f\xe9\x00\x00\x00\x00\x48\x8b\xd0\x49\x8b\xc8\xe8\x00\x00\x00\x00\xcc\x48\x8b\xd0\x49\x8b\xc8\xe8\x00\x00\x00\x00\xcc\xcc\xcc\xcc\xcc\xcc\xcc\xcc\x48\x83\xec\x00\x80\x3d\x00\x00\x00\x00\x00\x75\x00\x48\x8d\x0d\x00\x00\x00\x00\xe8\x00\x00\x00\x00\xc6\x05\x00\x00\x00\x00\x00\x48\x8b\x05\x00\x00\x00\x00\x83\xb8\x00\x00\x00\x00\x00\x75\x00\x48\x8b\xc8\xe8\x00\x00\x00\x00\x48\x8b\x05\x00\x00\x00\x00\x48\x8b\x80\x00\x00\x00\x00\x48\x8b\x40";
    const char* mask = "xxxx?xxxx?xxxx?xx?????xxxx?xxx????x????xxx????x????xxx????x????xx?????xxx????xx?????x?xxxx????xxx????xxx????xxx????xxx?x????xxx????xxxxxxxxxxxxx????xxxxxxxxxx????xxxxxxxx????xxxx?xxx????xxx????xxx?x?xxxxxxxxxxxxxx?xxx????xxx????xxx?xxx????xxxxxxxxxxx?xxx????xxx????xxx?xxxx?xxxx?xxx?xx????xxxxxxx????xxxxxxxx????xxxxxxxxxxx?xx?????x?xxx????x????xx?????xxx????xx?????x?xxxx????xxx????xxx????xxx";

    DWORD64 address = (DWORD64)scanner.FindPattern("GameAssembly.dll", pattern, mask);
    //std::cout << "ADDRESS: " << address << std::hex << std::endl;

    oPlayerOnDestroy = (PlayerOnDestroy)address;
    MH_CreateHook((LPVOID)oPlayerOnDestroy, &hkPlayerOnDestroy, (LPVOID*)&oPlayerOnDestroy);

    pattern = "\x40\x56\x48\x83\xec\x00\x80\x3d\x00\x00\x00\x00\x00\x48\x8b\xf1\x75\x00\x48\x8d\x0d\x00\x00\x00\x00\xe8\x00\x00\x00\x00\x48\x8d\x0d\x00\x00\x00\x00\xe8\x00\x00\x00\x00\x48\x8d\x0d\x00\x00\x00\x00\xe8\x00\x00\x00\x00\x48\x8d\x0d\x00\x00\x00\x00\xe8\x00\x00\x00\x00\xc6\x05\x00\x00\x00\x00\x00\x33\xd2";
    mask = "xxxxx?xx?????xxxx?xxx????x????xxx????x????xxx????x????xxx????x????xx?????xx";

    oShadyGuyAwake = (ShadyGuy_Awake)(DWORD64)scanner.FindPattern("GameAssembly.dll", pattern, mask); // 0x479530
    MH_CreateHook((LPVOID)oShadyGuyAwake, &hkShadyGuyAwake, (LPVOID*)&oShadyGuyAwake);

    pattern = "\x33\xd2\xc7\x41\x00\x00\x00\x00\x00\xc7\x41\x00\x00\x00\x00\x00\xc7\x41\x00\x00\x00\x00\x00\xc7\x41\x00\x00\x00\x00\x00\xe9\x00\x00\x00\x00\xcc\xcc\xcc\xcc\xcc\xcc\xcc\xcc\xcc\xcc\xcc\xcc\xcc\x40\x53\x48\x83\xec\x00\x80\x3d\x00\x00\x00\x00\x00\x48\x8b\xd9\x75";
    mask = "xxxx?????xx?????xx?????xx?????x????xxxxxxxxxxxxxxxxxx?xx?????xxxx";

    oPickupConstr = (PickupConstr)(DWORD64)scanner.FindPattern("GameAssembly.dll", pattern, mask); // 0x482FC0
    MH_CreateHook((LPVOID)oPickupConstr, &hkPickupConstre, (LPVOID*)&oPickupConstr);

    pattern = "\x33\xd2\xc6\x41\x00\x00\xe9\x00\x00\x00\x00\xcc\xcc\xcc\xcc\xcc\x40\x53\x48\x83\xec\x00\x80\x3d\x00\x00\x00\x00\x00\x48\x8b\xd9\x75\x00\x48\x8d\x0d\x00\x00\x00\x00\xe8\x00\x00\x00\x00\x48\x8d\x0d\x00\x00\x00\x00\xe8\x00\x00\x00\x00\xc6\x05";
    mask = "xxxx??x????xxxxxxxxxx?xx?????xxxx?xxx????x????xxx????x????xx";

    oBaseInterfaceConstructor = (BaseInterfaceConstructor_t)(DWORD64)scanner.FindPattern("GameAssembly.dll", pattern, mask); // 0x475AC0
    MH_CreateHook((LPVOID)oBaseInterfaceConstructor, &hkBaseInterfaceConstructor, (LPVOID*)&oBaseInterfaceConstructor);
    

    MH_EnableHook(MH_ALL_HOOKS);



    
    std::cout << "[DLL] Konsola podłączona!" << std::endl;
    CreateThread(nullptr, 0, RenderThread, nullptr, 0, nullptr);
    return 0;
}




// Punkt wejścia DLL
BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
    if (ul_reason_for_call == DLL_PROCESS_ATTACH) {
        DisableThreadLibraryCalls(hModule);
        CreateThread(nullptr, 0, MainThread, nullptr, 0, nullptr);
        
        //CreateThread(nullptr, 0, , nullptr, 0, nullptr);
    }
    return TRUE;
}
