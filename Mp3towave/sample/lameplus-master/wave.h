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

#ifndef LIB_WAVE_WAVE_H
#define LIB_WAVE_WAVE_H

#include <string>
#include <vector>       // std::vector
#include <stdint.h>     // int32_t, int16_t
#include <exception>    // std::exception

namespace wave
{

class Wave
{
public:
    Wave();
    Wave( const std::string & filename ) throw( std::exception );
    Wave( const Wave& w );

    Wave( int16_t nChannels, int32_t nSamplesPerSec, int16_t wBitsPerSample ) throw( std::exception );

    virtual ~Wave();

    Wave operator+( const Wave& wave ) const throw( std::exception );
    Wave& operator+=( const Wave& wave ) throw( std::exception );
    Wave& operator=( const Wave &w );

    int16_t get_channels() const;
    int32_t get_samples_per_sec() const;
    int32_t get_avg_bytes_per_sec() const;
    int32_t get_data_size() const;

    void get_samples( unsigned int offset, unsigned int size, std::vector<char> & samples ) const;
    void append_samples( const std::vector<char> & samples );
    void append_samples( const char* samples, int size );
    void append_samples( const std::vector<char> & samples_l, const std::vector<char> & samples_r );
    void append_samples( const char* samples_l, const char* samples_r, int size );

    void save( const std::string & filename );

    struct RIFF
    {
        char riffID[4];     //4
        int32_t riffSIZE;   //4
        char riffFORMAT[4]; //4
    };

    struct FMTHDR
    {
        char fmtID[4];      //4
        int32_t fmtSIZE;    //4
    };

    struct FMT
    {
        int16_t wFormatTag; //2
        int16_t nChannels;  //2
        int32_t nSamplesPerSec;     //4
        int32_t nAvgBytesPerSec;    //4
        int16_t nBlockAlign;        //2
        int16_t wBitsPerSample;     //2
    };

    struct DATA
    {
        char dataID[4];     // 4 bytes
        int32_t dataSIZE;   // 4 bytes
    };
    struct FACT
    {
        int32_t samplesNumber;
        int32_t t;
    };

public:
    static const int RIFF_SIZE      = 12;
    static const int FMTHDR_SIZE    = 8;
    static const int FMT_SIZE       = 16;
    static const int DATA_SIZE      = 8;
    static const int FACT_SIZE      = 8;

private:

    void init( const Wave& );

    static int32_t calc_riff_size( int32_t fmtSIZE, int32_t dataSIZE );
    void update_riff_size();
    void update_data_size();


private:
    std::vector<char>   wave_;

    RIFF        riff;
    FMTHDR      fmthdr;
    FMT         fmt;
    std::vector<char>   fmt_extra_bytes_;
    FACT        fact;
    DATA        data;
    int16_t     extra_param_length_;
    std::vector<char>   extra_param_;

};

} // namespace wave

#endif // LIB_WAVE_WAVE_H
