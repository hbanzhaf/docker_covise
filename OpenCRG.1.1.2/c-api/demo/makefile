#
# MASTER makefile for OpenCRG Demos, do not TOUCH!!!
#

MAKE_CMD = make

make_rule : default

default:
	@cd Reader; ${MAKE_CMD} ${MAKECMDGOALS}
	@cd EvalXYnUV; ${MAKE_CMD} ${MAKECMDGOALS}
	@cd EvalZ; ${MAKE_CMD} ${MAKECMDGOALS}
	@cd Simple; ${MAKE_CMD} ${MAKECMDGOALS}
	@cd EvalOptions; ${MAKE_CMD} ${MAKECMDGOALS}

debug: default

clean: default

distclean: default

