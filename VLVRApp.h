//  ----------------------------------
//  XMALab -- Copyright © 2015, Brown University, Providence, RI.
//  
//  All Rights Reserved
//   
//  Use of the XMALab software is provided under the terms of the GNU General Public License version 3 
//  as published by the Free Software Foundation at http://www.gnu.org/licenses/gpl-3.0.html, provided 
//  that this copyright notice appear in all copies and that the name of Brown University not be used in 
//  advertising or publicity pertaining to the use or distribution of the software without specific written 
//  prior permission from Brown University.
//  
//  See license.txt for further information.
//  
//  BROWN UNIVERSITY DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE WHICH IS 
//  PROVIDED “AS IS”, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS 
//  FOR ANY PARTICULAR PURPOSE.  IN NO EVENT SHALL BROWN UNIVERSITY BE LIABLE FOR ANY 
//  SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR FOR ANY DAMAGES WHATSOEVER RESULTING 
//  FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR 
//  OTHER TORTIOUS ACTION, OR ANY OTHER LEGAL THEORY, ARISING OUT OF OR IN CONNECTION 
//  WITH THE USE OR PERFORMANCE OF THIS SOFTWARE. 
//  ----------------------------------
//  
///\file VLVRApp.h
///\author Benjamin Knorlein
///\date 2/27/2019

#pragma once

#ifndef VLVRAPP_H
#define VLVRAPP_H
#include <vlGraphics/Framebuffer.hpp>
#include <vlGraphics/Rendering.hpp>
#include <vlGraphics/SceneManagerActorTree.hpp>
#include <vlGraphics/OpenGLContext.hpp>

#include <api/VRGraphicsState.h>

using vl::ref;
using vl::Framebuffer;

class VLVRApp : vl::OpenGLContext
{
public:
	VLVRApp(bool log_info = true, bool logOpenGLContext = false, bool checkOpenGLStates = false);
	~VLVRApp();

	void makeCurrent(){}
	void swapBuffers(){}
	void update(){}

	const vl::Rendering* rendering() const { return mRendering.get(); }
	vl::Rendering* rendering() { return mRendering.get(); }

	const vl::SceneManagerActorTree* sceneManager() const { return mSceneManager.get(); }
	vl::SceneManagerActorTree* sceneManager() { return mSceneManager.get(); }

	const vl::OpenGLContext* openglContext() const { return this; }
	vl::OpenGLContext* openglContext() { return this; }

protected:
	void pre_onRenderGraphicsContext(const MinVR::VRGraphicsState& renderState);
	void pre_onRenderGraphics(const MinVR::VRGraphicsState& renderState);

private:
	
	ref<vl::Rendering> mRendering;
	ref<vl::SceneManagerActorTree> mSceneManager;

	std::vector<ref<Framebuffer> > m_framebuffer;
	unsigned int m_renderCount;

	bool m_log_info;
	bool m_logOpenGLContext;
	bool m_checkOpenGLStates;
};

#endif // VLVRAPP_H
