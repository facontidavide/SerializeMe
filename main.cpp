#include <iostream>
#include <assert.h>
#include <DeserializeMe/deserialize.hpp>


using namespace std;


int main()
{
    uint8_t array[8];
    ByteSpan buffer(array);
    cout << "buffer size: " << buffer.size() << endl;

    // Serialize into the buffer

    ByteSpan buffer2 = buffer;
    cout << "\nbuffer2 size: " <<  buffer2.size() << endl;

    buffer2 = SerializeIntoBuffer( float(33.7), buffer2 );
    cout << "buffer2 size: " <<  buffer2.size() << endl;

    buffer2 = SerializeIntoBuffer( int32_t(-42), buffer2 );
    cout << "buffer2 size: " <<  buffer2.size() << endl;

    // Deserialize from the buffer

    float num_f = 0;
    int32_t num_i = 0;

    ByteSpan buffer3 = buffer;
    cout << "\nbuffer3 size: " <<  buffer3.size() << endl;

    buffer3 = DeserializeFromBuffer( buffer3, num_f );
    cout << "buffer3 size: " <<  buffer3.size() << endl;

    buffer3 = DeserializeFromBuffer( buffer3, num_i );
    cout << "buffer3 size: " <<  buffer3.size() << endl;

    cout << "\n" << num_f << " / " << num_i << endl;

    return 0;
}
