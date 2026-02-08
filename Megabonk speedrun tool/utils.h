#pragma once
#include <vector>

int GenerateRandomInt();
bool ValidateRandomInstance(void* randObjPtr);
void RemoveInvalidRandoms(std::vector<void*>& vec);


struct Il2CppClass;

struct Il2CppObject {
    Il2CppClass* klass;
    void* monitor;
};


struct InteractableChest {
    char _pad[0x58];

    int chestType;   // 0x58
    float rotation;  // 0x5C
    void* icon;      // 0x60 (Transform*)
    bool opening;    // 0x68
};

using il2cpp_object_get_class_t = void* (__cdecl*)(void* obj);
using il2cpp_class_get_type_t = void* (__cdecl*)(void* klass);
using il2cpp_type_get_name_t = const char* (__cdecl*)(void* type);


enum EItemRarity // TypeDefIndex: 6310
{
    Common = 0,
    Rare = 1,
    Epic = 2,
    Legendary = 3,
    Corrupted = 4,
    Quest = 5,
};

struct InteractableShadyGuy {

    // Fields
    char _pad[0x58];
    void* matRare; // 0x58
    void* matEpic; // 0x60
    void* matLegendary; // 0x68
    void* meshRenderer; // 0x70
    void* smokeFx; // 0x78
    void* purchaseSfx; // 0x80
    void* hideAfterPurchase; // 0x88
    EItemRarity rarity; // 0x90
    void* items; // 0x98
    void* prices; // 0xA0
    void* pricesMultipliers; // 0xA8
    void* dissapearText; // 0xB0
    bool done; // 0xB8

};

struct Pickup {
    char _pad[0x28];
    int pickupType;
    int value;
    bool pickedUp;
};

struct MoaiData {
    char _pad[0x68];
    bool done; // 0x68
};
struct GreedData {
    char _pad[0x70];
    bool done; // 0x68
};

struct MicroWave {
    char _pad[0x80];
    EItemRarity rarity; // 0x80
    int usesLeft; // 0x84;
};


struct Data_ChargeShrine
{
    // Fields
    char _pad[0xEC];
    bool completed; // 0xEC
};