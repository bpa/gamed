build:
	@for g in `find . -maxdepth 1 -type d ! -name .`; do \
        make -C $$g; done

clean:
	@for g in `find . -maxdepth 1 -type d ! -name .`; do \
        make -C $$g clean; done
