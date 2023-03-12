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


#include "container/stack.h"


/*===========================================================================*
 *                        The container private data                         *
 *===========================================================================*/
struct _stackData {
    unsigned size_;
    unsigned capacity_;
    void** elements_;
    stackClean func_clean_;
};

static const unsigned DEFAULT_CAPACITY = 32;


/*===========================================================================*
 *                  Definition for internal operations                       *
 *===========================================================================*/
#define likely(x)       __builtin_expect(!!(x), 1)
#define unlikely(x)     __builtin_expect(!!(x), 0)


/*===========================================================================*
 *               Implementation for the exported operations                  *
 *===========================================================================*/
stack* stackInit() {

    stack* obj = (stack*)malloc(sizeof(stack));
    if (unlikely(!obj))
        return NULL;

    stackData* data = (stackData*)malloc(sizeof(stackData));
    if (unlikely(!data)) {
        free(obj);
        return NULL;
    }

    void** elements = (void**)malloc(sizeof(void*) * DEFAULT_CAPACITY);
    if (unlikely(!elements)) {
        free(data);
        free(obj);
        return NULL;
    }

    data->size_ = 0;
    data->capacity_ = DEFAULT_CAPACITY;
    data->elements_ = elements;
    data->func_clean_ = NULL;

    obj->data = data;
    obj->push = stackPush;
    obj->top = stackTop;
    obj->pop = stackPop;
    obj->size = stackSize;
    obj->set_clean = stackSetClean;

    return obj;
}

void stackDeinit(stack* obj) {

    if (unlikely(!obj))
        return;

    stackData* data = obj->data;
    stackClean func_clean = data->func_clean_;
    void** elements = data->elements_;
    unsigned size = data->size_;

    unsigned i;
    if (func_clean) {
        for (i = 0 ; i < size ; ++i)
            func_clean(elements[i]);
    }

    free(elements);
    free(data);
    free(obj);
    return;
}

bool stackPush(stack *self, void* element) {

    stackData* data = self->data;
    void** elements = data->elements_;
    unsigned size = data->size_;
    unsigned capacity = data->capacity_;

    /* If the internal array is full, extend it to double capacity. */
    if (unlikely(size == capacity)) {
        unsigned new_capacity = capacity << 1;
        void** new_elements = (void**)realloc(elements, new_capacity * sizeof(void*));
        if (unlikely(!new_elements))
            return false;
        elements = data->elements_ = new_elements;
        data->capacity_ = new_capacity;
    }

    /* Insert the element to the tail of the array. */
    elements[size] = element;
    data->size_ = size + 1;

    return true;
}

bool stackPop(stack *self) {

    stackData* data = self->data;
    void** elements = data->elements_;
    unsigned size = data->size_;
    if (unlikely(size == 0))
        return false;

    /* Remove the element from the tail of the array. */
    --size;
    stackClean func_clean = data->func_clean_;
    if (func_clean)
        func_clean(elements[size]);
    data->size_ = size;

    return true;
}

bool stackTop(stack *self, void** p_elements) {

    stackData* data = self->data;
    void** elements = data->elements_;
    unsigned size = data->size_;
    if (unlikely(size == 0))
        return false;

    *p_elements = elements[size - 1];
    return true;
}

unsigned stackSize(stack *self) {
    return self->data->size_;
}

void stackSetClean(stack* self, stackClean func) {
    self->data->func_clean_ = func;
}
