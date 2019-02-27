#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
//#include <dbus/dbus-glib.h>
#include <dbus/dbus.h>
#include <unistd.h>
int main(void){
  printf("start\n");
  DBusError err;
  DBusConnection * connection;
  dbus_error_init (&err);
  // connect to the bus and check for errors
  connection = dbus_bus_get(DBUS_BUS_SESSION , &err );
  if(dbus_error_is_set(&err)) {
    fprintf(stderr, "Connection Error (%s)\n", err.message);
    dbus_error_free(&err);
  }
  if(NULL == connection) {
    fprintf(stderr, "Connection Null\n");
    exit(1);
  }
  //output uniquename
  const char *uniquename;
  uniquename = dbus_bus_get_unique_name(connection); 
  printf ("unuqiename ->%s\n",uniquename); 
  //request bus
  int ret;
  ret = dbus_bus_request_name(connection,"neo.method.client", DBUS_NAME_FLAG_REPLACE_EXISTING ,&err);
  if(dbus_error_is_set(&err)) { 
    fprintf(stderr, "Request Name Error (%s)\n", err.message);
    dbus_error_free(&err);
    exit(1);
  }
  if(DBUS_REQUEST_NAME_REPLY_PRIMARY_OWNER != ret) {
    fprintf(stderr, "Not Primary Owner (%d)\n", ret);
    exit(1);
  }
  //----------------- SEND -------------
  DBusMessage* msg;
  DBusMessageIter args;
  msg = dbus_message_new_signal(
        "/neo/neosignal/Object",  // object name
        "neo.neosignal.Type",     // interface name
        "neoSignal");                // name of signal
  if (NULL == msg) {
        printf("Message Null");
        exit(-1);
  }
  dbus_message_iter_init_append(msg, &args);
  dbus_uint32_t u32data = 123;
  if(!dbus_message_iter_append_basic(&args, DBUS_TYPE_UINT32, &u32data)) {
    printf("Out of memory\n");
    exit(-1);
  }
  //send
  dbus_uint32_t serial = 0;
  if(!dbus_connection_send(connection, msg, &serial)) {
    printf("Out of memory");
    exit(-1);
  }
  dbus_connection_flush(connection);
  //free msg
  dbus_message_unref(msg);
  //-----------------------------------
  return 0;
}
