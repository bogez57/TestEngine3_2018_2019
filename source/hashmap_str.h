#include "dynamic_array.h"

/*
    Duplicates are overwritten
*/

#if 0
enum Error
{
    HASH_DOES_NOT_EXIST = -1
};

template <typename Type>
class KeyInfo
{
public:
    const char* originalString{nullptr};
    i32 uniqueID{};
    Type value;
    KeyInfo<Type>* nextInfo;
};

template <typename ValueType>
class HashMap_Str
{
public:
    HashMap_Str() = default;
    HashMap_Str(i32 memParitionID) :
        keyInfos{4096, memParitionID}
    {};

    Dynam_Array<KeyInfo<ValueType>> keyInfos;
    i32 numOfCollisions{};
};

template <typename ValueType>
HashMap_Str<ValueType> CopyHashMap(HashMap_Str<ValueType> src)
{
    HashMap_Str<ValueType> dest = src;

    CopyArray(src.keyInfos, $(dest.keyInfos));

    return dest;
};

i32 _ProduceUniqueIDForString(const char* key)
{
    i32 uniqueID{};
    for (i32 i{}; key[i] != 0; ++i)
    {
        uniqueID += key[i];
    };

    return uniqueID;
};

ui16 _HashFunction(i32 numberToCondense)
{
    BGZ_ASSERT(numberToCondense < 0xFFFF, "Key originalString was too big!");

    ui16 indexIntoHashArr{};
    ui16 mask_clearNibble{ 0x0FFF };

    indexIntoHashArr = (ui16)numberToCondense;
    indexIntoHashArr = indexIntoHashArr & mask_clearNibble;

    return indexIntoHashArr;
};

template <typename ValueType>
void CleanUpHashMap_Str(HashMap_Str<ValueType>&& map)
{
    map.numOfCollisions = 0;
    CleanUpDynamArr($(map.keyInfos));
};

template <typename ValueType>
ui16 Insert(HashMap_Str<ValueType>&& map, const char* key, ValueType value)
{
    KeyInfo<ValueType> info{};
    info.originalString = key;
    info.value = value;
    info.uniqueID = _ProduceUniqueIDForString(key);

    ui16 indexIntoHashArr = _HashFunction(info.uniqueID);

    if (NOT map.keyInfos.At(indexIntoHashArr).uniqueID || map.keyInfos.At(indexIntoHashArr).uniqueID == info.uniqueID)
    {
        Insert($(map.keyInfos), info, indexIntoHashArr);
    }
    else
    {
        KeyInfo<ValueType>** nextInfo = &map.keyInfos.At(indexIntoHashArr).nextInfo;

        while (*nextInfo)
        {
            if ((*nextInfo)->uniqueID == info.uniqueID)
               break;
            else
                nextInfo = &(*nextInfo)->nextInfo;
        };

        BGZ_ASSERT(map.numOfCollisions < (map.keyInfos.size - 2) / 2, "Hash Table contains too many collisions! Reached end of space allocated for collision entries");

        *nextInfo = &map.keyInfos.At((map.keyInfos.size - 1) - map.numOfCollisions);
        **nextInfo = info;
        ++map.numOfCollisions;
    };

    return indexIntoHashArr;
};

template <typename ValueType>
i32 GetHashIndex(HashMap_Str<ValueType> map, const char* key)
{
    ui16 uniqueKeyID = _ProduceUniqueIDForString(key);

    ui16 mask_clearNibble{ 0x0FFF };

    i32 indexIntoHashArr{ Error::HASH_DOES_NOT_EXIST };

    i32 tempIndex = _HashFunction(uniqueKeyID);
    if (map.keyInfos.At(tempIndex).uniqueID)
        indexIntoHashArr = tempIndex;

    return indexIntoHashArr;
};

template <typename ValueType>
ValueType* GetVal(HashMap_Str<ValueType> map, i32 hashIndex, const char* key)
{
    ValueType* result{};

    ui16 uniqueKeyID = _ProduceUniqueIDForString(key);

    b run{ true };
    KeyInfo<ValueType>* nextKey{ &map.keyInfos.At(hashIndex) };
    while (run)
    {
        if (nextKey->uniqueID == uniqueKeyID)
        {
            result = &nextKey->value;
            run = false;
        }
        else
        {
            nextKey = nextKey->nextInfo;
        }
    };

    return result;
};
#endif