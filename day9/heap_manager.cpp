#include <iostream>
#include <cstdlib>

// Custom assertion macro definition
// Exit the program in case of failure
#define massert(cond) \
    if (! (cond)) { \
        std::cout << "Assertion failed: " #cond "." << std::endl; \
        std::cout << "File: " __FILE__ ", Line: " << __LINE__ << std::endl; \
        abort(); \
    }

// Header to keep the size of chunk, pointers to next and previous chunks
typedef struct metadata {
    // Chunk size
    size_t size;

    // Flag to mark the space if free
    bool is_free;

    // Next free block
    struct metadata* next;
    
    // Previous free block
    struct metadata* prev;

} metadata_t;

// Heap manager for demonstration purposes only
class HeapManager {
private:
    // Local heap buffer
    unsigned char* buffer;

public:
    ~HeapManager() {
        delete[] buffer;
    }

    HeapManager(const size_t n) {
        // Check preconditions
        massert(0 != n);

        // One header should be exists
        massert(n > sizeof(metadata_t));

        // Allocate the local heap data
        buffer = new unsigned char[n];
        massert(NULL != buffer);

        // cast the buffer to store initial header data
        metadata_t* meta = (metadata_t*)buffer;

        // Next, we have (n - sizeof(metadata_t)) bytes free for first use
        meta->size = n - sizeof(metadata_t);

        // No next, no previous pointers are available for first chunk
        meta->next = NULL;
        meta->prev = NULL;

        // Freee? yep
        meta->is_free = true;
    }

    // Dump the local buffer in order to test
    void dump() {
        // cast the buffer to get the first header data
        metadata_t* meta = (metadata_t*)buffer;
        massert(NULL != meta);

        std::cout << "Header size: " << sizeof(metadata_t) << std::endl;
        while (NULL != meta) {
            std::cout << "-header-";
            for (int i = 0; i < meta->size; ++i) {
                std::cout << int(meta->is_free);
            }
            meta = meta->next;
        }
        std::cout << std::endl;
    }

    // malloc 'n' bytes if possible, if not, NULL will be returned
    void* malloc(const size_t n) {
        massert(0 < n);

        // cast the buffer to get the first header data
        metadata_t* meta = (metadata_t*)buffer;
        massert(NULL != meta);

        // Look for free blocks to have more than 'n' bytes data
        // additonal 'sizeof(metadata_t) + 1 bytes are required to add a new header
        // Or, the free block size is equal to n, w'll just change its flag
        const size_t m = n + sizeof(metadata_t);
        while (! ((meta->size > m || meta->size == n) && meta->is_free)) {
            meta = meta->next;
            // There is no n-bytes free space, so we can not allocate new data
            if (NULL == meta) {
                return NULL;
            }
        }
        // Check the found block
        massert(NULL != meta);
        massert(meta->is_free);
        massert(meta->size > m || meta->size == n);

        // We got a new pointer here, nice :)
        unsigned char* ptr = (unsigned char*)meta + sizeof(metadata_t);
        // There is a space left after the new ptr, so we need to add a new header here
        if (n < meta->size) {
            // We should allocate new header here
            metadata_t* new_meta = (metadata_t*) (ptr + n);

            // assign pointers to keep track the loops with pointers
            new_meta->next = meta->next;
            new_meta->prev = meta;

            // Change the current meta pointer
            meta->next = new_meta;
            if (NULL != new_meta->next) {
                new_meta->next->prev = new_meta;
            }

            // assign new sizes
            new_meta->size = meta->size - m;
            massert(0 < new_meta->size);

            meta->size = n;
            new_meta->is_free = true;
        }
        meta->is_free = false;
        return (void*) ptr;
    }

    // Enlarge the block after the free API invocation
    void enlarge_left_block(metadata_t* meta) {
        // Check preconditions
        massert(NULL != meta);
        massert(meta->is_free);

        if (NULL != meta->prev && meta->prev->is_free) {
            // enlarge the space
            meta->prev->next = meta->next;
            meta->prev->size += (sizeof(metadata_t)) + meta->size;
            if (NULL != meta->next) {
                meta->next->prev = meta->prev;
            }
        }
    }

    // Ops, free the malloc-ed address
    void free(void* addr) {
        massert(NULL != addr);

        // Get the header start address
        metadata_t* meta = (metadata_t*)((unsigned char*)addr - sizeof(metadata_t));

        // Check preconditions
        massert(NULL != meta);
        massert(0 != meta->size);
        massert(! meta->is_free);

        // Free
        meta->is_free = true;

        // Enlarge the previous block if possible
        enlarge_left_block(meta);

        // Enlarge the next block if possible
        if (NULL != meta->next && meta->next->is_free) {
            enlarge_left_block(meta->next);
        }
    }
};

// test1, nop
void test1(HeapManager& manager)
{
    void* addr0 = NULL;
    void* addr1 = NULL;

    addr0 = manager.malloc(5);
    massert(NULL != addr0);
    manager.dump();

    addr1 = manager.malloc(11);
    massert(NULL != addr1);
    manager.dump();

    manager.free(addr0);
    manager.dump();

    manager.free(addr1);
    manager.dump();
}

// test2, nop
void test2(HeapManager& manager)
{
    void* addr0 = NULL;
    void* addr1 = NULL;
    void* addr2 = NULL;

    addr0 = manager.malloc(5);
    addr1 = manager.malloc(5);
    addr2 = manager.malloc(22);
    manager.dump();

    manager.free(addr0);
    manager.free(addr2);
    manager.free(addr1);
    manager.dump();
}

int main()
{
    HeapManager manager(128);
    manager.dump();

    test1(manager);
    test2(manager);

    return 0;
}
