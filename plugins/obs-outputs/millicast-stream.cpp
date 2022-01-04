// Copyright Dr. Alex. Gouaillard (2015, 2020)

#include <stdio.h>
#include <obs-module.h>
#include <obs-avc.h>
#include <util/platform.h>
#include <util/dstr.h>
#include <util/threading.h>
#include <inttypes.h>
#include <modules/audio_processing/include/audio_processing.h>

#define warn(format, ...) blog(LOG_WARNING, format, ##__VA_ARGS__)
#define info(format, ...) blog(LOG_INFO, format, ##__VA_ARGS__)
#define debug(format, ...) blog(LOG_DEBUG, format, ##__VA_ARGS__)

#define OPT_DROP_THRESHOLD "drop_threshold_ms"
#define OPT_PFRAME_DROP_THRESHOLD "pframe_drop_threshold_ms"
#define OPT_MAX_SHUTDOWN_TIME_SEC "max_shutdown_time_sec"
#define OPT_BIND_IP "bind_ip"
#define OPT_NEWSOCKETLOOP_ENABLED "new_socket_loop_enabled"
#define OPT_LOWLATENCY_ENABLED "low_latency_mode_enabled"

#include "WebRTCStream.h"
#include "WebRTCSubStream.h"

WebRTCSubStream *mt1_stream = nullptr;
WebRTCSubStream *mt2_stream = nullptr;
bool multi_codec = false;
extern "C" const char *millicast_stream_getname(void *unused)
{
	info("millicast_stream_getname");
	UNUSED_PARAMETER(unused);
	return obs_module_text("MILLICASTStream");
}

extern "C" void millicast_stream_destroy(void *data)
{
	info("millicast_stream_destroy");
	//Get stream
	WebRTCStream *stream = (WebRTCStream *)data;
	//Stop it
	stream->stop();
	//Remove ref and let it self destroy
	stream->Release();
	if(multi_codec){
		mt1_stream->stop();
		mt2_stream->stop();
		mt1_stream->Release();
		mt2_stream->Release();
		mt1_stream = nullptr;
		mt2_stream = nullptr;
		multi_codec = false;
	}
}

extern "C" void *millicast_stream_create(obs_data_t *, obs_output_t *output)
{
	info("WebRTC_stream_create");
    obs_service_t *service = obs_output_get_service(output);
    std::string video_codec = obs_service_get_codec(service) ? obs_service_get_codec(service) : "";
	if (video_codec == "h264") {
		multi_codec = true;
		video_codec = "VP9";
		mt1_stream = new WebRTCSubStream(output, "h264");
		mt2_stream = new WebRTCSubStream(output, "AV1");
	}
	// Create new stream
	WebRTCStream *stream = new WebRTCStream(output);
	// Don't allow it to be deleted
	stream->AddRef();
	// info("millicast_setCodec: h264");
	// stream->setCodec("h264");
	// Return it
	return (void *)stream;
}

extern "C" void millicast_stream_stop(void *data, uint64_t ts)
{
	info("millicast_stream_stop");
	UNUSED_PARAMETER(ts);
	// Get stream
	WebRTCStream *stream = (WebRTCStream *)data;
	// Stop it
	stream->stop();
	// Remove ref and let it self destroy
	stream->Release();
	if(multi_codec){
		mt1_stream->stop();
		mt2_stream->stop();
		mt1_stream->Release();
		mt2_stream->Release();
		mt1_stream = nullptr;
		mt2_stream = nullptr;
		multi_codec = false;
	}
}

extern "C" bool millicast_stream_start(void *data)
{
	info("Webrtc_stream_start");
	//Get stream
	WebRTCStream *stream = (WebRTCStream *)data;
	//Don't allow it to be deleted
	stream->AddRef();
	//Start it
	if(multi_codec){
		mt1_stream->start(WebRTCStream::Type::CustomWebrtc);
		mt2_stream->start(WebRTCStream::Type::CustomWebrtc);
	}
	return stream->start(WebRTCStream::Type::Millicast);
}

