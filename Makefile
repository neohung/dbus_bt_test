all:
	gcc -o server server.c `pkg-config --cflags --libs dbus-1`
	gcc -o client client.c `pkg-config --cflags --libs dbus-1`
	gcc -o signla_client client_signal.c `pkg-config --cflags --libs dbus-1`
	gcc -o method_client client_method.c `pkg-config --cflags --libs dbus-1`
	gcc -o bt_client client_bt.c `pkg-config --cflags --libs dbus-1`
clean:
	rm -f server client method_client signla_client bt_client
