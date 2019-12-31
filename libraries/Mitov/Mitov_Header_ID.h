////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2019 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _MITOV_HEADER_ID_h
#define _MITOV_HEADER_ID_h

#include <Mitov.h>

namespace Mitov
{
//---------------------------------------------------------------------------
	template<
		typename T_Enabled,
		typename T_OutputPin
	> class AddPacketHeaderID :
		public T_Enabled,
		public T_OutputPin
	{
	public:
		_V_PIN_( OutputPin )

	public:
		_V_PROP_( Enabled )

		ConstBytes	ID;

	protected:
		void send( const uint8_t* data, uint8_t len )
		{
			if( ! Enabled() )
				T_OutputPin::SetPinValue( Mitov::TDataBlock( len, data ));

			else
			{
				int ASize = len + ID._BytesSize;
				uint8_t *Adata = new uint8_t[ ASize ];

				memcpy( Adata, ID._Bytes, ID._BytesSize );
				memcpy( Adata + ID._BytesSize, data, len );

				T_OutputPin::SetPinValue( Mitov::TDataBlock( ASize, Adata ));

				delete [] Adata;
			}
		}

	public:
		void Print( Mitov::String AValue )
		{
			AValue += "\r\n";
			send( (uint8_t *)AValue.c_str(), AValue.length() );
		}

		void Print( float AValue )
		{
			char AText[ 16 ];
			dtostrf( AValue,  1, 2, AText );
			Print( Mitov::String( AText ));
		}

		void Print( int32_t AValue )
		{
			char AText[ 16 ];
			ltoa( AValue, AText, 10 );
			Print( Mitov::String( AText ));
		}

		void Print( uint32_t AValue )
		{
			char AText[ 16 ];
			ltoa( AValue, AText, 10 );
			Print( Mitov::String( AText ));
		}

		void PrintChar( char AValue )
		{
			send( (uint8_t*)&AValue, 1 );
		}

		void PrintChar( byte AValue )
		{
			send( &AValue, 1 );
		}

		void Write( uint8_t *AData, uint32_t ASize )
		{
			send( AData, ASize );
		}

	};
//---------------------------------------------------------------------------
	template<
		typename T_Enabled,
		typename T_OutputPin
	> class DetectPacketHeaderID :
		public T_Enabled,
		public T_OutputPin
	{
	public:
		_V_PIN_( OutputPin )

	public:
		_V_PROP_( Enabled )
		ConstBytes	ID;

	public:
		void InputPin_o_Receive( void *_Data )
		{
			if( ! Enabled() )
			{
				T_OutputPin::PinNotify( _Data );
				return;
			}

			Mitov::TDataBlock AInDataBlock = *(Mitov::TDataBlock*)_Data;

			if( AInDataBlock.Size < ID._BytesSize )
				return;

			if( memcmp( AInDataBlock.Data, ID._Bytes, ID._BytesSize ) != 0 )
				return;

			T_OutputPin::SetPinValue( Mitov::TDataBlock( AInDataBlock.Size - ID._BytesSize, AInDataBlock.Data + ID._BytesSize ));
		}
	};
//---------------------------------------------------------------------------
}

#endif
