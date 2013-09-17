//*****************************************************************************
// HTTP server.
// Adam Dunkels <adam@dunkels.com>
//
// Copyright (c) 2001, Adam Dunkels.
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions
// are met:
// 1. Redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
// 2. Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the
//    documentation and/or other materials provided with the distribution.
// 3. The name of the author may not be used to endorse or promote
//    products derived from this software without specific prior
//    written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS
// OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
// DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
// GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
// WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
// NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// This file is part of the uIP TCP/IP stack.
//
//*****************************************************************************

#include <string.h>
#include <stdio.h>
#include "uip.h"
#include "httpd.h"
//*****************************************************************************
//
// Macro for easy access to buffer data
//
//*****************************************************************************
#define BUF_APPDATA ((u8_t *)uip_appdata)

//*****************************************************************************
//
// Definitions of HTTP Server States
//
//*****************************************************************************
#define HTTP_NOGET      0
#define HTTP_FILE       1
#define HTTP_TEXT       2
#define HTTP_FUNC       3
#define HTTP_END        4

//*****************************************************************************
//
// Global for keeping up with web server state.
//
//*****************************************************************************
struct httpd_state *hs;

extern unsigned long g_temp;

//*****************************************************************************
//
// Default Web Page - allocated in three segments to allow easy update of a
// counter that is incremented each time the page is sent.
//
//*****************************************************************************
static const char page_not_found[] =
"HTTP/1.0 404 OK\r\n"
"Server: UIP/1.0 (http://www.sics.se/~adam/uip/)\r\n\r\n";

//*****************************************************************************
//
// Default Web Page - allocated in three segments to allow easy update of a
// counter that is incremented each time the page is sent.
//
//*****************************************************************************
static const char default_page_header[] =
"HTTP/1.0 200 OK\r\n"
"Server: UIP/1.0 (http://www.sics.se/~adam/uip/)\r\n"
"Content-type: text/html\r\n\r\n"
"<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.01 Transitional//EN" "http://www.w3.org/TR/html4/loose.dtd\">"
"<html>"
  "<head>"
    "<title>Welcome to the myCortex-LM8962 enet_thermometer example web server!</title>"
  "</head>"
  "<body>"
    "This web page is served by a small web server running on top of "
    "the <a href=\"http://www.sics.se/~adam/uip/\">uIP embedded TCP/IP "
    "stack</a>."
    "<hr>"
	"이 웹페이지는 <a href=\"http://withrobot.com\">withrobot Co.</a>에서 제작한  myCortex-LM8962 개발보드에서 서비스되고 있습니다.<br>"
	"This web page is served by myCortex-LM8962 Dev. board which is manufactured by <a href=\"http://withrobot.com\">withrobot Co.</a>."
	"<hr>"
    "현재 온도는 섭씨 ";
static const char default_page_footer[] =
                                   " 도 입니다."
  "</body>"
"</html>";
//static const char default_page_buf_footer[] =

//*****************************************************************************
//
// Initialize the web server.
//
// Starts to listen for incoming connection requests on TCP port 80.
//
//*****************************************************************************
void
httpd_init(void)
{
    //
    // Listen to port 80.
    //
    uip_listen(HTONS(80));
}

//*****************************************************************************
//
// HTTP Application Callback Function
//
//*****************************************************************************
void
httpd_appcall(void)
{
    switch(uip_conn->lport)
    {
        //
        // This is the web server:
        //
        case HTONS(80):
        {
            //
            // Pick out the application state from the uip_conn structure.
            //
            hs = (struct httpd_state *)&(uip_conn->appstate);

            //
            // We use the uip_ test functions to deduce why we were
            // called. If uip_connected() is non-zero, we were called
            // because a remote host has connected to us. If
            // uip_newdata() is non-zero, we were called because the
            // remote host has sent us new data, and if uip_acked() is
            // non-zero, the remote host has acknowledged the data we
            // previously sent to it. */
            if(uip_connected())
            {
                //
                // Since we have just been connected with the remote host, we
                // reset the state for this connection. The ->count variable
                // contains the amount of data that is yet to be sent to the
                // remote host, and the ->state is set to HTTP_NOGET to signal
                // that we haven't received any HTTP GET request for this
                // connection yet.
                //
                hs->state = HTTP_NOGET;
                hs->count = 0;
                return;
            }
            else if(uip_poll())
            {
                //
                // If we are polled ten times, we abort the connection. This is
                // because we don't want connections lingering indefinately in
                // the system.
                //
                if(hs->count++ >= 10)
                {
                    uip_abort();
                }
                return;
            }
            else if(uip_newdata() && hs->state == HTTP_NOGET)
            {
                //
                // This is the first data we receive, and it should contain a
                // GET.
                //
                // Check for GET.
                //
                if(BUF_APPDATA[0] != 'G' ||
                   BUF_APPDATA[1] != 'E' ||
                   BUF_APPDATA[2] != 'T' ||
                   BUF_APPDATA[3] != ' ')
                {
                    //
                    // If it isn't a GET, we abort the connection.
                    //
                    uip_abort();
                    return;
                }

                //
                // Send buffer 1
                //
                uip_send(default_page_header,
                        sizeof(default_page_header) - 1);
            }
            else if(uip_acked())
            {
                hs->count++;
                if(hs->count == 1)
                {
                	char buffer[16];
                	snprintf(buffer, 16, "%d.%d", g_temp/100, g_temp%100);
//                	buffer[0] = '3';
//                	buffer[1] = '0';
//                	buffer[2] = '.';
//                	buffer[3] = '5';
//                	buffer[4] = 0;

                	buffer[15] = 0;
                    uip_send(buffer, strlen(buffer));
                }
                else if(hs->count == 2)
                {
                    uip_send(default_page_footer,
                            sizeof(default_page_footer) - 1);
                }
                else
                {
                    uip_close();
                }
            }
            //
            // Finally, return to uIP. Our outgoing packet will soon be on its
            // way...
            //
            return;
        }

        default:
        {
            //
            // Should never happen.
            //
            uip_abort();
            break;
        }
    }
}
