// Timestamp in milliseconds. Defaults to 1 sec as the first couple frames don't contain color
static int playback(char *input_path, int timestamp = 1000, std::string output_filename = "output.ply")
{
    int returnCode = 1;
    k4a_playback_t playback = NULL;
    k4a_calibration_t calibration;
    k4a_transformation_t transformation = NULL;
    k4a_capture_t capture = NULL;
    k4a_image_t depth_image = NULL;
    k4a_image_t color_image = NULL;
    k4a_image_t uncompressed_color_image = NULL;

    k4a_result_t result;
    k4a_stream_result_t stream_result;

    // Open recording
    result = k4a_playback_open(input_path, &playback);
    if (result != K4A_RESULT_SUCCEEDED || playback == NULL)
    {
        printf("Failed to open recording %s\n", input_path);
        goto Exit;
    }

    result = k4a_playback_seek_timestamp(playback, timestamp * 1000, K4A_PLAYBACK_SEEK_BEGIN);
    if (result != K4A_RESULT_SUCCEEDED)
    {
        printf("Failed to seek timestamp %d\n", timestamp);
        goto Exit;
    }
    printf("Seeking to timestamp: %d/%d (ms)\n",
           timestamp,
           (int)(k4a_playback_get_recording_length_usec(playback) / 1000));

    stream_result = k4a_playback_get_next_capture(playback, &capture);
    if (stream_result != K4A_STREAM_RESULT_SUCCEEDED || capture == NULL)
    {
        printf("Failed to fetch frame\n");
        goto Exit;
    }

    if (K4A_RESULT_SUCCEEDED != k4a_playback_get_calibration(playback, &calibration))
    {
        printf("Failed to get calibration\n");
        goto Exit;
    }
    // From calibration you can print the intristic information of both cameras
    printf("DEPTH CAMERA INTRISTICS\n");
    const k4a_calibration_camera_t depth_camera = calibration.depth_camera_calibration;
    const k4a_calibration_intrinsic_parameters_t *params = &depth_camera.intrinsics.parameters;
    float cx = params->param.cx;
    float cy = params->param.cy;
    float fx = params->param.fx;
    float fy = params->param.fy;
    float k1 = params->param.k1;
    float k2 = params->param.k2;
    float k3 = params->param.k3;
    float k4 = params->param.k4;
    float k5 = params->param.k5;
    float k6 = params->param.k6;
    float codx = params->param.codx; // center of distortion is set to 0 for Brown Conrady model
    float cody = params->param.cody;
    float p1 = params->param.p1;
    float p2 = params->param.p2;
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
    float r11 = color_camera.extrinsics.rotation[0];
    float r12 = color_camera.extrinsics.rotation[1];
    float r13 = color_camera.extrinsics.rotation[2];
    float r21 = color_camera.extrinsics.rotation[3];
    float r22 = color_camera.extrinsics.rotation[4];
    float r23 = color_camera.extrinsics.rotation[5];
    float r31 = color_camera.extrinsics.rotation[6];
    float r32 = color_camera.extrinsics.rotation[7];
    float r33 = color_camera.extrinsics.rotation[8];
    float t1 = color_camera.extrinsics.translation[0];
    float t2 = color_camera.extrinsics.translation[1];
    float t3 = color_camera.extrinsics.translation[2];
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


    transformation = k4a_transformation_create(&calibration);

    // Fetch frame
    depth_image = k4a_capture_get_depth_image(capture);
    if (depth_image == 0)
    {
        printf("Failed to get depth image from capture\n");
        goto Exit;
    }

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
    if (point_cloud_depth_to_color(transformation, depth_image, uncompressed_color_image, output_filename) == false)
    {
        printf("Failed to transform depth to color\n");
        goto Exit;
    }

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

static void print_usage()
{
    printf("Usage: transformation_example capture <output_directory> [device_id]\n");
    printf("Usage: transformation_example playback <filename.mkv> [timestamp (ms)] [output_file]\n");
}

int main(int argc, char **argv)
{
    int returnCode = 0;

    if (argc < 2)
    {
        print_usage();
    }
    else
    {
        std::string mode = std::string(argv[1]);
        if (mode == "capture")
        {
            if (argc == 3)
            {
                returnCode = capture(argv[2]);
            }
            else if (argc == 4)
            {
                returnCode = capture(argv[2], (uint8_t)atoi(argv[3]));
            }
            else
            {
                print_usage();
            }
        }
        else if (mode == "playback")
        {
            if (argc == 3)
            {
                returnCode = playback(argv[2]);
            }
            else if (argc == 4)
            {
                returnCode = playback(argv[2], atoi(argv[3]));
            }
            else if (argc == 5)
            {
                returnCode = playback(argv[2], atoi(argv[3]), argv[4]);
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

