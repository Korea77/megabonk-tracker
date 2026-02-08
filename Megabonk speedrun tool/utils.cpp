
#include <windows.h>
#include <random>
#include <limits>
#include <cstdint>


int GenerateRandomInt()
{
    static std::mt19937 gen{ std::random_device{}() };
    static std::uniform_int_distribution<int> dist(INT32_MIN, INT32_MAX);
    return dist(gen);
}


bool ValidateRandomInstance(void* randObjPtr)
{
    if (!randObjPtr)
        return false;

    bool ok = true;

    __try
    {
        int inext = *reinterpret_cast<int*>((uintptr_t)randObjPtr + 0x10);
        int inextp = *reinterpret_cast<int*>((uintptr_t)randObjPtr + 0x14);
        int* seedArr = *reinterpret_cast<int**>((uintptr_t)randObjPtr + 0x18);

        if (inext < 0 || inext  > 55) ok = false;
        if (inextp < 0 || inextp > 55) ok = false;

        if (!seedArr) ok = false;

        volatile int test = seedArr[0];
        (void)test;
    }
    __except (EXCEPTION_EXECUTE_HANDLER)
    {
        ok = false;
    }

    return ok;
}

void RemoveInvalidRandoms(std::vector<void*>& vec)
{
    std::vector<void*> valid;
    valid.reserve(vec.size());

    for (auto* p : vec)
    {
        if (ValidateRandomInstance(p))
        {
            valid.push_back(p);
        }
    }

    vec.swap(valid);
}
