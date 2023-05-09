#pragma once

#include <array>
#include <cstdint>
#include <cstring>
#include <vector>

namespace SerializeMe {

class ByteSpan {
public:
  ByteSpan() = default;

  ByteSpan(uint8_t *ptr, size_t size) : data_(ptr), size_(size) {}

  template <size_t N>
  ByteSpan(std::array<uint8_t, N> &v) : data_(v.data()), size_(N) {}

  ByteSpan(std::vector<uint8_t> &v) : data_(v.data()), size_(v.size()) {}

  const uint8_t *data() const { return data_; }

  uint8_t *data() { return data_; }

  size_t size() const { return size_; }

private:
  uint8_t *data_ = nullptr;
  size_t size_ = 0;
};

// The wire format uses a little endian encoding (since that's efficient for
// the common platforms).
#if defined(__s390x__)
#define SERIALIZE_LITTLEENDIAN 0
#endif // __s390x__
#if !defined(SERIALIZE_LITTLEENDIAN)
#if defined(__GNUC__) || defined(__clang__) || defined(__ICCARM__)
#if (defined(__BIG_ENDIAN__) ||                                                \
     (defined(__BYTE_ORDER__) && __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__))
#define SERIALIZE_LITTLEENDIAN 0
#else
#define SERIALIZE_LITTLEENDIAN 1
#endif // __BIG_ENDIAN__
#elif defined(_MSC_VER)
#if defined(_M_PPC)
#define SERIALIZE_LITTLEENDIAN 0
#else
#define SERIALIZE_LITTLEENDIAN 1
#endif
#else
#error Unable to determine endianness, define SERIALIZE_LITTLEENDIAN.
#endif
#endif // !defined(SERIALIZE_LITTLEENDIAN)

template <typename T> inline T EndianSwap(T t) {
  static_assert(std::is_arithmetic<T>::value,
                "This function accepts only numeric types");
#if defined(_MSC_VER)
#define DESERIALIZE_ME_BYTESWAP16 _byteswap_ushort
#define DESERIALIZE_ME_BYTESWAP32 _byteswap_ulong
#define DESERIALIZE_ME_BYTESWAP64 _byteswap_uint64
#else
#if defined(__GNUC__) && __GNUC__ * 100 + __GNUC_MINOR__ < 408 &&              \
    !defined(__clang__)
// __builtin_bswap16 was missing prior to GCC 4.8.
#define DESERIALIZE_ME_BYTESWAP16(x)                                           \
  static_cast<uint16_t>(__builtin_bswap32(static_cast<uint32_t>(x) << 16))
#else
#define DESERIALIZE_ME_BYTESWAP16 __builtin_bswap16
#endif
#define DESERIALIZE_ME_BYTESWAP32 __builtin_bswap32
#define DESERIALIZE_ME_BYTESWAP64 __builtin_bswap64
#endif
  if (sizeof(T) == 1) { // Compile-time if-then's.
    return t;
  } else if (sizeof(T) == 2) {
    union {
      T t;
      uint16_t i;
    } u;
    u.t = t;
    u.i = DESERIALIZE_ME_BYTESWAP16(u.i);
    return u.t;
  } else if (sizeof(T) == 4) {
    union {
      T t;
      uint32_t i;
    } u;
    u.t = t;
    u.i = DESERIALIZE_ME_BYTESWAP32(u.i);
    return u.t;
  } else if (sizeof(T) == 8) {
    union {
      T t;
      uint64_t i;
    } u;
    u.t = t;
    u.i = DESERIALIZE_ME_BYTESWAP64(u.i);
    return u.t;
  } else {
    std::runtime_error("Problem with IndianSwap");
  }
}

/**
 * @brief This function looks at the S bytes of the buffer at the position given
 * by offset and cast them into a numeric value (integer or real number) with
 * type T.
 *
 * @param buffer   Input. We look at the position buffer.data()+offset
 * @param dest     destination of the cast
 * @return         The new (shifted) offset
 */
