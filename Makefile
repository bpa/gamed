all: build test games docs

build:
	cd src && make 

test:
	cd test && make

games:
	cd games && make

docs:
	echo "Future enhancement"
