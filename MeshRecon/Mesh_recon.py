import open3d as o3d
import numpy as np
import copy
import argparse


def main():
	parser = argparse.ArgumentParser(description='Mesh reconstruction.')
	parser.add_argument('--input', type=str)
	parser.add_argument('--strategy', type=str, default="Poisson", help='reconstruction strategy: Poisson / BPA')
	parser.add_argument('--use_filter', type=bool, default=False, help='use filter after recon or not')
	args = parser.parse_args()
	pcd_name = args.input
	Strategy = args.strategy
	Filter = args.use_filter

	# read point cloud (*.ply)
	pcd = o3d.io.read_point_cloud('./data/'+pcd_name)
	# remove outliers
	distances = pcd.compute_nearest_neighbor_distance()
	avg_dist = np.mean(distances)
	radius = 3 * avg_dist
	pcd_crop, _ = pcd.remove_radius_outlier(nb_points=5,radius=radius)
	# estimate normals
	pcd_crop.estimate_normals()

	# surface reconstruction
	if Strategy == "Poisson":
		mesh = o3d.geometry.TriangleMesh.create_from_point_cloud_poisson(pcd_crop, depth=8, width=0, scale=1.1, linear_fit=False)[0]
		obox = pcd_crop.get_oriented_bounding_box()
		mesh_temp = mesh.crop(obox)
		# o3d.visualization.draw_geometries([obox,mesh_temp])
	elif Strategy == "BPA":
		distances = pcd_crop.compute_nearest_neighbor_distance()
		avg_dist = np.mean(distances)
		radius = 3 * avg_dist
		mesh = o3d.geometry.TriangleMesh.create_from_point_cloud_ball_pivoting(pcd_crop,o3d.utility.DoubleVector([radius, radius * 2]))
		mesh_temp = mesh.simplify_quadric_decimation(100000)
		mesh_temp.remove_degenerate_triangles()
		mesh_temp.remove_duplicated_triangles()
		mesh_temp.remove_duplicated_vertices()
		mesh_temp.remove_non_manifold_edges()
		# o3d.visualization.draw_geometries([mesh_temp])

	# filter
	if Filter == True: 
		mesh_temp = mesh_temp.filter_smooth_simple(number_of_iterations=1)

	# remove spurious triangles
	with o3d.utility.VerbosityContextManager(o3d.utility.VerbosityLevel.Debug) as cm:
		triangle_clusters, cluster_n_triangles, _ = mesh_temp.cluster_connected_triangles()
	triangle_clusters = np.asarray(triangle_clusters)
	cluster_n_triangles = np.asarray(cluster_n_triangles)
	mesh_out = copy.deepcopy(mesh_temp)
	triangles_to_remove = cluster_n_triangles[triangle_clusters] < 500
	mesh_out.remove_triangles_by_mask(triangles_to_remove)
	# o3d.visualization.draw_geometries([mesh_out])
	
	o3d.io.write_triangle_mesh("result/"+pcd_name, mesh_out)

if __name__=='__main__':
	main()