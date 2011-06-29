/*==================================================================================
    Copyright (c) 2008, DAVA Consulting, LLC
    All rights reserved.

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
    notice, this list of conditions and the following disclaimer in the
    documentation and/or other materials provided with the distribution.
    * Neither the name of the DAVA Consulting, LLC nor the
    names of its contributors may be used to endorse or promote products
    derived from this software without specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE DAVA CONSULTING, LLC AND CONTRIBUTORS "AS IS" AND
    ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
    WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
    DISCLAIMED. IN NO EVENT SHALL DAVA CONSULTING, LLC BE LIABLE FOR ANY
    DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
    (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
    LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
    ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
    (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
    SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

    Revision History:
        * Created by Vitaliy Borodovsky 
=====================================================================================*/
#ifndef __DAVAENGINE_VERTEXBUFFER_H__
#define __DAVAENGINE_VERTEXBUFFER_H__

#include "Base/BaseObject.h"
#include "Render/RenderResource.h"

namespace DAVA
{

enum eBufferType
{
	EBT_STATIC = 0x00,
	EBT_DYNAMIC = 0x01,
};

// TODO: we have same structs & functions in PolygonGroup -- we should find a right place for them
enum eVertexFormat
{
	EVF_COORD			= 1,
	EVF_NORMAL			= 2,
	EVF_COLOR			= 4,
	EVF_TEXCOORD0		= 8,
	EVF_TEXCOORD1		= 16,
	EVF_TEXCOORD2		= 32,
	EVF_TEXCOORD3		= 64,
	EVF_TANGENT			= 128,
	EVF_BINORMAL		= 256,
	EVF_JOINTWEIGHT		= 512,
	EVF_LOWER_BIT		= EVF_COORD,
	EVF_HIGHER_BIT		= EVF_JOINTWEIGHT, 
	EVF_NEXT_AFTER_HIGHER_BIT
						= (EVF_HIGHER_BIT << 1),
	EVF_FORCE_DWORD     = 0x7fffffff,
};

inline int32 GetVertexSize(int32 flags)
{
	int32 size = 0;
	if (flags & EVF_COORD) size += 3 * sizeof(float32);
	if (flags & EVF_NORMAL) size += 3 * sizeof(float32);
	if (flags & EVF_COLOR) size += 4;
	if (flags & EVF_TEXCOORD0) size += 2 * sizeof(float32);
	if (flags & EVF_TEXCOORD1) size += 2 * sizeof(float32);
	if (flags & EVF_TEXCOORD2) size += 2 * sizeof(float32);
	if (flags & EVF_TEXCOORD3) size += 2 * sizeof(float32);
	if (flags & EVF_TANGENT) size += 3 * sizeof(float32);
	if (flags & EVF_BINORMAL) size += 3 * sizeof(float32);

	if (flags & EVF_JOINTWEIGHT) size += 2 * sizeof(float32); // 4 * 3 + 4 * 3= 12 + 12 
	return size;
}

//! Interface to work with VertexBuffers
class VertexBuffer : public RenderResource
{
public:
	VertexBuffer() {};
	virtual ~VertexBuffer() {};


	virtual void			* Lock(const int32 vertexCount, int32 & startVertex) = 0;
	virtual void			Unlock() = 0;
	virtual int32			GetFormat() = 0;
	virtual eBufferType		GetType() = 0;
	virtual void			Flush() = 0;
};

}



#endif // __LOGENGINE_VERTEXBUFFER_H__

