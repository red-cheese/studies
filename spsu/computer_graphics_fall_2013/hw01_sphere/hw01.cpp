#include <math.h>
#include <opencv2/opencv.hpp>

// Width of our square picture, 200px.
const int pictureWidth = 200;

// Sphere radius, 100px.
const int r = 100;

// Sphere center. The sphere is 50px away from us.
cv::Vec3d c(0, 0, 150);

// Camera (and origin).
cv::Vec3d e(0, 0, 0);

// Light.
cv::Vec3d l(25, 25, 0);

int discriminant(cv::Vec3d d) {
  return pow((d.t() * (e - c))[0], 2)
      - (d.t() * d)[0] * (((e - c).t() * (e - c))[0] - r * r);
}

// Calculates the first intersection point of the sphere and the ray with
// the direction d.
int calculateIntersectionPoint(cv::Vec3d d) {
  double t1 = (- (d.t() * (e - c))[0] + sqrt(discriminant(d)))
      / (d.t() * d)[0];
  double t2 = (- (d.t() * (e - c))[0] - sqrt(discriminant(d)))
      / (d.t() * d)[0];
  return std::min(t1, t2);
}

// Calculates the cosine between two cv::Vec3d vectors.
double cosVec3d(cv::Vec3d a, cv::Vec3d b) {
  // Sorry, I'm afraid OpenCV in Debian repos is out of date :( There aren't
  // even operator[] and operator/ for cv::Vec3d vectors.
  a = a * (1 / cv::norm(a));
  b = b * (1 / cv::norm(b));
  return (a.t() * b)[0];
}

int main() {
  cv::Mat m(pictureWidth, pictureWidth, CV_8UC1);

  for (int i = 0; i < m.rows; i++) {
    for (int j = 0; j < m.cols; j++) {
      // Get a new direction using a bit of magic for coordinates.
      cv::Vec3d d = e + cv::Vec3d(j - 100, 100 - i, 50);

      if (discriminant(d) >= 0) {
	// The ray intersects the sphere.
	double t = calculateIntersectionPoint(d);

	// Sphere surface normal.
	cv::Vec3d n = e + t * d - c;

	// Direction from the sphere to light.
	cv::Vec3d onLight = l - (e + t * d);

	// Lambertian.
	double color = std::max(0.0, cosVec3d(n, onLight)) * 255;
	m.at<char>(i, j) = color;
      } else {
	// The ray doesn't intersect the sphere.
	m.at<char>(i, j) = 0;
      }
    }
  }

  cv::imwrite("hw01.png", m);

  return 0;
}
