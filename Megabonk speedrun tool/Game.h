#include <vector>
#include "utils.h"
#include <map>

class _Game {
public:

    bool gameWorking = false;

    class _c {
    public:
        std::vector<InteractableChest*> list;

        int getFreeChests() {

            int count = 0;

            for (InteractableChest* var : this->list)
            {
                if (var->chestType == 2 && !var->opening) {
                    count++;
                }
            }

            return count;
        }

        int getChestCount() {
            return (int)this->list.size();
        }

        int getOpenedChest() {
            int count = 0;

            for (auto it = list.begin(); it != list.end(); ) {
                try {
                    InteractableChest* var = *it;
                    if (var->opening) {
                        count++;
                    }
                    ++it;
                }
                catch (...) {
                    it = list.erase(it);
                }
            }

            return count;
        }

    } Chests;



    class _c1 {
    public:
        std::vector<InteractableShadyGuy*> list;

        int count() {
            return this->list.size();
        }

        std::map<EItemRarity, int> getMerchants() {
            std::map<EItemRarity, int> map;

            for (auto it = this->list.begin(); it != this->list.end(); ) {
                try {
                    if (!*it) {
                        it = this->list.erase(it);
                        continue;
                    }

                    EItemRarity r = (*it)->rarity;
                    bool done = (*it)->done;
                    if (done) {
                        this->list.erase(it);
                        continue;
                    }
                    map[r]++;
                    ++it;

                }
                catch (...) {
                    it = this->list.erase(it);
                }
            }

            return map;
        }


    } Merchants;


    class _c2 {

    public:
        std::vector<Pickup*> list;


        int getExpOnGround() {
            unsigned int expOnGround = 0;

            for (auto it = list.begin(); it != list.end(); )
            {
                Pickup* pickup = *it;

                try
                {
                    if (!pickup)
                    {
                        it = list.erase(it);
                        continue;
                    }

                    if (pickup->pickupType != 0 || pickup->pickedUp == 1) {
                        it = list.erase(it);
                        continue;
                    }


                    expOnGround += pickup->value;

                    ++it;
                }
                catch (...)
                {
                    it = list.erase(it);
                }
            }
           return expOnGround;
        }


    } Pickups;

    class _c3 {
    public:
        std::vector<MoaiData*> list;

        int count() {
            int count = 0;

            for (auto it = list.begin(); it != list.end(); )
            {
                MoaiData* shrine = *it;

                try
                {
                    if (!shrine || shrine->done == 1)
                    {
                        it = list.erase(it);
                        continue;
                    }
                    count++;
                    ++it;
                }
                catch (...)
                {
                    it = list.erase(it);
                }
            }
            return count;
        }

    } MoaiShrines;

    _c3 MagnetShrines;
    _c3 CursedShrines;
    _c3 ChallengeShrines;
    class _c4 {
    public:
        std::vector<GreedData*> list;


        int count() {
            unsigned int count = 0;

            for (auto it = list.begin(); it != list.end(); )
            {
                GreedData* vv = *it;

                try
                {
                    if (!vv || vv->done == 1)
                    {
                        it = list.erase(it);
                        continue;
                    }


                    count++;

                    ++it;
                }
                catch (...)
                {
                    it = list.erase(it);
                }
            }
            return count;
        }
    } GreedShrines;

    class _c5 {
    public:
        std::vector< MicroWave* > list;
        std::map<EItemRarity, int> getMicrowaves() {
            std::map<EItemRarity, int> map;

            for (auto it = this->list.begin(); it != this->list.end(); ) {
                try {
                    if (!*it) {
                        it = this->list.erase(it);
                        continue;
                    }

                    EItemRarity r = (*it)->rarity;
                    int done = (*it)->usesLeft;
                    if (done == 0) {
                        this->list.erase(it);
                        continue;
                    }
                    map[r]++;
                    ++it;

                }
                catch (...) {
                    it = this->list.erase(it);
                }
            }

            return map;
        }
    } Microwaves;

};