/**
 * \file
 * \brief  Class representing the whole terminal
 */

#ifndef INCLUDE_VIEW_BASE_TERMINAL_H_
#define INCLUDE_VIEW_BASE_TERMINAL_H_

#include <memory>
#include <optional>
#include <vector>

#include "ftxui/component/captured_mouse.hpp"  // for ftxui
#include "ftxui/component/component_base.hpp"  // for Component
#include "ftxui/component/receiver.hpp"
#include "middleware/media_controller.h"
#include "model/application_error.h"
#include "view/base/block.h"
#include "view/base/custom_event.h"
#include "view/base/event_dispatcher.h"

//! Forward declaration
namespace audio {
class AudioControl;
}

namespace interface {

//! Using-declaration for every possible callback function
using EventCallback = std::function<void(ftxui::Event)>;
using Callback = std::function<void()>;

/**
 * @brief Manages the whole screen and contains all block views
 */
class Terminal : public EventDispatcher, public ftxui::ComponentBase {
 private:
  /**
   * @brief Construct a new Terminal object
   */
  Terminal();

 public:
  /**
   * @brief Factory method: Create, initialize internal components and return Terminal object
   * @return std::shared_ptr<Terminal> Terminal instance
   */
  static std::shared_ptr<Terminal> Create();

  /**
   * @brief Destroy the Terminal object
   */
  virtual ~Terminal();

  //! Remove these
  Terminal(const Terminal& other) = delete;             // copy constructor
  Terminal(Terminal&& other) = delete;                  // move constructor
  Terminal& operator=(const Terminal& other) = delete;  // copy assignment
  Terminal& operator=(Terminal&& other) = delete;       // move assignment

  /* ******************************************************************************************** */
  //! Internal operations
 private:
  /**
   * @brief Initialize internal components for Terminal object
   */
  void Init();

  /**
   * @brief Force application to exit
   */
  void Exit();

  /* ******************************************************************************************** */
  //! Binds and registrations
 public:
  /**
   * @brief Register listener to receive events from interface
   * @param listener Interface listener
   */
  void RegisterInterfaceListener(const std::shared_ptr<interface::Listener>& listener);

  /**
   * @brief Bind an external send event function to an internal function
   * @param cb Callback function to send custom events to terminal user interface
   */
  void RegisterEventSenderCallback(EventCallback cb);

  /**
   * @brief Bind an external exit function to an internal function
   * @param cb Callback function to exit graphical application
   */
  void RegisterExitCallback(Callback cb);

  /* ******************************************************************************************** */
  //! UI Interface API

  /**
   * @brief Renders the component
   * @return Element Built element based on internal state
   */
  ftxui::Element Render() override;

  /**
   * @brief Handles an event (from mouse/keyboard)
   * @param event Received event from screen
   * @return true if event was handled, otherwise false
   */
  bool OnEvent(ftxui::Event event) override;

  /* ******************************************************************************************** */
  //! Internal event handling
 private:
  /**
   * @brief When it has any custom event, handles to broadcast it to children
   * @return true if event was handled, otherwise false
   */
  bool OnCustomEvent();

  /**
   * @brief Handles a event when no internal mode has been set
   * @param event Received event from screen
   * @return true if event was handled, otherwise false
   */
  bool OnGlobalModeEvent(const ftxui::Event& event);

  /**
   * @brief Handles a event while on error mode (it means someone informed an error)
   * @param event Received event from screen
   * @return true if event was handled, otherwise false
   */
  bool OnErrorModeEvent(const ftxui::Event& event);

  /* ******************************************************************************************** */
  //! UI Event dispatching
 public:
  //! Send event to blocks
  void SendEvent(const CustomEvent& event) override;

  //! Set application error (can be originated from controller or any interface::block)
  void SetApplicationError(error::Code id) override;

  /* ******************************************************************************************** */
  //! Variables
 private:
  std::weak_ptr<interface::Listener> listener_;  //!< Outside listener for events from UI
  std::optional<error::Code> last_error_;        //!< Last application error

  ftxui::Receiver<CustomEvent> receiver_;  //! Custom event receiver
  ftxui::Sender<CustomEvent> sender_;      //! Custom event sender

  EventCallback cb_send_event_;  //!< Function to send custom events to terminal interface
  Callback cb_exit_;             //!< Function to exit from graphical interface
};

}  // namespace interface
#endif  // INCLUDE_VIEW_BASE_TERMINAL_H_
