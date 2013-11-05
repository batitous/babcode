// Copyright (c) 2013, Baptiste Burles
//
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// * Redistributions of source code must retain the above copyright
//   notice, this list of conditions and the following disclaimer.
// * Redistributions in binary form must reproduce the above copyright
//   notice, this list of conditions and the following disclaimer in the
//   documentation and/or other materials provided with the distribution.
// * Neither the name of Baptiste Burles nor the
//   names of its contributors may be used to endorse or promote products
//   derived from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY Baptiste Burles AND CONTRIBUTORS ``AS IS'' AND ANY
// EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL Baptiste Burles AND CONTRIBUTORS BE LIABLE FOR ANY
// DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#ifndef BABCODE_UART_H
#define BABCODE_UART_H

#ifdef __cplusplus
extern "C" {
#endif

#define UART_OK				0		/**< Ok */
#define UART_OPEN_FAILED	0x101	/**< Failed to open comport */
#define UART_CONF_FAILED	0x102	/**< Failed to configure comport */
#define UART_WRITE_FAILED	0x103	/**< Write failed */
#define UART_TIMEOUT_ERROR	0x104	/**< Last operation timeout */
#define UART_READ_FAILED	0x105	/**< Read failed */


/** @brief Init UART
 *
 * @param comport	Com port number
 * @param baudrate  Baudrate
 * @return 0 if no error.
 *
 */
extern UInt32 initUART(const UInt8 * comport, UInt32 baudrate);

    
#if PLATFORM == PLATFORM_MAC || PLATFORM == PLATFORM_UNIX

	#include <termios.h>

	speed_t UInt32ToSpeed_t(UInt32 baudrate);
#endif

/** @brief Send a byte
 *
 * @param byte	Byte to send
 * @return 0 if no error.
 */
extern UInt32 sendByteToUART(UInt8 byte);

/** @brief Send a buffer
 *
 * @param Buffer	Buffer to send
 * @param Count		Number of byte to send
 * @return 0 if no error
 */
extern UInt32 sendBufferToUART (UInt8 *Buffer, UInt32 Count);

/** @brief Get a byte
 *
 * @param [out] data	Pointer to a byte
 * @return 0 if no error.
 */
extern UInt32  getByteFromUART(UInt8 *data);

/** @brief Get a byte (with timeout)
 *
 * @param [out] data	Pointer to a byte
 * @return 0 if no error.
 */
extern UInt32 getByteFromUARTNoWait(UInt8 *data);

/** @brief Get a buffer
 *
 * @param Buffer	Array where store data
 * @param Count		Number of byte to read
 * @return 0 if no error.
 */
extern UInt32  getBufferFromUART (UInt8 *Buffer,UInt32 Count);

extern void closeUART(void);

// implemented on Linux/Mac OS X
extern void UARTSetDTR(void);
extern void UARTClearDTR(void);

extern void UARTSetRTS(void);
extern void UARTClearRTS(void);

extern UInt32 UARTGetStatus(void);

extern UInt8 UARTisDSRset(void);
extern UInt8 UARTisDTRset(void);
extern UInt8 UARTisRTSset(void);
extern UInt8 UARTisCTSset(void);
extern UInt8 UARTisDCDset(void);
extern UInt8 UARTisRNGset(void);



#ifdef __cplusplus
 }
#endif

#endif
