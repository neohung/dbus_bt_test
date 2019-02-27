#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
//#include <dbus/dbus-glib.h>
#include <dbus/dbus.h>
#include <unistd.h>

#include <termios.h>
const char* objpath;
void print_iter(DBusMessageIter *piter)
{
   do
   {
     int type = dbus_message_iter_get_arg_type (piter);
     switch (type) {
      case DBUS_TYPE_BOOLEAN:
        {   
          dbus_bool_t val;
          dbus_message_iter_get_basic (piter, &val);
          printf ("boolean %s\n", val ? "true" : "false");
        }   
        break;
      case DBUS_TYPE_BYTE:
        {   
          unsigned char val;
          dbus_message_iter_get_basic (piter, &val);
          printf ("byte %d\n", val);
        }   
        break;
      case DBUS_TYPE_DOUBLE:
          {   
            double val;
            dbus_message_iter_get_basic (piter, &val);
            printf ("double %g\n", val);
            break;
          }   
      case DBUS_TYPE_STRING:
        {   
          char *val;
          dbus_message_iter_get_basic (piter, &val);
          printf ("[%s]\n", val);
        }   
        break;
    case DBUS_TYPE_SIGNATURE:
        {
          char *val;
          dbus_message_iter_get_basic (piter, &val);
          printf ("[%s]\n", val);
        }
        break;
      case DBUS_TYPE_INT16:
        {
          dbus_int16_t val;
          dbus_message_iter_get_basic (piter, &val);
          printf ("int16 %d\n", val);
        }
        break;
      case DBUS_TYPE_UINT16:
        {
          dbus_uint16_t val;
          dbus_message_iter_get_basic (piter, &val);
          printf ("int16 %d\n", val);
        }
        break;
      case DBUS_TYPE_INT32:
        {
          dbus_int32_t val;
          dbus_message_iter_get_basic (piter, &val);
          printf ("int32 %d\n", val);
        }
        break;
    case DBUS_TYPE_UINT32:
        {
          dbus_uint32_t val;
          dbus_message_iter_get_basic (piter, &val);
          printf ("uint32 %u\n", val);
        }
        break;
      case DBUS_TYPE_INT64:
        {
          dbus_int64_t val; val;
          dbus_message_iter_get_basic (piter, &val);
          printf ("int64 %ld\n", val);
        }
        break;
      case DBUS_TYPE_UINT64:
        {
          dbus_uint64_t val; val;
          dbus_message_iter_get_basic (piter, &val);
          printf ("uint64 %lu\n", val);
        }
        break;
      case DBUS_TYPE_OBJECT_PATH:
        {
          char *val;
          dbus_message_iter_get_basic (piter, &val);
          printf ("[%s]\n", val);
        }
        break;
      case DBUS_TYPE_DICT_ENTRY:
      {
        printf("It is %s \n", "DBUS_TYPE_DICT_ENTRY");
        break;
      }
      default:
      break;
      }
   } while (dbus_message_iter_next (piter));
}

DBusConnection* init_bt(void)
{
  printf("init bt\r\n");
  DBusError err;
  DBusConnection * connection;
  dbus_error_init (&err);
  // connect to the bus and check for errors
  connection = dbus_bus_get(DBUS_BUS_SYSTEM , &err );
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
  printf ("unuqiename ->%s\r\n",uniquename);
  //
  dbus_bus_add_match(connection, "type='signal', interface='org.bluez.Adapter'", &err);
  dbus_connection_flush(connection);
  return connection;
}


void bt_on(DBusConnection* connection)
{
    DBusMessage* msg;
    msg = dbus_message_new_method_call("org.bluez", // target service name
         objpath, // object to call on ex: /org/bluez/945/hci0
         "org.bluez.Adapter", // interface to call on
         "StartDiscovery"); // method name
   if (NULL == msg) { 
      fprintf(stderr, "Message Null\n");
      exit(1);
   }
   DBusMessage *message, *reply;
   reply = dbus_connection_send_with_reply_and_block(connection,
                                                        msg, -1, NULL);
   
   //dbus_uint32_t serial;      if (!dbus_connection_send (connection, msg, &serial)) { 
   //   fprintf(stderr, "Out Of Memory!\n");
   //   exit(1);
   //}   
   //dbus_connection_flush(connection);
   dbus_message_unref(msg);
   if (!reply) return;
   dbus_message_unref(reply);

}

