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
