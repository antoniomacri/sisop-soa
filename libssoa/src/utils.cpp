/**
 * utils.cpp
 */

#include <ssoa/utils.h>

namespace ssoa
{
    void setup() {
        static RegistryMessageInstaller rm;
        static ServiceArgumentInstaller sa;
    }
}
