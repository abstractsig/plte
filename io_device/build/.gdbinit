#
# black magic probe debug
#
#------------------------------------------------------------------------------
#
# halt cpu (assumes started with r& or c&)
#
#------------------------------------------------------------------------------
define h
	dont-repeat
	interrupt
end
#------------------------------------------------------------------------------
#
# black magic probe
#
#------------------------------------------------------------------------------
define bmp 
	monitor swdp_scan
	attach 1
end
#------------------------------------------------------------------------------
#
# jlink run
#
#------------------------------------------------------------------------------
define jr 
	monitor reset
	c&
end
#------------------------------------------------------------------------------
#
# test
#
#------------------------------------------------------------------------------
define test 
	break verify_io.h:94
end

set confirm off
set mem inaccessible-by-default off

#
# to debug with jlink:
#
# arm-none-eabi-gdb -ex "target remote localhost:2331" plte.elf
# or bmp
#
# arm-none-eabi-gdb -ex "target extended-remote \\\\.\\COM21" plte.elf
#

