#include <iostream>
#include <string>
#include <gst/gst.h>
#include <fmt/core.h>
#include <gstreamer_wrapper_config.hpp>
#include "json/json.h"

using namespace std;

GstreamerWrapperConfig::GstreamerWrapperConfig()
{

}

GstreamerWrapperConfig::GstreamerWrapperConfig(std::string fromJsonStr)
{
    loadFromJsonStr(fromJsonStr);    
}


void GstreamerWrapperConfig::loadFromJsonStr(std::string jsonStr)
{
    Json::Value root;
    Json::Reader reader;
    bool parsingSuccessful = reader.parse( jsonStr, root, false );
    if ( !parsingSuccessful )
        throw std::invalid_argument("parsing json failed");
    

    if (root["gs_pipeline"].isArray())
    {
        for (int i = 0; i < root["gs_pipeline"].size(); i++ )
            gs_pipeline.push_back(root["gs_pipeline"][i].asString());
    }
    if (root["gs_waitForEosOnStop"].isNumeric())
        gs_waitForEosOnStop = root["gs_waitForEosOnStop"].asUInt64();


    if (root["restartMonitor_loopSleep"].isNumeric())
        restartMonitor_loopSleep = root["restartMonitor_loopSleep"].asUInt64();
    
    if (root["restartMonitor_playingStateExpiry"].isNumeric())
        restartMonitor_playingStateExpiry = root["restartMonitor_playingStateExpiry"].asUInt64();
    
}