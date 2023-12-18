#pragma once

#define warn_unused_result __attribute__((warn_unused_result))
#define no_return __attribute__((__noreturn__))
#define packed(x) __attribute__((packed, aligned(x)))