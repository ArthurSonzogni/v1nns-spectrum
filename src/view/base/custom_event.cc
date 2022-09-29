#include "view/base/custom_event.h"

namespace interface {

// Static
CustomEvent CustomEvent::ClearSongInfo() {
  return CustomEvent{
      .type = Type::FromAudioThreadToInterface,
      .id = Identifier::ClearSongInfo,
  };
}

/* ********************************************************************************************** */

// Static
CustomEvent CustomEvent::UpdateVolume(const model::Volume& sound_volume) {
  return CustomEvent{
      .type = Type::FromAudioThreadToInterface,
      .id = Identifier::UpdateVolume,
      .content = sound_volume,
  };
}

/* ********************************************************************************************** */

// Static
CustomEvent CustomEvent::UpdateSongInfo(const model::Song& info) {
  return CustomEvent{
      .type = Type::FromAudioThreadToInterface,
      .id = Identifier::UpdateSongInfo,
      .content = info,
  };
}

/* ********************************************************************************************** */

// Static
CustomEvent CustomEvent::UpdateSongState(const model::Song::CurrentInformation& new_state) {
  return CustomEvent{
      .type = Type::FromAudioThreadToInterface,
      .id = Identifier::UpdateSongState,
      .content = new_state,
  };
}

/* ********************************************************************************************** */

// Static
CustomEvent CustomEvent::DrawAudioSpectrum(const std::vector<double>& data) {
  return CustomEvent{
      .type = Type::FromAudioThreadToInterface,
      .id = Identifier::DrawAudioSpectrum,
      .content = data,
  };
}

/* ********************************************************************************************** */

// Static
CustomEvent CustomEvent::NotifyFileSelection(const std::filesystem::path file_path) {
  return CustomEvent{
      .type = Type::FromInterfaceToAudioThread,
      .id = Identifier::NotifyFileSelection,
      .content = file_path,
  };
}

/* ********************************************************************************************** */

// Static
CustomEvent CustomEvent::PauseOrResumeSong() {
  return CustomEvent{
      .type = Type::FromInterfaceToAudioThread,
      .id = Identifier::PauseOrResumeSong,
  };
}

/* ********************************************************************************************** */

// Static
CustomEvent CustomEvent::ClearCurrentSong() {
  return CustomEvent{
      .type = Type::FromInterfaceToAudioThread,
      .id = Identifier::ClearCurrentSong,
  };
}

/* ********************************************************************************************** */

// Static
CustomEvent CustomEvent::SetAudioVolume(const model::Volume& sound_volume) {
  return CustomEvent{
      .type = Type::FromInterfaceToAudioThread,
      .id = Identifier::SetAudioVolume,
      .content = sound_volume,
  };
}

/* ********************************************************************************************** */

// Static
CustomEvent CustomEvent::ResizeAnalysis(int bars) {
  return CustomEvent{
      .type = Type::FromInterfaceToAudioThread,
      .id = Identifier::ResizeAnalysis,
      .content = bars,
  };
}

/* ********************************************************************************************** */

// Static
CustomEvent CustomEvent::Refresh() {
  return CustomEvent{
      .type = Type::FromInterfaceToInterface,
      .id = Identifier::Refresh,
  };
}

}  // namespace interface
