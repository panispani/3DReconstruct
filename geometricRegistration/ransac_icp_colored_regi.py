import numpy as np
import copy
import open3d as o3d


def draw_registration_result_original_color(source, target, transformation):
    source_temp = copy.deepcopy(source)
    source_temp.transform(transformation)
    o3d.visualization.draw_geometries([source_temp, target])

def preprocess_point_cloud(pcd, voxel_size):
    print(":: Downsample with a voxel size %.3f." % voxel_size)
    pcd_down = pcd.voxel_down_sample(voxel_size)

    radius_normal = voxel_size * 2
    print(":: Estimate normal with search radius %.3f." % radius_normal)
    pcd_down.estimate_normals(
        o3d.geometry.KDTreeSearchParamHybrid(radius=radius_normal, max_nn=30))

    radius_feature = voxel_size * 5
    print(":: Compute FPFH feature with search radius %.3f." % radius_feature)
    pcd_fpfh = o3d.registration.compute_fpfh_feature(
        pcd_down,
        o3d.geometry.KDTreeSearchParamHybrid(radius=radius_feature, max_nn=100))
    return pcd_down, pcd_fpfh

def prepare_dataset(voxel_size, source, target):
    print(":: Load two point clouds and disturb initial pose.")
    trans_init = np.asarray([[0.0, 0.0, 1.0, 0.0], [1.0, 0.0, 0.0, 0.0],
                             [0.0, 1.0, 0.0, 0.0], [0.0, 0.0, 0.0, 1.0]])
    source.transform(trans_init)
    source_down, source_fpfh = preprocess_point_cloud(source, voxel_size)
    target_down, target_fpfh = preprocess_point_cloud(target, voxel_size)
    return source_down, target_down, source_fpfh, target_fpfh

if __name__ == "__main__":

    print("1. Load two point clouds and show initial pose")
    source = o3d.io.read_point_cloud("C:/Users/Yifei/Documents/ETH/2020S/3DV/bgRemoval/backgroundRemoval/objects/stereo_object1.pcd")
    target = o3d.io.read_point_cloud("C:/Users/Yifei/Documents/ETH/2020S/3DV/bgRemoval/backgroundRemoval/objects/stereo_object2.pcd")
    
    
    # draw initial alignment
    current_transformation = np.identity(4)
    draw_registration_result_original_color(source, target,
                                            current_transformation)
    
    # global registration
    voxel_size = 0.004
    distance_threshold = voxel_size * 7
    source_down, target_down, source_fpfh, target_fpfh = prepare_dataset(voxel_size, source, target)
    print("2. Global registration is applied on downsampled point clouds")
    result_ransac = o3d.registration.registration_ransac_based_on_feature_matching(
        source_down, target_down, source_fpfh, target_fpfh, distance_threshold,
        o3d.registration.TransformationEstimationPointToPoint(False), 4, [
            o3d.registration.CorrespondenceCheckerBasedOnEdgeLength(0.9),
            o3d.registration.CorrespondenceCheckerBasedOnDistance(
                distance_threshold)
        ], o3d.registration.RANSACConvergenceCriteria(4000000, 500))
    print(result_ransac)
    draw_registration_result_original_color(source, target,
                                            result_ransac.transformation)    
    
    # point to plane ICP
    current_transformation = result_ransac.transformation
    print("3. Point-to-plane ICP registration is applied on original point")
    print("   clouds to refine the alignment. Distance threshold 0.4.")
    result_icp = o3d.registration.registration_icp(
        source, target, 0.4, current_transformation,
        o3d.registration.TransformationEstimationPointToPlane())
    print(result_icp)
    draw_registration_result_original_color(source, target,
                                            result_icp.transformation)
    
    # colored pointcloud registration
    voxel_radius = [0.4, 0.1, 0.05]
    max_iter = [60, 20, 10]
    current_transformation = result_icp.transformation
    print("4. Colored point cloud registration")
    for scale in range(1):
        iter = max_iter[scale]
        radius = voxel_radius[scale]
        print([iter, radius, scale])
        result_colored = o3d.registration.registration_colored_icp(
            source, target, radius, current_transformation,
            o3d.registration.ICPConvergenceCriteria(relative_fitness=1e-6,
                                                    relative_rmse=1e-6,
                                                    max_iteration=iter))
        current_transformation = result_colored.transformation
        print(result_colored)
    draw_registration_result_original_color(source, target,
                                            result_colored.transformation)
    
    # Output
    output = o3d.geometry.PointCloud()
    source_transform = source.transform(result_colored.transformation)
    output = source_transform + target
    o3d.io.write_point_cloud("C:/Users/Yifei/Documents/ETH/2020S/3DV/registration/results/stereo_registered.pcd", output)