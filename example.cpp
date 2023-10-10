#include <iostream>
#include <assert.h>
#include <list>
#include <vector>
#include <string.h>
#include <SerializeMe/SerializeMe.hpp>

//---------- Custom types --------------

struct Point3D
{
  double x;
  double y;
  double z;
};

struct Quaternion
{
  double x;
  double y;
  double z;
  double w;
};

struct Pose
{
  Point3D pos;
  Quaternion rot;
};

struct Image
{
  // simple types
  int32_t width;
  int32_t height;
  // string
  std::string name;
  // vector of simple types
  std::vector<uint8_t> data;
  // composed custom type
  Pose pose;
  // containers
  std::array<int, 2> array;
  std::list<Quaternion> quats;
  std::vector<Point3D> points;
};

//---------- Specialization of serializer for custom type --------------

namespace SerializeMe
{
template <>
struct Serializer<Point3D>
{
  template <class Operator>
  void operator()(Point3D& obj, Operator& op)
  {
    op(obj.x);
    op(obj.y);
    op(obj.z);
  };
};

template <>
struct Serializer<Quaternion>
{
  template <class Operator>
  void operator()(Quaternion& obj, Operator& op)
  {
    op(obj.x);
    op(obj.y);
    op(obj.z);
    op(obj.w);
  };
};

template <>
struct Serializer<Pose>
{
  template <class Operator>
  void operator()(Pose& obj, Operator& op)
  {
    op(obj.pos);
    op(obj.rot);
  };
};

template <>
struct Serializer<Image>
{
  template <class Operator>
  void operator()(Image& image, Operator& op)
  {
    op(image.width);
    op(image.height);
    op(image.name);
    op(image.data);
    op(image.pose);
    op(image.points);
    op(image.quats);
    op(image.array);
  };
};
};   // namespace SerializeMe

int main()
{
  Image image;

  image.name = "pepito";
  image.width = 640;
  image.height = 480;
  image.data.resize(image.width * image.height, 0);
  image.pose.pos = {1, 2, 3};
  image.pose.rot = {0.1, 0.2, 0.3, 0.4};
  image.array = {42, 69};
  for (int i = 0; i < 4; i++)
  {
    image.points.push_back(Point3D{double(i), double(i + 1), double(i + 2)});
    image.quats.push_back({0.1, -0.2, 0.3, -0.4});
  }

  //--- This is the serialized buffer. Allocate memory manually -----
  std::vector<uint8_t> buffer;
  size_t size = SerializeMe::BufferSize(image);
  buffer.resize(size);

  //------ Serialize into the buffer -----
  SerializeMe::SpanBytes serialize_ptr(buffer);
  SerializeMe::SerializeIntoBuffer(serialize_ptr, image);

  //------ Deserialize from buffer to image -----
  Image image_out;
  SerializeMe::SpanBytesConst deserialize_ptr(buffer.data(), buffer.size());
  SerializeMe::DeserializeFromBuffer(deserialize_ptr, image_out);

  //------ Check results -------
  std::cout << "Image name: " << image_out.name << std::endl;
  std::cout << "Image width: " << image_out.width << std::endl;
  std::cout << "Image height: " << image_out.height << std::endl;
  std::cout << "Pixels count: " << image_out.data.size() << std::endl;

  std::cout << "Pose/pos: " << image_out.pose.pos.x << ", " << image_out.pose.pos.y
            << ", " << image_out.pose.pos.z << std::endl;

  std::cout << "Pose/rot: " << image_out.pose.rot.x << ", " << image_out.pose.rot.y
            << ", " << image_out.pose.rot.z << ", " << image_out.pose.rot.w << std::endl;

  for (int i = 0; i < image_out.points.size(); i++)
  {
    const auto& p = image_out.points[i];
    std::cout << "Point[" << i << "]: " << p.x << ", " << p.y << ", " << p.z << std::endl;
  }

  std::cout << "Buffer size: " << buffer.size() << std::endl;

  return 0;
}
