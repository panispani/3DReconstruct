# Object Mesh Reconstruction using Azure Kinect

The whole framework is the post/preprocess of BADSLAM (https://github.com/ETH3D/badslam) mapping result.

## Point Cloud generation
See README.md at *pointCloudGen/README.md*. [Link](pointCloudGen/README.md)

## Background removal

Source code: "backgroundRemoval/backgroundremovel.ipynb"

Dependencies: open3d library

open3d install:

```
pip install open3d
```

## Geometric registration

Source code: [geometricRegistration/ransac_icp_colored_regi.py](geometricRegistration/ransac_icp_colored_regi.py)

Dependencies: open3d library


## Surface Reconstruction

Source code: [MeshRcon/Mesh_recon.py](MeshRcon/Mesh_recon.py)

Dependencies: open3d library

## Depth error compensation

Training data generation source code: [bias correction/generate_traiing_data.ipynb](bias correction/generate_traiing_data.ipynb]), [bias correction/generate_traiing_data.ipynb](bias correction/generate_traiing_data.ipynb)

Learning source code: 

Random Forest: [bias correction/training_RF.ipynb](bias correction/training_RF.ipynb)

Neural Network: [bias correction/training.ipynb](bias correction/training.ipynb)

Dependencies: open3d, AprilTag (https://github.com/AprilRobotics/apriltag), tensorflow, sklearn

Setup AprilTag:

```
$ git clone https://github.com/AprilRobotics/apriltag.git
$ cd apriltag
$ cmake .
$ sudo make install

```

Setup other libraries:

```
pip install open3d

```

