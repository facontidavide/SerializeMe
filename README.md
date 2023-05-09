# SerializeMe

C++ utility to serialize/deserialize data from a buffer.

This is not a full serialization library like ProtoBuffers or Flatbuffers, it is just **the most primitive
building block** that takes care of memcpy and endianess for you.

It works only with numerical types, `std::string` and `std::vector<T>` where T is a numerical type. You should serialize complex types by hand.

This is made available only for didactic purposes.

Typical usage:

```c++
struct MyData{
  int x;
  float y;
  double z;
  std::string name;
  std::vector<int> data;
};

MyData in;

// Blob where we want to serialize MyData. Must be resized manually.
std::vector<uint8_t> buffer;
buffer.resize( BufferSize(in.x) +
               BufferSize(in.y) +
               BufferSize(in.z) +
               BufferSize(in.name) +
               BufferSize(in.data) );

//--- Serialize ---
ByteSpan write_ptr(buffer);

// order is important
SerializeIntoBuffer( write_ptr, in.x );
SerializeIntoBuffer( write_ptr, in.y );
SerializeIntoBuffer( write_ptr, in.z );
SerializeIntoBuffer( write_ptr, in.name );
SerializeIntoBuffer( write_ptr, in.data );

//--- Deserialize ---
MyData out;
ByteSpan read_ptr(buffer);

// order is important
DeserializeFromBuffer( read_ptr, out.x );
DeserializeFromBuffer( read_ptr, out.y );
DeserializeFromBuffer( read_ptr, out.z );
DeserializeFromBuffer( read_ptr, out.name );
DeserializeFromBuffer( read_ptr, out.data );
```

You may also add types creating manually your own template specialization as shown in **example.cpp**.