void bt_off(DBusConnection* connection)
{
    DBusMessage* msg;
    msg = dbus_message_new_method_call("org.bluez", // target service name
         objpath, // object to call on ex: /org/bluez/945/hci0
         "org.bluez.Adapter", // interface to call on
         "StopDiscovery"); // method name
   if (NULL == msg) { 
      fprintf(stderr, "Message Null\n");
      exit(1);
   }

   //
   DBusMessage *message, *reply;
   reply = dbus_connection_send_with_reply_and_block(connection,
							msg, -1, NULL);
   
   //dbus_uint32_t serial;      if (!dbus_connection_send (connection, msg, &serial)) { 
   //   fprintf(stderr, "Out Of Memory!\n");
   //   exit(1);
   //}   
   //dbus_connection_flush(connection);
   dbus_message_unref(msg);
   if (!reply) return;
   dbus_message_unref(reply);
}


void process_DeviceFound(DBusMessage* msg)
{
  //
  DBusMessageIter iter;
  if (!dbus_message_iter_init(msg, &iter)){
    printf("This Signal has no arguments\r\n");
  }else{
    //STRING First
    if (dbus_message_iter_get_arg_type(&iter) == DBUS_TYPE_STRING){
      char* val;
      dbus_message_iter_get_basic (&iter, &val);
      printf("MAC [%s]\r\n", val);
    }else{
      printf("process_DeviceFound: error Should be String\r\n");
      exit(-1);
    }
    if (dbus_message_iter_next(&iter)){
      //Array
      if (dbus_message_iter_get_arg_type(&iter) == DBUS_TYPE_ARRAY){
        int array_length = 0;
        DBusMessageIter subiter;
        dbus_message_iter_recurse(&iter, &subiter);
        int subtype = dbus_message_iter_get_arg_type(&subiter);
        if (subtype == DBUS_TYPE_DICT_ENTRY){
           while (dbus_message_iter_get_arg_type(&subiter) == DBUS_TYPE_DICT_ENTRY) {
             array_length++;
             DBusMessageIter dict_entry;
             dbus_message_iter_recurse(&subiter, &dict_entry);
             const char *key;
             if (dbus_message_iter_get_arg_type(&dict_entry) == DBUS_TYPE_STRING){
               dbus_message_iter_get_basic(&dict_entry, &key);
               if (!key) {
                 printf("ERROR no key\r\n");
                 dbus_message_iter_next(&subiter);
                 continue;
               }
             }else{
               printf("KEY should be STRING\r\n");
               exit(-1);
             }
             if (!dbus_message_iter_next(&dict_entry)) {
               printf("ERROR no value\r\n");
               dbus_message_iter_next(&subiter);;
               continue;
             }
             if (dbus_message_iter_get_arg_type(&dict_entry) == DBUS_TYPE_VARIANT){
                //
                 DBusMessageIter dict_entry_var_iter;
                 dbus_message_iter_recurse (&dict_entry, &dict_entry_var_iter);
                 int vartype = dbus_message_iter_get_arg_type(&dict_entry_var_iter);
                 switch (vartype) {
                   case DBUS_TYPE_BOOLEAN:
                   {   
                     dbus_bool_t val;
                     dbus_message_iter_get_basic (&dict_entry_var_iter, &val);
                     printf ("%s:%s\r\n", key ,val ? "true" : "false");
                   }   
                   break;
                   case DBUS_TYPE_STRING:
                   {   
                     char *val;
                     dbus_message_iter_get_basic (&dict_entry_var_iter, &val);
                     printf ("%s:[%s]\r\n", key ,val);
                   }   
                   break;
                   case DBUS_TYPE_INT16:
                   { 
                     dbus_int16_t val; 
                     dbus_message_iter_get_basic (&dict_entry_var_iter, &val);
                     printf ("%s:[%d]\r\n", key ,val);
                   }
                   break;
                  case DBUS_TYPE_UINT32:
                   {   
                     dbus_uint32_t val;
                     dbus_message_iter_get_basic (&dict_entry_var_iter, &val);
                     printf ("%s:[%u]\r\n", key ,val);
                   }
                   break;
   
                   default:
                   break;
                 }
                //
             }else{
               printf("VAL should be DBUS_TYPE_VARIANT\r\n");
               exit(-1);
             }
             //dbus_message_iter_get_basic(&dict_entry, &val);
             //printf("%s\r\n", key);
             dbus_message_iter_next(&subiter);
           }
           printf("Has %d DICT\r\n", array_length);
        }else{
          printf("process_DeviceFound: error Should be DICT_ENTRY\r\n");
          exit(-1);
        }
      }else{
        printf("process_DeviceFound: error Should be ARRAY\r\n");
        exit(-1);
      }
    }else{
        printf("process_DeviceFound: error Should has next iter\r\n");
        exit(-1);
    }
  }
 
}

