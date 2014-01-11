/**
 * utils.h
 */

#include <ssoa/registry/registrymessage.h>
#include <ssoa/registry/registryerrormessage.h>
#include <ssoa/registry/registryregistrationrequest.h>
#include <ssoa/registry/registryregistrationresponse.h>
#include <ssoa/registry/registryservicerequest.h>
#include <ssoa/registry/registryserviceresponse.h>

#include <ssoa/service/serviceargument.h>

namespace ssoa
{
    /// Helper struct used to install RegistryMessage's.
    struct RegistryMessageInstaller
    {
        RegistryMessageInstaller() {
            RegistryErrorMessage::install();
            RegistryRegistrationRequest::install();
            RegistryRegistrationResponse::install();
            RegistryServiceRequest::install();
            RegistryServiceResponse::install();
        }
    };

    /// Helper struct used to install ServiceArgument.
    struct ServiceArgumentInstaller
    {
        ServiceArgumentInstaller() {
            ServiceIntArgument::install();
            ServiceDoubleArgument::install();
            ServiceStringArgument::install();
            ServiceBufferArgument::install();
        }
    };

    /// Initializes the library.
    extern void setup();
}
