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
