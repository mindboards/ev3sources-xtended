/*
 * robotc.h
 *
 *  Created on: Jan 21, 2014
 *      Author: Xander Soldaat <xander@robotc.net>
 * 
 * Copyright (C) 2014 Robomatter/National Instruments
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#ifndef ROBOTC_H_
#define ROBOTC_H_

void vm_init(struct tVirtualMachineInfo *);
void vm_exit();
void vm_update();

void robotc_entry_0();
void robotc_entry_1();
void robotc_entry_2();
void robotc_entry_3();

#endif /* ROBOTC_H_ */
