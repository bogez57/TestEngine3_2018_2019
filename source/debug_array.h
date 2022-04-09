#ifndef DEBUG_ARRAY_H
#define DEBUG_ARRAY_H

#include <stdint.h>
#include <string.h>
#include <assert.h>
#include <boagz/error_handling.h>

namespace bgz
{
    template <typename Type, int64_t capacity = 1>
        struct DbgArray
    {
        public:
        Type& operator[](int64_t index)
        {
            BGZ_ASSERT(index < capacity);//Attempting to access an index which is outside of current arround bounds!
            BGZ_ASSERT(index < length);//Attempting to access an index which hasn't been initialized yet!
            
            return this->elements[index];
        };
        
        inline Type& Push(Type element = Type())
        {
            BGZ_ASSERT(length < capacity);//Attempting to push too many elements onto array!
            return (Type&)this->elements[length++] = element;
        };
        
        inline Type* Push(Type* element)
        {
            BGZ_ASSERT(length < capacity);//Attempting to push too many elements onto array!
            return (Type&)this->elements[length++] = element;
        };
        
        inline Type& LastElem()
        {
            BGZ_ASSERT(length != 0);
            return this->elements[this->length - 1];
        };
        
        inline int64_t Capacity()
        {
            return capacity;
        };
        
        int64_t length {};
        Type elements[capacity ? capacity : 1];
    };
    
    template <typename Type, int64_t capacity>
        Type* GetLastElem(bgz::DbgArray<Type, capacity>* array)
    {
        return &array->elements[array->length - 1];
    };
    
    template <typename Type, int64_t capacity>
        void ClearArray(bgz::DbgArray<Type, capacity>* array)
    {
        memset(array->elements, 0, array->length * sizeof(Type));
        array->length = 0;
    };
    
    template <typename Type, int64_t capacity>
        void ClearArrayButDontWipeData(bgz::DbgArray<Type, capacity>* array)
    {
        array->length = 0;
    };
}
#endif //DEBUG_ARRAY_H
