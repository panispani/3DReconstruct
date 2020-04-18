# Point cloud generation

Included directories:

- **pcd**
- **registration**
- **slam**
- **vid2dataset**
- **recordingInfo**

We elaborate on what is useful in each of these directories

- **recordingInfo**

*recordingInfo/kinect.yaml*: includes the configuration of the Azure Kinect for usage with slam (ORB-SLAM2)

*calibration.json*: Calibration information as extracted from the kinect, in a non-human-readable form

*extract.py*: Script can be used after a capture with Azure Kinect to split the videos into corresponding RGB and Depth frames. This has been integrated in the *vid2dataset* software but is kept here in case someone wants to take a look at the frames as split by the Kinect API.

- **registration**

*captureNregister.cpp*: This script takes video frames (RGB and Depth) and registers them using the extrinsic and intrinsic characteristics of the two cameras. This has been integrated in the *vid2dataset* software but is kept here in case someone wants to take a look at registration in isolation.

*depth\_rgb\_registration.m*: This script also performs RGB and Depth registration. It's just implemented in MATLAB. This was implemented first and uses no APIs(!). Needs for cross-platform running without having to setup MATLAB, made us choose C++ for this.

*visuals*: A directory including images of blent RGB and Depth images to see how good the registration works. This directory also includes 3 scripts, those were used to save and blend the images.

- **pcd**


*ply2pcd\_files*: Source code for converting .ply to .pcd

*ply2pcd*: Converts .ply to .pcd. Linux executable of what is included in ply2pcd_files.

USAGE

```
  ./ply2pcd <input.ply> <out.pcd>
```

ARGUMENTS


    <input.ply>: Location of the input .ply file
    <out.pcd>: Name of the output .pcd file to save the pointcloud

EXAMPLE

```
  ./ply2pcd ply2pcd_files/out.ply test_pcd.pcd
```

*pcd\_display\_files*: Source code for displaying .pcd files

*pcd\_display*: Displays .pcd files. Linux executable of what is included in pcd_display_files.

USAGE

```
  ./pcd_display -s
```

ARGUMENTS

    For now there is no argument, the file displayed should be in the same directory and have the name **test_pcd.pcd**. This could be added as an argument, but it's not important as this is just used for debugging that the point clouds make sense.


*test_pcd.pcd*: Example pcd file to use for visualization

*results*: Includes a few screenshots of plotted pointclouds

- **vid2dataset**

This is the most important directory (For the core functionality look at [vid2dataset/vid2dataset.cpp](pointCloudGen/vid2dataset/vid2dataset.cpp)). It provides the source code and *Widnows* executable (Executable is located at: [vid2dataset/x64/Debug/vid2dataset.exe](pointCloudGen/vid2dataset/x64/Debug/vid2dataset.exe)) for:

- Pointcloud at a given timestamp in a RGB-D video
- RGB and Depth corresponding frames given a time segment of the video (To be used as input to SLAM)
- IMU mode, which tries to compute the extrinsics of the camera using just an accelerometer and a gyrometer

USAGE of the vid2dataset executable

-POINTCLOUD MODE

DESCRIPTION

    Save the pointcloud of a RGB-D video of at specific timestamp
USAGE

    ./vid2dataset.exe pointcloud <out.mkv> <timestamp>
ARGUMENTS

    <out.mkv>: Provide the path to a video file (must be RGB-D, include both RGB and Depth tracks)
    <timestamp>: Timestamp in milliseconds (1 second = 1000 milliseconds)
    IMPORTANT: The least amount for the timestamp should be 1000 (1 second) because the first few frames from Kinect have no color
EXAMPLE

    Save the pointcloud at 2.5 seconds
    ./vid2dataset.exe pointcloud ../recordings/ball/scan1/out.mkv 2500

-FRAME MODE

DESCRIPTION

    Save a sequence of corresponding RGB and Depth frames given a start and end time as well as the desired fps.
    This mode saves matching frames in 2 directories depth/ and color/ (which need to pre-exist) and saves their correspondences in a file called 'associations.txt'
    This mode exists so that the frames are then fed to a SLAM algorithm.
    This algorithm could also be embedded in this executable in the future removing the need to save the frames on the disk.
USAGE

    ./vid2dataset.exe frame <directory> <fps> <timestampFrom> <timestampTo>
ARGUMENTS

    <directory>: A directory including the input video named EXACTLY 'out.mkv' AND including 2 empty directories named 'color' and 'depth' to store the corresponding frames. 'associations.txt' gets save here as well.
    <fps>: The desired frames-per-second
    <timestampFrom>: Starting timestamp in milliseconds (1 second = 1000 milliseconds). Minimum value = 1000 (1 second) because the first few frames from Kinect have no color
    <timestampTo>: Ending timestamp in milliseconds (1 second = 1000 milliseconds). This should be after the <timestampFrom> and before the end of the video!
EXAMPLE

    Save the corresponding RGB and D frames from 1.3 to 2.5 at 10 fps
    ./vid2dataset.exe frame ../recordings/ball/scan1/ 10 1300 2500

-IMU MODE (UNIMPLEMENTED YET)

DESCRIPTION

    Given a desired FPS, this mode translates each frame to a point cloud and using the IMU (accelerometer and gyrometer) combines all the pointclouds on the same coordinate frame.
    WARNING: Using a high FPS or a big time frame can result in huge files!
USAGE
    ./vid2dataset.exe imu <out.mkv> <fps> <timestampFrom> <timestampTo>
ARGUMENTS

    <out.mkv>: Provide the path to a video file (must be RGB-D with IMU data, include both RGB, Depth and IMU tracks)
    <fps>: The desired frames-per-second
    <timestampFrom>: Starting timestamp in milliseconds (1 second = 1000 milliseconds). Minimum value = 1000 (1 second) because the first few frames from Kinect have no color
    <timestampTo>: Ending timestamp in milliseconds (1 second = 1000 milliseconds). This should be after the <timestampFrom> and before the end of the video!
EXAMPLE

    Create a pointcloud mesh from all the frames from 2 to 6.7 seconds at 10 frames-per-second
    ./vid2dataset.exe imu ../recordings/ball/scan1/out.mkv 10 2000 6700

P.S. MAC / Linux users: Believe in yourself and compile everything from scratch.. or use a VM
