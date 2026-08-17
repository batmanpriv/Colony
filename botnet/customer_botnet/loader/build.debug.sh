#!/bin/bash

gcc -D DEBUG src/*.c -pthread -o loader.debug -s -Os -static