extern "C" void millicast_receive_video(void *data, struct video_data *frame)
{
	//Get stream
	WebRTCStream *stream = (WebRTCStream *)data;
	//Process audio
	if(multi_codec){
		mt1_stream->onVideoFrame(frame);
		mt2_stream->onVideoFrame(frame);
	}
	stream->onVideoFrame(frame);
}
extern "C" void millicast_receive_audio(void *data, struct audio_data *frame)
{
	//Get stream
	WebRTCStream *stream = (WebRTCStream *)data;
	//Process audio
	// if(multi_codec){
	// 	mt1_stream->onAudioFrame(frame);
	// 	mt2_stream->onAudioFrame(frame);
	// }
	stream->onAudioFrame(frame);
}

extern "C" void millicast_stream_defaults(obs_data_t *defaults)
{
	info("millicast_stream_defaults");
	obs_data_set_default_int(defaults, OPT_DROP_THRESHOLD, 700);
	obs_data_set_default_int(defaults, OPT_PFRAME_DROP_THRESHOLD, 900);
	obs_data_set_default_int(defaults, OPT_MAX_SHUTDOWN_TIME_SEC, 30);
	obs_data_set_default_string(defaults, OPT_BIND_IP, "default");
	obs_data_set_default_bool(defaults, OPT_NEWSOCKETLOOP_ENABLED, false);
	obs_data_set_default_bool(defaults, OPT_LOWLATENCY_ENABLED, false);
}

extern "C" obs_properties_t *millicast_stream_properties(void *unused)
{
	info("millicast_stream_properties");
	UNUSED_PARAMETER(unused);

	obs_properties_t *props = obs_properties_create();

	obs_properties_add_int(props, OPT_DROP_THRESHOLD,
			       obs_module_text("MILLICASTStream.DropThreshold"),
			       200, 10000, 100);

	obs_properties_add_bool(
		props, OPT_NEWSOCKETLOOP_ENABLED,
		obs_module_text("MILLICASTStream.NewSocketLoop"));
	obs_properties_add_bool(
		props, OPT_LOWLATENCY_ENABLED,
		obs_module_text("MILLICASTStream.LowLatencyMode"));

	return props;
}

// NOTE LUDO: #80 add getStats
extern "C" void millicast_stream_get_stats(void *data)
{
	// Get stream
	WebRTCStream *stream = (WebRTCStream *)data;
	stream->getStats();
}

extern "C" const char *millicast_stream_get_stats_list(void *data)
{
	// Get stream
	WebRTCStream *stream = (WebRTCStream *)data;
	return stream->get_stats_list();
}

extern "C" uint64_t millicast_stream_total_bytes_sent(void *data)
{
	//Get stream
	WebRTCStream *stream = (WebRTCStream *)data;
	return stream->getBitrate();
}

extern "C" int millicast_stream_dropped_frames(void *data)
{
	//Get stream
	WebRTCStream *stream = (WebRTCStream *)data;
	return stream->getDroppedFrames();
}

// #310 webrtc getstats()
extern "C" uint64_t millicast_stream_get_transport_bytes_sent(void *data)
{
	//Get stream
	WebRTCStream *stream = (WebRTCStream *)data;
	return stream->getTransportBytesSent();
}

extern "C" uint64_t millicast_stream_get_transport_bytes_received(void *data)
{
	//Get stream
	WebRTCStream *stream = (WebRTCStream *)data;
	return stream->getTransportBytesReceived();
}

extern "C" uint64_t millicast_stream_get_video_packets_sent(void *data)
{
	//Get stream
	WebRTCStream *stream = (WebRTCStream *)data;
	return stream->getVideoPacketsSent();
}

extern "C" uint64_t millicast_stream_get_video_bytes_sent(void *data)
{
	//Get stream
	WebRTCStream *stream = (WebRTCStream *)data;
	return stream->getVideoBytesSent();
}

extern "C" uint64_t millicast_stream_get_video_fir_count(void *data)
{
	//Get stream
	WebRTCStream *stream = (WebRTCStream *)data;
	return stream->getVideoFirCount();
}

