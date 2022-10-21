#include "audio/driver/ffmpeg.h"

#include <libavutil/channel_layout.h>

namespace driver {

FFmpeg::FFmpeg()
    : ch_layout_{new AVChannelLayout{}},
      input_stream_{},
      decoder_{},
      resampler_{},
      stream_index_{} {
  // Set output channel layout to stereo (2-channel)
  av_channel_layout_default(ch_layout_.get(), 2);

  // TODO: Control this with a parameter
  av_log_set_level(AV_LOG_QUIET);
}

/* ********************************************************************************************** */

error::Code FFmpeg::OpenInputStream(const std::string &filepath) {
  AVFormatContext *ptr = nullptr;

  if (avformat_open_input(&ptr, filepath.c_str(), nullptr, nullptr) < 0) {
    return error::kFileNotSupported;
  }

  input_stream_.reset(std::move(ptr));

  if (avformat_find_stream_info(input_stream_.get(), nullptr) < 0) {
    return error::kFileNotSupported;
  }

  return error::kSuccess;
}

/* ********************************************************************************************** */

error::Code FFmpeg::ConfigureDecoder() {
  const AVCodec *codec = nullptr;
  AVCodecParameters *parameters = nullptr;

  for (int i = 0; i < input_stream_->nb_streams; i++) {
    parameters = input_stream_->streams[i]->codecpar;
    if (parameters->codec_type == AVMEDIA_TYPE_AUDIO) {
      stream_index_ = i;
      codec = avcodec_find_decoder(parameters->codec_id);

      if (!codec) {
        return error::kUnknownError;
      }

      break;
    }
  }

  decoder_ = CodecContext{avcodec_alloc_context3(codec)};

  if (!decoder_ || avcodec_parameters_to_context(decoder_.get(), parameters) < 0) {
    return error::kUnknownError;
  }

  if (!codec->ch_layouts) {
    auto dummy = (AVChannelLayout)AV_CHANNEL_LAYOUT_STEREO;
    av_channel_layout_copy(&decoder_->ch_layout, &dummy);
  }

  if (avcodec_open2(decoder_.get(), codec, nullptr) < 0) {
    return error::kUnknownError;
  }

  return error::kSuccess;
}

/* ********************************************************************************************** */

error::Code FFmpeg::ConfigureResampler() {
  SwrContext *dummy{};
  swr_alloc_set_opts2(&dummy, ch_layout_.get(), kSampleFormat, kSampleRate, &decoder_->ch_layout,
                      decoder_->sample_fmt, decoder_->sample_rate, 0, nullptr);

  resampler_.reset(std::move(dummy));

  if (!resampler_ || swr_init(resampler_.get()) < 0) {
    return error::kUnknownError;
  }

  return error::kSuccess;
}

/* ********************************************************************************************** */

void FFmpeg::FillAudioInformation(model::Song *audio_info) {
  // use this to get all metadata associated to this audio file
  //   const AVDictionaryEntry *tag = nullptr;
  //   while ((tag = av_dict_get(input_stream_->metadata, "", tag, AV_DICT_IGNORE_SUFFIX)))
  //     printf("%s=%s\n", tag->key, tag->value);
  const AVDictionaryEntry *tag = nullptr;

  // Get track name
  tag = av_dict_get(input_stream_->metadata, "title", tag, AV_DICT_IGNORE_SUFFIX);
  if (tag) audio_info->title = std::string{tag->value};

  // Get artist name
  tag = av_dict_get(input_stream_->metadata, "artist", tag, AV_DICT_IGNORE_SUFFIX);
  if (tag) audio_info->artist = std::string{tag->value};

  const AVCodecParameters *audio_stream = input_stream_->streams[stream_index_]->codecpar;

  audio_info->num_channels = (uint16_t)audio_stream->ch_layout.nb_channels,
  audio_info->sample_rate = (uint32_t)audio_stream->sample_rate;
  audio_info->bit_rate = (uint32_t)audio_stream->bit_rate,
  audio_info->bit_depth = (uint32_t)sample_fmt_info[audio_stream->format].bits;
  audio_info->duration = (uint32_t)(input_stream_->duration / AV_TIME_BASE);
}

/* ********************************************************************************************** */

error::Code FFmpeg::OpenFile(model::Song *audio_info) {
  auto clean_up_and_return = [&](error::Code error_code) mutable {
    ClearCache();
    return error_code;
  };

  error::Code result = OpenInputStream(audio_info->filepath);
  if (result != error::kSuccess) return clean_up_and_return(result);

  result = ConfigureDecoder();
  if (result != error::kSuccess) return clean_up_and_return(result);

  result = ConfigureResampler();
  if (result != error::kSuccess) return clean_up_and_return(result);

  // At this point, we can get detailed information about the song
  FillAudioInformation(audio_info);

  return result;
}

/* ********************************************************************************************** */

error::Code FFmpeg::Decode(int samples, AudioCallback callback) {
  int max_buffer_size = av_samples_get_buffer_size(nullptr, decoder_->ch_layout.nb_channels,
                                                   samples, decoder_->sample_fmt, 1);

  Packet packet = Packet{av_packet_alloc()};
  Frame frame = Frame{av_frame_alloc()};

  if (!packet || !frame) {
    return error::kUnknownError;
  }

  DataBuffer allocated_buffer = DataBuffer{(uint8_t *)av_malloc(max_buffer_size)};
  uint8_t *buffer = allocated_buffer.get();

  // Control flags
  bool continue_decoding = true;

  // Flag to seek frame based on value informed by callback (in this case, set by player)
  bool seek_frame = false;
  int64_t position = 0;

  while (av_read_frame(input_stream_.get(), packet.get()) >= 0 && continue_decoding) {
    if (packet->stream_index != stream_index_) {
      av_packet_unref(packet.get());
      continue;
    }

    if (avcodec_send_packet(decoder_.get(), packet.get()) < 0) {
      return error::kDecodeFileFailed;
    }

    while (avcodec_receive_frame(decoder_.get(), frame.get()) >= 0 && continue_decoding) {
      // Note that AVPacket.pts is in AVStream.time_base units, not AVCodecContext.time_base units
      position = packet->pts / input_stream_->streams[stream_index_]->time_base.den;
      int64_t old_position = position;

      int samples_size = swr_convert(resampler_.get(), &buffer, samples,
                                     (const uint8_t **)(frame->data), frame->nb_samples);

      while (samples_size > 0 && continue_decoding) {
        continue_decoding = callback(buffer, max_buffer_size, samples_size, position);
        samples_size = swr_convert(resampler_.get(), &buffer, samples, nullptr, 0);

        if (position != old_position) seek_frame = true;
      }

      if (seek_frame) {
        avcodec_flush_buffers(decoder_.get());
        int64_t target = av_rescale_q(position * AV_TIME_BASE, AV_TIME_BASE_Q,
                                      input_stream_->streams[stream_index_]->time_base);

        if (av_seek_frame(input_stream_.get(), stream_index_, target, AVSEEK_FLAG_BACKWARD) < 0)
          return error::kSeekFrameFailed;

        seek_frame = false;
      }

        av_frame_unref(frame.get());
    }

    av_packet_unref(packet.get());
  }

  return error::kSuccess;
}

/* ********************************************************************************************** */

void FFmpeg::ClearCache() {
  input_stream_.reset();
  decoder_.reset();
  resampler_.reset();

  stream_index_ = 0;
}

}  // namespace driver
