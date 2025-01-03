#include "hashset.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>

void HashSetNew(hashset *h, int elemSize, int numBuckets,
		HashSetHashFunction hashfn, HashSetCompareFunction comparefn, HashSetFreeFunction freefn)
{
	assert(elemSize > 0);
	assert(numBuckets > 0);
	assert(hashfn != NULL);
	assert(comparefn != NULL);
	h->elemSize = elemSize;
	h->numBuckets = numBuckets;
	h->hashfn = hashfn;
	h->comparefn = comparefn;
	h->freefn = freefn;
	h->elemNum = 0;
	h->bucket = malloc(numBuckets * sizeof(vector));
	for(int i = 0; i < numBuckets; i++)
		VectorNew((void*)((char*)h->bucket + i * sizeof(vector)), h->elemSize, freefn, 4);
}

void HashSetDispose(hashset *h)
{
	for(int i = 0; i < h->numBuckets; i++)
		VectorDispose(h->bucket + i);
	free(h->bucket);
}

int HashSetCount(const hashset *h)
{
	return h->elemNum;
}

void HashSetMap(hashset *h, HashSetMapFunction mapfn, void *auxData)
{
	assert(mapfn != NULL);
	for(int i = 0; i < h->numBuckets; i++)
		VectorMap(h->bucket + i, mapfn, auxData);
}

void HashSetEnter(hashset *h, const void *elemAddr)
{
	assert(elemAddr	!= NULL);

	int hash = h->hashfn(elemAddr, h->numBuckets);
	assert(hash >= 0);
	assert(hash < h->numBuckets);

	int elemIdx = VectorSearch(h->bucket + hash, elemAddr,
								h->comparefn, 0, false);

	if(elemIdx != -1){
		VectorReplace(h->bucket + hash, elemAddr, elemIdx);
		return;
	}
	else VectorAppend(h->bucket + hash, elemAddr);

	h->elemNum++;
}

void *HashSetLookup(const hashset *h, const void *elemAddr)
{ 
	assert(elemAddr	!= NULL);

	int hash = h->hashfn(elemAddr, h->numBuckets);
	assert(hash >= 0);
	assert(hash < h->numBuckets);

	int elemIdx = VectorSearch(h->bucket + hash, elemAddr,
								h->comparefn, 0, false);
	
  if(elemIdx != -1) return VectorNth(h->bucket + hash, elemIdx);
  else return NULL;
}
