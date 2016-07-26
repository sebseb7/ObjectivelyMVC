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
 * 3. This notice may not be removed or altered from any source distribution.
 */

#include <assert.h>

#include <ObjectivelyMVC/Text.h>

#define _Class _Text

#pragma mark - ObjectInterface

/**
 * @see ObjectInterface::dealloc(Object *)
 */
static void dealloc(Object *self) {

	Text *this = (Text *) self;
	
	release(this->font);
	
	free(this->text);

	SDL_DestroyTexture(this->texture);

	super(Object, self, dealloc);
}

#pragma mark - View

/**
 * @see View::render(View *, SDL_Renderer *)
 */
static void render(View *self, SDL_Renderer *renderer) {

	super(View, self, render, renderer);

	Text *this = (Text *) self;
	
	if (this->text && strlen(this->text)) {
		
		if (this->texture == NULL) {
			SDL_Surface *surface = $(this->font, renderCharacters, this->text, this->color);

			this->texture = SDL_CreateTextureFromSurface(renderer, surface);

			SDL_FreeSurface(surface);
		}
		
		assert(this->texture);
		
		const SDL_Rect frame = $(self, renderFrame);
		SDL_RenderCopy(renderer, this->texture, NULL, &frame);
	}
}

/**
 * @see View::renderDeviceDidReset(View *)
 */
static void renderDeviceDidReset(View *self) {

	super(View, self, renderDeviceDidReset);

	Text *this = (Text *) self;

	if (this->texture) {
		SDL_DestroyTexture(this->texture);
		this->texture = NULL;
	}
}

/**
 * @see View::sizeThatFits(View *)
 */
static SDL_Size sizeThatFits(const View *self) {

	return $((Text *) self, naturalSize);
}

#pragma mark - Text

/**
 * @fn Text *Text::initWithText(Text *self, const char *text, Font *font)
 *
 * @memberof Text
 */
static Text *initWithText(Text *self, const char *text, Font *font) {

	self = (Text *) super(View, self, initWithFrame, NULL);
	if (self) {

		self->color = Colors.White;

		$(self, setFont, font);
		$(self, setText, text);
	}

	return self;
}

/**
 * @fn SDL_Size Text::naturalSize(const Text *self)
 *
 * @memberof Text
 */
static SDL_Size naturalSize(const Text *self) {

	SDL_Size size = MakeSize(0, 0);

	if (self->font && self->text) {
		$(self->font, sizeCharacters, self->text, &size.w, &size.h);
	}

	return size;
}

/**
 * @fn void Text::setFont(Text *self, Font *font)
 *
 * @memberof Text
 */
static void setFont(Text *self, Font *font) {
	
	font = font ?: $$(Font, defaultFont, FontCategoryDefault);
	
	if (font != self->font) {
		
		release(self->font);
		self->font = retain(font);
		
		if (self->texture) {
			SDL_DestroyTexture(self->texture);
			self->texture = NULL;
		}

		$((View *) self, sizeToFit);
	}
}

/**
 * @fn void Text::setText(Text *self, const char *text)
 *
 * @memberof Text
 */
static void setText(Text *self, const char *text) {

	free(self->text);

	if (text) {
		self->text = strdup(text);
	} else {
		self->text = NULL;
	}
	
	if (self->texture) {
		SDL_DestroyTexture(self->texture);
		self->texture = NULL;
	}
	
	$((View *) self, sizeToFit);
}

#pragma mark - Class lifecycle

/**
 * @see Class::initialize(Class *)
 */
static void initialize(Class *self) {

	((ObjectInterface *) self->interface)->dealloc = dealloc;

	((ViewInterface *) self->interface)->render = render;
	((ViewInterface *) self->interface)->renderDeviceDidReset = renderDeviceDidReset;
	((ViewInterface *) self->interface)->sizeThatFits = sizeThatFits;

	((TextInterface *) self->interface)->initWithText = initWithText;
	((TextInterface *) self->interface)->naturalSize = naturalSize;
	((TextInterface *) self->interface)->setFont = setFont;
	((TextInterface *) self->interface)->setText = setText;
}

Class _Text = {
	.name = "Text",
	.superclass = &_View,
	.instanceSize = sizeof(Text),
	.interfaceOffset = offsetof(Text, interface),
	.interfaceSize = sizeof(TextInterface),
	.initialize = initialize,
};

#undef _Class
