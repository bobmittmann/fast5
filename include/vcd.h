/*
 * vcd - VCD encoder libary 
 * 
 * This file is part of libfast5.
 *
 * Ell is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

/*! 
 * \file      vcd.h
 * \brief     VCD library public API
 * \author    Bob Mittmann <bobmittmann@gmail.com>
 * \copyright 2017, Bob Mittmann
 */ 

/* 
   Value change dump (also known less commonly as "Variable Change Dump") is 
   an ASCII-based format for dumpfiles generated by EDA logic simulation tools.
   
   The standard, four-value VCD format was defined along with the Verilog 
   hardware description language by the IEEE Standard 1364-1995 in 1995. 
   An Extended VCD format defined six years later in the IEEE Standard 
   1364-2001 supports the logging of signal strength and directionality. The 
   simple and yet compact structure of the VCD format has allowed its use to 
   become ubiquitous and to spread into non-Verilog tools such as the VHDL 
   simulator GHDL and various kernel tracers. A limitation of the format is 
   that it is unable to record the values in memories.
*/

#ifndef __VCD_H__
#define __VCD_H__

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <limits.h>

/* Opaque VCD file structure */
struct vcd;

#ifdef __cplusplus
extern "C" {
#endif

struct vcd * vcd_create(const char * path, double timescale);

int vcd_close(struct vcd * vcd);

#ifdef __cplusplus
}
#endif

#endif /* __VCD_H__ */

