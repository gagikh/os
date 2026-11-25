// Day 3: Files and Directories
// Goal: build a tiny ext2-like file system that lives in an array, and see from
//       the inside what inodes, blocks, directories, hard links and holes are.
//
//   "disk"      : kBlockCount blocks of kBlockSize bytes (block 0 is reserved)
//   inode       : type, link count, size, 12 direct block numbers, 1 indirect block
//   directory   : a file whose data is an array of {inode number, name} entries
//   root        : inode 2, as in ext2
//
// Compile:  g++ -std=c++17 -Wall -Wextra -O2 template.cpp -o day03
// Run:      ./day03      (every test prints PASS or FAIL; exit status = number of failures)
//
// Work through the TODOs in order: each one turns more tests from FAIL to PASS.

#include <algorithm>
#include <array>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <string>
#include <vector>

// ---------------------------------------------------------------------------
// Geometry. Real ext4 uses 4096-byte blocks; small numbers here make the
// indirect block and the "disk full" case easy to reach in a test.
// ---------------------------------------------------------------------------
constexpr std::uint32_t kBlockSize = 256;
constexpr std::uint32_t kBlockCount = 128;                     // a 32 KB disk
constexpr std::uint32_t kInodeCount = 32;
constexpr std::uint32_t kDirect = 12;                          // direct pointers per inode
constexpr std::uint32_t kPointersPerBlock = kBlockSize / sizeof(std::uint32_t);   // 64
constexpr std::uint32_t kMaxFileSize = (kDirect + kPointersPerBlock) * kBlockSize;  // 19456
constexpr std::uint32_t kNoBlock = 0;     // block 0 is reserved, so 0 can mean "no block"
constexpr std::uint32_t kNoInode = 0;     // inode 0 is never used, as in ext2
constexpr std::uint32_t kRootInode = 2;   // ext2's root inode number
constexpr std::uint32_t kNameLen = 28;

enum class type_t : std::uint8_t { free, file, dir };

struct inode_t {
    type_t type = type_t::free;
    std::uint16_t links = 0;                // how many directory entries point here
    std::uint32_t size = 0;                 // bytes
    std::uint32_t direct[kDirect] = {};     // block numbers of the first 12 blocks
    std::uint32_t indirect = kNoBlock;      // block that holds 64 more block numbers
};

struct dirent_t {                           // one directory entry: 32 bytes
    std::uint32_t inode = kNoInode;         // kNoInode marks an empty slot
    char name[kNameLen] = {};
};

using block_t = std::array<std::uint8_t, kBlockSize>;

class mini_fs {
public:
    mini_fs() { format(); }

    // =======================================================================
    // Helpers: ready to use, read them but do not change them
    // =======================================================================

    // Like mke2fs: empty disk, empty inode table, a root directory with . and ..
    void format()
    {
        disk_.assign(kBlockCount, block_t{});
        block_used_.assign(kBlockCount, false);
        block_used_[0] = true;                       // reserved, like the ext2 boot block
        inodes_.assign(kInodeCount, inode_t{});
        inodes_[0].type = inodes_[1].type = type_t::file;   // reserved numbers 0 and 1
        inodes_[kRootInode].type = type_t::dir;
        add_entry(kRootInode, ".", kRootInode);
        add_entry(kRootInode, "..", kRootInode);
    }

    std::uint32_t alloc_block()
    {
        for (auto b = std::uint32_t{1}; b < kBlockCount; ++b) {
            if (!block_used_[b]) {
                block_used_[b] = true;
                disk_[b].fill(0);
                return b;
            }
        }
        return kNoBlock;                             // disk full
    }

    void free_block(std::uint32_t b)
    {
        if (kNoBlock != b) block_used_[b] = false;
    }

    std::uint32_t alloc_inode(type_t type)
    {
        for (auto i = std::uint32_t{1}; i < kInodeCount; ++i) {
            if (type_t::free == inodes_[i].type) {
                inodes_[i] = inode_t{};
                inodes_[i].type = type;
                return i;
            }
        }
        return kNoInode;                             // no free inodes: "No space left on device"
    }

