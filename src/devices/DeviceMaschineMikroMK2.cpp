/*----------------------------------------------------------------------------------------------------------------------

                 %%%%%%%%%%%%%%%%%
                 %%%%%%%%%%%%%%%%%
                 %%%           %%%
                 %%%           %%%
                 %%%           %%%
%%%%%%%%%%%%%%%%%%%%           %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%%%%%%           %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% www.shaduzlabs.com %%%%%

------------------------------------------------------------------------------------------------------------------------

  Copyright (C) 2014 Vincenzo Pacella

  This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public
  License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later
  version.

  This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied
  warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

  You should have received a copy of the GNU General Public License along with this program.
  If not, see <http://www.gnu.org/licenses/>.

----------------------------------------------------------------------------------------------------------------------*/

#include "devices/DeviceMaschineMikroMK2.h"
#include "comm/Driver.h"
#include "comm/Transfer.h"
#include "Macros.h"
#include "util/Functions_SL.h"

#include <thread>

#include "gfx/displays/GDisplayMaschineMikro.h"

//\todo delete debug includes
#include <iostream>
#include <iomanip>

//----------------------------------------------------------------------------------------------------------------------

namespace
{

static const uint16_t kMikroMK2_vendorId = 0x17CC;
static const uint16_t kMikroMK2_productId = 0x1200;

static const uint8_t kMikroMK2_endpointDisplay = 0x08;
static const uint8_t kMikroMK2_endpointLeds = 0x01;
static const uint8_t kMikroMK2_endpointInput = 0x84;
}

//----------------------------------------------------------------------------------------------------------------------

namespace sl
{
namespace kio
{

//----------------------------------------------------------------------------------------------------------------------

enum class DeviceMaschineMikroMK2::Led : uint8_t
{
  F1,
  F2,
  F3,
  Control,
  Nav,
  BrowseLeft,
  BrowseRight,
  Main,
  Group,
  GroupR = Group,
  GroupG,
  GroupB,
  Browse,
  Sampling,
  NoteRepeat,
  Restart,
  TransportLeft,
  TransportRight,
  Grid,
  Play,
  Rec,
  Erase,
  Shift,
  Scene,
  Pattern,
  PadMode,
  View,
  Duplicate,
  Select,
  Solo,
  Mute,
  Pad13,
  Pad13R = Pad13,
  Pad13G,
  Pad13B,
  Pad14,
  Pad14R = Pad14,
  Pad14G,
  Pad14B,
  Pad15,
  Pad15R = Pad15,
  Pad15G,
  Pad15B,
  Pad16,
  Pad16R = Pad16,
  Pad16G,
  Pad16B,
  Pad9,
  Pad9R = Pad9,
  Pad9G,
  Pad9B,
  Pad10,
  Pad10R = Pad10,
  Pad10G,
  Pad10B,
  Pad11,
  Pad11R = Pad11,
  Pad11G,
  Pad11B,
  Pad12,
  Pad12R = Pad12,
  Pad12G,
  Pad12H,
  Pad5,
  Pad5R = Pad5,
  Pad5G,
  Pad5B,
  Pad6,
  Pad6R = Pad6,
  Pad6G,
  Pad6B,
  Pad7,
  Pad7R = Pad7,
  Pad7G,
  Pad7B,
  Pad8,
  Pad8R = Pad8,
  Pad8G,
  Pad8B,
  Pad1,
  Pad1R = Pad1,
  Pad1G,
  Pad1B,
  Pad2,
  Pad2R = Pad2,
  Pad2G,
  Pad2B,
  Pad3,
  Pad3R = Pad3,
  Pad3G,
  Pad3B,
  Pad4,
  Pad4R = Pad4,
  Pad4G,
  Pad4B,
  Unknown,
};

//----------------------------------------------------------------------------------------------------------------------


enum class DeviceMaschineMikroMK2::Button : uint8_t
{
  Shift,
  Erase,
  Rec,
  Play,
  Grid,
  TransportRight,
  TransportLeft,
  Restart,

