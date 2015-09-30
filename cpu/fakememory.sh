#!/bin/sh

PORT=$(cat config.cfg | grep PUERTO_MEMORIA | grep -oP '[0-9]+' | tr '\n' ' ')

while true; do nc -l $PORT -v; done