    // Number of 32-bit block numbers stored inside block b (used for the indirect block)
    std::uint32_t *pointers(std::uint32_t b)
    {
        return reinterpret_cast<std::uint32_t *>(disk_[b].data());
    }

    // Read up to len bytes from offset. A missing block (a hole) reads as zeros.
    std::uint32_t read(std::uint32_t ino, std::uint32_t offset, void *buf, std::uint32_t len)
    {
        auto &node = inodes_[ino];
        if (offset >= node.size) return 0;
        len = std::min(len, node.size - offset);
        auto out = static_cast<std::uint8_t *>(buf);
        for (auto done = std::uint32_t{0}; done < len;) {
            auto const pos = offset + done;
            auto const in_block = pos % kBlockSize;
            auto const chunk = std::min(kBlockSize - in_block, len - done);
            auto const b = block_of(node, pos / kBlockSize, false);
            if (kNoBlock == b) std::memset(out + done, 0, chunk);
            else std::memcpy(out + done, disk_[b].data() + in_block, chunk);
            done += chunk;
        }
        return len;
    }

    // Append an entry to a directory and count the new link to `ino`
    bool add_entry(std::uint32_t dir, const std::string &name, std::uint32_t ino)
    {
        if (name.empty() || name.size() >= kNameLen) return false;
        auto e = dirent_t{};
        e.inode = ino;
        std::strncpy(e.name, name.c_str(), kNameLen - 1);
        if (sizeof(e) != write(dir, inodes_[dir].size, &e, sizeof(e))) return false;
        ++inodes_[ino].links;
        return true;
    }

    // Empty the slot of `name` in a directory. Does NOT touch link counts.
    bool remove_entry(std::uint32_t dir, const std::string &name)
    {
        for (auto off = std::uint32_t{0}; off < inodes_[dir].size; off += sizeof(dirent_t)) {
            auto e = dirent_t{};
            read(dir, off, &e, sizeof(e));
            if (kNoInode != e.inode && name == e.name) {
                auto const empty = dirent_t{};
                write(dir, off, &empty, sizeof(empty));
                return true;
            }
        }
        return false;
    }

    // "/docs/a.txt" -> {"/docs", "a.txt"};  "/a.txt" -> {"/", "a.txt"}
    static std::pair<std::string, std::string> split_path(const std::string &path)
    {
        auto const slash = path.find_last_of('/');
        auto const parent = (0 == slash) ? std::string("/") : path.substr(0, slash);
        return {parent, path.substr(slash + 1)};
    }

    // Create an empty file or directory (like touch / mkdir). Returns its inode.
    std::uint32_t create(const std::string &path, type_t type)
    {
        auto const [parent_path, name] = split_path(path);
        auto const parent = resolve(parent_path);
        if (kNoInode == parent || type_t::dir != inodes_[parent].type) return kNoInode;
        if (kNoInode != lookup(parent, name)) return kNoInode;          // already exists
        auto const ino = alloc_inode(type);
        if (kNoInode == ino) return kNoInode;
        add_entry(parent, name, ino);
        if (type_t::dir == type) {
            add_entry(ino, ".", ino);
            add_entry(ino, "..", parent);
        }
        return ino;
    }

    const inode_t &inode(std::uint32_t ino) const { return inodes_[ino]; }

    std::uint32_t free_blocks() const
    {
        auto n = std::uint32_t{0};
        for (auto used : block_used_) n += used ? 0 : 1;
        return n;
    }

    std::uint32_t free_inodes() const
    {
        auto n = std::uint32_t{0};
        for (auto &node : inodes_) n += (type_t::free == node.type) ? 1 : 0;
        return n;
    }

    // Like stat(1): what the inode knows. Note that the name is not there.
    void print_stat(std::uint32_t ino) const
    {
        auto &n = inodes_[ino];
        std::cout << "  inode " << ino << ": " << (type_t::dir == n.type ? "directory" : "regular file")
                  << ", size " << n.size << ", links " << n.links << ", blocks [";
        for (auto b : n.direct) if (kNoBlock != b) std::cout << ' ' << b;
        std::cout << " ]" << (kNoBlock != n.indirect ? " + indirect" : "") << '\n';
    }

