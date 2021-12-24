#pragma once

#include "path.hpp"

#include "util/bitmaskenum.hpp"
#include "types.hpp"


enum class FSPermission {

    OwnerRead = 0x100,
    OwnerWrite = 0x80,
    OwnerExecute = 0x40,

    GroupRead = 0x20,
    GroupWrite = 0x10,
    GroupExecute = 0x8,

    UserRead = 0x4,
    UserWrite = 0x2,
    UserExecute = 0x1,

    OwnerAll = OwnerRead | OwnerWrite | OwnerExecute,
    GroupAll = GroupRead | GroupWrite | GroupExecute,
    UserAll = UserRead | UserWrite | UserExecute,

    SetUID = 0x800,
    SetGID = 0x400,
    SBit = 0x200,

    All = OwnerAll | GroupAll | UserAll,
    None = 0x0,
    Mask = 0xFFF,
    Unknown = 0xFFFF

};

ARC_CREATE_BITMASK_ENUM(FSPermission)



class FSEntry {

public:

    enum class Type {
        File = 0x1,
        Directory = 0x2,
        Symlink = 0x4,
        BlockDevice = 0x8,
        CharacterDevice = 0x10,
        Pipe = 0x20,
        Socket = 0x40,
        Unknown = 0x8000,
        All = 0x807F
    };

    using enum Type;

    FSEntry(const Path& path);
    explicit FSEntry(const std::filesystem::directory_entry& entry);

    bool exists() const;
    Type getType() const;

    bool isFile() const;
    bool isDirectory() const;
    bool isSymlink() const;
    bool isBlockDevice() const;
    bool isCharDevice() const;
    bool isPipe() const;
    bool isSocket() const;
    bool isUnknown() const;

    Path getPath() const;

    umax getHardLinkCount() const;

    u64 getLastModifiedTime() const;
    void setLastModifiedTime(u64 nanos);

    FSPermission getPermissions() const;
    bool hasPermissions(FSPermission perms) const;
    void setPermissions(FSPermission perms);
    void addPermissions(FSPermission perms);
    void removePermissions(FSPermission perms);

    std::string getPermissionString() const;
    std::string getModeString() const;

    static std::string getPermissionString(FSPermission perms);
    static std::string getModeString(Type type, FSPermission perms);
    static bool hasPermissions(FSPermission perms, FSPermission compare);

private:

    std::filesystem::directory_entry entry;

};


ARC_CREATE_BITMASK_ENUM(FSEntry::Type)