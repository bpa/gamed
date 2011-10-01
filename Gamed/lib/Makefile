support.so: support.c
	gcc support.c -o support.so -shared -fPIC `libgcrypt-config --libs` `pkg-config --cflags --libs glib-2.0`

