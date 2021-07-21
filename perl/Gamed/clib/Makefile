client.so: client.c
	gcc client.c -o client.so -Wall -Wextra -shared -I/usr/local/include -fPIC `libgcrypt-config --libs` `pkg-config --cflags --libs glib-2.0` -levent_core

