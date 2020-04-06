# SerializeMe

C++ utility to serialize/deserialize data from a buffer.

This is not a full serialization library like ProtoBuffers or Flatbuffers, it is just **the most primitive
building block** that take care of endianess for you.

This is made available onlu for didactic purposes.

Typical usage:

```c++
struct MyData{

  int32_t x;
  float y;
  double z;
};

MyData obj;

// Blob where we want to serialize MyData. Must be resized manually.
std::vector<uint8_t> buffer;
buffer.resize( sizeof(obj.x) + sizeof(obj.y) + sizeof(obj.z) );

//--- Serialize ---
ByteSpan buffer_ptr(buffer);

buffer_ptr = SerializeIntoBuffer( buffer_ptr, obj.x );
buffer_ptr = SerializeIntoBuffer( buffer_ptr, obj.y );
buffer_ptr = SerializeIntoBuffer( buffer_ptr, obj.z );

//--- Deserialize ---
MyData out;
buffer_ptr = ByteSpan(buffer); // reset the span

buffer_ptr = DeserializeFromBuffer( buffer_ptr, out.x );
buffer_ptr = DeserializeFromBuffer( buffer_ptr, out.y );
buffer_ptr = DeserializeFromBuffer( buffer_ptr, out.z );