extern "C" uint32_t millicast_stream_get_video_pli_count(void *data)
{
	//Get stream
	WebRTCStream *stream = (WebRTCStream *)data;
	return stream->getVideoPliCount();
}

extern "C" uint64_t millicast_stream_get_video_nack_count(void *data)
{
	//Get stream
	WebRTCStream *stream = (WebRTCStream *)data;
	return stream->getVideoNackCount();
}

extern "C" uint64_t millicast_stream_get_video_qp_sum(void *data)
{
	//Get stream
	WebRTCStream *stream = (WebRTCStream *)data;
	return stream->getVideoQpSum();
}

extern "C" uint64_t millicast_stream_get_audio_packets_sent(void *data)
{
	//Get stream
	WebRTCStream *stream = (WebRTCStream *)data;
	return stream->getAudioPacketsSent();
}

extern "C" uint64_t millicast_stream_get_audio_bytes_sent(void *data)
{
	//Get stream
	WebRTCStream *stream = (WebRTCStream *)data;
	return stream->getAudioBytesSent();
}

extern "C" uint32_t millicast_stream_get_track_audio_level(void *data)
{
	//Get stream
	WebRTCStream *stream = (WebRTCStream *)data;
	return stream->getTrackAudioLevel();
}

extern "C" uint32_t millicast_stream_get_track_total_audio_energy(void *data)
{
	//Get stream
	WebRTCStream *stream = (WebRTCStream *)data;
	return stream->getTrackTotalAudioEnergy();
}

extern "C" uint32_t
millicast_stream_get_track_total_samples_duration(void *data)
{
	//Get stream
	WebRTCStream *stream = (WebRTCStream *)data;
	return stream->getTrackTotalSamplesDuration();
}

extern "C" uint32_t millicast_stream_get_track_frame_width(void *data)
{
	//Get stream
	WebRTCStream *stream = (WebRTCStream *)data;
	return stream->getTrackFrameWidth();
}

extern "C" uint32_t millicast_stream_get_track_frame_height(void *data)
{
	//Get stream
	WebRTCStream *stream = (WebRTCStream *)data;
	return stream->getTrackFrameHeight();
}

extern "C" uint64_t millicast_stream_get_track_frames_sent(void *data)
{
	//Get stream
	WebRTCStream *stream = (WebRTCStream *)data;
	return stream->getTrackFramesSent();
}

extern "C" uint64_t millicast_stream_get_track_huge_frames_sent(void *data)
{
	//Get stream
	WebRTCStream *stream = (WebRTCStream *)data;
	return stream->getTrackHugeFramesSent();
}

extern "C" float millicast_stream_congestion(void *data)
{
	UNUSED_PARAMETER(data);
	return 0.0f;
}

