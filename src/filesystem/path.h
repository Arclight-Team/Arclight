#pragma once

#include <filesystem>
#include <unordered_map>


class Path {

public:

    constexpr static auto separator = std::filesystem::path::preferred_separator;

    Path();
    Path(const std::string& path);
    explicit Path(const std::filesystem::path& path);

    Path& set(Path&& path);
    Path& set(const Path& path);

    Path& append(const std::string& str);
    Path& concat(const std::string& str);

    Path& setStem(const std::string& stem);
    Path& setExtension(const std::string& ext);
    Path& setFilename(const std::string& name);

    std::string getPath() const;
    std::string getPreferredPath() const;

    std::string getStem() const;
    std::string getExtension() const;
    std::string getFilename() const;

    bool hasStem() const;
    bool hasExtension() const;
    bool hasFilename() const;

    Path& toAbsolute();
    Path& toCanonical();
    Path& toRelative();
    
    std::string relativeAgainst(const Path& base) const;

    bool isAbsolute() const;
    bool isRelative() const;

    void clear();
    bool isEmpty() const;

    bool exists() const;
    bool equivalent(const Path& other) const;

    Path root() const;
    Path parent() const;

    operator std::string() const;

    static Path getCurrentWorkingDirectory();
    static void setCurrentWorkingDirectory(const Path& path);

    static Path getApplicationDirectory();

    static std::string convertAnnotatedPath(const std::string& annotatedPath);
    
    static std::string getAnnotatedPathPrefix(char annotation);
    static void setAnnotatedPathPrefix(char annotation, const std::string& prefix);

private:

    std::filesystem::path path;
    static inline std::unordered_map<char, std::string> annotatedPrefixes;

};