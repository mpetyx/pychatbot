#ifndef __CS_EV_H__
#define __CS_EV_H__

#ifndef DISCARDSERVER
#ifdef EVSERVER
#define EV_STANDALONE 1
#define EV_CHILD_ENABLE 1
#include "ev.h"
#endif
#endif

#endif /* __CS_EV_H__ */