void process_bt_msg(DBusConnection * connection)
{
    DBusMessage* msg;
    dbus_connection_read_write(connection, 0);
    msg = dbus_connection_pop_message(connection);
    // loop again if we haven't got a message
    if(NULL == msg) {
      return;
    }
    if(dbus_message_is_signal(msg, "org.bluez.Adapter", "PropertyChanged")) {
      printf("PropertyChanged\r\n");
      DBusMessageIter iter;
      if (!dbus_message_iter_init(msg, &iter)){
        printf("This Signal has no arguments\r\n");
      }else{
         do{
             int type = dbus_message_iter_get_arg_type(&iter);
             if (type == DBUS_TYPE_INVALID)
	       break;
             switch (type) {
               case DBUS_TYPE_STRING:
               {
                 //printf("string: ");
                 char *val;
                 dbus_message_iter_get_basic (&iter, &val);
                 printf ("Property: %s=", val);
               }
               break;
               case DBUS_TYPE_VARIANT:
               {
                 DBusMessageIter subiter;
                 dbus_message_iter_recurse (&iter, &subiter);
                 int subtype = dbus_message_iter_get_arg_type(&subiter);
                 switch (subtype) {
                   case DBUS_TYPE_BOOLEAN:
                   {   
                     dbus_bool_t val;
                     dbus_message_iter_get_basic (&subiter, &val);
                     printf ("%s\r\n", val ? "true" : "false");
                   }   
                   break;
                   default:
                   break;
                 }
               }
               default:
               break;
             }
         }while(dbus_message_iter_next(&iter));
      }
    }
    if(dbus_message_is_signal(msg, "org.bluez.Adapter", "DeviceFound")) {
      printf("DeviceFound\r\n");
      process_DeviceFound(msg);
     }
    dbus_message_unref(msg);  
}

struct termios orig_termios;
void reset_terminal_mode()
{
  printf("reset_terminal_mode\r\n");
  tcsetattr(STDIN_FILENO, TCSANOW, &orig_termios);
}

int main(int argc, char** argv){
  if (argc < 2){
    printf("Neet to assign obj path first\r\n");
    printf("EX: [/org/bluez/945/hci0]\r\n");
    exit(-1);
  }
  objpath = argv[1];
  printf("Assign objpath:[%s] \r\n", objpath); 
  struct termios new_termios;
  if(tcgetattr(STDIN_FILENO,&orig_termios)==-1){
    printf("Cannot get standard input!\n");
    exit(-1);
  }
  memcpy(&new_termios, &orig_termios, sizeof(new_termios));
  //Change to 1 char mode
  cfmakeraw(&new_termios);
  tcsetattr(STDIN_FILENO, TCSANOW, &new_termios);
  atexit(reset_terminal_mode);
  unsigned char buffer[300];
  //
  DBusConnection* conn = init_bt();
  //
  while(1){
    struct timeval tv = { 0L, 0L };
    int err;
    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(STDIN_FILENO, &fds);
    err = select(STDIN_FILENO+1, &fds, NULL, NULL, &tv);
    if ((err > 0) && (FD_ISSET(STDIN_FILENO, &fds))){
      FD_CLR(STDIN_FILENO, &fds);
      unsigned char c;
      int r;
      // you can just read 1 byte
      r = read(STDIN_FILENO, buffer, sizeof(buffer));
#if 1
      printf("keycode: ");
      int j;
      for(j=r;j>0;j--){
        printf("0x%02X ",buffer[r-j]);
      }
      printf("\r\n");
#endif
      if ((r==1) && (buffer[0]==27)){
        exit(1);
      }
      if (r == 1){
        switch(buffer[0]){
          case 'a':
            printf("bt_on\r\n");
            bt_on(conn);
          break;
          case 'b':
            printf("bt_off\r\n");
            bt_off(conn);
          break;
          default:
          break;
        }
      }
    }
    process_bt_msg(conn);
  }
  return 0;
}
