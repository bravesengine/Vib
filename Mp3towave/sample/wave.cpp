/*

 WAV file appender.

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

// $Revision: 3198 $ $Date:: 2016-01-18 #$ $Author: serge $

#include "wave.h"           // self

#include <stdexcept>        // std::runtime_error
#include <fstream>          // std::ofstream
#include <errno.h>          // errno
#include <cstring>          // strerror

using namespace wave;

Wave::Wave( const std::string & filename ) throw( std::exception )
{
    fmt.wFormatTag      = 0;
    extra_param_length_ = 0;
    fact.samplesNumber  = -1;

    std::ifstream file( filename.c_str(), std::ios_base::binary | std::ios_base::in );
    if( file.is_open() == false )
    {
        throw std::runtime_error( strerror( errno ) );
    }

    file.read( reinterpret_cast<char*>( &riff ), RIFF_SIZE );
    file.read( reinterpret_cast<char*>( &fmthdr ), FMTHDR_SIZE );

    file.read( reinterpret_cast<char*>( &fmt ), FMT_SIZE );

    unsigned fmt_extra_bytes    = fmthdr.fmtSIZE - FMT_SIZE;

    if( fmt_extra_bytes > 0 )
    {
        fmt_extra_bytes_.resize( fmt_extra_bytes );

        file.read( & fmt_extra_bytes_[0], fmt_extra_bytes );
    }

    if( fmt.wFormatTag != 1 )
    {
        file.read( reinterpret_cast<char*>( &extra_param_length_ ), 2 ); //2 bytes
        if( extra_param_length_ > 0 )
        {
            extra_param_.resize( extra_param_length_ );
            file.read( & extra_param_[0], extra_param_length_ );
        }
    }

    file.read( reinterpret_cast<char*>( &data.dataID ), 4 );

    if( data.dataID[0] == 'f' && data.dataID[1] == 'a' && data.dataID[2] == 'c' && data.dataID[3] == 't' )
    {
        file.read( reinterpret_cast<char*>( &fact ), FACT_SIZE );
        file.read( reinterpret_cast<char*>( &data ), DATA_SIZE );
    }
    else
        file.read( reinterpret_cast<char*>( &data.dataSIZE ), 4 );

    wave_.resize( data.dataSIZE );

    file.read( & wave_[0], data.dataSIZE );
}

Wave::Wave()
{
    extra_param_length_ = 0;
    fmt.wFormatTag      = 0;
    fact.samplesNumber  = -1;
}

Wave::Wave( int16_t nChannels, int32_t nSamplesPerSec, int16_t wBitsPerSample ) throw( std::exception )
{
    int16_t bytes = ( wBitsPerSample + 7 ) / 8;

    memcpy( riff.riffID, "RIFF", 4 );
    riff.riffSIZE           = 0;

    memcpy( riff.riffFORMAT, "WAVE", 4 );

    memcpy( fmthdr.fmtID, "fmt ", 4 );
    fmthdr.fmtSIZE          = sizeof( FMT );

    fmt.wFormatTag          = 1;
    fmt.nChannels           = nChannels;
    fmt.nSamplesPerSec      = nSamplesPerSec;
    fmt.nAvgBytesPerSec     = nChannels * nSamplesPerSec * bytes;
    fmt.nBlockAlign         = nChannels * bytes;
    fmt.wBitsPerSample      = wBitsPerSample;

    extra_param_length_     = 0;
    fact.samplesNumber      = -1;

    memcpy( data.dataID, "data", 4 );
    data.dataSIZE       = 0;

    update_riff_size();
}

Wave::Wave( const Wave& w )
{
    init( w );
}
Wave& Wave::operator=( const Wave &w )
{
    init( w );
    return *this;
}
Wave::~Wave()
{
}

Wave Wave::operator+( const Wave &w ) const throw( std::exception )
{
    if( fmt.wFormatTag != w.fmt.wFormatTag )
        throw std::runtime_error( "Can't concatenate waves with different format tags" );

    Wave res;
    res.fmthdr  = w.fmthdr;
    res.fmt     = w.fmt;
    res.fmt_extra_bytes_    = w.fmt_extra_bytes_;

    res.riff = w.riff;
    res.data = w.data;
    res.data.dataSIZE = data.dataSIZE + w.data.dataSIZE;

    res.extra_param_length_ = w.extra_param_length_;
    if( w.extra_param_length_ )
    {
        res.extra_param_    = w.extra_param_;
    }

    res.wave_   = wave_;

    res.wave_.insert( res.wave_.end(), w.wave_.begin(), w.wave_.end() );

    res.update_riff_size();

    return res;
}

Wave& Wave::operator+=( const Wave &w ) throw( std::exception )
{
    if( fmt.wFormatTag == 0 )
    {
        init( w );
        return *this;
    }

    if( fmt.wFormatTag != w.fmt.wFormatTag )
        throw std::runtime_error( "Can't concatenate waves with different format tags" );

    if( fmt.nChannels != w.fmt.nChannels )
        throw std::runtime_error( "different number of channels" );

    if( fmt.nSamplesPerSec != w.fmt.nSamplesPerSec )
        throw std::runtime_error( "different number of samples per second" );

    if( fmt.wBitsPerSample != w.fmt.wBitsPerSample )
        throw std::runtime_error( "different number of bits per sample" );

    wave_.insert( wave_.end(), w.wave_.begin(), w.wave_.end() );

    data.dataSIZE   += w.data.dataSIZE;

    update_riff_size();

    return *this;
}

void Wave::init( const Wave& w )
{
    fmthdr = w.fmthdr;
    fmt = w.fmt;
    fmt_extra_bytes_    = w.fmt_extra_bytes_;
    riff = w.riff;
    data = w.data;
    fact = w.fact;

    extra_param_length_ = w.extra_param_length_;
    if( w.extra_param_length_ )
    {
        extra_param_ = w.extra_param_;
    }
    wave_ = w.wave_;
}

int32_t Wave::calc_riff_size( int32_t fmtSIZE, int32_t dataSIZE )
{
    return RIFF_SIZE - 4 + FMTHDR_SIZE + fmtSIZE + DATA_SIZE + dataSIZE;
}

void Wave::update_riff_size()
{
    riff.riffSIZE   = calc_riff_size( fmthdr.fmtSIZE, data.dataSIZE );
}

void Wave::update_data_size()
{
    data.dataSIZE   = wave_.size();
}

int16_t Wave::get_channels() const
{
    return fmt.nChannels;
}

int32_t Wave::get_samples_per_sec() const
{
    return fmt.nSamplesPerSec;
}

int32_t Wave::get_avg_bytes_per_sec() const
{
    return fmt.nAvgBytesPerSec;
}

int32_t Wave::get_data_size() const
{
    return data.dataSIZE;
}

void Wave::get_samples( unsigned int offset, unsigned int size, std::vector<char> & samples ) const
{
    if( offset > ( unsigned )data.dataSIZE )
        return;

    unsigned int real_size = ( offset + size ) < ( unsigned )data.dataSIZE ? size : ( unsigned )data.dataSIZE - offset;

    samples.insert( samples.end(), & wave_[offset], & wave_[offset + real_size] );
}

void Wave::append_samples( const std::vector<char> & samples )
{
    wave_.insert( wave_.end(), samples.begin(), samples.end() );

    update_data_size();
    update_riff_size();
}

void Wave::append_samples( const char* samples, int size )
{
    wave_.insert( wave_.end(), samples, samples + size );

    update_data_size();
    update_riff_size();
}

void Wave::append_samples( const std::vector<char> & samples_l, const std::vector<char> & samples_r )
{
    if( fmt.nChannels != 2 )
        throw std::logic_error( (
                "Wave::append_samples(): cannot add stereo samples, nChannels = " +
                std::to_string( fmt.nChannels )).c_str() );

    if( samples_l.size() != samples_r.size() )
        throw std::logic_error( (
                "Wave::append_samples(): samples have different sizes, l " +
                std::to_string( samples_l.size() ) + " r " + std::to_string( samples_r.size() )).c_str() );

    int bytes_per_sample   = fmt.wBitsPerSample / 8;

    size_t size = samples_l.size();

    for( size_t i = 0; i < size; i = i + bytes_per_sample )
    {
        wave_.insert( wave_.end(), samples_l[i], samples_l[i + bytes_per_sample] );
        wave_.insert( wave_.end(), samples_r[i], samples_r[i + bytes_per_sample] );
    }

    update_data_size();
    update_riff_size();
}

void Wave::append_samples( const char* samples_l, const char* samples_r, int size )
{
    if( fmt.nChannels != 2 )
        throw std::logic_error( (
                "Wave::append_samples(): cannot add stereo samples, nChannels = " +
                std::to_string( fmt.nChannels )).c_str() );

    int bytes_per_sample   = fmt.wBitsPerSample / 8;

    for( int i = 0; i < size; i = i + bytes_per_sample )
    {
        wave_.insert( wave_.end(), samples_l[i], samples_l[i + bytes_per_sample] );
        wave_.insert( wave_.end(), samples_r[i], samples_r[i + bytes_per_sample] );
    }

    update_data_size();
    update_riff_size();
}

void Wave::save( const std::string & filename )
{
    std::ofstream file( filename.c_str(), std::ios_base::binary | std::ios_base::out  );

    file.write( reinterpret_cast<char*>( & riff ), RIFF_SIZE );
    file.write( reinterpret_cast<char*>( & fmthdr ), FMTHDR_SIZE );

    file.write( reinterpret_cast<char*>( & fmt ), FMT_SIZE );
    file.write( & fmt_extra_bytes_[0] , fmt_extra_bytes_.size() );

    if( fmt.wFormatTag > 1 )
    {
        file.write( reinterpret_cast<char*>( &extra_param_length_ ), 2 );
        if( extra_param_length_ > 0 )
            file.write( & extra_param_[0], extra_param_length_ );
    }
    if( fact.samplesNumber > -1 )
    {
        file.write( const_cast<char*>( "fact" ), 4 );
        file.write( reinterpret_cast<char*>( & fact ), FACT_SIZE );
    }

    file.write( reinterpret_cast<char*>( & data ), DATA_SIZE );
    file.write( & wave_[0] , data.dataSIZE );
}
