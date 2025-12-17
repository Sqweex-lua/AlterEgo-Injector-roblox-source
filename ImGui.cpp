// @DarkCheatcc@DarkCheatcc@DarkCheatcc@DarkCheatcc@DarkCheatcc@DarkCheatcc@DarkCheatcc@DarkCheatcc

#include <Windows.h>
#include <iostream>
#include <cstdio>
#include <d3d11.h>
#include <dxgi.h>
#include <string>
#include <vector>
#include <sstream>
#include <iomanip>
#include <Psapi.h>

// @DarkCheatcc@DarkCheatcc@DarkCheatcc@DarkCheatcc@DarkCheatcc@DarkCheatcc
#define PIPE_NAME_W L"\\\\.\\pipe\\RobloxInjectorPipe"
#define PIPE_BUFFER_SIZE 4096

// @DarkCheatcc@DarkCheatcc@DarkCheatcc@DarkCheatcc@DarkCheatcc
#include "imgui-master/imgui.h" 
#include "imgui-master/backends/imgui_impl_win32.h"
#include "imgui-master/backends/imgui_impl_dx11.h" 

typedef long(__stdcall* PresentFn)(IDXGISwapChain*, UINT, UINT);
PresentFn oPresent = nullptr; 

ID3D11Device* pDevice = nullptr;
ID3D11DeviceContext* pContext = nullptr;
HWND hWnd = nullptr; 

bool bImGuiInitialized = false;
bool bShowMenu = true;
std::string lastReceivedScript = "No script received yet.";
DWORD_PTR* pSwapChainVMT = nullptr;

// тут адрес луа стейта
uintptr_t g_LuaStateAddress = 0; 
// ------------------------------

// Forward declarations
void SetupConsole();
bool InitializeHook();
void MainLoop(HMODULE hModule);
DWORD WINAPI PipeListenerThread(LPVOID lpParam);
uintptr_t FindPattern(HMODULE module, const char* pattern, const char* mask);
void FindRobloxAddresses();
void ExecuteLuaU(const std::string& script);

// аоб скан (тут тоже поменять над) @DarkCheatcc@DarkCheatcc@DarkCheatcc@DarkCheatcc@DarkCheatcc@DarkCheatcc

uintptr_t FindPattern(HMODULE module, const char* pattern, const char* mask) {
    MODULEINFO mi = {};
    if (!GetModuleInformation(GetCurrentProcess(), module, &mi, sizeof(mi))) {
        return 0; 
    }

    uintptr_t base = (uintptr_t)mi.lpBaseOfDll;
    size_t size = mi.SizeOfImage;

    size_t patternLength = strlen(mask);
    
    //@DarkCheatcc

    for (uintptr_t i = 0; i < size - patternLength; i++) {
        bool found = true;
        for (size_t j = 0; j < patternLength; j++) {
            if (mask[j] != '?' && pattern[j] != *(char*)(base + i + j)) {
                found = false;
                break;
            }
        }
        if (found) {
            return base + i;
        }
    }
    return 0;
}

void FindRobloxAddresses() {
    std::cout << "--- Starting Address Scan ---" << std::endl;
    
    HMODULE hModule = GetModuleHandle(NULL); 
    
    // @DarkCheatcc

    // заменитььь патерн и маск @DarkCheatcc@DarkCheatcc@DarkCheatcc@DarkCheatcc@DarkCheatcc@DarkCheatcc
    const char* LUA_STATE_PATTERN = "\x48\x8B\x0D\x00\x00\x00\x00\xE8\x00\x00\x00\x00\x48\x8B\xD8";
    const char* LUA_STATE_MASK = "xxx????x????xxx";

    uintptr_t result = FindPattern(hModule, LUA_STATE_PATTERN, LUA_STATE_MASK);
    
    if (result != 0) {
        // g_LuaStateAddress = calculated_address; 
        
        g_LuaStateAddress = result; 
        std::cout << "SUCCESS: Lua State Pattern Found at 0x" << std::hex << g_LuaStateAddress << std::dec << std::endl;
    } else {
        std::cout << "FAIL: Lua State Pattern" << std::endl;
    }
    
    // @DarkCheatcc
    std::cout << "Address Scan Complete" << std::endl;
}
// @DarkCheatcc


