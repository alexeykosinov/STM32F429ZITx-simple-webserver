#include "lwip/apps/fs.h"
#include "string.h"
#include "httpserver-netconn.h"
#include "cmsis_os.h"
#include <stdio.h>

#define WEBSERVER_THREAD_PRIO    ( osPriorityAboveNormal )

#define HTTP_STATUS_LINE "HTTP/1.0 200 OK"
#define HTTP_HEADER_FIELDS "Content-Type: text/html; charset=utf-8"
#define HTTP_MESSAGE_BODY ""                                     \
"<html>" "\r\n"                                                  \
"<head>" "\r\n"                                                  \
"<title>STM32F429ZI-NUCLEO</title>" "\r\n" 									     \
"</head>" "\r\n"         																   	     \
"  <body style=\"display:flex;text-align:center\">" "\r\n"       \
"    <div style=\"margin:auto\">" "\r\n"                         \
"      <h1>Hello World</h1>" "\r\n"                              \
"      <p>It works !</p>" "\r\n"                                 \
"    </div>" "\r\n"                                              \
"  </body>" "\r\n"                                               \
"</html>"

#define HTTP_RESPONSE HTTP_STATUS_LINE "\r\n"   \
                      HTTP_HEADER_FIELDS "\r\n" \
                      "\r\n"                    \
                      HTTP_MESSAGE_BODY "\r\n"


static void http_server_serve(struct netconn *conn) 
{
  struct netbuf *inbuf;
  err_t recv_err;
  char* buf;
  u16_t buflen;

  
	// my
	char * buffer = pvPortMalloc(2048);

  /* Read the data from the port, blocking if nothing yet there. We assume the request (the part we care about) is in one netbuf */
  recv_err = netconn_recv(conn, &inbuf);
  
  if (recv_err == ERR_OK){
    if (netconn_err(conn) == ERR_OK){
      netbuf_data(inbuf, (void**)&buf, &buflen);
    
      /* Is this an HTTP GET command? (only check the first 5 chars, since there are other formats for GET, and we're keeping it very simple )*/
      if ((buflen >=5) && (strncmp(buf, "GET /", 5) == 0)){				
				//sprintf(buffer, "HELLO MOTHERFUCKERS!\r\n");
				netconn_write(conn, HTTP_RESPONSE, strlen(HTTP_RESPONSE), NETCONN_COPY);

			
				
				
				
      }      
    }
  }

  netconn_close(conn); /* Close the connection (server closes in HTTP) */
	netbuf_delete(inbuf); /* Delete the buffer (netconn_recv gives us ownership, so we have to make sure to deallocate the buffer) */
	
}


static void http_server_netconn_thread(void *arg){ 
  struct netconn *conn, *newconn;
  err_t err, accept_err;
  conn = netconn_new(NETCONN_TCP); /* Create a new TCP connection handle */
  if (conn!= NULL){
    err = netconn_bind(conn, NULL, 80); /* Bind to port 80 (HTTP) with default IP address */
    if (err == ERR_OK){
      netconn_listen(conn); /* Put the connection into LISTEN state */
      while(1){
        accept_err = netconn_accept(conn, &newconn); /* accept any icoming connection */
        if(accept_err == ERR_OK){
					http_server_serve(newconn); /* serve connection */
					netconn_delete(newconn); /* delete connection */
        }
      }
    }
  }
}


void http_server_netconn_init(){ sys_thread_new("HTTP", http_server_netconn_thread, NULL, DEFAULT_THREAD_STACKSIZE, WEBSERVER_THREAD_PRIO); }
