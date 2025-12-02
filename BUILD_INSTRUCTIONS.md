# CSOPESY Emulator - Build & Run Instructions

## Prerequisites

- **MinGW-w64 with POSIX threads** (via MSYS2 recommended)
- G++ compiler with C++17 support
- MSYS2 path: `C:\msys64\mingw64\bin` added to system PATH (or use full path)

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
g++ -std=c++17 -static main.cpp ConfigLoader.cpp Process.cpp Scheduler.cpp ConsoleManager.cpp -o csopesy_emulator.exe
```

Run:

```powershell
.\csopesy_emulator.exe
```

### Option B: Build with Dynamic Linking

Requires DLLs in the same folder or in PATH.

**Compile:**

```powershell
g++ -std=c++17 main.cpp ConfigLoader.cpp Process.cpp Scheduler.cpp ConsoleManager.cpp -o csopesy_emulator.exe
```

**Copy required DLLs** (from MinGW-w64 bin folder):

```powershell
Copy-Item C:\msys64\mingw64\bin\libstdc++-6.dll .
Copy-Item C:\msys64\mingw64\bin\libgcc_s_seh-1.dll .
Copy-Item C:\msys64\mingw64\bin\libwinpthread-1.dll .
```

**Run:**

```powershell
.\csopesy_emulator.exe
```

## Usage

1. Type `initialize` to load configuration from `config.txt`
2. Type `scheduler-start` to begin automatic process generation
3. Type `screen -ls` to list all processes
4. Type `screen -s <name>` to create and attach to a new process
5. Type `screen -r <name>` to reattach to an existing process
6. Type `report-util` to generate a report to `csopesy-log.txt`
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
