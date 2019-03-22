#include "VolumeVisualizationApp.h"

#include <cmath>
#include <cctype>

#include <vlGraphics/Rendering.hpp>
#include <vlVolume/VolumeUtils.hpp>
#include <vlGraphics/GeometryPrimitives.hpp>
#include <vlCore/ResourceDatabase.hpp>
#include <vlCore/Time.hpp>

using vl::Rendering;
using vl::Effect;
using vl::GLSLVertexShader;
using vl::GLSLFragmentShader;
using vl::Texture;
using vl::vec3;
using vl::fvec3;
using vl::fvec4;
using vl::Time;

VolumeVisualizationApp::VolumeVisualizationApp(int argc, char** argv) : VRApp(argc, argv), SAMPLE_STEP{ 256.0f }
, MODE{ RaycastBrightnessControl_Mode }, DYNAMIC_LIGHTS{ true }, COLORED_LIGHTS{ false }, PRECOMPUTE_GRADIENT{ false }
{
	
}


VolumeVisualizationApp::~VolumeVisualizationApp()
{

}


void VolumeVisualizationApp::onAnalogChange(const VRAnalogEvent &state) {
    // This routine is called for all Analog_Change events.  Check event->getName()
    // to see exactly which analog input has been changed, and then access the
    // new value with event->getValue().
    
}


void VolumeVisualizationApp::onButtonDown(const VRButtonEvent &event) {
    // This routine is called for all Button_Down events.  Check event->getName()
    // to see exactly which button has been pressed down.

	if (event.getName() == "HTC_Controller_Right_Axis0Button_Down")
	{
		int mode = MODE;
		mode++;
		if (mode > RaycastColorControl_Mode)
		{
			MODE = Isosurface_Mode;
		} 
		else
		{
			MODE = static_cast<RaycastMode>(mode);
		}

		setupScene();
	}
	else if (event.getName() == "HTC_Controller_Right_Axis1Button_Down")
	{
		float val_threshold = 0.0f;
		mValThreshold->getUniform(&val_threshold);
		val_threshold += 0.05f;
		if(val_threshold > 1.0) val_threshold = 0.0;
		mValThreshold->setUniformF(val_threshold);
	}
}


void VolumeVisualizationApp::onButtonUp(const VRButtonEvent &event) {
    // This routine is called for all Button_Up events.  Check event->getName()
    // to see exactly which button has been released.

}


void VolumeVisualizationApp::onTrackerMove(const VRTrackerEvent &event) {
    // This routine is called for all Tracker_Move events.  Check event->getName()
    // to see exactly which tracker has moved, and then access the tracker's new
    // 4x4 transformation matrix with event->getTransform().
}


    
void VolumeVisualizationApp::onRenderGraphicsContext(const VRGraphicsState &renderState) {
    // This routine is called once per graphics context at the start of the
    // rendering process.  So, this is the place to initialize textures,
    // load models, or do other operations that you only want to do once per
    // frame when in stereo mode.
	pre_onRenderGraphicsContext(renderState);
	
	if (renderState.isInitialRenderCall())
	{
		mLight0 = new Light;
		mLight1 = new Light;
		mLight2 = new Light;

		mLight0Tr = new Transform;
		mLight1Tr = new Transform;
		mLight2Tr = new Transform;

		rendering()->as<Rendering>()->transform()->addChild(mLight0Tr.get());
		rendering()->as<Rendering>()->transform()->addChild(mLight1Tr.get());
		rendering()->as<Rendering>()->transform()->addChild(mLight2Tr.get());

		// volume transform
		mVolumeTr = new Transform;

		// val_threshold: manipulated via mouse wheel
		// - In Isosurface_Mode controls the iso-value of the isosurface
		// - In Isosurface_Transp_Mode controls the iso-value of the isosurface
		// - In MIP_Mode all the volume values less than this are discarded
		// - In RaycastBrightnessControl_Mode controls the brightness of the voxels
		// - In RaycastDensityControl_Mode controls the density of the voxels
		// - In RaycastColorControl_Mode controls the color-bias of the voxels
		mValThreshold = new Uniform("val_threshold");
		mValThreshold->setUniformF(0.5f);

		// default volume image

		//mVolumeImage = vl::loadImage("/volume/VLTest.dat");
		//loadFolder("C:\\Workspace\\projects\\MinVR_Volume\\build\\Release\\test\\ch1");
		loadFolder("D:\\Test_images_for_Ben_Knorlein\\GPA_test_Images_E5\\out\\tmp");
		setupScene();
	}

	updateScene();
}


