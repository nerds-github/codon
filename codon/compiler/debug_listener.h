#pragma once

#include <memory>
#include <vector>

#include "codon/sir/llvm/llvm.h"

namespace codon {

class DebugListener : public llvm::JITEventListener {
public:
  class ObjectInfo {
  private:
    ObjectKey key;
    std::unique_ptr<llvm::object::ObjectFile> object;
    std::unique_ptr<llvm::MemoryBuffer> buffer;
    uintptr_t start;
    uintptr_t stop;

  public:
    ObjectInfo(ObjectKey key, std::unique_ptr<llvm::object::ObjectFile> object,
               std::unique_ptr<llvm::MemoryBuffer> buffer, uintptr_t start,
               uintptr_t stop)
        : key(key), object(std::move(object)), buffer(std::move(buffer)), start(start),
          stop(stop) {}

    ObjectKey getKey() const { return key; }
    const llvm::object::ObjectFile &getObject() const { return *object; }
    uintptr_t getStart() const { return start; }
    uintptr_t getStop() const { return stop; }
    bool contains(uintptr_t pc) const { return start <= pc && pc < stop; }
  };

private:
  llvm::symbolize::LLVMSymbolizer sym;
  std::vector<ObjectInfo> objects;

  void notifyObjectLoaded(ObjectKey key, const llvm::object::ObjectFile &obj,
                          const llvm::RuntimeDyld::LoadedObjectInfo &L) override;
  void notifyFreeingObject(ObjectKey key) override;

public:
  DebugListener() : llvm::JITEventListener(), sym(), objects() {}

  llvm::Expected<llvm::DILineInfo> symbolize(uintptr_t pc);
  llvm::Expected<std::string> getPrettyBacktrace(uintptr_t pc);
  std::string getPrettyBacktrace(const std::vector<uintptr_t> &backtrace);
};

} // namespace codon
