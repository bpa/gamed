all: test build docs

test:
	cd src && make test

build:
	cd src && make 

docs:
	echo "Future enhancement"