void VolumeVisualizationApp::onRenderGraphicsScene(const VRGraphicsState &renderState) {
    // This routine is called once per eye.  This is the place to actually
    // draw the scene.
	// update the projection matrix
	pre_onRenderGraphics(renderState);

	float tmp[16];
	memcpy(tmp, renderState.getProjectionMatrix(), sizeof(tmp));
	vl::fmat4 proj_matr = vl::fmat4(tmp);
	rendering()->camera()->setProjectionMatrix(proj_matr, vl::PMT_PerspectiveProjectionFrustum);

	memcpy(tmp, renderState.getViewMatrix(), sizeof(tmp));
	vl::fmat4 view_matr = vl::fmat4(tmp);
	rendering()->camera()->setViewMatrix(view_matr);

	// execute rendering
	rendering()->render();
}

void VolumeVisualizationApp::setupScene()
{
	mRaycastVolume = new vl::RaycastVolume;
	mVolumeAct = new Actor;

	// scrap previous scene
	sceneManager()->tree()->eraseAllChildren();
	sceneManager()->tree()->actors()->clear();
	mLight0->bindTransform(NULL);
	mLight1->bindTransform(NULL);
	mLight2->bindTransform(NULL);

	ref<Effect> volume_fx = new Effect;
	// we don't necessarily need this:
	// volume_fx->shader()->enable( EN_BLEND );
	volume_fx->shader()->enable(vl::EN_CULL_FACE);
	volume_fx->shader()->enable(vl::EN_DEPTH_TEST);

	// NOTE
	// in these cases we render the back faces and raycast in back to front direction
	// in the other cases we render the front faces and raycast in front to back direction
	if (MODE == RaycastBrightnessControl_Mode || MODE == RaycastDensityControl_Mode || MODE == RaycastColorControl_Mode)
	{
		volume_fx->shader()->enable(vl::EN_CULL_FACE);
		volume_fx->shader()->gocCullFace()->set(vl::PF_FRONT);
	}

	mRaycastVolume->lights().push_back(mLight0);

	// light bulbs
	if (DYNAMIC_LIGHTS)
	{
		// you can color the lights!
		if (COLORED_LIGHTS)
		{
			mLight0->setAmbient(fvec4(0.1f, 0.1f, 0.1f, 1.0f));
			mLight1->setAmbient(fvec4(0.1f, 0.1f, 0.1f, 1.0f));
			mLight2->setAmbient(fvec4(0.1f, 0.1f, 0.1f, 1.0f));
			mLight0->setDiffuse(vl::gold);
			mLight1->setDiffuse(vl::green);
			mLight2->setDiffuse(vl::royalblue);
		}

		// add the other two lights
		mRaycastVolume->lights().push_back(mLight1);
		mRaycastVolume->lights().push_back(mLight2);

		// animate the three lights
		mLight0->bindTransform(mLight0Tr.get());
		mLight1->bindTransform(mLight1Tr.get());
		mLight2->bindTransform(mLight2Tr.get());

		// add also a light bulb actor
		ref<Effect> fx_bulb = new Effect;
		fx_bulb->shader()->enable(vl::EN_DEPTH_TEST);
		ref<vl::Geometry> light_bulb = vl::makeIcosphere(vec3(0, 0, 0), 1, 1);
		sceneManager()->tree()->addActor(light_bulb.get(), fx_bulb.get(), mLight0Tr.get());
		sceneManager()->tree()->addActor(light_bulb.get(), fx_bulb.get(), mLight1Tr.get());
		sceneManager()->tree()->addActor(light_bulb.get(), fx_bulb.get(), mLight2Tr.get());
	}

	// the GLSL program that performs the actual raycasting
	mGLSL = volume_fx->shader()->gocGLSLProgram();
	mGLSL->gocUniform("sample_step")->setUniformF(1.0f / SAMPLE_STEP);

	// attach vertex shader (common to all the raycasting techniques)
	mGLSL->attachShader(new GLSLVertexShader("/glsl/volume_luminance_light.vs"));

	// attach fragment shader implementing the specific raycasting tecnique
	if (MODE == Isosurface_Mode)
		mGLSL->attachShader(new GLSLFragmentShader("/glsl/volume_raycast_isosurface.fs"));
	else
		if (MODE == Isosurface_Transp_Mode)
			mGLSL->attachShader(new GLSLFragmentShader("/glsl/volume_raycast_isosurface_transp.fs"));
		else
			if (MODE == MIP_Mode)
				mGLSL->attachShader(new GLSLFragmentShader("/glsl/volume_raycast_mip.fs"));
			else
				if (MODE == RaycastBrightnessControl_Mode)
					mGLSL->attachShader(new GLSLFragmentShader("glsl/volume_raycast01.fs"));
				else
					if (MODE == RaycastDensityControl_Mode)
						mGLSL->attachShader(new GLSLFragmentShader("glsl/volume_raycast02.fs"));
					else
						if (MODE == RaycastColorControl_Mode)
							mGLSL->attachShader(new GLSLFragmentShader("glsl/volume_raycast03.fs"));

	// volume actor
	mVolumeAct->setEffect(volume_fx.get());
	mVolumeAct->setTransform(mVolumeTr.get());
	sceneManager()->tree()->addActor(mVolumeAct.get());
	// bind val_threshold uniform to the volume actor
	mVolumeAct->setUniform(mValThreshold.get());

	// RaycastVolume will generate the actual actor's geometry upon setBox() invocation.
	// The geometry generated is actually a simple box with 3D texture coordinates.
	float size = 0.5f;
	mRaycastVolume->bindActor(mVolumeAct.get());
	vl::AABB volume_box(vec3(-size, -size, -size), vec3(size, size, size));
	mRaycastVolume->setBox(volume_box);

	// let's visualize the volume!
	setupVolume();
}

