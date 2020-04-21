// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include <k4a/k4a.h>
#include <k4arecord/playback.h>
#include <string>
#include "transformation_helpers.h"
#include <fstream>
#include <iostream>
#include <ctime>
#include "turbojpeg.h"
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
using namespace cv;

static bool save_matching_frames(k4a_transformation_t transformation_handle,
    const k4a_image_t depth_image,
    const k4a_image_t color_image,
    std::string input_path,
    std::string filename)
{
    // COLOR IMAGE DIMENSIONS
    int color_image_width_pixels = k4a_image_get_width_pixels(color_image);
    int color_image_height_pixels = k4a_image_get_height_pixels(color_image);
    // CREATE NEW TRANSFORMED DEPTH IMAGE
    k4a_image_t transformed_depth_image = NULL;
    if (K4A_RESULT_SUCCEEDED != k4a_image_create(K4A_IMAGE_FORMAT_DEPTH16,
        color_image_width_pixels,
        color_image_height_pixels,
        color_image_width_pixels * (int)sizeof(uint16_t),
        &transformed_depth_image))
    {
        printf("Failed to create transformed depth image\n");
        return false;
    }
    // Transform depth image into color camera geometry
    if (K4A_RESULT_SUCCEEDED !=
        k4a_transformation_depth_image_to_color_camera(transformation_handle, depth_image, transformed_depth_image))
    {
        printf("Failed to compute transformed depth image\n");
        return false;
    }

    // SAVE DEPTH FRAME
    uint8_t* buffer = k4a_image_get_buffer(transformed_depth_image);
    uint16_t* depth_buffer = reinterpret_cast<uint16_t*>(buffer);
    cv::Mat depth_frame = cv::Mat(color_image_height_pixels, color_image_width_pixels, CV_16UC1, depth_buffer, cv::Mat::AUTO_STEP);
    imwrite(input_path + "/depth/" + filename + ".png", depth_frame);

    // SAVE COLOR FRAME
    uint8_t* color_buffer = (uint8_t*)(void*)k4a_image_get_buffer(color_image);
    cv::Mat color_frame = cv::Mat(color_image_height_pixels, color_image_width_pixels, CV_8UC4, color_buffer, cv::Mat::AUTO_STEP);
    imwrite(input_path + "/color/" + filename + ".png", color_frame);
    k4a_image_release(transformed_depth_image);
    return true;

}

static bool point_cloud_depth_to_color(k4a_transformation_t transformation_handle,
    const k4a_image_t depth_image,
    const k4a_image_t color_image,
    std::string file_name)
{
    // transform color image into depth camera geometry
    int color_image_width_pixels = k4a_image_get_width_pixels(color_image);
    int color_image_height_pixels = k4a_image_get_height_pixels(color_image);
    // <> this image is what we are after, transformed depth image
    k4a_image_t transformed_depth_image = NULL;
    if (K4A_RESULT_SUCCEEDED != k4a_image_create(K4A_IMAGE_FORMAT_DEPTH16,
        color_image_width_pixels,
        color_image_height_pixels,
        color_image_width_pixels * (int)sizeof(uint16_t),
        &transformed_depth_image))
    {
        printf("Failed to create transformed depth image\n");
        return false;
    }

    k4a_image_t point_cloud_image = NULL;
    if (K4A_RESULT_SUCCEEDED != k4a_image_create(K4A_IMAGE_FORMAT_CUSTOM,
        color_image_width_pixels,
        color_image_height_pixels,
        color_image_width_pixels * 3 * (int)sizeof(int16_t),
        &point_cloud_image))
    {
        printf("Failed to create point cloud image\n");
        return false;
    }

    if (K4A_RESULT_SUCCEEDED !=
        k4a_transformation_depth_image_to_color_camera(transformation_handle, depth_image, transformed_depth_image))
    {
        printf("Failed to compute transformed depth image\n");
        return false;
    }

    if (K4A_RESULT_SUCCEEDED != k4a_transformation_depth_image_to_point_cloud(transformation_handle,
        transformed_depth_image,
        K4A_CALIBRATION_TYPE_COLOR,
        point_cloud_image))
    {
        printf("Failed to compute point cloud\n");
        return false;
    }

    tranformation_helpers_write_point_cloud(point_cloud_image, color_image, file_name.c_str());

    k4a_image_release(transformed_depth_image);
    k4a_image_release(point_cloud_image);

    return true;
}

