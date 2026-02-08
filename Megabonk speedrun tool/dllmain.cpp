#include <windows.h>
#include <iostream>
#include <string>
#include <map>


#include "Sigscan.h"


#include "assert/MinHook.h"


#pragma comment(lib, "libMinHook.x64.lib")


#include "utils.h"
#include "Game.h"

#include "HookDX11.h"



_Game* Game = nullptr;




typedef void(__cdecl* tInitState)(int);
typedef void*(__fastcall* Random_Ctor_t)(void* _this, int seed);
Random_Ctor_t oRandom_Ctor = nullptr;
tInitState oInitState;


std::vector<void*> createdRandom;


void* __fastcall hkIl2CppRandom(void* rcx, int seed) {

    std::cout << "dziala tu " << seed << std::endl;

    if (Game->bSeed) {
        
        
        RemoveInvalidRandoms(createdRandom);

        if (createdRandom.size() > 0) {
            std::cout << "dziala tu" << std::endl;
            oInitState(Game->iSeed);


            for (auto pair : createdRandom) {
                oRandom_Ctor(pair, Game->iSeed);
            }


        }
        
        return oRandom_Ctor(rcx, Game->iSeed);
    
    
    }




    return oRandom_Ctor(rcx, seed);

    //std::cout << "dziala tu" << std::endl;
    //if (!Game->bSeed) {
    //    oRandom_Ctor(rcx, seed);
    //    return;
    //}

    //RemoveInvalidRandoms(createdRandom);

    //if (createdRandom.size() > 0) {
    //    std::cout << "dziala tu" << std::endl;
    //    oInitState(Game->iSeed);


    //    for (auto pair : createdRandom) {
    //        oRandom_Ctor(pair, (void*)Game->iSeed);
    //    }


    //}
    //



    //oRandom_Ctor(rcx, (void*)Game->iSeed);

}

typedef void(__fastcall* Random_Ctor_Default_t)(void* _this, void* rdx);
Random_Ctor_Default_t oRandom_Ctor_Default = nullptr;


void __fastcall hkRandom_Ctor_Default(void* _this, void* rdx)
{    
    createdRandom.push_back(_this);
    if (Game->bSeed) {
        oRandom_Ctor(_this, Game->iSeed);
        return;
    }

    //std::cout << "hkRandom_Ctor_Default called" << std::endl;

    oRandom_Ctor_Default(_this, rdx);




}



typedef void(__fastcall* PlayerOnDestroy)(void* rcx, void* rdx);
PlayerOnDestroy oPlayerOnDestroy = nullptr;

