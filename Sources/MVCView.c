/*
 * Simple DirectMedia Layer - MVC
 * Copyright (C) 2014 Jay Dolan <jay@jaydolan.com>
 *
 * @author jdolan
 */

#include "MVC_view.h"

#define __class __MVC_View

#pragma mark - ObjectInterface

/**
 * @see ObjectInterface::dealloc(Object *)
 */
static void dealloc(Object *self) {

	MVC_View *this = (MVC_View *) self;

	$(this, removeFromSuperview);

	$(this->subviews, removeAllObjects);

	release(this->subviews);

	super(Object, self, dealloc);
}

#pragma mark - View initializers

/**
 * @see ViewInterface::init(View *, SDL_Rect *)
 */
static MVC_View *initWithFrame(MVC_View *self, SDL_Rect *frame) {

	self = (MVC_View *) super(Object, self, init);
	if (self) {
		self->subviews = $(alloc(Array), init);

		self->window = SDL_GL_GetCurrentWindow();
		self->renderer = SDL_GetRenderer(self->window);
		if (self->renderer == NULL) {
			self->renderer = SDL_CreateRenderer(self->window, -1, SDL_RENDERER_ACCELERATED);
			if (self->renderer == NULL) {
				SDL_SetError("%s: Failed to create renderer", __func__);
			}
		}

		if (frame) {
			self->frame = *frame;
		} else {
			SDL_LogDebug(0, "%s: NULL frame", __func__);
			SDL_GetWindowSize(self->window, &self->frame.w, &self->frame.h);
		}

		self->backgroundColor = MVC_Colors.Clear;
	}

	return self;
}

#pragma mark - ViewInterface

/**
 * @see ViewInterface::addSubview(View *, View *)
 */
static void addSubview(MVC_View *self, MVC_View *subview) {

	if (subview) {
		if ($(self->subviews, indexOfObject) == -1) {
			$(self->subviews, addObject, subview);
			subview->superview = self;
		}
	}
}

/**
 * @see ViewInterface::removeSubview(View *, View *)
 */
static void removeSubview(MVC_View *self, MVC_View *subview) {

	if (subview) {
		if ($(self->subviews, indexOfObject, subview) != -1) {
			$(self->subviews, removeObject, subview);
			subview->superview = NULL;
		}
	}
}

/**
 * @see ViewInterface::removeFromSuperview(View *)
 */
static void removeFromSuperview(MVC_View *self) {

	if (self->superview) {
		$(self->superview, removeSubview, self);
	}
}

/**
 * @see ViewInterface::draw(View *)
 */
static void draw(MVC_View *self) {

	if (self->backgroundColor.a) {

		SDL_Color c = self->backgroundColor;
		SDL_SetRenderDrawColor(self->renderer, c.r, c.g, c.b, c.a);

		SDL_RenderFillRect(self->renderer, &self->frame);

		SDL_Color w = MVC_Colors.White;
		SDL_SetRenderDrawColor(self->renderer, w.r, w.g, w.b, w.a);
	}
}

/**
 * @see ViewInterface::activate(View *)
 */
static void activate(MVC_View *self) {
	// TODO
}

/**
 * @see ViewInterface::deactivate(View *)
 */
static void deactivate(MVC_View *self) {
	// TODO
}

/**
 * @see ViewInterface::respondToEvent(View *, SDL_Event *, SDL_bool *)
 */
static void respondToEvent(MVC_View *self, SDL_Event *event, SDL_bool *cancel) {
	// TODO
}

#pragma mark - View class methods

/**
 * @see Class::initialize(Class *)
 */
static void initialize(Class *clazz) {

	ObjectInterface *object = (ObjectInterface *) clazz->interface;

	object->dealloc = dealloc;

	MVC_ViewInterface *view = (MVC_ViewInterface *) clazz->interface;

	view->initWithFrame = initWithFrame;

	view->addSubview = addSubview;
	view->removeSubview = removeSubview;
	view->removeFromSuperview = removeFromSuperview;

	view->draw = draw;

	view->activate = activate;
	view->deactivate = deactivate;
	view->respondToEvent = respondToEvent;
}

Class __MVC_View = {
	.name = "View",
	.superclass = &__Object,
	.instanceSize = sizeof(MVC_View),
	.interfaceOffset = offsetof(MVC_View, interface),
	.interfaceSize = sizeof(MVC_ViewInterface),
	.initialize = initialize,
};

#undef __class
