client.so: client.c
	gcc client.c -o client.so -shared -fPIC `libgcrypt-config --libs` `pkg-config --cflags --libs glib-2.0`

