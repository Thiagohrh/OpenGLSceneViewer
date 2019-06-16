#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <list>
#include "readerCGS.h"

using namespace std;

class SceneFromFile
{
public:
	void AddModelToList(ModelInfo model) { models.push_back(model); }
	void AddLightToList(LightInfo light) { lights.push_back(light); }
	void AddCameraToList(CameraInfo camera) { cameras.push_back(camera); }
	void SetInitialCamera(CameraInfo camera) { initialCamera = camera; }
	void LoadFile(string directory);

private:
	list<ModelInfo> models;
	list<LightInfo> lights;
	list<CameraInfo> cameras;
	CameraInfo initialCamera;
};

void SceneFromFile::LoadFile(string directory)
{
	SceneInfo scene = ReadCGS(directory);
	bool first_camera = true;
	for each (ModelInfo mi in scene.models)
	{
		AddModelToList(mi);
	}
	for each (LightInfo li in scene.lights)
	{
		AddLightToList(li);
	}
	for each (CameraInfo ci in scene.cameras)
	{
		if (first_camera)
		{
			SetInitialCamera(ci);
			first_camera = false;
		}
		AddCameraToList(ci);
	}

}
