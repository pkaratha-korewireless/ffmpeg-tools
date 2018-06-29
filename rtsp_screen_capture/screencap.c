#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
int screencap(char *,char*);
void SaveFrame(AVFrame *, int , int , int,char*);
void *cap1(void* arg){
	printf("In thhread1\n");
	screencap(arg,"in");
	return 0;
	}
void *cap2(void* arg){
	screencap(arg,"out");
	printf("In thhread2\n");
	return 0;
	}



int screencap(char *url,char*name) {
	av_register_all();
	avformat_network_init();
	AVFormatContext *pFormatCtx = NULL;

	// Open video file
	if(avformat_open_input(&pFormatCtx, url, 0, NULL)!=0){
  		return -1; // Couldn't open file
	}

	// Retrieve stream information
	if(avformat_find_stream_info(pFormatCtx, NULL)<0){
  		return -1; // Couldn't find stream information
	}
	// Dump information about file onto standard error
	av_dump_format(pFormatCtx, 0, url, 0);

	int i;
	AVCodecContext *pCodecCtxOrig = NULL;
	AVCodecContext *pCodecCtx = NULL;

	// Find the first video stream
	int videoStream=-1;
	for(i=0; i<pFormatCtx->nb_streams; i++){
  		if(pFormatCtx->streams[i]->codec->codec_type==AVMEDIA_TYPE_VIDEO) {
    		videoStream=i;
    		break;
  		}
	}
	if(videoStream==-1){
  		return -1; // Didn't find a video stream
	}

	// Get a pointer to the codec context for the video stream
	pCodecCtx=pFormatCtx->streams[videoStream]->codec;
	AVCodec *pCodec = NULL;

	// Find the decoder for the video stream
	pCodec=avcodec_find_decoder(pCodecCtx->codec_id);
	if(pCodec==NULL) {
  		fprintf(stderr, "Unsupported codec!\n");
  		return -1; // Codec not found
	}
// Copy context
	pCodecCtxOrig = avcodec_alloc_context3(pCodec);
	if(avcodec_copy_context(pCodecCtxOrig,pCodecCtx) != 0) {
  		fprintf(stderr, "Couldn't copy codec context");
  		return -1; // Error copying codec context
	}
// Open codec
	if(avcodec_open2(pCodecCtx, pCodec,NULL)<0){
  		return -1; // Could not open codec
	}


	AVFrame *pFrame = NULL;
	AVFrame *pFrameRGB =NULL;

	// Allocate video frame
	pFrame=av_frame_alloc();


	// Allocate an AVFrame structure
	pFrameRGB=av_frame_alloc();
	if(pFrameRGB==NULL){
 		return -1;
	}

uint8_t *buffer = NULL;
int numBytes;
// Determine required buffer size and allocate buffer
numBytes=avpicture_get_size(PIX_FMT_RGB24, pCodecCtx->width,
                            pCodecCtx->height);
buffer=(uint8_t *)av_malloc(numBytes*sizeof(uint8_t));

// Assign appropriate parts of buffer to image planes in pFrameRGB
// Note that pFrameRGB is an AVFrame, but AVFrame is a superset
// of AVPicture
avpicture_fill((AVPicture *)pFrameRGB, buffer, PIX_FMT_RGB24,
                pCodecCtx->width, pCodecCtx->height);


struct SwsContext *sws_ctx = NULL;
int frameFinished;
AVPacket packet;
// initialize SWS context for software scaling
sws_ctx = sws_getContext(pCodecCtx->width,
    pCodecCtx->height,
    pCodecCtx->pix_fmt,
    pCodecCtx->width,
    pCodecCtx->height,
    PIX_FMT_RGB24,
    SWS_BILINEAR,
    NULL,
    NULL,
    NULL
    );

i=0;
while(av_read_frame(pFormatCtx, &packet)>=0) {
  // Is this a packet from the video stream?
  if(packet.stream_index==videoStream) {
	// Decode video frame
    avcodec_decode_video2(pCodecCtx, pFrame, &frameFinished, &packet);
    
    // Did we get a video frame?
    if(frameFinished) {
    // Convert the image from its native format to RGB
        sws_scale(sws_ctx, (uint8_t const * const *)pFrame->data,
		  pFrame->linesize, 0, pCodecCtx->height,
		  pFrameRGB->data, pFrameRGB->linesize);
	
        // Save the frame to disk
        if(++i<=5){
          SaveFrame(pFrameRGB, pCodecCtx->width, 
                    pCodecCtx->height, i,name);
		}
		//else
			//return 0;
    }
  }
    
  // Free the packet that was allocated by av_read_frame
  av_free_packet(&packet);
}
}
void SaveFrame(AVFrame *pFrame, int width, int height, int iFrame,char*name) {
  FILE *pFile;
  char szFilename[32];
  int  y;
  
  // Open file
  sprintf(szFilename, "%s%d.ppm", name,iFrame);
  pFile=fopen(szFilename, "wb");
  if(pFile==NULL)
    return;
  
  // Write header
  fprintf(pFile, "P6\n%d %d\n255\n", width, height);
  
  // Write pixel data
  for(y=0; y<height; y++)
    fwrite(pFrame->data[0]+y*pFrame->linesize[0], 1, width*3, pFile);
  
  // Close file
  fclose(pFile);
}

int main(int argc,char* argv[]){
	/*pthread_t tid1,tid2;
	pthread_create(&tid1, NULL, cap1, argv[1]);
	pthread_create(&tid2, NULL, cap2, argv[2]);
	pthread_join(tid1,NULL);
	pthread_join(tid2,NULL);*/
	screencap(argv[1],argv[2]);
	
	}
