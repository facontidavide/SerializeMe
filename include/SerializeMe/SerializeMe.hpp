/**
 * MIT License
 *
 * Copyright (c) 2019-2023 Davide Faconti
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 *all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 **/

#pragma once

#include <array>
#include <cstdint>
#include <cstring>
#include <limits>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <vector>

namespace SerializeMe
{

template <typename T>
class Span
{
public:
  Span() = default;

  Span(T* ptr, size_t size) : data_(ptr), size_(size)
  {}

  template <size_t N>
  Span(std::array<T, N>& v) : data_(v.data()), size_(N)
  {}

  Span(std::vector<T>& v) : data_(v.data()), size_(v.size())
  {}

  T const* data() const
  {
    return data_;
  }

  T* data()
  {
    return data_;
  }

  size_t size() const
  {
    return size_;
  }

private:
  T* data_ = nullptr;
  size_t size_ = 0;
};

using SpanBytes = Span<uint8_t>;
using SpanBytesConst = Span<uint8_t const>;
using StringSize = uint16_t;

//------------- Forward declarations of BufferSize ------------------

template <typename T>
size_t BufferSize(const T& val);

template <>
size_t BufferSize(const std::string& str);

template <class T, size_t N>
size_t BufferSize(const std::array<T, N>& v);

template <template <class, class> class Container, class T, class... TArgs>
size_t BufferSize(const Container<T, TArgs...>& vect);

//---------- Forward declarations of DeserializeFromBuffer -----------

template <typename T>
void DeserializeFromBuffer(SpanBytesConst& buffer, T& dest);

template <>
void DeserializeFromBuffer(SpanBytesConst& buffer, std::string& str);

template <class T, size_t N>
void DeserializeFromBuffer(SpanBytesConst& buffer, std::array<T, N>& v);

template <template <class, class> class Container, class T, class... TArgs>
void DeserializeFromBuffer(SpanBytesConst& buffer, Container<T, TArgs...>& dest);

//---------- Forward declarations of SerializeIntoBuffer -----------

template <typename T>
void SerializeIntoBuffer(SpanBytes& buffer, const T& value);

template <>
void SerializeIntoBuffer(SpanBytes& buffer, const std::string& str);

template <class T, size_t N>
void SerializeIntoBuffer(SpanBytes& buffer, const std::array<T, N>& v);

template <template <class, class> class Container, class T, class... TArgs>
void SerializeIntoBuffer(SpanBytes& buffer, const Container<T, TArgs...>& vect);

//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------

// The wire format uses a little endian encoding (since that's efficient for
// the common platforms).
#if defined(__s390x__)
#define SERIALIZE_LITTLEENDIAN 0
#endif   // __s390x__
#if !defined(SERIALIZE_LITTLEENDIAN)
#if defined(__GNUC__) || defined(__clang__) || defined(__ICCARM__)
#if (defined(__BIG_ENDIAN__) ||                                                          \
     (defined(__BYTE_ORDER__) && __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__))
#define SERIALIZE_LITTLEENDIAN 0
#else
#define SERIALIZE_LITTLEENDIAN 1
#endif   // __BIG_ENDIAN__
#elif defined(_MSC_VER)
#if defined(_M_PPC)
#define SERIALIZE_LITTLEENDIAN 0
#else
#define SERIALIZE_LITTLEENDIAN 1
#endif
#else
#error Unable to determine endianness, define SERIALIZE_LITTLEENDIAN.
#endif
#endif   // !defined(SERIALIZE_LITTLEENDIAN)

template <typename T>
inline T EndianSwap(T t)
{
  static_assert(std::is_arithmetic<T>::value, "This function accepts only numeric types");
#if defined(_MSC_VER)
#define DESERIALIZE_ME_BYTESWAP16 _byteswap_ushort
#define DESERIALIZE_ME_BYTESWAP32 _byteswap_ulong
#define DESERIALIZE_ME_BYTESWAP64 _byteswap_uint64
#else
#if defined(__GNUC__) && __GNUC__ * 100 + __GNUC_MINOR__ < 408 && !defined(__clang__)
// __builtin_bswap16 was missing prior to GCC 4.8.
#define DESERIALIZE_ME_BYTESWAP16(x)                                                     \
  static_cast<uint16_t>(__builtin_bswap32(static_cast<uint32_t>(x) << 16))
#else
#define DESERIALIZE_ME_BYTESWAP16 __builtin_bswap16
#endif
#define DESERIALIZE_ME_BYTESWAP32 __builtin_bswap32
#define DESERIALIZE_ME_BYTESWAP64 __builtin_bswap64
#endif
  if constexpr (sizeof(T) == 1)
  {   // Compile-time if-then's.
    return t;
  }
  else if constexpr (sizeof(T) == 2)
  {
    union
    {
      T t;
      uint16_t i;
    } u;
    u.t = t;
    u.i = DESERIALIZE_ME_BYTESWAP16(u.i);
    return u.t;
  }
  else if constexpr (sizeof(T) == 4)
  {
    union
    {
      T t;
      uint32_t i;
    } u;
    u.t = t;
    u.i = DESERIALIZE_ME_BYTESWAP32(u.i);
    return u.t;
  }
  else if (sizeof(T) == 8)
  {
    union
    {
      T t;
      uint64_t i;
    } u;
    u.t = t;
    u.i = DESERIALIZE_ME_BYTESWAP64(u.i);
    return u.t;
  }
  else
  {
    std::runtime_error("Problem with IndianSwap");
  }
}

