
///\file VLVRFramebufferObject.h
///\author Benjamin Knorlein
///\date 2/27/2019

#pragma once

#ifndef VLVRFRAMEBUFFER_H
#define VLVRFRAMEBUFFER_H

#include <vlGraphics/Framebuffer.hpp>

	class VLVRFramebufferObject: public vl::Framebuffer
	{
	public:
		VLVRFramebufferObject(GLint fboID_read, GLint fboID_draw, vl::OpenGLContext* ctx, int w, int h, vl::EReadDrawBuffer draw_buffer, vl::EReadDrawBuffer read_buffer);
		~VLVRFramebufferObject(){};

		GLuint handle() const override { return m_fboID_draw; }

		void bindFramebuffer(vl::EFramebufferBind target = vl::FBB_FRAMEBUFFER) override;

	private:
		GLint m_fboID_read;
		GLint m_fboID_draw;
	};

#endif // VLVRFRAMEBUFFER_H
