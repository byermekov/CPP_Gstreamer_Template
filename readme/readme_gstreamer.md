# Readme (Gstreamer)

## Install
- Reference: https://gstreamer.freedesktop.org/documentation/installing/on-linux.html?gi-language=c
- Gstreamer has to be installed in the system and link using CMakeLists.txt

- For Jetson, Gstreamer (with hardware acceleration) has installed by jetpack by default 

- For Ubuntu, install Gstreamer from apt channel
```bash
sudo apt-get install libgstreamer1.0-dev libgstreamer-plugins-base1.0-dev libgstreamer-plugins-bad1.0-dev gstreamer1.0-plugins-base gstreamer1.0-plugins-good gstreamer1.0-plugins-bad gstreamer1.0-plugins-ugly gstreamer1.0-libav gstreamer1.0-tools gstreamer1.0-x gstreamer1.0-alsa gstreamer1.0-gl gstreamer1.0-gtk3 gstreamer1.0-qt5 gstreamer1.0-pulseaudio
```

### Hardware Accleration
- Verify hardware accleration after installation
- Output will be empty if there is no hardware accleration
```bash
gst-inspect-1.0 | grep 'nvenc\|nvdec\|nvcodec'
```

- For Ubuntu <= 20.04, the Gstreamer installed from the apt channel does not include hardware accleration. Building from source is needed.


#### Building nvenc & nvdec From Source

0. Prerequisite 
    - Nvidia Driver & CUDA installed
    - sudo apt install build-essential

1. Before building, check the gst version first
    ```bash
    gst-launch-1.0 --gst-version
    GStreamer Core Library version 1.16.3
    ```

2. Git clone the source and checkout the specific version
    ```bash
    mkdir ~/gstreamer_build
    cd ~/gstreamer_build
    git clone https://github.com/GStreamer/gst-plugins-bad

    cd gst-plugins-bad
    git checkout 1.16.3
    ```

3. Build
    ```bash
    cd ~/gstreamer_build/gst-plugins-bad
    NVENCODE_CFLAGS="-I/home/risksis/gstreamer_build/gst-plugins-bad/sys/nvenc" ./autogen.sh --with-cuda-prefix="/usr/local/cuda" --disable-gtk-doc
    cd ~/gstreamer_build/gst-plugins-bad/sys/nvenc
    make
    cd ~/gstreamer_build/gst-plugins-bad/sys/nvdec
    make
    ```


4. Install plugin nvenc & nvdec
    ```bash
    sudo cp ~/gstreamer_build/gst-plugins-bad/sys/nvenc/.libs/libgstnvenc.so /usr/lib/x86_64-linux-gnu/gstreamer-1.0/
    sudo cp ~/gstreamer_build/gst-plugins-bad/sys/nvdec/.libs/libgstnvdec.so /usr/lib/x86_64-linux-gnu/gstreamer-1.0/
    ```

5. Verify & cleanup   
    ```bash
    gst-inspect-1.0 | grep 'nvenc\|nvdec\|nvcodec'
    ```
    - Tip: Deleting ~/.cache/gstreamer-1.0 may help to refresh the plugin list
    - ~/gstreamer_build is no longer needed after installation



## Include Gstreamer in CmakeLists.txt

```cmake
message(STATUS "Using gstreamer installed in system")
find_package(PkgConfig REQUIRED)
pkg_check_modules(GSTREAMER REQUIRED IMPORTED_TARGET gstreamer-1.0)
target_link_libraries(${PROJECT_NAME} PRIVATE PkgConfig::GSTREAMER)
```


## Useful cmds

### list all installed plugin
- gst-inspect-1.0

### Show info of specific plugin name
- gst-inspect-1.0 [plugin name]

### Launch gst pipeline
- gst-launch-1.0 [pipeline str]
