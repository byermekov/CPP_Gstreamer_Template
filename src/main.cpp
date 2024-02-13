#include <iostream>
#include <fstream>
#include <sstream>

#include "gstreamer_wrapper/gstreamer_wrapper.hpp"
#include "gstreamer_wrapper/gstreamer_wrapper_config.hpp"

// #include "ort_wrapper/mask_rcnn_config.hpp"

#include "spdlog/spdlog.h"

#include <signal.h>
#include <opencv2/opencv.hpp>
#include <file_util.hpp>

#include <color_util.hpp>


/**************************************************
# Global Vars
**************************************************/
bool g_loopEnable = true;

// auto g_detectorConfig = MMDeployGenericDetectorConfig(
//     FileUtil::readTextFromFile("../ort_mmdeploy_generic_detector_config.json"));

// auto g_detector = MMDeployGenericDetector(&g_detectorConfig);



/**************************************************
# Functions
**************************************************/

void interupt_handler(sig_atomic_t s)
{
    g_loopEnable = false;
    spdlog::warn("Interupted");
}

GstPadProbeReturn cb_have_data(GstPad *pad, GstPadProbeInfo *info, gpointer user_data)
{
    GstMapInfo map;
    GstBuffer *buffer;

    buffer = GST_PAD_PROBE_INFO_BUFFER(info);
    buffer = gst_buffer_make_writable(buffer);

    GstCaps *caps = gst_pad_get_current_caps(pad);

    auto structure = gst_caps_get_structure(caps, 0);
    int width = -1;
    int height = -1;
    gst_structure_get_int(structure, "width", &width);
    gst_structure_get_int(structure, "height", &height);

    if (buffer == NULL)
    {
        spdlog::warn("Cannot open buffer\n");
        return GST_PAD_PROBE_OK;
    }
    
    /* Mapping a buffer can fail (non-writable) */
    guint8 *ptr, t;
    if (gst_buffer_map(buffer, &map, GST_MAP_WRITE))
    {
        // spdlog::info("Got frame..");
        ptr = (guint8 *)map.data;

        cv::Mat frame(cv::Size(width, height), CV_8UC3, (char*)map.data, cv::Mat::AUTO_STEP);


        // Perform operations/modifications on the captured frame and feed back to pipeline
        cv::rectangle(frame, cv::Rect(10, 10, 100, 100), cv::Scalar(0, 255, 0), 2);


        gst_buffer_unmap(buffer, &map);
        // spdlog::info("Released 2");
    }
    else
    {
        spdlog::warn("Buffer not writable\n");
    }

    GST_PAD_PROBE_INFO_DATA(info) = buffer;

    return GST_PAD_PROBE_OK;
}

void gsHookAfterBuild(GstElement *pipeline)
{
    GstElement *end_elem = gst_bin_get_by_name((GstBin *)pipeline, "frameout");
    if (end_elem != NULL)
    {
        GstPad *end_elem_pad = gst_element_get_static_pad(end_elem, "sink");
        if (end_elem_pad != NULL)
        {
            // add probe to obtain the frame and perform operations on it
            auto probe_id = gst_pad_add_probe(end_elem_pad, GST_PAD_PROBE_TYPE_BUFFER, (GstPadProbeCallback)cb_have_data, NULL, NULL);
            spdlog::info("Added probe {}", probe_id);
        }
    }
    else
        throw std::logic_error("Cannot found element to hook");
}

/**************************************************
# Entry point
**************************************************/
int main(int arg, char *argv[])
{


    spdlog::info("Logger started");

    signal(SIGINT, interupt_handler);

    auto wrapper = GstreamerWrapper(
        GstreamerWrapperConfig(FileUtil::readTextFromFile("../gstreamer_wrapper_config.json")),
        gsHookAfterBuild);

    wrapper.start();

    while (g_loopEnable)
        this_thread::sleep_for(chrono::seconds(1));


    wrapper.stop();
}
