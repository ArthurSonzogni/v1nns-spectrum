#include "view/element/error_dialog.h"

namespace interface {

ErrorDialog::ErrorDialog()
    : style_{DialogStyle{
          .background = ftxui::Color::DarkRedBis,
          .foreground = ftxui::Color::Grey93,
      }},
      opened_{false},
      message_{} {}

/* ********************************************************************************************** */

ftxui::Element ErrorDialog::Render() {
  using ftxui::WIDTH, ftxui::HEIGHT, ftxui::EQUAL;

  auto decorator = ftxui::size(HEIGHT, EQUAL, kMaxLines) | ftxui::size(WIDTH, EQUAL, kMaxColumns) |
                   ftxui::borderDouble | ftxui::bgcolor(style_.background) |
                   ftxui::color(style_.foreground) | ftxui::center;

  return ftxui::vbox({
             ftxui::text(" ERROR") | ftxui::bold,
             ftxui::text(""),
             ftxui::paragraph(message_) | ftxui::center | ftxui::bold,
         }) |
         decorator;
}

/* ********************************************************************************************** */

bool ErrorDialog::OnEvent(ftxui::Event event) {
  if (event == ftxui::Event::Return || event == ftxui::Event::Escape ||
      event == ftxui::Event::Character('q')) {
    Clear();
  }

  // This is to ensure that no one else will treat any event while on error mode
  return true;
}

/* ********************************************************************************************** */

void ErrorDialog::SetErrorMessage(const std::string& message) {
  message_ = message;
  opened_ = true;
}

/* ********************************************************************************************** */

void ErrorDialog::Clear() {
  opened_ = false;
  message_.clear();
}

}  // namespace interface
