/**
 * \file
 * \brief  Class representing the whole terminal
 */

#ifndef INCLUDE_UI_BASE_TERMINAL_H_
#define INCLUDE_UI_BASE_TERMINAL_H_

#include <memory>
#include <optional>
#include <vector>

#include "error/error_table.h"
#include "ftxui/component/component_base.hpp"      // for ComponentBase
#include "ftxui/component/screen_interactive.hpp"  // for ScreenInteractive
#include "ftxui/dom/elements.hpp"  // for operator|, separator, Element, Decorator, color, text, hbox, size, bold, frame, inverted, vbox, HEIGHT, LESS_THAN, border
#include "ui/block/list_directory.h"

namespace interface {

using namespace ftxui;

/**
 * @brief Base class that manages the whole screen and hold all blocks
 */
class Terminal {
 public:
  /**
   * @brief Construct a new Terminal object
   */
  Terminal();

  /**
   * @brief Destroy the Terminal object
   */
  virtual ~Terminal();

  /* ******************************************************************************************** */
  //! Remove these constructors/operators
  Terminal(const Terminal& other) = delete;             // copy constructor
  Terminal(Terminal&& other) = delete;                  // move constructor
  Terminal& operator=(const Terminal& other) = delete;  // copy assignment
  Terminal& operator=(Terminal&& other) = delete;       // move assignment

  /* ******************************************************************************************** */
  /**
   * @brief Initialize screen for Terminal object
   *
   * @return int Error code from operation
   */
  void Init();

  /**
   * @brief Force application to exit
   */
  void Exit();

  /**
   * @brief Main loop for the graphical interface
   */
  void Loop();

  /* ******************************************************************************************** */
 private:
  Component container_;
};

}  // namespace interface
#endif  // INCLUDE_UI_BASE_TERMINAL_H_