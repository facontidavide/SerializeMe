# SerializeMe

C++ utility to serialize/deserialize data from a buffer.

This is not a full serialization library like ProtoBuffers or Flatbuffers, it is just **the most primitive
building block** that takes care of memcpy and endianess for you.

It works only with numerical types, `std::string` and `std::vector<T>` where T is a numerical type. You should serialize complex types by hand.

This is made available only for didactic purposes.

Typical usage:

```c++
// Given these structures

struct Point3D
{
  double x;
  double y;
  double z;
}

struct MyData{
  Point3D point;
  std::string name;
  std::vector<int> data;
};

// You must define these template specializations
namespace SerializeMe
{
template <> struct Serializer<Point3D>
{
  template <class Operator> void operator()(Point3D& obj, Operator& op)
  {
    op(obj.x);
    op(obj.y);
    op(obj.z);
  };
};

template <> struct Serializer<MyData>
{
  template <class Operator> void operator()(MyData& obj, Operator& op)
  {
    op(obj.point);
    op(obj.name);
    op(obj.data);
  };
};
} // end namespace SerializeMe

//-------- Usage: ----------------

using namespace SerializeMe;
MyData data_in;

// Blob where we want to serialize MyData. Buffer MUST be resized manually.
std::vector<uint8_t> buffer;
buffer.resize( BufferSize(data_in) );

//--- Serialize ---
SpanBytes write_view(buffer);
SerializeIntoBuffer( write_view, data_in );

//--- Deserialize ---
MyData data_out;
SpanBytesConst read_view(buffer);
DeserializeFromBuffer( read_view, data_out );
```

