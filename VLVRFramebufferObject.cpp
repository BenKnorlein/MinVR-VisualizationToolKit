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
///\file VLVRFramebufferObject.cpp
///\author Benjamin Knorlein
///\date 2/27/2019

#pragma once

#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

#include "VLVRFramebufferObject.h"

VLVRFramebufferObject::VLVRFramebufferObject(GLint fboID_read, GLint fboID_draw, vl::OpenGLContext* ctx, int w, int h, vl::EReadDrawBuffer draw_buffer, vl::EReadDrawBuffer read_buffer) :m_fboID_read{ fboID_read }, m_fboID_draw{ fboID_draw }, vl::Framebuffer(ctx, w, h, draw_buffer, read_buffer)
{

}

void VLVRFramebufferObject::bindFramebuffer(vl::EFramebufferBind target)
{
	if (GL_READ_FRAMEBUFFER == target)
	{
		vl::glBindFramebuffer(GL_READ_FRAMEBUFFER, m_fboID_read);
	}
	else if (GL_DRAW_FRAMEBUFFER == target)
	{
		vl::glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_fboID_draw);
	} else
	{
		vl::glBindFramebuffer(GL_READ_FRAMEBUFFER, m_fboID_read);
		vl::glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_fboID_draw);
	}
}
