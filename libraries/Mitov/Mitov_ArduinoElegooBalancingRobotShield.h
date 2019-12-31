////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2019 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _MITOV_ELEGOO_BALANCING_ROBOT_SHIELD_h
#define _MITOV_ELEGOO_BALANCING_ROBOT_SHIELD_h

#include <Mitov.h>

#ifdef __TEST_FOR_DEBUG_PRINTS__
#define Serial UNGUARDED DEBUG PRINT!!!
#endif

namespace Mitov
{
//---------------------------------------------------------------------------
	template <
		typename T_Enabled
	> class ArduinoElegooBalancingRobotShieldMotorsStandby :
		public T_Enabled
	{
	public:
		_V_PROP_( Enabled )

	public:
		inline void UpdateEnabled()
		{
			digitalWrite( 8, Enabled() ? HIGH : LOW );
		}

	public:
		inline void SystemStart()
		{
			UpdateEnabled();
		}

	};
//---------------------------------------------------------------------------
}

#ifdef __TEST_FOR_DEBUG_PRINTS__
#undef Serial
#endif

#endif // _MITOV_ELEGOO_BALANCING_ROBOT_SHIELD_h
