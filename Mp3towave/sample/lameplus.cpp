/*

Very thin C++ wrapper for LAME library.

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

// $Revision: 1634 $ $Date:: 2015-03-25 #$ $Author: serge $

#include "lameplus.h"       // self

#include <cstring>          // memset

NAMESPACE_LAMEPLUS_START

LamePlus::LamePlus()
{
    lame_ = lame_init();
}

LamePlus::~LamePlus()
{
    lame_close( lame_ );
}

int LamePlus::set_in_samplerate( int sample_rate )
{
    return lame_set_in_samplerate( lame_, sample_rate );
}

int LamePlus::set_brate( int byte_rate )
{
    return lame_set_brate( lame_, byte_rate );
}

int LamePlus::set_num_channels( int channels )
{
    return lame_set_num_channels( lame_, channels );
}

int LamePlus::set_mode( MPEG_mode_e mode )
{
    return lame_set_mode( lame_, mode );
}

int LamePlus::set_decode_only( int p )
{
    return lame_set_decode_only( lame_, p );
}

int LamePlus::set_VBR( vbr_mode_e mode )
{
    return lame_set_VBR( lame_, mode );
}

int LamePlus::init_params()
{
    return lame_init_params( lame_ );
}

int LamePlus::encode_flush( unsigned char * mp3buf, int size )
{
    return lame_encode_flush( lame_, mp3buf, size );
}

int LamePlus::encode_buffer(
        const short int     buffer_l [],
        const short int     buffer_r [],
        const int           nsamples,
        unsigned char*      mp3buf,
        const int           mp3buf_size )
{
    return lame_encode_buffer( lame_, buffer_l, buffer_r, nsamples, mp3buf, mp3buf_size );
}

int LamePlus::encode_buffer_interleaved(
        short int           pcm[],
        int                 num_samples,
        unsigned char*      mp3buf,
        int                 mp3buf_size )
{
    return lame_encode_buffer_interleaved( lame_, pcm, num_samples, mp3buf, mp3buf_size );
}

MP3Data::MP3Data()
{
    std::memset( &mp3data_, 0, sizeof( mp3data_ ) );
}

bool MP3Data::is_header_parsed() const
{
    return mp3data_.header_parsed == 1;
}

int MP3Data::get_stereo() const
{
    return mp3data_.stereo;
}

int MP3Data::get_samplerate() const
{
    return mp3data_.samplerate;
}

int MP3Data::get_bitrate() const
{
    return mp3data_.bitrate;
}


Hip::Hip()
{
    hip_    = hip_decode_init();
}

Hip::~Hip()
{
    hip_decode_exit( hip_ );
}

int Hip::decode1(
        unsigned char*  mp3buf
        , size_t          len
        , short           pcm_l[]
        , short           pcm_r[] )
{
    return hip_decode1( hip_, mp3buf, len, pcm_l, pcm_r );
}

int Hip::decode1_headers(
        unsigned char*  mp3buf
        , size_t          len
        , short           pcm_l[]
        , short           pcm_r[]
        , MP3Data       & mp3data )
{
    return hip_decode1_headers( hip_, mp3buf, len, pcm_l, pcm_r, & mp3data.mp3data_ );
}

NAMESPACE_LAMEPLUS_END
