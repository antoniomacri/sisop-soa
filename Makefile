
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
all: library registry storageprovider imagemanipulationprovider client


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
LIBSSOATEST_LIBS := ssoa boost_regex boost_system boost_unit_test_framework yaml-cpp

$(LIBSSOATEST): $(LIBSSOA) $(LIBSSOATEST_OBJECTS)
	$(call LINK,$(LIBSSOATEST_OBJECTS),$(LIBSSOATEST_LIBS))

libssoa-test/obj/%.o: libssoa-test/src/%.cpp
	$(call COMPILE,$(LIBSSOATEST_INCLUDES))

-include $(LIBSSOATEST_DEPS)

TEST += $(LIBSSOATEST)
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
### ssoa-registry-test
###
REGISTRYTEST := $(BIN)/ssoa-registry-test
.PHONY: test-registry
test-registry: $(REGISTRYTEST)

REGISTRYTEST_INCLUDES := ssoa-registry-test/src libssoa/api
REGISTRYTEST_OBJECTS := $(call GETOBJECTS,ssoa-registry-test)
REGISTRYTEST_DEPS := $(REGISTRYTEST_OBJECTS:.o=.d)
REGISTRYTEST_LIBS := ssoa pthread boost_regex boost_system boost_unit_test_framework yaml-cpp

$(REGISTRYTEST): $(LIBSSOA) $(REGISTRY) $(REGISTRYTEST_OBJECTS)
	$(call LINK,$(REGISTRYTEST_OBJECTS),$(REGISTRYTEST_LIBS))

ssoa-registry-test/obj/%.o: ssoa-registry-test/src/%.cpp
	$(call COMPILE,$(REGISTRYTEST_INCLUDES))

-include $(REGISTRYTEST_DEPS)

TEST += $(REGISTRYTEST)
CLEAN += $(REGISTRYTEST_OBJECTS) $(REGISTRYTEST_DEPS) ssoa-registry-test/obj
DISTCLEAN += $(REGISTRYTEST) $(BIN)


###
### ssoa-storageprovider
###
STORAGEPROVIDER := $(BIN)/ssoa-storageprovider
.PHONY: storageprovider
storageprovider: $(STORAGEPROVIDER)

STORAGEPROVIDER_INCLUDES := ssoa-storageprovider/include ssoa-storageprovider/src libssoa/api
STORAGEPROVIDER_OBJECTS := $(call GETOBJECTS,ssoa-storageprovider)
STORAGEPROVIDER_DEPS := $(STORAGEPROVIDER_OBJECTS:.o=.d)
STORAGEPROVIDER_LIBS := ssoa boost_thread pthread boost_regex boost_system boost_program_options boost_filesystem yaml-cpp

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
IMAGEMANIPULATIONPROVIDER_LIBS := ssoa \
	boost_thread boost_regex boost_system boost_filesystem boost_program_options \
	pthread yaml-cpp jpeg X11

$(IMAGEMANIPULATIONPROVIDER): $(LIBSSOA) $(IMAGEMANIPULATIONPROVIDER_OBJECTS)
	$(call LINK,$(IMAGEMANIPULATIONPROVIDER_OBJECTS),$(IMAGEMANIPULATIONPROVIDER_LIBS))

ssoa-imagemanipulationprovider/obj/%.o: ssoa-imagemanipulationprovider/src/%.cpp
	$(call COMPILE,$(IMAGEMANIPULATIONPROVIDER_INCLUDES))

-include $(IMAGEMANIPULATIONPROVIDER_DEPS)

CLEAN += $(IMAGEMANIPULATIONPROVIDER_OBJECTS) $(IMAGEMANIPULATIONPROVIDER_DEPS) ssoa-imagemanipulationprovider/obj
DISTCLEAN += $(IMAGEMANIPULATIONPROVIDER) $(BIN)


###
### ssoa-client
###
CLIENT := $(BIN)/ssoa-client
.PHONY: client
client: $(CLIENT)

CLIENT_INCLUDES := ssoa-client/src libssoa/api ssoa-storageprovider/api ssoa-imagemanipulationprovider/api
CLIENT_OBJECTS := $(call GETOBJECTS,ssoa-client)
CLIENT_DEPS := $(CLIENT_OBJECTS:.o=.d)
CLIENT_LIBS := ssoa pthread boost_regex boost_system boost_filesystem boost_program_options yaml-cpp

$(CLIENT): $(LIBSSOA) $(CLIENT_OBJECTS)
	$(call LINK,$(CLIENT_OBJECTS),$(CLIENT_LIBS))

ssoa-client/obj/%.o: ssoa-client/src/%.cpp
	$(call COMPILE,$(CLIENT_INCLUDES))

-include $(CLIENT_DEPS)

CLEAN += $(CLIENT_OBJECTS) $(CLIENT_DEPS) ssoa-client/obj
DISTCLEAN += $(CLIENT) $(BIN)


###
### test
###
.PHONY: test
test: $(TEST)


###
### testcase
###
TESTCASE := $(BIN)/testcase
.PHONY: testcase
testcase: $(TESTCASE)

TESTCASE_INCLUDES := testcase/src libssoa/api
TESTCASE_OBJECTS := $(call GETOBJECTS,testcase)
TESTCASE_DEPS := $(TESTCASE_OBJECTS:.o=.d)
TESTCASE_LIBS := ssoa

$(TESTCASE): $(REGISTRY) $(IMAGEMANIPULATIONPROVIDER) $(STORAGEPROVIDER) $(CLIENT) $(TESTCASE_OBJECTS)
	$(call LINK,$(TESTCASE_OBJECTS),$(TESTCASE_LIBS))

testcase/obj/%.o: testcase/src/%.cpp
	$(call COMPILE,$(TESTCASE_INCLUDES))

-include $(TESTCASE_DEPS)

CLEAN += $(TESTCASE_OBJECTS) $(TESTCASE_DEPS) testcase/obj
DISTCLEAN += $(TESTCASE) $(BIN)


###
### documentation
###
.PHONY: documentation
documentation:
	doxygen

DISTCLEAN += $(wildcard doc/html/search/*) $(wildcard doc/html/*) doc/html


###
### report
###
.PHONY: report
report:
	cd doc/; pdflatex -shell-escape ssoa-report.tex

CLEAN += $(foreach ext, aux log out synctex.gz toc, doc/ssoa-report.$(ext)) doc/*-help.txt
DISTCLEAN += doc/ssoa-report.pdf


###
### Clean
###
.PHONY: clean distclean
clean:
	-@rm -fd $(CLEAN) || true
distclean: clean
	-@rm -fd $(DISTCLEAN) || true
