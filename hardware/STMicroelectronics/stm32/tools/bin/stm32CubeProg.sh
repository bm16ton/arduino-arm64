#!/bin/bash
dfu-util -a 0 --dfuse-address 0x08000000 -R -D $1
