/*  version.h
 *
 *  Copyright (C) 2012 Willem Kemp <http://www.thenocturnaltree.com>
 *  All rights reserved.
 *
 *  This file is part of PortScope.
 *
 *  PortScope is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  PortScope is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with PortScope. If not, see http://www.gnu.org/licenses/.
 *
 */
#ifndef __VERSION_H__
#define __VERSION_H__

#define STR(x) #x
#define VER_STR(x) STR(x) 


#define PORTSCOPE_VERSION_MAJOR     1
#define PORTSCOPE_VERSION_MINOR     0
#define PORTSCOPE_VERSION_BUILD     0

#define PORTSCOPE_VERSION_STRING    \
    VER_STR(PORTSCOPE_VERSION_MAJOR) "." \
    VER_STR(PORTSCOPE_VERSION_MINOR) "." \
    VER_STR(PORTSCOPE_VERSION_BUILD)

#endif
