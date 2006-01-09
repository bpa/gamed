build:
	@for g in `find . -depth 1 -type d | grep -v .svn`; do \
        make -C $$g; done

clean:
	@for g in `find . -depth 1 -type d | grep -v .svn`; do \
        make -C $$g clean; done
