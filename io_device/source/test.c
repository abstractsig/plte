/*
 *
 * verify device functionality
 *
 */
#define IMPLEMENT_VERIFY_IO_CORE
#define  IMPLEMENT_VERIFY_IO_CORE_VALUES
#define  IMPLEMENT_VERIFY_IO_CORE_SOCKETS
#define  IMPLEMENT_VERIFY_IO_DLC_SOCKET
#define IMPLEMENT_VERIFY_IO_MATH
#define IMPLEMENT_VERIFY_IO_TLS
#define IMPLEMENT_VERIFY_IO_CPU
#define IMPLEMENT_VERIFY_IO_DEVICE
#include <io_core.h>

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
	verify_io (&runner);
	run_ut_io_dlc_socket (&runner);
	print_unit_test_report (&runner);

	return runner.total_failed == 0;
}
