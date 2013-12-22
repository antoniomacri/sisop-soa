
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


###
### Main target
###
.PHONY: all
all: library registry


###
### libssoa
###
LIBSSOA := $(LIB)/libssoa.a
.PHONY: library
library: $(LIBSSOA)

LIBSSOA_INCLUDES := libssoa/api libssoa/src
LIBSSOA_SOURCES := $(wildcard libssoa/src/*.cpp)
LIBSSOA_OBJECTS := $(patsubst libssoa/src/%,libssoa/obj/%, $(LIBSSOA_SOURCES:.cpp=.o))
LIBSSOA_DEPS := $(LIBSSOA_OBJECTS:.o=.d)

$(LIBSSOA): $(LIBSSOA_OBJECTS)
	$(call ARCHIVE,$(LIBSSOA_OBJECTS))

libssoa/obj/%.o: libssoa/src/%.cpp
	$(call COMPILE,$(LIBSSOA_INCLUDES))

-include $(LIBSSOA_DEPS)

CLEAN += $(LIBSSOA_OBJECTS) $(LIBSSOA_DEPS) libssoa/obj
DISTCLEAN += $(LIBSSOA) $(LIB)


###
### libssoa-test
###
LIBSSOATEST := $(BIN)/libssoa-test
.PHONY: test-library
test-library: $(LIBSSOATEST)

LIBSSOATEST_INCLUDES := libssoa-test/src libssoa/api
LIBSSOATEST_SOURCES := $(wildcard libssoa-test/src/*.cpp)
LIBSSOATEST_OBJECTS := $(patsubst libssoa-test/src/%,libssoa-test/obj/%, $(LIBSSOATEST_SOURCES:.cpp=.o))
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
REGISTRY_SOURCES := $(wildcard ssoa-registry/src/*.cpp)
REGISTRY_OBJECTS := $(patsubst ssoa-registry/src/%,ssoa-registry/obj/%, $(REGISTRY_SOURCES:.cpp=.o))
REGISTRY_DEPS := $(REGISTRY_OBJECTS:.o=.d)
REGISTRY_LIBS := ssoa boost_thread pthread boost_regex boost_system yaml-cpp

$(REGISTRY): $(LIBSSOA) $(REGISTRY_OBJECTS)
	$(call LINK,$(REGISTRY_OBJECTS),$(REGISTRY_LIBS))

ssoa-registry/obj/%.o: ssoa-registry/src/%.cpp
	$(call COMPILE,$(REGISTRY_INCLUDES))

-include $(REGISTRY_DEPS)

CLEAN += $(REGISTRY_OBJECTS) $(REGISTRY_DEPS) ssoa-registry/obj
DISTCLEAN += $(REGISTRY) $(BIN)


###
### Clean
###
.PHONY: clean distclean
clean:
	-@rm -fd $(CLEAN) || true
distclean: clean
	-@rm -fd $(DISTCLEAN) || true