extern "C" {
#ifdef _WIN32
struct obs_output_info millicast_output_info = {
	"millicast_output",                 //id
	OBS_OUTPUT_AV | OBS_OUTPUT_SERVICE, //flags
	millicast_stream_getname,           //get_name
	millicast_stream_create,            //create
	millicast_stream_destroy,           //destroy
	millicast_stream_start,             //start
	millicast_stream_stop,              //stop
	millicast_receive_video,            //raw_video
	millicast_receive_audio,            //raw_audio
	nullptr,                            //encoded_packet
	nullptr,                            //update
	millicast_stream_defaults,          //get_defaults
	millicast_stream_properties,        //get_properties
	nullptr,                            //unused1 (formerly pause)
	// NOTE LUDO: #80 add getStats
	millicast_stream_get_stats, millicast_stream_get_stats_list,
	millicast_stream_total_bytes_sent, //get_total_bytes
	millicast_stream_dropped_frames,   //get_dropped_frames
	// #310 webrtc getstats()
	millicast_stream_get_transport_bytes_sent, //get_transport_bytes_sent
	millicast_stream_get_transport_bytes_received, //get_transport_bytes_received
	millicast_stream_get_video_packets_sent,       //get_video_packets_sent
	millicast_stream_get_video_bytes_sent,         //get_video_bytes_sent
	millicast_stream_get_video_fir_count,          //get_video_fir_count
	millicast_stream_get_video_pli_count,          //get_video_pli_count
	millicast_stream_get_video_nack_count,         //get_video_nack_count
	millicast_stream_get_video_qp_sum,             //get_video_qp_sum
	millicast_stream_get_audio_packets_sent,       //get_audio_packets_sent
	millicast_stream_get_audio_bytes_sent,         //get_audio_bytes_sent
	millicast_stream_get_track_audio_level,        //get_track_audio_level
	millicast_stream_get_track_total_audio_energy, //get_track_total_audio_energy
	millicast_stream_get_track_total_samples_duration, //get_track_total_samples_duration
	millicast_stream_get_track_frame_width,      //get_track_frame_width
	millicast_stream_get_track_frame_height,     //get_track_frame_height
	millicast_stream_get_track_frames_sent,      //get_track_frames_sent
	millicast_stream_get_track_huge_frames_sent, //get_track_huge_frames_sent
	nullptr,                                     //type_data
	nullptr,                                     //free_type_data
	millicast_stream_congestion,                 //get_congestion
	nullptr,                                     //get_connect_time_ms
	"vp8",                                       //encoded_video_codecs
	"opus",                                      //encoded_audio_codecs
	nullptr                                      //raw_audio2
};
#else
struct obs_output_info millicast_output_info = {
	.id = "millicast_output",
	.flags = OBS_OUTPUT_AV | OBS_OUTPUT_SERVICE,
	.get_name = millicast_stream_getname,
	.create = millicast_stream_create,
	.destroy = millicast_stream_destroy,
	.start = millicast_stream_start,
	.stop = millicast_stream_stop,
	.raw_video = millicast_receive_video,
	.raw_audio = millicast_receive_audio, //for single-track
	.encoded_packet = nullptr,
	.update = nullptr,
	.get_defaults = millicast_stream_defaults,
	.get_properties = millicast_stream_properties,
	.unused1 = nullptr,
	// NOTE LUDO: #80 add getStats
	.get_stats = millicast_stream_get_stats,
	.get_stats_list = millicast_stream_get_stats_list,
	.get_total_bytes = millicast_stream_total_bytes_sent,
	.get_dropped_frames = millicast_stream_dropped_frames,
	// #310 webrtc getstats()
	.get_transport_bytes_sent = millicast_stream_get_transport_bytes_sent,
	.get_transport_bytes_received =
		millicast_stream_get_transport_bytes_received,
	.get_video_packets_sent = millicast_stream_get_video_packets_sent,
	.get_video_bytes_sent = millicast_stream_get_video_bytes_sent,
	.get_video_fir_count = millicast_stream_get_video_fir_count,
	.get_video_pli_count = millicast_stream_get_video_pli_count,
	.get_video_nack_count = millicast_stream_get_video_nack_count,
	.get_video_qp_sum = millicast_stream_get_video_qp_sum,
	.get_audio_packets_sent = millicast_stream_get_audio_packets_sent,
	.get_audio_bytes_sent = millicast_stream_get_audio_bytes_sent,
	.get_track_audio_level = millicast_stream_get_track_audio_level,
	.get_track_total_audio_energy =
		millicast_stream_get_track_total_audio_energy,
	.get_track_total_samples_duration =
		millicast_stream_get_track_total_samples_duration,
	.get_track_frame_width = millicast_stream_get_track_frame_width,
	.get_track_frame_height = millicast_stream_get_track_frame_height,
	.get_track_frames_sent = millicast_stream_get_track_frames_sent,
	.get_track_huge_frames_sent =
		millicast_stream_get_track_huge_frames_sent,
	.type_data = nullptr,
	.free_type_data = nullptr,
	.get_congestion = millicast_stream_congestion,
	.get_connect_time_ms = nullptr,
	.encoded_video_codecs = "vp8",
	.encoded_audio_codecs = "opus",
	.raw_audio2 = nullptr
	// .raw_audio2           = millicast_receive_multitrack_audio, //for multi-track
};
#endif
}
