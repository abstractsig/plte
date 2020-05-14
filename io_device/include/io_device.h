/*
 *
 * pinky io device
 *
 *                        .--------------------.
 *                  Reset |16 0.18             |
 *                        |15 3v3              |
 *            (Button 1)  |14 0.11             |
 *                        |13 0v               |
 *                        |12 0.03     Vbat 12 | 
 *                        |11 0.04     Pen  11 | 
 *                        |10 0.28     Vusb 10 | 
 *                        | 9 0.29     1.03  9 | 
 *                        | 8 0.30     1.12  8 | (Blue LED)
 *              SS        | 7 0.31     1.11  7 |
 *              SCLK      | 6 1.15     1.10  6 |
 *              MOSI      | 5 1.13     1.08  5 |
 *              MISO      | 4 1.14     1.02  4 | 
 *     Console Uart Rx    | 3 0.08     1.01  3 |
 *     Console Uart Tx    | 2 0.06     0.27  2 |  SCL
 *                        | 1 nc       0.26  1 |  SDA
 *                        `--------------------'
 *
 *
 */
#ifndef io_device_H_
#define io_device_H_
#include <io_board.h>
#include <io_graphics.h>
#include <sockets/io_beacon_socket.h>

io_cpu_clock_pointer_t io_device_get_core_clock (io_t*);
io_socket_t* io_device_get_socket (io_t*,int32_t);

#define SPECIALISE_IO_DEVICE_IMPLEMENTATION(S) \
	SPECIALISE_IO_BOARD_IMPLEMENTATION(S) \
	.get_core_clock = io_device_get_core_clock,\
	.get_socket = io_device_get_socket,\
	.get_shared_key = lookup_shared_key,\
	/**/


//
// allocate GPIOTE channels (0..7)
//
#define SPI_INTERRUPT_GPIOTE_CHANNEL		2

//
// allocate PPI channels (0..19)
//
#define TIME_CLOCK_PPI_CHANNEL					1

#define OLED_FEATHER_I2C_ADDRESS					0x3c
#define OLED_FEATHER_LCDWIDTH						128
#define OLED_FEATHER_LCDHEIGHT					32

enum {
	USART0,
	USART1,
	SPI0,
	QSPI_SOCKET,

	RADIO_SOCKET,
	RADIO_BEACON_SOCKET,
	
	NUMBER_OF_IO_SOCKETS // capture the socket count for this device
};

typedef struct PACK_STRUCTURE device_io_t {
	NRF52840_IO_CPU_STRUCT_MEMBERS

	io_socket_t* sockets[NUMBER_OF_IO_SOCKETS];
	
} device_io_t;

io_t*	initialise_device_io (void);
bool	test_device (io_t*,vref_t);
bool	start_io_device (io_t*);

#ifdef IMPLEMENT_IO_DEVICE
//-----------------------------------------------------------------------------
//
// Implementation
//
//-----------------------------------------------------------------------------
#include <io_device_values.h>

#define UMM_VALUE_MEMORY_HEAP_SIZE		KB(4)
#define UMM_BYTE_MEMORY_SIZE				(KB(200) + UMM_VALUE_MEMORY_HEAP_SIZE)
#define UMM_SECTION_DESCRIPTOR      	__attribute__ ((section(".umm")))

static uint8_t ALLOCATE_ALIGN(8) UMM_SECTION_DESCRIPTOR
heap_byte_memory_bytes[UMM_BYTE_MEMORY_SIZE];
io_byte_memory_t
heap_byte_memory = {
	.heap = (umm_block_t*) heap_byte_memory_bytes,
	.number_of_blocks = (UMM_BYTE_MEMORY_SIZE / sizeof(umm_block_t)),
};

static io_value_memory_t* value_memories[1] = {0};
io_value_memory_t*
io_get_value_memory_by_id (uint32_t id) {
	if (id < SIZEOF(value_memories)) {
		return value_memories[id];
	} else {
		return NULL;
	}
}

bool
register_io_value_memory (io_value_memory_t *vm) {
	if (value_memories[io_value_memory_id(vm)] == NULL) {
		value_memories[io_value_memory_id(vm)] = vm;
		return true;
	} else {
		return false;
	}
}

