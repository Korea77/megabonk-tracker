#pragma once
#include "utils.h"
#include <vector>
#include <map>
#include <algorithm>

template<typename T, typename Predicate>
size_t eraseAndCount(std::vector<T*>& vec, Predicate valid)
{
    vec.erase(
        std::remove_if(vec.begin(), vec.end(),
            [&](T* v) {
                return !v || !valid(v);
            }),
        vec.end()
                ); 
    return vec.size();
}

template<typename T, typename Predicate, typename Accumulator>
void eraseAndAccumulate(std::vector<T*>& vec, Predicate valid, Accumulator acc)
{
    vec.erase(
        std::remove_if(vec.begin(), vec.end(),
            [&](T* v) {
                if (!v || !valid(v))
                    return true;
                acc(v);
                return false;
            }),
        vec.end()
                );
}

// =====================================================
// GAME
// =====================================================

class _Game {
public:
    bool bSeed = false;
    bool bSeedLocked = false;
    int  iSeed = -1;

    bool bUnlockEverything = false;
    bool gameWorking = false;

    // =================================================
    // CHESTS
    // =================================================
    class {
    public:
        std::vector<InteractableChest*> list;

        int getFreeChests()
        {
            int count = 0;
            for (auto* c : list)
                if (c && c->chestType == 2 && !c->opening)
                    count++;
            return count;
        }

        int getOpenedChests()
        {
            int count = 0;
            eraseAndAccumulate(list,
                [](InteractableChest* c) { return true; },
                [&](InteractableChest* c) {
                    if (c->opening) count++;
                });
            return count;
        }

        int getChestCount() const
        {
            return static_cast<int>(list.size());
        }
    } Chests;

    // =================================================
    // MERCHANTS
    // =================================================
    class {
    public:
        std::vector<InteractableShadyGuy*> list;

        std::map<EItemRarity, int> getMerchants()
        {
            std::map<EItemRarity, int> result;

            eraseAndAccumulate(list,
                [](InteractableShadyGuy* m) {
                    return !m->done;
                },
                [&](InteractableShadyGuy* m) {
                    result[m->rarity]++;
                });

            return result;
        }

        int count() const
        {
            return static_cast<int>(list.size());
        }
    } Merchants;

    // =================================================
    // PICKUPS
    // =================================================
    class {
    public:
        std::vector<Pickup*> list;

        int getExpOnGround()
        {
            int exp = 0;

            eraseAndAccumulate(list,
                [](Pickup* p) {
                    return p->pickupType == 0 && !p->pickedUp;
                },
                [&](Pickup* p) {
                    exp += p->value;
                });

            return exp;
        }
    } Pickups;

    // =================================================
    // MOAI / SHRINES
    // =================================================
    class ShrineCounter {
    public:
        std::vector<MoaiData*> list;

        int count()
        {
            return static_cast<int>(
                eraseAndCount(list,
                    [](MoaiData* s) { return !s->done; }
                )
                );
        }
    };

    ShrineCounter MoaiShrines;
    ShrineCounter MagnetShrines;
    ShrineCounter CursedShrines;
    ShrineCounter ChallengeShrines;

    // =================================================
    // GREED SHRINES
    // =================================================
    class {
    public:
        std::vector<GreedData*> list;

        int count()
        {
            return static_cast<int>(
                eraseAndCount(list,
                    [](GreedData* g) { return !g->done; }
                )
                );
        }
    } GreedShrines;

    // =================================================
    // MICROWAVES
    // =================================================
    class {
    public:
        std::vector<MicroWave*> list;

        std::map<EItemRarity, int> getMicrowaves()
        {
            std::map<EItemRarity, int> result;

            eraseAndAccumulate(list,
                [](MicroWave* m) {
                    return m->usesLeft > 0;
                },
                [&](MicroWave* m) {
                    result[m->rarity]++;
                });

            return result;
        }
    } Microwaves;

    // =================================================
    // CHARGE SHRINES
    // =================================================
    class {
    public:
        std::vector<Data_ChargeShrine*> list;

        int count()
        {
            return static_cast<int>(
                eraseAndCount(list,
                    [](Data_ChargeShrine* c) { return !c->completed; }
                )
                );
        }
    } ChargableShrines;
};
