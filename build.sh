#!/bin/bash

gcc main.c \
    ll_buffer.c \
    renderer.c \
    input_handler.c \
    -o editor
