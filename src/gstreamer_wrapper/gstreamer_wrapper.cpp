#include <iostream>
#include <string>
#include <gst/gst.h>
#include <fmt/core.h>
#include <gstreamer_wrapper.hpp>
#include "spdlog/spdlog.h"
#include <chrono>
#include <functional>

using namespace std;

/******************************
* Public
*******************************/

/// @brief Constructor of GstreamerWrapper
/// @param config GstreamerWrapperConfig
/// @param gsHookAfterBuild void func(GstElement*). Will be invoked after gst_parse_launch
GstreamerWrapper::GstreamerWrapper(GstreamerWrapperConfig config, std::function<void(GstElement*)> gsHookAfterBuild)
{
    this->m_config = config;

    // std::atomic_bool gsThreadStopRequested(false);
    // m_gsThreadStopRequested_ptr = &gsThreadStopRequested;
    m_gsThreadStopRequested_ptr = std::unique_ptr<std::atomic_bool>(new std::atomic_bool());
    

    m_gsHookAfterBuild = gsHookAfterBuild;

}

void GstreamerWrapper::start()
{
    // startGs();
    startGsThread();
}


void GstreamerWrapper::stop()
{
    stopGsThread();
    // stopGs();
}


/******************************
* Private
*******************************/

/// @brief Build the gstreamer pipeline string
/// @return 
std::string GstreamerWrapper::buildPipelineString()
{
    std::string ret = "";
    for (int i = 0; i < m_config.gs_pipeline.size(); i++)
    {
        if (i != 0) 
            ret += " ! ";
        ret += m_config.gs_pipeline[i];
    }
    return ret;
}


/// @brief Start GS pipeline
void GstreamerWrapper::startGs()
{
    spdlog::info("GS init...");
    gst_init(NULL, NULL);    

    std::string pipelineStr = buildPipelineString();
    spdlog::info("Launching GS pipeline: {}", pipelineStr);
    m_gsPipeline_ptr = gst_parse_launch(pipelineStr.c_str(), nullptr);
    // fmt::print("gst_parse_launch called!!\n");

    m_gsHookAfterBuild(m_gsPipeline_ptr);

    spdlog::info("Setting playing state...");
    gst_element_set_state(m_gsPipeline_ptr, GST_STATE_PLAYING);
    // fmt::print("gst_element_set_state\n");

    // spdlog::info("Getting bus...");
    // m_gsBus_ptr = gst_element_get_bus(m_gsPipeline_ptr);
    // fmt::print("gst_element_get_bus\n");

}

/// @brief Stop GS pipeline
/// @param waitForEos millisecond for waiting eos signal
void GstreamerWrapper::stopGs(uint64_t waitForEos)
{
    if (m_gsPipeline_ptr == nullptr)
        throw std::logic_error("m_gsPipeline_ptr is null");
    
    spdlog::info("Stopping GS...");

    spdlog::info("Stopping GS (sending eos)...");
    gst_element_send_event (m_gsPipeline_ptr, gst_event_new_eos());


    spdlog::info("Stopping GS (waiting for eos)...");
    auto msg = gst_bus_timed_pop_filtered(gst_element_get_bus(m_gsPipeline_ptr), waitForEos * GST_MSECOND, static_cast<GstMessageType>(GST_MESSAGE_ERROR | GST_MESSAGE_EOS));
    if (msg == NULL)
        spdlog::warn("Cannot get eos message from bus. Maybe timed out.");
    else if (msg->type == GST_MESSAGE_ERROR)
        spdlog::warn("Expect GST_MESSAGE_EOS from bus but GST_MESSAGE_ERROR is found");


    spdlog::info("Stopping GS (setting state)...");
    gst_element_set_state (m_gsPipeline_ptr, GST_STATE_NULL);
    
    spdlog::info("Stopping GS (unrefencing)...");
    if (msg != NULL)
        gst_message_unref(msg);
    gst_object_unref(m_gsPipeline_ptr);
    m_gsPipeline_ptr = nullptr;

    spdlog::info("Stopped GS");
}

/// @brief Start GS monitoring thread
void GstreamerWrapper::startGsThread()
{
    if (m_gsThread_ptr != nullptr)
        throw std::logic_error("m_gsThread_ptr is not null");

    spdlog::info("Starting GS thread...");
    *m_gsThreadStopRequested_ptr = false;
    m_gsThread_ptr = std::unique_ptr<std::thread>(new std::thread(&GstreamerWrapper::gsThreadDoWork, this));
    // std::thread thread(&GstreamerWrapper::gs    ThreadDoWork, this);
    // m_gsThread_ptr = &thread;
    spdlog::info("Started GS thread");
}

/// @brief Stop the current gs monitoring thread
void GstreamerWrapper::stopGsThread()
{
    if (m_gsThread_ptr == nullptr)
        throw std::logic_error("m_gsThread_ptr is null");
    
    spdlog::info("Stopping GS thread...");

    *m_gsThreadStopRequested_ptr = true;
    m_gsThread_ptr->join();

    m_gsThread_ptr.release();
    m_gsThread_ptr = nullptr;
    spdlog::info("Stopped GS thread");
}


void GstreamerWrapper::gsThreadDoWork()
{   
    uint64_t lastPlayingAt = getCurrentUnix();
    bool isFirstLoop = true;

    while (!*m_gsThreadStopRequested_ptr)
    {
        // usleep(m_config.restartMonitor_loopSleep * 1000);
        std::this_thread::sleep_for(std::chrono::milliseconds(m_config.restartMonitor_loopSleep));
        auto currentUnix = getCurrentUnix();
        bool needRestart = false;

        if (m_gsPipeline_ptr == nullptr)
        {
            needRestart = true;
            if (!isFirstLoop)
                spdlog::warn("m_gsPipeline_ptr is null, starting GS...");
        }
        else
        {
            // last playing state check
            if (m_gsPipeline_ptr->current_state == GST_STATE_PLAYING)
                lastPlayingAt = currentUnix;
            else if (currentUnix - lastPlayingAt > m_config.restartMonitor_playingStateExpiry)
            {
                needRestart = true;
                spdlog::warn("{}ms since the last GST_STATE_PLAYING. Current state is {}. restarting GS...", currentUnix - lastPlayingAt, m_gsPipeline_ptr->current_state);
            }
        }


        if (needRestart)
        {
            if (m_gsPipeline_ptr != nullptr)
                stopGs();
            startGs();

            lastPlayingAt = currentUnix;
        }

        isFirstLoop = false;

    }

    stopGs();
}

uint64_t GstreamerWrapper::getCurrentUnix()
{
    return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();;
}