#include <stdlib.h>
#include <crioulo/pool_allocator.h>

template <class T>
PoolAllocator<T>::~PoolAllocator()
{
  for(void* blockPtr : m_blockPtrs)
  {
    free(blockPtr);
  }
}

template <class T>
T *PoolAllocator<T>::allocate()
{
  if (m_alloc == nullptr) {
    m_alloc = allocateBlock();
  }
 
  Chunk* freeChunk = m_alloc;
 
  m_alloc = m_alloc->next;
 
  return reinterpret_cast<T*>(freeChunk);
}

template <class T>
void PoolAllocator<T>::deallocate(T* p_chunk)
{ 
  reinterpret_cast<Chunk*>(p_chunk)->next = m_alloc;
 
  m_alloc = reinterpret_cast<Chunk*>(p_chunk);
}

template <class T>
Chunk* PoolAllocator<T>::allocateBlock()
{
    size_t blockSize = m_chunksPerBlock * sizeof(T);

    Chunk* blockBegin = reinterpret_cast<Chunk*>(malloc(blockSize));
    m_blockPtrs.push_back(blockBegin);
 
    Chunk* chunk = blockBegin;
 
    for (int i = 0; i < m_chunksPerBlock - 1; ++i) {
      chunk->next = reinterpret_cast<Chunk*>(reinterpret_cast<char*>(chunk) + sizeof(T));
      chunk = chunk->next;
    }
 
    chunk->next = nullptr;
 
    return blockBegin;
}
