////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2019 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _MITOV_MAXIM_THERMOCOUPLE_h
#define _MITOV_MAXIM_THERMOCOUPLE_h

#include <Mitov.h>

#ifdef __TEST_FOR_DEBUG_PRINTS__
#define Serial UNGUARDED DEBUG PRINT!!!
#endif

namespace Mitov
{
//---------------------------------------------------------------------------
/*
	class NotConnectedPinBasicImplementation
	{
	public:
		bool	Enabled : 1;
		bool	InFahrenheit : 1;

	protected:
		bool	FDataSent : 1;
		bool	FNotConnected : 1;
		bool	FColdJunction : 1;
		bool	FShortToGroundOutputPin : 1;

	public:
		NotConnectedPinBasicImplementation() :
			Enabled( true ),
			InFahrenheit( false ),
			FDataSent( false ),
			FNotConnected( false ),
			FColdJunction( false ),
			FShortToGroundOutputPin( false )
		{
		}

	};
//---------------------------------------------------------------------------
	class NotConnectedPinImplementation : public NotConnectedPinBasicImplementation
	{
	public:
		OpenWire::SourcePin	NotConnectedOutputPin ;

	protected:
		inline void SetNonConnected( bool AValue, bool AChangeOnly )
		{
			if( AChangeOnly )
				if( FNotConnected == AValue )
					return;

			FNotConnected = AValue;
			NotConnectedOutputPin.Notify( &AValue );
		}

	};
//---------------------------------------------------------------------------
	class NotConnectedPinNullImplementation : public NotConnectedPinBasicImplementation
	{
	protected:
		inline void SetNonConnected( bool AValue, bool AChangeOnly ) {}

	};
//---------------------------------------------------------------------------
	template<typename T_BASE> class ColdJunctionOutputPinImplementation : public T_BASE
	{
	public:
		OpenWire::TypedSourcePin<float>	ColdJunctionOutputPin ;

	protected:
		inline void SetColdJunction( float AValue, bool AChangeOnly )
		{
			ColdJunctionOutputPin.SetValue( AValue, AChangeOnly );
		}

	};
//---------------------------------------------------------------------------
	template<typename T_BASE> class ColdJunctionOutputPinNullImplementation : public T_BASE
	{
	protected:
		inline void SetColdJunction( bool AValue, bool AChangeOnly ) {}

	};
//---------------------------------------------------------------------------
	template<typename T_BASE> class ShortToPowerOutputPinImplementation : public T_BASE
	{
	public:
		OpenWire::SourcePin	ShortToPowerOutputPin ;

	protected:
		inline void SetShortToPower( bool AValue, bool AChangeOnly )
		{
			if( AChangeOnly )
				if( T_BASE::FShortToGround == AValue )
					return;

			T_BASE::FShortToGround = AValue;
			ShortToPowerOutputPin.Notify( &AValue );
		}

	};
//---------------------------------------------------------------------------
	template<typename T_BASE> class ShortToPowerOutputPinNullImplementation : public T_BASE
	{
	protected:
		inline void SetShortToPower( bool AValue, bool AChangeOnly ) {}

	};
//---------------------------------------------------------------------------
	template<typename T_BASE> class ShortToGroundOutputPinImplementation : public T_BASE
	{
	public:
		OpenWire::SourcePin	ShortToGroundOutputPin ;

	protected:
		inline void SetShortToGround( bool AValue, bool AChangeOnly )
		{
			if( AChangeOnly )
				if( T_BASE::FShortToGround == AValue )
					return;

			T_BASE::FShortToGround = AValue;
			ShortToGroundOutputPin.Notify( &AValue );
		}

	};
//---------------------------------------------------------------------------
	template<typename T_BASE> class ShortToGroundOutputPinNullImplementation : public T_BASE
	{
	protected:
		inline void SetShortToGround( bool AValue, bool AChangeOnly ) {}

	};
*/
//---------------------------------------------------------------------------
	template<
		typename T_SPI, T_SPI &C_SPI,
		typename T_ChipSelectOutputPin,
		typename T_Enabled,
		typename T_FDataSent,
		typename T_InFahrenheit,
		typename T_NotConnectedOutputPin,
		typename T_OutputPin
	> class ThermocoupleMAX6675 :
		public T_ChipSelectOutputPin,
		public T_Enabled,
		public T_FDataSent,
		public T_InFahrenheit,
		public T_NotConnectedOutputPin,
		public T_OutputPin
	{
	public:
		_V_PIN_( OutputPin )
		_V_PIN_( ChipSelectOutputPin )
		_V_PIN_( NotConnectedOutputPin )

	public:
		_V_PROP_( Enabled )
		_V_PROP_( InFahrenheit )

	protected:
		_V_PROP_( FDataSent )

	protected:
		void ReadSensor( bool AChangeOnly )
		{
			if( ! Enabled() )
				return;

			T_ChipSelectOutputPin::SetPinValueLow();

			C_SPI.beginTransaction(SPISettings(4000000, MSBFIRST, SPI_MODE0));

			uint16_t AValue = uint16_t( C_SPI.transfer(0) ) << 8;
			AValue |= C_SPI.transfer(0);

			C_SPI.endTransaction();

			T_ChipSelectOutputPin::SetPinValueHigh();

			float ATemperature;
			if ( AValue & 0b100 )
			{
				ATemperature = 0;
				T_NotConnectedOutputPin::SetPinValue( true, AChangeOnly );
			}

			else
			{
				T_NotConnectedOutputPin::SetPinValue( false, AChangeOnly );
				AValue >>= 3;
				if( InFahrenheit() )
					ATemperature = AValue * 0.25 * 9.0 / 5.0 + 32;

				else
					ATemperature = AValue * 0.25;

			}

			T_OutputPin::SetPinValue( ATemperature, AChangeOnly );
		}

	public:
		inline void ClockInputPin_o_Receive( void *_Data )
		{
			ReadSensor( false );
		}

	public:
		inline void SystemLoopBegin()
		{
			ReadSensor( FDataSent() );
			FDataSent() = true;
		}

	public:
		inline ThermocoupleMAX6675()
		{
			FDataSent() = false;
		}
	};
//---------------------------------------------------------------------------
	template<
		typename T_SPI, T_SPI &C_SPI,
		typename T_ChipSelectOutputPin,
		typename T_ColdJunctionOutputPin,
		typename T_Enabled,
		typename T_FDataSent,
		typename T_InFahrenheit,
		typename T_NotConnectedOutputPin,
		typename T_OutputPin,
		typename T_ShortToGroundOutputPin,
		typename T_ShortToPowerOutputPin
	> class ThermocoupleMAX31855 : 
		public T_ChipSelectOutputPin,
		public T_ColdJunctionOutputPin,
		public T_Enabled,
		public T_FDataSent,
		public T_InFahrenheit,
		public T_NotConnectedOutputPin,
		public T_OutputPin,
		public T_ShortToGroundOutputPin,
		public T_ShortToPowerOutputPin
	{
	public:
		_V_PIN_( OutputPin )
		_V_PIN_( ChipSelectOutputPin )
		_V_PIN_( ColdJunctionOutputPin )
		_V_PIN_( NotConnectedOutputPin )
		_V_PIN_( ShortToGroundOutputPin )
		_V_PIN_( ShortToPowerOutputPin )

	public:
		_V_PROP_( Enabled )
		_V_PROP_( InFahrenheit )

	protected:
		_V_PROP_( FDataSent )

	protected:
		void ReadSensor( bool AChangeOnly )
		{
			if( ! Enabled() )
				return;

			T_ChipSelectOutputPin::SetPinValueLow();

			C_SPI.beginTransaction(SPISettings(4000000, MSBFIRST, SPI_MODE0));

			uint16_t Avalue0 = uint16_t( C_SPI.transfer(0) ) << 8;
			Avalue0 |= uint32_t( C_SPI.transfer(0) );

			uint16_t Avalue1 = uint16_t( C_SPI.transfer(0) ) << 8;
			Avalue1 |= C_SPI.transfer(0);

			C_SPI.endTransaction();
			T_ChipSelectOutputPin::SetPinValueHigh();
			T_NotConnectedOutputPin::SetPinValue( ( Avalue1 & 0b001 ), AChangeOnly );
			T_ShortToGroundOutputPin::SetPinValue( ( Avalue1 & 0b010 ), AChangeOnly );
			T_ShortToPowerOutputPin::SetPinValue( ( Avalue1 & 0b100 ), AChangeOnly );
			float ATemperature;
			float AColdJunctionTemperature;
			if ( Avalue1 & 0b111 )
			{
				ATemperature = 0;
				AColdJunctionTemperature = 0;
			}

			else
			{
				if( InFahrenheit() )
				{
					ATemperature = ( Avalue0 & 0b1111111111111100 ) * ( 0.25 / 4 ) * 9.0 / 5.0 + 32; 
					AColdJunctionTemperature = ( Avalue1 & 0b1111111111110000 ) * ( 0.0625 / 16 ) * 9.0 / 5.0 + 32;
				}

				else
				{
					ATemperature = ( Avalue0 & 0b1111111111111100 ) * ( 0.25 / 4 );
					AColdJunctionTemperature = ( Avalue1 & 0b1111111111110000 ) * ( 0.0625 / 16 );
				}
			}

			T_OutputPin::SetPinValue( ATemperature, AChangeOnly );
			T_ColdJunctionOutputPin::SetPinValue( AColdJunctionTemperature, AChangeOnly );
		}

	public:
		inline void ClockInputPin_o_Receive( void *_Data )
		{
			ReadSensor( false );
		}

	public:
		inline void SystemLoopBegin()
		{
			ReadSensor( FDataSent() );
			FDataSent() = true;
		}

	public:
		inline ThermocoupleMAX31855()
		{
			FDataSent() = false;
		}
	};
//---------------------------------------------------------------------------
}

#ifdef __TEST_FOR_DEBUG_PRINTS__
#undef Serial
#endif

#endif