void __fastcall hkPlayerOnDestroy(void* rcx, void* rdx) {

    if (!Game->bSeedLocked && Game->bSeed) {
        Game->iSeed = GenerateRandomInt();
    }

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
    Game->ChargableShrines.list.clear();

    
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


typedef void(__fastcall* ChargeShrineAwake_t)(void* _this, void* rdx);
ChargeShrineAwake_t oChargeShrineAwake;

void __fastcall hkChargeShrineAwake(void* _this, void* rdx) {




    oChargeShrineAwake(_this, rdx);
    Game->ChargableShrines.list.push_back((Data_ChargeShrine*)_this);
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

typedef bool(__cdecl* achivment_IsUnlocked)(const char* unlockName, char& string);
achivment_IsUnlocked oachivment_IsUnlocked;

bool hkachivment_IsUnlocked(const char* unlockName, char& string) {
    return Game->bUnlockEverything ? true : oachivment_IsUnlocked(unlockName, string);
}

typedef void(__fastcall* shop_setunlocked)(void* rcx, void* rdx);
shop_setunlocked oshop_setunlocked;

void __fastcall hkshop_setunlocked(void* rcx, void* rdx) {
    oshop_setunlocked(rcx, rdx);
    //*((bool*)((DWORD64)rcx + 0x5A)) = true;
    //*((bool*)((DWORD64)rcx + 0x59)) = true;
    //*((bool*)((DWORD64)rcx + 0x5A)) = true;
}


DWORD WINAPI MainThread(LPVOID) {

    Game = new _Game();
    Game->iSeed = GenerateRandomInt();
    // for dev
    //AllocConsole();
    //FILE* f;
    //freopen_s(&f, "CONOUT$", "w", stdout);



    if (MH_Initialize() != MH_OK) {
        MessageBoxA(NULL, "ERROR", "MinHook init failed", 0);
        return 0;
    }






    HANDLE dll = 0;
    while (!dll) {
        dll = GetModuleHandleA("GameAssembly.dll");
    }
    //std::cout << "[+] FOUND DLL" << std::endl;
    SigScan scanner;

    const char* pattern = "\x48\x89\x5C\x24\x00\x57\x48\x83\xEC\x00\x80\x3D\x00\x00\x00\x00\x00\x8B\xFA\x48\x8B\xD9\x75\x00\x48\x8D\x0D\x00\x00\x00\x00\xE8\x00\x00\x00\x00\x48\x8D\x0D\x00\x00\x00\x00\xE8\x00\x00\x00\x00\xC6\x05\x00\x00\x00\x00\x00\x48\x8B\x0D\x00\x00\x00\x00\xBA";
    const char* mask = "xxxx?xxxx?xx?????xxxxxx?xxx????x????xxx????x????xx?????xxx????x";
      
    //auto InitState = (void(*)(int))((uintptr_t)GetModuleHandleA("GameAssembly.dll") + 0x218AFA0);

    //oachivment_IsUnlocked = (achivment_IsUnlocked)((uintptr_t)dll + 0x3DEBF0); // 0x3DF120
    //MH_CreateHook((LPVOID)oachivment_IsUnlocked, &hkachivment_IsUnlocked, (LPVOID*)&oachivment_IsUnlocked);

    //oshop_setunlocked = (shop_setunlocked)((DWORD64)dll + 0x528530);
    //MH_CreateHook((LPVOID)oshop_setunlocked, &hkshop_setunlocked, (LPVOID*)&oshop_setunlocked);
    oRandom_Ctor = (Random_Ctor_t)scanner.FindPattern("GameAssembly.dll", pattern, mask);

    //std::cout << "oRandom_Ctor Address: " << std::hex << scanner.FindPattern("GameAssembly.dll", pattern, mask) << std::dec << std::endl;
    
    pattern = "\x40\x53\x48\x83\xec\x00\x48\x8b\x05\x00\x00\x00\x00\x8b\xd9\x48\x85\xc0\x75\x00\x48\x8d\x0d\x00\x00\x00\x00\xe8\x00\x00\x00\x00\x48\x89\x05\x00\x00\x00\x00\x8b\xcb\x48\x83\xc4\x00\x5b\x48\xff\xe0\xcc\xcc\xcc\xcc\xcc\xcc\xcc\xcc\xcc\xcc\xcc\xcc\xcc\xcc\xcc\x48\x89\x5c\x24\x00\x57\x48\x83\xec\x00\x48\x8b\x05\x00\x00\x00\x00\x8b\xda\x8b\xf9\x48\x85\xc0\x75\x00\x48\x8d\x0d\x00\x00\x00\x00\xe8\x00\x00\x00\x00\x48\x89\x05\x00\x00\x00\x00\x8b\xd3\x8b\xcf\x48\x8b\x5c\x24\x00\x48\x83\xc4\x00\x5f\x48\xff\xe0\xcc\xcc\x48\x83\xec";
    mask = "xxxxx?xxx????xxxxxx?xxx????x????xxx????xxxxx?xxxxxxxxxxxxxxxxxxxxxxx?xxxx?xxx????xxxxxxxx?xxx????x????xxx????xxxxxxxx?xxx?xxxxxxxxx";
    oInitState = (tInitState)scanner.FindPattern("GameAssembly.dll", pattern, mask);
    
    //std::cout << "oInitState Address: " << std::hex << scanner.FindPattern("GameAssembly.dll", pattern, mask) << std::dec << std::endl;

    MH_CreateHook((LPVOID)oRandom_Ctor, &hkIl2CppRandom, (LPVOID*)&oRandom_Ctor);
    
    pattern = "\x40\x53\x48\x83\xec\x00\x80\x3d\x00\x00\x00\x00\x00\x48\x8b\xd9\x75\x00\x48\x8d\x0d\x00\x00\x00\x00\xe8\x00\x00\x00\x00\xc6\x05\x00\x00\x00\x00\x00\x48\x8b\x0d\x00\x00\x00\x00\x83\xb9\x00\x00\x00\x00\x00\x75\x00\xe8\x00\x00\x00\x00\x33\xc9\xe8\x00\x00\x00\x00\x45\x33\xc0\x8b\xd0";
    mask = "xxxxx?xx?????xxxx?xxx????x????xx?????xxx????xx?????x?x????xxx????xxxxx";

    //std::cout << "oRandom_Ctor_Default Address: " << std::hex << scanner.FindPattern("GameAssembly.dll", pattern, mask) << std::dec << std::endl;
    oRandom_Ctor_Default = (Random_Ctor_Default_t)scanner.FindPattern("GameAssembly.dll", pattern, mask);
    MH_CreateHook((LPVOID)oRandom_Ctor_Default, &hkRandom_Ctor_Default, (LPVOID*)&oRandom_Ctor_Default);


    pattern = "\x33\xd2\xc7\x41\x00\x00\x00\x00\x00\xc7\x41\x00\x00\x00\x00\x00\xc6\x81";
    mask = "xxxx?????xx?????xx";
    //std::cout << "oChargeShrineAwake Address: " << std::hex << scanner.FindPattern("GameAssembly.dll", pattern, mask) << std::dec << std::endl;
    oChargeShrineAwake = (ChargeShrineAwake_t)scanner.FindPattern("GameAssembly.dll", pattern, mask);
    MH_CreateHook((LPVOID)oChargeShrineAwake, &hkChargeShrineAwake, (LPVOID*)&oChargeShrineAwake);

    pattern = "\x48\x89\x5C\x24\x00\x55\x48\x83\xEC\x00\x80\x3D\x00\x00\x00\x00\x00\x48\x8B\xE9\x75\x00\x48\x8D\x0D\x00\x00\x00\x00\xE8\x00\x00\x00\x00\x48\x8D\x0D\x00\x00\x00\x00\xE8\x00\x00\x00\x00\x48\x8D\x0D\x00\x00\x00\x00\xE8\x00\x00\x00\x00\x48\x8D\x0D\x00\x00\x00\x00\xE8\x00\x00\x00\x00\x48\x8D\x0D\x00\x00\x00\x00\xE8\x00\x00\x00\x00\x48\x8D\x0D\x00\x00\x00\x00\xE8\x00\x00\x00\x00\x48\x8D\x0D\x00\x00\x00\x00\xE8\x00\x00\x00\x00\x48\x8D\x0D\x00\x00\x00\x00\xE8\x00\x00\x00\x00\x48\x8D\x0D\x00\x00\x00\x00\xE8\x00\x00\x00\x00\x48\x8D\x0D\x00\x00\x00\x00\xE8\x00\x00\x00\x00\xC6\x05\x00\x00\x00\x00\x00\x48\x8B\x05";
    mask = "xxxx?xxxx?xx?????xxxx?xxx????x????xxx????x????xxx????x????xxx????x????xxx????x????xxx????x????xxx????x????xxx????x????xxx????x????xxx????x????xx?????xxx";
    //std::cout << "oPlayerOnDestroy Address: " << std::hex << scanner.FindPattern("GameAssembly.dll", pattern, mask) << std::dec << std::endl;
    oPlayerOnDestroy = (PlayerOnDestroy)scanner.FindPattern("GameAssembly.dll", pattern, mask);
    MH_CreateHook((LPVOID)oPlayerOnDestroy, &hkPlayerOnDestroy, (LPVOID*)&oPlayerOnDestroy);
     
    pattern = "\x40\x56\x48\x83\xec\x00\x80\x3d\x00\x00\x00\x00\x00\x48\x8b\xf1\x75\x00\x48\x8d\x0d\x00\x00\x00\x00\xe8\x00\x00\x00\x00\x48\x8d\x0d\x00\x00\x00\x00\xe8\x00\x00\x00\x00\x48\x8d\x0d\x00\x00\x00\x00\xe8\x00\x00\x00\x00\x48\x8d\x0d\x00\x00\x00\x00\xe8\x00\x00\x00\x00\xc6\x05\x00\x00\x00\x00\x00\x33\xd2";
    mask = "xxxxx?xx?????xxxx?xxx????x????xxx????x????xxx????x????xxx????x????xx?????xx";
    //std::cout << "oShadyGuyAwake Address: " << std::hex << scanner.FindPattern("GameAssembly.dll", pattern, mask) << std::dec << std::endl;
    oShadyGuyAwake = (ShadyGuy_Awake)(DWORD64)scanner.FindPattern("GameAssembly.dll", pattern, mask); // 0x479530
    MH_CreateHook((LPVOID)oShadyGuyAwake, &hkShadyGuyAwake, (LPVOID*)&oShadyGuyAwake);

    pattern = "\x33\xd2\xc7\x41\x00\x00\x00\x00\x00\xc7\x41\x00\x00\x00\x00\x00\xc7\x41\x00\x00\x00\x00\x00\xc7\x41\x00\x00\x00\x00\x00\xe9\x00\x00\x00\x00\xcc\xcc\xcc\xcc\xcc\xcc\xcc\xcc\xcc\xcc\xcc\xcc\xcc\x40\x53\x48\x83\xec\x00\x80\x3d\x00\x00\x00\x00\x00\x48\x8b\xd9\x75";
    mask = "xxxx?????xx?????xx?????xx?????x????xxxxxxxxxxxxxxxxxx?xx?????xxxx";
    //std::cout << "oPickupConstr Address: " << std::hex << scanner.FindPattern("GameAssembly.dll", pattern, mask) << std::dec << std::endl;
    oPickupConstr = (PickupConstr)(DWORD64)scanner.FindPattern("GameAssembly.dll", pattern, mask); // 0x482FC0
    MH_CreateHook((LPVOID)oPickupConstr, &hkPickupConstre, (LPVOID*)&oPickupConstr);

    pattern = "\x33\xD2\xC6\x41\x00\x00\xE9\x00\x00\x00\x00\xCC\xCC\xCC\xCC\xCC\x48\x89\x5C\x24\x00\x48\x89\x74\x24\x00\x57\x48\x83\xEC\x00\x80\x3D\x00\x00\x00\x00\x00\x48\x8B\xF1";
    mask = "xxxx??x????xxxxxxxxx?xxxx?xxxx?xx?????xxx";
    //std::cout << "oBaseInterfaceConstructor Address: " << std::hex << scanner.FindPattern("GameAssembly.dll", pattern, mask) << std::dec << std::endl;
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
