#include "scene.h"



void Scene::loadScene(SceneID id) {

	if (loaded) {
		freeScene();
	}

	switch (id) {

		case SceneID::MarioSDS:
			{

				Loader::loadCubemap(skyboxTexture, {
					":/textures/cubemaps/dikhololo/px.png",
					":/textures/cubemaps/dikhololo/nx.png",
					":/textures/cubemaps/dikhololo/py.png",
					":/textures/cubemaps/dikhololo/ny.png",
					":/textures/cubemaps/dikhololo/pz.png",
					":/textures/cubemaps/dikhololo/nz.png"
				}, true);

				models.resize(5);
				Loader::loadModel(models[SceneModel::getModelID(SceneModel::MarioSDSID::Mario)], ":/models/mario/mario.fbx");
				Loader::loadModel(models[SceneModel::getModelID(SceneModel::MarioSDSID::Luigi)], ":/models/luigi/luigi.fbx");
				Loader::loadModel(models[SceneModel::getModelID(SceneModel::MarioSDSID::CastleGrounds)], ":/models/Level Models/Castle Grounds/grounds.fbx", true);
				Loader::loadModel(models[SceneModel::getModelID(SceneModel::MarioSDSID::Melascula)], ":/models/Melascula/Melascula.obj", false);
				Loader::loadModel(models[SceneModel::getModelID(SceneModel::MarioSDSID::Galand)], ":/models/galand_realistic/Galand.obj", false);

				skyboxTexture.bind();
				skyboxTexture.setMinFilter(GLE::TextureFilter::Trilinear);
				skyboxTexture.setMagFilter(GLE::TextureFilter::Bilinear);
				skyboxTexture.setWrapU(GLE::TextureWrap::Clamp);
				skyboxTexture.setWrapV(GLE::TextureWrap::Clamp);

				models[SceneModel::getModelID(SceneModel::MarioSDSID::Mario)].transform = Mat4f::fromTranslation(-20, 20, 0);
				models[SceneModel::getModelID(SceneModel::MarioSDSID::Luigi)].transform = Mat4f::fromTranslation(+20, 20, 0);
				models[SceneModel::getModelID(SceneModel::MarioSDSID::Melascula)].transform = Mat4f::fromScale(10, 10, 10).translate(0, 20, 0);
				models[SceneModel::getModelID(SceneModel::MarioSDSID::Galand)].transform = Mat4f::fromScale(1, 1, 1).translate(0, -50, -50);
				models[SceneModel::getModelID(SceneModel::MarioSDSID::Melascula)].root.children[4].visible = false;

				setTextureFilters(SceneModel::getModelID(SceneModel::MarioSDSID::Mario), GLE::TextureFilter::None, GLE::TextureFilter::None);
				setTextureFilters(SceneModel::getModelID(SceneModel::MarioSDSID::Luigi), GLE::TextureFilter::None, GLE::TextureFilter::None);
				setTextureFilters(SceneModel::getModelID(SceneModel::MarioSDSID::CastleGrounds), GLE::TextureFilter::None, GLE::TextureFilter::None);
				setTextureFilters(SceneModel::getModelID(SceneModel::MarioSDSID::Melascula), GLE::TextureFilter::None, GLE::TextureFilter::None);
				setTextureFilters(SceneModel::getModelID(SceneModel::MarioSDSID::Galand), GLE::TextureFilter::None, GLE::TextureFilter::None);
				setTextureWrap(SceneModel::getModelID(SceneModel::MarioSDSID::Mario), GLE::TextureWrap::Repeat, GLE::TextureWrap::Repeat);
				setTextureWrap(SceneModel::getModelID(SceneModel::MarioSDSID::Luigi), GLE::TextureWrap::Mirror, GLE::TextureWrap::Mirror);
				setTextureWrap(SceneModel::getModelID(SceneModel::MarioSDSID::CastleGrounds), GLE::TextureWrap::Mirror, GLE::TextureWrap::Mirror);
				setTextureWrap(SceneModel::getModelID(SceneModel::MarioSDSID::Melascula), GLE::TextureWrap::Repeat, GLE::TextureWrap::Repeat);
				setTextureWrap(SceneModel::getModelID(SceneModel::MarioSDSID::Galand), GLE::TextureWrap::Repeat, GLE::TextureWrap::Repeat);

				std::vector<Material>& m = models[SceneModel::getModelID(SceneModel::MarioSDSID::CastleGrounds)].materials;

				for (auto& [name, texture] : m[9].textures) {
					texture.bind();
					texture.setWrapU(GLE::TextureWrap::Repeat);
					texture.setWrapV(GLE::TextureWrap::Repeat);
				}

			}

			break;

		case SceneID::Forest:
			{

				Loader::loadCubemap(skyboxTexture, {
					":/textures/cubemaps/dikhololo/px.png",
					":/textures/cubemaps/dikhololo/nx.png",
					":/textures/cubemaps/dikhololo/py.png",
					":/textures/cubemaps/dikhololo/ny.png",
					":/textures/cubemaps/dikhololo/pz.png",
					":/textures/cubemaps/dikhololo/nz.png"
				}, true);

				models.resize(1);
				Loader::loadModel(models[SceneModel::getModelID(SceneModel::ForestID::ForestMain)], ":/models/trees/1/1.FBX");

			}

			break;

	}

	currentScene = id;
	loaded = true;

}



void Scene::freeScene() {

	if (!loaded) {
		return;
	}

	for (auto& m : models) {
		m.destroy();
	}

	skyboxTexture.destroy();
	models.clear();

	loaded = false;

}



void Scene::setTextureFilters(u32 modelID, GLE::TextureFilter min, GLE::TextureFilter mag) {

	for (Material& material : models[modelID].materials) {

		for (auto& [name, texture] : material.textures) {

			texture.bind();
			texture.setMinFilter(min);
			texture.setMagFilter(mag);

		}

	}

}



void Scene::setTextureWrap(u32 modelID, GLE::TextureWrap wrapU, GLE::TextureWrap wrapV) {

	for (Material& material : models[modelID].materials) {

		for (auto& [name, texture] : material.textures) {

			texture.bind();
			texture.setWrapU(wrapU);
			texture.setWrapV(wrapV);

		}

	}

}



SceneID Scene::getCurrentSceneID() {
	return currentScene;
}



std::vector<Model>& Scene::getModels() {
	return models;
}



GLE::CubemapTexture& Scene::getSkyboxTexture() {
	return skyboxTexture;
}