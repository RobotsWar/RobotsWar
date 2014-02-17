/**
 * * RobotCampus 2013
 * * @author Leph
 * *
 * * Adding base classe for Harware and USB Serial
 * */

#ifndef _WIRISH_SERIAL_H_
#define _WIRISH_SERIAL_H_

#include <wirish/Print.h>
#include <wirish/Read.h>

class Serial : public Print, public Read {
};

#endif