template <class Type>
struct Serializer
{
  Serializer() = delete;

  template <class Operator>
  void operator()(Type& obj, Operator& op);
};

template <typename T, class = void>
struct is_serializer_specialized : std::false_type
{
};

template <typename T>
struct is_serializer_specialized<T, decltype(Serializer<T>(), void())> : std::true_type
{
};

template <typename T>
constexpr bool is_serializer_defined()
{
  return is_serializer_specialized<T>::value;
}

template <typename T>
inline constexpr bool is_arithmetic()
{
  return std::is_arithmetic_v<T> || std::is_same_v<T, std::byte>;
}

template <typename>
struct is_std_vector : std::false_type
{
};

template <typename T, typename A>
struct is_std_vector<std::vector<T, A>> : std::true_type
{
};

template <typename>
struct is_std_array : std::false_type
{
};

template <typename T, size_t S>
struct is_std_array<std::array<T, S>> : std::true_type
{
};

template <typename T>
inline constexpr bool is_vector()
{
  return (is_std_vector<T>::value || is_std_array<T>::value);
}

//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------

template <typename T>
inline size_t BufferSize(const T& val)
{
  if constexpr (is_arithmetic<T>())
  {
    return sizeof(T);
  }
  else
  {
    static_assert(is_serializer_defined<T>(), "Missing specialization of class "
                                              "Serialize<>. Check errors below");

    if constexpr (is_serializer_defined<T>())
    {
      size_t total_size = 0;
      auto func = [&total_size](auto const& field) { total_size += BufferSize(field); };

      Serializer<T>().operator()(const_cast<T&>(val), func);
      return total_size;
    }
  }
}

template <>
inline size_t BufferSize(const std::string& str)
{
  return sizeof(StringSize) + str.size();
}

template <class T, size_t N>
inline size_t BufferSize(const std::array<T, N>& v)
{
  return sizeof(uint32_t) + BufferSize(T{}) * N;
}

template <template <class, class> class Container, class T, class... TArgs>
inline size_t BufferSize(const Container<T, TArgs...>& vect)
{
  if constexpr (std::is_trivially_copyable_v<T> && is_vector<Container<T, TArgs...>>())
  {
    return sizeof(uint32_t) + std::size(vect) * BufferSize<T>(T{});
  }
  else
  {
    auto size = sizeof(uint32_t);
    for (const auto& v : vect)
    {
      size += BufferSize(v);
    }
    return size;
  }
}

//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------

template <typename T>
inline void DeserializeFromBuffer(SpanBytesConst& buffer, T& dest)
{
  if constexpr (std::is_arithmetic_v<T> || std::is_same_v<T, std::byte>)
  {
    auto const S = sizeof(T);
    if (S > buffer.size())
    {
      throw std::runtime_error("DeserializeFromBuffer: buffer overflow");
    }
    dest = *(reinterpret_cast<T const*>(buffer.data()));

#if SERIALIZE_LITTLEENDIAN == 0
    dest = EndianSwap<T>(dest);
#endif
    buffer = SpanBytesConst(buffer.data() + S, buffer.size() - S);   // NOLINT
  }
  else
  {
    static_assert(is_serializer_defined<T>(), "Missing specialization of class "
                                              "Serialize<>. Check errors below");
    if constexpr (is_serializer_defined<T>())
    {
      auto func = [&buffer](auto& field) { DeserializeFromBuffer(buffer, field); };
      Serializer<T>().operator()(const_cast<T&>(dest), func);
    }
  }
}

template <>
inline void DeserializeFromBuffer(SpanBytesConst& buffer, std::string& dest)
{
  StringSize size = 0;
  DeserializeFromBuffer(buffer, size);

  if (size > buffer.size())
  {
    throw std::runtime_error("DeserializeFromBuffer: buffer overflow");
  }

  dest.assign(reinterpret_cast<char const*>(buffer.data()), size);
  buffer = SpanBytesConst(buffer.data() + size, buffer.size() - size);
}

