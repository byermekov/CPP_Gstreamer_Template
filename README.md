# CPP Gstreamer template



## Notes

### GStreamer & Opencv4
- If GStreamer is installed using vcpkg, there will be no plugins. Thus, must be installed using apt in the system.
- This two packages must be installed on the system instead of vcpkg.
- This is because they both require Glib to run. The glib used must be the same. Using vcpkg will create multiple glib versions causing errors during runtime.



## Code
### gstreamer_wrapper
- gstreamer_wrapper : Wrapper class using Gstreamer to build pipeline
- The function gsHookAfterBuild() in main.cpp is passed to the wrapper constructor for adding a probe to intercept a BGR frame



## Performance tunning

### Enable CUDA lazy loading to reduce VRAM usage
```bash
export CUDA_MODULE_LOADING=LAZY
```




### Adding queue element in between GST pipeline
- Adding queue around inference can improve overall throughput
- It is also worth to try Wrapping other element with queue may also increase throughput.
- After adding queue, make sure set sink=false for sink

#### Delays
- To control delays, adjust max-size-time, max-size-bytes, max-size-buffers and set leaky=2
- Example of allowing accumulate at most 5 seocnds before purge
```
queue max-size-time=5000000000 max-size-bytes=0 max-size-buffers=0 leaky=2
```

#### Example
```json
"gs_pipeline": [
    "filesrc location=\"YOUR_FILE_LOCATION\"",
    "decodebin", 
    "videoconvert",
    "queue max-size-time=1000000000 max-size-bytes=0 max-size-buffers=0 leaky=2", // Add queue
    "capsfilter caps=\"video/x-raw,format=BGR\" name=frameout",
    "queue max-size-time=1000000000 max-size-bytes=0 max-size-buffers=0 leaky=2", // Add queue
    "videoconvert",
    "fpsdisplaysink sync=false" // set sync = false
]
```


### Using HW decoder
#### Use nvh264dec
- Decode H264 using Nvidia GPU
- Not available on Jetson
- Example
```json
"gs_pipeline": [
    "rtspsrc location=YOUR_RTSP_SRC latency=10",
    "rtph264depay",
    "h264parse",
    "nvh264dec", // Hardware decode
    "queue max-size-time=250000000 max-size-bytes=0 max-size-buffers=0 leaky=2",
    "videoconvert",
    "queue max-size-time=2500000000 max-size-bytes=0 max-size-buffers=0 leaky=2",
    "capsfilter caps=\"video/x-raw, format=BGR\" name=frameout",
    "queue max-size-time=250000000 max-size-bytes=0 max-size-buffers=0 leaky=2",
    "videoconvert",
    "fpsdisplaysink sync=false"
]
```

### Use nvv4l2decoder
- Available on Jetson
- Use nvvidconv afterward to convert GPU mat to CPU
```json
"gs_pipeline": [
    "rtspsrc location=YOUR_RTSP_SRC",
    "rtph264depay",
    "h264parse",
    "nvv4l2decoder", "nvvidconv", "video/x-raw,format=BGRx", // Hardware decode
    "queue max-size-time=250000000 leaky=2",
    "videoconvert",
    "queue max-size-time=500000000 max-size-bytes=0 max-size-buffers=0 leaky=2",
    "capsfilter caps=\"video/x-raw, format=BGR\" name=frameout",
    "queue max-size-time=250000000 leaky=2",
    "fpsdisplaysink video-sink=nveglglessink sync=false"
]
```


### Building

#### Use VSCode CMake Extension
- Click the "Build" button in the CMake bar of VSCode to start building
- For deployment, Make sure the building profile is "CMake: [Release]"
![VSCode CMake Bar](images/readme_template_vscode_bar.png)
