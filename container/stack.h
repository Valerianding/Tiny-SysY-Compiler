/**
 *   The MIT License (MIT)
 *   Copyright (C) 2016 ZongXian Shen <andy.zsshen@gmail.com>
 *
 *   Permission is hereby granted, free of charge, to any person obtaining a
 *   copy of this software and associated documentation files (the "Software"),
 *   to deal in the Software without restriction, including without limitation
 *   the rights to use, copy, modify, merge, publish, distribute, sublicense,
 *   and/or sell copies of the Software, and to permit persons to whom the
 *   Software is furnished to do so, subject to the following conditions:
 *
 *   The above copyright notice and this permission notice shall be included in
 *   all copies or substantial portions of the Software.
 *
 *   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 *   THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 *   OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 *   IN THE SOFTWARE.
 */


/**
 * @file stack.h The LIFO stack.
 */

#ifndef _stack_H_
#define _stack_H_

#include "../util.h"

#ifdef __cplusplus
extern "C" {
#endif

/** stackData is the data type for the container private information. */
typedef struct _stackData stackData;

/** Element clean function called when an element is removed. */
typedef void (*stackClean) (void*);


/** The implementation for stack. */
typedef struct _stack {
    /** The container private information */
    stackData *data;

    /** Push an element to the top of the stack.
        @see stackPush */
    bool (*push) (struct _stack*, void*);

    /** Retrieve an element from the top of the stack.
        @see stackTop */
    bool (*top) (struct _stack*, void**);

    /** Remove an element from the top of the stack.
        @see stackPop */
    bool (*pop) (struct _stack*);

    /** Return the number of stored elements.
        @see stackSize */
    unsigned (*size) (struct _stack*);

    /** Set the custom element cleanup function.
        @see SetClean */
    void (*set_clean) (struct _stack*, stackClean func);
} stack;


/*===========================================================================*
 *             Definition for the exported member operations                 *
 *===========================================================================*/
/**
 * @brief The constructor for stack.
 *
 * @retval obj          The successfully constructed stack
 * @retval NULL         Insufficient memory for stack construction
 */
stack* stackInit();

/**
 * @brief The destructor for stack.
 *
 * @param obj           The pointer to the to be destructed stack
 */
void stackDeinit(stack* obj);

/**
 * @brief Push an element to the top of the stack.
 *
 * @param self          The pointer to stack structure
 * @param element       The specified element
 *
 * @retval true         The element is successfully pushed
 * @retval false        The element cannot be pushed due to insufficient memory
 */
bool stackPush(stack* self, void* element);

/**
 * @brief Retrieve an element from the top of the stack.
 *
 * @param self          The pointer to stack structure
 * @param p_element     The pointer to the returned element
 *
 * @retval true         The element is successfully retrieved
 * @retval false        The stack is empty
 */
bool stackTop(stack* self, void** p_element);

/**
 * @brief Remove an element from the top of the stack.
 *
 * This function removes an element from the top of the stack. Also, the cleanup
 * function is invoked for the removed element.
 *
 * @param self          The pointer to stack structure
 *
 * @retval true         The top element is successfully removed
 * @retval false        The stack is empty
 */
bool stackPop(stack* self);

/**
 * @brief Return the number of stored elements.
 *
 * @param self          The pointer to stack structure
 *
 * @retval size         The number of stored elements
 */
unsigned stackSize(stack* self);

/**
 * @brief Set the custom element cleanup function.
 *
 * @param self          The pointer to stack structure
 * @param func          The custom function
 */
void stackSetClean(stack* self, stackClean func);

#ifdef __cplusplus
}
#endif

#endif