#!/usr/bin/env python2
# -*- coding: utf-8 -*-
import socket
import sys
import string
import time
HOST = sys.argv[1]
PORT = int(sys.argv[2])
print "Destination Address :", HOST, PORT
s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
while 1:
	sbuf = 'Time:' + str(time.time())
	s.sendto(sbuf, 0, (HOST, PORT))
	print "[Send] (" + str(len(sbuf)) + ") ", sbuf
	rbuf = s.recvfrom(2048)
	print "[Recv] Addr" + str(rbuf[1]) + " Len(" + str(len(rbuf[0])) + ")"
	print "		Buff(" + rbuf[0] + ")" # rbuf[0] is data, [1] is address
	time.sleep(2)
s.close()
