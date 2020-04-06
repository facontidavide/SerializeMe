# SerializeMe

C++ utility to serialize/deserialize data from a buffer.

This is not a full serialization library like ProtoBuffers or Flatbuffers, it is just **the most primitive
building block** that takes care of memcpy and endianess for you.

It works only with numeric types and `std::string` and `std::vector<T>` with numeric types. You should serialize complex types by hand.

This is made available onlu for didactic purposes.

Typical usage:

```c++
struct MyData{

  int x;
  float y;
  double z;
  std::string name;
  std::vector<int> data;
};

MyData obj;

// Blob where we want to serialize MyData. Must be resized manually.
std::vector<uint8_t> buffer;
buffer.resize( sizeof(obj.x) + sizeof(obj.y) + sizeof(obj.z) );

//--- Serialize ---
ByteSpan buffer_ptr(buffer);

// order is important
buffer_ptr = SerializeIntoBuffer( buffer_ptr, obj.x );
buffer_ptr = SerializeIntoBuffer( buffer_ptr, obj.y );
buffer_ptr = SerializeIntoBuffer( buffer_ptr, obj.z );
buffer_ptr = SerializeIntoBuffer( buffer_ptr, obj.name );
buffer_ptr = SerializeIntoBuffer( buffer_ptr, obj.data );

//--- Deserialize ---
MyData out;
buffer_ptr = ByteSpan(buffer); // reset the span

// order is important
buffer_ptr = DeserializeFromBuffer( buffer_ptr, out.x );
buffer_ptr = DeserializeFromBuffer( buffer_ptr, out.y );
buffer_ptr = DeserializeFromBuffer( buffer_ptr, out.z );
buffer_ptr = DeserializeFromBuffer( buffer_ptr, out.name );
buffer_ptr = DeserializeFromBuffer( buffer_ptr, out.data );
