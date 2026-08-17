#!/bin/bash

gcc -pthread src/*.c -o loader -s -Os -static
