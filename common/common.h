#ifndef __COMMON_H
#define __COMMON_H

#include "cmsis.h"

#define INVALID_IRQN	(RESERVED2_IRQn)
#define INVALID_REG		(0)

#define REG_IS_VALID(pReg)	((pReg) != INVALID_REG)


#endif
