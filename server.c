#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
//#include <dbus/dbus-glib.h>
#include <dbus/dbus.h>
#include <unistd.h>

void neomethod_calling(DBusMessage* msg, DBusConnection* conn)
{
  DBusMessage* reply;
  DBusMessageIter args;
  printf("neomethod_calling\n");
  char* param = "";
  // read the arguments
  if (!dbus_message_iter_init(msg, &args))
      fprintf(stderr, "Message has no arguments!\n"); 
  else if (DBUS_TYPE_STRING != dbus_message_iter_get_arg_type(&args)) 
      fprintf(stderr, "Argument is not string!\n"); 
  else 
      dbus_message_iter_get_basic(&args, &param);
  printf("Method called got param [%s]\n", param);
  // create a reply from the message
  reply = dbus_message_new_method_return(msg);
  // add the arguments to the reply
  dbus_bool_t stat = true;   // <<------ No bool use!!!
  int data = 1234;
  dbus_message_iter_init_append(reply, &args);
  if (!dbus_message_iter_append_basic(&args, DBUS_TYPE_BOOLEAN, &stat)) { 
      fprintf(stderr, "Out Of Memory!\n"); 
      exit(1);
  }
  if (!dbus_message_iter_append_basic(&args, DBUS_TYPE_UINT32, &data)) { 
      fprintf(stderr, "Out Of Memory!\n"); 
      exit(1);
  }
  dbus_uint32_t serial = 0;
  if (!dbus_connection_send(conn, reply, &serial)) { 
      fprintf(stderr, "Out Of Memory!\n"); 
      exit(1);
  }
  dbus_connection_flush(conn);
  // free the reply
  dbus_message_unref(reply);
}
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
  ret = dbus_bus_request_name(connection,"neo.method.server", DBUS_NAME_FLAG_REPLACE_EXISTING ,&err);
  if(dbus_error_is_set(&err)) { 
    fprintf(stderr, "Request Name Error (%s)\n", err.message);
    dbus_error_free(&err);
    exit(1);
  }
  if(DBUS_REQUEST_NAME_REPLY_PRIMARY_OWNER != ret) {
    fprintf(stderr, "Not Primary Owner (%d)\n", ret);
    exit(1);
  }
  //
  dbus_bus_add_match(connection, "type='signal', interface='neo.neosignal.Type'", &err);
  dbus_connection_flush(connection);
  //
  
  DBusMessage* msg;
  // loop, testing for new messages
  while(true) {
    // non blocking read of the next available message
    dbus_connection_read_write(connection, 0);
    msg = dbus_connection_pop_message(connection);
    // loop again if we haven't got a message
    if(NULL == msg) {
      sleep(1);
      continue;
    }

    //
    DBusMessageIter args;
    dbus_uint32_t data = 0;
    if(dbus_message_is_signal(msg, "neo.neosignal.Type", "neoSignal")) {
      if(!dbus_message_iter_init(msg, &args)) {
        printf("dbus_message_iter_init error, msg has no arguments!\n");
      }else if (DBUS_TYPE_UINT32 != dbus_message_iter_get_arg_type(&args)){
         printf("not a uint 32 type !\n");
      }else{
         dbus_message_iter_get_basic(&args, &data);
         printf("Got signal with value %d\n", data);
      }
    }

    if (dbus_message_is_method_call(msg, "neo.neomethod.Type", "neoMethod")){
       //Do what do you want  
       neomethod_calling(msg, connection);
    }
    //
/*
    if(dbus_message_is_method_call(msg, "test.method.Type", "Method")){
    //  reply_to_method_call(msg, connection);}
      printf("test.method.Type Method call\n");
    }
    if(dbus_message_is_method_call(msg, "org.freedesktop.DBus.Introspectable", "Introspect")){
    //reply_to_Introspect(msg, connection);
      printf("org.freedesktop.DBus.Introspectable Introspect\n");
    }
*/
     // free the message
    dbus_message_unref(msg);
  }
  return 0;
}
