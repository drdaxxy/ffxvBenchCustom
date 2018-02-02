# ffxvbench

This patch allows you to customise graphics settings in the recently released FINAL FANTASY XV WINDOWS EDITION benchmark.

The benchmark normally sends performance data to Square Enix. This patch disables the data collection when running with customised settings so as to not taint their statistics.

### Usage

##### Step-by-step

* **[Download](https://github.com/drdaxxy/ffxvBenchCustom/releases/latest)** the current version
* **Extract `dinput8.dll`** from the archive into the folder where you installed the benchmark.
* **Create a custom configuration file** somewhere on your drive, e.g. `C:\Users\<username>\Documents\ffxv_custom.ini`. Refer to `ffxv_Lite.ini`, `ffxv_Standard.ini` and `ffxv_High.ini` for the standard presets.
* **Create a shortcut** to `ffxv.exe` (right-click `ffxv.exe` in the installation folder, "Create Shortcut").
* **Right-click the just created shortcut** (which should have appared in the installation folder, or on the desktop if you were asked to put it there), select Properties.
* At the end of "Target", after the closing quotation mark, **append**: ` --graphicsIni <path> --displayResolution <width> <height> --renderingResolution <width> <height> --locale=<en/jp>`, and `-f` if you want to run in fullscreen mode, and `--loop_mode` if you want an infinite loop.

**For example**: `"C:\Program Files\SquareEnix\FINAL FANTASY XV BENCHMARK\ffxv.exe" --graphicsIni "C:\Users\Noctis\Documents\ffxv_custom.ini" --displayResolution 1920 1080 --renderingResolution 3840 2160 --locale=en -f` to run the benchmark in English using the settings specified in `C:\Users\Noctis\Documents\ffxv_custom.ini` in fullscreen mode at 1080p, but rendering at 4K (supersampling), if you installed it to the default location, `C:\Program Files\SquareEnix\FINAL FANTASY XV BENCHMARK`.

##### Details

`ffxv.exe` commonly uses the following commandline parameters:

* `--graphicsIni <path>`: Path to the graphics configuration file. By default, the program will only accept hardcoded paths to standard configurations (with hardcoded settings in the executable). This patch lets you specify any file on your system.
* `--displayResolution <width> <height>`: Resolution of the window / display mode
* `--renderingResolution <width> <height>`: Resolution the benchmark is *rendered* at
* `-f`: Fullscreen mode
* `--locale=<en/jp>`: Language
* `--loop_mode`: Run in an infinite loop

The executable lists more options (`--ui_lang`, `--numThreads`, `--numAsyncThreads`, `--720p`, `--2160p`, `--noNvidiaAfterMath`) - these are not used by the original launcher (`ffxvbench.exe`) though. Feel free to experiment.

The default configurations (Lite / Standard / High) are provided in the release archive.

### Download

See the [Releases section](https://github.com/drdaxxy/ffxvBenchCustom/releases).
Please do not reupload in case I'll have to update this for some reason.

### Full INI parameters

The executable lists INI parameters not used in the standard presets. I have not looked into what types these are, whether they work as intended, or even whether they do anything at all, but here's the full list (just names):

```
[BasicSettings]

DisplayResolutionWH
RenderingResolutionWH
MaxFramerate
ShowFPS
ShowConfig
FixedFramerate

[DisplaySettings]

HardwareFullScreenMode
FullScreenModeOnStartup
HighPrecisionRenderTarget
FP16BackBuffer
DisplayScalingMode
VSync
HDRGammaScale
HDRLuminanceScale

[RenderingSettings]

HighSpecAsset
TextureStreamingMemory
TextureAnisotropicFilter
LightingQuality
AmbientOcclusion
ShadowResolution
ShadowDistanceScaling
ModelLODScaling
ScreenFilterDetail
Antialias
TextureSpec
ParticleSpec
HeightFieldSpec
ProceduralSkySpec
DisabledLightProbe

[NVIDIAGameWorksSettings]

NvidiaHairWorks
NvidiaVXAO
NvidiaHBAO
NvidiaTurf
NvidiaShadowLibs
NvidiaFlow
NvidiaTerrainTesselation
NvidiaAnsel
NvidiaHighlights
NvidiaSLI
```

(Technically, there's also `DisplayResolution` and `RenderingResolution`. These take the height and make the corresponding 16:9 resolution - e.g. `DisplayResolution = 1080` for 1080p - but they're overridden by `DisplayResolutionWH`/`RenderingResolutionWH` and the command-line parameters anyway)