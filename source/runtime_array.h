#ifndef RUNTIME_ARRAY_H
#define RUNTIME_ARRAY_H

template <typename Type>
struct RunTimeArr
{
    public:
    RunTimeArr() = default;
    RunTimeArr(bgz::Memory_Partition* memPart, s64 size)
    {
        this->size = size;
        this->elements = PushType(memPart, Type, size);
    };
    
    Type& operator[](s64 index)
    {
        BGZ_ASSERT(index < size);//Attempting to access an index which is outside of current arround bounds!
        
        return this->elements[index];
    };
    
    inline s64 Size() const
    {
        return size;
    };
    
    s64 size {};
    Type* elements {};
};

template <typename Type>
void InitArr(RunTimeArr<Type>&& varArr, bgz::Memory_Partition* memPart, s64 size)
{
    BGZ_ASSERT(varArr.size == 0);//Trying to initialize array twice!
    
    varArr.size = size;
    varArr.elements = PushType(memPart, Type, size);
};

template <typename Type>
void CopyArray(RunTimeArr<Type> sourceArray, RunTimeArr<Type>&& destinationArray)
{
    BGZ_ASSERT(destinationArray.size == sourceArray.size);//Variable Array capacities do not match!
    BGZ_ASSERT(destinationArray.elements != sourceArray.elements);//Both varialbe arrays pointing to same memory address!
    
    memcpy(destinationArray.elements, sourceArray.elements, sizeof(Type) * sourceArray.Size());
};

template <typename Type>
RunTimeArr<Type> CopyArray(bgz::Memory_Partition* memPart, RunTimeArr<Type> sourceArray)
{
    RunTimeArr<Type> destinationArray{memPart, sourceArray.size};
    
    memcpy(destinationArray.elements, sourceArray.elements, sizeof(Type) * sourceArray.Size());
    
    return destinationArray;
};

#endif //VARIABLE_ARRAY_H