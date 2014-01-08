
BIN = bin
LIB = lib

CXX = g++
AR = ar
CXXFLAGS += -Wall -Wextra -std=c++11
LDFLAGS += -L$(LIB)

ifdef DEBUG
CXXFLAGS += -g -DLOG_LEVEL=LOG_DEBUG
else
CXXFLAGS += -O3 -DLOG_LEVEL=LOG_INFO
endif


# Takes an argument containing a list of search paths for header files (-I).
define COMPILE
	@echo ''
	@echo 'Invoking GCC C++ Compiler for target "$@"...'
	-@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $(addprefix -I,$1) -MMD -MP -MF"$(@:%.o=%.d)" $< -o $@
endef

# Takes an argument containing the list of object files (.o) to put into the library.
define ARCHIVE
	@echo ''
	@echo 'Invoking GCC Archiver for target "$@"...'
	-@mkdir -p $(@D)
	$(AR) -r $@ $1
endef

# Takes two arguments.
# The first argument contains the list of object files (.o) to link into the executable.
# The second argument contains the list of required libraries (-l).
define LINK
	@echo ''
	@echo 'Invoking GCC C++ Linker for target "$@"...'
	-@mkdir -p $(@D)
	$(CXX) $(LDFLAGS) -o $@ $1 $(addprefix -l,$2)
endef

# Takes an argument containing the folder name of the module.
define GETOBJECTS
	$(eval SRCS:=$(shell find $1/src -name *.cpp -type f -print))$(patsubst $1/src/%,$1/obj/%, $(SRCS:.cpp=.o))
endef


###
### Main target
###
.PHONY: all
all: library registry storageprovider imagemanipulationprovider


###
### libssoa
###
LIBSSOA := $(LIB)/libssoa.a
.PHONY: library
library: $(LIBSSOA)

LIBSSOA_INCLUDES := libssoa/api libssoa/src
LIBSSOA_OBJECTS := $(call GETOBJECTS,libssoa)
LIBSSOA_DEPS := $(LIBSSOA_OBJECTS:.o=.d)

$(LIBSSOA): $(LIBSSOA_OBJECTS)
	$(call ARCHIVE,$(LIBSSOA_OBJECTS))

libssoa/obj/%.o: libssoa/src/%.cpp
	$(call COMPILE,$(LIBSSOA_INCLUDES))

-include $(LIBSSOA_DEPS)

