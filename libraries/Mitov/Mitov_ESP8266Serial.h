////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2019 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _MITOV_EPS8266_SERIAL_h
#define _MITOV_EPS8266_SERIAL_h

#include <Mitov.h>

namespace Mitov
{
	template<
		typename T_SERIAL_TYPE, T_SERIAL_TYPE &C_SERIAL,
		typename T_AfterSendingDelay,
		typename T_DataBits,
		typename T_Enabled,
		typename T_FEndTime,
		typename T_FSending,
		typename T_OutputPin,
		typename T_Parity,
		typename T_ReceiveEnabled,
		typename T_SendingOutputPin,
		typename T_Speed,
		typename T_StopBits,
		typename T_TransmitEnabled,
		typename T_UseAlternativePins
	> class SerialESP8266Port : public BasicSerialWrapper<T_SERIAL_TYPE, C_SERIAL, T_Enabled, T_OutputPin>,
		public T_AfterSendingDelay,
		public T_DataBits,
		public T_FEndTime,
		public T_FSending,
		public T_Parity,
		public T_ReceiveEnabled,
		public T_SendingOutputPin,
		public T_Speed,
		public T_StopBits,
		public T_TransmitEnabled,
		public T_UseAlternativePins
	{
		typedef BasicSerialWrapper<T_SERIAL_TYPE, C_SERIAL, T_Enabled, T_OutputPin> inherited;

	public:
		_V_PIN_( SendingOutputPin )

	public:
		_V_PROP_( Speed )

	public:
//		_V_PROP_( Enabled )
		_V_PROP_( AfterSendingDelay )
		_V_PROP_( ReceiveEnabled )
		_V_PROP_( TransmitEnabled )
		_V_PROP_( UseAlternativePins )

	protected:
		_V_PROP_( FSending )

	public:
//#ifndef VISUINO_CHIPKIT
		_V_PROP_( Parity )
		_V_PROP_( StopBits )
		_V_PROP_( DataBits )
//#endif // VISUINO_CHIPKIT

#ifdef _MITOV_SERIAL_SENDING_PIN_
		uint32_t GetByteSpeed()
		{ 
			return Speed().GetValue() / ( StopBits().GetValue() + DataBits().GetValue() + 1 ); 
		}
#endif

	public:
		void ResetSerial()
		{
			C_SERIAL.end();
			StartPort();
		}

	protected:
		inline void SerialEnd() {} // Placeholder

		void StartPort()
		{
			int AIndex = ((int)Parity()) * 8 + ( StopBits() - 1 ) * 4 + ( DataBits() - 5);

			if( ReceiveEnabled().GetValue() && TransmitEnabled().GetValue() )
				C_SERIAL.begin( Speed(), CSerialInits[ AIndex ], SERIAL_FULL );

			else if( ReceiveEnabled() )
				C_SERIAL.begin( Speed(), CSerialInits[ AIndex ], SERIAL_RX_ONLY );

			else if( TransmitEnabled() )
				C_SERIAL.begin( Speed(), CSerialInits[ AIndex ], SERIAL_TX_ONLY );

			if( UseAlternativePins() )
				C_SERIAL.swap();

		}


/*
#ifdef _MITOV_SERIAL_SENDING_PIN_
		virtual uint32_t GetByteSpeed()
		{ 
			return 9600 / 11;
		}
#endif
*/
	protected:
		_V_PROP_( FEndTime )
//#ifdef _MITOV_SERIAL_SENDING_PIN_
//		uint32_t	FEndTime;
//#endif

	protected:
//	#if ( defined( VISUINO_CONTROLLINO_MAXI ) || defined( VISUINO_CONTROLLINO_MEGA ) || defined( VISUINO_M_DUINO_ETHERNET_58 ))
//		virtual
//	#endif
		inline void UpdateSending( bool ASending )
		{
#ifdef _MITOV_SERIAL_SENDING_PIN_
			T_SendingOutputPin::SetPinValue( ASending );
#endif // _MITOV_SERIAL_SENDING_PIN_
		}

#ifdef _MITOV_SERIAL_SENDING_PIN_
		void SendingCount( uint32_t ACount )
		{
//			Serial.println( ACount );

			ACount = 1; // Most conrtrollers can send only one character at a time.
#ifdef __AVR
			ACount = 1; // The AVR can send only one character at a time.
#endif
			if( ! FSending() )
			{
				FSending() = true;
				UpdateSending( FSending() );
				uint32_t ATime = micros();
				FEndTime() = ATime + AfterSendingDelay() + ACount * ( 10000000 / GetByteSpeed() );
//				Serial.println( 10000000 / GetByteSpeed() );
//				Serial.println( int32_t( FEndTime - ATime ));
			}

			else
			{
				UpdateSending( FSending() );
				uint32_t ATime = micros();
				if( int32_t( FEndTime().GetValue() - ATime ) >= AfterSendingDelay() )
					FEndTime() = FEndTime().GetValue() + ACount * ( 10000000 / GetByteSpeed() );

				else
					FEndTime() = ATime + AfterSendingDelay() + ACount * ( 10000000 / GetByteSpeed() );

//					Serial.println( 10000000 / GetByteSpeed() );
//					Serial.println( int32_t( FEndTime - ATime ));
			}

//			Serial.println( 10000000 / GetByteSpeed() );
//			Serial.println( int32_t( FEndTime - ATime ));
		}
#endif

	public:
		template<typename T> void Print( T AValue )
		{
			if( inherited::Enabled() )
			{
#ifdef _MITOV_SERIAL_SENDING_PIN_
				SendingCount( 1 );
#endif
				C_SERIAL.println( AValue );
#ifdef __AVR
//				SendingCount( 1 );
#endif
			}
		}

#ifdef _MITOV_SERIAL_SENDING_PIN_
		void Print( Mitov::String AValue )
		{
			if( inherited::Enabled() )
			{
#ifdef _MITOV_SERIAL_SENDING_PIN_
				SendingCount( AValue.length() );
#endif
				C_SERIAL.println( AValue );
#ifdef _MITOV_SERIAL_SENDING_PIN_
#ifdef __AVR
//				SendingCount( 1 );
#endif
#endif
			}
		}

		void Print( char *AValue )
		{
			if( inherited::Enabled() )
			{
#ifdef _MITOV_SERIAL_SENDING_PIN_
				SendingCount( strlen( AValue ) );
#endif
				C_SERIAL.println( AValue );
#ifdef _MITOV_SERIAL_SENDING_PIN_
#ifdef __AVR
//				SendingCount( 1 );
#endif
#endif
			}
		}
#endif

		void PrintChar( char AValue )
		{
			if( inherited::Enabled() )
			{
#ifdef _MITOV_SERIAL_SENDING_PIN_
				SendingCount( 1 );
#endif
				C_SERIAL.print( AValue );
			}
		}

		void Write( uint8_t *AData, uint32_t ASize )
		{
#ifdef _MITOV_SERIAL_SENDING_PIN_
			SendingCount( ASize );
#endif
			C_SERIAL.write( AData, ASize );
#ifdef _MITOV_SERIAL_SENDING_PIN_
#ifdef __AVR
//			SendingCount( 1 );
#endif
#endif
		}

	public:
		inline void SystemLoopBegin()
		{
#ifdef _MITOV_SERIAL_SENDING_PIN_
			if( FSending() )
			{
				unsigned long ACurrentMicros = micros();
				if( int32_t( FEndTime().GetValue() - ACurrentMicros ) < 0 )
				{
//					Serial.print( "STOP: " );
//					Serial.println( int32_t( FEndTime - ACurrentMicros ) );
					FSending() = false;
					UpdateSending( FSending() );
				}
			}

#endif
		}

		inline void SystemInit()
		{
			if( inherited::Enabled() )
				StartPort();

		}

	public:
        void RestartPort() // Used by setters
		{
			if( ! inherited::Enabled() )
				return;

			C_SERIAL.end();
			StartPort();
		}

	public:
		void SetEnabled( bool AValue )
		{
            if( inherited::Enabled() == AValue )
                return;

            inherited::Enabled() = AValue;
			if( inherited::Enabled() )
				StartPort();

			else
			{
				C_SERIAL.end();
				SerialEnd();
			}
		}

	public:
		inline SerialESP8266Port()
		{
#ifdef _MITOV_SERIAL_SENDING_PIN_
			FSending() = false;
#endif
		}

	};
//---------------------------------------------------------------------------
	template<
		typename T_SERIAL_TYPE, T_SERIAL_TYPE &C_SERIAL,
		typename T_AfterSendingDelay,
		typename T_DataBits,
		typename T_Enabled,
		typename T_FEndTime,
		typename T_FSending,
		typename T_Parity,
		typename T_SendingOutputPin,
		typename T_Speed,
		typename T_StopBits
	> class SerialESP8266PortSendOnly : // public BasicSerialWrapper<T_SERIAL_TYPE, C_SERIAL, T_Enabled, T_OutputPin>,
		public T_AfterSendingDelay,
		public T_DataBits,
		public T_Enabled,
		public T_FEndTime,
		public T_FSending,
		public T_Parity,
		public T_SendingOutputPin,
		public T_Speed,
		public T_StopBits
	{
//		typedef BasicSerialWrapper<T_SERIAL_TYPE, C_SERIAL, T_Enabled, T_OutputPin> inherited;

	public:
		_V_PIN_( SendingOutputPin )

	public:
		_V_PROP_( Enabled )
		_V_PROP_( Speed )

	public:
		_V_PROP_( AfterSendingDelay )

	protected:
		_V_PROP_( FSending )

	public:
//#ifndef VISUINO_CHIPKIT
		_V_PROP_( Parity )
		_V_PROP_( StopBits )
		_V_PROP_( DataBits )
//#endif // VISUINO_CHIPKIT

/*
#ifdef VISUINO_ESP8266
		bool	ReceiveEnabled : 1;
		bool	TransmitEnabled : 1;
		bool	UseAlternativePins : 1;

#endif // VISUINO_ESP8266
*/
	protected:
		_V_PROP_( FEndTime )
//#ifdef _MITOV_SERIAL_SENDING_PIN_
//		uint32_t	FEndTime;
//#endif

	public:
		inline Stream &GetStream()
		{
			return C_SERIAL;
		}

#ifdef _MITOV_SERIAL_SENDING_PIN_
	#ifndef VISUINO_CHIPKIT
		uint32_t GetByteSpeed()
		{ 
			return Speed().GetValue() / ( StopBits().GetValue() + DataBits().GetValue() + 1 ); 
		}
	#endif // VISUINO_CHIPKIT
#endif

	public:
		void ResetSerial()
		{
			C_SERIAL.end();
			StartPort();
		}

	protected:
		inline void SerialEnd() {} // Placeholder

		void StartPort()
		{
#if ( defined( VISUINO_CHIPKIT ) || defined( VISUINO_STMDUINO ) || defined( VISUINO_INTEL_EDISON ) || defined( VISUINO_DIGISPARK_PRO ) || defined( VISUINO_DIGISPARK ) || defined( NO_SERIAL_FORMAT ) )
			C_SERIAL.begin( Speed() );

#else // VISUINO_CHIPKIT

			int AIndex = ((int)Parity()) * 8 + ( StopBits() - 1 ) * 4 + ( DataBits() - 5);
			C_SERIAL.begin( Speed(), CSerialInits[ AIndex ] );
/*
#ifdef VISUINO_ESP8266
			if( ReceiveEnabled && TransmitEnabled )
				C_SERIAL.begin( Speed, CSerialInits[ AIndex ], SERIAL_FULL );

			else if( ReceiveEnabled )
				C_SERIAL.begin( Speed, CSerialInits[ AIndex ], SERIAL_RX_ONLY );

			else if( TransmitEnabled )
				C_SERIAL.begin( Speed, CSerialInits[ AIndex ], SERIAL_TX_ONLY );

			if( UseAlternativePins )
				C_SERIAL.swap();

#else // VISUINO_ESP8266
*/
#ifdef VISUINO_MACCHINA_M2
			
			C_SERIAL.begin( Speed(), UARTClass::UARTModes( CSerialInits[ AIndex ] ));

#else
#endif // VISUINO_MACCHINA_M2
//#endif // VISUINO_ESP8266

#endif // VISUINO_CHIPKIT
		}


/*
#ifdef _MITOV_SERIAL_SENDING_PIN_
		virtual uint32_t GetByteSpeed()
		{ 
			return 9600 / 11;
		}
#endif
*/
	protected:
//	#if ( defined( VISUINO_CONTROLLINO_MAXI ) || defined( VISUINO_CONTROLLINO_MEGA ) || defined( VISUINO_M_DUINO_ETHERNET_58 ))
//		virtual
//	#endif
		inline void UpdateSending( bool ASending )
		{
#ifdef _MITOV_SERIAL_SENDING_PIN_
//			T_PARENT::UpdateSending( ASending );
			T_SendingOutputPin::SetPinValue( ASending );
#endif // _MITOV_SERIAL_SENDING_PIN_
		}

#ifdef _MITOV_SERIAL_SENDING_PIN_
		void SendingCount( uint32_t ACount )
		{
//			Serial.println( ACount );

			ACount = 1; // Most conrtrollers can send only one character at a time.
#ifdef __AVR
			ACount = 1; // The AVR can send only one character at a time.
#endif
			if( ! FSending() )
			{
				FSending() = true;
				UpdateSending( FSending() );
				uint32_t ATime = micros();
				FEndTime() = ATime + AfterSendingDelay() + ACount * ( 10000000 / GetByteSpeed() );
//				Serial.println( 10000000 / GetByteSpeed() );
//				Serial.println( int32_t( FEndTime - ATime ));
			}

			else
			{
				UpdateSending( FSending() );
				uint32_t ATime = micros();
				if( int32_t( FEndTime().GetValue() - ATime ) >= AfterSendingDelay() )
					FEndTime() = FEndTime().GetValue() + ACount * ( 10000000 / GetByteSpeed() );

				else
					FEndTime() = ATime + AfterSendingDelay() + ACount * ( 10000000 / GetByteSpeed() );

//					Serial.println( 10000000 / GetByteSpeed() );
//					Serial.println( int32_t( FEndTime - ATime ));
			}

//			Serial.println( 10000000 / GetByteSpeed() );
//			Serial.println( int32_t( FEndTime - ATime ));
		}
#endif

	public:
		template<typename T> void Print( T AValue )
		{
			if( Enabled() )
			{
#ifdef _MITOV_SERIAL_SENDING_PIN_
				SendingCount( 1 );
#endif
				C_SERIAL.println( AValue );
#ifdef __AVR
//				SendingCount( 1 );
#endif
			}
		}

#ifdef _MITOV_SERIAL_SENDING_PIN_
		void Print( Mitov::String AValue )
		{
			if( Enabled() )
			{
#ifdef _MITOV_SERIAL_SENDING_PIN_
				SendingCount( AValue.length() );
#endif
				C_SERIAL.println( AValue );
#ifdef _MITOV_SERIAL_SENDING_PIN_
#ifdef __AVR
//				SendingCount( 1 );
#endif
#endif
			}
		}

		void Print( char *AValue )
		{
			if( Enabled() )
			{
#ifdef _MITOV_SERIAL_SENDING_PIN_
				SendingCount( strlen( AValue ) );
#endif
				C_SERIAL.println( AValue );
#ifdef _MITOV_SERIAL_SENDING_PIN_
#ifdef __AVR
//				SendingCount( 1 );
#endif
#endif
			}
		}
#endif

		void PrintChar( char AValue )
		{
			if( Enabled() )
			{
#ifdef _MITOV_SERIAL_SENDING_PIN_
				SendingCount( 1 );
#endif
				C_SERIAL.print( AValue );
			}
		}

		void Write( uint8_t *AData, uint32_t ASize )
		{
#ifdef _MITOV_SERIAL_SENDING_PIN_
			SendingCount( ASize );
#endif
			C_SERIAL.write( AData, ASize );
#ifdef _MITOV_SERIAL_SENDING_PIN_
#ifdef __AVR
//			SendingCount( 1 );
#endif
#endif
		}

	public:
		inline void SystemLoopBegin()
		{
#ifdef _MITOV_SERIAL_SENDING_PIN_
			if( FSending() )
			{
				unsigned long ACurrentMicros = micros();
				if( int32_t( FEndTime().GetValue() - ACurrentMicros ) < 0 )
				{
//					Serial.print( "STOP: " );
//					Serial.println( int32_t( FEndTime - ACurrentMicros ) );
					FSending() = false;
					UpdateSending( FSending() );
				}
			}

#endif
		}

		inline void SystemInit()
		{
			if( Enabled() )
				StartPort();

		}

	public:
        void RestartPort() // Used by setters
		{
			if( ! Enabled() )
				return;

			C_SERIAL.end();
			StartPort();
		}

	public:
		void SetEnabled( bool AValue )
		{
            if( Enabled() == AValue )
                return;

            Enabled() = AValue;
			if( Enabled() )
				StartPort();

			else
			{
				C_SERIAL.end();
				SerialEnd();
			}
		}

	public:
		inline SerialESP8266PortSendOnly()
		{
#ifdef _MITOV_SERIAL_SENDING_PIN_
			FSending() = false;
#endif
		}

	};
//---------------------------------------------------------------------------
} // Mitov

#endif // _MITOV_EPS8266_SERIAL_h
