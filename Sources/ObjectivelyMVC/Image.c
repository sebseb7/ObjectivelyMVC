/*
 * ObjectivelyMVC: MVC framework for OpenGL and SDL2 in c.
 * Copyright (C) 2014 Jay Dolan <jay@jaydolan.com>
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 * claim that you wrote the original software. If you use this software
 * in a product, an acknowledgment in the product documentation would be
 * appreciated but is not required.
 *
 * 2. Altered source versions must be plainly marked as such, and must not be
 * misrepresented as being the original software.
 *
 */

#include <assert.h>
#include <unistd.h>

#include <SDL2/SDL_image.h>

#include <ObjectivelyMVC/Image.h>
#include <ObjectivelyMVC/Log.h>

#define _Class _Image

#pragma mark - Object

/**
 * @see Object::dealloc(Object *)
 */
static void dealloc(Object *self) {

	Image *this = (Image *) self;

	SDL_FreeSurface(this->surface);

	super(Object, self, dealloc);
}

#pragma mark - Image

/**
 * @fn Image *Image::initWithName(Image *self, const char *name)
 *
 * @memberof Image
 */
static Image *initWithName(Image *self, const char *name) {
	
	assert(name);
	
#if defined(PACKAGE_DATA_DIR)
	char *path;
	asprintf(&path, "%s/%s", PACKAGE_DATA_DIR, name);
#else
	char *path = strdup(name);
#endif
	
	LogDebug("Resolved path %s for %s\n", path, name);

	self = $(self, initWithSurface, IMG_Load(path));
	
	free(path);
	
	return self;
}

/**
 * @fn Image *Image::initWithSurface(Image *self, SDL_Surface *surface)
 *
 * @memberof Image
 */
Image *initWithSurface(Image *self, SDL_Surface *surface) {

	self = (Image *) super(Object, self, init);
	if (self) {
		self->surface = surface;
		assert(self->surface);
	}

	return self;
}

#pragma mark - Class lifecycle

/**
 * @see Class::initialize(Class *)
 */
static void initialize(Class *clazz) {

	((ObjectInterface *) clazz->interface)->dealloc = dealloc;

	((ImageInterface *) clazz->interface)->initWithName = initWithName;
	((ImageInterface *) clazz->interface)->initWithSurface = initWithSurface;
}

Class _Image = {
	.name = "Image",
	.superclass = &_Object,
	.instanceSize = sizeof(Image),
	.interfaceOffset = offsetof(Image, interface),
	.interfaceSize = sizeof(ImageInterface),
	.initialize = initialize,
};

#undef _Class