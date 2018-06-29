/*This header is defined to use following functions in RTSP_RELAY*/
#ifndef RTSP_RELAY_H
#define RTSP_RELAY_H
int init_streams(char*,char*,int (*func)());
/*init_stream will read incoming stream and it will create an RTSP output stream and do all the initialisation steps*/
/*1. url_in should be an rtsp url (example: "rtsp://[host]:[port]/application")
  2. url_out should be an rtmp url with proper authentication if authentication is enabeled((example: "rtmp://[host]:[port]/application"))
*/
int start_stream(char *url_out);
/*This function will start relaying input stream to output */
int stop_stream();
/*This function will stop streaming will free all the packets */
#endif /*RTSP_RELAY_H*/
