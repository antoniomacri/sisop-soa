/*
 * serviceargument.cpp
 */

#include <service/serviceargument.h>

static ssoa::ServiceArgument::installer<ssoa::ServiceIntArgument> installInt;
static ssoa::ServiceArgument::installer<ssoa::ServiceDoubleArgument> installDouble;
static ssoa::ServiceArgument::installer<ssoa::ServiceStringArgument> installString;
static ssoa::ServiceArgument::installer<ssoa::ServiceBufferArgument> installBuffer;
