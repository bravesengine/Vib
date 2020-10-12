/*

 Convenience MP3 library.
 Enables easy PCM to MP3 conversion and vice versa.

 Copyright (C) 2015 Sergey Kolevatov

 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program. If not, see <http://www.gnu.org/licenses/>.

 */

// $Revision: 1655 $ $Date:: 2015-03-25 #$ $Author: serge $

#include "codec.h"          // self

#include <fstream>                  // std::ofstream
#include <stdexcept>                // std::logic_error
#include "../lameplus/lameplus.h"   // LamePlus
#include "../wave/wave.h"           // Wave

NAMESPACE_CONVIMP3_START

bool Codec::encode( const char *in_file, const char *out_file )
{
    wave::Wave pcm( in_file );

    unsigned sample_rate    = pcm.get_samples_per_sec();
    unsigned byte_rate      = pcm.get_avg_bytes_per_sec();
    unsigned channels       = pcm.get_channels();

    std::ofstream mp3( out_file, std::ios_base::binary | std::ios_base::out  );

    const int PCM_SIZE = 8192;
    const int MP3_SIZE = 8192;

    unsigned offset = 0;

    std::vector<char> pcm_buffer;
    pcm_buffer.reserve( sizeof(short int) * PCM_SIZE * 2 );

    unsigned char mp3_buffer[MP3_SIZE];

    lameplus::LamePlus l;

    l.set_in_samplerate( sample_rate );
    l.set_brate( byte_rate );

    if( channels == 1 )
    {
        l.set_num_channels( 1 );
        l.set_mode( MONO );
    }
    else
    {
        l.set_num_channels( channels );
    }

    l.set_VBR( vbr_default );
    l.init_params();

    while( true )
    {
        int k = ( channels == 1 ) ? 1 : 2;

        unsigned size = PCM_SIZE * k * sizeof( short int );

        pcm.get_samples( offset, size, pcm_buffer );

        unsigned read = pcm_buffer.size();

        offset += read;

        if( read > 0 )
        {
            unsigned read_shorts = read / 2;  // need number of 'short int' read

            int write = 0;

            if( channels == 1 )
            {
                write = l.encode_buffer( reinterpret_cast<short int*>( &pcm_buffer[0] ), NULL, read_shorts, mp3_buffer, MP3_SIZE );
            }
            else
            {
                write = l.encode_buffer_interleaved( reinterpret_cast<short int*>( &pcm_buffer[0] ), read_shorts, mp3_buffer, MP3_SIZE );
            }

            pcm_buffer.clear();

            mp3.write( reinterpret_cast<char*>( mp3_buffer ) , write );
        }

        if( read < size )
        {
            int write = l.encode_flush( mp3_buffer, MP3_SIZE );

            mp3.write( reinterpret_cast<char*>( mp3_buffer ), write );

            break;
        }

    };

    return true;
}

bool Codec::decode( const char *in_file, const char *out_file )
{
    std::ifstream mp3( in_file, std::ios_base::binary | std::ios_base::in );

    if( mp3.is_open() == false )
    {
        return false;
    }

    const int PCM_SIZE = 8192;
    const int MP3_SIZE = 8192;

    char pcm_buffer_l[ PCM_SIZE * sizeof(short int) ];
    char pcm_buffer_r[ PCM_SIZE * sizeof(short int) ];
    unsigned char mp3_buffer[MP3_SIZE];

    lameplus::LamePlus l;

    l.set_decode_only( 1 );

    if( l.init_params() == -1 )
        return false;

    lameplus::Hip       hip;

    bool got_header = false;

    wave::Wave res;

    int channels        = 0;

    while( true )
    {
        mp3.read( reinterpret_cast<char*>( mp3_buffer ), sizeof(char) * MP3_SIZE );

        int read = mp3.gcount();

        if( read <= 0 )
            break;

        int num_samples = 0;

        while( true )
        {
            if( got_header )
            {
                num_samples = hip.decode1(
                        mp3_buffer, read,
                        reinterpret_cast<short int*>( & pcm_buffer_l[0] ),
                        reinterpret_cast<short int*>( & pcm_buffer_r[0] ) );
            }
            else
            {
                lameplus::MP3Data   mp3data;

                num_samples = hip.decode1_headers(
                        mp3_buffer, read,
                        reinterpret_cast<short int*>( & pcm_buffer_l[0] ),
                        reinterpret_cast<short int*>( & pcm_buffer_r[0] ), mp3data );

                if( mp3data.is_header_parsed() )
                {
                    got_header = true;

                    channels        = mp3data.get_stereo();
                    int samplerate  = mp3data.get_samplerate();
                    int bitrate     = mp3data.get_bitrate() * 1000;

                    int bits_per_sample = bitrate / ( samplerate * channels );

                    bits_per_sample = 16;

                    res = wave::Wave( channels, samplerate, bits_per_sample );
                }
            }

            if( num_samples == 0 )
            {
                if( read > 0 )  // need more data to continue decoding
                {
                    read = 0;
                    continue;
                }
                break;
            }

            if( num_samples == -1 )
                throw std::logic_error( "Codec::decode(): decoding error" );

            if( got_header == false )
                throw std::logic_error( "Codec::decode(): got samples without header" );

            read    = 0;

            if( channels == 2 )
                res.append_samples( pcm_buffer_l, pcm_buffer_r, num_samples * sizeof( short int ) );
            else
                res.append_samples( pcm_buffer_l, num_samples * sizeof( short int ) );
        }

        if( mp3.fail() )
            break;
    }

    if( got_header == false )
        return false;

    res.save( out_file );

    return true;
}

NAMESPACE_CONVIMP3_END
