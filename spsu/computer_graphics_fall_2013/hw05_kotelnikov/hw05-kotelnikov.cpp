#include <opencv2/opencv.hpp>

int main() {
  cv::Mat m = cv::imread("image.jpg");

  int height = m.rows;
  int width = m.cols;

  int height_large = height * 2;
  int width_large = width * 2;

  int height_small = height / 2;
  int width_small = width / 2;

  cv::Mat m_large_bicubic(height_large, width_large, m.type());
  cv::resize(m, m_large_bicubic, m_large_bicubic.size(), 0, 0, cv::INTER_CUBIC);
  cv::imwrite("large-bicubic.jpg", m_large_bicubic);

  cv::Mat m_small_bicubic(height_small, width_small, m.type());
  cv::resize(m, m_small_bicubic, m_small_bicubic.size(), 0, 0, cv::INTER_CUBIC);
  cv::imwrite("small-bicubic.jpg", m_small_bicubic);

  cv::Mat m_large_nn(height_large, width_large, m.type());
  cv::resize(m, m_large_nn, m_large_nn.size(), 0, 0, cv::INTER_NEAREST);
  cv::imwrite("large-nearest-neighbor.jpg", m_large_nn);

  cv::Mat m_small_nn(height_small, width_small, m.type());
  cv::resize(m, m_small_nn, m_small_nn.size(), 0, 0, cv::INTER_NEAREST);
  cv::imwrite("small-nearest-neighbor.jpg", m_small_nn);

  return 0;
}