CLEAN += $(LIBSSOA_OBJECTS) $(LIBSSOA_DEPS) $(shell find libssoa/obj/* -type d -print 2>/dev/null) libssoa/obj
DISTCLEAN += $(LIBSSOA) $(LIB)


###
### libssoa-test
###
LIBSSOATEST := $(BIN)/libssoa-test
.PHONY: test-library
test-library: $(LIBSSOATEST)

LIBSSOATEST_INCLUDES := libssoa-test/src libssoa/api
LIBSSOATEST_OBJECTS := $(call GETOBJECTS,libssoa-test)
LIBSSOATEST_DEPS := $(LIBSSOATEST_OBJECTS:.o=.d)
LIBSSOATEST_LIBS := ssoa boost_regex boost_unit_test_framework

$(LIBSSOATEST): $(LIBSSOA) $(LIBSSOATEST_OBJECTS)
	$(call LINK,$(LIBSSOATEST_OBJECTS),$(LIBSSOATEST_LIBS))

libssoa-test/obj/%.o: libssoa-test/src/%.cpp
	$(call COMPILE,$(LIBSSOATEST_INCLUDES))

-include $(LIBSSOATEST_DEPS)

CLEAN += $(LIBSSOATEST_OBJECTS) $(LIBSSOATEST_DEPS) libssoa-test/obj
DISTCLEAN += $(LIBSSOATEST) $(BIN)


###
### ssoa-registry
###
REGISTRY := $(BIN)/ssoa-registry
.PHONY: registry
registry: $(REGISTRY)

REGISTRY_INCLUDES := ssoa-registry/include ssoa-registry/src libssoa/api
REGISTRY_OBJECTS := $(call GETOBJECTS,ssoa-registry)
REGISTRY_DEPS := $(REGISTRY_OBJECTS:.o=.d)
REGISTRY_LIBS := ssoa boost_thread pthread boost_regex boost_system boost_program_options yaml-cpp

$(REGISTRY): $(LIBSSOA) $(REGISTRY_OBJECTS)
	$(call LINK,$(REGISTRY_OBJECTS),$(REGISTRY_LIBS))

ssoa-registry/obj/%.o: ssoa-registry/src/%.cpp
	$(call COMPILE,$(REGISTRY_INCLUDES))

-include $(REGISTRY_DEPS)

CLEAN += $(REGISTRY_OBJECTS) $(REGISTRY_DEPS) ssoa-registry/obj
DISTCLEAN += $(REGISTRY) $(BIN)


###
### ssoa-storageprovider
###
STORAGEPROVIDER := $(BIN)/ssoa-storageprovider
.PHONY: storageprovider
storageprovider: $(STORAGEPROVIDER)

STORAGEPROVIDER_INCLUDES := ssoa-storageprovider/include ssoa-storageprovider/src libssoa/api
STORAGEPROVIDER_OBJECTS := $(call GETOBJECTS,ssoa-storageprovider)
STORAGEPROVIDER_DEPS := $(STORAGEPROVIDER_OBJECTS:.o=.d)
STORAGEPROVIDER_LIBS := ssoa boost_thread pthread boost_regex boost_system boost_program_options yaml-cpp

$(STORAGEPROVIDER): $(LIBSSOA) $(STORAGEPROVIDER_OBJECTS)
	$(call LINK,$(STORAGEPROVIDER_OBJECTS),$(STORAGEPROVIDER_LIBS))

ssoa-storageprovider/obj/%.o: ssoa-storageprovider/src/%.cpp
	$(call COMPILE,$(STORAGEPROVIDER_INCLUDES))

-include $(STORAGEPROVIDER_DEPS)

CLEAN += $(STORAGEPROVIDER_OBJECTS) $(STORAGEPROVIDER_DEPS) ssoa-storageprovider/obj
DISTCLEAN += $(STORAGEPROVIDER) $(BIN)


###
### ssoa-imagemanipulationprovider
###
IMAGEMANIPULATIONPROVIDER := $(BIN)/ssoa-imagemanipulationprovider
.PHONY: imagemanipulationprovider
imagemanipulationprovider: $(IMAGEMANIPULATIONPROVIDER)

IMAGEMANIPULATIONPROVIDER_INCLUDES := ssoa-imagemanipulationprovider/src libssoa/api
IMAGEMANIPULATIONPROVIDER_OBJECTS := $(call GETOBJECTS,ssoa-imagemanipulationprovider)
IMAGEMANIPULATIONPROVIDER_DEPS := $(IMAGEMANIPULATIONPROVIDER_OBJECTS:.o=.d)
IMAGEMANIPULATIONPROVIDER_LIBS := ssoa boost_thread pthread boost_regex boost_system boost_program_options yaml-cpp

$(IMAGEMANIPULATIONPROVIDER): $(LIBSSOA) $(IMAGEMANIPULATIONPROVIDER_OBJECTS)
	$(call LINK,$(IMAGEMANIPULATIONPROVIDER_OBJECTS),$(IMAGEMANIPULATIONPROVIDER_LIBS))

ssoa-imagemanipulationprovider/obj/%.o: ssoa-imagemanipulationprovider/src/%.cpp
	$(call COMPILE,$(IMAGEMANIPULATIONPROVIDER_INCLUDES))

-include $(IMAGEMANIPULATIONPROVIDER_DEPS)

CLEAN += $(IMAGEMANIPULATIONPROVIDER_OBJECTS) $(IMAGEMANIPULATIONPROVIDER_DEPS) ssoa-imagemanipulationprovider/obj
DISTCLEAN += $(IMAGEMANIPULATIONPROVIDER) $(BIN)


###
### documentation
###
.PHONY: documentation
documentation:
	doxygen

DISTCLEAN += $(wildcard doc/html/search/*) $(wildcard doc/html/*) doc/html


###
### Clean
###
.PHONY: clean distclean
clean:
	-@rm -fd $(CLEAN) || true
distclean: clean
	-@rm -fd $(DISTCLEAN) || true
