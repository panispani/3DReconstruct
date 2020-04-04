# BADSLAM setup instructions on leonhard

## Ssh into leonhard
```
$ ssh <username>@login.leonhard.ethz.ch
```
## Clone repositories (yes you need to build badslam :/ )
```
$ git clone https://github.com/panpan2/3DReconstruct
$ git clone https://github.com/ETH3D/badslam
```

## Load cmake dependencies
```
$ module load cmake/3.9.4
```
If issues arise with gcc or cmake versions take a look here: https://scicomp.ethz.ch/wiki/Leonhard_applications_and_libraries

## Build instructions for linux
```
$ cd badslam
$ mkdir build_RelWithDebInfo
$ cd build_RelWithDebInfo
$ cmake -DCMAKE_BUILD_TYPE=RelWithDebInfo -DCMAKE_CUDA_FLAGS="-arch=sm_61" ..
$ make -j badslam  # Reduce the number of threads if running out of memory, e.g., -j3
```
If something doesn't seem to work make sure you have the dependencies of the REAMDE.md in badlsam

TODO LIST:
- how does the association look like, what does ORB actually read and look for
create depth.txt and rgb.txt from the directories, figure out what that timestamp actually means
what double timestamp does the TUM dataset use, maybe use the same format and create the association.txt file
It's UNIX dude... 
current unix, since we have 5 fps, we add 0.2 per frame, we don't include this in the filename but it shouldn't be a problem...
just write a simple script converter for this

- how does ORB read the calibration file, how to get it into that format
https://medium.com/@j.zijlmans/orb-slam-2052515bd84c
calibration and distortion parameters
json['CalibrationInformation'] = ["Cameras", "InertialSensors", "Metadata"]
the 'Metadata' is irrelevant
'InertialSensors' is Gyro and Accelerometer calibration information that we don't care about
'Cameras' has 2 entries: Depth and RGB camera. We care about this one!
0: {Intrinsics: {…}, Location: "CALIBRATION_CameraLocationD0", Purpose: "CALIBRATION_CameraPurposeDepth"
1: {Intrinsics: {…}, Location: "CALIBRATION_CameraLocationPV0", Purpose: "CALIBRATION_CameraPurposePhotoVideo"
Well, depth and photo is not taken at the exact same place.. there is a different lens....
For PV: intristics, Rt, sensorheight, sensorWidth
LensDistortionModelBrownConrady model of 14 parameters
we also have the same kind of information for the D camera... oh no...
everything is different, even the sensor height and width, did we assume the depth maps have the same dimensions?
yes, do we just resize the images to the depth map dimensions?
there was a github issue about this.
https://github.com/microsoft/Azure-Kinect-Sensor-SDK/issues/803
https://stackoverflow.com/questions/58942612/azure-kinect-how-to-get-a-depth-video-recording-in-color
Here is the 14 parameters: https://github.com/microsoft/Azure-Kinect-Sensor-SDK/blob/39319dcc1e64507b459bbb2594bfc54dfa50c0cc/src/transformation/intrinsic_transformation.c#L51
Verify that this is the order they use in the file and you are done with the params
Here they just memcpy so the order depends on the definitions of calibration_t and k4a_calibration_camera_t
https://github.com/microsoft/Azure-Kinect-Sensor-SDK/blob/dd0787629296f7846b53fb90a5c02431a31746e5/src/calibration/calibration.c#L725
*FIND THESE TWO TYPES*
fill intrinsics for calibration_t, they seem to just copy array of floats, so depends on the order in k4a_calibration_camera_t
and here are they https://github.com/microsoft/Azure-Kinect-Sensor-SDK/blob/cca87fc36acab7ed9a484aaa978ab6bcd7b54d34/include/k4a/k4atypes.h#L1065
this is the order: https://github.com/microsoft/Azure-Kinect-Sensor-SDK/blob/cca87fc36acab7ed9a484aaa978ab6bcd7b54d34/include/k4a/k4atypes.h#L1029
also look if there is an API way to do this



- check the number of depth frames matches the number of rgb images
yes! 5fps

- how to convert to point cloud using the mis-sized depth images
https://github.com/microsoft/Azure-Kinect-Sensor-SDK/tree/develop/examples/transformation
playback mode, color and depth images to point cloud, this is good since you do want to combine point clouds but also to see how to combine depth and color images since they have different sizes
In order to use the different resolution images together, the color image must be registered to the depth image, convert to the same resolution somehow, see here https://www.codefull.org/2016/03/align-depth-and-color-frames-depth-and-rgb-registration/ we need to perform registration


- what does the camera trajectory mean for each image
quaternion to R
https://vision.in.tum.de/data/datasets/rgbd-dataset/file_formats
then you have t and R of the camera, how do I reverse it again? for all the points. But how do you find the 3D position of each pixel using a depth map? That's another question
There is conversion code given some depth factor..
