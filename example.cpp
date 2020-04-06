#include <iostream>
#include <assert.h>
#include <vector>
#include <string.h>
#include <SerializeMe/deserialize.hpp>

struct Image
{
    std::string name;
    int32_t width;
    int32_t height;
    std::vector<uint8_t> data;
};


template <> size_t BufferSize( const Image& image )
{
    return BufferSize(image.width) +
           BufferSize(image.height) +
           BufferSize(image.name) +
           BufferSize(image.data);
}

template <> ByteSpan SerializeIntoBuffer<Image>( ByteSpan& buffer, const Image& image )
{
    buffer = SerializeIntoBuffer( buffer, image.width );
    buffer = SerializeIntoBuffer( buffer, image.height );
    buffer = SerializeIntoBuffer( buffer, image.data );
    buffer = SerializeIntoBuffer( buffer, image.name );
    return buffer;
}

template <> ByteSpan DeserializeFromBuffer<Image>( const ByteSpan& buffer, Image& image )
{
    ByteSpan data_pt(buffer);
    data_pt = DeserializeFromBuffer( data_pt, image.width );
    data_pt = DeserializeFromBuffer( data_pt, image.height );
    data_pt = DeserializeFromBuffer( data_pt, image.data );
    data_pt = DeserializeFromBuffer(data_pt, image.name);
    return data_pt;
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
    buffer.resize( BufferSize(image) );

    //------ Serialize into the buffer -----
    ByteSpan data_pt(buffer);
    SerializeIntoBuffer( data_pt, image );

    //------ Deserialize from buffer to image -----
    Image image_out;
    DeserializeFromBuffer( buffer, image_out);

    //------ Check results -------
    std::cout << "Image name: " <<  image_out.name << std::endl;
    std::cout << "Image width: " <<  image_out.width << std::endl;
    std::cout << "Image height: " <<  image_out.height << std::endl;
    std::cout << "Pixels count: " <<  image_out.data.size() << std::endl;

    return 0;
}
