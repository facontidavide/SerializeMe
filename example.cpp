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

int main()
{

    // le's create an empty image
    Image image;
    image.name = "pepito";
    image.width = 640;
    image.height = 480;
    image.data.resize( image.width * image.height, 0 );

    // we need a sufficently large buffer
    std::vector<uint8_t> buffer;
    buffer.resize( BufferSize(image.width) +
                   BufferSize(image.height) +
                   BufferSize(image.name) +
                   BufferSize(image.data) );

    // ByteSpan is a convenient wrapper to manipulate a pointer to buffer.data()
    ByteSpan data_pt(buffer);


    //------ Serialize into the buffer --------------

    data_pt = SerializeIntoBuffer( data_pt, image.width );
    data_pt = SerializeIntoBuffer( data_pt, image.height );
    data_pt = SerializeIntoBuffer( data_pt, image.data );
    data_pt = SerializeIntoBuffer(data_pt, image.name);

    //------ Deserialize from buffer to image --------------

    //reset the Span;
    data_pt = ByteSpan(buffer);

    Image image_out;

    data_pt = DeserializeFromBuffer(data_pt, image_out.width);
    data_pt = DeserializeFromBuffer(data_pt, image_out.height);
    data_pt = DeserializeFromBuffer(data_pt, image_out.data);
    data_pt = DeserializeFromBuffer(data_pt, image_out.name);

    //------ Check results -------
    std::cout << "Image name: " <<  image_out.name << std::endl;
    std::cout << "Image width: " <<  image_out.width << std::endl;
    std::cout << "Image height: " <<  image_out.height << std::endl;
    std::cout << "Pixels count: " <<  image_out.data.size() << std::endl;

    return 0;
}
