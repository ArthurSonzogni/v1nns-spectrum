/**
 * \file
 * \brief  Class for block containing audio player
 */

#ifndef INCLUDE_VIEW_BLOCK_AUDIO_PLAYER_H_
#define INCLUDE_VIEW_BLOCK_AUDIO_PLAYER_H_

#include <memory>  // for shared_ptr, unique_ptr

#include "ftxui/component/captured_mouse.hpp"  // for ftxui
#include "ftxui/dom/elements.hpp"              // for Element
#include "model/song.h"                        // for Song
#include "view/base/block.h"                   // for Block, BlockEvent (ptr...

namespace interface {

/**
 * @brief Component with detailed information about the chosen file (in this case, some music file)
 */
class AudioPlayer : public Block {
 public:
  /**
   * @brief Construct a new Audio Player object
   * @param d Block event dispatcher
   */
  explicit AudioPlayer(const std::shared_ptr<EventDispatcher>& d);

  /**
   * @brief Destroy the Audio Player object
   */
  virtual ~AudioPlayer() = default;

  /**
   * @brief Renders the component
   * @return Element Built element based on internal state
   */
  ftxui::Element Render() override;

  /**
   * @brief Handles an event (from mouse/keyboard)
   *
   * @param event Received event from screen
   * @return true if event was handled, otherwise false
   */
  bool OnEvent(ftxui::Event event) override;

  /**
   * @brief Handles an event (from another block)
   * @param event Received event from dispatcher
   */
  void OnBlockEvent(BlockEvent event) override;
};

}  // namespace interface
#endif  // INCLUDE_VIEW_BLOCK_AUDIO_PLAYER_H_