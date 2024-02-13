#pragma once
#include <iostream>
#include <string>
#include <gst/gst.h>
#include <fmt/core.h>
#include <vector>

using namespace std;

class GstreamerWrapperConfig
{
    private:
        void loadFromJsonStr(std::string jsonStr);

    public:
        GstreamerWrapperConfig();
        GstreamerWrapperConfig(std::string fromJsonStr);

        
        vector<std::string> gs_pipeline;
        uint64_t gs_waitForEosOnStop = 10000;

        uint64_t restartMonitor_loopSleep = 100;
        uint64_t restartMonitor_playingStateExpiry = 10000;
        
};