io_cpu_clock_pointer_t
io_device_get_core_clock (io_t *io) {
	extern EVENT_DATA nrf52_core_clock_t cpu_core_clock;
	return IO_CPU_CLOCK(&cpu_core_clock);
}

io_socket_t*
io_device_get_socket (io_t *io,int32_t handle) {
	if (handle >= 0 && handle < NUMBER_OF_IO_SOCKETS) {
		device_io_t *this = (device_io_t*) io;
		return this->sockets[handle];
	} else {
		return NULL;
	}
}

/*
 *-----------------------------------------------------------------------------
 *
 * CPU Clock Tree
 *
 *    +----------+                      64MHz   +--------+
 *    | XTAL/1   |-+--------------------------->| CPU    |		          
 *    +----------+ |                            +--------+
 *                 |
 *                 |       +---------+          +---------+
 *                 +------>| APB0    |--------->| UART1   |		// console uart
 *                 |       +---------+   |      +---------+
 *                 |                     |
 *                 |                     |      +---------+
 *                 |                     |----->| RTC     |		// time clock
 *                 |                     |      +---------+
 *
 *-----------------------------------------------------------------------------
 */
extern EVENT_DATA nrf52_core_clock_t cpu_core_clock;

EVENT_DATA nrf52_oscillator_t crystal_oscillator = {
	.implementation = &nrf52_crystal_oscillator_implementation,
	.outputs = (const io_cpu_clock_pointer_t []) {
		decl_io_cpu_clock_pointer(&cpu_core_clock),
		{NULL}
	},
};

EVENT_DATA nrf52_core_clock_t cpu_core_clock = {
	.implementation = &nrf52_core_clock_implementation,
	.input = IO_CPU_CLOCK(&crystal_oscillator),
};

/*
static nrf52_uart_t uart0 = {
	.implementation = &nrf52_uart_implementation,
	.address = io_invalid_address (),
	.encoding = IO_ENCODING_IMPLEMENATAION (&io_text_encoding_implementation),
	
	.uart_registers = NRF_UARTE0,
	.interrupt_number = UARTE0_UART0_IRQn,
	.baud_rate = UARTE_BAUDRATE_BAUDRATE_Baud115200,
	
	.tx_pin = def_nrf_io_alternate_pin(0,6,0),
	.rx_pin = def_nrf_io_alternate_pin(0,8,0),
	.rts_pin = def_nrf_gpio_null_pin(),
	.cts_pin = def_nrf_gpio_null_pin(),

};
*/

static io_socket_t*
uart0_socket (io_t *io,io_address_t address) {
	static nrf52_uart_t uart0 = {
		.implementation = &nrf52_uart_implementation,
		.address = io_invalid_address (),
		.encoding = &io_text_encoding_implementation,
		
		.uart_registers = NRF_UARTE0,
		.interrupt_number = UARTE0_UART0_IRQn,
		.baud_rate = UARTE_BAUDRATE_BAUDRATE_Baud115200,
		
		.tx_pin = def_nrf_io_alternate_pin(0,6,0),
		.rx_pin = def_nrf_io_alternate_pin(0,8,0),
		.rts_pin = def_nrf_gpio_null_pin(),
		.cts_pin = def_nrf_gpio_null_pin(),

	};
	return (io_socket_t*) &uart0;
}

static EVENT_DATA io_settings_t console_uart_settings = {
	.encoding = IO_ENCODING_IMPLEMENATAION (&io_text_encoding_implementation),
	.transmit_pipe_length = 18,
	.receive_pipe_length = 128,
};

static EVENT_DATA io_settings_t default_uart_settings = {
	.encoding = IO_ENCODING_IMPLEMENATAION (&io_text_encoding_implementation),
	.transmit_pipe_length = 5,
	.receive_pipe_length = 128,
};

static io_socket_t*
uart1_socket (io_t *io,io_address_t address) {
	static nrf52_uart_t uart1 = {
		.implementation = &nrf52_uart_implementation,
		.address = io_invalid_address (),
		.encoding = IO_ENCODING_IMPLEMENATAION (&io_text_encoding_implementation),
		
		.uart_registers = NRF_UARTE1,
		.interrupt_number = UARTE1_IRQn,
		.baud_rate = UARTE_BAUDRATE_BAUDRATE_Baud115200,
		
		.tx_pin = def_nrf_io_alternate_pin(1,2,0),
		.rx_pin = def_nrf_io_alternate_pin(1,1,0),
		.rts_pin = def_nrf_gpio_null_pin(),
		.cts_pin = def_nrf_gpio_null_pin(),

	};
	
	return (io_socket_t*) &uart1;
}

