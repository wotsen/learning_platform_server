# target marcros
TARGET := libeasylogger.a #libeasylogger.so
MAIN_SRC := 

# compile marcros
DIRS := src plugins port 
OBJS := easylogger_setup.o

# intermedia compile marcros
ALL_OBJS := $(OBJS)
CLEAN_FILES := $(TARGET) $(OBJS)
DIST_CLEAN_FILES := $(OBJS)

# recursive wildcard
rwildcard=$(foreach d,$(wildcard $(addsuffix *,$(1))),$(call rwildcard,$(d)/,$(2))$(filter $(subst *,%,$(2)),$(d)))

# default target
default: show-info all

# non-phony targets
#@echo -e "\t" CC $(ALL_OBJS) $(MAIN_SRC) -o $@ $(CCFLAG)
#@$(CC) $(ALL_OBJS) $(MAIN_SRC) -o $@ $(CCFLAG) 
$(TARGET): build-subdirs $(OBJS) find-all-objs
	@echo -e "\t" CC $@
	@$(AR) $@ $(ALL_OBJS) $(MAIN_SRC)  

# phony targets
.PHONY: all
all: $(TARGET)
	@echo Target $(TARGET) build finished.

.PHONY: clean
clean: clean-subdirs
	@echo CLEAN $(CLEAN_FILES)
	@rm -f $(CLEAN_FILES)
	@rm -rf easylogger/*

.PHONY: distclean
distclean: clean-subdirs
	@echo CLEAN $(DIST_CLEAN_FILES)
	@rm -f $(DIST_CLEAN_FILES)

# phony funcs
.PHONY: find-all-objs
find-all-objs:
	$(eval ALL_OBJS += $(call rwildcard,$(DIRS),*.o))

.PHONY: show-info
show-info:
	@echo Building Project

.PHONY:install
install:
	mkdir easylogger/include -p
	mkdir easylogger/lib -p
	mkdir easylogger/include/inc -p
	mkdir easylogger/include/plugins/file -p
	cp $(TARGET) easylogger/lib -f
	cp easylogger_setup.h easylogger/include -f
	cp inc/*.h easylogger/include/inc -f
	cp plugins/file/*.h easylogger/include/plugins/file -f

.PHONY:update
update:
	mkdir ../../includes/easylogger -p
	mkdir ../../libs -p
	cp easylogger/include/* ../../includes/easylogger -rf
	cp easylogger/lib/$(TARGET) ../../libs -rf

# need to be placed at the end of the file
mkfile_path := $(abspath $(lastword $(MAKEFILE_LIST)))
export PROJECT_PATH := $(patsubst %/,%,$(dir $(mkfile_path)))
export MAKE_INCLUDE=$(PROJECT_PATH)/make.global
export SUB_MAKE_INCLUDE=$(PROJECT_PATH)/submake.global
include $(MAKE_INCLUDE)