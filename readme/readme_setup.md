# Setup

## GCC / G++ >= 10 
- Install only if the system GCC / G++ is not at least version 10.
- **Default GCC / G++ on Jetpack 4 is lower than version 10**
```bash
sudo apt update && \
sudo apt install build-essential software-properties-common -y && \
sudo add-apt-repository ppa:ubuntu-toolchain-r/test -y && \
sudo apt update && \
sudo apt install gcc-10 g++-10 -y && \
sudo update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-10 60 --slave /usr/bin/g++ g++ /usr/bin/g++-10 && \
gcc -v
```
---

## VCPKG
### CMake
- Update CMake to at least 3.21.4 using APT
- For Jetson AGX, there is no CMake 3.21.4 on APT channel. Building from source is needed:
```bash
sudo apt-get install libssl-dev
sudo apt remove cmake

cd ~/Downloads
wget https://cmake.org/files/v3.21/cmake-3.21.4.tar.gz
tar xf cmake-3.21.4.tar.gz
cd cmake-3.21.4
./configure
sudo make install
cmake --version
```

### Clone and Install Vcpkg 
```bash
sudo apt install curl zip unzip tar
sudo apt install git
git clone https://github.com/Microsoft/vcpkg.git && ./vcpkg/bootstrap-vcpkg.sh
```
---

## VSCode (For Development)
1. Install extension:
    - C/C++
    - C/C++ Extension Pack
    - CMake
    - CMake Tools

2. Trigger CMake Extension
    - View > Command Palette > CMake: Quick Start

3. Create / Edit .vscode json
    - May have to replace **x64** to such as **arm64** etc. depending on the environment

    - .vscode/c_cpp_properties.json
    ```json
    {
        "configurations": [
            {
                "name": "Linux",
                "includePath": [
                    "${workspaceFolder}/**",
                    "./vcpkg_installed/x64-linux/include/",
                    "./vcpkg_installed/x64-linux/include/**"
                ],
                "defines": []
            }
        ],
        "version": 4
    }
    ```

    - .vscode/launch.json
    ```json
    {
        "configurations": [
            {
                "name": "(gdb) Launch",
                "type": "cppdbg",
                "request": "launch",
                "program": "${command:cmake.launchTargetPath}",
                "args": [],
                "stopAtEntry": false,
                "cwd": "${workspaceFolder}",
                "environment": [
                    {
                        "name": "PATH",
                        "value": "${env:PATH}:${command:cmake.getLaunchTargetDirectory}"
                    }
                ],
                "MIMode": "gdb",
                "setupCommands": [
                    {
                        "description": "Enable pretty-printing for gdb",
                        "text": "-enable-pretty-printing",
                        "ignoreFailures": true
                    }
                ]
            }
        ],
        "version": "2.0.0"
    }
    ```

    - .vscode/settings.json
    ```json
    {
        "terminal.integrated.env.linux": {
            "PATH": "vcpkg:${env:PATH}",
        },
        "cmake.configureSettings": {
            "CMAKE_TOOLCHAIN_FILE": "./vcpkg/scripts/buildsystems/vcpkg.cmake",
            "VCPKG_TARGET_TRIPLET": "x64-linux"
        },
        "files.associations": {
            "*.py": "python",
            "iostream": "cpp"
        },
        "C_Cpp.default.intelliSenseMode": "linux-clang-x64",
        "C_Cpp.default.configurationProvider": "ms-vscode.cmake-tools"
    }
    ```

    - (FOR JETSON) .vscode/settings.json
    ```json
    {
        "cmake.environment": {
            "VCPKG_FORCE_SYSTEM_BINARIES": "arm64"
        }
    }
    ```

---
