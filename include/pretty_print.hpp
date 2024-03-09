#pragma once

#include <iostream>
#include <string>
#include "math_types.hpp"
#include "glm/gtx/string_cast.hpp"

#define BUFSIZE 10

static char* buf[BUFSIZE];
static int cur = -1;

static void nextBuf() {
  cur++;
  if (cur == BUFSIZE) cur = 0;
  free(buf[cur]);
}

template<typename T>
const char* ToString(T glmStruct) {
  nextBuf();
  std::string result = glm::to_string(glmStruct);

  buf[cur] = new char[result.size() + 1]();
  memcpy(buf[cur], result.c_str(), result.size());
  buf[cur][result.size()] = 0;
  return buf[cur];
}
