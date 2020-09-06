#
# MASTER makefile for OpenCRG, do not TOUCH!!!
#

MAKE_CMD = make

make_rule : default

default:
	@cd baselib; ${MAKE_CMD} ${MAKECMDGOALS}
	@cd demo; ${MAKE_CMD} ${MAKECMDGOALS}
	@cd test; ${MAKE_CMD} ${MAKECMDGOALS}

clean: default

