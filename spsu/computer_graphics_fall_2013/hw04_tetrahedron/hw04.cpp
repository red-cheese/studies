#include <opencv2/opencv.hpp>
#include <float.h>
#include <math.h>

const int nx = 400; // Picture width, pixels.
const int ny = 400; // Picture height, pixels.

const double n = 1.0, f = -2.0, l = -2.0, r = 2.0, b = -2.0, t = 2.0;

// Vertices in real world coordinates.
cv::Vec4d v1(0.0, 0.0, 1.0, 1.0);
// Helper to calculate view direction.
cv::Vec3d v1_helper(0.0, 0.0, 1.0);
cv::Vec4d v2(-2.0, 0.0, -2.0, 1.0); 
cv::Vec4d v3(2.0, 2.0, -2.0, 1.0);
cv::Vec4d v4(2.0, -2.0, -2.0, 1.0);

// Colors of vertices, BGR.
double v1_b = 0.0;
double v1_g = 254.0;
double v1_r = 0.0;

double v2_b = 254.0;
double v2_g = 0.0;
double v2_r = 0.0;

double v3_g = 0.0;
double v3_b = 0.0;
double v3_r = 254.0;

double v4_g = 254.0;
double v4_b = 254.0;
double v4_r = 254.0;

// View point (camera).
cv::Vec3d e(0.0, 0.0, 3.0);
cv::Vec3d t_vector(0.0, 1.0, 0.0); // Up.
cv::Vec3d g = v1_helper - e; // View direction.

// Z-buffer.
double z_buffer[nx][ny];

// Two dummy util functions.

int min_coordinate(double c1, double c2, double c3) {
  return (int) std::min(std::min(c1, c2), c3);
}

int max_coordinate(double c1, double c2, double c3) {
  return (int) std::max(std::max(c1, c2), c3) + 1;
}

// This functions calculates f(x, y) where f is a line between (x1, y1) and (x2, y2).
double calc_line(double x1, double y1, double x2, double y2, double x, double y) {
  return (y1 - y2) * x + (x2 - x1) * y + x1 * y2 - x2 * y1;
}

void draw_triangle(cv::Mat& image,
		   double ax, double ay,
		   double bx, double by,
		   double cx, double cy,
		   double a_depth, double b_depth, double c_depth,
		   double a_green, double a_blue, double a_red,
		   double b_green, double b_blue, double b_red,
		   double c_green, double c_blue, double c_red) {
  int x_min = min_coordinate(ax, bx, cx);
  int x_max = max_coordinate(ax, bx, cx);
  int y_min = min_coordinate(ay, by, cy);
  int y_max = max_coordinate(ay, by, cy);
  double alpha, beta, gamma, green, blue, red;
  for (int i = x_min; i <= x_max; i++) {
    for (int j = y_min; j <= y_max; j++) {
      double calc_line_a = calc_line(bx, by, cx, cy, ax, ay);
      double calc_line_b = calc_line(cx, cy, ax, ay, bx, by);
      double calc_line_c = calc_line(ax, ay, bx, by, cx, cy);
      alpha = calc_line(bx, by, cx, cy, i, j) / calc_line_a;
      beta = calc_line(cx, cy, ax, ay, i, j) / calc_line_b;
      gamma = calc_line(ax, ay, bx, by, i, j) / calc_line_c;
      if (alpha >= 0 && beta >= 0 && gamma >= 0) {
	// (-1.5, -1.0) is our helper point for shared edges.
	if ((alpha > 0 or calc_line_a * calc_line(bx, by, cx, cy, -1.5, -1.0) > 0)
	    and (beta > 0 or calc_line_b * calc_line(cx, cy, ax, ay, -1.5, -1.0) > 0)
	    and (gamma > 0 or calc_line_c * calc_line(ax, ay, bx, by, -1.5, -1.0) > 0)) {
	  double depth = alpha * a_depth + beta * b_depth + gamma * c_depth;
	  if (depth < z_buffer[i][j]) {
	    z_buffer[i][j] = depth;
	    blue = alpha * a_blue + beta * b_blue + gamma * c_blue;
	    image.at<cv::Vec3b>(j, i)[0] = blue;
	    green = alpha * a_green + beta * b_green + gamma * c_green;
	    image.at<cv::Vec3b>(j, i)[1] = green;
	    red = alpha * a_red + beta * b_red + gamma * c_red;
	    image.at<cv::Vec3b>(j, i)[2] = red;
	  }
	}
      }
    }
  }
}

