/*
||
|| @file Keypad_tw.h
|| @version 1.0 - Convert Keypad_I2C to use TinyWireM library for ATtiny micros
|| @version 2.0 - PCF8575 support added by Paul Williamson
|| @author G. D. (Joe) Young, ptw
|| @contact "G. D. (Joe) Young" <gdyoung@islandnet.com>
||
|| @description
|| | Keypad_tw provides an interface for using matrix keypads that
|| | are attached with I2C port expanders. It supports multiple keypads,
|| | user selectable pins, and user defined keymaps.
|| |
|| | This version replaces all references to Wire functions (TwoWire) with
|| | their TinyWireM (USI_TWI) equivalents
|| #
||
|| @license
|| | This library is free software; you can redistribute it and/or
|| | modify it under the terms of the GNU Lesser General Public
|| | License as published by the Free Software Foundation; version
|| | 2.1 of the License.
|| |
|| | This library is distributed in the hope that it will be useful,
|| | but WITHOUT ANY WARRANTY; without even the implied warranty of
|| | MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
|| | Lesser General Public License for more details.
|| |
|| | You should have received a copy of the GNU Lesser General Public
|| | License along with this library; if not, write to the Free Software
|| | Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
|| #
||
*/

#include "Keypad_tw.h"

// Let the user define a keymap - assume the same row/column count as defined in constructor
void Keypad_tw::begin(char *userKeymap) {
    Keypad::begin(userKeymap);
	pinState = pinState_set( );
}


// Initialize tw
void Keypad_tw::begin(void) {
//	pinState = 0xff;
	pinState = pinState_set( );
}
#if 0
// Initialize tw							//TinyWireM does not allow re-assigning bus adr
void Keypad_tw::begin(byte address) {
	twaddr = address;
	USI_TWI::begin(address);
//	pinState = 0xff;
	pinState = pinState_set( );
}

// Initialize tw
void Keypad_tw::begin(int address) {
	twaddr = address;
	USI_TWI::begin(address);
//	pinState = 0xff;
	pinState = pinState_set( );
}
#endif

void Keypad_tw::pin_write(byte pinNum, boolean level) {
	word mask = 1<<pinNum;
	if( level == HIGH ) {
		pinState |= mask;
	} else {
		pinState &= ~mask;
	}
	port_write( pinState );
} // twxWrite( )


int Keypad_tw::pin_read(byte pinNum) {
	word mask = 0x1<<pinNum;
	Wire.requestFrom((int)twaddr, (int)twwidth);
	word pinVal = Wire.read( );
	if (twwidth > 1) {
		pinVal |= Wire.read( ) << 8;
	} 
	pinVal &= mask;
	if( pinVal == mask ) {
		return 1;
	} else {
		return 0;
	}
}

void Keypad_tw::port_write( word twportval ) {
	Wire.beginTransmission((int)twaddr);
	Wire.write( twportval & 0x00FF);
	if (twwidth > 1) {
		Wire.write( twportval >> 8 );
	}
	Wire.endTransmission();
	pinState = twportval;
} // port_write( )

word Keypad_tw::pinState_set( ) {
	Wire.requestFrom( (int)twaddr, (int)twwidth );
	pinState = Wire.read( );
	if (twwidth > 1) {
		pinState |= Wire.read( ) << 8;
	}
	return pinState;
} // set_pinState( )


/*
|| @changelog
|| | 1.0 2015-05-24 - Joe Young : initial conversion from Wire to TinyWireM
|| |
|| | 2.0 2013-08-31 - Paul Williamson : Added i2cwidth parameter for PCF8575 support
|| |
|| | 1.0 2012-07-12 - Joe Young : Initial Release
|| #
*/
