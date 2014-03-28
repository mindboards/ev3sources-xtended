/*
 * robotc.h
 *
 *  Created on: Jan 21, 2014
 *      Author: Xander Soldaat <xander@robotc.net>
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
