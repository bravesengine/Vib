#include "wave.h"   // Wave

using namespace wave;

int main( int argc, char* argv[] )
{
    Wave wave1( "a.wav" );
    Wave wave2( "b.wav" );
    Wave wave3 = wave1 + wave2;
    wave3.save( "c.wav" );

    Wave wave4;
    wave4   += wave2;
    wave4.save( "c2.wav" );

    Wave wave5( 1, 44100, 16 );
    wave5.save( "empty.wav" );

    return 0;
}