template <typename T>
inline void DeserializeFromBuffer(ByteSpan &buffer, T &dest) {
  static_assert(std::is_arithmetic<T>::value,
                "This function accepts only numeric types");
  const auto S = sizeof(T);
  if (S > buffer.size()) {
    throw std::runtime_error("DeserializeFromBuffer: buffer overflow");
  }
  dest = *(reinterpret_cast<const T *>(buffer.data()));

#if SERIALIZE_LITTLEENDIAN == 0
  dest = EndianSwap<T>(dest);
#endif
  buffer = ByteSpan(buffer.data() + S, buffer.size() - S);
}

template <>
inline void DeserializeFromBuffer(ByteSpan &buffer, std::string &dest) {
  uint32_t S = 0;
  DeserializeFromBuffer(buffer, S);

  if (S > buffer.size()) {
    throw std::runtime_error("DeserializeFromBuffer: buffer overflow");
  }

  dest.assign(reinterpret_cast<const char *>(buffer.data()), S);

  buffer = ByteSpan(buffer.data() + S, buffer.size() - S);
}

template <typename T>
inline void DeserializeFromBuffer(ByteSpan &buffer, std::vector<T> &dest) {
  static_assert(std::is_arithmetic<T>::value,
                "This function accepts only vectors of numeric types");

  uint32_t num_values = 0;
  DeserializeFromBuffer(buffer, num_values);

  const size_t S = num_values * sizeof(T);

  if (S > buffer.size()) {
    throw std::runtime_error("DeserializeFromBuffer: buffer overflow");
  }

  dest.resize(num_values);

  if (SERIALIZE_LITTLEENDIAN || sizeof(T) == 1) {
    memcpy(dest.data(), buffer.data(), S);
    buffer = ByteSpan(buffer.data() + S, buffer.size() - S);
  } else {
    for (size_t i = 0; i < num_values; i++) {
      DeserializeFromBuffer(buffer, dest[i]);
    }
  }
}

template <typename T>
inline void SerializeIntoBuffer(ByteSpan &buffer, const T &value) {
  static_assert(std::is_arithmetic<T>::value,
                "This function accepts only numeric types");
  const size_t S = sizeof(T);
  if (S > buffer.size()) {
    throw std::runtime_error("DeserializeFromBuffer: buffer overflow");
  }

#if SERIALIZE_LITTLEENDIAN == 0
  *(reinterpret_cast<T *>(buffer.data())) = EndianSwap<T>(value);
#else
  *(reinterpret_cast<T *>(buffer.data())) = value;
#endif
  buffer = ByteSpan(buffer.data() + S, buffer.size() - S);
}

template <>
inline void SerializeIntoBuffer(ByteSpan &buffer, const std::string &str) {
  const uint32_t S = static_cast<uint32_t>(str.size());
  SerializeIntoBuffer(buffer, S);

  if (S > buffer.size()) {
    throw std::runtime_error("SerializeIntoBuffer: buffer overflow");
  }

  memcpy(buffer.data(), str.data(), S);
  buffer = ByteSpan(buffer.data() + S, buffer.size() - S);
}

template <typename T>
inline void SerializeIntoBuffer(ByteSpan &buffer, const std::vector<T> &vect) {
  static_assert(std::is_arithmetic<T>::value,
                "This function accepts only numeric types");

  const uint32_t num_values = static_cast<uint32_t>(vect.size());
  SerializeIntoBuffer(buffer, num_values);

  const size_t S = num_values * sizeof(T);

  if (S > buffer.size()) {
    throw std::runtime_error("SerializeIntoBuffer: buffer overflow");
  }

  if (SERIALIZE_LITTLEENDIAN || sizeof(T) == 1) {
    memcpy(buffer.data(), vect.data(), S);
    buffer = ByteSpan(buffer.data() + S, buffer.size() - S);
  } else {
    for (const T &v : vect) {
      SerializeIntoBuffer(buffer, v);
    }
  }
}

template <typename T> inline size_t BufferSize(const T &val) {
  static_assert(std::is_arithmetic<T>::value,
                "This function accepts only numeric types");
  return sizeof(T);
}

template <> inline size_t BufferSize(const std::string &str) {
  return sizeof(uint32_t) + str.size();
}

template <typename T> inline size_t BufferSize(const std::vector<T> &vect) {
  static_assert(std::is_arithmetic<T>::value,
                "This function accepts only numeric types");
  return sizeof(uint32_t) + vect.size() * sizeof(T);
}

} // namespace SerializeMe
