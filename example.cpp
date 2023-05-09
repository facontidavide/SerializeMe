#include <iostream>
#include <assert.h>
#include <vector>
#include <string.h>
#include <SerializeMe/SerializeMe.hpp>

struct Image
{
    std::string name;
    int32_t width;
    int32_t height;
    std::vector<uint8_t> data;
};

namespace SerializeMe
{
template <> size_t BufferSize( const Image& image )
{
    return BufferSize(image.width) +
           BufferSize(image.height) +
           BufferSize(image.name) +
           BufferSize(image.data);
}

template <> void SerializeIntoBuffer<Image>(ByteSpan& buffer, const Image& image )
{
    SerializeIntoBuffer( buffer, image.width );
    SerializeIntoBuffer( buffer, image.height );
    SerializeIntoBuffer( buffer, image.data );
    SerializeIntoBuffer( buffer, image.name );
}

template <> void DeserializeFromBuffer<Image>(ByteSpan& buffer, Image& image )
{
    DeserializeFromBuffer( buffer, image.width );
    DeserializeFromBuffer( buffer, image.height );
    DeserializeFromBuffer( buffer, image.data );
    DeserializeFromBuffer( buffer, image.name);
}
}

int main()
{
    // le's create an empty image
    Image image;
    image.name = "pepito";
    image.width = 640;
    image.height = 480;
    image.data.resize( image.width * image.height, 0 );

    //--- This is the serialized buffer. Allocate memory manually -----
    std::vector<uint8_t> buffer;
    buffer.resize( SerializeMe::BufferSize(image) );

    //------ Serialize into the buffer -----
    SerializeMe::ByteSpan serialize_ptr(buffer);
    SerializeMe::SerializeIntoBuffer( serialize_ptr, image );

    //------ Deserialize from buffer to image -----
    Image image_out;
    SerializeMe::ByteSpan deserialize_ptr(buffer);
    SerializeMe::DeserializeFromBuffer(deserialize_ptr, image_out);

    //------ Check results -------
    std::cout << "Image name: " <<  image_out.name << std::endl;
    std::cout << "Image width: " <<  image_out.width << std::endl;
    std::cout << "Image height: " <<  image_out.height << std::endl;
    std::cout << "Pixels count: " <<  image_out.data.size() << std::endl;

    return 0;
}
