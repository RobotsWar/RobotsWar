# Standard things
sp              := $(sp).x
dirstack_$(sp)  := $(d)
d               := $(dir)
BUILDDIRS       += $(BUILD_PATH)/$(d)

# Safe includes for Wirish.
WIRISH_INCLUDES := -I$(LIB_MAPLE_HOME)/wirish/include -I$(LIB_MAPLE_HOME)/wirish/$(WIRISH_BOARD_PATH)/include

# LibRobotCampus includes
LIBROBOT_INCLUDES := -I$(LIB_ROBOT_HOME)

# Local flags
CFLAGS_$(d) = $(LIBMAPLE_INCLUDES) $(WIRISH_INCLUDES) $(LIBROBOT_INCLUDES) -Wall -Werror

# Local rules and targets
cppSRCS_$(d) := terminal.cpp
cppSRCS_$(d) += servos.cpp
cppSRCS_$(d) += commands.cpp
cppSRCS_$(d) += spline.cpp

cppFILES_$(d) := $(cppSRCS_$(d):%=$(d)/%)

OBJS_$(d) := $(cppFILES_$(d):%.cpp=$(BUILD_PATH)/%.o)
DEPS_$(d) := $(OBJS_$(d):%.o=%.d)

$(OBJS_$(d)): TGT_CFLAGS := $(CFLAGS_$(d))
$(OBJS_$(d)): TGT_ASFLAGS :=

TGT_BIN += $(OBJS_$(d))

# Standard things
-include        $(DEPS_$(d))
d               := $(dirstack_$(sp))
sp              := $(basename $(sp))
