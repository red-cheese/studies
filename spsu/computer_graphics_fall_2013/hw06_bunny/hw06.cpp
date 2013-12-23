#include <pcl/point_cloud.h>
#include <pcl/point_types.h>
#include <pcl/PolygonMesh.h>
#include <pcl/io/vtk_lib_io.h>
#include <pcl/common/eigen.h>
#include <pcl/ros/conversions.h>

const int vertex_count = 2503;
const int face_count = 4968;

pcl::PolygonMesh mesh; // Main (original) mesh.
pcl::PointCloud<pcl::PointXYZ> cloud_bunny;

void fillMatrixWithZeros(Eigen::MatrixXd& mat) {
  for (int i = 0; i < mat.rows(); i++) {
    for (int j = 0; j < mat.cols(); j++) {
      mat(i, j) = 0.0;
    }
  }
}

void normalizeMatrix(Eigen::MatrixXd& mat) {
  // All our matrices are symmetric.
  for (int i = 0; i < mat.rows(); i++) {
    double sum = 0;

    for (int j = 0; j < mat.cols(); j++) {
      sum += mat(i, j);
    }

    for (int j = 0; j < mat.cols(); j++) {
      mat(i, j) = mat(i, j) / sum;
    }
  }
}

void constructCombinatorialMatrix(Eigen::MatrixXd& mat) {
  fillMatrixWithZeros(mat);

  for (int i = 0; i < face_count; i++) {
    int v1_index = mesh.polygons[i].vertices[0];
    int v2_index = mesh.polygons[i].vertices[1];
    int v3_index = mesh.polygons[i].vertices[2];

    mat(v1_index, v2_index) = 1.0;
    mat(v2_index, v1_index) = 1.0;

    mat(v2_index, v3_index) = 1.0;
    mat(v3_index, v2_index) = 1.0;

    mat(v1_index, v3_index) = 1.0;
    mat(v3_index, v1_index) = 1.0;
  }

  for (int i = 0; i < vertex_count; i++) {
    mat(i, i) = 1.0;
  }

  normalizeMatrix(mat);
}

double calculateEuclidDistance(pcl::PointXYZ& a, pcl::PointXYZ& b) {
  return 1 / ((a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y) + (a.z - b.z) * (a.z - b.z));
}

void constructDistanceMatrix(Eigen::MatrixXd& mat) {
  fillMatrixWithZeros(mat);

  for (int i = 0; i < face_count; i++) {
    int v1_index = mesh.polygons[i].vertices[0];
    int v2_index = mesh.polygons[i].vertices[1];
    int v3_index = mesh.polygons[i].vertices[2];

    double dst = calculateEuclidDistance(cloud_bunny.points[v1_index], cloud_bunny.points[v2_index]);
    mat(v1_index, v2_index) = dst;
    mat(v2_index, v1_index) = dst;;

    dst = calculateEuclidDistance(cloud_bunny.points[v3_index], cloud_bunny.points[v2_index]);
    mat(v2_index, v3_index) = dst;
    mat(v3_index, v2_index) = dst;

    dst = calculateEuclidDistance(cloud_bunny.points[v1_index], cloud_bunny.points[v3_index]);
    mat(v1_index, v3_index) = dst;
    mat(v3_index, v1_index) = dst;
  }

  normalizeMatrix(mat);
}

double calculateCtgA(pcl::PointXYZ& a, pcl::PointXYZ& b, pcl::PointXYZ& c) {
  double cosA = ((b.x - a.x) * (c.x - a.x) + (b.y - a.y) * (c.y - a.y) + (b.z - a.z) * (c.z - a.z))
    / (sqrt((b.x - a.x) * (b.x - a.x) + (b.y - a.y) * (b.y - a.y) + (b.z - a.z) * (b.z - a.z))
       * sqrt((c.x - a.x) * (c.x - a.x) + (c.y - a.y) * (c.y - a.y) + (c.z - a.z) * (c.z - a.z)));
  return 1.0 / (sqrt(1.0 / (cosA * cosA) - 1.0));
}

void constructConformalMatrix(Eigen::MatrixXd& mat) {
  fillMatrixWithZeros(mat);

  Eigen::MatrixXd combinatorial_matrix(vertex_count, vertex_count); // This is a helper.
  constructCombinatorialMatrix(combinatorial_matrix);

  for (int i = 0; i < vertex_count; i++) {
    for (int j = i + 1; j < vertex_count; j++) {
      if (combinatorial_matrix(i, j) > 0) {
        // There are only 2 vertices that are neighbours to both i and j.
        int n1 = 0, n2 = 0;
        bool f = false;
        for (int k = 0; k < vertex_count; k++) {
          if (combinatorial_matrix(i, k) > 0
              && combinatorial_matrix(j, k) > 0
              && k != i && k != j) {
            if (!f) {
              n1 = k;
              f = true;
            } else {
              n2 = k;
              break;
            }
          }
        }

        pcl::PointXYZ i_p = cloud_bunny.points[i];
        pcl::PointXYZ j_p = cloud_bunny.points[j];
        pcl::PointXYZ n1_p = cloud_bunny.points[n1];
        pcl::PointXYZ n2_p = cloud_bunny.points[n2];

        double ctg_n1 = calculateCtgA(n1_p, i_p, j_p);
        double ctg_n2 = calculateCtgA(n2_p, i_p, j_p);

        mat(i, j) = ctg_n1 + ctg_n2;
        mat(j, i) = mat(i, j);
      }
    }
  }

  normalizeMatrix(mat);
}

