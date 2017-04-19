#include <iostream>
#include <cstdlib>

#include <vector>
#include <iterator>
#include <algorithm>

// Custom assertion macro definition
// Exit the program in case of failure
#define massert(cond) \
    if (! (cond)) { \
        std::cout << "Assertion failed: " #cond "." << std::endl; \
        std::cout << "File: " __FILE__ ", Line: " << __LINE__ << std::endl; \
        abort(); \
    }

// Header to keep the fs inode data block
#define CHUNK_SIZE 4096
class data_t {
    // Block pointer
    char* ptr;

    // already filled bytes count
    size_t filled_size;

    // initially is free
    bool is_free;

    public:
    data_t(char* ptr) : ptr(ptr), filled_size(0), is_free(true) {}

    void setFill(size_t len) {
        massert(CHUNK_SIZE >= len);
        filled_size = len;
    }

    void setFree(bool flag) { 
        is_free = flag;
    }

    size_t getFilledSize() { return filled_size; }
    bool isFree() { return is_free; }

    // operator overload to read the byte at the address 'i'
    char& operator [] (size_t i) {
        massert(! isFree());
        massert(i < filled_size);
        massert(NULL != ptr);
        return ptr[i];
    }

    // puts a char at the end of the block
    void put(char c) {
        massert(filled_size < CHUNK_SIZE);
        ptr[filled_size++] = c;
    }
};

// Memory allocator block to manage free/used block from FS
class FS_allocator {
    std::vector<data_t*> blocks;
    char* fs;

    public:
    // totoal file size to be managed
    // allocates data blocks
    FS_allocator(size_t fs_size) {

        size_t c = fs_size / CHUNK_SIZE;
        massert(0 == (fs_size % CHUNK_SIZE));
        fs = new char[fs_size];
        massert(NULL != fs);

        for (size_t i = 0; i < c; ++i) {
            blocks.push_back(new data_t(fs + i * CHUNK_SIZE));
        }
    }

    ~FS_allocator() {
        delete fs;
        for (size_t i = 0; i < blocks.size(); ++i) {
            delete blocks[i];
        }
    }

    // Heavy check the free blocks and return
    data_t* getFreeBlock() {
        for (size_t i = 0; i < blocks.size(); ++i) {
            data_t* blk = blocks[i];
            if (blk->isFree()) {
                blk->setFree(false);
                blk->setFill(0);

                return blocks[i];
            }
        }
        return NULL;
    }
};

// Mimic the FS inode for a file
class inode_t {
    // used blocks
    std::vector<data_t*> blocks;

    // Reference to FS allocator
    FS_allocator* fs_allocator;

    // Current file size
    size_t file_size;

    // add a new block to list of used blocks
    void addBlock(data_t* block) {
        massert(NULL != block);
        blocks.push_back(block);

        block->setFree(false);
        block->setFill(0);
    }

    public:
    // construct here
    inode_t(FS_allocator* allocator) : file_size(0), fs_allocator(allocator) {
        massert(NULL != allocator);
    }

    // read a char from the file at the address 'i'
    char& operator [](size_t i) {
        size_t idx = i / CHUNK_SIZE;
        size_t offset = i % CHUNK_SIZE;
        massert (idx < blocks.size());

        return (*blocks[idx])[offset];
    }

    // empty file content
    void clear() {
        for (size_t i = 0; i < blocks.size(); ++i) {
            blocks[i]->setFree(true);
            blocks[i]->setFill(0);
        }
        blocks.clear();
    }

    // put a char to the end of the file
    void put(char c) {
        size_t i = file_size;
        size_t idx = i / CHUNK_SIZE;

        if (idx == blocks.size()) {
            massert(NULL != fs_allocator);
            addBlock(fs_allocator->getFreeBlock());
        }

        blocks[idx]->put(c);
        ++file_size;
    }
};

// Main FS manager with allocator
class FS
{
    FS_allocator fs_allocator;
    std::vector<inode_t*> fs;
    public:

    FS(size_t fs_size) : fs_allocator(fs_size) {}

    ~FS() {
        for (size_t i = 0; i < fs.size(); ++i) {
            delete fs[i];
        }
    }

    // Open a file if exists otherwise create a new one
    inode_t* open(size_t i) {
        if (i == fs.size()) {
            inode_t* node = new inode_t(&fs_allocator);
            fs.push_back(node);
        }
        massert(i < fs.size());
        if (NULL == fs[i]) {
            fs[i] = new inode_t(&fs_allocator);
        }
        massert(NULL != fs[i]);

        return fs[i];
    }

    // Unlink the file from the FS
    void unlink(inode_t* node) {
        auto it = std::find(fs.begin(), fs.end(), node);
        delete *it;
        *it = NULL;
    }

    // Put a char at the end of file
    void write(inode_t* node, char c) {
        massert(NULL != node);
        node->put(c);
    }

    // Write a char at the given address
    void write(inode_t* node, size_t addr, char c) {
        massert(NULL != node);
        (*node)[addr] = c;
    }

    // Read from the address
    char read(inode_t* node, size_t addr) {
        massert(NULL != node);
        return (*node)[addr];
    }
};

void test1()
{
    FS(CHUNK_SIZE);
}

void test2()
{
    FS fs(5 * CHUNK_SIZE);
    inode_t* n = fs.open(0);

    std::cout << "write: " << 1 << std::endl;
    fs.write(n, '1');

    std::cout << "write: " << 2 << std::endl;
    fs.write(n, 0, '2');

    std::cout << "read: " << fs.read(n, 0) << std::endl;
}

void test3()
{
    FS fs(5 * CHUNK_SIZE);
    inode_t* n = fs.open(0);

    for (size_t i = 0; i < 5 * CHUNK_SIZE; ++i) {
        fs.write(n, char(i));
        massert(char(i) == fs.read(n, i));
    }
    fs.unlink(n);
}

void test4()
{
    FS fs(6 * CHUNK_SIZE);
    for (size_t j = 0; j < 3; ++j) {
        inode_t* n = fs.open(j);

        for (size_t i = 0; i < 2 * CHUNK_SIZE; ++i) {
            fs.write(n, char(i));
            massert(char(i) == fs.read(n, i));
        }
    }
    for (size_t j = 0; j < 3; ++j) {
        inode_t* n = fs.open(j);
        fs.unlink(n);
    }
}

void test5()
{
    FS fs(6 * CHUNK_SIZE);
    for (size_t j = 0; j < 3; ++j) {
        inode_t* n = fs.open(j);

        for (size_t i = 0; i < 2 * CHUNK_SIZE; ++i) {
            fs.write(n, char(i));
            massert(char(i) == fs.read(n, i));
        }
    }
    for (size_t j = 0; j < 3; ++j) {
        inode_t* n = fs.open(j);
        fs.unlink(n);
    }
}

int main()
{
    test1();
    test2();
    test3();
    test4();
    test5();
    return 0;
}