  MainEncoder = 11,
  NoteRepeat,
  Sampling,
  Browse,
  Group,

  Main,
  BrowseRight,
  BrowseLeft,
  Nav,
  Control,
  F3,
  F2,
  F1,

  Mute,
  Solo,
  Select,
  Duplicate,
  View,
  PadMode,
  Pattern,
  Scene,

  Pad13,
  Pad14,
  Pad15,
  Pad16,
  Pad9,
  Pad10,
  Pad11,
  Pad12,
  Pad5,
  Pad6,
  Pad7,
  Pad8,
  Pad1,
  Pad2,
  Pad3,
  Pad4,
  None,
};

//----------------------------------------------------------------------------------------------------------------------

DeviceMaschineMikroMK2::DeviceMaschineMikroMK2()
  : Device(Driver::tDriver::HIDAPI)
  , m_display(new GDisplayMaschineMikro)
  , m_isDirtyLeds(false)
{
 m_buttons.resize(kMikroMK2_buttonsDataSize);
 m_leds.resize(kMikroMK2_ledsDataSize);
}

//----------------------------------------------------------------------------------------------------------------------

DeviceMaschineMikroMK2::~DeviceMaschineMikroMK2()
{
  getDeviceHandle().disconnect();
}


//----------------------------------------------------------------------------------------------------------------------

bool DeviceMaschineMikroMK2::connect()
{
  if (!getDeviceHandle().connect(kMikroMK2_vendorId, kMikroMK2_productId))
  {
    return false;
  }

  init();
  return true;
}

//----------------------------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------------------------

void DeviceMaschineMikroMK2::setLed(Device::Button btn_, uint8_t val_)
{
  Led led = getLed(btn_);

  if (isRGBLed(led))
  {
    uint8_t currentR = m_leds[static_cast<uint16_t>(led)];
    uint8_t currentG = m_leds[static_cast<uint16_t>(led) + 1];
    uint8_t currentB = m_leds[static_cast<uint16_t>(led) + 2];

    m_leds[static_cast<uint16_t>(led)] = val_;
    m_leds[static_cast<uint16_t>(led) + 1] = val_;
    m_leds[static_cast<uint16_t>(led) + 2] = val_;

    m_isDirtyLeds = (currentR != val_ || currentG != val_ || currentB != val_);
  }
  else if (Led::Unknown != led)
  {
    uint8_t currentVal = m_leds[static_cast<uint16_t>(led)];
    uint8_t newVal = val_;

    m_leds[static_cast<uint16_t>(led)] = newVal;
    m_isDirtyLeds = (currentVal != newVal);
  }
}

//----------------------------------------------------------------------------------------------------------------------

void DeviceMaschineMikroMK2::setLed(Device::Button btn_, uint8_t r_, uint8_t g_, uint8_t b_)
{
  Led led = getLed(btn_);

  if (isRGBLed(led))
  {
    uint8_t currentR = m_leds[static_cast<uint16_t>(led)];
    uint8_t currentG = m_leds[static_cast<uint16_t>(led) + 1];
    uint8_t currentB = m_leds[static_cast<uint16_t>(led) + 2];

    m_leds[static_cast<uint16_t>(led)] = r_;
    m_leds[static_cast<uint16_t>(led) + 1] = g_;
    m_leds[static_cast<uint16_t>(led) + 2] = b_;

    m_isDirtyLeds = (currentR != r_ || currentG != g_ || currentB != b_);
  }
  else if (Led::Unknown != led)
  {
    uint8_t currentVal = m_leds[static_cast<uint16_t>(led)];

    // Use "Maximum decomposition" -> take the channel with the highest value
    uint8_t newVal = util::max<uint8_t>(r_, g_, b_);

    m_leds[static_cast<uint16_t>(led)] = newVal;
    m_isDirtyLeds = (currentVal != newVal);
  }
}

//----------------------------------------------------------------------------------------------------------------------

GDisplay* DeviceMaschineMikroMK2::getDisplay(uint8_t displayIndex_)
{
  if (displayIndex_ > 0)
  {
    return nullptr;
  }

  return m_display.get();
}

//----------------------------------------------------------------------------------------------------------------------

bool DeviceMaschineMikroMK2::tick()
{
  static int state = 0;
  bool success = false;

  //\todo enable once display dirty flag is properly set
  if (state == 0) //&& m_display->isDirty())
  {
    success = sendFrame();
  }

  else if (state == 1)
  {
    success = sendLeds();
  }
  else if (state == 2)
  {
    success = read();
  }

  if (++state >= 3)
  {
    state = 0;
  }

  return success;
}

//----------------------------------------------------------------------------------------------------------------------

void DeviceMaschineMikroMK2::init()
{
  // Display
  initDisplay();
  m_display.get()->white();

  // Leds
  m_isDirtyLeds = true;
}

//----------------------------------------------------------------------------------------------------------------------

void DeviceMaschineMikroMK2::initDisplay() const
{
  //\todo set backlight
  return;
}

//----------------------------------------------------------------------------------------------------------------------

bool DeviceMaschineMikroMK2::sendFrame()
{
  uint8_t yOffset = 0;
  for (int chunk = 0; chunk < 4; chunk++, yOffset += 2)
  {
    const uint8_t* ptr = m_display->getPtr(chunk * 256);
    if(!getDeviceHandle().write(Transfer({0xE0, 0x00, 0x00, yOffset, 0x00, 0x80, 0x00, 0x02, 0x00}, ptr, 256),
                   kMikroMK2_endpointDisplay))
    {
      return false;
    }
  }
  return true;
}

//----------------------------------------------------------------------------------------------------------------------

bool DeviceMaschineMikroMK2::sendLeds()
{
//  if (m_isDirtyLeds)
  {
    if(!getDeviceHandle().write(Transfer({0x80}, &m_leds[0], 78), kMikroMK2_endpointLeds))
    {
      return false;
    }
    m_isDirtyLeds = false;
  }
  return true;
}

//----------------------------------------------------------------------------------------------------------------------

bool DeviceMaschineMikroMK2::read()
{
  Transfer input;
  for (uint8_t n = 0; n < 32; n++)
  {
    if (!getDeviceHandle().read(input, kMikroMK2_endpointInput))
    {
      return false;
    }
    else if (input && input[0] == 0x01)
    {
      processButtons(input);
      break;
    }
    else if (input && input[0] == 0x20 && n % 8 == 0) // Too many pad messages, need to skip some...
    {
      processPads(input);
    }
/*
        std::cout << std::setfill('0') << std::internal;

        for( int i = 0; i < input.getSize(); i++ )
        {
          std::cout << std::hex << std::setw(2) << (int)input[i] <<  std::dec << " " ;
        }

        std::cout << std::endl << std::endl;*/
  }
  return true;
}

//----------------------------------------------------------------------------------------------------------------------

void DeviceMaschineMikroMK2::processButtons(const Transfer& input_)
{
  bool shiftPressed(isButtonPressed(input_, Button::Shift));
  Device::Button changedButton(Device::Button::Unknown);
  bool buttonPressed(false);

  for (int i = 0; i < kMikroMK2_buttonsDataSize - 1; i++) // Skip the last byte (encoder value)
  {
    for (int k = 0; k < 8; k++)
    {
      uint8_t btn = (i * 8) + k;
      Button currentButton(static_cast<Button>(btn));
      if (currentButton == Button::Shift)
      {
        continue;
      }
      buttonPressed = isButtonPressed(input_, currentButton);
      if (buttonPressed != m_buttonStates[btn])
      {
        m_buttonStates[btn] = buttonPressed;
        changedButton = getDeviceButton(currentButton);
        if (changedButton != Device::Button::Unknown)
        {
      //    std::copy(&input_[1],&input_[kMikroMK2_buttonsDataSize],m_buttons.begin());
          buttonChanged(changedButton, buttonPressed, shiftPressed);
          }
        }
      }

    // Now process the encoder data
    uint8_t currentEncoderValue = input_.getData()[kMikroMK2_buttonsDataSize];
    if (m_encoderValue != currentEncoderValue)
    {
      bool valueIncreased
        = ((m_encoderValue < currentEncoderValue) || ((m_encoderValue == 0x0f) && (currentEncoderValue == 0x00)))
          && (!((m_encoderValue == 0x0) && (currentEncoderValue == 0x0f)));
        encoderChanged(0, valueIncreased, shiftPressed);
      m_encoderValue = currentEncoderValue;
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------

void DeviceMaschineMikroMK2::processPads(const Transfer& input_)
{
  //\todo process pad data
  for (int i = 1; i <= kMikroMK2_padDataSize; i += 2)
  {
    uint16_t l = input_[i];
    uint16_t h = input_[i + 1];
    uint8_t pad = (h & 0xF0) >> 4;
    m_padsRawData[pad].write(((h & 0x0F) << 8) | l);
    m_padsAvgData[pad] = (((h & 0x0F) << 8) | l);

    Device::Button btn(Device::Button::Unknown);

#define M_PAD_CASE(value, pad) \
  case value:                  \
    btn = Device::Button::pad; \
    break

    switch (pad)
    {
      M_PAD_CASE(0, Pad13);
      M_PAD_CASE(1, Pad14);
      M_PAD_CASE(2, Pad15);
      M_PAD_CASE(3, Pad16);
      M_PAD_CASE(4, Pad9);
      M_PAD_CASE(5, Pad10);
      M_PAD_CASE(6, Pad11);
      M_PAD_CASE(7, Pad12);
      M_PAD_CASE(8, Pad5);
      M_PAD_CASE(9, Pad6);
      M_PAD_CASE(10, Pad7);
      M_PAD_CASE(11, Pad8);
      M_PAD_CASE(12, Pad1);
      M_PAD_CASE(13, Pad2);
      M_PAD_CASE(14, Pad3);
      M_PAD_CASE(15, Pad4);
    }

#undef M_PAD_CASE

    if (m_padsAvgData[pad] > 1000)
    {
      padChanged(btn, m_padsAvgData[pad],isButtonPressed(input_, Button::Shift));
    }    
  }
}

//----------------------------------------------------------------------------------------------------------------------

bool DeviceMaschineMikroMK2::isRGBLed(Led led_)
{
  if (Led::Group == led_ || Led::Pad1 == led_ || Led::Pad2 == led_ || Led::Pad3 == led_ || Led::Pad4 == led_
      || Led::Pad5 == led_ || Led::Pad6 == led_ || Led::Pad7 == led_ || Led::Pad8 == led_ || Led::Pad9 == led_
      || Led::Pad10 == led_ || Led::Pad11 == led_ || Led::Pad12 == led_ || Led::Pad13 == led_ || Led::Pad14 == led_
      || Led::Pad15 == led_ || Led::Pad16 == led_)
  {
    return true;
  }

  return false;
}

//----------------------------------------------------------------------------------------------------------------------

DeviceMaschineMikroMK2::Led DeviceMaschineMikroMK2::getLed(Device::Button btn_) const noexcept
{
#define M_LED_CASE(idLed)     \
  case Device::Button::idLed: \
    return Led::idLed

  switch (btn_)
  {
    M_LED_CASE(F1);
    M_LED_CASE(F2);
    M_LED_CASE(F3);
    M_LED_CASE(Control);
    M_LED_CASE(Nav);
    M_LED_CASE(BrowseLeft);
    M_LED_CASE(BrowseRight);
    M_LED_CASE(Main);
    M_LED_CASE(Group);
    M_LED_CASE(Browse);
    M_LED_CASE(Sampling);
    M_LED_CASE(NoteRepeat);
    M_LED_CASE(Restart);
    M_LED_CASE(TransportLeft);
    M_LED_CASE(TransportRight);
    M_LED_CASE(Grid);
    M_LED_CASE(Play);
    M_LED_CASE(Rec);
    M_LED_CASE(Erase);
    M_LED_CASE(Shift);
    M_LED_CASE(Scene);
    M_LED_CASE(Pattern);
    M_LED_CASE(PadMode);
    M_LED_CASE(View);
    M_LED_CASE(Duplicate);
    M_LED_CASE(Select);
    M_LED_CASE(Solo);
    M_LED_CASE(Mute);
    M_LED_CASE(Pad1);
    M_LED_CASE(Pad2);
    M_LED_CASE(Pad3);
    M_LED_CASE(Pad4);
    M_LED_CASE(Pad5);
    M_LED_CASE(Pad6);
    M_LED_CASE(Pad7);
    M_LED_CASE(Pad8);
    M_LED_CASE(Pad9);
    M_LED_CASE(Pad10);
    M_LED_CASE(Pad11);
    M_LED_CASE(Pad12);
    M_LED_CASE(Pad13);
    M_LED_CASE(Pad14);
    M_LED_CASE(Pad15);
    M_LED_CASE(Pad16);

    default:
    {
      return Led::Unknown;
    }
  }

#undef M_LED_CASE
}

//----------------------------------------------------------------------------------------------------------------------

Device::Button DeviceMaschineMikroMK2::getDeviceButton(Button btn_) const noexcept
{
#define M_BTN_CASE(idBtn) \
  case Button::idBtn:     \
    return Device::Button::idBtn

  switch (btn_)
  {
    M_BTN_CASE(F1);
    M_BTN_CASE(F2);
    M_BTN_CASE(F3);
    M_BTN_CASE(Control);
    M_BTN_CASE(Nav);
    M_BTN_CASE(BrowseLeft);
    M_BTN_CASE(BrowseRight);
    M_BTN_CASE(Main);
    M_BTN_CASE(Group);
    M_BTN_CASE(Browse);
    M_BTN_CASE(Sampling);
    M_BTN_CASE(NoteRepeat);
    M_BTN_CASE(Restart);
    M_BTN_CASE(TransportLeft);
    M_BTN_CASE(TransportRight);
    M_BTN_CASE(Grid);
    M_BTN_CASE(Play);
    M_BTN_CASE(Rec);
    M_BTN_CASE(Erase);
    M_BTN_CASE(Shift);
    M_BTN_CASE(Scene);
    M_BTN_CASE(Pattern);
    M_BTN_CASE(PadMode);
    M_BTN_CASE(View);
    M_BTN_CASE(Duplicate);
    M_BTN_CASE(Select);
    M_BTN_CASE(Solo);
    M_BTN_CASE(Mute);
    M_BTN_CASE(MainEncoder);
    M_BTN_CASE(Pad1);
    M_BTN_CASE(Pad2);
    M_BTN_CASE(Pad3);
    M_BTN_CASE(Pad4);
    M_BTN_CASE(Pad5);
    M_BTN_CASE(Pad6);
    M_BTN_CASE(Pad7);
    M_BTN_CASE(Pad8);
    M_BTN_CASE(Pad9);
    M_BTN_CASE(Pad10);
    M_BTN_CASE(Pad11);
    M_BTN_CASE(Pad12);
    M_BTN_CASE(Pad13);
    M_BTN_CASE(Pad14);
    M_BTN_CASE(Pad15);
    M_BTN_CASE(Pad16);

    default:
    {
      return Device::Button::Unknown;
    }
  }

#undef M_LED_CASE
}

//----------------------------------------------------------------------------------------------------------------------

bool DeviceMaschineMikroMK2::isButtonPressed(Button button_) const noexcept
{
  uint8_t buttonPos = static_cast<uint8_t>(button_);
  return ((m_buttons[buttonPos >> 3] & (1 << (buttonPos % 8))) != 0);
}

//----------------------------------------------------------------------------------------------------------------------

bool DeviceMaschineMikroMK2::isButtonPressed(const Transfer& transfer_, Button button_) const noexcept
{
  uint8_t buttonPos = static_cast<uint8_t>(button_);
  return ((transfer_[1 + (buttonPos >> 3)] & (1 << (buttonPos % 8))) != 0);
}

//----------------------------------------------------------------------------------------------------------------------

} // kio
} // sl