// executable 
void ExecuteLuaU(const std::string& script) {
    if (g_LuaStateAddress == 0) {
        lastReceivedScript = "Error: Lua State.";
        return;
    }

    lastReceivedScript = "Script (" + std::to_string(script.length()) + " bytes). TRYING...";
    std::cout << "(Received " << script.length() << " bytes)..." << std::endl;

    // суда над свой код 
    // rlua_State L из g_LuaStateAddress
    // rlua_load и rlua_pcall
}


// @DarkCheatcc
long __stdcall hkPresent(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags) {
    
    if (!bImGuiInitialized) {
        if (SUCCEEDED(pSwapChain->GetDevice(__uuidof(ID3D11Device), (void**)&pDevice))) {
            pDevice->GetImmediateContext(&pContext);
            
            DXGI_SWAP_CHAIN_DESC sd;
            pSwapChain->GetDesc(&sd);
            hWnd = sd.OutputWindow;
            
            ImGui::CreateContext();
            ImGuiIO& io = ImGui::GetIO(); (void)io;
            io.ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;
            
            ImGui_ImplWin32_Init(hWnd);
            ImGui_ImplDX11_Init(pDevice, pContext);
            
            bImGuiInitialized = true;
        }
    }
    
    if (bImGuiInitialized) {
        ImGui_ImplDX11_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();
        
        if (GetAsyncKeyState(VK_INSERT) & 1) { 
            bShowMenu = !bShowMenu;
        }

        if (bShowMenu) {
            ImGui::SetNextWindowSize(ImVec2(400, 300), ImGuiCond_Once);
            ImGui::Begin("AlterEgoDLL", nullptr, ImGuiWindowFlags_NoResize);
            
            ImGui::Text("DLL Status: HOOKED");
            ImGui::Separator();
            
            //@DarkCheatcc
            ImGui::Text("Lua State Address:");
            if (g_LuaStateAddress != 0) {
                ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "Found: 0x%p", (void*)g_LuaStateAddress);
            } else {
                ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Searching...");
            }

            ImGui::Text("Last Script Status:");
            ImGui::TextWrapped(lastReceivedScript.c_str()); 
            
            ImGui::Spacing();
            if (ImGui::Button("Unload")) {
            }
            ImGui::End();
        }
        
        ImGui::Render();
        ID3D11RenderTargetView* renderTarget = NULL;
        pContext->OMSetRenderTargets(1, &renderTarget, 0); 
        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
    }
    
    return oPresent(pSwapChain, SyncInterval, Flags); 
}

// ----------------------------------------------------
// @DarkCheatcc
// ----------------------------------------------------
DWORD WINAPI PipeListenerThread(LPVOID lpParam) {
    char scriptBuffer[PIPE_BUFFER_SIZE];
    DWORD dwRead;
    HANDLE hPipe;

    while (true) {
        hPipe = CreateNamedPipeW(
            PIPE_NAME_W,
            PIPE_ACCESS_DUPLEX,
            PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT,
            PIPE_UNLIMITED_INSTANCES,
            PIPE_BUFFER_SIZE,
            PIPE_BUFFER_SIZE,
            0,
            NULL
        );

        if (hPipe == INVALID_HANDLE_VALUE) {
            Sleep(100);
            continue;
        }

        if (ConnectNamedPipe(hPipe, NULL) != FALSE) {
            if (ReadFile(hPipe, scriptBuffer, PIPE_BUFFER_SIZE - 1, &dwRead, NULL) != FALSE) {
                scriptBuffer[dwRead] = '\0'; 
                
                ExecuteLuaU(scriptBuffer);
            }
        }

        DisconnectNamedPipe(hPipe);
        CloseHandle(hPipe);
    }
    return 0;
}


