#include "Peg.h"
#include <mfobjects.h>

//#pragma warning(disable:4996)
 /*namespace {*/
	 void CPeg::saveBMP(struct SwsContext *img_convert_ctx, AVFrame *frame, char *filename)
	 {
		 int w = frame->width;
		 int h = frame->height;

		 int numBytes = avpicture_get_size(AV_PIX_FMT_RGB24, w, h);
		 uint8_t *buffer = (uint8_t*)av_malloc(numBytes * sizeof(uint8_t));
		 if (buffer==nullptr){
			 return;
		 }

		 AVFrame* pFrameRGB = av_frame_alloc();
		 avpicture_fill((AVPicture*)pFrameRGB, buffer, AV_PIX_FMT_RGB24, w, h);

		 sws_scale(img_convert_ctx, frame->data, frame->linesize,
			 0, h, pFrameRGB->data, pFrameRGB->linesize);

		 {
			 if (frame->width != 0 && frame->height != 0){
				 std::unique_lock<std::mutex> lock(m_DataMtx);
				 if (m_data == nullptr) {
					 m_data = new unsigned char[numBytes * sizeof(uint8_t)];
				 }
				 memcpy(m_data, pFrameRGB->data[0], numBytes * sizeof(uint8_t));
			 }
		 }
		 if (0){
			 //2 构造 BITMAPINFOHEADER
			 BITMAPINFOHEADER header;
			 header.biSize = sizeof(BITMAPINFOHEADER);

			 header.biWidth = w;
			 header.biHeight = h * (-1);
			 header.biBitCount = 24;
			 header.biCompression = 0;
			 header.biSizeImage = 0;
			 header.biClrImportant = 0;
			 header.biClrUsed = 0;
			 header.biXPelsPerMeter = 0;
			 header.biYPelsPerMeter = 0;
			 header.biPlanes = 1;

			 //3 构造文件头
			 BITMAPFILEHEADER bmpFileHeader = { 0, };
			 //HANDLE hFile = NULL;
			 DWORD dwTotalWriten = 0;
			 DWORD dwWriten;

			 bmpFileHeader.bfType = 0x4d42; //'BM';
			 bmpFileHeader.bfSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + numBytes;
			 bmpFileHeader.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

			 FILE* pf = fopen(filename, "wb");
			 fwrite(&bmpFileHeader, sizeof(BITMAPFILEHEADER), 1, pf);
			 fwrite(&header, sizeof(BITMAPINFOHEADER), 1, pf);
			 fwrite(pFrameRGB->data[0], 1, numBytes, pf);
			 fclose(pf);
		 }
		 //释放资源
		 //av_free(buffer);
		 av_freep(&pFrameRGB[0]);
		 av_free(pFrameRGB);
	 }

	 int CPeg::decode_write_frame(const char* outfilename, AVCodecContext* avctx, struct SwsContext* image_convert_ctx,
		 AVFrame* frame, int* frame_count, AVPacket* pkt, int last)
	 {
		 int len, got_frame;
		 char buf[1024];

		 len = avcodec_decode_video2(avctx, frame, &got_frame, pkt);
		 if (len < 0){
			 return -1;
		 }
		 
		 if (got_frame){
			 snprintf(buf, sizeof(buf), "%s-%d.bmp", outfilename, *frame_count);
			 saveBMP(image_convert_ctx, frame, buf);
			 //(*frame_count)++;
		 }
		 return 0;
	 }
/* }*/

CPeg& CPeg::GetInstacen()
{
	static CPeg s_peg;
	return s_peg;
}
int64_t lastReadPacktTime = 0;
static int interrupt_cb(void *ctx)
{
	int timeout = 30;
	if (av_gettime() - lastReadPacktTime > timeout*1000*1000){
		return -1;
	}
	return 0;
}

void CPeg::Init()
{
	av_register_all();
	avcodec_register_all();
	//avfilter_register_all();
	avformat_network_init();
	avdevice_register_all();
	avcodec_register_all();
	
	av_log_set_level(AV_LOG_ERROR);
}

int CPeg::OpenInput(std::string url)
{
	
	AVDictionary *format_opts = nullptr;
	//av_dict_set_int(&format_opts, "rtbufsize", 3041280 * 10, 0);
 	av_dict_set(&format_opts, "avioflags", "direct", 0);
 	av_dict_set(&format_opts, "video_size", "960x720", 0);
 	av_dict_set(&format_opts, "framerate", "30", 0);
 	av_dict_set(&format_opts, "vcodec", "mjpeg", 0);
	AVInputFormat *inputFormat = av_find_input_format("dshow");
	std::string urlex = "video=" + url;

	inputContext = avformat_alloc_context();
	lastReadPacktTime = av_gettime();
	inputContext->interrupt_callback.callback = interrupt_cb;
	int ret = avformat_open_input(&inputContext, urlex.c_str(), inputFormat, &format_opts);
	if (ret < 0){
		av_log(NULL, AV_LOG_ERROR, "Input file open input failed\n");
		return ret;
	}
	ret = avformat_find_stream_info(inputContext, nullptr);
	if (ret < 0){
		av_log(NULL, AV_LOG_ERROR, "Find input file stream inform failed\n");
	}
	else {
		av_log(NULL, AV_LOG_FATAL, "Open input file  %s success\n", url.c_str());
	}
	return 0;
}

