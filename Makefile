all: test build docs games

test:
	cd games && make test
	cd src && make test

build:
	cd src && make 

docs:
	echo "Future enhancement"

games:
	cd games && make
