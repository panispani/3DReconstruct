High-level steps
1. Register the depth and rgb image, the input of ORB_SLAM needs to be synchronized and registered
2. Create association.txt file
3. Modify the yaml file according to the parameters of the kinect camera (take into account the transformations of the 1st step)
4. Generate a point cloud of a camera
5. Using the trajectory information convert all of them in the same coordinate frame
6. Read about kinect calibration issues and suggest improvements to the pipeline
7. Deliverables

Step details
1. +
- Read about intrinsic and extrinsic matrices
- Think about the matlab code that isn't giving correct registration (https://www.codefull.org/2016/03/align-depth-and-color-frames-depth-and-rgb-registration/)
- The OpenNI driver uses this calibration for undistorting the images, and for registering the depth images (taken by the IR camera) to the RGB images (https://vision.in.tum.de/data/datasets/rgbd-dataset/file_formats)
- Think of how you would define the reverse transformation for registration
- Look into the CV assignments for anything similar
- Use Kinect SDK to create a point cloud, observe in the code how they register the frames (https://github.com/microsoft/Azure-Kinect-Sensor-SDK/tree/develop/examples/transformation)
- Add code in the Kinect SDK and recompile to verify the calibration parameters are interpreted correctly
- Read other articles about registration and alignment
2. +
- Write a simple script to produce associations.txt. The timestamp they use is UNIX. Use date.now() + 0.2s per frame (5fps)
3. +
- Create a yaml file using the format here: https://medium.com/@j.zijlmans/orb-slam-2052515bd84c , supposedly their order is https://github.com/microsoft/Azure-Kinect-Sensor-SDK/blob/cca87fc36acab7ed9a484aaa978ab6bcd7b54d34/include/k4a/k4atypes.h#L1029 but you might want to modify the azure kinect SDK to observe it (copying fields: https://github.com/microsoft/Azure-Kinect-Sensor-SDK/blob/39319dcc1e64507b459bbb2594bfc54dfa50c0cc/src/transformation/intrinsic_transformation.c#L51)
- is there an API call to just print the calibration parameters of the two cameras?
4. +
- This example generates a point cloud https://github.com/microsoft/Azure-Kinect-Sensor-SDK/tree/develop/examples/transformation remember to use  playback mode (color and depth images to point cloud)
5. +
- The camera extrinsic parameters (https://vision.in.tum.de/data/datasets/rgbd-dataset/file_formats) are given as quaternion(R) and a translation vector. Reverse them
6. +
- https://www.codefull.net/2017/04/practical-kinect-calibration/
- point cloud overlay inaccuracy: https://github.com/microsoft/Azure-Kinect-Sensor-SDK/issues/803
7. +
- Tidy the repository with only the needed code
- Provide the pcl for each frame separately and for all the frames together combined
