/*
 * This file is part of libmonome.
 * libmonome is copyright 2007, 2008 will light <visinin@gmail.com>
 *
 * libmonome is free software. It comes without any warranty, to
 * the extent permitted by applicable law. You can redistribute it
 * and/or modify it under the terms of the Do What The Fuck You Want
 * To Public License, Version 2, as published by Sam Hocevar. See
 * http://sam.zoy.org/wtfpl/COPYING for more details.
 *
 */

#include <dlfcn.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "monome.h"
#include "monome_internal.h"
#include "monome_platform.h"

#ifndef PROTO
#define PROTO "40h"
#endif

#ifndef LIBSUFFIX
#define LIBSUFFIX ".so"
#endif

#ifndef LIBDIR
#define LIBDIR "/usr/lib"
#endif

/**
 * private
 */

static ssize_t monome_read(monome_t *monome, uint8_t *buf, ssize_t count) {
    return monome_platform_read(monome, buf, count);
}

/**
 * public
 */

monome_t *monome_init() {
	void *protocol_lib;
	monome_t *(*monome_protocol_new)();
	monome_t *monome;
	
	protocol_lib = dlopen(LIBDIR "/monome/protocol_" PROTO LIBSUFFIX, RTLD_NOW);
	
	if( !protocol_lib ) {
		fprintf(stderr, "couldn't load monome protocol module.  dlopen said:\n\t%s\n\n"
				"please make sure that libmonome is installed correctly!\n", dlerror());
		return NULL;
	}
	
	monome_protocol_new = dlsym(protocol_lib, "monome_protocol_new");
	
	if( !monome_protocol_new ) {
		fprintf(stderr, "couldn't initialize monome protocol module. dlopen said:\n\t%s\n\n"
				"please make sure you're using a valid protocol library!\n"
				"if this is a protocol library you wrote, make sure you're providing a \e[1mmonome_protocol_new\e[0m function.\n", dlerror());
		return NULL;
	}
	
	monome = (*monome_protocol_new)();
	
	if( !monome )
		return NULL;

	return monome;
}

monome_t *monome_open(const char *dev) {
	monome_t *monome = monome_init();
	
	if( !monome )
		return NULL;
	
	monome->dev = strdup(dev);
	
	if( monome_platform_open(monome) )
		return NULL;
	
	return monome;
}

void monome_close(monome_t *monome) {
	if( tcsetattr(monome->fd, TCSANOW, &monome->ot) < 0) {
		perror("libmonome: could not restore terminal attributes");
		return;
	}
	
	close(monome->fd);
	
	free(monome->dev);
	free(monome);
}

void monome_register_handler(monome_t *monome, unsigned int event_type, monome_callback_function_t cb, void *data) {
	monome_callback_t *handler;
	
	if( event_type > 2 || !cb )
		return;
	
	handler       = &monome->handlers[event_type];
	handler->cb   = cb;
	handler->data = data;
}

void monome_unregister_handler(monome_t *monome, unsigned int event_type) {
	monome_register_handler(monome, event_type, NULL, NULL);
}

void monome_main_loop(monome_t *monome) {
	monome_callback_t *handler;
	monome_event_t e;
	uint8_t buf[2] = {0, 0};
	
	e.monome = monome;
	
	while( monome_read(monome, buf, sizeof(buf)) > 0 ) {
		if( monome->populate_event(&e, buf, sizeof(buf)) )
			continue;
			
		handler = &monome->handlers[e.event_type];
		if( handler->cb )
			handler->cb(&e, handler->data);
	}
}

int monome_clear(monome_t *monome, monome_clear_status_t status) {
	return monome->clear(monome, status);
}

int monome_intensity(monome_t *monome, unsigned int brightness) {
	return monome->intensity(monome, brightness);
}

int monome_mode(monome_t *monome, monome_mode_t mode) {
	return monome->mode(monome, mode);
}

int monome_led_on(monome_t *monome, unsigned int x, unsigned int y) {
	return monome->led_on(monome, x, y);
}

int monome_led_off(monome_t *monome, unsigned int x, unsigned int y) {
	return monome->led_off(monome, x, y);
}

int monome_led_col_8(monome_t *monome, unsigned int col, unsigned int *col_data) {
	return monome->led_col_8(monome, col, col_data);
}

int monome_led_row_8(monome_t *monome, unsigned int row, unsigned int *row_data) {
	return monome->led_row_8(monome, row, row_data);
}

int monome_led_col_16(monome_t *monome, unsigned int col, unsigned int *col_data) {
	return monome->led_col_16(monome, col, col_data);
}

int monome_led_row_16(monome_t *monome, unsigned int row, unsigned int *row_data) {
	return monome->led_row_16(monome, row, row_data);
}

int monome_led_frame(monome_t *monome, unsigned int quadrant, unsigned int *frame_data) {
	return monome->led_frame(monome, quadrant, frame_data);
}