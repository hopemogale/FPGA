// server-GPIO.c - Simple JSON-RPC based GPIO server

#include <stdio.h>
#include <wiringPi.h>
#include <wiringPiSPI.h>
#include "mongoose.h"

static struct mg_serve_http_opts s_http_server_opts;
static char *s_http_port = "8080";

unsigned int GPIO20;

static int rpc_setGPIO(char *buf, int len, struct mg_rpc_request *req)
{
  unsigned int  GPIO18, GPIO19;

   GPIO18 = atoi(req->params[1].ptr);
   GPIO19 = atoi(req->params[2].ptr);
   printf("Set GPIO18:%d, GPIO19:%d\n", GPIO18, GPIO19);

   if (GPIO18==0) digitalWrite(18, LOW);
   else           digitalWrite(18, HIGH);
   if (GPIO19==0) digitalWrite(19, LOW);
   else           digitalWrite(19, HIGH);
   
   return mg_rpc_create_reply(buf, len, req, "i", 1);
}

static int rpc_getGPIO(char *buf, int len, struct mg_rpc_request *req)
{
  unsigned int  GPIO24, GPIO25;

  /* flash and pulse */
  if (GPIO20==0) {digitalWrite(20, LOW);  GPIO20=1;}
  else           {digitalWrite(20, HIGH); GPIO20=0;}
  digitalWrite(21, HIGH);
  digitalWrite(21, LOW);

  /* get */
  GPIO24 = digitalRead(24);
  GPIO25 = digitalRead(25);

  sprintf(buf,
	  "%s\"gpio20\":%d,\"gpio24\":%d,\"gpio25\":%d}",
	  "{\"jsonrpc\":\"2.0\",\"id\":1,",GPIO20,GPIO24,GPIO25);
  
  return 0;
}


static void ev_handler(struct mg_connection *nc, int ev, void *ev_data)
{
   struct http_message *hm = (struct http_message *) ev_data;
   static const char *methods[] = { "setGPIO", "getGPIO", NULL };
   static mg_rpc_handler_t handlers[] = { rpc_setGPIO, rpc_getGPIO, NULL };
   char buf[1000];
   
   buf[0]=0;
   if (ev == MG_EV_HTTP_REQUEST) {
      if (mg_vcmp(&hm->uri, "/json-rpc") == 0) {
         mg_rpc_dispatch(hm->body.p, hm->body.len, buf, sizeof(buf),
                         methods, handlers);

	 printf("%d : %s\n",(int) strlen(buf), buf);


	 mg_printf(nc, "HTTP/1.0 200 OK\r\nContent-Length: %d\r\n"
                   "Content-Type: application/json\r\n\r\n%s",
                   (int) strlen(buf), buf);
         nc->flags |= MG_F_SEND_AND_CLOSE;
      } else {
         // serve static content
         mg_serve_http(nc, hm, s_http_server_opts);
      }
   }
}

int main(int argc, char *argv[])
{
   struct mg_mgr mgr;
   struct mg_connection *nc;
   
   wiringPiSetupGpio();
   pinMode(18, OUTPUT);
   pinMode(19, OUTPUT);
   pinMode(20, OUTPUT);
   pinMode(21, OUTPUT);
   pinMode(24, INPUT);
   pinMode(25, INPUT);

   mg_mgr_init(&mgr, NULL);
   nc = mg_bind(&mgr, s_http_port, ev_handler);
   if (nc == NULL) {
      fprintf(stderr, "Error starting server on port %s\n", s_http_port);
      exit(1);
   }

   GPIO20 = 0;
   
   mg_set_protocol_http_websocket(nc);
   s_http_server_opts.document_root = ".";
   s_http_server_opts.enable_directory_listing = "yes";
   
   printf("Starting server on port %s\n", s_http_port);

   while(1) {
      mg_mgr_poll(&mgr, 1000);
   }
   
   return 0;
}