// Timestamp in milliseconds. Defaults to 1 sec as the first couple frames don't contain color
static int pointcloudmode(std::string input_path, int timestamp = 1000)
{
    if (timestamp < 1000)
    {
        printf("ERROR! Don't generate pointcloud for timestamps < 1000 ms\n");
        printf("Execution will proceed, Undefined behavior!\n");
    }
    /* Predef of params */
    int returnCode = 1;
    k4a_playback_t playback = NULL;
    k4a_calibration_t calibration; // <>
    k4a_transformation_t transformation = NULL;
    k4a_capture_t capture = NULL;
    k4a_image_t depth_image = NULL;
    k4a_image_t color_image = NULL;
    k4a_image_t uncompressed_color_image = NULL;
    k4a_result_t result;
    k4a_stream_result_t stream_result;
    double now = (double)std::time(0);

    std::string tmp = input_path + "out.mkv";
    // Open recording
    result = k4a_playback_open(tmp.c_str(), &playback);
    if (result != K4A_RESULT_SUCCEEDED || playback == NULL)
    {
        printf("Failed to open recording %s\n", input_path.c_str());
        goto Exit;
    }

    // Seek timestamp
    result = k4a_playback_seek_timestamp(playback, timestamp * 1000, K4A_PLAYBACK_SEEK_BEGIN);
    if (result != K4A_RESULT_SUCCEEDED)
    {
        printf("Failed to seek frame at %d seconds\n", timestamp / 1000);
        goto Exit;
    }
    printf("Getting capture at timestamp: %d/%d (ms)\n", timestamp, (int)(k4a_playback_get_recording_length_usec(playback) / 1000));

    // Get next capture
    stream_result = k4a_playback_get_next_capture(playback, &capture);
    if (stream_result != K4A_STREAM_RESULT_SUCCEEDED || capture == NULL)
    {
        printf("Failed to fetch frame\n");
        goto Exit;
    }
    // <> get calibration from playback
    if (K4A_RESULT_SUCCEEDED != k4a_playback_get_calibration(playback, &calibration))
    {
        printf("Failed to get calibration\n");
        goto Exit;
    }

    // <> create transformation from one camera to the other
    transformation = k4a_transformation_create(&calibration);

    // Fetch depth frame
    depth_image = k4a_capture_get_depth_image(capture);
    if (depth_image == 0)
    {
        printf("Failed to get depth image from capture\n");
        goto Exit;
    }
    // Fetch color frame
    color_image = k4a_capture_get_color_image(capture);
    if (color_image == 0)
    {
        printf("Failed to get color image from capture\n");
        goto Exit;
    }

    ///////////////////////////////
    // Convert color frame from mjpeg to bgra
    k4a_image_format_t format;
    format = k4a_image_get_format(color_image);
    if (format != K4A_IMAGE_FORMAT_COLOR_MJPG)
    {
        printf("Color format not supported. Please use MJPEG\n");
        goto Exit;
    }

    int color_width, color_height;
    color_width = k4a_image_get_width_pixels(color_image);
    color_height = k4a_image_get_height_pixels(color_image);

    if (K4A_RESULT_SUCCEEDED != k4a_image_create(K4A_IMAGE_FORMAT_COLOR_BGRA32,
        color_width,
        color_height,
        color_width * 4 * (int)sizeof(uint8_t),
        &uncompressed_color_image))
    {
        printf("Failed to create image buffer\n");
        goto Exit;
    }

    tjhandle tjHandle;
    tjHandle = tjInitDecompress();
    if (tjDecompress2(tjHandle,
        k4a_image_get_buffer(color_image),
        static_cast<unsigned long>(k4a_image_get_size(color_image)),
        k4a_image_get_buffer(uncompressed_color_image),
        color_width,
        0, // pitch
        color_height,
        TJPF_BGRA,
        TJFLAG_FASTDCT | TJFLAG_FASTUPSAMPLE) != 0)
    {
        printf("Failed to decompress color frame\n");
        if (tjDestroy(tjHandle))
        {
            printf("Failed to destroy turboJPEG handle\n");
        }
        goto Exit;
    }
    if (tjDestroy(tjHandle))
    {
        printf("Failed to destroy turboJPEG handle\n");
    }

    ///////////////////////////////
    // Compute color point cloud by warping depth image into color camera geometry
    printf("Saving point cloud...\n");
    if (point_cloud_depth_to_color(transformation, depth_image, uncompressed_color_image, input_path + std::to_string(timestamp) + ".ply") == false)
    {
        printf("Failed to transform depth to color\n");
        goto Exit;
    }
    printf("Saved!\n");
    returnCode = 0;

Exit:
    if (playback != NULL)
    {
        k4a_playback_close(playback);
    }
    if (depth_image != NULL)
    {
        k4a_image_release(depth_image);
    }
    if (color_image != NULL)
    {
        k4a_image_release(color_image);
    }
    if (uncompressed_color_image != NULL)
    {
        k4a_image_release(uncompressed_color_image);
    }
    if (capture != NULL)
    {
        k4a_capture_release(capture);
    }
    if (transformation != NULL)
    {
        k4a_transformation_destroy(transformation);
    }
    return returnCode;
    return 0;
}

