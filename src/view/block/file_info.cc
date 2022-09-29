#include "view/block/file_info.h"

#include <string>
#include <utility>  // for move
#include <vector>   // for vector

#include "ftxui/component/event.hpp"  // for Event

namespace interface {

/* ********************************************************************************************** */

FileInfo::FileInfo(const std::shared_ptr<EventDispatcher>& dispatcher)
    : Block{dispatcher, Identifier::FileInfo, interface::Size{.width = 0, .height = kMaxRows}},
      audio_info_{} {}

/* ********************************************************************************************** */

ftxui::Element FileInfo::Render() {
  ftxui::Elements lines;

  // Choose a different color for when there is no current song
  ftxui::Color::Palette256 color =
      audio_info_.filepath.empty() ? ftxui::Color::Grey50 : ftxui::Color::Grey82;

  // Use istringstream to split string into lines and parse it as <Field, Value>
  std::istringstream input{model::to_string(audio_info_)};
  size_t pos;

  for (std::string line; std::getline(input, line);) {
    pos = line.find_first_of(':');
    std::string field = line.substr(0, pos), value = line.substr(pos + 1);

    // Create element
    ftxui::Element item = ftxui::hbox({
        ftxui::text(field) | ftxui::bold | ftxui::color(ftxui::Color::CadetBlue),
        ftxui::filler(),
        ftxui::text(value) | ftxui::align_right | ftxui::color(ftxui::Color(color)),
    });

    lines.push_back(item);
  }

  ftxui::Element content = ftxui::vbox(std::move(lines));

  using ftxui::HEIGHT, ftxui::EQUAL;
  return ftxui::window(ftxui::text(" information "), std::move(content)) |
         ftxui::size(HEIGHT, EQUAL, kMaxRows);
}

/* ********************************************************************************************** */

bool FileInfo::OnEvent(ftxui::Event event) { return false; }

/* ********************************************************************************************** */

bool FileInfo::OnCustomEvent(const CustomEvent& event) {
  // Do not return true because other blocks may use it
  if (event == CustomEvent::Identifier::ClearSongInfo) {
    audio_info_ = model::Song{};
  }

  // Do not return true because other blocks may use it
  if (event == CustomEvent::Identifier::UpdateSongInfo) {
    audio_info_ = event.GetContent<model::Song>();
  }

  return false;
}

}  // namespace interface
