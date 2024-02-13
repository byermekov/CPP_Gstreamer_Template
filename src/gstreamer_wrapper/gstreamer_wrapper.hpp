#pragma once
#include <iostream>
#include <string>
#include <gst/gst.h>
#include <fmt/core.h>
#include <thread>
#include <gstreamer_wrapper_config.hpp>
#include <atomic>
#include "spdlog/spdlog.h"
#include <functional>

using namespace std;

class GstreamerWrapper 
{
  private:
    GstreamerWrapperConfig m_config;
    std::unique_ptr<std::thread> m_gsThread_ptr = nullptr;
    std::unique_ptr<std::atomic_bool> m_gsThreadStopRequested_ptr = nullptr;
    
    GstElement* m_gsPipeline_ptr = nullptr;



    // std::m_gsThread_exception_ptr 

    std::string buildPipelineString();
    void startGs();
    void stopGs(uint64_t waitForEos = 10);
    void startGsThread();
    void stopGsThread();
    void gsThreadDoWork();

    uint64_t getCurrentUnix();

    // hook functions
    std::function<void(GstElement*)> m_gsHookAfterBuild;


  public:   
    GstreamerWrapper(
      GstreamerWrapperConfig config,
      std::function<void(GstElement*)> gsHookAfterBuild = [](GstElement*) {}
    );
    // ~GstreamerWrapper();
    
    void init();

    void start();

    void stop();
};