int CPeg::OpenOutput(std::string url)
{
	int ret = avformat_alloc_output_context2(&outputContext, nullptr, "mpegts", url.c_str());
	if (ret < 0){
		av_log(NULL, AV_LOG_ERROR, "open output context failed\n");
		goto ERROR1;
	}
	ret = avio_open2(&outputContext->pb, url.c_str(), AVIO_FLAG_WRITE, nullptr, nullptr);
	if (ret  < 0 ){
		av_log(NULL, AV_LOG_ERROR, "open avio failed");
		goto ERROR1;
	}
	for (int i = 0; i < inputContext->nb_streams; i++) {
		AVStream* stream = avformat_new_stream(outputContext, inputContext->streams[i]->codec->codec);
		ret = avcodec_copy_context(stream->codec, inputContext->streams[i]->codec);
		if (ret < 0){
			av_log(NULL, AV_LOG_ERROR, "copy coddec context failed");
			goto ERROR1;
		}
	}
	ret = avformat_write_header(outputContext, nullptr);
	if (ret < 0){
		av_log(NULL, AV_LOG_ERROR, "format write header failed");
		goto ERROR1;
	}
	av_log(NULL, AV_LOG_FATAL, " Open output file success %s\n", url.c_str());
	return ret;

ERROR1:
	if (outputContext){
		for (int i = 0; i < outputContext->nb_streams; i++) {
			avcodec_close(outputContext->streams[i]->codec);
		}
		avformat_close_input(&outputContext);
	}
	return ret;
}


void CPeg::CloseInput()
{
	if (inputContext != nullptr){
		avformat_close_input(&inputContext);
	}
}

void CPeg::CloseOutput()
{
	if (outputContext != nullptr){
		for (int i = 0; i < outputContext->nb_streams; ++i) {
			AVCodecContext *codecContext = outputContext->streams[i]->codec;
			avcodec_close(codecContext);
		}
		avformat_close_input(&outputContext);
	}
}

std::shared_ptr<AVPacket> CPeg::ReadPacketFromSource()
{
	std::shared_ptr<AVPacket> packet(static_cast<AVPacket*>(av_malloc(sizeof(AVPacket))), [&](AVPacket* p) {av_packet_free(&p); av_free(p); });
	av_init_packet(packet.get());
	lastReadPacktTime = av_gettime();
	int ret = av_read_frame(inputContext, packet.get());
	if (ret >=0 ){
		return packet;
	}
	return nullptr;
}

void av_packet_rescale_ts(AVPacket *pkt, AVRational src_tb, AVRational dst_tb)
{
	if (pkt->pts != AV_NOPTS_VALUE)
		pkt->pts = av_rescale_q(pkt->pts, src_tb, dst_tb);
	if (pkt->dts != AV_NOPTS_VALUE)
		pkt->dts = av_rescale_q(pkt->dts, src_tb, dst_tb);
	if (pkt->duration > 0)
		pkt->duration = av_rescale_q(pkt->duration, src_tb, dst_tb);
}

int CPeg::WritePacket(std::shared_ptr<AVPacket> packet)
{
	auto inputStream = inputContext->streams[packet->stream_index];
	auto outputStream = outputContext->streams[packet->stream_index];
	av_packet_rescale_ts(packet.get(), inputStream->time_base, outputStream->time_base);
	return av_interleaved_write_frame(outputContext, packet.get());
}

void CPeg::ProcessImage()
{
	//av_decode_video
	struct SwsContext* image_convert_ctx;
	std::string outfileName = "D:\\test\\bmp\\testPeg.Bmp";

	int ret = av_find_best_stream(inputContext, AVMEDIA_TYPE_VIDEO, -1, -1, NULL, 0);
	if (ret < 0){
		return;
	}
	int streamIndex = ret;
	AVStream* st = inputContext->streams[streamIndex];

	const AVCodec* codec;
	codec = avcodec_find_decoder(st->codecpar->codec_id);
	if (!codec){
		return;
	}

	//分配输出文件的上下文
	AVCodecContext* c = NULL;
	c = avcodec_alloc_context3(NULL);
	if (!c){
		return;
	}
	if ((ret = avcodec_parameters_to_context(c,st->codecpar)) < 0){
		return;
	}

	//打开解码器
	if (avcodec_open2(c, codec, NULL) < 0){
		return;
	}

	image_convert_ctx = sws_getContext(c->width, c->height, c->pix_fmt,
		c->width, c->height, AV_PIX_FMT_RGB24,
		SWS_BICUBIC, NULL, NULL, NULL);
	if (image_convert_ctx == NULL){
		return;
	}

	AVFrame* frame = av_frame_alloc();
	if (!frame) {
		return;
	}
	int frame_count = 0;
	AVPacket packet;
	av_init_packet(&packet);
	//ReadPacketFromSource();
// 	ret = av_read_frame(inputContext, &packet);
// 	char err[100] = {0};
// 	av_strerror(ret, err, 100);
	while(av_read_frame(inputContext,&packet)>=0)
	{
		if (packet.stream_index == streamIndex){
 			if (decode_write_frame(outfileName.c_str(), c, image_convert_ctx, frame, &frame_count, &packet, 0) < 0){
 				return;
 			}
		}
		av_packet_unref(&packet);
	}
	//packet.data = NULL;
	//packet.size = 0;
	//decode_write_frame(outfileName.c_str(), c, image_convert_ctx, frame, &frame_count, &packet, 1);

	sws_freeContext(image_convert_ctx);
	avcodec_free_context(&c);
	av_frame_free(&frame);
}