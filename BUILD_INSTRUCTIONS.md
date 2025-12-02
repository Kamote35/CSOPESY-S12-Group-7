# CSOPESY Emulator - Build & Run Instructions

## Prerequisites

- **MinGW-w64 with POSIX threads** (via MSYS2)
- G++ compiler with C++17 support

## Installing the Compiler (MSYS2 + MinGW-w64)

### Step 1: Download MSYS2

1. Go to <https://www.msys2.org/>
2. Click the download button to get the installer (e.g., `msys2-x86_64-XXXXXXXX.exe`)
3. Run the installer
4. Use the default installation path: `C:\msys64`
5. Complete the installation and launch MSYS2

### Step 2: Install MinGW-w64 GCC Compiler

1. Open **MSYS2 MINGW64** from the Start Menu (important: use MINGW64, not MSYS2)
2. Update the package database:
   ```bash
   pacman -Syu
   ```
3. If prompted to close the terminal, close it and reopen **MSYS2 MINGW64**
4. Install the C++ compiler and tools:
   ```bash
   pacman -S mingw-w64-x86_64-gcc mingw-w64-x86_64-make
   ```
5. Press `Y` to confirm installation

### Step 3: Add MinGW-w64 to System PATH

1. Press `Win + R`, type `sysdm.cpl`, and press Enter
2. Go to the **Advanced** tab
3. Click **Environment Variables**
4. Under **System variables**, find and select `Path`, then click **Edit**
5. Click **New** and add:
   ```
   C:\msys64\mingw64\bin
   ```
6. Click **OK** on all dialogs to save

### Step 4: Verify Installation

1. Open a **new** PowerShell or Command Prompt window
2. Run:
   ```powershell
   g++ --version
   ```
3. You should see output like:
   ```
   g++.exe (Rev8, Built by MSYS2 project) 15.x.x
   ```

If you see the version, the compiler is ready!

## Files Required

```
main.cpp
ConfigLoader.cpp
ConfigLoader.h
ConsoleManager.cpp
ConsoleManager.h
Process.cpp
Process.h
Scheduler.cpp
Scheduler.h
Global.h
config.txt
```

## Build Instructions

### Option A: Build with Static Linking (Recommended)

No DLLs needed - produces a standalone executable.

```powershell
g++ -std=c++17 -static main.cpp ConfigLoader.cpp Process.cpp Scheduler.cpp ConsoleManager.cpp -o os_emulator.exe
```

Run:

```powershell
.\os_emulator.exe
```

### Option B: Build with Dynamic Linking

Requires DLLs in the same folder or in PATH.

**Compile:**

```powershell
g++ -std=c++17 main.cpp ConfigLoader.cpp Process.cpp Scheduler.cpp ConsoleManager.cpp -o os_emulator.exe
```

**Copy required DLLs** (from MinGW-w64 bin folder):

```powershell
Copy-Item C:\msys64\mingw64\bin\libstdc++-6.dll .
Copy-Item C:\msys64\mingw64\bin\libgcc_s_seh-1.dll .
Copy-Item C:\msys64\mingw64\bin\libwinpthread-1.dll .
```

**Run:**

```powershell
.\os_emulator.exe
```

## Usage

1. Type `initialize` to load configuration from `config.txt`
2. Type `scheduler-start` to begin automatic process generation
3. Type `screen -ls` to list all processes
4. Type `screen -s <name>` to create and attach to a new process
5. Type `screen -r <name>` to reattach to an existing process
6. Type `report-util` to generate a report to `os-log.txt`
7. Type `scheduler-stop` to stop the scheduler
8. Type `exit` to quit

## Configuration (config.txt)

```
num-cpu 4
scheduler "rr"
quantum-cycles 5
batch-process-freq 1
min-ins 1000
max-ins 2000
delay-per-exec 0
```

## Troubleshooting

### Error: `'thread' in namespace 'std' does not name a type`

Your MinGW installation doesn't support `std::thread`. Install MinGW-w64 with POSIX threads via MSYS2:

1. Install MSYS2 from https://www.msys2.org/
2. Open MSYS2 MINGW64 terminal
3. Run: `pacman -S mingw-w64-x86_64-gcc`
4. Add `C:\msys64\mingw64\bin` to your system PATH

### Error: Program exits immediately / crashes

The executable can't find required DLLs. Either:
- Compile with `-static` flag, OR
- Copy the DLLs to the same folder as the executable, OR
- Add `C:\msys64\mingw64\bin` to your system PATH
