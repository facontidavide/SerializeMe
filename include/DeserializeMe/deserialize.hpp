#ifndef DESERIALIZE_HPP
#define DESERIALIZE_HPP

#include <DeserializeMe/span.hpp>

using ByteSpan = nonstd::span<uint8_t>;

template<typename T> inline
T EndianSwap(T t)
{
    static_assert( std::is_arithmetic<T>::value, "This function accepts only numeric types");
  #if defined(_MSC_VER)
    #define DESERIALIZE_ME_BYTESWAP16 _byteswap_ushort
    #define DESERIALIZE_ME_BYTESWAP32 _byteswap_ulong
    #define DESERIALIZE_ME_BYTESWAP64 _byteswap_uint64
  #else
    #if defined(__GNUC__) && __GNUC__ * 100 + __GNUC_MINOR__ < 408 && !defined(__clang__)
      // __builtin_bswap16 was missing prior to GCC 4.8.
      #define DESERIALIZE_ME_BYTESWAP16(x) \
        static_cast<uint16_t>(__builtin_bswap32(static_cast<uint32_t>(x) << 16))
    #else
      #define DESERIALIZE_ME_BYTESWAP16 __builtin_bswap16
    #endif
    #define DESERIALIZE_ME_BYTESWAP32 __builtin_bswap32
    #define DESERIALIZE_ME_BYTESWAP64 __builtin_bswap64
  #endif
  if (sizeof(T) == 1) {   // Compile-time if-then's.
    return t;
  } else if (sizeof(T) == 2) {
    union { T t; uint16_t i; } u;
    u.t = t;
    u.i = DESERIALIZE_ME_BYTESWAP16(u.i);
    return u.t;
  } else if (sizeof(T) == 4) {
    union { T t; uint32_t i; } u;
    u.t = t;
    u.i = DESERIALIZE_ME_BYTESWAP32(u.i);
    return u.t;
  } else if (sizeof(T) == 8) {
    union { T t; uint64_t i; } u;
    u.t = t;
    u.i = DESERIALIZE_ME_BYTESWAP64(u.i);
    return u.t;
  } else {
    std::runtime_error("Problem with IndianSwap");
  }
}

#if defined(__s390x__)
  #define AM_I_LITTLEENDIAN 0
#endif // __s390x__
#if !defined(AM_I_LITTLEENDIAN)
  #if defined(__GNUC__) || defined(__clang__)
    #ifdef __BIG_ENDIAN__
      #define AM_I_LITTLEENDIAN 0
    #else
      #define AM_I_LITTLEENDIAN 1
    #endif // __BIG_ENDIAN__
  #elif defined(_MSC_VER)
    #if defined(_M_PPC)
      #define AM_I_LITTLEENDIAN 0
    #else
      #define AM_I_LITTLEENDIAN 1
    #endif
  #else
    #error Unable to determine endianness, define AM_I_LITTLEENDIAN.
  #endif
#endif // !defined(AM_I_LITTLEENDIAN)

/** This function looks at the first S bytes of the buffer, where S is size(T),
 * and cast them into a numeric value (integer or real number) with type T.
 *
 * Value is returned in "dest"
 */

/**
 * @brief This function looks at the first S bytes of the buffer, where S is size(T),
 * and cast them into a numeric value (integer or real number) with type T.
 *
 * @param buffer          Input. We are interested only in the initial bytes
 * @param dest            Output.
 * @param swap_endianess  If true, swap the bytes to take into account different endianess.
 * @return                The new buffer, where the first S bytes have been removed
 */
template<typename T> inline ByteSpan DeserializeFromBuffer( const ByteSpan& buffer, T& dest, bool swap_endianess = false )
{
    static_assert( std::is_arithmetic<T>::value, "This function accepts only numeric types");
    const auto S = sizeof(T);
    if( S > buffer.size())
    {
       std::runtime_error("DeserializeFromBuffer: buffer overflow");
    }
    dest = *( reinterpret_cast<T*>( buffer.data() ));
    if( swap_endianess )
    {
        dest = EndianSwap<T>(dest);
    }
    return ByteSpan( buffer.data() + S, buffer.size() - S);
}

/**
 * @brief This function looks at the S bytes of the buffer at the position given by offset
 * and cast them into a numeric value (integer or real number) with type T.
 *
 * @param buffer          Input. We look at the position buffer.data()+offset
 * @param offset          Offset in bytes.
 * @param dest            Output.
 * @param swap_endianess  If true, swap the bytes to take into account different endianess.
 * @return                The new (shifted) offset
 */
template<typename T> inline int DeserializeFromBuffer( const ByteSpan& buffer, int offset, 
                                                      T& dest, bool swap_endianess = false )
{
    static_assert( std::is_arithmetic<T>::value, "This function accepts only numeric types");
    const auto S = sizeof(T);
    if( offset + S > buffer.size())
    {
       std::runtime_error("DeserializeFromBuffer: buffer overflow");
    }
    dest = *( reinterpret_cast<T*>( buffer.data()+offset ));
    if( swap_endianess )
    {
        dest = EndianSwap<T>(dest);
    }
    return offset + S;
}


template<typename T> inline ByteSpan SerializeIntoBuffer( T value, ByteSpan& buffer, bool swap_endianess = false )
{
    static_assert( std::is_arithmetic<T>::value, "This function accepts only numeric types");
    const auto S = sizeof(T);
    if( S > buffer.size())
    {
       std::runtime_error("DeserializeFromBuffer: buffer overflow");
    }

    if( swap_endianess )
    {
        *( reinterpret_cast<T*>( buffer.data() )) = EndianSwap<T>(value);
    }
    else{
        *( reinterpret_cast<T*>( buffer.data() )) = value;
    }
    return ByteSpan( buffer.data() + S, buffer.size() - S);
}



#endif // DESERIALIZE_HPP
