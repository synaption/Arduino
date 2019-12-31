////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2019 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _MITOV_REMOTE_XY_h
#define _MITOV_REMOTE_XY_h

#include <Mitov.h>
//#include <EmbeddedRemoteXY/Mitov_RemoteXY_API.h>

#ifdef __TEST_FOR_DEBUG_PRINTS__
#define Serial UNGUARDED DEBUG PRINT!!!
#endif

namespace Mitov
{
	template <
		typename T_ACCSES,
		uint16_t C_CONFIG_LENGTH,
		uint8_t C_CONFIG_VERSION,
		uint16_t C_INPUT_LENGTH,
		uint16_t C_OUTPUT_LENGTH,
		typename T_FProtocolConnectedFlags
	> class MitovRemoteXY :
		public T_ACCSES,
		public T_FProtocolConnectedFlags
	{
	protected:
		_V_PROP_( FProtocolConnectedFlags )

	public:
		inline uint16_t static GetOutputLength() { return C_OUTPUT_LENGTH; }
		inline uint16_t static GetInputLength() { return C_INPUT_LENGTH; }
		inline uint16_t static GetConfLength() { return C_CONFIG_LENGTH; }
		inline uint8_t static GetConfVersion() { return C_CONFIG_VERSION; }
		inline bool GetIsConnected() { return ( FProtocolConnectedFlags().GetValue() != 0 ); }
		inline void SetConnectedFlag( uint8_t AIndex, bool AValue )
		{
			if( AValue )
				FProtocolConnectedFlags() = ( 1 << AIndex ) | FProtocolConnectedFlags().GetValue();

			else
				FProtocolConnectedFlags() = ( ~( 1 << AIndex )) & FProtocolConnectedFlags().GetValue();
		}

	public:
		inline static uint8_t getConfByte( uint8_t* p )
		{
			return pgm_read_byte_near( p );
		}

	public:
		inline MitovRemoteXY()
		{
			FProtocolConnectedFlags() = 0;
		}
	};
//---------------------------------------------------------------------------
	template <
		typename T_INTERFACE,
		typename T_OWNER, T_OWNER &C_OWNER,
//		const void * C_CONFIG,
//		void * C_DATA,
		typename T_ConnectedOutputPin,
		uint8_t C_INDEX,
		typename T_Password
	> class MitovRemoteXYProtpcol : // public CRemoteXY_API,
		public T_INTERFACE,
		public T_ConnectedOutputPin,
		public T_Password
	{
//		typedef CRemoteXY_API inherited;

	protected:
		static const uint8_t REMOTEXY_PACKAGE_START_BYTE = 0x55;
		static const uint8_t REMOTEXY_PASSWORD_LENGTH_MAX = 26;
		static const uint16_t REMOTEXY_TIMEOUT = 5000;

	protected:
		uint8_t *receiveBuffer;
		uint16_t receiveBufferLength;
		uint16_t receiveIndex;
		uint16_t receiveCRC;

		uint32_t wireTimeOut;
  
	public:
		_V_PIN_( ConnectedOutputPin )

	public:
		_V_PROP_( Password )

	protected:
		inline void sendByte( uint8_t b )
		{
			T_INTERFACE::sendByte( b );
		}
  
		inline void sendStart( uint16_t len ) 
		{
			T_INTERFACE::sendStart( len );
		};

/*
	public:
		inline MitovRemoteXY( const void * _conf, void * _var )
		{
		}
*/
	public:
		inline void _DirectPinReceive( void *_Data )
		{
			Mitov::TDataBlock ADataBlock = *(Mitov::TDataBlock*)_Data;
			for( int i = 0; i < ADataBlock.Size; ++i )
				ProcessByte( ADataBlock.Data[ i ] );

			T_INTERFACE::ByteReceived();
		}

	protected:
		void ProcessByte( uint8_t AByte )
		{
//			Serial.println( AByte );
			if(( receiveIndex == 0 ) && ( AByte != REMOTEXY_PACKAGE_START_BYTE )) 
				return;

//			Serial.print( "receiveIndex : " );	Serial.println( receiveIndex );

			receiveBuffer[receiveIndex++] = AByte;
			updateCRC( &receiveCRC, AByte );
			if( receiveIndex > receiveBufferLength )
				searchStartByte( 1 ); //receiveBuffer overflow

			while( true ) 
			{
				if (receiveIndex<6)
					break;

				uint16_t packageLength = receiveBuffer[1]|(receiveBuffer[2]<<8);
				if( packageLength > receiveBufferLength )
					searchStartByte(1); // error

				else if( packageLength < 6 )
					searchStartByte(1); // error

				else if( packageLength == receiveIndex )
				{
					if( receiveCRC==0 )
					{
						if( handleReceivePackage())
						{
							receiveIndex = 0;
							receiveCRC = initCRC();
							break;
						}
					}

					searchStartByte (1); // error 
				}

				else if( packageLength<receiveIndex )
				{
					uint16_t crc = initCRC();
					uint8_t *p = receiveBuffer;
					uint16_t i = packageLength;
					while( i-- )
						updateCRC( &crc, *(p++)); 

					if (crc==0)
					{
						if (handleReceivePackage())
						{
							searchStartByte( packageLength );
							continue;
						}
					}

					searchStartByte( 1 );
				}

				else
					break;
			}
		}  
    
		void init()
		{
			uint16_t varLength = C_OWNER.GetOutputLength() + C_OWNER.GetInputLength();
        
//			accessPassword = (uint8_t*)_accessPassword;

			receiveBufferLength = C_OWNER.GetInputLength();
			if (( Password().GetValue().length() > 0 )&&(receiveBufferLength<REMOTEXY_PASSWORD_LENGTH_MAX))
			  receiveBufferLength = REMOTEXY_PASSWORD_LENGTH_MAX;

			receiveBufferLength += 6;  

//			Serial.print( "receiveBufferLength = " ); Serial.println( receiveBufferLength );
    
			receiveBuffer = (uint8_t*)malloc( receiveBufferLength );
    
			uint8_t* p = C_OWNER.GetData();
			uint8_t i = varLength;
			while( i-- )
				*p++ = 0;   
    
			resetWire();
 
#if defined(REMOTEXY__DEBUGLOGS)
			Serial.println("RemoteXY started");
#endif
			T_INTERFACE::Init( C_OWNER, *this ); 
		}

	public:
		inline uint16_t GetOutputLength() { return C_OWNER.GetOutputLength(); }
		inline uint16_t GetInputLength() { return C_OWNER.GetInputLength(); }
		inline uint16_t GetConfLength() { return C_OWNER.GetConfLength(); }
		inline uint16_t GetReceiveBufferLength() { return receiveBufferLength; }

	private:
		inline uint16_t initCRC()
		{
			return 0xffff;
		}          

	private:
		void updateCRC( uint16_t *crc, uint8_t b )
		{
			*crc ^= b;
			for( uint8_t i = 0; i < 8; ++i )
			{
				if ((*crc) & 1)
					*crc = ((*crc) >> 1) ^ 0xA001;

				else
					*crc >>= 1;
			}
		} 
  
	private:  
		void sendByteUpdateCRC( uint8_t b, uint16_t *crc )
		{
			sendByte( b );
			updateCRC( crc, b );
		}  

  public:
		void sendPackage( uint8_t command, uint8_t *p, uint16_t length, uint8_t itConf ) 
		{
			uint16_t crc = initCRC();
			uint16_t packageLength = length + 6;
			sendStart( packageLength );
			sendByteUpdateCRC( REMOTEXY_PACKAGE_START_BYTE, &crc );
			sendByteUpdateCRC( packageLength, &crc);
			sendByteUpdateCRC( packageLength>>8, &crc );
			sendByteUpdateCRC( command, &crc );
			uint8_t b;
			while( length -- )
			{
				if( itConf )
					b = T_OWNER::getConfByte(p++);

				else
					b = *p++;

				sendByteUpdateCRC( b, &crc );
			}

			sendByte( crc );  
			sendByte( crc >> 8 );
		}
  
	private:  
		void searchStartByte( uint16_t pos ) 
		{
			uint8_t *p, *kp;
			uint16_t ri = receiveIndex;
			p = receiveBuffer + pos;
			receiveCRC=initCRC ();
			for( uint16_t i = pos; i < ri; i ++ )
			{
				if (*p==REMOTEXY_PACKAGE_START_BYTE)
				{
					kp=receiveBuffer;
					receiveIndex=receiveIndex-i;
					while( i++ < ri ) 
					{
						updateCRC( &receiveCRC, *p );
						*(kp++) = *(p++);
					}

					return;
				}

				p++;
			}        

			receiveIndex=0;
		}  
  
		uint8_t handleReceivePackage()
		{
			uint16_t i;
			uint8_t *p, *kp;
       
			uint16_t length = ( receiveBuffer[1]|(uint8_t( receiveBuffer[2] ) << 8 )) - 6;
			uint8_t command = receiveBuffer[3];

//			Serial.print( "command = " ); Serial.println( command );
//			Serial.print( "length = " ); Serial.println( length );

			switch( command )
			{
				case 0x00:  
					uint8_t available;
					if( length == 0 )
					{
						if( Password().GetValue() == "" )
							available=1;

						else
							available=0;

//						Serial.print( "available : " ); Serial.println( available );
					}

					else
					{
						uint8_t ch;
						available = 1;
						p = receiveBuffer+4;
						kp = (uint8_t *)Password().c_str(); 
						while (true)
						{
							ch=*kp++;
							if( ch!=*p++ )
								available=0;

							if( !ch )
								break;
						}                               
					}

					if( available != 0 )
					{
						sendPackage(command, C_OWNER.GetConfig(), C_OWNER.GetConfLength(),  1);
						C_OWNER.SetConnectedFlag( C_INDEX, true );
						T_ConnectedOutputPin::SetPinValue( true, true );
					}

					else
					{
						uint8_t buf[4];
						p = buf;
						kp = C_OWNER.GetConfig();
						i= C_OWNER.GetConfVersion() >= 5 ? 3 : 2;
						length = i+1;
						while (i--)
							*p++ = T_OWNER::getConfByte(kp++);

						*p++ = 0xf0;
						sendPackage (command, buf, length,  0);
					}

					break;   

			  case 0x40:  
					sendPackage (command, C_OWNER.GetData(), C_OWNER.GetInputLength() + C_OWNER.GetOutputLength(), 0); 
					break;

			  case 0x80:  
					if (length == C_OWNER.GetInputLength() )
					{
						p=receiveBuffer+4;
						kp = C_OWNER.GetData();
						i = C_OWNER.GetInputLength();
						while (i--)
							*kp++=*p++;
					}

					sendPackage( command, 0, 0, 0 );
					break;

				case 0xC0:  
					sendPackage (command, C_OWNER.GetData() + C_OWNER.GetInputLength(), C_OWNER.GetOutputLength(), 0);
					break;

/*
#if defined(REMOTEXY_CLOUD)
				case 0x10: // echo
					sendPackage (command, 0, 0, 0);
					break;

				case 0x11:
					if (cloudState==REMOTEXY_CLOUD_STATE_REGISTRATION)
						setCloudState (REMOTEXY_CLOUD_STATE_WORKING);

					break;   
#endif //REMOTEXY_CLOUD       
*/
				default:
					if( ! T_INTERFACE::handleReceivePackage( *this, command ) )
						return 0;
			}  
    
			wireTimeOut=millis();    
//#if defined(REMOTEXY_CLOUD)  
//			if (cloudState==REMOTEXY_CLOUD_STATE_WORKING)
//			  cloudTimeOut=millis();

//#endif //REMOTEXY_CLOUD       
			return 1;
		}

	private:
		void resetWire() 
		{
			receiveIndex = 0; 
			receiveCRC = initCRC();
			C_OWNER.SetConnectedFlag( C_INDEX, false );
			T_ConnectedOutputPin::SetPinValue( false, true );
			wireTimeOut = millis();
		}

	public:
		inline void SystemStart()
		{
			init(); //, Password().c_str() );
			T_ConnectedOutputPin::SetPinValue( false, false );
		}

		inline void SystemLoopBegin()
		{
//			inherited::handler();
			T_INTERFACE::Handler( *this );
			if( millis() - wireTimeOut > REMOTEXY_TIMEOUT )
				resetWire();

		}

	};
//---------------------------------------------------------------------------
	template <
		typename T_OBJECT,
		T_OBJECT &C_OBJECT
	> class RemoteXY_Serial
	{
	public:
		template<typename T_TEMOTE_XY, typename T_PROTOCOL> inline void Init( T_TEMOTE_XY &AOwner, T_PROTOCOL &AProtocol ) {}
		template<typename T> inline void Handler( T &AOwner ) {}

		inline void ByteReceived() {};

		template<typename T> inline bool handleReceivePackage( T &AOwner, uint8_t command ) {}

		inline void sendStart (uint16_t len) 
		{
		}

		inline void sendByte( uint8_t b )
		{
			C_OBJECT.GetStream().write( b );
		}

/*
		inline uint8_t receiveByte() 
		{
			return C_OBJECT.GetStream().read();
		}

		inline uint8_t availableByte() 
		{
			return C_OBJECT.GetStream().available();
		};  
*/
	};
//---------------------------------------------------------------------------
	template <
		typename T_SOCKET, T_SOCKET &C_SOCKET,
		uint16_t C_MaxPacketSize
	> class RemoteXY_TCP
	{
		static const uint16_t REMOTEXY_SERVER_TIMEOUT = 7000;

	protected:
		uint32_t serverTimeOut;

		uint8_t sendBuffer[ C_MaxPacketSize ];
		uint16_t sendBufferCount;
		uint16_t sendBytesAvailable;  

	protected:
		inline void resetServerTimeOut()
		{
			serverTimeOut = millis(); //REMOTEXY_SERVER_TIMEOUT;
		}

	public:
		template<typename T_TEMOTE_XY, typename T_PROTOCOL> inline void Init( T_TEMOTE_XY &AOwner, T_PROTOCOL &AProtocol )
		{
			serverTimeOut = 0;
		}

		template<typename T> inline void Handler( T &AOwner )
		{
			if( C_SOCKET.IsConnected() )
				if( millis() - serverTimeOut > REMOTEXY_SERVER_TIMEOUT )
					C_SOCKET.StopSocket();
		}

		inline void ByteReceived() 
		{
			resetServerTimeOut();
		};

		template<typename T> inline bool handleReceivePackage( T &AOwner, uint8_t command ) {}

		inline void sendStart( uint16_t len )
		{
//			Serial.print( "sendStart : " );	Serial.println( len );
			sendBytesAvailable = len;
			sendBufferCount = 0;
		}

		void sendByte( uint8_t b ) // Do not inline!
		{
			sendBuffer[sendBufferCount++] = b;
			sendBytesAvailable --;    

//			Serial.print( "sendBufferCount = " );
//			Serial.println( sendBufferCount );

			if(( sendBufferCount == C_MaxPacketSize ) || ( sendBytesAvailable==0 )) 
			{
//				Serial.println( "SEND 1" );
//				uint8_t buf[sendBufferCount];
//				for( uint16_t i=0; i<sendBufferCount; i++ )
//					buf[i]=sendBuffer[i];

				if( C_SOCKET.CanSend() )
				{
//					Serial.println( "SEND 2" );
					C_SOCKET.BeginPacket();
					C_SOCKET.GetPrint()->write( sendBuffer, sendBufferCount );
					C_SOCKET.EndPacket();
				}

				sendBufferCount=0;
				resetServerTimeOut();
			}
		}

	};
//---------------------------------------------------------------------------
	template <
		typename T_SOCKET, T_SOCKET &C_SOCKET,
		typename T_ConfugureSocket,
		uint16_t C_MaxPacketSize,
		typename T_Token
	> class RemoteXY_Cloud :
		public T_ConfugureSocket,
		public T_Token
	{
		static const uint8_t REMOTEXY_CLOUD_STATE_STOP = 0;
		static const uint8_t REMOTEXY_CLOUD_STATE_WAIT_RECONNECT = 1;
//		static const uint8_t REMOTEXY_CLOUD_STATE_WAIT_NEXT_TRY = 2;
		static const uint8_t REMOTEXY_CLOUD_STATE_CONNECTION = 3;
		static const uint8_t REMOTEXY_CLOUD_STATE_REGISTRATION = 6;
		static const uint8_t REMOTEXY_CLOUD_STATE_WORKING = 7;

//		static const uint16_t REMOTEXY_CLOUD_RETRY_TIMEOUT = 500;
		static const uint16_t REMOTEXY_CLOUD_CONNECT_TIMEOUT = 10000;
		static const uint16_t REMOTEXY_CLOUD_RECONNECT_TIMEOUT = 30000;
		static const uint16_t REMOTEXY_CLOUD_ECHO_TIMEOUT = 60000;

	public:
		_V_PROP_( ConfugureSocket )
		_V_PROP_( Token )

	protected:
		uint8_t sendBuffer[ C_MaxPacketSize ];
		uint16_t sendBufferCount;
		uint16_t sendBytesAvailable;  

		uint8_t cloudRegistPackage[ 38 ];
		uint8_t cloudState;   
		uint32_t cloudTimeOut;

	protected:
		void setCloudState( uint8_t state )
		{
			cloudState = state;
			cloudTimeOut = millis(); 
#if defined(REMOTEXY__DEBUGLOGS)
			switch (state)
			{
				case REMOTEXY_CLOUD_STATE_WAIT_RECONNECT: 
					DEBUGLOGS_write("Waiting to reconnect to the cloud server");
					break;

//				case REMOTEXY_CLOUD_STATE_WAIT_NEXT_TRY: 
//					DEBUGLOGS_write("Waiting to next try to connect to the cloud server");
//					break;

				case REMOTEXY_CLOUD_STATE_CONNECTION: 
					DEBUGLOGS_write("Started connecting to cloud server");
					break;

				case REMOTEXY_CLOUD_STATE_REGISTRATION: 
					DEBUGLOGS_write("Waiting for registration on cloud server");
					break;

				case REMOTEXY_CLOUD_STATE_WORKING: 
					DEBUGLOGS_write("Connect to the cloud server successfully");
					break;

				default:
					DEBUGLOGS_write("Unknown cloud state ");
					REMOTEXY__DEBUGLOGS.print(cloudState);
					break;

			}
#endif
		}

	public:
		inline void ByteReceived() 
		{
			if( cloudState == REMOTEXY_CLOUD_STATE_WORKING )
			  cloudTimeOut = millis();

		};

		template<typename T> inline bool handleReceivePackage( T &AOwner, uint8_t command )
		{
			switch( command )
			{
				case 0x10: // echo
					AOwner.sendPackage( command, 0, 0, 0 );
					break;

				case 0x11:
					if( cloudState == REMOTEXY_CLOUD_STATE_REGISTRATION )
						setCloudState( REMOTEXY_CLOUD_STATE_WORKING );

					break;   

				default:
					return 0;
			}

			return 1;
		}

		template<typename T_TEMOTE_XY, typename T_PROTOCOL> inline void Init( T_TEMOTE_XY &AOwner, T_PROTOCOL &AProtocol )
		{
			uint8_t *p = cloudRegistPackage;
			*p++ = AOwner.getConfByte( AOwner.GetConfig() );
			*p++ = 0;
			char *_cloudToken = (char *)Token().c_str();
			for ( uint8_t i = 0; i < 32; i++ ) 
			{
				if( *_cloudToken == 0 )
					*(p++) = 0;

				else
					*(p++) = *(_cloudToken++);
			}

			uint16_t *len = (uint16_t*)p;
			*len = AOwner.GetOutputLength() + AOwner.GetInputLength();
			uint16_t confLength = AOwner.GetConfLength();
			if( confLength > *len )
				*len = confLength;   

			*len += 6 + 1;
			len = (uint16_t*)( p + 2 );
			*len = AProtocol.GetReceiveBufferLength();

//			Serial.print( "*len = " ); Serial.println( *len );
    
			cloudState = REMOTEXY_CLOUD_STATE_STOP;
		}

		inline bool connectServerCloud()
		{
//			Serial.println( "connectServerCloud" );
			if( ConfugureSocket() )
			{
				C_SOCKET.Host() = "cloud.remotexy.com";
				C_SOCKET.Port() = 6376;

//				Serial.println( C_SOCKET.Host().c_str() );
			}

//			C_SOCKET.Enabled() = true;
			C_SOCKET.TryStartSocket();
			return C_SOCKET.CanSend();
		}

		inline void stopCloud()
		{
			C_SOCKET.StopSocket();
		}

		template<typename T> inline void Handler( T &AOwner )
		{
/*
			static uint8_t Test1 = 88;
			if( Test1 != cloudState )
			{
				Serial.print( "Handler : " );
				Serial.println( cloudState );
			}

			Test1 = cloudState;
*/
//			if( ! moduleRunning )
//				return;

			switch( cloudState )
			{      
				case REMOTEXY_CLOUD_STATE_STOP:  
					if( connectServerCloud() )
						setCloudState( REMOTEXY_CLOUD_STATE_CONNECTION );

					break;

				case REMOTEXY_CLOUD_STATE_WAIT_RECONNECT:  
					if( millis() - cloudTimeOut > REMOTEXY_CLOUD_RECONNECT_TIMEOUT )
						setCloudState (REMOTEXY_CLOUD_STATE_CONNECTION);

					break;
        
//				case REMOTEXY_CLOUD_STATE_WAIT_NEXT_TRY:
//					if( millis() - cloudTimeOut > REMOTEXY_CLOUD_RETRY_TIMEOUT )
//						setCloudState (REMOTEXY_CLOUD_STATE_CONNECTION);

//					break;
        
				case REMOTEXY_CLOUD_STATE_CONNECTION:
					if( connectServerCloud() )
					{          
						setCloudState( REMOTEXY_CLOUD_STATE_REGISTRATION );
//						for( auto i = 0; i < 38; ++ i )
//							Serial.println( cloudRegistPackage[ i ], HEX );

						AOwner.sendPackage( 0x11, cloudRegistPackage, 38, 0 );
					}

					else
						setCloudState( REMOTEXY_CLOUD_STATE_WAIT_RECONNECT );

					break;
        
				case REMOTEXY_CLOUD_STATE_REGISTRATION:  
					if( millis() - cloudTimeOut > REMOTEXY_CLOUD_CONNECT_TIMEOUT )
						stopCloud();

					break;
        
				case REMOTEXY_CLOUD_STATE_WORKING:  
					if( millis() - cloudTimeOut > REMOTEXY_CLOUD_ECHO_TIMEOUT )
						stopCloud();

					break;
			}
		}

		inline void sendStart( uint16_t len )
		{
//			Serial.print( "sendStart : " );	Serial.println( len );
			sendBytesAvailable = len;
			sendBufferCount = 0;
		}

		void sendByte( uint8_t b ) // Do not inline!
		{
			sendBuffer[sendBufferCount++] = b;
			sendBytesAvailable --;    

//			Serial.print( "sendBufferCount = " );
//			Serial.println( sendBufferCount );

			if(( sendBufferCount == C_MaxPacketSize ) || ( sendBytesAvailable==0 )) 
			{
//				Serial.println( "SEND 1" );
//				uint8_t buf[sendBufferCount];
//				for( uint16_t i=0; i<sendBufferCount; i++ )
//					buf[i]=sendBuffer[i];

				if( C_SOCKET.CanSend() )
				{
//					Serial.println( "SEND 2" );
					C_SOCKET.BeginPacket();
					C_SOCKET.GetPrint()->write( sendBuffer, sendBufferCount );
					C_SOCKET.EndPacket();
				}

				sendBufferCount=0;
			}
		}

	};
//---------------------------------------------------------------------------
	template <
		typename T_OWNER, T_OWNER &C_OWNER,
		typename T_InitialValue,
		typename T_OutputPin,
		typename T_UseInitialValueOnDisconnect
	> class MitovRemoteXYDigitalOutput :
		public T_InitialValue,
		public T_OutputPin,
		public T_UseInitialValueOnDisconnect
	{
	public:
		_V_PIN_( OutputPin )

	public:
		_V_PROP_( InitialValue )
		_V_PROP_( UseInitialValueOnDisconnect )

	public:
		inline void SystemStart()
		{
			T_OutputPin::SetPinValue( InitialValue(), false );
		}

		inline void SystemLoopBegin( uint8_t &AValue )
		{
			if( C_OWNER.GetIsConnected() )
				T_OutputPin::SetPinValue( AValue != 0, true );

			else if( UseInitialValueOnDisconnect().GetValue() )
				T_OutputPin::SetPinValue( InitialValue(), true );

		}

	};
//---------------------------------------------------------------------------
	template <
		typename T_OWNER, T_OWNER &C_OWNER,
		typename T_InitialValue,
		typename T_OutputPin,
		typename T_UseInitialValueOnDisconnect
	> class MitovRemoteXYSlider :
		public T_InitialValue,
		public T_OutputPin,
		public T_UseInitialValueOnDisconnect
	{
	public:
		_V_PIN_( OutputPin )

	public:
		_V_PROP_( InitialValue )
		_V_PROP_( UseInitialValueOnDisconnect )

	public:
		inline void SystemStart()
		{
			T_OutputPin::SetPinValue( InitialValue(), false );
		}

		inline void SystemLoopBegin( uint8_t &AValue )
		{
			if( C_OWNER.GetIsConnected() )
				T_OutputPin::SetPinValue( float( AValue ) / 100, true );

			else if( UseInitialValueOnDisconnect().GetValue() )
				T_OutputPin::SetPinValue( InitialValue(), true );

		}

	};
//---------------------------------------------------------------------------
	template <
		typename T_OWNER, T_OWNER &C_OWNER,
		typename T_InitialValue,
		typename T_OutputPin,
		typename T_UseInitialValueOnDisconnect
	> class MitovRemoteXYSelect :
		public T_InitialValue,
		public T_OutputPin,
		public T_UseInitialValueOnDisconnect
	{
	public:
		_V_PIN_( OutputPin )

	public:
		_V_PROP_( InitialValue )
		_V_PROP_( UseInitialValueOnDisconnect )

	public:
		inline void SystemStart()
		{
			T_OutputPin::SetPinValue( InitialValue(), false );
		}

		inline void SystemLoopBegin( uint8_t &AValue )
		{
			if( C_OWNER.GetIsConnected() )
				T_OutputPin::SetPinValue( AValue, true );

			else if( UseInitialValueOnDisconnect().GetValue() )
				T_OutputPin::SetPinValue( InitialValue(), true );

		}

	};
//---------------------------------------------------------------------------
	template <
		typename T_OWNER, T_OWNER &C_OWNER,
		typename T_InitialX,
		typename T_InitialY,
		typename T_OutputPins_X,
		typename T_OutputPins_Y,
		typename T_UseInitialValueOnDisconnect
	> class MitovRemoteXYJoystick :
		public T_InitialX,
		public T_InitialY,
		public T_OutputPins_X,
		public T_OutputPins_Y,
		public T_UseInitialValueOnDisconnect
	{
	public:
		_V_PIN_( OutputPins_X )
		_V_PIN_( OutputPins_Y )

	public:
		_V_PROP_( InitialX )
		_V_PROP_( InitialY )
		_V_PROP_( UseInitialValueOnDisconnect )

	public:
		inline void SystemStart()
		{
			T_OutputPins_X::SetPinValue( InitialX(), false );
			T_OutputPins_Y::SetPinValue( InitialY(), false );
		}

		inline void SystemLoopBegin( int8_t &AValueX, int8_t &AValueY )
		{
			if( C_OWNER.GetIsConnected() )
			{
				T_OutputPins_X::SetPinValue( ( float( AValueX ) + 100 ) / 200, true );
				T_OutputPins_Y::SetPinValue( ( float( AValueY ) + 100 ) / 200, true );
			}

			else if( UseInitialValueOnDisconnect().GetValue() )
			{
				T_OutputPins_X::SetPinValue( InitialX(), true );
				T_OutputPins_Y::SetPinValue( InitialY(), true );
			}

		}

	};
//---------------------------------------------------------------------------
	template <
		typename T_OWNER, T_OWNER &C_OWNER,
		typename T_InitialValue,
		typename T_OutputPin,
		typename T_UseInitialValueOnDisconnect
	> class MitovRemoteXYColor :
		public T_InitialValue,
		public T_OutputPin,
		public T_UseInitialValueOnDisconnect
	{
	public:
		_V_PIN_( OutputPin )

	public:
		_V_PROP_( InitialValue )
		_V_PROP_( UseInitialValueOnDisconnect )

	public:
		inline void SystemStart()
		{
			T_OutputPin::SetPinValue( InitialValue().GetValue(), false );
		}

		inline void SystemLoopBegin( uint8_t &AValueR, uint8_t &AValueG, uint8_t &AValueB )
		{
			if( C_OWNER.GetIsConnected() )
				T_OutputPin::SetPinValue( TColor( AValueR, AValueG, AValueB ), true );

			else if( UseInitialValueOnDisconnect().GetValue() )
				T_OutputPin::SetPinValue( InitialValue().GetValue(), true );

		}

	};
//---------------------------------------------------------------------------
	template <
		typename T_OWNER, T_OWNER &C_OWNER,
		typename T_InitialValue,
		typename T_OutputPin,
		typename T_UseInitialValueOnDisconnect
	> class MitovRemoteXYEdit :
		public T_InitialValue,
		public T_OutputPin,
		public T_UseInitialValueOnDisconnect
	{
	public:
		_V_PIN_( OutputPin )

	public:
		_V_PROP_( InitialValue )
		_V_PROP_( UseInitialValueOnDisconnect )

	public:
		inline void SystemStart()
		{
			T_OutputPin::SetPinValue( InitialValue(), false );
		}

		inline void SystemLoopBegin( char *AValue )
		{
			if( C_OWNER.GetIsConnected() )
				T_OutputPin::SetPinValue( AValue, true );

			else if( UseInitialValueOnDisconnect().GetValue() )
				T_OutputPin::SetPinValue( InitialValue(), true );

		}

	};
//---------------------------------------------------------------------------
	template <
		typename T_IMPLEMENTATION
	> class MitovRemoteXYLed
	{
	public:
		inline void InputPin_o_Receive( void *_Data )
		{
			T_IMPLEMENTATION::SetRGB( *(( TColor * )_Data ) );
		}

	};
//---------------------------------------------------------------------------
	template <
		typename T_IMPLEMENTATION
	> class MitovRemoteXYLevel
	{
	public:
		inline void InputPin_o_Receive( void *_Data )
		{
			int8_t AValue = constrain( *(float *)_Data, 0.0, 1.0 ) * 100 + 0.5;
			T_IMPLEMENTATION::SetValue( AValue );
		}

	};
//---------------------------------------------------------------------------
	template <
		typename T_IMPLEMENTATION
	> class MitovRemoteXYText
	{
	public:
		inline void InputPin_o_Receive( void *_Data )
		{
			T_IMPLEMENTATION::SetValue( (char *)_Data );
		}

	};
//---------------------------------------------------------------------------
	template <
		typename T_IMPLEMENTATION,
		uint8_t COUNT_InputPins
	> class MitovRemoteXYGraph
	{
	public:
		inline void InputPins_o_Receive( uint8_t AIndex, void *_Data )
		{
			T_IMPLEMENTATION::SetValue( AIndex, *(float *)_Data );
		}

	};
//---------------------------------------------------------------------------
}

#ifdef __TEST_FOR_DEBUG_PRINTS__
#undef Serial
#endif

#endif // _MITOV_REMOTE_XY_h