/*
static nrf52_spi_t spi0 = {
	.implementation = &nrf52_spi_implementation,
	.encoding = NULL,
	.mosi_pin = def_nrf_io_alternate_pin(1,13,0),
	.miso_pin = def_nrf_io_alternate_pin(1,14,0),
	.sclk_pin = def_nrf_io_alternate_pin(1,15,0),
	.cs_pin = def_nrf_io_output_pin(0,31,NRF_GPIO_ACTIVE_LEVEL_LOW,GPIO_PIN_INACTIVE),
};
*/

/*
static io_socket_t*
qspi_socket (io_t *io) {
	static nrf52_qspi_t qspi = {
		.implementation = &nrf52_qspi_implementation,
		.address = io_invalid_address (),
		.qspi_registers = NRF_QSPI,
		.interrupt_number = QSPI_IRQn,
		.expected_chip = {
			.device = {
				.manufacturer = SPI_FLASH_MACRONIX,
				.type = 0x20,
				.capacity = 0x16,
			}
		},
		.number_of_physical_sectors = 16,// shorten for testing 1024,
		.sector_size = 4096,
		.page_size = 256,
		.cs_pin = def_nrf_io_alternate_pin (0,17,3),
		.sck_pin = def_nrf_io_alternate_pin (0,19,3),
		.io0_pin = def_nrf_io_alternate_pin (0,20,3),
		.io1_pin = def_nrf_io_alternate_pin (0,21,3),
		.io2_pin = def_nrf_io_alternate_pin (0,22,3),
		.io3_pin = def_nrf_io_alternate_pin (0,23,3),
	};
	return (io_socket_t*) &qspi;
}

*/
nrf52_qspi_t qspi = {
	.implementation = &nrf52_qspi_implementation,
	.address = io_invalid_address (),
	.qspi_registers = NRF_QSPI,
	.interrupt_number = QSPI_IRQn,
	.expected_chip = {
		.device = {
			.manufacturer = SPI_FLASH_MACRONIX,
			.type = 0x20,
			.capacity = 0x16,
		}
	},
	.number_of_physical_sectors = 16,// shorten for testing 1024,
	.sector_size = 4096,
	.page_size = 256,
	.cs_pin = def_nrf_io_alternate_pin (0,17,3),
	.sck_pin = def_nrf_io_alternate_pin (0,19,3),
	.io0_pin = def_nrf_io_alternate_pin (0,20,3),
	.io1_pin = def_nrf_io_alternate_pin (0,21,3),
	.io2_pin = def_nrf_io_alternate_pin (0,22,3),
	.io3_pin = def_nrf_io_alternate_pin (0,23,3),
};

static EVENT_DATA io_settings_t default_twi_settings = {
	.encoding = &io_twi_encoding_implementation,
	.transmit_pipe_length = 5,
	.receive_pipe_length = 5,
};

io_socket_t*
oled_display_slot (io_t *io) {
	io_socket_t *socket = io_byte_memory_allocate (
		io_get_byte_memory (io),sizeof(io_adapter_socket_t)
	);
	socket->implementation = &nrf52_twi_slave_implementation;
	socket->address = def_io_u8_address (OLED_FEATHER_I2C_ADDRESS);
	return socket;
}

io_socket_t*
twi0_socket (io_t *io) {
	static nrf52_twi_master_t twim0 = {
		.implementation = &nrf52_twi_master_implementation,
		.address = io_invalid_address (),
		.maximum_speed = 400000,
		.registers = NRF_TWI0,
		.interrupt_number = SPIM0_SPIS0_TWIM0_TWIS0_SPI0_TWI0_IRQn,
		.sda_pin = def_nrf_io_alternate_pin (0,26,GPIO_PIN_CNF_DRIVE_S0D1),
		.scl_pin = def_nrf_io_alternate_pin (0,27,GPIO_PIN_CNF_DRIVE_S0D1),
	};
	return (io_socket_t*) &twim0;
}

static EVENT_DATA io_settings_t radio_constructor = {
	.encoding = &nrf52_radio_encoding_implementation,
	.transmit_pipe_length = 5,
	.receive_pipe_length = 5,
};

