/**
* RobotCampus 2013
* @author Leph
*
* Adding Read interface to Harware and USB Serial
*/

#ifndef _WIRISH_READ_H_
#define _WIRISH_READ_H_

#include <libmaple/libmaple_types.h>

class Read {
    public:
        virtual uint32 available(void) = 0;
        virtual uint8 read(void) = 0;
};

#endif
