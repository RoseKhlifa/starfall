#include "../includes.h"
#include "kernel.h"
#include <vector>
#include "kernel.h"
#include <winternl.h>
#pragma comment(lib, "ntdll.lib")

namespace utils
{
  inline  uintptr_t get_process_base_id(DWORD processID)
    {
        DWORD_PTR   baseAddress = 0;
        HANDLE      processHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, processID);
        HMODULE* moduleArray;
        LPBYTE      moduleArrayBytes;
        DWORD       bytesRequired;

        if (processHandle)
        {
            if (EnumProcessModules(processHandle, NULL, 0, &bytesRequired))
            {
                if (bytesRequired)
                {
                    moduleArrayBytes = (LPBYTE)LocalAlloc(LPTR, bytesRequired);

                    if (moduleArrayBytes)
                    {
                        unsigned int moduleCount;

                        moduleCount = bytesRequired / sizeof(HMODULE);
                        moduleArray = (HMODULE*)moduleArrayBytes;

                        if (EnumProcessModules(processHandle, moduleArray, bytesRequired, &bytesRequired))
                        {
                            baseAddress = (DWORD_PTR)moduleArray[0];
                        }

                        LocalFree(moduleArrayBytes);
                    }
                }
            }

            CloseHandle(processHandle);
        }

        return baseAddress;
    }

    typedef struct _SYSTEM_MODULE {
        PVOID  Reserved1;
        PVOID  Reserved2;
        PVOID  ImageBase;
        ULONG  ImageSize;
        ULONG  Flags;
        USHORT Index;
        USHORT Unknown;
        USHORT LoadCount;
        USHORT ModuleNameOffset;
        CHAR   ImageName[256];
    } SYSTEM_MODULE, * PSYSTEM_MODULE;

    typedef struct _SYSTEM_MODULE_INFORMATION {
        ULONG NumberOfModules;
        SYSTEM_MODULE Modules[1];
    } SYSTEM_MODULE_INFORMATION, * PSYSTEM_MODULE_INFORMATION;

    inline uintptr_t GetDriverModuleBase(const char* driver) {
        ULONG size = 0;
        NTSTATUS status = NtQuerySystemInformation((SYSTEM_INFORMATION_CLASS)0x0B, nullptr, 0, &size);

        if (status != 0xC0000004) {
            return 0;
        }

        std::vector<BYTE> buffer(size);
        PSYSTEM_MODULE_INFORMATION moduleInfo = reinterpret_cast<PSYSTEM_MODULE_INFORMATION>(buffer.data());

        status = NtQuerySystemInformation((SYSTEM_INFORMATION_CLASS)0x0B, moduleInfo, size, &size);
        if (!NT_SUCCESS(status)) {
            return 0;
        }

        for (ULONG i = 0; i < moduleInfo->NumberOfModules; i++) {
            SYSTEM_MODULE& mod = moduleInfo->Modules[i];
            const char* moduleName = strrchr(mod.ImageName, '\\');
            if (moduleName) moduleName++; else moduleName = mod.ImageName;
            if (_stricmp(moduleName, driver) == 0) {
                return (uintptr_t)mod.ImageBase;
            }
        }
        return 0;
    }


      typedef struct ShadowRegionsDataStructure
    {
        uintptr_t OriginalPML4_t;
        uintptr_t ClonedPML4_t;
        uintptr_t GameCr3;
        uintptr_t ClonedCr3;
        uintptr_t FreeIndex;
    } ShadowRegionsDataStructure;



    inline   uint64_t find_RAX()
    {
        int cpuInfo[4] = { 0 };
        __cpuid(cpuInfo, 1);
        return (uint64_t)cpuInfo[0];
    }

    inline uintptr_t decryptedClonedCR3(uintptr_t vgkbase) {
        uint64_t _RAX = find_RAX(); // EAX
        BYTE byte_1400839C0 = km::RPM<BYTE>(vgkbase + 0x839C0);
        uint64_t key = km::RPM<uint64_t>(vgkbase + 0x83910);

        uint64_t v5 = byte_1400839C0 & 0x73;
        uint64_t v11 = ~(int)_RAX;

        uint64_t v12 =
            (0x8000000000000001ULL * (int)_RAX
                + 0xAFDBF65F8A4AC9C9ULL * ~key
                + 0x2FDBF65F8A4AC9C9ULL * (key + 1)
                + (((key ^ (int)_RAX) << 63) ^ 0x8000000000000000ULL))
            *
            (0x7D90DC33C620C593ULL * key * (0x13D0F34E00000000ULL * key + 0x483C4F8900000000ULL)
                + 0xFD90DC33C620C592ULL * ~(key * (0x13D0F34E00000000ULL * key + 0x483C4F8900000000ULL))
                + (key *
                    (0xCE3CE5E180000000ULL * ~key
                        + 0x55494E5B80000000ULL * (int)_RAX
                        + 0xC83B18136241A38DULL * v11
                        + 0x72F1C9B7E241A38DULL *
                        (
                            ((int)_RAX | 0x3F71D992FBB2CCEBULL) -
                            (0x3F71D992FBB2CCEAULL - ((int)_RAX & 0x3F71D992FBB2CCEBULL))
                            )
                        )
                    + 0x71C31A1E80000000ULL)
                *
                (0x99BF7D2380CF6EC3ULL * (int)_RAX
                    + 0x664082DC7F30913EULL * (v5 | key)
                    + 0x19BF7D2380CF6EC2ULL * v11
                    + 0xE64082DC7F30913EULL * (~key & ~v5)
                    + ((key + (v5 | (int)_RAX) + (v5 & (key ^ (int)_RAX))) << 63))
                + 0x2183995CC620C592ULL);

        uint64_t expr =
            0x49B74B6480000000ULL * key +
            0xC2D8B464B4418C6CULL * ~v5 +
            0x66B8CDC1FFFFFFFFULL * v5 +
            0x5C1FE6A2B4418C6DULL * ((v5 & key) - (key | ~v5));

        uint64_t logic =
            -3 * ~key +
            (v5 ^ key) +
            -2 * (v5 ^ (v5 | key)) +
            2 * (((v5 | 0x3F71D992FBB2CCEBULL) ^ 0xC08E266D044D3314ULL) + (~key | v5 ^ 0x3F71D992FBB2CCEBULL)) -
            (v5 ^ ~key ^ 0x3F71D992FBB2CCEBULL) -
            0x3F71D992FBB2CCEBULL;

        uint64_t v13 =
            ((~(expr * logic) ^ ~v12) << 63) +
            0x6C80110954C7304DULL *
            (
                ((int)_RAX & (expr * logic)) -
                (v11 & ~(expr * logic)) -
                (int)_RAX
                );

        uint64_t DecryptedCR3 =
            0x137FEEF6AB38CFB4ULL * (expr * logic) +
            v13 -
            0x7FFFFFFFFFFFFFFFULL * v12 -
            0x4F167C5CD4C7304EULL;

        return DecryptedCR3;
    }


    inline  uintptr_t find_pml4_base()
    {
        ShadowRegionsDataStructure Data = km::RKA<ShadowRegionsDataStructure>(km::vgk + 0x838F8);
        uintptr_t DecClonedCr3 = decryptedClonedCR3(km::vgk);
        if (!DecClonedCr3) return 0;
        km::cr3_context = DecClonedCr3;
        uintptr_t source = Data.FreeIndex << 39;
        return source;
    }
}