    // =======================================================================
    // Your turn: replace each TODO body
    // =======================================================================

    // TODO 1: which disk block holds block number `index` of the file?
    //   - index < kDirect:  node.direct[index]
    //   - otherwise:        entry (index - kDirect) of the indirect block, pointers(node.indirect)
    //   - index >= kDirect + kPointersPerBlock: too big, return kNoBlock
    //   - if the block (or the indirect block itself) is kNoBlock:
    //       allocate == false -> return kNoBlock (a hole)
    //       allocate == true  -> alloc_block() and store the new number in the inode / indirect block
    std::uint32_t block_of(inode_t &node, std::uint32_t index, bool allocate)
    {
        (void)node;
        (void)index;
        (void)allocate;    // remove these lines when you use the parameters
        return kNoBlock;
    }

    // TODO 2: write len bytes at offset. Returns the number of bytes written.
    //   - refuse writes that would end past kMaxFileSize (return 0)
    //   - copy block by block, exactly like read() above, but with block_of(..., true)
    //     (if it returns kNoBlock the disk is full: stop and return what was written)
    //   - grow node.size if the write ended past the old size
    //   - writing at an offset far past the end leaves a hole: the skipped blocks stay kNoBlock
    std::uint32_t write(std::uint32_t ino, std::uint32_t offset, const void *buf, std::uint32_t len)
    {
        (void)ino;
        (void)offset;
        (void)buf;
        (void)len;    // remove these lines when you use the parameters
        return 0;
    }

    // TODO 3: find `name` in directory `dir`; return its inode number or kNoInode.
    //   - the directory's data is an array of dirent_t: read them one by one with read()
    //   - skip empty slots (e.inode == kNoInode); compare with: name == e.name
    std::uint32_t lookup(std::uint32_t dir, const std::string &name)
    {
        (void)dir;
        (void)name;    // remove these lines when you use the parameters
        return kNoInode;
    }

    // TODO 4: path -> inode number, the way ext2 finds a file:
    //   start at kRootInode; for each component between '/' characters, lookup() it
    //   in the current directory. Every component except the last must be a directory.
    //   "/" -> kRootInode.  Missing component -> kNoInode.
    std::uint32_t resolve(const std::string &path)
    {
        (void)path;    // remove this line when you use the parameter
        return kNoInode;
    }

    // TODO 5: hard link, like ln target new_path. Returns true on success.
    //   - target must exist and be a regular file (no hard links to directories: why?)
    //   - new_path's parent must exist and new_path must not
    //   - add_entry(parent, name, target_inode): the SAME inode, one more link
    bool link(const std::string &target, const std::string &new_path)
    {
        (void)target;
        (void)new_path;    // remove these lines when you use the parameters
        return false;
    }

    // TODO 6: remove a name, like rm path. Returns true on success.
    //   - path must exist and be a regular file
    //   - remove_entry(parent, name) and decrease the link count
    //   - when links reaches 0, free the data: every direct block, every block listed in
    //     the indirect block, the indirect block itself; then mark the inode type_t::free
    bool unlink(const std::string &path)
    {
        (void)path;    // remove this line when you use the parameter
        return false;
    }

private:
    std::vector<block_t> disk_;
    std::vector<bool> block_used_;
    std::vector<inode_t> inodes_;
};

// ---------------------------------------------------------------------------
// Tests: already complete. Read them to see what each TODO must do.
// ---------------------------------------------------------------------------
int failures = 0;

void check(bool ok, const char *what)
{
    std::cout << (ok ? "  PASS  " : "  FAIL  ") << what << '\n';
    failures += ok ? 0 : 1;
}

std::string read_all(mini_fs &fs, std::uint32_t ino)
{
    auto s = std::string(fs.inode(ino).size, '\0');
    fs.read(ino, 0, s.data(), static_cast<std::uint32_t>(s.size()));
    return s;
}

