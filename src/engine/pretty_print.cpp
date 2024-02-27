#include <pretty_print.hpp>

#include<iostream>

const char* ToString(Vec2 vec) {
    return ("Vec2(" +
        std::to_string(vec.x) + ", " +
        std::to_string(vec.y) + ")").c_str();
}

const char* ToString(Vec3 vec) {
    return ("Vec3(" +
        std::to_string(vec.x) + ", " +
        std::to_string(vec.y) + ", " +
        std::to_string(vec.z) + ")").c_str();
}

const char* ToString(Vec4 vec) {
    return ("Vec4(" +
        std::to_string(vec.x) + ", " +
        std::to_string(vec.y) + ", " +
        std::to_string(vec.z) + ", " +
        std::to_string(vec.w) + ")").c_str();
}

const char* ToString(Mat3 mat) {
  static char buffer[200];
  snprintf(buffer, sizeof(buffer), "\nMat3(%8f, %8f, %8f\n     %8f, %8f, %8f\n     %8f, %8f, %8f)",
  mat[0][0], mat[0][1], mat[0][2],
  mat[1][0], mat[1][1], mat[1][2],
  mat[2][0], mat[2][1], mat[2][2]);

  return buffer;
}

const char* ToString(Mat4 mat) {
  static char buffer[200];
  snprintf(buffer,
           sizeof(buffer),
           "\nMat4(%8f, %8f, %8f, %8f\n     %8f, %8f, %8f, %8f\n   "
           "  %8f, %8f, %8f, %8f\n     %8f, %8f, %8f, %8f)",
  mat[0][0], mat[0][1], mat[0][2], mat[0][3],
  mat[1][0], mat[1][1], mat[1][2], mat[1][3],
  mat[2][0], mat[2][1], mat[2][2], mat[2][3],
  mat[3][0], mat[3][1], mat[3][2], mat[3][3]);

  return buffer;
}

