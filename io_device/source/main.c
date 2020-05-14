/*
 *
 * pinky starts here
 *
 */
#define IMPLEMENT_IO_CORE
#define IMPLEMENT_IO_BEACON_SOCKET
#define IMPLEMENT_IO_CPU
#define IMPLEMENT_IO_TLS
#define IMPLEMENT_IO_DEVICE
#define IMPLEMENT_NRF52_RADIO
#define IMPLEMENT_IO_BOARD
#include <io_device.h>


int
main (void) {
	io_t *io = initialise_device_io ();
	bool first_run = io_is_first_run (io);
	
	io->log_level = IO_INFO_LOG_LEVEL;
	
	if (first_run) {
		if (!test_device (io,cr_NIL)) {
			io_printf(io,"\n"DEVICE_NAME" device test failed\n");
			return 1;
		}
	}

	if (start_io_device (io)) {
		io_socket_t *radio = io_get_socket (io,RADIO_BEACON_SOCKET);
		io_beacon_socket_set_interval (radio,millisecond_time(1000));
		
		io_printf (
			io,"%-*s%-*scomplete\n",
			DBP_FIELD1,DEVICE_NAME,DBP_FIELD2,"startup"
		);
		
		//io->log_level = IO_WARNING_LOG_LEVEL;
		io_socket_open (radio,IO_SOCKET_OPEN_CONNECT);

		while (1) {
			io_wait_for_event (io);
			io_do_gc (io,-1);
		}
		return 0;
	} else {
		io_printf(io,"%s"DEVICE_NAME" device failed to start\n",(first_run)? "":"\n");
		return 1;
	}
}
