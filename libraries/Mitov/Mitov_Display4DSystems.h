////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2019 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _MITOV_DISPLAY_4D_SYSTEMS_h
#define _MITOV_DISPLAY_4D_SYSTEMS_h

#include <Mitov.h>
#include <genieArduino.h>

namespace Mitov
{


/*
	void myGenieEventHandler(void)
	{
		Serial.println( "myGenieEventHandler" );
	}
*/
//	class Display4DSystems;
//---------------------------------------------------------------------------
/*
	template<typename T_OWNER> class ViSiGenieBasicObject
	{
	protected:
		T_OWNER &FOwner;

	public:
		virtual void Start() {}
		virtual void Process() {}

	public:
		ViSiGenieBasicObject( T_OWNER &AOwner ) :
			FOwner( AOwner )
		{
			AOwner.FElements.push_back( this );
		}

	};
*/
//---------------------------------------------------------------------------
/*
	template<typename T_OWNER> class ViSiGenieBasicOutObject
	{
	public:
		virtual bool ProcessOut( Genie &AGenie, genieFrame &Event ) = 0;

	public:
		ViSiGenieBasicOutObject( T_OWNER &AOwner )
		{
			AOwner.FOutElements.push_back( this );
		}

	};
*/
//---------------------------------------------------------------------------
/*
	template<typename T_OWNER, T_OWNER &C_OWNER, int V_OBJECT, int V_INDEX> class ViSiGenieBasicTypedOutObject // : public ViSiGenieBasicOutObject<T_OWNER>
	{
//		typedef	ViSiGenieBasicOutObject<T_OWNER> inherited;

	protected:
		virtual void PrcessInValue( uint16_t AValue ) = 0;

	public:
		inline void  ProcessOut( Genie &AGenie, genieFrame &Event, bool &AProcessed )
		{
			if( AProcessed )
				return;

//			Serial.println( "ProcessOut" );
			if( Event.reportObject.cmd == GENIE_REPORT_EVENT )
			{
/ *
				Serial.println( "GENIE_REPORT_EVENT" );
				Serial.print( Event.reportObject.object );
				Serial.print( " - " );
				Serial.println( Event.reportObject.index );
* /
				if( Event.reportObject.object == V_OBJECT )
					if( Event.reportObject.index == V_INDEX )
					{
						uint16_t AValue = AGenie.GetEventData(&Event);
//						Serial.println( AValue );
						PrcessInValue( AValue );

						AProcessed = true;
					}

			}
		}


	};
*/
//---------------------------------------------------------------------------
	template<
		typename T_OWNER, T_OWNER &C_OWNER,
		int V_INDEX,
		typename T_OutputPin,
		int V_OBJECT
	> class ViSiGenieTypedCharOut : // public ViSiGenieBasicTypedOutObject<T_OWNER, C_OWNER, V_OBJECT, V_INDEX>
		public T_OutputPin
	{
//		typedef	ViSiGenieBasicTypedOutObject<T_OWNER, C_OWNER, V_OBJECT, V_INDEX> inherited;

	public:
		_V_PIN_( OutputPin )

//	public:
//		bool	EnterNewLine;

	protected:
		inline void PrcessInValue( uint16_t AValue )
		{
			char ATypedValue = *(char *)AValue;
			T_OutputPin::SetPinValue( ATypedValue );
//			if( EnterNewLine )
//				if( *ATypedValue == '\r' )
//					OutputPin.SendValue( '\n' );
		}

	public:
		inline void Start() {}
		inline void Process() {}
		inline void ProcessOut( Genie &AGenie, genieFrame &Event, bool &AProcessed )
		{
			if( AProcessed )
				return;

//			Serial.println( "ProcessOut" );
			if( Event.reportObject.cmd == GENIE_REPORT_EVENT )
			{
/*
				Serial.println( "GENIE_REPORT_EVENT" );
				Serial.print( Event.reportObject.object );
				Serial.print( " - " );
				Serial.println( Event.reportObject.index );
*/
				if( Event.reportObject.object == V_OBJECT )
					if( Event.reportObject.index == V_INDEX )
					{
						uint16_t AValue = AGenie.GetEventData(&Event);
//						Serial.println( AValue );
						PrcessInValue( AValue );

						AProcessed = true;
					}

			}
		}

	};
//---------------------------------------------------------------------------
/*
	template<typename T_OWNER, T_OWNER &C_OWNER, int V_OBJECT, int V_INDEX, typename T_DATA> class ViSiGenieBasicTypedIn // : public ViSiGenieBasicObject<T_OWNER>
	{
//		typedef	ViSiGenieBasicObject<T_OWNER> inherited;

	public:
		T_DATA		InitialValue;

	public:
		bool	OnlyChanged : 1;

	public:
		bool		FStarted : 1;
		bool		FReceived : 1;

	public:
		T_DATA		FOldValue;

		
	public:
		inline void InputPin_o_Receive( void *_Data )
		{
			InitialValue = *(T_DATA *)_Data;
			if( OnlyChanged && FStarted )
				if( InitialValue == FOldValue )
					return;

			FReceived = true;
			C_OWNER.FModified() = true;
		}

		inline void RefreshInputPin_o_Receive( void *_Data )
		{
			FStarted = false;
		}

	public:
		ViSiGenieBasicTypedIn()
		{
			FStarted = false;
			FReceived = false;
			FOldValue = InitialValue;
		}
	};
//---------------------------------------------------------------------------
	template<typename T_OWNER, T_OWNER &C_OWNER, int V_OBJECT, int V_INDEX, typename T_DATA> class ViSiGenieTypedIn // : public ViSiGenieBasicTypedIn<T_OWNER, C_OWNER, V_OBJECT, V_INDEX, T_DATA>
	{
	public:
		T_DATA		InitialValue;

	public:
		bool	OnlyChanged : 1;

	public:
		bool		FStarted : 1;
		bool		FReceived : 1;

	public:
		T_DATA		FOldValue;

		
	public:
		inline void InputPin_o_Receive( void *_Data )
		{
			InitialValue = *(T_DATA *)_Data;
			if( OnlyChanged && FStarted )
				if( InitialValue == FOldValue )
					return;

			FReceived = true;
			C_OWNER.FModified() = true;
		}

		inline void RefreshInputPin_o_Receive( void *_Data )
		{
			FStarted = false;
		}

	protected:
		inline uint16_t	GetValue()	{ return InitialValue; };

	public:
		inline void Process()
		{
			if( OnlyChanged && FStarted )
				if( FOldValue == InitialValue )
					return;

			if( ! FReceived )
				return;

			FStarted = true;
			FOldValue = InitialValue;

			C_OWNER.FGenie.WriteObject(V_OBJECT, V_INDEX, GetValue() );
		}

	public:
		ViSiGenieTypedIn()
		{
			FStarted = false;
			FReceived = false;
			FOldValue = InitialValue;
		}

	};
*/
//---------------------------------------------------------------------------
/*
	template<typename T_OWNER, T_OWNER &C_OWNER, int V_OBJECT, int V_INDEX, typename T_DATA> class ViSiGenieTypedInOut // : 
//		public ViSiGenieTypedIn<T_OWNER, C_OWNER, V_OBJECT, V_INDEX, T_DATA>,
//		public ViSiGenieBasicTypedOutObject<T_OWNER, C_OWNER, V_OBJECT, V_INDEX>
	{
	public:
		OpenWire::SourcePin	OutputPin;

	public:
		T_DATA		InitialValue;

	public:
		bool	OnlyChanged : 1;

	public:
		bool		FStarted : 1;
		bool		FReceived : 1;

	public:
		T_DATA		FOldValue;

		
	public:
		inline void InputPin_o_Receive( void *_Data )
		{
			InitialValue = *(T_DATA *)_Data;
			if( OnlyChanged && FStarted )
				if( InitialValue == FOldValue )
					return;

			FReceived = true;
			C_OWNER.FModified() = true;
		}

		inline void RefreshInputPin_o_Receive( void *_Data )
		{
			FStarted = false;
		}

	protected:
		inline uint16_t	GetValue()	{ return InitialValue; };

	public:
		inline void Process()
		{
			if( OnlyChanged && FStarted )
				if( FOldValue == InitialValue )
					return;

			if( ! FReceived )
				return;

			FStarted = true;
			FOldValue = InitialValue;

			C_OWNER.FGenie.WriteObject(V_OBJECT, V_INDEX, GetValue() );
		}

/ *
	public:
		virtual bool ProcessOut( Genie &AGenie, genieFrame &Event )
		{
		}
* /
	protected:
		virtual void PrcessInValue( uint16_t AValue ) override
		{
			T_DATA ATypedValue = (T_DATA)AValue;
			OutputPin.Notify( &ATypedValue );
		}

	public:
		inline void ProcessOut( Genie &AGenie, genieFrame &Event, bool &AProcessed )
		{
			if( AProcessed )
				return;

//			Serial.println( "ProcessOut" );
			if( Event.reportObject.cmd == GENIE_REPORT_EVENT )
			{
/ *
				Serial.println( "GENIE_REPORT_EVENT" );
				Serial.print( Event.reportObject.object );
				Serial.print( " - " );
				Serial.println( Event.reportObject.index );
* /
				if( Event.reportObject.object == V_OBJECT )
					if( Event.reportObject.index == V_INDEX )
					{
						uint16_t AValue = AGenie.GetEventData(&Event);
//						Serial.println( AValue );
						PrcessInValue( AValue );

						AProcessed = true;
					}

			}
		}

	public:
		ViSiGenieTypedInOut()
		{
			FStarted = false;
			FReceived = false;
			FOldValue = InitialValue;
		}

	};
*/
//---------------------------------------------------------------------------
/*
	template<typename T_OWNER, int V_OBJECT, int V_INDEX> class ViSiGenieTypedAnalogIn<T_OWNER> : public ViSiGenieTypedIn<T_OWNER, V_OBJECT, V_INDEX, float>
	{
		typedef	ViSiGenieTypedIn<T_OWNER, V_OBJECT, V_INDEX, float> inherited;

	protected:
		virtual uint16_t	GetValue()	{ return inherited::FValue + 0.5; } override;

	public:
		ViSiGenieTypedAnalogIn( T_OWNER &AOwner ) :
			inherited( AOwner, 0.0 )
		{
		}
	};
*/
//---------------------------------------------------------------------------
	template<
		typename T_OWNER, T_OWNER &C_OWNER, 
		typename T_FReceived,
		typename T_FStarted,
		int V_INDEX,
		typename T_InitialValue,
		typename T_OnlyChanged,
		int V_OBJECT
	> class ViSiGenieTypedUnsignedIn : // public ViSiGenieBasicTypedIn<T_OWNER, C_OWNER, V_OBJECT, V_INDEX, uint32_t>
		public T_FReceived,
		public T_FStarted,
		public T_InitialValue,
		public T_OnlyChanged
	{
	public:
		_V_PROP_( InitialValue )

	public:
		_V_PROP_( OnlyChanged )

	public:
		_V_PROP_( FStarted )
		_V_PROP_( FReceived )

	public:
		uint32_t	FOldValue;
		
	public:
		inline void Start() {}
		inline void ProcessOut( Genie &AGenie, genieFrame &Event, bool &AProcessed ) {}

		inline void Process()
		{
			if( OnlyChanged().GetValue() && FStarted().GetValue() )
				if( FOldValue == InitialValue().GetValue() )
					return;

			if( ! FReceived().GetValue() )
				return;

			FStarted() = true;
			FOldValue = InitialValue();

			C_OWNER.FGenie.WriteObject(V_OBJECT, V_INDEX, InitialValue() );
		}

	public:
		inline void InputPin_o_Receive( void *_Data )
		{
			InitialValue() = *(uint32_t *)_Data;
			if( OnlyChanged().GetValue() && FStarted().GetValue() )
				if( InitialValue() == FOldValue )
					return;

			FReceived() = true;
			C_OWNER.FModified() = true;
		}

		inline void RefreshInputPin_o_Receive( void *_Data )
		{
			FStarted() = false;
		}

	public:
		ViSiGenieTypedUnsignedIn()
		{
			FStarted() = false;
			FReceived() = false;
			FOldValue = InitialValue();
		}
	};
//---------------------------------------------------------------------------
	template<
		typename T_OWNER, T_OWNER &C_OWNER,
		typename T_FReceived,
		typename T_FStarted,
		int V_INDEX,
		typename T_InitialValue,
		typename T_OnlyChanged,
		typename T_OutputPin,
		int V_OBJECT
	> class ViSiGenieTypedUnsignedInOut : // public ViSiGenieTypedInOut<T_OWNER, C_OWNER, V_OBJECT, V_INDEX, uint32_t>
		public T_FReceived,
		public T_FStarted,
		public T_InitialValue,
		public T_OnlyChanged,
		public T_OutputPin
	{
//		typedef	ViSiGenieTypedInOut<T_OWNER, C_OWNER, V_OBJECT, V_INDEX, uint32_t> inherited;

	public:
		_V_PIN_( OutputPin )

	public:
		_V_PROP_( InitialValue )

	public:
		_V_PROP_( OnlyChanged )

	public:
		_V_PROP_( FStarted )
		_V_PROP_( FReceived )

	public:
		uint32_t		FOldValue;

		
	public:
		inline void InputPin_o_Receive( void *_Data )
		{
			InitialValue() = *(uint32_t *)_Data;
			if( OnlyChanged().GetValue() && FStarted().GetValue() )
				if( InitialValue().GetValue() == FOldValue )
					return;

			FReceived() = true;
			C_OWNER.FModified() = true;
		}

		inline void RefreshInputPin_o_Receive( void *_Data )
		{
			FStarted() = false;
		}

	protected:
		inline uint16_t	GetValue()	{ return InitialValue(); };

	public:
		inline void Process()
		{
			if( OnlyChanged().GetValue() && FStarted().GetValue() )
				if( FOldValue == InitialValue() )
					return;

			if( ! FReceived() )
				return;

			FStarted() = true;
			FOldValue = InitialValue();

			C_OWNER.FGenie.WriteObject(V_OBJECT, V_INDEX, GetValue() );
		}

/*
	public:
		virtual bool ProcessOut( Genie &AGenie, genieFrame &Event )
		{
		}
*/
	protected:
		void PrcessInValue( uint16_t AValue )
		{
			uint32_t ATypedValue = (uint32_t)AValue;
			T_OutputPin::SetPinValue( ATypedValue, true );
		}

	public:
		inline void  ProcessOut( Genie &AGenie, genieFrame &Event, bool &AProcessed )
		{
			if( AProcessed )
				return;

//			Serial.println( "ProcessOut" );
			if( Event.reportObject.cmd == GENIE_REPORT_EVENT )
			{
/*
				Serial.println( "GENIE_REPORT_EVENT" );
				Serial.print( Event.reportObject.object );
				Serial.print( " - " );
				Serial.println( Event.reportObject.index );
*/
				if( Event.reportObject.object == V_OBJECT )
					if( Event.reportObject.index == V_INDEX )
					{
						uint16_t AValue = AGenie.GetEventData(&Event);
//						Serial.println( AValue );
						PrcessInValue( AValue );

						AProcessed = true;
					}

			}
		}

	public:
		inline void Start()
		{
			T_OutputPin::SetPinValue( InitialValue(), false );
		}

	public:
		ViSiGenieTypedUnsignedInOut()
		{
			FStarted() = false;
			FReceived() = false;
			FOldValue = InitialValue();
		}

	};
//---------------------------------------------------------------------------
	template<
		typename T_OWNER, T_OWNER &C_OWNER,
		typename T_FOldValue,
		typename T_FReceived,
		typename T_FStarted,
		int V_INDEX,
		typename T_InitialValue,
		typename T_OnlyChanged,
		int V_OBJECT
	> class ViSiGenieTypedDigitalIn : // public ViSiGenieTypedIn<T_OWNER, C_OWNER, V_OBJECT, V_INDEX, bool>
		public T_FOldValue,
		public T_FReceived,
		public T_FStarted,
		public T_InitialValue,
		public T_OnlyChanged
	{
	public:
		_V_PROP_( InitialValue )

	public:
		_V_PROP_( OnlyChanged )

	public:
		_V_PROP_( FStarted )
		_V_PROP_( FReceived )

	public:
		_V_PROP_( FOldValue )
		
	public:
		inline void InputPin_o_Receive( void *_Data )
		{
			InitialValue() = *(bool *)_Data;
			if( OnlyChanged().GetValue() && FStarted().GetValue() )
				if( InitialValue().GetValue() == FOldValue().GetValue() )
					return;

			FReceived() = true;
			C_OWNER.FModified() = true;
		}

		inline void RefreshInputPin_o_Receive( void *_Data )
		{
			FStarted() = false;
		}

	protected:
		inline uint16_t	GetValue() { return InitialValue() ? 1 : 0; }

	public:
		inline void Start() {}
		inline void ProcessOut( Genie &AGenie, genieFrame &Event, bool &AProcessed ) {}
		inline void Process()
		{
			if( OnlyChanged().GetValue() && FStarted().GetValue() )
				if( FOldValue().GetValue() == InitialValue().GetValue() )
					return;

			if( ! FReceived() )
				return;

			FStarted() = true;
			FOldValue() = InitialValue();

			C_OWNER.FGenie.WriteObject(V_OBJECT, V_INDEX, GetValue() );
		}

	public:
		ViSiGenieTypedDigitalIn()
		{
			FStarted() = false;
			FReceived() = false;
			FOldValue() = InitialValue();
		}

	};
//---------------------------------------------------------------------------
	template<
		typename T_OWNER, T_OWNER &C_OWNER,
		typename T_FOldReceivedValue,
		typename T_FOldValue,
		typename T_FReceived,
		typename T_FStarted,
		int V_INDEX,
		typename T_InitialValue,
		typename T_OnlyChanged,
		typename T_OutputPin,
		int V_OBJECT
	> class ViSiGenieTypedDigitalInOut : // public ViSiGenieTypedInOut<T_OWNER, C_OWNER, V_OBJECT, V_INDEX, bool>
		public T_FOldReceivedValue,
		public T_FOldValue,
		public T_FReceived,
		public T_FStarted,
		public T_InitialValue,
		public T_OnlyChanged,
		public T_OutputPin
	{
	public:
		_V_PIN_( OutputPin )

	public:
		_V_PROP_( InitialValue )

	public:
		_V_PROP_( OnlyChanged )

	public:
		_V_PROP_( FStarted )
		_V_PROP_( FReceived )

	public:
		_V_PROP_( FOldValue )

	protected:
		_V_PROP_( FOldReceivedValue )

		
	public:
		inline void InputPin_o_Receive( void *_Data )
		{
			InitialValue() = *(bool *)_Data;
			if( OnlyChanged().GetValue() && FStarted().GetValue() )
				if( InitialValue().GetValue() == FOldValue().GetValue() )
					return;

			FReceived() = true;
			C_OWNER.FModified() = true;
		}

		inline void RefreshInputPin_o_Receive( void *_Data )
		{
			FStarted() = false;
		}

	public:
		inline void Process()
		{
			if( OnlyChanged().GetValue() && FStarted().GetValue() )
				if( FOldValue().GetValue() == InitialValue().GetValue() )
					return;

			if( ! FReceived() )
				return;

			FStarted() = true;
			FOldValue() = InitialValue();

			C_OWNER.FGenie.WriteObject(V_OBJECT, V_INDEX, GetValue() );
		}

/*
	public:
		virtual bool ProcessOut( Genie &AGenie, genieFrame &Event )
		{
		}
*/
	public:
		inline void Start() {}
		inline void ProcessOut( Genie &AGenie, genieFrame &Event, bool &AProcessed )
		{
			if( AProcessed )
				return;

//			Serial.println( "ProcessOut" );
			if( Event.reportObject.cmd == GENIE_REPORT_EVENT )
			{
/*
				Serial.println( "GENIE_REPORT_EVENT" );
				Serial.print( Event.reportObject.object );
				Serial.print( " - " );
				Serial.println( Event.reportObject.index );
*/
				if( Event.reportObject.object == V_OBJECT )
					if( Event.reportObject.index == V_INDEX )
					{
						uint16_t AValue = AGenie.GetEventData(&Event);
//						Serial.println( AValue );
						PrcessInValue( AValue );

						AProcessed = true;
					}

			}
		}

	protected:
		inline uint16_t	GetValue() { return InitialValue().GetValue() ? 1 : 0; } 

	protected:
		inline void PrcessInValue( uint16_t AValue )
		{
			bool ATypedValue = (bool)AValue;
			if( ! FOldReceivedValue() )
				if( ! ATypedValue )
					T_OutputPin::SetPinValue( true );

			FOldReceivedValue() = ATypedValue;
			T_OutputPin::SetPinValue( ATypedValue );
		}

	public:
		inline ViSiGenieTypedDigitalInOut()
		{
			FStarted() = false;
			FReceived() = false;
			FOldReceivedValue() = false;
			FOldValue() = InitialValue();
		}

	};
//---------------------------------------------------------------------------
	template<
		typename T_OWNER, T_OWNER &C_OWNER,
		typename T_FReceived,
		typename T_FStarted,
		int V_INDEX,
//		typename T_InitialValue,
		typename T_OnlyChanged,
		typename T_OutputPin,
		int V_OBJECT
	> class ViSiGenieTypedColorInOut : // public ViSiGenieTypedInOut<T_OWNER, C_OWNER, V_OBJECT, V_INDEX, Mitov::TColor>
		public T_FReceived,
		public T_FStarted,
//		public T_InitialValue,
		public T_OnlyChanged,
		public T_OutputPin
	{
	public:
		_V_PIN_( OutputPin )

	public:
		Mitov::TColor	InitialValue;

	public:
		_V_PROP_( OnlyChanged )

	public:
		_V_PROP_( FStarted )
		_V_PROP_( FReceived )

	public:
		Mitov::TColor	FOldValue;

	public:
		inline void InputPin_o_Receive( void *_Data )
		{
			InitialValue = *(Mitov::TColor *)_Data;
			if( OnlyChanged().GetValue() && FStarted().GetValue() )
				if( InitialValue == FOldValue )
					return;

			FReceived() = true;
			C_OWNER.FModified() = true;
		}

		inline void RefreshInputPin_o_Receive( void *_Data )
		{
			FStarted() = false;
		}

	protected:
		inline uint16_t GetValue()
		{ 
			return ( ( InitialValue.Red >> 3 ) << ( 6 + 5 )) | ( ( InitialValue.Green >> 2 ) & 0b111111 ) << 5 | ( ( InitialValue.Blue >> 3 ) & 0b11111 );
		}

	public:
		inline void Start() {}
		inline void Process()
		{
			if( OnlyChanged().GetValue() && FStarted().GetValue() )
				if( FOldValue == InitialValue )
					return;

			if( ! FReceived() )
				return;

			FStarted() = true;
			FOldValue = InitialValue;

			C_OWNER.FGenie.WriteObject(V_OBJECT, V_INDEX, GetValue() );
		}

/*
	public:
		virtual bool ProcessOut( Genie &AGenie, genieFrame &Event )
		{
		}
*/
	protected:
		inline void PrcessInValue( uint16_t AValue )
		{
			Mitov::TColor ATypedValue; // = AValue;
			ATypedValue.Red = ( AValue & 0b1111100000000000 ) >> ( 6 + 5 - 3 ); //0b1111100000000000 
			ATypedValue.Green = ( AValue & 0b11111100000 ) >> ( 5 - 2 ); 
			ATypedValue.Blue = ( AValue & 0b11111 ) << 3; 
			T_OutputPin::SetPinValue( ATypedValue );
		}

	public:
		inline void  ProcessOut( Genie &AGenie, genieFrame &Event, bool &AProcessed )
		{
			if( AProcessed )
				return;

//			Serial.println( "ProcessOut" );
			if( Event.reportObject.cmd == GENIE_REPORT_EVENT )
			{
/*
				Serial.println( "GENIE_REPORT_EVENT" );
				Serial.print( Event.reportObject.object );
				Serial.print( " - " );
				Serial.println( Event.reportObject.index );
*/
				if( Event.reportObject.object == V_OBJECT )
					if( Event.reportObject.index == V_INDEX )
					{
						uint16_t AValue = AGenie.GetEventData(&Event);
//						Serial.println( AValue );
						PrcessInValue( AValue );

						AProcessed = true;
					}

			}
		}

	public:
		inline ViSiGenieTypedColorInOut()
		{
			FStarted() = false;
			FReceived() = false;
//			FOldValue = InitialValue;
		}

	};
//---------------------------------------------------------------------------
	template<
		typename T_OWNER, T_OWNER &C_OWNER,
		typename T_FReceived,
		typename T_FStarted,
		int V_INDEX,
		typename T_OnlyChanged,
		int V_OBJECT
	> class ViSiGenieTypedStringIn : // public ViSiGenieBasicTypedIn<T_OWNER, C_OWNER, V_OBJECT, V_INDEX, Mitov::String>
		public T_FReceived,
		public T_FStarted,
		public T_OnlyChanged
	{
	public:
		Mitov::String	InitialValue;

	public:
		_V_PROP_( OnlyChanged )

	public:
		_V_PROP_( FStarted )
		_V_PROP_( FReceived )

	public:
		Mitov::String	FOldValue;
		
	public:
		inline void RefreshInputPin_o_Receive( void *_Data )
		{
			FStarted() = false;
		}

	public:
		inline void InputPin_o_Receive( void *_Data )
		{
			InitialValue = (char *)_Data;
			if( OnlyChanged().GetValue() && FStarted().GetValue() )
				if( InitialValue == FOldValue )
					return;

			FReceived() = true;
			C_OWNER.FModified() = true;
		}

	public:
		inline void Start() {}
		inline void ProcessOut( Genie &AGenie, genieFrame &Event, bool &AProcessed ) {}

		inline void Process()
		{
			if( OnlyChanged().GetValue() && FStarted().GetValue() )
				if( FOldValue == InitialValue )
					return;

			if( ! FReceived() )
				return;

			FStarted() = true;
			FOldValue = InitialValue;

			C_OWNER.FGenie.WriteStr(V_INDEX, InitialValue.c_str() );
		}

	public:
		ViSiGenieTypedStringIn()
		{
			FStarted() = false;
			FReceived() = false;
			FOldValue = InitialValue;
		}

	};
//---------------------------------------------------------------------------
	template<
		typename T_OWNER, T_OWNER &C_OWNER,
		int V_INDEX,
		typename T_OutputPin,
		int V_OBJECT
	> class ViSiGenieTypedClockInOut : // public ViSiGenieBasicObject<T_OWNER> //, public ViSiGenieBasicOutObject<T_OWNER>
		public T_OutputPin
	{
//		typedef	ViSiGenieBasicObject<T_OWNER> inherited;

	public:
		_V_PIN_( OutputPin )

	public:
		inline void InputPin_o_Receive( void *_Data )
		{
			C_OWNER.FGenie.WriteObject( V_OBJECT, V_INDEX, 0 );
		}


	public:
		inline void Start() {}
		inline void Process() {}
		inline void ProcessOut( Genie &AGenie, genieFrame &Event, bool &AProcessed )
		{
			if( AProcessed )
				return;

//			Serial.println( "ProcessOut" );
			if( Event.reportObject.cmd == GENIE_REPORT_EVENT )
			{
/*
				Serial.println( "GENIE_REPORT_EVENT" );
				Serial.print( Event.reportObject.object );
				Serial.print( " - " );
				Serial.println( Event.reportObject.index );
*/
				if( Event.reportObject.object == V_OBJECT )
					if( Event.reportObject.index == V_INDEX )
					{
						T_OutputPin::ClockPin();
//						uint16_t AValue = AGenie.GetEventData(&Event);
//						Serial.println( AValue );
//						PrcessInValue( AValue );

						AProcessed = true;
					}

			}
		}

	};
//---------------------------------------------------------------------------
	template<
		typename T_OWNER, T_OWNER &C_OWNER,
		uint8_t C_NUM_CHANNELS,
		int V_INDEX
	> class ViSiGenieSpectrum // : public ViSiGenieBasicObject<T_OWNER>
	{
//		typedef	ViSiGenieBasicObject<T_OWNER> inherited;

	protected:
		float	FValues[ C_NUM_CHANNELS ];
		uint8_t	FModified[ ( C_NUM_CHANNELS + 7 ) / 8 ];

	public:
		void ColumnsInputPins_o_Receive( int AIndex, void *_Data )
		{
			float AValue = constrain( *(float *)_Data, 0.0f, 1.0f );
			if( FValues[ AIndex ] == AValue )
				return;

			Func::SetBitField( FModified, AIndex, true );
			C_OWNER.FModified() = true;
			FValues[ AIndex ] = AValue;
		}
		
	public:
		inline void Start() {}
		inline void ProcessOut( Genie &AGenie, genieFrame &Event, bool &AProcessed ) {}

		inline void Process()
		{
			for( int i = 0; i < C_NUM_CHANNELS; i ++ )
				if(( FModified[ i / 8 ] & ( 1 << ( i & 0b111 ))) != 0 )
				{
					uint16_t AValue = ( i << 8 ) | ( uint8_t( FValues[ i ] * 255 + 0.5 ));
					C_OWNER.FGenie.WriteObject( GENIE_OBJ_SPECTRUM, V_INDEX, AValue );
				}

		}

	};
//---------------------------------------------------------------------------
	template<
		typename T_SERIAL, T_SERIAL &C_SERIAL,
		typename T_Contrast,
		typename T_FModified,
		typename T_Objects_Process,
		typename T_Objects_ProcessOut,
		typename T_Objects_Start,
		typename T_ResetOutputPin
	> class Display4DSystems :
		public T_Contrast,
		public T_FModified,
		public T_ResetOutputPin
	{
	public:
		_V_PIN_( ResetOutputPin )

	public:
		_V_PROP_( Contrast )

	public:
		_V_PROP_( FModified )

//		Mitov::SimpleList<ViSiGenieBasicObject<Display4DSystems<T_SERIAL, C_SERIAL>> *>		FElements;
//		Mitov::SimpleList<ViSiGenieBasicOutObject<Display4DSystems<T_SERIAL, C_SERIAL>> *>	FOutElements;

	public:
		Genie FGenie;

	public:
		void SetContrast( float AValue )
		{
			AValue = constrain( AValue, 0.0, 1.0 );
			if( AValue == Contrast() )
				return;

			Contrast() = AValue;
			FGenie.WriteContrast( Contrast() * 15 + 0.5 );
		}

	public:
		inline void SystemStart() 
		{
			FGenie.Begin( C_SERIAL.GetStream() );

  //FGenie.AttachEventHandler(myGenieEventHandler); // Attach the user function Event Handler for processing events
			T_ResetOutputPin::SetPinValueHigh();

			delay( 100 );

			T_ResetOutputPin::SetPinValueLow();

			delay( 3500 );
			FGenie.WriteContrast( Contrast() * 15 + 0.5 );

			T_Objects_Start::ChainCall();
//			for( int i = 0; i < FElements.size(); ++ i )
//				FElements[ i ]->Start();

		}

		inline void SystemLoopEnd() 
		{
			FGenie.DoEvents(false); // This calls the library each loop to process the queued responses from the display

			if( FModified() )
				T_Objects_Process::ChainCall();
//				for( int i = 0; i < FElements.size(); ++ i )
//					FElements[ i ]->Process();

			genieFrame AEvent;
			if( FGenie.DequeueEvent(&AEvent))
			{
//				Serial.println( "DequeueEvent" );
				bool AProcessed = false;
				T_Objects_ProcessOut::ChainCall( FGenie, AEvent, AProcessed );
//				for( int i = 0; i < FOutElements.size(); ++ i )
//					if( FOutElements[ i ]->ProcessOut( FGenie, AEvent ))
//						break;
			}
		}
	};
//---------------------------------------------------------------------------
	template<
		typename T_OWNER, T_OWNER &C_OWNER,
		uint16_t C_COUNT_CHANNELS,
		typename T_Volume
	> class ViSiGenieSounds : // public ViSiGenieBasicObject<T_OWNER>
		public T_Volume
	{
	public:
		_V_PROP_( Volume )

	public:
		void SetVolume( float AValue )
		{
			AValue = constrain( AValue, 0.0, 1.0 );
			if( AValue == Volume().GetValue() )
				return;

			Volume() = AValue;
			C_OWNER.FGenie.WriteObject( GENIE_OBJ_SOUND, 1, Volume().GetValue() * 100 + 0.5 );
		}

	public:
		inline void Start()
		{
			C_OWNER.FGenie.WriteObject( GENIE_OBJ_SOUND, 1, Volume().GetValue() * 100 + 0.5 );
		}

		inline void Process() {}
		inline void ProcessOut( Genie &AGenie, genieFrame &Event, bool &AProcessed ) {}

	public:
		inline void TracksStartInputPins_o_Receive( int AIndex, void *_Data )
		{
			C_OWNER.FGenie.WriteObject( GENIE_OBJ_SOUND, 0, AIndex );
		}

		inline void StopInputPin_o_Receive( void *_Data )
		{
			C_OWNER.FGenie.WriteObject( GENIE_OBJ_SOUND, 4, 0 );
		}

		inline void PauseInputPin_o_Receive( void *_Data )
		{
			C_OWNER.FGenie.WriteObject( GENIE_OBJ_SOUND, 2, 0 );
		}

		inline void ResumeInputPin_o_Receive( void *_Data )
		{
			C_OWNER.FGenie.WriteObject( GENIE_OBJ_SOUND, 3, 0 );
		}

	};
//---------------------------------------------------------------------------
}

#endif
