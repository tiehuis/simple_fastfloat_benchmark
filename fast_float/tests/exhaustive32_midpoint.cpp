#include "fast_float/fast_float.h"


#include <cassert>
#include <cmath>

template <typename T> char *to_string(T d, char *buffer) {
  auto written = std::snprintf(buffer, 64, "%.*e",
                               std::numeric_limits<T>::max_digits10 - 1, d);
  return buffer + written;
}

void allvalues() {
  char buffer[64];
  for (uint64_t w = 0; w <= 0xFFFFFFFF; w++) {
    float v;
    if ((w % 1048576) == 0) {
      std::cout << ".";
      std::cout.flush();
    }
    uint32_t word = w;
    memcpy(&v, &word, sizeof(v));
    if(std::isfinite(v)) { 
      float nextf = std::nextafterf(v, INFINITY);
      if(!std::isfinite(nextf)) { continue; }
      double v1{v};
      double v2{nextf};
      double midv{(v1 + v2) / 2};
      float expected_midv(midv);
      if(expected_midv == v) {
          std::cout << "potential round to even " << std::hexfloat << midv << std::endl;
          std::cout << std::dec;  
      }
      const char *string_end = to_string(midv, buffer);
      float result_value;
      auto result = fast_float::from_chars(buffer, string_end, result_value);
      if (result.ec != std::errc()) {
        std::cerr << "parsing error ? " << buffer << std::endl;
        abort();
      }
      if (std::isnan(v)) {
        if (!std::isnan(result_value)) {
          std::cerr << "not nan" << buffer << std::endl;
          abort();
        }
      } else if (result_value != expected_midv) {
        std::cerr << "no match ? " << buffer << std::endl;
        std::cout << "started with " << std::hexfloat << v << std::endl;
        std::cout << "got back " << std::hexfloat << result_value << std::endl; 
        std::cout << std::dec;
        abort();
      }
    }
  }
  std::cout << std::endl;
}

int main() {
  allvalues();
  std::cout << std::endl;
  std::cout << "all ok" << std::endl;
  return EXIT_SUCCESS;
}