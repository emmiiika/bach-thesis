SUBDIRS = $(wildcard */.)

all:
	@for d in $(SUBDIRS); do \
		echo "============================== $$d =============================="; \
		$(MAKE) -C "$$d" all; \
	done

clean:
	@for d in $(SUBDIRS); do \
		echo "============================== $$d =============================="; \
		$(MAKE) -C "$$d" clean; \
	done
	rm -rf headers.txt

includes:
	./test_includes.sh
	
list_headers:
	./test_list_headers.sh | grep -v "include/impl" > headers.txt
	cat headers.txt

.PHONY: all clean includes