static int imumode()
{
    // k4a_record_configuration_t config;
    // result = k4a_playback_get_record_configuration(playback, &config);
    // if (result != K4A_RESULT_SUCCEEDED)
    // {
    //     printf("Failed to get record config\n");
    //     goto Exit;
    // }
    // printf("Is the IMU enabled: %d\n", config.imu_track_enabled);
}

static int print_calibration_information(std::string input_file)
{
    float r11, r12, r13, r21, r22, r23, r31, r32, r33, t1, t2, t3;
    float cx, cy, fx, fy, k1, k2, k3, k4, k5, k6, codx, cody, p1, p2;
    const k4a_calibration_intrinsic_parameters_t* params;
    k4a_calibration_t calibration; // <>
    k4a_playback_t playback = NULL;
    k4a_result_t result;
    int returnCode = 1;

    // Open recording
    result = k4a_playback_open(input_file.c_str(), &playback);
    if (result != K4A_RESULT_SUCCEEDED || playback == NULL)
    {
        printf("Failed to open recording %s\n", input_file.c_str());
        goto Exit;
    }
    // <> get calibration from playback
    if (K4A_RESULT_SUCCEEDED != k4a_playback_get_calibration(playback, &calibration))
    {
        printf("Failed to get calibration\n");
        goto Exit;
    }

    // CALIBRATION INFORMATION
    printf("----------------------------\n");
    printf("CALIBRATION INFORMATION:\n");
    printf("----------------------------\n");
    printf("DEPTH CAMERA INTRISTICS\n");
    const k4a_calibration_camera_t depth_camera = calibration.depth_camera_calibration;
    params = &depth_camera.intrinsics.parameters;
    cx = params->param.cx;
    cy = params->param.cy;
    fx = params->param.fx;
    fy = params->param.fy;
    k1 = params->param.k1;
    k2 = params->param.k2;
    k3 = params->param.k3;
    k4 = params->param.k4;
    k5 = params->param.k5;
    k6 = params->param.k6;
    codx = params->param.codx; // center of distortion is set to 0 for Brown Conrady model
    cody = params->param.cody;
    p1 = params->param.p1;
    p2 = params->param.p2;
    printf("cx: %f\n", cx);
    printf("cy: %f\n", cy);
    printf("fx: %f\n", fx);
    printf("fy: %f\n", fy);
    printf("k1: %f\n", k1);
    printf("k2: %f\n", k2);
    printf("k3: %f\n", k3);
    printf("k4: %f\n", k4);
    printf("k5: %f\n", k5);
    printf("k6: %f\n", k6);
    printf("codx: %f\n", codx);
    printf("cody: %f\n", cody);
    printf("p1: %f\n", p1);
    printf("p2: %f\n", p2);
    printf("----------------------------\n");
    printf("COLOR CAMERA INTRISTICS\n");
    const k4a_calibration_camera_t color_camera = calibration.color_camera_calibration;
    params = &color_camera.intrinsics.parameters;
    cx = params->param.cx;
    cy = params->param.cy;
    fx = params->param.fx;
    fy = params->param.fy;
    k1 = params->param.k1;
    k2 = params->param.k2;
    k3 = params->param.k3;
    k4 = params->param.k4;
    k5 = params->param.k5;
    k6 = params->param.k6;
    codx = params->param.codx; // center of distortion is set to 0 for Brown Conrady model
    cody = params->param.cody;
    p1 = params->param.p1;
    p2 = params->param.p2;
    printf("cx: %f\n", cx);
    printf("cy: %f\n", cy);
    printf("fx: %f\n", fx);
    printf("fy: %f\n", fy);
    printf("k1: %f\n", k1);
    printf("k2: %f\n", k2);
    printf("k3: %f\n", k3);
    printf("k4: %f\n", k4);
    printf("k5: %f\n", k5);
    printf("k6: %f\n", k6);
    printf("codx: %f\n", codx);
    printf("cody: %f\n", cody);
    printf("p1: %f\n", p1);
    printf("p2: %f\n", p2);
    printf("----------------------------\n");
    printf("COLOR CAMERA EXTRINSICS\n");
    r11 = color_camera.extrinsics.rotation[0];
    r12 = color_camera.extrinsics.rotation[1];
    r13 = color_camera.extrinsics.rotation[2];
    r21 = color_camera.extrinsics.rotation[3];
    r22 = color_camera.extrinsics.rotation[4];
    r23 = color_camera.extrinsics.rotation[5];
    r31 = color_camera.extrinsics.rotation[6];
    r32 = color_camera.extrinsics.rotation[7];
    r33 = color_camera.extrinsics.rotation[8];
    t1 = color_camera.extrinsics.translation[0];
    t2 = color_camera.extrinsics.translation[1];
    t3 = color_camera.extrinsics.translation[2];
    printf("r11: %f\n", r11);
    printf("r12: %f\n", r12);
    printf("r13: %f\n", r13);
    printf("r21: %f\n", r21);
    printf("r22: %f\n", r22);
    printf("r23: %f\n", r23);
    printf("r31: %f\n", r31);
    printf("r32: %f\n", r32);
    printf("r33: %f\n", r33);
    printf("t1: %f\n", t1);
    printf("t2: %f\n", t2);
    printf("t3: %f\n", t3);
    printf("----------------------------\n");
    returnCode = 0;

Exit:
    if (playback != NULL)
    {
        k4a_playback_close(playback);
    }
    return returnCode;
}

