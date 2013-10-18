dist/java.jar: src/*/*.java src/*/*/*.java src/*/*/*/*.java
	rm -rf build/classes
	rm -f dist/java.jar
	mkdir build/classes
	javac -source 1.6 -target 1.6 -cp src -d build/classes src/*/*.java src/*/*/*.java src/*/*/*/*.java
	jar cf dist/java.jar -C build/classes .
