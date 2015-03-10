# Standard things
sp              := $(sp).x
dirstack_$(sp)  := $(d)
d               := $(dir)
# This is not really clean, because it relies on the ".c=.o" system
RHOCK			:= ../Rhock
BUILDDIRS       += $(BUILD_PATH)/$(d) $(BUILD_PATH)/$(d)/$(RHOCK)/src/rhock/

# Safe includes for Wirish.
WIRISH_INCLUDES := -I$(LIB_MAPLE_HOME)/wirish/include -I$(LIB_MAPLE_HOME)/wirish/$(WIRISH_BOARD_PATH)/include

# LibRobotCampus includes
LIBRHOCK_INCLUDES := -I$(d)/$(RHOCK)/src
GLOBAL_CFLAGS	  += $(LIBRHOCK_INCLUDES) -DRHOCK
GLOBAL_CXXFLAGS	  += $(LIBRHOCK_INCLUDES) -DRHOCK

# Local flags
CFLAGS_$(d) = $(LIBMAPLE_INCLUDES) $(WIRISH_INCLUDES) $(LIBRHOCK_INCLUDES) -Wall -Werror

# Local rules and targets
RHOCK_SRC := $(RHOCK)/src/rhock/

cppSRCS_$(d) := $(RHOCK_SRC)/chain.c \
        $(RHOCK_SRC)/context.c \
        $(RHOCK_SRC)/crc16.c \
        $(RHOCK_SRC)/event.c \
        $(RHOCK_SRC)/memory.c \
        $(RHOCK_SRC)/native.c \
        $(RHOCK_SRC)/obj.c \
        $(RHOCK_SRC)/opcode.c \
        $(RHOCK_SRC)/print.c \
        $(RHOCK_SRC)/program.c \
        $(RHOCK_SRC)/std.c \
        $(RHOCK_SRC)/store.c \
        $(RHOCK_SRC)/stream.c \
        $(RHOCK_SRC)/vm.c \
		commands.cpp hal.cpp print.cpp store.cpp

cppFILES_$(d) := $(cppSRCS_$(d):%=$(d)/%)

OBJS_$(d)_C := $(cppFILES_$(d):%.c=$(BUILD_PATH)/%.o)
OBJS_$(d) := $(OBJS_$(d)_C:%.cpp=$(BUILD_PATH)/%.o)
DEPS_$(d) := $(OBJS_$(d):%.o=%.d)

$(OBJS_$(d)): TGT_CFLAGS := $(CFLAGS_$(d))
$(OBJS_$(d)): TGT_ASFLAGS :=

TGT_BIN += $(OBJS_$(d))

# Standard things
-include        $(DEPS_$(d))
d               := $(dirstack_$(sp))
sp              := $(basename $(sp))
