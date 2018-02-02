#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>

typedef HRESULT(WINAPI *DirectInput8CreateProc)(HINSTANCE hinst,
                                                DWORD dwVersion, REFIID riidltf,
                                                LPVOID *ppvOut,
                                                LPUNKNOWN punkOuter);

static DirectInput8CreateProc realDirectInput8Create = NULL;
static HINSTANCE hRealDinput8 = NULL;

static bool patched = false;

void patch() {}

#pragma comment(linker, "/EXPORT:DirectInput8Create=ProxyDirectInput8Create")
extern "C" HRESULT WINAPI ProxyDirectInput8Create(HINSTANCE hinst,
                                                  DWORD dwVersion,
                                                  REFIID riidltf,
                                                  LPVOID *ppvOut,
                                                  LPUNKNOWN punkOuter) {
  if (!hRealDinput8) {
    TCHAR expandedPath[MAX_PATH];
    ExpandEnvironmentStrings(L"%WINDIR%\\System32\\dinput8.dll", expandedPath,
                             MAX_PATH);
    hRealDinput8 = LoadLibrary(expandedPath);
    if (!hRealDinput8)
      return DIERR_OUTOFMEMORY;
    realDirectInput8Create = (DirectInput8CreateProc)GetProcAddress(
        hRealDinput8, "DirectInput8Create");
    if (!realDirectInput8Create)
      return DIERR_OUTOFMEMORY;
  }
  return realDirectInput8Create(hinst, dwVersion, riidltf, ppvOut, punkOuter);
}

BOOLEAN WINAPI DllMain(IN HINSTANCE hDllHandle, IN DWORD nReason,
                       IN LPVOID Reserved) {
  if (nReason == DLL_PROCESS_ATTACH) {
    if (!patched) {
      patch();
      patched = true;
    }
  }
  return TRUE;
}