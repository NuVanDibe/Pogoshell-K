#ifndef POGO_H
#define POGO_H

#include "core.h"
#include "device.h"
#include "font.h"
#include "screen.h"
#include "io.h"
#include "string.h"
#include "max_file_count.h"

#ifdef DEBUG
#define DPRINTF _dprintf
#else
#define DPRINTF (void)
#endif

#endif
