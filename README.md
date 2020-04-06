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
ByteSpan buffer_ptr(buffer);

// order is important
buffer_ptr = SerializeIntoBuffer( buffer_ptr, in.x );
buffer_ptr = SerializeIntoBuffer( buffer_ptr, in.y );
buffer_ptr = SerializeIntoBuffer( buffer_ptr, in.z );
buffer_ptr = SerializeIntoBuffer( buffer_ptr, in.name );
buffer_ptr = SerializeIntoBuffer( buffer_ptr, in.data );

//--- Deserialize ---
MyData out;
buffer_ptr = ByteSpan(buffer); // reset the span

// order is important
buffer_ptr = DeserializeFromBuffer( buffer_ptr, out.x );
buffer_ptr = DeserializeFromBuffer( buffer_ptr, out.y );
buffer_ptr = DeserializeFromBuffer( buffer_ptr, out.z );
buffer_ptr = DeserializeFromBuffer( buffer_ptr, out.name );
buffer_ptr = DeserializeFromBuffer( buffer_ptr, out.data );
```

You may also add types creating manually your own template specialization as shown in **example.cpp**.


