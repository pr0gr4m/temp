# Makefile - General Makefile for build
#


# execute file list
DIRS = serv clnt

BUILD_DIRS = ${DIRS}

all :
	@ for dir in ${BUILD_DIRS}; do (cd $${dir}; ${MAKE}); done

clean : 
	@ for dir in ${BUILD_DIRS}; do (cd $${dir}; ${MAKE} clean); done

