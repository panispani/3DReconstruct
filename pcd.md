# Point cloud display

In pcd directory, in build/ there is a binary *pcd_disply* that displays the pointcloud in test_pcd.pcd.
Replace with your own pcd

For pcd you need to install the pcl library and then just normally
```
mkdir build
cd build
cmake ..
cmake --build .
cp pcd_display ..
cd ..
pcd_display -s
```
When test_pcd.pcd in that directory is the point cloud you want to visualize