template <typename T, size_t N>
inline void DeserializeFromBuffer(SpanBytesConst& buffer, std::array<T, N>& dest)
{
  uint32_t num_values = 0;
  DeserializeFromBuffer(buffer, num_values);

  if (num_values * BufferSize(T{}) > buffer.size())
  {
    throw std::runtime_error("DeserializeFromBuffer: buffer overflow");
  }

  if (N != num_values)
  {
    throw std::runtime_error("DeserializeFromBuffer: unexpected size");
  }

  if constexpr (sizeof(T) == 1)
  {
    memcpy(dest.data(), buffer.data(), num_values);
    buffer = SpanBytesConst(buffer.data() + num_values, buffer.size() - num_values);
  }
  else
  {
    for (size_t i = 0; i < num_values; i++)
    {
      DeserializeFromBuffer(buffer, dest[i]);
    }
  }
}

template <template <class, class> class Container, class T, class... TArgs>
inline void DeserializeFromBuffer(SpanBytesConst& buffer, Container<T, TArgs...>& dest)
{
  uint32_t num_values = 0;
  DeserializeFromBuffer(buffer, num_values);

  // if the container offers contiguous memory, you can just use memcpy
  if constexpr (sizeof(T) == 1 && is_vector<Container<T, TArgs...>>())
  {
    if constexpr (is_std_vector<Container<T, TArgs...>>::value)
    {
      dest.resize(num_values);
    }
    else if constexpr (std::is_array_v<Container<T, TArgs...>>)
    {
      if (std::size(dest) != num_values)
      {
        throw std::runtime_error("DeserializeFromBuffer: wrong size in static container");
      }
    }

    const size_t size = num_values * BufferSize(T{});
    memcpy(dest.data(), buffer.data(), size);
    buffer = SpanBytesConst(buffer.data() + size, buffer.size() - size);
  }
  else
  {
    dest.clear();
    for (size_t i = 0; i < num_values; i++)
    {
      T temp;
      DeserializeFromBuffer(buffer, temp);
      std::back_inserter(dest) = std::move(temp);
    }
  }
}

//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------

template <typename T>
inline void SerializeIntoBuffer(SpanBytes& buffer, T const& value)
{
  if constexpr (is_arithmetic<T>())
  {
    const size_t S = sizeof(T);
    if (S > buffer.size())
    {
      throw std::runtime_error("SerializeIntoBuffer: buffer overflow");
    }
#if SERIALIZE_LITTLEENDIAN == 0
    *(reinterpret_cast<T*>(buffer.data())) = EndianSwap<T>(value);
#else
    *(reinterpret_cast<T*>(buffer.data())) = value;
#endif
    buffer = SpanBytes(buffer.data() + S, buffer.size() - S);   // NOLINT
  }
  else
  {
    static_assert(is_serializer_defined<T>(), "Missing specialization of class "
                                              "Serialize<>. Check errors below");
    if constexpr (is_serializer_defined<T>())
    {
      auto func = [&buffer](auto const& field) { SerializeIntoBuffer(buffer, field); };
      Serializer<T>().operator()(const_cast<T&>(value), func);
    }
  }
}

template <>
inline void SerializeIntoBuffer(SpanBytes& buffer, std::string const& str)
{
  if (str.size() > std::numeric_limits<StringSize>::max())
  {
    throw std::runtime_error("SerializeIntoBuffer: string exceeds maximum size");
  }

  if ((str.size() + sizeof(StringSize)) > buffer.size())
  {
    throw std::runtime_error("SerializeIntoBuffer: buffer overflow");
  }

  const auto size = static_cast<StringSize>(str.size());
  SerializeIntoBuffer(buffer, size);

  memcpy(buffer.data(), str.data(), size);
  buffer = SpanBytes(buffer.data() + size, buffer.size() - size);
}

template <typename T, size_t N>
inline void SerializeIntoBuffer(SpanBytes& buffer, std::array<T, N> const& vect)
{
  if (N > std::numeric_limits<uint32_t>::max())
  {
    throw std::runtime_error("SerializeIntoBuffer: array exceeds maximum size");
  }

  if ((N + sizeof(uint32_t)) > buffer.size())
  {
    throw std::runtime_error("SerializeIntoBuffer: buffer overflow");
  }

  const auto size = static_cast<uint32_t>(vect.size());
  SerializeIntoBuffer(buffer, size);

  for (const T& v : vect)
  {
    SerializeIntoBuffer(buffer, v);
  }
}

template <template <class, class> class Container, class T, class... TArgs>
inline void SerializeIntoBuffer(SpanBytes& buffer, Container<T, TArgs...> const& vect)
{
  const auto num_values = static_cast<uint32_t>(vect.size());
  SerializeIntoBuffer(buffer, num_values);

  // can use memcpy if the size of T is 1
  if constexpr (sizeof(T) == 1 && is_vector<Container<T, TArgs...>>())
  {
    const size_t size = num_values;
    if (size > buffer.size())
    {
      throw std::runtime_error("SerializeIntoBuffer: buffer overflow");
    }
    memcpy(buffer.data(), vect.data(), size);
    buffer = SpanBytes(buffer.data() + size, buffer.size() - size);
  }
  else
  {
    for (const T& v : vect)
    {
      SerializeIntoBuffer(buffer, v);
    }
  }
}

}   // namespace SerializeMe