// Timestamp in milliseconds. Defaults to 1 sec as the first couple frames don't contain color
static int framemode(std::string input_path, int fps, int timestampFrom = 1000, int timestampTo = 1000)
{
    /* Predef of params */
    int returnCode = 1;
    int timestamp; // current timestamp, should loop between timestampFrom and timestampTo
    k4a_playback_t playback = NULL;
    k4a_calibration_t calibration; // <>
    k4a_transformation_t transformation = NULL;
    k4a_capture_t capture = NULL;
    k4a_image_t depth_image = NULL;
    k4a_image_t color_image = NULL;
    k4a_image_t uncompressed_color_image = NULL;
    k4a_result_t result;
    k4a_stream_result_t stream_result;
    double now = (double)std::time(0);
    std::string assocFile;
    std::ofstream fout;
    bool erroneous;
    std::string tmp = input_path + "out.mkv";

    // Open recording
    result = k4a_playback_open(tmp.c_str(), &playback);
    if (result != K4A_RESULT_SUCCEEDED || playback == NULL)
    {
        printf("Failed to open recording %s\n", input_path.c_str());
        goto Exit;
    }

    // Pre-condition checks
    if (timestampFrom < 1000)
    {
        printf("ERROR! Initial timestamp shuold be > 1000, but it's %d\n", timestampFrom);
        printf("Execution will proceed, Undefined behavior!\n");
    }
    if (timestampTo < timestampFrom)
    {
        printf("ERROR! Final timestamp should be after the initial one but %d < %d!\n", timestampTo, timestampFrom);
        goto Exit;
    }
    if (timestampTo > (int)(k4a_playback_get_recording_length_usec(playback) / 1000))
    {
        printf("ERROR! Final timestamp should be before the end of the video, but %d < %d!\n", timestampTo, (int)(k4a_playback_get_recording_length_usec(playback) / 1000));
        goto Exit;
    }

    // loop over timestamps
    assocFile = (std::string)input_path + "associations.txt";
    fout.open(assocFile);
    timestamp = timestampFrom;
    result = k4a_playback_seek_timestamp(playback, timestamp * 1000, K4A_PLAYBACK_SEEK_BEGIN);
    // create rgb and depth directories
    // TODO, created for us in this case by python script
    erroneous = true;
    while (result == K4A_RESULT_SUCCEEDED) {
        printf("Seeking to timestamp: %d/%d (ms)\n",
            timestamp,
            (int)(k4a_playback_get_recording_length_usec(playback) / 1000));

        // Get next capture
        stream_result = k4a_playback_get_next_capture(playback, &capture);
        if (stream_result != K4A_STREAM_RESULT_SUCCEEDED || capture == NULL)
        {
            printf("Failed to fetch frame\n");
            break;
        }
        // <> get calibration from playback
        if (K4A_RESULT_SUCCEEDED != k4a_playback_get_calibration(playback, &calibration))
        {
            printf("Failed to get calibration\n");
            goto Exit;
        }

        // <> create transformation from one camera to the other
        transformation = k4a_transformation_create(&calibration);

        // Fetch depth frame
        depth_image = k4a_capture_get_depth_image(capture);
        if (depth_image == 0)
        {
            printf("Failed to get depth image from capture\n");
            goto Exit;
        }
        // Fetch color frame
        color_image = k4a_capture_get_color_image(capture);
        if (color_image == 0)
        {
            printf("Failed to get color image from capture\n");
            goto Exit;
        }

        ///////////////////////////////
        // Convert color frame from mjpeg to bgra
        k4a_image_format_t format;
        format = k4a_image_get_format(color_image);
        if (format != K4A_IMAGE_FORMAT_COLOR_MJPG)
        {
            printf("Color format not supported. Please use MJPEG\n");
            goto Exit;
        }

        int color_width, color_height;
        color_width = k4a_image_get_width_pixels(color_image);
        color_height = k4a_image_get_height_pixels(color_image);

        if (K4A_RESULT_SUCCEEDED != k4a_image_create(K4A_IMAGE_FORMAT_COLOR_BGRA32,
            color_width,
            color_height,
            color_width * 4 * (int)sizeof(uint8_t),
            &uncompressed_color_image))
        {
            printf("Failed to create image buffer\n");
            goto Exit;
        }

        tjhandle tjHandle;
        tjHandle = tjInitDecompress();
        if (tjDecompress2(tjHandle,
            k4a_image_get_buffer(color_image),
            static_cast<unsigned long>(k4a_image_get_size(color_image)),
            k4a_image_get_buffer(uncompressed_color_image),
            color_width,
            0, // pitch
            color_height,
            TJPF_BGRA,
            TJFLAG_FASTDCT | TJFLAG_FASTUPSAMPLE) != 0)
        {
            printf("Failed to decompress color frame\n");
            if (tjDestroy(tjHandle))
            {
                printf("Failed to destroy turboJPEG handle\n");
            }
            goto Exit;
        }
        if (tjDestroy(tjHandle))
        {
            printf("Failed to destroy turboJPEG handle\n");
        }

        ///////////////////////////////
        // Compute color point cloud by warping depth image into color camera geometry
        //if (point_cloud_depth_to_color(transformation, depth_image, uncompressed_color_image, output_filename) == false)
        std::string snow = std::to_string(now);
        if (save_matching_frames(transformation, depth_image, uncompressed_color_image, input_path, snow) == false)
        {
            printf("Failed to transform depth to color\n");
            goto Exit;
        }
        fout << snow << " color/" << snow << ".png " << snow << " depth/" << snow << ".png" << std::endl;
        now += 1.0 / (double)fps;
        timestamp += round(1000.0 / (double)fps);
        if (timestamp > timestampTo)
        {
            erroneous = false;
            break;
        }
        result = k4a_playback_seek_timestamp(playback, 1000 * timestamp, K4A_PLAYBACK_SEEK_BEGIN);
    }
    fout.close();
    if (erroneous)
    {
        printf("Failed to seek frame at %d seconds\n", timestamp / 1000);
    }
    printf("Finished saving frames!\n");
    returnCode = 0;
Exit:
    if (playback != NULL)
    {
        k4a_playback_close(playback);
    }
    if (depth_image != NULL)
    {
        k4a_image_release(depth_image);
    }
    if (color_image != NULL)
    {
        k4a_image_release(color_image);
    }
    if (uncompressed_color_image != NULL)
    {
        k4a_image_release(uncompressed_color_image);
    }
    if (capture != NULL)
    {
        k4a_capture_release(capture);
    }
    if (transformation != NULL)
    {
        k4a_transformation_destroy(transformation);
    }
    return returnCode;
}

