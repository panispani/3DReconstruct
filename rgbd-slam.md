# RGB-D slam

# First candidate: https://github.com/raulmur/ORB_SLAM2

Prerequisites to install (similar for most RGB-D slam software):
- Pangolin (https://github.com/stevenlovegrove/Pangolin)
- OpenCV 3.2, this doesn't support OpenCV 4 (tested by me)
- Engen3 3.1.0

# RGB-D dataset format: (taken from http://vision.in.tum.de/data/datasets/rgbd-dataset/download)
rgb frame folder
depth-map frame folder
rgb.txt: timestamp filename
accelerometer.txt: timestamp ax ay az
depth.txt: timestamp filename
groundtruth.txt: timestamp tx ty tz qx qy qz qw

Generate associations between rgb frames and depthmap frames (get associate.py file from tum website)
```
python associate.py PATH_TO_SEQUENCE/rgb.txt PATH_TO_SEQUENCE/depth.txt > associations.txt
```

```
./Examples/RGB-D/rgbd_tum Vocabulary/ORBvoc.txt Examples/RGB-D/TUM1.yaml PATH_TO_SEQUENCE_FOLDER ASSOCIATIONS_FILE
```

To process your own sequences:
- Create a settings file with the calibration of your camera. See the settings file provided for the TUM dataset for the RGB-D camera. We use the calibration model of OpenCV. 
- See the examples to learn how to create a program that makes use of the ORB-SLAM2 library and how to pass images to the SLAM system.
- RGB-D input must be synchronized and depth registered.

