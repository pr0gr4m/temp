# Makefile - General Makefile for build
#

# execute file list
DIRS = ipc msg select sock

BUILD_DIRS = ${DIRS}

all :
	@ for dir in ${BUILD_DIRS}; do (cd $${dir}; ${MAKE}); done

clean : 
	@ for dir in ${BUILD_DIRS}; do (cd $${dir}; ${MAKE} clean); done

