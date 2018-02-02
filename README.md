# ffxvbench

This patch allows you to customise graphics settings in the recently released FINAL FANTASY XV WINDOWS EDITION benchmark.

The benchmark normally sends performance data to Square Enix. This patch disables the data collection when running with customised settings so as to not taint their statistics.

To use, extract `dinput8.dll` to the installation folder, create your configuration, then run `ffxv.exe` with the appropriate parameters:

* `--graphicsIni <path>`: Path to the graphics configuration file. By default, the program will only accept hardcoded paths to standard configurations (with hardcoded settings in the executable). This patch lets you specify any file on your system.
* `--displayResolution <width> <height>`: Resolution of the window / display mode
* `--renderingResolution <width> <height>`: Resolution the benchmark is *rendered* at
* `-f`: Fullscreen mode
* `--locale=<en/jp>`: Language
* `--loop_mode`: Run in an infinite loop

The executable lists more options (`--ui_lang`, `--numThreads`, `--numAsyncThreads`, `--720p`, `--2160p`, `--noNvidiaAfterMath`) - these are not used by the original launcher (`ffxvbench.exe`) though.

For example, you could use `ffxv.exe --graphicsIni "%USERPROFILE%\Documents\ffxv_custom.ini" --displayResolution 1920 1080 --renderingResolution 3840 2160 --locale=en -f` to run the benchmark in English using the settings specified in `%USERPROFILE%\Documents\ffxv_custom.ini` (e.g. `C:\Users\<username>\Documents\ffxv_custom.ini`) in fullscreen mode at 1080p, but rendering at 4K (supersampling).

The default configurations (Lite / Standard / High) are provided in the release archive.