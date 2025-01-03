#include "vector.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

void VectorNew(vector *v, int elemSize, VectorFreeFunction freeFn, int initialAllocation)
{
    if(initialAllocation == 0) initialAllocation = 4;
    assert(initialAllocation > 0);
    v->allocLen = initialAllocation;
    v->logLen = 0;
    v->freeFn = freeFn;
    v->elemSize = elemSize;
    v->elem = malloc(v->allocLen * elemSize);
    assert(v->elem != NULL);
}

void VectorDispose(vector *v)
{
    if(v->freeFn != NULL){
        for(int i = 0; i < v->logLen; i++){
            void* elem = (char*) v->elem + v->elemSize * i;
            v->freeFn(elem);
        }
    }
    free(v->elem);
}

int VectorLength(const vector *v)
{ 
    return v->logLen; 
}

void *VectorNth(const vector *v, int position)
{
    assert(position >= 0);
    assert(v->logLen - 1 >= position);
    return (char*)v->elem + v->elemSize * position;
}

void VectorReplace(vector *v, const void *elemAddr, int position)
{
    assert(position >= 0);
    assert(v->logLen - 1 >= position);
    if(v->freeFn != NULL) v->freeFn((void*)((char*)v->elem + v->elemSize * position));
    memcpy((void*)((char*)v->elem + v->elemSize * position), elemAddr, v->elemSize);
}

void VectorInsert(vector *v, const void *elemAddr, int position)
{
    assert(position >= 0);
    assert(v->logLen >= position);
    if(v->logLen == v->allocLen){
        v->allocLen *= 2;
        v->elem = realloc(v->elem, v->allocLen * v->elemSize);
        assert(v->elem != NULL);        
    }

    memmove((void*)((char*)v->elem + v->elemSize * (position + 1)), (void*)((char*)v->elem + v->elemSize * position),
                (v->logLen - position) * v->elemSize);
    
    memcpy((void*)((char*)v->elem + v->elemSize * position), elemAddr, v->elemSize);

    v->logLen++;
}

void VectorAppend(vector *v, const void *elemAddr)
{
    if(v->logLen == v->allocLen){
        v->allocLen *= 2;
        v->elem = realloc(v->elem, v->allocLen * v->elemSize);
        assert(v->elem != NULL);        
    }

    memcpy((void*)((char*)v->elem + v->elemSize * v->logLen), elemAddr, v->elemSize);
    v->logLen++;
}

void VectorDelete(vector *v, int position)
{
    assert(position >= 0);
    assert(v->logLen - 1 >= position);
    if(v->freeFn != NULL){
        v->freeFn((void*)((char*)v->elem + v->elemSize * position));
    }

    memmove((void*)((char*)v->elem + v->elemSize * position), (void*)((char*)v->elem + v->elemSize * (position + 1)),
                (v->logLen - position - 1) * v->elemSize);
    v->logLen--;
}

void VectorSort(vector *v, VectorCompareFunction compare)
{
    assert(compare != NULL);
    qsort(v->elem, v->logLen, v->elemSize, compare);
}

void VectorMap(vector *v, VectorMapFunction mapFn, void *auxData)
{
    assert(mapFn != NULL);
    for (int i = 0; i < v->logLen; i++)
        mapFn((void*)((char*)v->elem + i * v->elemSize), auxData);
}

static const int kNotFound = -1;
int VectorSearch(const vector *v, const void *key, VectorCompareFunction searchFn, int startIndex, bool isSorted)
{
    assert(startIndex >= 0);
    assert(startIndex <= v->logLen);
    assert(key != NULL);
    assert(searchFn != NULL);

    void* foundAddr = NULL; 
    if(isSorted) 
        foundAddr = bsearch(key, (char*)v->elem + (startIndex * v->elemSize),
                 v->logLen - startIndex, v->elemSize, searchFn);
    else{
        for(int i = startIndex; i < v->logLen; i++){
            void* elem = (char*)v->elem + i * v->elemSize;
            if(!searchFn(key, elem)) return i;
        }
    }

    if(foundAddr == NULL) return kNotFound;

    return (foundAddr - v->elem) / v->elemSize;
        
} 
