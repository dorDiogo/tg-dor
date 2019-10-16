#ifndef FINGERPRINT_BUILDER_H
#define FINGERPRINT_BUILDER_H

#include <string>

#include "index_builder.h"

struct Fingerprint {
  int64_t text_position;
  int pattern_position;
  int weight;

  Fingerprint(int64_t text_position, int pattern_position, int weight)
      : text_position(text_position),
        pattern_position(pattern_position),
        weight(weight) {}

  bool operator<(const Fingerprint& other) const {
    if (text_position != other.text_position)
      return text_position < other.text_position;
    if (pattern_position != other.pattern_position)
      return pattern_position < other.pattern_position;
    return weight < other.weight;
  }
};

class FingerprintBuilder {
 public:
  FingerprintBuilder(std::string_view pattern,
                     const std::vector<Index>& indexes, int w,
                     const std::vector<int>& K);
  const std::vector<Fingerprint>& GetFingerprint() const {
    return fingerprints_;
  }

 private:
  std::vector<Fingerprint> fingerprints_;
};

#endif  // FINGERPRINT_BUILDER_H
