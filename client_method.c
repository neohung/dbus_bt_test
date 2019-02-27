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
  //----------------- SEND Signal -------------
  /*
  DBusMessage* msg;
  DBusMessageIter args;
  msg = dbus_message_new_signal(
        "/a/b/Object",  // object name
        "a.b.Type",     // interface name
        "DEF");                // name of signal
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
  */
  //----------------------------------- 
  DBusMessage* msg;
  DBusMessageIter args;
  DBusPendingCall* pending;
  bool reply_stat;
  int reply_data;
  msg = dbus_message_new_method_call("neo.method.server", // target for the method call
         "/neo/neomethod/Object", // object to call on
         "neo.neomethod.Type", // interface to call on
         "neoMethod"); // method name
   if (NULL == msg) { 
      fprintf(stderr, "Message Null\n");
      exit(1);
   }
   dbus_message_iter_init_append(msg, &args);
   char* param = "NEOTEST";
   if (!dbus_message_iter_append_basic(&args, DBUS_TYPE_STRING, &param)) { 
      fprintf(stderr, "Out Of Memory!\n"); 
      exit(1);
   }
   // send message and get a handle for a reply
   if (!dbus_connection_send_with_reply (connection, msg, &pending, -1)) { // -1 is default timeout
      fprintf(stderr, "Out Of Memory!\n"); 
      exit(1);
   }
   if (NULL == pending) { 
      fprintf(stderr, "Pending Call Null\n"); 
      exit(1); 
   }
   dbus_connection_flush(connection);
   // block until we receive a reply
   printf("wait reply\n");
   dbus_pending_call_block(pending);
   printf("reply now\n");
   // get the reply message
   msg = dbus_pending_call_steal_reply(pending);
   if (NULL == msg) {
      fprintf(stderr, "Reply Null\n"); 
      exit(1); 
   }
   // free the pending message handle
   dbus_pending_call_unref(pending);
   // read the parameters
   if (!dbus_message_iter_init(msg, &args))
      fprintf(stderr, "Message has no arguments!\n"); 
   else if (DBUS_TYPE_BOOLEAN != dbus_message_iter_get_arg_type(&args)) 
      fprintf(stderr, "Argument is not boolean!\n"); 
   else
      dbus_message_iter_get_basic(&args, &reply_stat);
   
   //Read next parameter
   if (!dbus_message_iter_next(&args))
      fprintf(stderr, "Message has too few arguments!\n"); 
   else if (DBUS_TYPE_UINT32 != dbus_message_iter_get_arg_type(&args)) 
      fprintf(stderr, "Argument is not int!\n"); 
   else
      dbus_message_iter_get_basic(&args, &reply_data);

   printf("Got Reply: %d, %d\n", reply_stat, reply_data);
   // free reply and close connection
   dbus_message_unref(msg);   
  //---------------------------------- 
  return 0;
}
