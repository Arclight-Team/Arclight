#ifndef IMPORTER_H
#define IMPORTER_H

#include "types.h"
#include "amdmodel.h"

#include <QString>

struct ImportConfiguration;

struct aiNode;
struct aiMesh;

class Importer {

public:

    Importer();

    AMDModel import(const QString& path, const ImportConfiguration& config);

    QString getErrorString() const;
    bool validImport() const;

private:
    u32 processAMDNode(AMDModel& model, const aiNode* sceneNode, u32& nodeID, u32 parentID);
    void setImportError(const QString& msg);
    AMDPrimitiveMode getPrimitiveMode(const aiMesh* mesh);

    template<class T>
    inline void addAttributeData(std::vector<u8>& data, u32 offset, T t){
        std::memcpy(&data[offset], &t, sizeof(t));
    }

    bool imported;
    QString error;

};

#endif // IMPORTER_H
