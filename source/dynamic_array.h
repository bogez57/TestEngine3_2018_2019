/* date = June 8th 2021 6:45 pm */

#ifndef DYNAMIC_ARRAY_H
#define DYNAMIC_ARRAY_H

#include <stdint.h>
#include <assert.h>

///Klib Dynamic Array ///////////////////////////////////////////////

/* The MIT License

   Copyright (c) 2008, by Attractive Chaos <attractor@live.co.uk>

   Permission is hereby granted, free of charge, to any person obtaining
   a copy of this software and associated documentation files (the
   "Software"), to deal in the Software without restriction, including
   without limitation the rights to use, copy, modify, merge, publish,
   distribute, sublicense, and/or sell copies of the Software, and to
   permit persons to whom the Software is furnished to do so, subject to
   the following conditions:

   The above copyright notice and this permission notice shall be
   included in all copies or substantial portions of the Software.

   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
   EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
   MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
   NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
   BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
   ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
   CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
   SOFTWARE.
*/

/*
  An example:

#include "kvec.h"
int main() {
	kvec_t(int) array;
	kv_init(array);
	kv_push(int, array, 10); // append
	kv_At(int, array, 20) = 5; // dynamic
	kv_at(array, 20) = 4; // static
	kv_destroy(array);
	return 0;
}
*/

/*
  2008-09-22 (0.1.0):

	* The initial version.

*/

#ifndef AC_KVEC_H
#define AC_KVEC_H

#include <stdlib.h>

#define kv_roundup32(x) (--(x), (x)|=(x)>>1, (x)|=(x)>>2, (x)|=(x)>>4, (x)|=(x)>>8, (x)|=(x)>>16, ++(x))

#define kvec_t(type) struct { size_t size, cap; type *arr; }
#define kv_init(v) ((v).size = (v).cap = 0, (v).arr = 0)
#define kv_destroy(v) free((v).arr)
#define kv_at(v, i) ((v).arr[(i)])
#define kv_pop(v) ((v).arr[--(v).size])
#define kv_size(v) ((v).size)
#define kv_max(v) ((v).cap)

#define kv_resize(type, v, s)  ((v).cap = (s), (v).arr = (type*)realloc((v).arr, sizeof(type) * (v).cap))

#define kv_copy(type, v1, v0) do {							\
if ((v1).cap < (v0).size) kv_resize(type, v1, (v0).size);	\
(v1).size = (v0).size;									\
memcpy((v1).arr, (v0).arr, sizeof(type) * (v0).size);		\
} while (0)												\

#define kv_push(type, v, x) do {									\
if ((v).size == (v).cap) {										\
(v).cap = (v).cap? (v).cap<<1 : 2;							\
(v).arr = (type*)realloc((v).arr, sizeof(type) * (v).cap);	\
}															\
(v).arr[(v).size++] = (x);										\
} while (0)

#define kv_pushp(type, v) (((v).size == (v).cap)?							\
((v).cap = ((v).cap? (v).cap<<1 : 2),				\
(v).arr = (type*)realloc((v).arr, sizeof(type) * (v).cap), 0)	\
: 0), ((v).arr + ((v).size++))

#define kv_At(type, v, i) (((v).cap <= (size_t)(i)? \
((v).cap = (v).size = (i) + 1, kv_roundup32((v).cap), \
(v).arr = (type*)realloc((v).arr, sizeof(type) * (v).cap), 0) \
: (v).size <= (size_t)(i)? (v).size = (i) + 1 \
: 0), (v).arr[(i)])

#endif

///Klib Dynamic Array ///////////////////////////////////////////////

namespace bgz
{
    template <typename Type>
        class Dynam_Array
    {
        public:
        Dynam_Array() = default;
        
        Type& operator[](int64_t index)
        {
            assert(index < (int64_t)elems.size);//Trying to access out of bounds!
            assert(index >= 0);//Cannot access a negative index!"
            return kv_at(this->elems, index);
        };
        
        kvec_t(Type) elems{};
    };
    
    template <typename Type> void Init(Dynam_Array<Type>* arr, int numItems)
    {
        assert(numItems > 0);
        
        kv_init(arr->elems);
        kv_resize(Type, arr->elems, numItems);
    };
    
    template <typename Type>
        void PopBack(Dynam_Array<Type>& arr)
    {
        assert(arr.elems.size > 0);//Cannot pop off an empty dynamic array!"
    };
    
    //Resize array if appending over bounds
    template <typename Type>
        void Push(Dynam_Array<Type>& arr, Type e = Type())
    {
        kv_push(Type, arr.elems, e);
    };
    
    template <typename Type>
        void Insert(Dynam_Array<Type>&& arr, Type e, int32_t AtIndex)
    {
        assert(AtIndex >= 0);//Cannot access a negative index!"
    };
    
    template <typename Type>
        void Reserve(Dynam_Array<Type>& arr, int32_t numItems)
    {
        assert(numItems >= 0);//Cannot reserve a negative number of items!"
        kv_resize(Type, arr.elems, numItems);
    };
    
    template <typename Type>
        void CleanUp(Dynam_Array<Type>&& arr)
    {
    };
    
    template <typename Type>
        void Clear(Dynam_Array<Type>& arr)
    {
        size_t size = arr.elems.size;
        kv_destroy(arr.elems);
        kv_init(arr.elems);
        kv_resize(Type, arr.elems, size);
    };
    
    template <typename Type>
        bool Empty(Dynam_Array<Type>* arr)
    {
        return (arr->elems.size == 0) ? true : false;
    };
    
    template <typename Type>
        Type& LastElem(Dynam_Array<Type>* arr)
    {
        assert(arr->elems.size != 0);//Nothing has been pushed or insereted onto array"
        return arr->elems.arr[arr->elems.size - 1];
    };
    
    template <typename Type>
        void ResizeArray(Dynam_Array<Type>& arrayToResize, int64_t size)
    {
        kv_resize(Type, arrayToResize.elems, size);
        for(int i{}; i < size; ++i)
        {
            kv_at(arrayToResize.elems, i) = {};
        };
    };
    
    template <typename Type>
        void CopyArray(Dynam_Array<Type> sourceArray, Dynam_Array<Type>& destinationArray)
    {
        kv_copy(Type, destinationArray.elems, sourceArray.elems);
    };
    
    template <typename Type>
        void SwapArrays(Dynam_Array<Type>* array1, Dynam_Array<Type>* array2)
    {
    };
    
    template <typename Type>
        size_t Size(Dynam_Array<Type>* arr)
    {
        return arr->elems.size;
    };
}

#endif //DYNAMIC_ARRAY_H
