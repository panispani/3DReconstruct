High-level steps
1. Register the depth and rgb image, the input of ORB_SLAM needs to be synchronized and registered
2. Create association.txt file
3. Modify the yaml file according to the parameters of the kinect camera (take into account the transformations of the 1st step)
4. Generate a point cloud of a camera
5. Using the trajectory information convert all of them in the same coordinate frame
6. Read about kinect calibration issues and suggest improvements to the pipeline
7. Deliverables

Step details
Example kinect point cloud: https://sketchfab.com/3d-models/more-xmas-model-stills-6f9f6b08027b4f9b9f13ef81d07da895?fbclid=IwAR1rzYwJOr8kM_Nl6j5jDtC7GpMCfaAgjHqiEG8bPBRQF3EhTBBc3Sf9hX4
1. +
- Read about intrinsic and extrinsic matrices
The extrinsic matrix converts from 3D world coordinates to 3D camera coordinates
R | t   with reverse  R^T | -t
0 | 1                  0  | 1
- Think about the matlab code that isn't giving correct registration (https://www.codefull.org/2016/03/align-depth-and-color-frames-depth-and-rgb-registration/)
Don't understand..
- The OpenNI driver uses this calibration for undistorting the images, and for registering the depth images (taken by the IR camera) to the RGB images (https://vision.in.tum.de/data/datasets/rgbd-dataset/file_formats)
Registration is explained there
https://s3.amazonaws.com/com.occipital.openni/OpenNI_Programmers_Guide.pdf
- Think of how you would define the reverse transformation for registration
- Look into the CV assignments for anything similar
- Use Kinect SDK to create a point cloud, observe in the code how they register the frames (https://github.com/microsoft/Azure-Kinect-Sensor-SDK/tree/develop/examples/transformation)
Use k4a_transformation_color_image_to_depth_camera() and then write to file the transformed point!!
Also do point cloud stuff using this library, they have kinect stuff ready, don't die trying to do it yourself.
- Add code in the Kinect SDK and recompile to verify the calibration parameters are interpreted correctly
- Read other articles about registration and alignment
Explaining of what registration is: https://s3.amazonaws.com/com.occipital.openni/OpenNI_Programmers_Guide.pdf
2. +
- Write a simple script to produce associations.txt. The timestamp they use is UNIX. Use date.now() + 0.2s per frame (5fps)
3. +
- Create a yaml file using the format here: https://medium.com/@j.zijlmans/orb-slam-2052515bd84c , supposedly their order is https://github.com/microsoft/Azure-Kinect-Sensor-SDK/blob/cca87fc36acab7ed9a484aaa978ab6bcd7b54d34/include/k4a/k4atypes.h#L1029 but you might want to modify the azure kinect SDK to observe it (copying fields: https://github.com/microsoft/Azure-Kinect-Sensor-SDK/blob/39319dcc1e64507b459bbb2594bfc54dfa50c0cc/src/transformation/intrinsic_transformation.c#L51)
- is there an API call to just print the calibration parameters of the two cameras?
4. +
- This example generates a point cloud https://github.com/microsoft/Azure-Kinect-Sensor-SDK/tree/develop/examples/transformation remember to use  playback mode (color and depth images to point cloud)
5. +
- The camera extrinsic parameters (https://vision.in.tum.de/data/datasets/rgbd-dataset/file_formats) are given as quaternion(R) and a translation vector. Reverse them, see here http://www.pointclouds.org/documentation/tutorials/matrix_transform.php
6. +
- https://www.codefull.net/2017/04/practical-kinect-calibration/
- point cloud overlay inaccuracy: https://github.com/microsoft/Azure-Kinect-Sensor-SDK/issues/803
- kinect camera calibration https://sourceforge.net/projects/kinectcalib/files/v2.2/
- interactive kinect calibration https://stackoverflow.com/questions/17012585/how-do-you-map-kinects-depth-data-to-its-rgb-color
7. +
- Tidy the repository with only the needed code
- Provide the pcl for each frame separately and for all the frames together combined


Manage to use the sdk for the transformation example (before compilation add on line L328: )
(
  In case of failure
  https://www.mathworks.com/matlabcentral/answers/268152-mapping-rgb-and-depth-kinect-v2
  http://jacoposerafin.com/nicp/index.php/tutorials/depth-image-registration/
  https://www.mathworks.com/matlabcentral/answers/164342-how-to-calibrating-and-register-two-cameras-of-different-resolutions
  https://www.mathworks.com/matlabcentral/answers/153470-stereo-calibration-of-two-cameras-with-different-camera-resolutions
  https://stackoverflow.com/questions/15936330/kinect-sdk-align-depth-and-color-frames
  https://github.com/IntelRealSense/librealsense/issues/2552
  https://github.com/chaowang15/RGBDCapture
  https://github.com/IntelRealSense/librealsense/wiki/Projection-in-RealSense-SDK-2.0#frame-alignment
)
Then see if you can actually print the calibration information completely
(
  Define calibration https://github.com/microsoft/Azure-Kinect-Sensor-SDK/blob/develop/examples/transformation/main.cpp#L292
  Then get it from data https://github.com/microsoft/Azure-Kinect-Sensor-SDK/blob/develop/examples/transformation/main.cpp#L327
  We just need to add a print statement for it
  This will just print the params as you know them:
  https://github.com/microsoft/Azure-Kinect-Sensor-SDK/blob/cca87fc36acab7ed9a484aaa978ab6bcd7b54d34/include/k4a/k4atypes.h#L1099
  calibration.depth_camera_calibration.intrinsics (k4a_calibration_camera_t)
  You want to get: k4a_calibration_intrinsic_parameters_t
  const k4a_calibration_camera_t *camera_calibration; //*
  const k4a_calibration_intrinsic_parameters_t *params = &camera_calibration->intrinsics.parameters;//*
  float cx = params->param.cx;
  I GOT IT
  you have: k4a_calibration_t calibration;
  then at line 328 you do
  const k4a_calibration_camera_t depth_camera = calibration.depth_camera_calibration pointer pending
  then do the above and print, we will make sure we read the parameters correctly this way
)
With calibration information retry this: https://www.codefull.net/2016/03/align-depth-and-color-frames-depth-and-rgb-registration/