/* visualize the given volume */
void VolumeVisualizationApp::setupVolume()
{
	Effect* volume_fx = mVolumeAct->effect();

	volume_fx->shader()->enable(vl::EN_BLEND);

	// for semplicity we don't distinguish between different image formats, i.e. IF_LUMINANCE, IF_RGBA etc.

	ref<Image> gradient;
	if (PRECOMPUTE_GRADIENT)
	{
		// note that this can take a while...
		gradient = vl::genGradientNormals(mVolumeImage.get());
	}

	// volume image textue must be on sampler #0
	vl::ref< vl::Texture > vol_tex = new vl::Texture(mVolumeImage.get(), vl::TF_RED, false, false);
	volume_fx->shader()->gocTextureSampler(0)->setTexture(vol_tex.get());
	vol_tex->getTexParameter()->setMagFilter(vl::TPF_LINEAR);
	vol_tex->getTexParameter()->setMinFilter(vl::TPF_LINEAR);
	vol_tex->getTexParameter()->setWrap(vl::TPW_CLAMP_TO_EDGE);
	volume_fx->shader()->gocUniform("volume_texunit")->setUniformI(0);
	mRaycastVolume->generateTextureCoordinates(vl::ivec3(mVolumeImage->width(), mVolumeImage->height(), mVolumeImage->depth()));

	// generate a simple colored transfer function
	ref<Image> trfunc;
	if (COLORED_LIGHTS && DYNAMIC_LIGHTS) {
		trfunc = vl::makeColorSpectrum(128, vl::white, vl::white); // let the lights color the volume
	}
	else {
		trfunc = vl::makeColorSpectrum(128, vl::blue, vl::royalblue, vl::green, vl::yellow, vl::crimson);
	}

	// installs the transfer function as texture #1
	vl::ref< vl::Texture > trf_tex = new Texture(trfunc.get(), vl::TF_RGBA, false, false);
	trf_tex->getTexParameter()->setMagFilter(vl::TPF_LINEAR);
	trf_tex->getTexParameter()->setMinFilter(vl::TPF_LINEAR);
	trf_tex->getTexParameter()->setWrap(vl::TPW_CLAMP_TO_EDGE);
	volume_fx->shader()->gocTextureSampler(1)->setTexture(trf_tex.get());
	volume_fx->shader()->gocUniform("trfunc_texunit")->setUniformI(1);

	// gradient computation, only use for isosurface methods
	if (MODE == Isosurface_Mode || MODE == Isosurface_Transp_Mode)
	{
		volume_fx->shader()->gocUniform("precomputed_gradient")->setUniformI(PRECOMPUTE_GRADIENT ? 1 : 0);
		if (PRECOMPUTE_GRADIENT)
		{
			volume_fx->shader()->gocUniform("gradient_texunit")->setUniformI(2);
			vl::ref< vl::Texture > tex = new Texture(gradient.get(), vl::TF_RGBA, false, false);
			tex->getTexParameter()->setMagFilter(vl::TPF_LINEAR);
			tex->getTexParameter()->setMinFilter(vl::TPF_LINEAR);
			tex->getTexParameter()->setWrap(vl::TPW_CLAMP_TO_EDGE);
			volume_fx->shader()->gocTextureSampler(2)->setTexture(tex.get());
		}
	}

	// refresh window
	openglContext()->update();
}

