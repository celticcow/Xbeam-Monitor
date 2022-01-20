#!/bin/bash

echo "./create_ip380-1.sh $1_$2 1210869000"
echo "./create_mem.sh $1_$2 1210869000"
echo "./create_linuxcpu.sh $1_$2 1210869000 1"
echo "./create_linuxcpu.sh $1_$2 1210869000 2"
