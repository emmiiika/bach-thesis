PREFIX = ..

include $(PREFIX)/makerc

CXXFLAGS = $(DEFAULT_CXXFLAGS)
DBGFLAGS = $(DEFAULT_DBGFLAGS)
LDFLAGS = $(DEFAULT_LDFLAGS)
LDLIBS = $(DEFAULT_LDLIBS)

COMPILE_DBG = $(CXX) $(CXXFLAGS) $(DBGFLAGS) $(LDFLAGS)

TESTS_TO_RUN := $(patsubst test_%.cpp, %, $(wildcard test_*.cpp))

SUBDIRS = $(wildcard */.)


all: implementation_basic run
	@for d in $(SUBDIRS); do \
		echo "============================== $$d =============================="; \
		$(MAKE) -C "$$d" all; \
	done

implementation_basic:
	cp $(PREFIX)/implementation_basic.h implementation.h
	@for d in $(SUBDIRS); do \
		echo "============================== $$d =============================="; \
		$(MAKE) -C "$$d" implementation_basic; \
	done

run: $(TESTS_TO_RUN)
	@for d in $(SUBDIRS); do \
		echo "============================== $$d =============================="; \
		$(MAKE) -C "$$d" run; \
	done

$(TESTS_TO_RUN): %:
	@echo "------------------------------ $* ------------------------------"
	$(COMPILE_DBG) test_$*.cpp -o $*.out $(LDLIBS)
	./$*.out

clean:
	rm -rf *.out
	$(MAKE) -C object_collection clean
	$(MAKE) -C path_decomposition clean
	$(MAKE) -C isomorphism clean


.PHONY: all clean run implementation_basic
