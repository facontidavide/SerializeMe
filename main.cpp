#include <iostream>
#include <assert.h>
#include <DeserializeMe/deserialize.hpp>


using namespace std;


int main()
{
    uint8_t array[8];
    ByteSpan buffer(array);
    cout << buffer.size() << endl;


    ByteSpan buffer2 = buffer;
    buffer2 = SerializeIntoBuffer( float(33.7), buffer2 );
    buffer2 = SerializeIntoBuffer( int32_t(-42), buffer2 );
    cout << buffer2.size() << endl;

    ByteSpan buffer3 = buffer;
    float num_f = 0;
    int32_t num_i = 0;
    buffer3 = DeserializeFromBuffer( buffer3, num_f );
    buffer3 = DeserializeFromBuffer( buffer3, num_i );
    cout << buffer3.size() << endl;
    cout << num_f << " / " << num_i << endl;

    return 0;
}
