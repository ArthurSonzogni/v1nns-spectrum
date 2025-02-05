/**
 * \file
 * \brief  Class for block containing tab view
 */

#ifndef INCLUDE_VIEW_BLOCK_TAB_VIEWER_H_
#define INCLUDE_VIEW_BLOCK_TAB_VIEWER_H_

#include <memory>
#include <unordered_map>

#include "view/base/block.h"
#include "view/element/button.h"
#include "view/element/tab_item.h"

namespace interface {

/**
 * @brief Component to display a set of tabs and their respective content
 */
class TabViewer : public Block {
 public:
  /**
   * @brief Construct a new TabViewer object
   * @param dispatcher Block event dispatcher
   */
  explicit TabViewer(const std::shared_ptr<EventDispatcher>& dispatcher);

  /**
   * @brief Destroy the TabViewer object
   */
  virtual ~TabViewer() = default;

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

  /**
   * @brief Handles a custom event
   * @param event Received event (probably sent by Audio thread)
   * @return true if event was handled, otherwise false
   */
  bool OnCustomEvent(const CustomEvent& event) override;

  /**
   * @brief Possible tab views to render on this block
   */
  enum class View {
    Visualizer,  //!< Display spectrum visualizer (default)
    Equalizer,   //!< Display audio equalizer
    LAST,
  };

  /* ******************************************************************************************** */
  //! Private methods
 private:
  //! Handle mouse event
  bool OnMouseEvent(ftxui::Event event);

  //! For readability
  using Item = std::unique_ptr<TabItem>;
  using Keybinding = std::string;

  //! Get active tabview
  Item& active() { return views_[active_].item; }

  /* ******************************************************************************************** */
  //! Variables
 private:
  WindowButton btn_help_, btn_exit_;  //!< Buttons located on the upper-right border of block window

  //! Represent a single tab item entry
  struct Tab {
    Keybinding key;       //!< Keybinding to set item as active
    WindowButton button;  //!< Button to render in the tab border
    Item item;            //!< View to render in the tab content
  };

  View active_;                          //!< Current view displayed on block
  std::unordered_map<View, Tab> views_;  //!< All possible views to render in this component
};

}  // namespace interface

#endif  // INCLUDE_VIEW_BLOCK_TAB_VIEWER_H_
