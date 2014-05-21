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

typedef struct _Uart
{
#if PLATFORM == PLATFORM_WINDOWS
    HANDLE  handle;
#else
    int     handle;
#endif
} Uart;

/** @brief Init UART
 *
 * @param comport	Com port number
 * @param baudrate  Baudrate
 * @return 0 if no error.
 *
 */
extern uint32_t initUART(Uart * uart, const char * comport, uint32_t baudrate);

    
#if PLATFORM == PLATFORM_MAC || PLATFORM == PLATFORM_UNIX

	#include <termios.h>

	speed_t uint32_tToSpeed_t(uint32_t baudrate);
#endif

/** @brief Send a byte
 *
 * @param byte	Byte to send
 * @return 0 if no error.
 */
extern uint32_t sendByteToUART(Uart * uart, uint8_t byte);

/** @brief Send a buffer
 *
 * @param Buffer	Buffer to send
 * @param Count		Number of byte to send
 * @return 0 if no error
 */
extern uint32_t sendBufferToUART (Uart * uart, uint8_t *Buffer, uint32_t Count);

/** @brief Get a byte
 *
 * @param [out] data	Pointer to a byte
 * @return 0 if no error.
 */
extern uint32_t  getByteFromUART(Uart * uart, uint8_t *data);

/** @brief Get a byte (with timeout)
 *
 * @param [out] data	Pointer to a byte
 * @return 0 if no error.
 */
extern uint32_t getByteFromUARTNoWait(Uart * uart, uint8_t *data);

/** @brief Get a buffer
 *
 * @param Buffer	Array where store data
 * @param Count		Number of byte to read
 * @return 0 if no error.
 */
extern uint32_t  getBufferFromUART (Uart * uart, uint8_t *Buffer,uint32_t Count);

extern void closeUART(Uart * uart);

// implemented on Linux/Mac OS X
extern void UARTSetDTR(Uart * uart);
extern void UARTClearDTR(Uart * uart );

extern void UARTSetRTS(Uart * uart);
extern void UARTClearRTS(Uart * uart);

extern uint32_t UARTGetStatus(Uart * uart);

extern uint8_t UARTisDSRset(Uart * uart);
extern uint8_t UARTisDTRset(Uart * uart);
extern uint8_t UARTisRTSset(Uart * uart);
extern uint8_t UARTisCTSset(Uart * uart);
extern uint8_t UARTisDCDset(Uart * uart);
extern uint8_t UARTisRNGset(Uart * uart);



#ifdef __cplusplus
 }
#endif

#endif
