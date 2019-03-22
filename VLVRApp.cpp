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
///\file VLVRApp.cpp
///\author Benjamin Knorlein
///\date 2/27/2019

#pragma once

#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

#include "VLVRApp.h"
#include <vlCore/VisualizationLibrary.hpp>
#include <vlCore/GlobalSettings.hpp>
#include <vlCore/Time.hpp>
#include "VLVRFramebufferObject.h"
#include <config/VRDataIndex.h>


using vl::Time;

VLVRApp::VLVRApp(bool log_info, bool logOpenGLContext, bool checkOpenGLStates) :m_log_info{ log_info }, m_logOpenGLContext{ logOpenGLContext }, m_checkOpenGLStates{ checkOpenGLStates }
{
	vl::VisualizationLibrary::init(m_log_info);
}

VLVRApp::~VLVRApp()
{
	vl::VisualizationLibrary::shutdown();
}

void VLVRApp::pre_onRenderGraphicsContext(const MinVR::VRGraphicsState& renderState)
{
	if (renderState.isInitialRenderCall())
	{
		//We first have to initialize vl and the vl variables.
		vl::initializeOpenGL();
		this->initGLContext(m_logOpenGLContext);
		vl::globalSettings()->setCheckOpenGLStates(m_checkOpenGLStates);

		mRendering = new vl::Rendering;
		mSceneManager = new vl::SceneManagerActorTree;
		mRendering->sceneManagers()->push_back(mSceneManager.get());
		rendering()->renderer()->setFramebuffer(openglContext()->framebuffer());

		//The call to onRenderGraphics always comes from the window so we set the viewport
		GLint viewport[4];
		glGetIntegerv(GL_VIEWPORT, viewport);
		rendering()->camera()->viewport()->set(viewport[0], viewport[1], viewport[2], viewport[3]);
	}

	// set frame time (used for animation updates etc.)
	//TODO : should be syncronized time.
	rendering()->setFrameClock(vl::Time::currentTime());

	m_renderCount = 0;

}

void VLVRApp::pre_onRenderGraphics(const MinVR::VRGraphicsState& renderState)
{
	if (renderState.isInitialRenderCall())
	{
		//For each rendering we have to set the framebuffer to which vl should render
		GLint drawFboId = 0, readFboId = 0;
		glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, &drawFboId);
		glGetIntegerv(GL_READ_FRAMEBUFFER_BINDING, &readFboId);

		if (drawFboId != 0 || readFboId != 0)
		{
			//If a framebufferobject is used we have to set our own Framebuffer
			GLint viewport[4];
			glGetIntegerv(GL_VIEWPORT, viewport);
			rendering()->camera()->viewport()->set(viewport[0], viewport[1], viewport[2], viewport[3]);

			ref<Framebuffer> fb = new VLVRFramebufferObject(readFboId, drawFboId, this, viewport[2], viewport[3], vl::RDB_COLOR_ATTACHMENT0, vl::RDB_COLOR_ATTACHMENT0);
			m_framebuffer.push_back(fb);
		}
		else
		{
			//Otherwise it is rendered to the normal Framebuffer, but we have to decide if the left or right framebuffer is used.
			std::string eye = renderState.index().getValue("Eye");
			if (eye == "Right")
			{
				m_framebuffer.push_back(this->rightFramebuffer());
			}
			else
			{
				m_framebuffer.push_back(this->leftFramebuffer());
			}
		}
	}

	//For each rendering we then select the appropriate framebuffer
	rendering()->renderer()->setFramebuffer(m_framebuffer[m_renderCount].get());
	m_renderCount++;
}
