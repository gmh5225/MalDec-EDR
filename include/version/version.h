/**
 * @file version.h
 * @brief Header file for the version MalDec-EDR
 */

#pragma once

#define MALDEC_EDR_VERSION_CODE 65536
#define MALDEC_EDR_VERSION(a, b, c) \
  (((a) << 16) + ((b) << 8) + ((c) > 255 ? 255 : (c)))
#define MALDEC_EDR_VERSION_MAJOR 1
#define MALDEC_EDR_VERSION_PATCHLEVEL 0
#define MALDEC_EDR_VERSION_SUBLEVEL 0