/* 
CGS is exclusive for program and it stands for Computer Graphics Scene

#model indicates that the next lines are related to a model
#light indicates that the next lines are related to a light
#camera indicates that the next lines are related to a camera

The first camera to be read will be set as the initial camera position

-----------
for a #model, the reader will gather information in this order:

Model directory
Animation style - 0 means no animation
X position
Y position
Z position
X rotation in degrees
Y rotation in degrees
Z rotation in degrees
X scale
Y scale
Z scale
-----------
for a #light, the reader will gather information in this order:

X position
Y position
Z position
-----------
for a #camera, the reader will gather information in this order:

X position
Y position
Z position
Yaw
Pitch
*/

#include <fstream>
#include <list>
#include <string>
#include <iostream>

enum AnimationEquation
{
	none, anim1, anim2, anim3
};

struct ModelInfo {
	float xpos; float ypos; float zpos; // Position
	float xrot; float yrot; float zrot; // Rotation
	float xsc; float ysc; float zsc; // Scale
	int animationID;
	string modelDirectory;
	AnimationEquation animation;

	ModelInfo()
	{
	}

	Model GetModel()
	{
		Model m(modelDirectory);
		return m;
	}

	AnimationEquation GetAnimation(int equationID)
	{
		switch (equationID)
		{
		case 0: return none;
		case 1: return anim1;
		case 2: return anim2;
		case 3: return anim3;
		default:
			return none;
		}
	}
};

struct LightInfo {
	float xpos, ypos, zpos;

	LightInfo() {}
};

struct CameraInfo {
	float xpos, ypos, zpos;
	float yaw, pitch;

	CameraInfo() {}
};

struct SceneInfo {
	list<ModelInfo> models;
	list<LightInfo> lights;
	list<CameraInfo> cameras;

	SceneInfo() {}
	void DebugSceneInfo()
	{
		cout << "Models: " << models.size() << endl;
		cout << "Lights: "  << lights.size() << endl;
		cout << "Cameras: " << cameras.size() << endl;
	}
};

SceneInfo ReadCGS(string directory)
{
	ifstream cgs;
	SceneInfo scene;
	list<string> info;

	cgs.open(directory);

	string word;

	while (cgs >> word)
	{
		info.push_back(word);
		//cout << word << endl;
	}

	cgs.close();

	while (!info.empty())
	{
		if (info.front() == "#model")
		{
			ModelInfo newModel;
			info.pop_front();
			newModel.modelDirectory = info.front(); info.pop_front();
			newModel.animationID = stof(info.front()); info.pop_front();
			newModel.xpos = stof(info.front()); info.pop_front();
			newModel.ypos = stof(info.front()); info.pop_front();
			newModel.zpos = stof(info.front()); info.pop_front();
			newModel.xrot = stof(info.front()); info.pop_front();
			newModel.yrot = stof(info.front()); info.pop_front();
			newModel.zrot = stof(info.front()); info.pop_front();
			newModel.xsc = stof(info.front()); info.pop_front();
			newModel.ysc = stof(info.front()); info.pop_front();
			newModel.zsc = stof(info.front()); info.pop_front();
			newModel.GetAnimation(newModel.animationID);
			scene.models.push_back(newModel);

		}

		if (info.front() == "#light")
		{
			LightInfo newLight;
			info.pop_front();
			newLight.xpos = stof(info.front()); info.pop_front();
			newLight.ypos = stof(info.front()); info.pop_front();
			newLight.zpos = stof(info.front()); info.pop_front();
			scene.lights.push_back(newLight);

		}

		if (info.front() == "#camera")
		{
			CameraInfo newCamera;
			info.pop_front();
			newCamera.xpos = stof(info.front()); info.pop_front();
			newCamera.ypos = stof(info.front()); info.pop_front();
			newCamera.zpos = stof(info.front()); info.pop_front();
			newCamera.yaw = stof(info.front()); info.pop_front();
			newCamera.pitch = stof(info.front()); info.pop_front();
			scene.cameras.push_back(newCamera);

		}

	}
	scene.DebugSceneInfo();
	return scene;
}