int main() {
  cv::Mat m(nx, ny, CV_8UC3);

  // Init z-buffer.
  for (int i = 0; i < nx; i++) {
    for (int j = 0; j < ny; j++) {
      z_buffer[i][j] = DBL_MAX;
    }
  }
  
  // Calculate matrices.
  // Camera tranformation (M_cam).
  cv::Vec3d w = - g * (1 / cv::norm(g));
  cv::Vec3d u = t_vector.cross(w) * (1 / cv::norm(t_vector.cross(w)));
  cv::Vec3d v = w.cross(u);
  cv::Matx<double, 4, 4> m1(u[0], u[1], u[2], 0.0,
			    v[0], v[1], v[2], 0.0,
			    w[0], w[1], w[2], 0.0,
			    0.0, 0.0, 0.0, 1.0);
  cv::Matx<double, 4, 4> m2(1.0, 0.0, 0.0, -e[0],
			    0.0, 1.0, 0.0, -e[1],
			    0.0, 0.0, 1.0, -e[2],
			    0.0, 0.0, 0.0, 1.0);
  cv::Matx<double, 4, 4> M_cam = m1 * m2;

  // Viewport transformation (M_vp).
  cv::Matx<double, 4, 4> M_vp(nx / 2.0, 0.0, 0.0, (nx - 1) / 2.0,
			      0.0, ny / 2.0, 0.0, (ny - 1) / 2.0,
			      0.0, 0.0, 1.0, 0.0,
			      0.0, 0.0, 0.0, 1.0);

  // Build projection matrix (M_per = M_orth * P).
  /*
  cv::Matx<double, 4, 4> M_orth(2.0 / (r - l), 0.0, 0.0, - (r + l) / (r - l),
				0.0, 2.0 / (t - b), 0.0, - (t + b) / (t - b),
				0.0, 0.0, 2.0 / (n - f), - (n + f) / (n - f),
				0.0, 0.0, 0.0, 1.0);
  cv::Matx<double, 4, 4> P(n, 0.0, 0.0, 0.0,
			   0.0, n, 0.0, 0.0,
			   0.0, 0.0, n + f, - f * n,
			   0.0, 0.0, 1.0, 0.0);
  cv::Matx<double, 4, 4> M_per = M_orth * P;
  */
  cv::Matx<double, 4, 4> M_per(2.0 * n / (r - l), 0.0, (l + r) / (l - r), 0.0,
			       0.0, 2.0 * n / (t - b), (b + t) / (b - t), 0.0,
			       0.0, 0.0, (f + n) / (n - f), 2.0 * f * n / (f - n),
			       0.0, 0.0, 1.0, 0.0);
  
  cv::Matx<double, 4, 4> M_result = M_vp * M_per * M_cam;

  // Project vertices.
  cv::Vec4d a = M_result * v1;
  cv::Point p1(a[0] / a[3], a[1] / a[3]);
  a = M_result * v2;
  cv::Point p2(a[0] / a[3], a[1] / a[3]);
  a = M_result * v3;
  cv::Point p3(a[0] / a[3], a[1] / a[3]);
  a = M_result * v4;
  cv::Point p4(a[0] / a[3], a[1] / a[3]);

  // Draw sides (triangles).
  draw_triangle(m,
		p4.x, p4.y,
		p2.x, p2.y,
		p1.x, p1.y,
		e[2] - v4[2], e[2] - v2[2], e[2] - v1[2], // Depths.
		v4_g, v4_b, v4_r,
		v2_g, v2_b, v2_r,
		v1_g, v1_b, v1_r);
  draw_triangle(m,
		p4.x, p4.y,
		p2.x, p2.y,
		p3.x, p3.y,
		e[2] - v4[2], e[2] - v2[2], e[2] - v3[2],
		v4_g, v4_b, v4_r,
		v2_g, v2_b, v2_r,
		v3_g, v3_b, v3_r);
  draw_triangle(m,
		p1.x, p1.y,
		p2.x, p2.y,
		p3.x, p3.y,
		e[2] - v1[2], e[2] - v2[2], e[2] - v3[2],
		v1_g, v1_b, v1_r,
		v2_g, v2_b, v2_r,
		v3_g, v3_b, v3_r);
  draw_triangle(m,
		p4.x, p4.y,
		p1.x, p1.y,
		p3.x, p3.y,
		e[2] - v4[2], e[2] - v1[2], e[2] - v3[2],
		v4_g, v4_b, v4_r,
		v1_g, v1_b, v1_r,
		v3_g, v3_b, v3_r);

  cv::imwrite("hw04.png", m);

  return 0;
}