static io_socket_t*
radio_socket (io_t *io,io_address_t address) {
	static nrf52_radio_t radio_socket = {
		.implementation = &nrf52_radio_socket_implementation,
		.address = io_invalid_address (),
		.registers = NRF_RADIO,
		.interrupt_number = RADIO_IRQn,
	};
	return (io_socket_t*) &radio_socket;
}

static bool
lookup_shared_key (io_t *io,io_uid_t const *uid,io_authentication_key_t *key) {
	return false;
}

io_implementation_t plite_io_implementation = {
	SPECIALISE_IO_DEVICE_IMPLEMENTATION(NULL)
};

static io_implementation_t io_i = {
	SPECIALISE_IO_DEVICE_IMPLEMENTATION(NULL)
};

static device_io_t dev_io = {
	.tc = {
		.high_timer = NRF_TIMER4,
		.low_timer = NRF_TIMER3,
		.interrupt_number = TIMER3_IRQn,
		.ppi_channel = TIME_CLOCK_PPI_CHANNEL,
	},
	.prbs_state = { 0x8764000b, 0xf542d2d3, 0x6fa035c3, 0x77f2db5b },
};

const io_settings_t bus = {
	.receive_pipe_length = 3,
	.make = NULL,
	.notify = NULL,
};

const socket_builder_t my_sockets[] = {
	{USART0,						uart0_socket,io_invalid_address(),&console_uart_settings,true,NULL},
	{USART1,						uart1_socket,io_invalid_address(),&default_uart_settings,false,NULL},
	{RADIO_BEACON_SOCKET,	allocate_io_beacon_socket,IO_BEACON_LAYER_ID,&bus,false,BINDINGS({RADIO_BEACON_SOCKET,RADIO_SOCKET},END_OF_BINDINGS)},
	{RADIO_SOCKET,				radio_socket,io_invalid_address(),&radio_constructor,false,NULL},
};

io_t*
initialise_device_io (void) {
	io_t *io = (io_t*) &dev_io;
	
	initialise_io (io,&io_i);

	dev_io.bm = initialise_io_byte_memory (io,&heap_byte_memory,UMM_BLOCK_SIZE_8);
	dev_io.vm = mk_umm_io_value_memory (io,UMM_VALUE_MEMORY_HEAP_SIZE,STVM);
	register_io_value_memory (dev_io.vm);

	initialise_io_board (io);

	io_cpu_clock_start (io,io_get_core_clock(io));

	io_set_pin_to_output(io,LED1);

	io_i.value_implementation_map = mk_string_hash_table (dev_io.bm,21);
	add_core_value_implementations_to_hash (io_i.value_implementation_map);
	
	dev_io.tasks = mk_io_value_pipe (dev_io.bm,3);

	dev_io.prbs_state[0] = io_get_random_u32(io);

	memset (dev_io.sockets,0,sizeof(io_socket_t*) * NUMBER_OF_IO_SOCKETS);
//	add_io_sockets_to_device (io,dev_io.sockets,my_sockets,SIZEOF(my_sockets));
	build_io_sockets ((io_t*) io,dev_io.sockets,my_sockets,SIZEOF(my_sockets));
	
	return io;
}

bool
start_io_device (io_t *io) {
	
	
	return true;
}

#endif /* IMPLEMENT_IO_DEVICE */
#include <io_device_verify.h>
#endif
/*
------------------------------------------------------------------------------
This software is available under 2 licenses -- choose whichever you prefer.
------------------------------------------------------------------------------
ALTERNATIVE A - MIT License
Copyright (c) 2020 Gregor Bruce
Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:
The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
------------------------------------------------------------------------------
ALTERNATIVE B - Public Domain (www.unlicense.org)
This is free and unencumbered software released into the public domain.
Anyone is free to copy, modify, publish, use, compile, sell, or distribute this
software, either in source code form or as a compiled binary, for any purpose,
commercial or non-commercial, and by any means.
In jurisdictions that recognize copyright laws, the author or authors of this
software dedicate any and all copyright interest in the software to the public
domain. We make this dedication for the benefit of the public at large and to
the detriment of our heirs and successors. We intend this dedication to be an
overt act of relinquishment in perpetuity of all present and future rights to
this software under copyright law.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
------------------------------------------------------------------------------
*/
