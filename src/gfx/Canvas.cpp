/*
        ##########    Copyright (C) 2015 Vincenzo Pacella
        ##      ##    Distributed under MIT license, see file LICENSE
        ##      ##    or <http://opensource.org/licenses/MIT>
        ##      ##
##########      ############################################################# shaduzlabs.com #####*/

#include "gfx/Canvas.h"

#include <algorithm>

#include <cstdio>
#include <cstdlib>

#include "util/Functions.h"

//--------------------------------------------------------------------------------------------------

#define M_SWAP(a, b) \
  {                  \
    a ^= b;          \
    b ^= a;          \
    a ^= b;          \
  }

//--------------------------------------------------------------------------------------------------

namespace sl
{
namespace cabl
{

//--------------------------------------------------------------------------------------------------

Canvas::Canvas(uint16_t width_, uint16_t height_)
  : m_width(width_), m_height(height_), m_pFont(FontManager::instance().getDefaultFont())
{
  initialize();
}

//--------------------------------------------------------------------------------------------------

void Canvas::invert()
{
  std::for_each(m_data.begin(), m_data.end(), [](uint8_t& pixel_) { pixel_ = ~pixel_; });
}

//--------------------------------------------------------------------------------------------------

void Canvas::fillPattern(uint8_t value_)
{
  std::fill(m_data.begin(), m_data.end(), value_);
}

//--------------------------------------------------------------------------------------------------

void Canvas::setPixel(uint16_t x_, uint16_t y_, const util::ColorRGB& color_)
{
  if (x_ >= width() || y_ >= height() || color_.transparent())
  {
    return;
  }

  util::ColorRGB oldColor = pixel(x_, y_);
  unsigned byteIndex = (canvasWidthInBytes() * y_) + ( 3 * x_) ;

  if (color_.blendMode() == util::ColorRGB::BlendMode::Invert)
  {
    data()[byteIndex] = ~oldColor.red();
    data()[byteIndex + 1] = ~oldColor.green();
    data()[byteIndex + 2] = ~oldColor.blue();
  }
  else
  {
    data()[byteIndex] = color_.red();
    data()[byteIndex + 1] = color_.green();
    data()[byteIndex + 2] = color_.blue();
  }
}

//--------------------------------------------------------------------------------------------------

util::ColorRGB Canvas::pixel(uint16_t x_, uint16_t y_) const
{
  if (x_ >= width() || y_ >= height())
  {
    return {};
  }
  unsigned byteIndex = (canvasWidthInBytes() * y_) + (3 * x_) ;
  
  return {data()[byteIndex], data()[byteIndex + 1], data()[byteIndex + 2] };
}

//--------------------------------------------------------------------------------------------------

void Canvas::line(
  uint16_t x0_, uint16_t y0_, uint16_t x1_, uint16_t y1_, const util::ColorRGB& color_)
{
  int32_t e;
  int32_t dx, dy;
  int16_t s1, s2;
  int16_t x, y;
  bool bSwapped = false;

  x = x0_;
  y = y0_;

  if (x1_ < x0_)
  {
    dx = x0_ - x1_;
    s1 = -1;
  }
  else if (x1_ == x0_)
  {
    dx = 0;
    s1 = 0;
  }
  else
  {
    dx = x1_ - x0_;
    s1 = 1;
  }

  if (y1_ < y0_)
  {
    dy = y0_ - y1_;
    s2 = -1;
  }
  else if (y1_ == y0_)
  {
    dy = 0;
    s2 = 0;
  }
  else
  {
    dy = y1_ - y0_;
    s2 = 1;
  }

  if (dy > dx)
  {
    M_SWAP(dx, dy);
    bSwapped = true;
  }

  e = ((int)dy << 1) - dx;

  for (uint16_t j = 0; j <= dx; j++)
  {
    setPixel(x, y, color_);

    if (e >= 0)
    {
      if (bSwapped)
      {
        x = x + s1;
      }
      else
      {
        y = y + s2;
      }
      e = e - ((int)dx << 1);
    }
    if (bSwapped)
    {
      y = y + s2;
    }
    else
    {
      x = x + s1;
    }
    e = e + ((int)dy << 1);
  }
}

//--------------------------------------------------------------------------------------------------

void Canvas::Canvas::lineVertical(
  uint16_t x_, uint16_t y_, uint16_t l_, const util::ColorRGB& color_)
{
  for (unsigned y = y_; y < y_ + l_; y++)
  {
    setPixel(x_, y, color_);
  }
}

//--------------------------------------------------------------------------------------------------

void Canvas::lineHorizontal(uint16_t x_, uint16_t y_, uint16_t l_, const util::ColorRGB& color_)
{
  for (uint16_t x = x_; x < x_ + l_; x++)
  {
    setPixel(x, y_, color_);
  }
}

//--------------------------------------------------------------------------------------------------

void Canvas::triangle(uint16_t x0_,
  uint16_t y0_,
  uint16_t x1_,
  uint16_t y1_,
  uint16_t x2_,
  uint16_t y2_,
  const util::ColorRGB& color_)
{
  line(x0_, y0_, x1_, y1_, color_);
  line(x1_, y1_, x2_, y2_, color_);
  line(x2_, y2_, x0_, y0_, color_);
}

//--------------------------------------------------------------------------------------------------

void Canvas::triangleFilled(uint16_t x0_,
  uint16_t y0_,
  uint16_t x1_,
  uint16_t y1_,
  uint16_t x2_,
  uint16_t y2_,
  const util::ColorRGB& color_,
  const util::ColorRGB& fillColor_)
{
  // Original Author: Adafruit Industries (Adafruit GFX library)

  int16_t a, b, y, last;

  // Sort coordinates by y order (y2 >= y1 >= y0)
  if (y0_ > y1_)
  {
    M_SWAP(y0_, y1_);
    M_SWAP(x0_, x1_);
  }
  if (y1_ > y2_)
  {
    M_SWAP(y2_, y1_);
    M_SWAP(x2_, x1_);
  }
  if (y0_ > y1_)
  {
    M_SWAP(y0_, y1_);
    M_SWAP(x0_, x1_);
  }

  if (y0_ == y2_)
  { // Handle awkward all-on-same-line case as its own thing
    a = b = x0_;
    if (x1_ < a)
    {
      a = x1_;
    }
    else if (x1_ > b)
    {
      b = x1_;
    }
    if (x2_ < a)
    {
      a = x2_;
    }
    else if (x2_ > b)
    {
      b = x2_;
    }
    lineHorizontal(a, y0_, b - a + 1, color_);
    return;
  }

  int16_t dx01 = x1_ - x0_, dy01 = y1_ - y0_, dx02 = x2_ - x0_, dy02 = y2_ - y0_, dx12 = x2_ - x1_,
          dy12 = y2_ - y1_;
  int32_t sa = 0, sb = 0;

  // For upper part of triangle, find scanline crossings for segments
  // 0-1 and 0-2.  If y1=y2 (flat-bottomed triangle), the scanline y1
  // is included here (and second loop will be skipped, avoiding a /0
  // error there), otherwise scanline y1 is skipped here and handled
  // in the second loop...which also avoids a /0 error here if y0=y1
  // (flat-topped triangle).
  if (y1_ == y2_)
  {
    last = y1_; // Include y1 scanline
  }
  else
  {
    last = y1_ - 1; // Skip it
  }

  for (y = y0_; y <= last; y++)
  {
    a = x0_ + sa / dy01;
    b = x0_ + sb / dy02;
    sa += dx01;
    sb += dx02;
    /* longhand:
     a = x0 + (x1 - x0) * (y - y0) / (y1 - y0);
     b = x0 + (x2 - x0) * (y - y0) / (y2 - y0);
     */
    if (a > b)
    {
      M_SWAP(a, b);
    }
    lineHorizontal(a, y, b - a + 1, color_);
  }

  // For lower part of triangle, find scanline crossings for segments
  // 0-2 and 1-2.  This loop is skipped if y1=y2.
  sa = dx12 * (y - y1_);
  sb = dx02 * (y - y0_);
  for (; y <= y2_; y++)
  {
    a = x1_ + sa / dy12;
    b = x0_ + sb / dy02;
    sa += dx12;
    sb += dx02;
    /* longhand:
     a = x1 + (x2 - x1) * (y - y1) / (y2 - y1);
     b = x0 + (x2 - x0) * (y - y0) / (y2 - y0);
     */
    if (a > b)
    {
      M_SWAP(a, b);
    }
    lineHorizontal(a, y, b - a + 1, color_);
  }
}

//--------------------------------------------------------------------------------------------------

void Canvas::rectangle(
  uint16_t x_, uint16_t y_, uint16_t w_, uint16_t h_, const util::ColorRGB& color_)
{
  rectangleFilled(x_, y_, w_, h_, color_, {});
}

//--------------------------------------------------------------------------------------------------

void Canvas::rectangleFilled(uint16_t x_,
  uint16_t y_,
  uint16_t w_,
  uint16_t h_,
  const util::ColorRGB& color_,
  const util::ColorRGB& fillColor_)
{
  if (x_ > width() || y_ > height() || w_ == 0 || h_ == 0)
  {
    return;
  }

  lineHorizontal(x_, y_, w_, color_);
  lineHorizontal(x_, y_ + h_ - 1, w_, color_);
  if (h_ <= 2)
  {
    return;
  }

  lineVertical(x_, y_ + 1, h_ - 2, color_);
  lineVertical(x_ + w_ - 1, y_ + 1, h_ - 2, color_);

  if (fillColor_.transparent())
  {
    return;
  }

  if (w_ > h_)
  {
    uint16_t lineWidth = w_ - 2;
    for (uint16_t i = y_ + 1; i < y_ + h_ - 1; i++)
    {
      lineHorizontal(x_ + 1, i, lineWidth, fillColor_);
    }
  }
  else
  {
    uint16_t lineHeight = h_ - 2;
    for (uint16_t i = x_ + 1; i < x_ + w_ - 1; i++)
    {
      lineVertical(i, y_ + 1, lineHeight, fillColor_);
    }
  }
}

//--------------------------------------------------------------------------------------------------

void Canvas::rectangleRounded(
  uint16_t x_, uint16_t y_, uint16_t w_, uint16_t h_, uint16_t r_, const util::ColorRGB& color_)
{
  rectangleRoundedFilled(x_, y_, w_, h_, r_, color_, {});
}

//--------------------------------------------------------------------------------------------------

void Canvas::rectangleRoundedFilled(uint16_t x_,
  uint16_t y_,
  uint16_t w_,
  uint16_t h_,
  uint16_t r_,
  const util::ColorRGB& color_,
  const util::ColorRGB& fillColor_)
{
  if (x_ > width() || y_ > height() || w_ == 0 || h_ == 0)
  {
    return;
  }

  uint16_t smallestSide = (w_ >= h_) ? h_ : w_;
  uint16_t rOffset = 2 * r_;
  if (rOffset > smallestSide)
  {
    r_ = smallestSide / 2;
    rOffset = smallestSide;
  }
  lineHorizontal((x_ + r_), (y_), (w_ - rOffset), color_);
  lineHorizontal((x_ + r_), (y_ + h_ - 1), (w_ - rOffset), color_);
  lineVertical((x_), (y_ + r_), (h_ - rOffset), color_);
  lineVertical((x_ + w_ - 1), (y_ + r_), (h_ - rOffset), color_);

  circleFilled((x_ + r_), (y_ + r_), r_, color_, fillColor_, CircleType::QuarterTopLeft);

  circleFilled(
    (x_ + w_ - r_ - 1), (y_ + r_), r_, color_, fillColor_, CircleType::QuarterTopRight);

  circleFilled(
    (x_ + w_ - r_ - 1), (y_ + h_ - r_ - 1), r_, color_, fillColor_, CircleType::QuarterBottomRight);

  circleFilled(
    (x_ + r_), (y_ + h_ - r_ - 1), r_, color_, fillColor_, CircleType::QuarterBottomLeft);

  if (fillColor_.transparent() || h_ <= 2 || w_ <= 2)
  {
    return;
  }

  rectangleFilled((x_ + r_), (y_ + 1), (w_ - rOffset), r_, fillColor_, fillColor_);

  rectangleFilled((x_ + 1), (y_ + r_), (w_ - 2), (h_ - rOffset), fillColor_, fillColor_);

  rectangleFilled((x_ + r_), (y_ + h_ - 1 - r_), (w_ - rOffset), (r_), fillColor_, fillColor_);
}

//--------------------------------------------------------------------------------------------------

void Canvas::circle(
  uint16_t x_, uint16_t y_, uint16_t r_, const util::ColorRGB& color_, CircleType type_)
{
  circleFilled(x_, y_, r_, color_, {}, type_);
}

//--------------------------------------------------------------------------------------------------

void Canvas::circleFilled(uint16_t x_,
  uint16_t y_,
  uint16_t r_,
  const util::ColorRGB& color_,
  const util::ColorRGB& fillColor_,
  CircleType type_)
{
  if (r_ == 0)
  {
    return;
  }

  int x, y, rX0, rX1, rY0, rY1;
  rX0 = rY0 = -r_;
  rX1 = rY1 = r_;

  switch (type_)
  {
    case CircleType::SemiLeft:
      rX1 = 0;
      break;
    case CircleType::SemiRight:
      rX0 = 0;
      break;
    case CircleType::SemiTop:
      rY1 = 0;
      break;
    case CircleType::SemiBottom:
      rY0 = 0;
      break;

    case CircleType::QuarterTopLeft:
      rX1 = 0;
      rY1 = 0;
      break;
    case CircleType::QuarterTopRight:
      rX0 = 0;
      rY1 = 0;
      break;
    case CircleType::QuarterBottomLeft:
      rX1 = 0;
      rY0 = 0;
      break;
    case CircleType::QuarterBottomRight:
      rX0 = 0;
      rY0 = 0;
      break;

    default:
    case CircleType::Full:
      break;
  }

  for (x = rX0; x <= rX1; x++)
  {
    for (y = rY0; y <= rY1; y++)
    {
      int32_t xysq = ((x * x) + (y * y));
      int32_t rsq = r_ * r_;
      if ((rsq - xysq < r_) && (xysq - rsq < r_))
      {
        setPixel((x + x_), (y + y_), color_);
      }
      else if (!fillColor_.transparent() && (xysq < rsq))
      {
        setPixel((x + x_), (y + y_), fillColor_);
      }
    }
  }
}

//--------------------------------------------------------------------------------------------------

void Canvas::bitmap(uint16_t x_,
  uint16_t y_,
  uint16_t w_,
  uint16_t h_,
  const uint8_t* pBitmap_,
  const util::ColorRGB& color_)
{
  if ((x_ >= m_width) || (y_ >= m_height))
  {
    return;
  }

  uint16_t drawableHeight = ((y_ + h_) > m_height) ? (m_height - y_) : h_;
  uint16_t drawableWidth = ((x_ + w_) > m_width) ? (m_width - x_) : w_;

  for (uint8_t j = 0; j < drawableHeight; j++)
  {
    for (uint8_t i = 0; i < drawableWidth; i++)
    {
      if (pBitmap_[(i >> 3) + j * (w_ >> 3)] & (0x01 << (7 - (i & 7))))
      {
        setPixel(x_ + i, y_ + j, color_);
      }
    }
  }
}

//--------------------------------------------------------------------------------------------------

void Canvas::canvas(const Canvas& c_,
  uint16_t xDest_,
  uint16_t yDest_,
  uint16_t xSource_,
  uint16_t ySource_,
  uint16_t w_,
  uint16_t h_)
{
  uint16_t cw = c_.width();
  uint16_t ch = c_.height();
  
  if ((xDest_ >= m_width) || (yDest_ >= m_height) || (xSource_ >= cw) || (ySource_ >= ch))
  {
    return;
  }

  uint16_t width = (w_ <= cw && w_ > 0) ? w_ : cw;
  uint16_t height = (h_ <= ch && h_ > 0) ? h_ : ch;

  uint16_t drawableHeight = ((yDest_ + height) > m_height) ? (m_height - yDest_) : height;
  uint16_t drawableWidth = ((xDest_ + width) > m_width) ? (m_width - xDest_) : width;

  for (uint16_t j = 0; j < drawableHeight; j++)
  {
    for (uint16_t i = 0; i < drawableWidth; i++)
    {
      setPixel(xDest_ + i, yDest_ + j, c_.pixel(xSource_ + i, ySource_ + j));
    }
  }
}

//--------------------------------------------------------------------------------------------------

void Canvas::character(
  uint16_t x_, uint16_t y_, char c_, const util::ColorRGB& color_, const std::string& font_)
{
  const Font* pFont = FontManager::instance().getFont(font_);
  uint8_t c = c_ - pFont->firstChar();

  if ((x_ >= m_width) || (y_ >= m_height) || c > pFont->lastChar() || c_ < pFont->firstChar())
  {
    return;
  }

  for (uint8_t y = 0; y < pFont->height(); y++)
  {
    for (uint8_t x = 0; x < pFont->height(); x++)
    {
      if (pFont->pixel(c, x, y))
      {
        setPixel((x_ + x), y_ + y, color_);
      }
    }
  }
}

//--------------------------------------------------------------------------------------------------

void Canvas::text(uint16_t x_,
  uint16_t y_,
  const char* pStr_,
  const util::ColorRGB& color_,
  const std::string& font_,
  uint8_t spacing_)
{
  const Font* pFont = FontManager::instance().getFont(font_);
  uint8_t charWidth = pFont->charSpacing() + spacing_;
  if (y_ >= m_height || x_ > m_width)
  {
    return;
  }
  for (unsigned i = 0; static_cast<unsigned>(pStr_[i]) != 0; i++)
  {
    if (x_ > (m_width + charWidth))
    {
      return;
    }
    character(x_, y_, pStr_[i], color_, font_);
    x_ += charWidth;
  }
}

//--------------------------------------------------------------------------------------------------

void Canvas::initialize()
{
  m_data.resize(canvasWidthInBytes() * m_height);
  black();
}

//--------------------------------------------------------------------------------------------------

uint16_t Canvas::canvasWidthInBytes() const
{
  return m_width * 3;
}

//--------------------------------------------------------------------------------------------------

} // namespace cabl
} // namespace sl

//--------------------------------------------------------------------------------------------------

#undef M_SWAP
