#ifndef CRIOULO_POOL_ALLOCATOR_H
#define CRIOULO_POOL_ALLOCATOR_H

#include <vector>

struct Chunk {
  Chunk *next;
};

template <class T>
class PoolAllocator {
  public:
    PoolAllocator(size_t p_chunksPerBlock)
      : m_chunksPerBlock(p_chunksPerBlock) 
    {
      m_blockPtrs.reserve(100);

      //If the size of T is not big enough to hold Chunk it'll lead to overflows.
      //Todo: Log properly only on Debug.
      if(sizeof(T) < sizeof(Chunk))
      {
        throw "The type is too small for storing in a PoolAllocator."
      }
    }

    ~PoolAllocator()
    {
      for(void* blockPtr : m_blockPtrs)
      {
        free(blockPtr);
      }
    }
  
    T* allocate()
    {
      if (m_alloc == nullptr) {
        m_alloc = allocateBlock();
      }
     
      Chunk* freeChunk = m_alloc;
     
      m_alloc = m_alloc->next;
     
      return reinterpret_cast<T*>(freeChunk);
    }

    void deallocate(T* p_chunk)
    { 
      reinterpret_cast<Chunk*>(p_chunk)->next = m_alloc;
     
      m_alloc = reinterpret_cast<Chunk*>(p_chunk);
    }
 
  private:
    size_t m_chunksPerBlock;
 
    Chunk* m_alloc = nullptr;

    std::vector<void*> m_blockPtrs;
 
    Chunk* allocateBlock()
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
};
#endif

