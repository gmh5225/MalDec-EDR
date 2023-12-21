/**
 * @file my_macros.h
 * @brief Custom macros with Doxygen documentation.
 */

#pragma once

/**
 * @defgroup MyMacros Custom Macros
 * @{
 */

/**
 * @brief Attribute to warn if the result of a function is unused.
 *
 * Use this macro to mark a function so that the compiler generates a warning
 * if its result is not used.
 *
 * Example:
 * @code
 *   @warn_unused_result
 *   int add(int a, int b);
 * @endcode
 */
#define warn_unused_result __attribute__((warn_unused_result))

/**
 * @brief Attribute to indicate that a function does not return.
 *
 * Use this macro to mark a function as not returning. This can be useful to
 * suppress compiler warnings when a function is known not to return.
 *
 * Example:
 * @code
 *   @no_return
 *   void exit_program();
 * @endcode
 */
#define no_return __attribute__((__noreturn__))

/**
 * @brief Attribute to specify packing and alignment for a structure.
 *
 * Use this macro to specify the packing and alignment of a structure. The
 * parameter 'x' should be the desired alignment value.
 *
 * Example:
 * @code
 *   struct packed(4) MyPackedStruct {
 *       int a;
 *       char b;
 *   };
 * @endcode
 */
#define packed(x) __attribute__((packed, aligned(x)))