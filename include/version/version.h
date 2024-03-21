/**
 * @file version.h
 * @brief Header file for the version UnixProtection
 */

#pragma once

#define LINUX_DEFENDER_VERSION_CODE 65536
#define LINUX_DEFENDER_VERSION(a, b, c) \
  (((a) << 16) + ((b) << 8) + ((c) > 255 ? 255 : (c)))
#define LINUX_DEFENDER_VERSION_MAJOR 1
#define LINUX_DEFENDER_VERSION_PATCHLEVEL 0
#define LINUX_DEFENDER_VERSION_SUBLEVEL 0