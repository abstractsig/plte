/*
 *
 * verify plte device functionality
 *
 */
#define IMPLEMENT_IO_VERIFY
#define IMPLEMENT_VERIFY_IO_CORE_CONTAINERS
#define IMPLEMENT_VERIFY_IO_CORE_VALUES
#define IMPLEMENT_VERIFY_IO_CORE_SOCKETS
#define IMPLEMENT_VERIFY_IO_CORE_TWI_LAYER
#define IMPLEMENT_VERIFY_IO_DLC_SOCKET
#define IMPLEMENT_VERIFY_IO_BEACON_SOCKET
#define IMPLEMENT_VERIFY_IO_CPU
#define IMPLEMENT_VERIFY_IO_DEVICE
#include <sockets/io_beacon_socket.h>
#include <io_verify_core_math.h>
#include <io_verify_core_containers.h>
#include <io_verify_core_security.h>
#include <io_verify_core_sockets.h>
#include <io_verify_core_values.h>
#include <io_verify_core_graphics.h>
#include <io_device_verify.h>
#include <io_cpu_verify.h>

/*
 *-----------------------------------------------------------------------------
 *
 * test_device --
 *
 *-----------------------------------------------------------------------------
 */
bool
test_device (io_t *io,vref_t r_led) {
	V_runner_t runner = {
		.user_value = r_led,
		.io = io,
	};

	V_start_tests(&runner);
	io_log (
		io,IO_INFO_LOG_LEVEL,
		"%-*s%-*sstart\n",
		DBP_FIELD1,DEVICE_NAME,
		DBP_FIELD2,"self test"
	);
	flush_io_log (io);

	run_ut_io_device (&runner);
	run_ut_io_core_containers (&runner);
	run_ut_io_core_values (&runner);
	run_ut_io_core_sockets (&runner);
	run_ut_io_dlc_socket (&runner);
	run_ut_io_beacon_socket (&runner);

	print_unit_test_report (&runner);
	return runner.total_failed == 0;
}