/* animate the lights */
void VolumeVisualizationApp::updateScene()
{
	if (DYNAMIC_LIGHTS)
	{
		mat4 mat;
		// light 0 transform.
		mat = mat4::getRotation(Time::currentTime() * 43, 0, 1, 0) * mat4::getTranslation(20, 20, 20);
		mLight0Tr->setLocalMatrix(mat);
		// light 1 transform.
		mat = vl::mat4::getRotation(Time::currentTime() * 47, 0, 1, 0) * mat4::getTranslation(-20, 0, 0);
		mLight1Tr->setLocalMatrix(mat);
		// light 2 transform.
		mat = mat4::getRotation(Time::currentTime() * 47, 0, 1, 0) * mat4::getTranslation(+20, 0, 0);
		mLight2Tr->setLocalMatrix(mat);
	}
}

#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#include <windows.h>
#define OS_SLASH "\\"
#include "dirent.h"
#else
#define OS_SLASH "//"
#include <dirent.h>
#endif

bool ends_with_string(std::string const& str, std::string const& what) {
	return what.size() <= str.size()
		&& str.find(what, str.size() - what.size()) != str.npos;
}

bool contains_string(std::string const& str, std::string const& what) {
	return str.find(what) != std::string::npos;
}

void VolumeVisualizationApp::loadFolder(std::string foldername)
{
	std::vector <std::string> out_vector;
	DIR *dir;
	struct dirent *ent;
	if ((dir = opendir(foldername.c_str())) != NULL) {
		/* print all the files and directories within directory */
		while ((ent = readdir(dir)) != NULL) {
			if (ends_with_string(ent->d_name, "tiff"))
			{
				out_vector.push_back(foldername + OS_SLASH + ent->d_name);
			}
		}
		closedir(dir);
	}
	else {
		std::cerr << "Could not open folder" << std::endl;
	}
	std::sort(out_vector.begin(), out_vector.end());

	std::vector< ref<Image> > images;
	for (unsigned int i = 0; i<out_vector.size(); ++i)
	{
		std::cerr << out_vector[i] << std::endl;
		images.push_back(vl::loadImage(out_vector[i]));
	}
	// assemble the volume
	mVolumeImage = assemble3DImage(images);
}
