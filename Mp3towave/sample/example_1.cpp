#include <cstdio>

#include "codec.h"  // convimp3::Codec

int main()
{
    printf( "Hello, world\n" );

    convimp3::Codec::encode( "test.wav", "test.mp3" );

    convimp3::Codec::decode( "test.mp3", "test_decoded.wav" );

    convimp3::Codec::decode( "test2.mp3", "test2_decoded.wav" );

    return 0;
}
