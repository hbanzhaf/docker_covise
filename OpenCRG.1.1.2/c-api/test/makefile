#
# MASTER makefile for OpenCRG Tests, do not TOUCH!!!
#

MAKE_CMD = make

make_rule : default

default:
	@cd PerfTest;  ${MAKE_CMD} ${MAKECMDGOALS}
	@cd MemTest;   ${MAKE_CMD} ${MAKECMDGOALS}
	@cd Dump;      ${MAKE_CMD} ${MAKECMDGOALS}
	@cd Verify;    ${MAKE_CMD} ${MAKECMDGOALS}
	@cd MultiRead; ${MAKE_CMD} ${MAKECMDGOALS}
	@cd MultiCp;   ${MAKE_CMD} ${MAKECMDGOALS}
	@cd Scan;      ${MAKE_CMD} ${MAKECMDGOALS}

debug: default

clean: default

distclean: default

