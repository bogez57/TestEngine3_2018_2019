#include <boagz/error_handling.h>

template <typename Type, i64 size = 1>
class Array
{
    public:
    Type& operator[](i64 index)
    {
        BGZ_ASSERT(index < size);
        return this->elements[index];
    };
    
    inline i64 Size() const
    {
        return size;
    };
    
    Type elements[size ? size : 1];
};

template <typename Type, i64 size>
void CopyArray(Array<Type, size> sourceArray, Array<Type, size>&& destinationArray)
{
    BGZ_ASSERT(destinationArray.Size() >= sourceArray.Size());// "Dest array needs to be at least as large as source array!");
    
    memcpy(destinationArray.elements, sourceArray.elements, sizeof(Type) * sourceArray.Size());
};