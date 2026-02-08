#pragma once

#include <Windows.h>
#include <Psapi.h>
#include <cstdint>
#include <cstddef>
#include <cstring>

#pragma comment(lib, "Psapi.lib")

class SigScan
{
private:
    MODULEINFO GetModuleInfo(const char* szModule)
    {
        MODULEINFO modinfo{};
        HMODULE hModule = GetModuleHandleA(szModule);
        if (hModule == nullptr) return modinfo;
        K32GetModuleInformation(GetCurrentProcess(), hModule, &modinfo, sizeof(MODULEINFO));
        return modinfo;
    }

public:
    uintptr_t FindPattern(const char* module, const char* pattern, const char* mask)
    {
        if (!module || !pattern || !mask) return 0;

        MODULEINFO mInfo = GetModuleInfo(module);
        if (mInfo.lpBaseOfDll == nullptr || mInfo.SizeOfImage == 0) return 0;

        const uintptr_t base = reinterpret_cast<uintptr_t>(mInfo.lpBaseOfDll);
        const size_t size = static_cast<size_t>(mInfo.SizeOfImage);
        const size_t patternLength = std::strlen(mask);

        if (patternLength == 0 || patternLength > size) return 0;

        for (size_t i = 0; i + patternLength <= size; ++i)
        {
            bool found = true;
            for (size_t j = 0; j < patternLength; ++j)
            {
                char m = mask[j];
                if (m != '?' && pattern[j] != *reinterpret_cast<char*>(base + i + j))
                {
                    found = false;
                    break;
                }
            }
            if (found)
                return base + i;
        }

        return 0;
    }
};
