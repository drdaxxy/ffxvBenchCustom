#include "ffxvBenchCustom.h"

#include <cstdint>
#include <cstdio>
#include <shellapi.h>
#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>

#include "SigScan.h"
#include <MinHook.h>

typedef HRESULT(WINAPI *DirectInput8CreateProc)(HINSTANCE hinst,
                                                DWORD dwVersion, REFIID riidltf,
                                                LPVOID *ppvOut,
                                                LPUNKNOWN punkOuter);

static DirectInput8CreateProc realDirectInput8Create = NULL;
static HINSTANCE hRealDinput8 = NULL;

static bool patched = false;
static bool configIsModified = false;

static const char LoadBenchmarkGraphicsIniSig[] =
    "48 8B C4 55 48 8B EC 48 ?? ?? ?? ?? ?? ?? 48 ?? ?? ?? ?? ?? ?? ?? 48 89 "
    "58 08 48 89 70 10 48 89 78 18 4C 89 70 20 48 8B FA";
typedef void(__fastcall *LoadBenchmarkGraphicsIniProc)(void *state,
                                                       const char *path);
static LoadBenchmarkGraphicsIniProc LoadBenchmarkGraphicsIniOrig = NULL;
static LoadBenchmarkGraphicsIniProc LoadBenchmarkGraphicsIniReal = NULL;

static const char EngineLoadStringSig[] =
    "40 53 48 83 EC 20 48 8B D9 48 8B 09 48 3B CA";
typedef int64_t(__fastcall *EngineLoadStringProc)(void *dst, const char *src);
static EngineLoadStringProc EngineLoadString = NULL;

static const char SubmitBenchmarkResultsSig[] =
    "40 55 56 57 48 8D 6C 24 B9 48 ?? ?? ?? ?? ?? ?? 48 ?? ?? ?? ?? ?? ?? ?? "
    "?? ?? ?? ?? ?? ?? ?? ?? 48 8B DA";
typedef void(__fastcall *SubmitBenchmarkResultsProc)(uintptr_t a1,
                                                     uintptr_t a2);
static SubmitBenchmarkResultsProc SubmitBenchmarkResultsOrig = NULL;
static SubmitBenchmarkResultsProc SubmitBenchmarkResultsReal = NULL;

static const char defaultIniPath[] =
    "config\\GraphicsConfig_BenchmarkMiddle.ini";

static const uintptr_t graphicsConfigOffset = 440;

void __fastcall SubmitBenchmarkResultsHook(uintptr_t a1, uintptr_t a2) {
    // let's try not to screw up SE's stat collection, shall we?
    if (configIsModified) return;

    return SubmitBenchmarkResultsReal(a1, a2);
}

void __fastcall LoadBenchmarkGraphicsIniHook(void *state, const char *path) {
  // path is cut off after a space, so we need to parse the commandline argument
  // ourselves
  const wchar_t *realpath = NULL;
  int nArgs;
  wchar_t **arglist = CommandLineToArgvW(GetCommandLineW(), &nArgs);
  for (int i = 0; i < nArgs; i++) {
    if (_wcsicmp(arglist[i], L"--graphicsIni") == 0 && i + 1 < nArgs) {
      realpath = arglist[i + 1];
      break;
    }
  }

  if (realpath == NULL) {
    return LoadBenchmarkGraphicsIniReal(state, defaultIniPath);
  }

  wchar_t expandedPath[MAX_PATH];
  ExpandEnvironmentStringsW(realpath, expandedPath, MAX_PATH);
  FILE *ini;
  errno_t err = _wfopen_s(&ini, expandedPath, L"rb");
  if (err != 0) {
    MessageBoxA(NULL,
                "Custom settings INI file not found/readable, continuing with "
                "default settings",
                "Custom settings", MB_OK);
    return LoadBenchmarkGraphicsIniReal(state, defaultIniPath);
  }

  configIsModified = true;

  fseek(ini, 0L, SEEK_END);
  int iniSz = ftell(ini);
  fseek(ini, 0L, SEEK_SET);
  char *text = (char *)calloc(iniSz, 1);
  fread(text, 1, iniSz, ini);
  fclose(ini);

  void *graphicsConfig = (void *)((uintptr_t)state + graphicsConfigOffset);
  EngineLoadString(graphicsConfig, text);
}

void patch() {
  MH_STATUS mhStatus = MH_Initialize();
  if (mhStatus != MH_OK) {
    MessageBoxA(NULL,
                "Custom settings failed to initialize, continuing with default "
                "settings",
                "Custom settings", MB_OK);
    return;
  }

  EngineLoadString = (EngineLoadStringProc)sigScan(EngineLoadStringSig);
  LoadBenchmarkGraphicsIniOrig =
      (LoadBenchmarkGraphicsIniProc)sigScan(LoadBenchmarkGraphicsIniSig);
  SubmitBenchmarkResultsOrig =
      (SubmitBenchmarkResultsProc)sigScan(SubmitBenchmarkResultsSig);

  if (EngineLoadString == NULL || LoadBenchmarkGraphicsIniOrig == NULL || SubmitBenchmarkResultsOrig == NULL) {
    MessageBoxA(NULL,
                "Failed to find functions, continuing with default settings",
                "Custom settings", MB_OK);
    return;
  }

  if (MH_CreateHook((void *)SubmitBenchmarkResultsOrig,
      (void *)SubmitBenchmarkResultsHook,
      (void **)&SubmitBenchmarkResultsReal) != MH_OK ||
      MH_EnableHook((void *)SubmitBenchmarkResultsOrig) != MH_OK ||
      MH_CreateHook((void *)LoadBenchmarkGraphicsIniOrig,
                    (void *)LoadBenchmarkGraphicsIniHook,
                    (void **)&LoadBenchmarkGraphicsIniReal) != MH_OK ||
      MH_EnableHook((void *)LoadBenchmarkGraphicsIniOrig) != MH_OK) {
    MessageBoxA(NULL,
                "Failed to hook functions, continuing with default settings",
                "Custom settings", MB_OK);
    return;
  }
}

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