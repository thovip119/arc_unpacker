#ifndef OUTPUT_FILES_H
#define OUTPUT_FILES_H
#include <boost/filesystem/path.hpp>
#include <memory>
#include <string>
#include <vector>
#include "file.h"

class FileSaver
{
public:
    virtual void save(std::shared_ptr<File> file) const = 0;
};

class FileSaverHdd : public FileSaver
{
public:
    FileSaverHdd(boost::filesystem::path output_dir);
    ~FileSaverHdd();

    virtual void save(std::shared_ptr<File> file) const override;
private:
    struct Internals;
    std::unique_ptr<Internals> internals;
};

typedef std::function<void(std::shared_ptr<File>)> FileSaveCallback;

class FileSaverCallback : public FileSaver
{
public:
    FileSaverCallback(FileSaveCallback callback);
    ~FileSaverCallback();

    virtual void save(std::shared_ptr<File> file) const override;
private:
    struct Internals;
    std::unique_ptr<Internals> internals;
};

#endif
