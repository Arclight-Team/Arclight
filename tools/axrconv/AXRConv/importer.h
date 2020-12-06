#ifndef IMPORTER_H
#define IMPORTER_H

#include "types.h"
#include "amdmodel.h"

#include <QString>

struct ImportConfiguration;

struct aiNode;
struct aiMesh;
struct aiColor3D;

class Importer {

public:

	Importer();

	AMDModel import(const QString& path, const ImportConfiguration& config);

	QString getErrorString() const;
	bool validImport() const;

private:
	u32 processAMDNode(AMDModel& model, const aiNode* sceneNode, u32& nodeID, u32 parentID);
	void loadTexture(AMDTexture& texture, const QString& path);

	void setImportError(const QString& msg);
	AMDPrimitiveMode getPrimitiveMode(const aiMesh* mesh);
	AMDVector3 convertColor(const aiColor3D* color);

	void addAttribute(AMDMesh& mesh, const aiMesh* sceneMesh, AMDAttributeType type);

	bool imported;
	QString error;

};

#endif // IMPORTER_H