void test_write_read()                                    // TODO 1, 2
{
    std::cout << "write and read back\n";
    auto fs = mini_fs{};
    auto const before = fs.free_blocks();
    auto const ino = fs.alloc_inode(type_t::file);
    check(5 == fs.write(ino, 0, "hello", 5), "write returns 5");
    check("hello" == read_all(fs, ino), "read gives \"hello\"");
    check(before - 1 == fs.free_blocks(), "a 5-byte file takes one whole block");
    fs.print_stat(ino);
}

void test_indirect()                                      // TODO 1, 2
{
    std::cout << "a file bigger than 12 blocks needs the indirect block\n";
    auto fs = mini_fs{};
    auto const ino = fs.alloc_inode(type_t::file);
    auto data = std::string(14 * kBlockSize, '\0');
    for (auto i = std::size_t{0}; i < data.size(); ++i) data[i] = static_cast<char>('a' + i % 26);
    check(data.size() == fs.write(ino, 0, data.data(), static_cast<std::uint32_t>(data.size())), "write 14 blocks");
    check(kNoBlock != fs.inode(ino).indirect, "indirect block allocated");
    check(data == read_all(fs, ino), "content survives the trip through the indirect block");
    check(0 == fs.write(ino, kMaxFileSize, "x", 1), "writing past kMaxFileSize is refused");
    fs.print_stat(ino);
}

void test_hole()                                          // TODO 1, 2
{
    std::cout << "a sparse file: writing far past the end leaves a hole\n";
    auto fs = mini_fs{};
    auto const before = fs.free_blocks();
    auto const ino = fs.alloc_inode(type_t::file);
    fs.write(ino, 10 * kBlockSize, "!", 1);
    check(10 * kBlockSize + 1 == fs.inode(ino).size, "size counts the hole");
    check(before - 1 == fs.free_blocks(), "but only one block is used on disk");
    auto c = 'x';
    fs.read(ino, 5, &c, 1);
    check('\0' == c, "the hole reads as zeros");
    fs.print_stat(ino);
}

void test_directories()                                   // TODO 3, 4
{
    std::cout << "directories and paths\n";
    auto fs = mini_fs{};
    check(kRootInode == fs.lookup(kRootInode, "."), "/ contains . pointing to itself");
    auto const docs = fs.create("/docs", type_t::dir);
    auto const note = fs.create("/docs/note.txt", type_t::file);
    check(kNoInode != docs && kNoInode != note, "mkdir /docs, touch /docs/note.txt");
    check(note == fs.resolve("/docs/note.txt"), "resolve finds /docs/note.txt");
    check(kRootInode == fs.resolve("/docs/.."), "/docs/.. is /");
    check(kNoInode == fs.resolve("/docs/missing"), "a missing name resolves to nothing");
    check(2 == fs.inode(docs).links, "a new directory has 2 links: its name and its .");
    check(3 == fs.inode(kRootInode).links, "/ has 3 links: . .. and /docs/..");
    fs.print_stat(docs);
}

void test_hard_links()                                    // TODO 5, 6
{
    std::cout << "hard links and unlink\n";
    auto fs = mini_fs{};
    auto const before = fs.free_blocks();
    fs.create("/docs", type_t::dir);
    auto const ino = fs.create("/a.txt", type_t::file);
    fs.write(ino, 0, "shared", 6);
    check(fs.link("/a.txt", "/docs/b.txt"), "ln /a.txt /docs/b.txt");
    check(ino == fs.resolve("/docs/b.txt"), "both names lead to the same inode");
    check(2 == fs.inode(ino).links, "link count is 2");
    check(!fs.link("/docs", "/d2"), "hard link to a directory is refused");
    check(fs.unlink("/a.txt"), "rm /a.txt");
    check(kNoInode == fs.resolve("/a.txt") && "shared" == read_all(fs, ino), "data still there through /docs/b.txt");
    check(fs.unlink("/docs/b.txt"), "rm /docs/b.txt");
    check(type_t::free == fs.inode(ino).type, "last link gone: inode is free");
    check(before - 1 == fs.free_blocks(), "the file's block is free again; only /docs's block stays");
}

int main()
{
    test_write_read();
    test_indirect();
    test_hole();
    test_directories();
    test_hard_links();
    std::cout << (0 == failures ? "all tests passed\n" : "some tests failed\n");
    return failures;
}