DWORD WINAPI MainThread(HMODULE hModule) {
    SetupConsole();
    std::cout << "DLL Injected!" << std::endl;

    //@DarkCheatcc
    FindRobloxAddresses();

    //@DarkCheatcc
    CreateThread(NULL, 0, PipeListenerThread, NULL, 0, NULL);
    std::cout << "Named Pipe Listener started." << std::endl;

    //@DarkCheatcc
    if (InitializeHook()) {
        std::cout << "Hooked successfully." << std::endl;
        MainLoop(hModule);
    } else {
        std::cerr << "ERROR: Failed to hook" << std::endl;
        FreeLibraryAndExitThread(hModule, 0); 
        return 1;
    }
    return 0;
}

void SetupConsole() {
    AllocConsole();
    FILE* f;
    freopen_s(&f, "CONOUT$", "w", stdout);
    SetConsoleTitleA("AlterEgo");
}

void MainLoop(HMODULE hModule) {
    while (!GetAsyncKeyState(VK_END)) { 
        Sleep(5); 
    }

    //@DarkCheatcc
    if (pSwapChainVMT && oPresent) {
        DWORD oldProtection;
        VirtualProtect(pSwapChainVMT + 8, sizeof(DWORD_PTR), PAGE_EXECUTE_READWRITE, &oldProtection);
        pSwapChainVMT[8] = (DWORD_PTR)oPresent;
        VirtualProtect(pSwapChainVMT + 8, sizeof(DWORD_PTR), oldProtection, &oldProtection);
    }

    if (bImGuiInitialized) {
        ImGui_ImplDX11_Shutdown();
        ImGui_ImplWin32_Shutdown();
        ImGui::DestroyContext();
        if (pDevice) pDevice->Release();
        if (pContext) pContext->Release();
    }

    FreeConsole();
    FreeLibraryAndExitThread(hModule, 0);
}

bool InitializeHook() { 
    WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC, DefWindowProc, 0L, 0L, GetModuleHandle(NULL), NULL, NULL, NULL, NULL, "DX", NULL };
    RegisterClassEx(&wc);
    HWND temp_hWnd = CreateWindow(wc.lpszClassName, "overlap dx11", WS_OVERLAPPEDWINDOW, 100, 100, 300, 300, NULL, NULL, wc.hInstance, NULL);

    D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_0;
    DXGI_SWAP_CHAIN_DESC sd = {};
    sd.BufferCount = 1;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = temp_hWnd;
    sd.SampleDesc.Count = 1;
    sd.Windowed = TRUE;
    sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

    ID3D11Device* pDummyDevice = nullptr;
    ID3D11DeviceContext* pDummyContext = nullptr;
    IDXGISwapChain* pDummySwapChain = nullptr;

    if (FAILED(D3D11CreateDeviceAndSwapChain(
        NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, 0, &featureLevel, 1, 
        D3D11_SDK_VERSION, &sd, &pDummySwapChain, &pDummyDevice, NULL, &pDummyContext))) {
        DestroyWindow(temp_hWnd);
        UnregisterClass(wc.lpszClassName, wc.hInstance);
        return false;
    }
    
    //@DarkCheatcc
    pSwapChainVMT = (DWORD_PTR*)pDummySwapChain;
    pSwapChainVMT = (DWORD_PTR*)pSwapChainVMT[0];

    oPresent = (PresentFn)pSwapChainVMT[8];
    
    DWORD oldProtection;
    VirtualProtect(pSwapChainVMT + 8, sizeof(DWORD_PTR), PAGE_EXECUTE_READWRITE, &oldProtection);
    pSwapChainVMT[8] = (DWORD_PTR)hkPresent;
    VirtualProtect(pSwapChainVMT + 8, sizeof(DWORD_PTR), oldProtection, &oldProtection);

    pDummySwapChain->Release();
    pDummyDevice->Release();
    pDummyContext->Release();
    DestroyWindow(temp_hWnd);
    UnregisterClass(wc.lpszClassName, wc.hInstance);
    
    return true;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
    switch (ul_reason_for_call) {
    case DLL_PROCESS_ATTACH:
        DisableThreadLibraryCalls(hModule);
        CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)MainThread, hModule, 0, NULL);
        break;
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;

}
