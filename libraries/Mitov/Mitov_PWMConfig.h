////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2019 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _MITOV_PWM_CONFIG_h
#define _MITOV_PWM_CONFIG_h

#include <Mitov.h>

namespace Mitov
{
	template <
		typename T_Frequency
	> class ArduinoPWMConfig :
		public T_Frequency
	{
	public:
		_V_PROP_( Frequency )

	public:
		inline void SetFrequency( uint32_t AValue )
		{
			if( AValue < 1 )
				AValue = 1;

			else if( AValue > 0xFFFF )
				AValue = 0xFFFF;

			if( AValue == Frequency().GetValue() )
				return;

			Frequency() = AValue;
			analogWriteFreq( AValue );
		}

	public:
		inline void SystemStart()
		{
			analogWriteFreq( Frequency().GetValue() );
		}

	};
}

#endif
