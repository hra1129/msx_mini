#!/usr/bin/bash

while :; do
	./bluemsx_launcher
	if [ $? -ne 0 ]; then
		break
	fi
done
