#pragma once

#include "loader.h"


enum class SceneID {
	MarioSDS,
	City
};

namespace SceneModel {

	enum class MarioSDSID : int {
		Mario,
		Luigi,
		CastleGrounds,
		Melascula,
		Galand
	};

	template<typename T>
	constexpr u32 getModelID(T id) {
		return static_cast<u32>(id);
	}

}



class Scene {

public:

	inline Scene() : currentScene(SceneID::MarioSDS), loaded(false) {}
	inline ~Scene() { freeScene(); }

	void loadScene(SceneID id);
	void freeScene();

	void setTextureFilters(u32 modelID, GLE::TextureFilter min, GLE::TextureFilter mag);
	void setTextureWrap(u32 modelID, GLE::TextureWrap wrapU, GLE::TextureWrap wrapV);

	SceneID getCurrentSceneID();
	std::vector<Model>& getModels();
	GLE::CubemapTexture& getSkyboxTexture();

private:

	SceneID currentScene;
	std::vector<Model> models;
	bool loaded;

	GLE::CubemapTexture skyboxTexture;

};