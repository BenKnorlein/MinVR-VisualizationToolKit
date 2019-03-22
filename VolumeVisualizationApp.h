#ifndef VOLUMEVISUALIZATIONAPP_H
#define VOLUMEVISUALIZATIONAPP_H


#include <api/MinVR.h>

#include <vlGraphics/Light.hpp>
#include <vlGraphics/FontManager.hpp>
#include <vlGraphics/GLSL.hpp>
#include <vlVolume/RaycastVolume.hpp>
#include "VLVRApp.h"

using namespace MinVR;

using vl::mat4;
using vl::Transform;
using vl::Uniform;
using vl::Text;
using vl::Light;
using vl::GLSLProgram;
using vl::Actor;
using vl::RaycastVolume;
using vl::Image;


class VolumeVisualizationApp : public VRApp, VLVRApp {
	/* ----- raycast volume rendering options ----- */

	/* The sample step used to render the volume, the smaller the number
	the  better ( and slower ) the rendering will be. */
	float SAMPLE_STEP;

	/* volume visualization mode */
	enum RaycastMode {
		Isosurface_Mode,
		Isosurface_Transp_Mode,
		MIP_Mode,
		RaycastBrightnessControl_Mode,
		RaycastDensityControl_Mode,
		RaycastColorControl_Mode
	} MODE;

	/* If enabled, renders the volume using 3 animated lights. */
	bool DYNAMIC_LIGHTS;

	/* If enabled 3 colored lights are used to render the volume. */
	bool COLORED_LIGHTS;

	/* Use a separate 3d texture with a precomputed gradient to speedup the fragment shader.
	Requires more memory ( for the gradient texture ) but can speedup the rendering. */
	bool PRECOMPUTE_GRADIENT;

public:

	/** The constructor passes argc, argv, and a MinVR config file on to VRApp.
	 */
	VolumeVisualizationApp(int argc, char** argv);
	virtual ~VolumeVisualizationApp();

	/** USER INTERFACE CALLBACKS **/

	virtual void onAnalogChange(const VRAnalogEvent &state);

	virtual void onButtonDown(const VRButtonEvent &state);

	virtual void onButtonUp(const VRButtonEvent &state);

	virtual void onTrackerMove(const VRTrackerEvent &state);


	/** RENDERING CALLBACKS **/

	virtual void onRenderGraphicsScene(const VRGraphicsState& state);

	virtual void onRenderGraphicsContext(const VRGraphicsState& state);

private:

	void setupScene();
	void setupVolume();
	/* animate the lights */
	void updateScene();
	void loadFolder(std::string folder);

	ref<Transform> mVolumeTr;
	ref<Transform> mLight0Tr;
	ref<Transform> mLight1Tr;
	ref<Transform> mLight2Tr;
	ref<Uniform> mValThreshold;
	ref<Light> mLight0;
	ref<Light> mLight1;
	ref<Light> mLight2;
	ref<GLSLProgram> mGLSL;
	ref<Actor> mVolumeAct;
	ref<RaycastVolume> mRaycastVolume;
	ref<Image> mVolumeImage;
};

#endif