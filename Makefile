export CFLAGS := -g -O2 -Wall -msse2 --std gnu99
export INCLUDE = $(PWD)/include
export SOURCE  = $(PWD)/src
export BUILD   = $(PWD)/build

.PHONY: build example clean

SUBDIR  = rlnc systematic

all: build example

build:
	if test ! -d build; then \
		mkdir build; \
	fi;
	$(MAKE) -C $(SOURCE) || exit 1;

example: 
	for n in $(SUBDIR); do $(MAKE) -C example/$$n OBJECT="$(wildcard $(BUILD)/*.o)"|| exit 1; done

clean:
	for n in $(SUBDIR); do $(MAKE) -C example/$$n clean; done
	rm -rf build