/* DONT READ, NOT IMPORTANT FOR IMPLEMENTATION */
static void print_usage()
{
    std::string DOCSTRING = "USAGE\n\n\
    -POINTCLOUD MODE \n\
    DESCRIPTION \n\
      Save the pointcloud of a RGB-D video of at specific timestamp \n\
    USAGE \n\
      ./vid2dataset.exe pointcloud <directory> <timestamp> \n\
    ARGUMENTS \n\
      <directory>: A directory including the input video named EXACTLY 'out.mkv'. The output pointcloud gets saved here as well. The input video must be RGB-D, include both RGB and Depth tracks. \n\
      <timestamp>: Timestamp in milliseconds (1 second = 1000 milliseconds) \n\
      IMPORTANT: The least value for the timestamp should be 1000 (1 second) because the first few frames from Kinect have no color \n\
    EXAMPLE: \n\
      Save the pointcloud at 2.5 seconds \n\
      ./vid2dataset.exe pointcloud ../recordings/ball/scan1/ 2500 \n\n\
    -FRAME MODE \n\
    DESCRIPTION \n\
      Save a sequence of corresponding RGB and Depth frames given a start and end time as well as the desired fps. \n\
      This mode saves matching frames in 2 directories depth/ and color/ (which need to pre-exist) and saves their correspondences in a file called 'associations.txt' \n\
      This mode exists so that the frames are then fed to a SLAM algorithm. \n\
      This algorithm could also be embedded in this executable in the future removing the need to save the frames on the disk. \n\
    USAGE \n\
      ./vid2dataset.exe frame <directory> <fps> <timestampFrom> <timestampTo> \n\
    ARGUMENTS \n\
      <directory>: A directory including the input video named EXACTLY 'out.mkv' AND including 2 empty directories named 'color' and 'depth' to store the corresponding frames. 'associations.txt' gets save here as well. \n\
      <fps>: The desired frames-per-second \n\
      <timestampFrom>: Starting timestamp in milliseconds (1 second = 1000 milliseconds). Minimum value = 1000 (1 second) because the first few frames from Kinect have no color \n\
      <timestampTo>: Ending timestamp in milliseconds (1 second = 1000 milliseconds). This should be after the <timestampFrom> and before the end of the video! \n\
    EXAMPLE: \n\
      Save the corresponding RGB and D frames from 1.3 to 2.5 at 10 fps \n\
      ./vid2dataset.exe frame ../recordings/ball/scan1/ 10 1300 2500 \n\n\
    -IMU MODE \n\
    DESCRIPTION \n\
      Given a desired FPS, this mode translates each frame to a point cloud and using the IMU (accelerometer and gyrometer) combines all the pointclouds on the same coordinate frame. \n\
      WARNING: Using a high FPS or a big time frame can result in huge files! \n\
    USAGE \n\
      ./vid2dataset.exe imu <out.mkv> <fps> <timestampFrom> <timestampTo> \n\
    ARGUMENTS \n\
      <out.mkv>: Provide the path to a video file (must be RGB-D with IMU data, include both RGB, Depth and IMU tracks) \n\
      <fps>: The desired frames-per-second \n\
      <timestampFrom>: Starting timestamp in m illiseconds (1 second = 1000 milliseconds). Minimum value = 1000 (1 second) because the first few frames from Kinect have no color \n\
      <timestampTo>: Ending timestamp in milliseconds (1 second = 1000 milliseconds). This should be after the <timestampFrom> and before the end of the video! \n\
    EXAMPLE: \n\
      Create a pointcloud mesh from all the frames from 2 to 6.7 seconds at 10 frames-per-second \n\
      ./vid2dataset.exe imu ../recordings/ball/scan1/out.mkv 10 2000 6700 \n\n\
    MAC and Linux users: Believe in yourself and compile everything from scratch.. or use a VM\n";
    printf("%s", DOCSTRING.c_str());
}

int main(int argc, char** argv)
{
    int returnCode = 0;

    if (argc < 2)
    {
        print_usage();
    }
    else
    {
        std::string mode = std::string(argv[1]);
        if (mode == "pointcloud")
        {
            printf("POINTCLOUD MODE\n");
            if (argc == 4)
            {
                returnCode = pointcloudmode(std::string(argv[2]), atoi(argv[3]));
            }
            else
            {
                print_usage();
            }
        }
        else if (mode == "frame")
        {
            printf("FRAME MODE\n");
            if (argc == 6)
            {
                returnCode = framemode(std::string(argv[2]), atoi(argv[3]), atoi(argv[4]), atoi(argv[5]));
            }
            else
            {
                print_usage();
            }
        }
        else if (mode == "imu")
        {
            printf("IMU MODE\n");
            if (argc == 6)
            {
                // returnCode = imumode();
            }
            else
            {
                print_usage();
            }
        }
        else if (mode == "calibration")
        {
            printf("CALIBRATION INFORMATION MODE\n");
            if (argc == 3)
            {
                returnCode = print_calibration_information(std::string(argv[2]));
            }
            else
            {
                print_usage();
            }
        }
        else
        {
            print_usage();
        }
    }
    return returnCode;
}