void constructOriginalMatrix(Eigen::MatrixXd& mat) {
  for (int i = 0; i < vertex_count; i++) {
    mat(i, 0) = cloud_bunny.points[i].x;
    mat(i, 1) = cloud_bunny.points[i].y;
    mat(i, 2) = cloud_bunny.points[i].z;
  }
}

void apply(Eigen::MatrixXd& orig_matrix, Eigen::MatrixXd& local_operator, Eigen::MatrixXd& out,
           pcl::PointCloud<pcl::PointXYZ>& cloud_res, pcl::PolygonMesh& mesh_res,
           const std::string& file_name) {
  out = local_operator * orig_matrix;
  for (int i = 0; i < vertex_count; i++) {
    cloud_res.points[i].x = out(i, 0);
    cloud_res.points[i].y = out(i, 1);
    cloud_res.points[i].z = out(i, 2);
  }
  pcl::toROSMsg(cloud_res, mesh_res.cloud);
  pcl::io::savePolygonFile(file_name, mesh_res);
}

int main() {
  pcl::io::loadPolygonFile("bunny.obj", mesh);
  pcl::fromROSMsg(mesh.cloud, cloud_bunny);
  Eigen::MatrixXd orig_matrix(vertex_count, 3);
  constructOriginalMatrix(orig_matrix);

  // Structures to store results.
  pcl::PolygonMesh mesh_res;
  pcl::io::loadPolygonFile("bunny.obj", mesh_res);
  pcl::PointCloud<pcl::PointXYZ> cloud_bunny_res;
  pcl::fromROSMsg(mesh_res.cloud, cloud_bunny_res);
  Eigen::MatrixXd out(vertex_count, 3);

  // GO!

  // COMBINATORIAL

  Eigen::MatrixXd w1_matrix(vertex_count, vertex_count);
  constructCombinatorialMatrix(w1_matrix);
  // 1st iteration.
  apply(orig_matrix, w1_matrix, out, cloud_bunny_res, mesh_res, "bunny-combinatorial-1.vtk");
  // 2nd iteration.
  apply(out, w1_matrix, out, cloud_bunny_res, mesh_res, "bunny-combinatorial-2.vtk");
  // 3rd iteration.
  apply(out, w1_matrix, out, cloud_bunny_res, mesh_res, "bunny-combinatorial-3.vtk");

  out = w1_matrix * out;
  out = w1_matrix * out;
  out = w1_matrix * out;
  out = w1_matrix * out;
  apply(out, w1_matrix, out, cloud_bunny_res, mesh_res, "bunny-combinatorial-n.vtk");


  // DISTANCE

  Eigen::MatrixXd w2_matrix(vertex_count, vertex_count);
  constructDistanceMatrix(w2_matrix);
  // 1st iteration.
  apply(orig_matrix, w2_matrix, out, cloud_bunny_res, mesh_res, "bunny-distance-1.vtk");
  // 2nd iteration.
  apply(out, w2_matrix, out, cloud_bunny_res, mesh_res, "bunny-distance-2.vtk");
  // 3rd iteration.
  apply(out, w2_matrix, out, cloud_bunny_res, mesh_res, "bunny-distance-3.vtk");

  out = w2_matrix * out;
  out = w2_matrix * out;
  out = w2_matrix * out;
  out = w2_matrix * out;
  apply(out, w2_matrix, out, cloud_bunny_res, mesh_res, "bunny-distance-n.vtk");


  // CONFORMAL
  Eigen::MatrixXd w3_matrix(vertex_count, vertex_count);
  constructConformalMatrix(w3_matrix);
  // 1st iteration.
  apply(orig_matrix, w3_matrix, out, cloud_bunny_res, mesh_res, "bunny-conformal-1.vtk");
  // 2nd iteration.
  apply(out, w3_matrix, out, cloud_bunny_res, mesh_res, "bunny-conformal-2.vtk");
  // 3rd iteration.
  apply(out, w3_matrix, out, cloud_bunny_res, mesh_res, "bunny-conformal-3.vtk");

  out = w3_matrix * out;
  out = w3_matrix * out;
  out = w3_matrix * out;
  out = w3_matrix * out;
  apply(out, w3_matrix, out, cloud_bunny_res, mesh_res, "bunny-conformal-n.vtk");

  return 0;
}
