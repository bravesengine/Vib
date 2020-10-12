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

// $Revision: 1631 $ $Date:: 2015-03-24 #$ $Author: serge $

#ifndef LIB_CONVIMP3_CODEC_H
#define LIB_CONVIMP3_CODEC_H

#include "namespace_lib.h"

NAMESPACE_CONVIMP3_START

class Codec
{
public:
    static bool encode( const char *in_file, const char *out_file );
    static bool decode( const char *in_file, const char *out_file );
};

NAMESPACE_CONVIMP3_END

#endif // LIB_CONVIMP3_CODEC_H
