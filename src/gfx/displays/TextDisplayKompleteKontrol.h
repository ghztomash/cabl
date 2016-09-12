/*
        ##########    Copyright (C) 2015 Vincenzo Pacella
        ##      ##    Distributed under MIT license, see file LICENSE
        ##      ##    or <http://opensource.org/licenses/MIT>
        ##      ##
##########      ############################################################# shaduzlabs.com #####*/

#pragma once

#include <cstdint>

#include "gfx/TextDisplay.h"

namespace sl
{
namespace cabl
{
class TextDisplayKompleteKontrol : public TextDisplayBase<8, 3, 48>
{

public:
  void putCharacter(uint8_t col_, uint8_t row_, char c_) override;

  void putText(const std::string& string_, uint8_t row_, Alignment align_) override;

  void putText(int value_, uint8_t row_, Alignment align_) override;

  void putText(double value_, uint8_t row_, Alignment align_) override;

  void putValue(float value_, uint8_t row_, Alignment align_) override;

private:
  std::string alignText(const std::string&, Alignment align_) const;

  void setDot(uint8_t nDot_, uint8_t row_, bool visible_ = true);

  void resetDots(uint8_t row_);
};

//--------------------------------------------------------------------------------------------------

} // namespace cabl
} // namespace sl