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

#include <Objectively/String.h>

#include <ObjectivelyMVC/TextView.h>

#define _Class _TextView

#pragma mark - Object

/**
 * @see Object::dealloc(Object *)
 */
static void dealloc(Object *self) {
	
	TextView *this = (TextView *) self;
	
	release(this->text);
	
	release(this->attributedText);
	
	super(Object, self, dealloc);
}

#pragma mark - View

/**
 * @see View::render(View *, SDL_Renderer *)
 */
static void render(View *self, SDL_Renderer *renderer) {
	
	super(View, self, render, renderer);
	
	TextView *this = (TextView *) self;
	
	char *text = this->attributedText->string.chars;
	
	if (text == NULL || strlen(text) == 0) {
		if ((this->control.state & ControlStateFocused) == 0) {
			text = this->defaultText;
		}
	}
	
	if (text == NULL) {
		$(this->text, setText, NULL);
	} else {
		if (this->text->text) {
			if (strcmp(text, this->text->text)) {
				$(this->text, setText, text);
			}
		} else {
			$(this->text, setText, text);
		}
	}
}

/**
 * @see View::respondToEvent(View *, const SDL_Event *)
 */
static _Bool respondToEvent(View *self, const SDL_Event *event) {
	
	TextView *this = (TextView *) self;
	
	_Bool didEdit = false, didHandleEvent = false;
	
	if (event->type == SDL_MOUSEBUTTONDOWN) {
		const SDL_Point point = { .x = event->button.x, .y = event->button.y };
		if ($(self, containsPoint, &point)) {
			if ((this->control.state & ControlStateFocused) == 0) {
				this->control.state |= ControlStateFocused;
				SDL_StartTextInput();
				if (this->delegate.didBeginEditing) {
					this->delegate.didBeginEditing(this);
				}
			}
			didHandleEvent = true;
		} else {
			if (this->control.state & ControlStateFocused) {
				this->control.state &= ~ControlStateFocused;
				SDL_StopTextInput();
				if (this->delegate.didEndEditing) {
					this->delegate.didEndEditing(this);
				}
			}
		}
	} else if (event->type == SDL_TEXTINPUT) {
		if (this->control.state & ControlStateFocused) {
			if (this->position == this->attributedText->string.length) {
				$(this->attributedText, appendCharacters, event->text.text);
			} else {
				$(this->attributedText, insertCharactersAtIndex, event->text.text, this->position);
			}
			this->position += strlen(event->text.text);
			didEdit = didHandleEvent = true;
		}
	} else if (event->type == SDL_KEYDOWN) {
		if (this->control.state & ControlStateFocused) {
			didHandleEvent = true;
			
			const char *chars = this->attributedText->string.chars;
			const size_t len = this->attributedText->string.length;
			
			switch (event->key.keysym.sym) {
					
				case SDLK_ESCAPE:
				case SDLK_KP_ENTER:
				case SDLK_KP_TAB:
				case SDLK_RETURN:
				case SDLK_TAB:
					this->control.state &= ~ControlStateFocused;
					SDL_StopTextInput();
					if (this->delegate.didEndEditing) {
						this->delegate.didEndEditing(this);
					}
					break;
					
				case SDLK_BACKSPACE:
				case SDLK_KP_BACKSPACE:
					if (this->position > 0) {
						const Range range = { .location = this->position - 1, .length = 1 };
						$(this->attributedText, deleteCharactersInRange, range);
						this->position--;
						didEdit = true;
					}
					break;
					
				case SDLK_DELETE:
					if (this->position < len) {
						const Range range = { .location = this->position, .length = 1 };
						$(this->attributedText, deleteCharactersInRange, range);
						didEdit = true;
					}
					break;
					
				case SDLK_LEFT:
					if (SDL_GetModState() & KMOD_CTRL) {
						while (this->position > 0 && chars[this->position] == ' ') {
							this->position--;
						}
						while (this->position > 0 && chars[this->position] != ' ') {
							this->position--;
						}
					} else if (this->position > 0) {
						this->position--;
					}
					break;
					
				case SDLK_RIGHT:
					if (SDL_GetModState() & KMOD_CTRL) {
						while (this->position < len && chars[this->position] == ' ') {
							this->position++;
						}
						while (this->position < len && chars[this->position] != ' ') {
							this->position++;
						}
						if (this->position < len) {
							this->position++;
						}
					} else if (this->position < len) {
						this->position++;
					}
					break;
					
				case SDLK_HOME:
					this->position = 0;
					break;
					
				case SDLK_END:
					this->position = len;
					break;
					
				case SDLK_a:
					if (SDL_GetModState() & KMOD_CTRL) {
						this->position = 0;
					}
					break;
				case SDLK_e:
					if (SDL_GetModState() & KMOD_CTRL) {
						this->position = len;
					}
					break;
				
				case SDLK_v:
					if ((SDL_GetModState() & (KMOD_CTRL | KMOD_GUI)) && SDL_HasClipboardText()) {
						const char *text = SDL_GetClipboardText();
						if (this->position == len) {
							$(this->attributedText, appendCharacters, text);
						} else {
							$(this->attributedText, insertCharactersAtIndex, text, this->position);
						}
						this->position += strlen(text);
						didEdit = true;
					}
					break;
				
				default:
					break;
			}
		}
	}
	
	if (didEdit) {
		if (this->delegate.didEdit) {
			this->delegate.didEdit(this);
		}
	}
	
	return didHandleEvent ?: super(View, self, respondToEvent, event);
}

#pragma mark - TextView

/**
 * @fn TextView *TextView::initWithFrame(TextView *self, const SDL_Rect *frame, ControlStyle style)
 *
 * @memberof TextView
 */
static TextView *initWithFrame(TextView *self, const SDL_Rect *frame, ControlStyle style) {

	self = (TextView *) super(Control, self, initWithFrame, frame, style);
	if (self) {
		self->attributedText = $$(MutableString, string);
		assert(self->attributedText);
		
		self->editable = true;
		
		self->text = $(alloc(Label), initWithText, NULL, NULL);
		assert(self->text);
		
		$((View *) self, addSubview, (View *) self->text);
		
		if (self->control.style == ControlStyleDefault) {
			self->control.bevel = BevelTypeInset;
			
			self->control.view.backgroundColor = Colors.DimGray;
			
			if (self->control.view.frame.w == 0) {
				self->control.view.frame.w = DEFAULT_TEXTVIEW_WIDTH;
			}
		}
	}

	return self;
}

#pragma mark - Class lifecycle

/**
 * @see Class::initialize(Class *)
 */
static void initialize(Class *clazz) {
	
	((ObjectInterface *) clazz->interface)->dealloc = dealloc;
	
	((ViewInterface *) clazz->interface)->render = render;
	((ViewInterface *) clazz->interface)->respondToEvent = respondToEvent;

	((TextViewInterface *) clazz->interface)->initWithFrame = initWithFrame;
}

Class _TextView = {
	.name = "TextView",
	.superclass = &_Control,
	.instanceSize = sizeof(TextView),
	.interfaceOffset = offsetof(TextView, interface),
	.interfaceSize = sizeof(TextViewInterface),
	.initialize = initialize,
};

#undef _Class
