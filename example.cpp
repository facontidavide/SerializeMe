#include <iostream>
#include <assert.h>
#include <vector>
#include <string.h>
#include <SerializeMe/deserialize.hpp>

struct Image
{
    int32_t width;
    int32_t height;
    std::vector<uint8_t> data;
};

int main()
{

    // le's create an empty image
    Image image;
    image.width = 640;
    image.height = 480;
    image.data.resize( image.width * image.height, 0 );

    // we need a sufficently large buffer
    std::vector<uint8_t> buffer;
    buffer.resize( sizeof(image.width) +
                   sizeof(image.height) +
                   sizeof(int32_t) +
                   image.data.size() );

    // ByteSpan is a convenient wrapper to manipulate a pointer to buffer.data()
    ByteSpan data_pt(buffer);


    //------ Serialize into the buffer --------------

    // serialize width and height
    data_pt = SerializeIntoBuffer( data_pt, image.width );
    data_pt = SerializeIntoBuffer( data_pt, image.height );
    //Before serializing size(), cast it to 4 bytes
    data_pt = SerializeIntoBuffer( data_pt, static_cast<int32_t>(image.data.size()) );
    // to serialize the image, we can just memcpy
    memcpy( data_pt.data(), image.data.data(), image.data.size() );

    //------ Deserialize from buffer to image --------------

    //reset the Span;
    data_pt = ByteSpan(buffer);

    Image image_out;

    data_pt = DeserializeFromBuffer(data_pt, image_out.width);
    data_pt = DeserializeFromBuffer(data_pt, image_out.height);
    int32_t pixel_count;
    data_pt = DeserializeFromBuffer(data_pt, pixel_count);
    image.data.resize( size_t(pixel_count) );
    //memcpy again
    memcpy( image.data.data(), data_pt.data(), pixel_count );

    //------ Check results -------

    std::cout << "Image width: " <<  image_out.width << std::endl;
    std::cout << "Image height: " <<  image_out.height << std::endl;
    std::cout << "Pixels count: " <<  pixel_count << std::endl;

    return 0;
}
