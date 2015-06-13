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

#include "comm/Driver.h"
#include "comm/DriverImpl.h"
#include "comm/DeviceHandle.h"

#include "comm/drivers/DriverMOCK.h"

#if defined (__SAM3X8E__)
#include "comm/drivers/DriverSAM3X8E.h"
#elif defined (__MAX3421E__)
#include "comm/drivers/DriverMAX3421E.h"
#else
#include "comm/drivers/DriverHIDAPI.h"
#include "comm/drivers/DriverLIBUSB.h"
#endif

namespace sl
{
namespace kio
{

Driver::Driver( tDriver type_ )
{
  switch( type_ )
  {
#if defined (__SAM3X8E__)
    case tDriver::SAM3X8E:
      m_pImpl.reset( new DriverSAM3X8E );
      break;
#elif defined (__MAX3421E__)
    case tDriver::MAX3421E:
      m_pImpl.reset( new DriverMAX3421E );
      break;
#else
    case tDriver::HIDAPI:
      m_pImpl.reset( new DriverHIDAPI );
      break;
    case tDriver::LIBUSB:
      m_pImpl.reset( new DriverLIBUSB );
      break;
#endif
    case tDriver::MOCK:
    default:
      m_pImpl.reset( new DriverMOCK );
      break;
    }
}
  
//----------------------------------------------------------------------------------------------------------------------
  
Driver::~Driver()
{

}
  
//----------------------------------------------------------------------------------------------------------------------
  
tPtr<DeviceHandle> Driver::connect( tVendorId vid_, tProductId pid_  )
{
  return tPtr<DeviceHandle>( new DeviceHandle(m_pImpl->connect( vid_,pid_ )));
}

//----------------------------------------------------------------------------------------------------------------------

} // kio
} // sl
