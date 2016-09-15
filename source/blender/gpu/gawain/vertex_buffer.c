
// Gawain geometry batch
//
// This code is part of the Gawain library, with modifications
// specific to integration with Blender.
//
// Copyright 2016 Mike Erwin
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of
// the MPL was not distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

#include "vertex_buffer.h"
#include <stdlib.h>
#include <string.h>

VertexBuffer* create_VertexBuffer()
	{
	VertexBuffer* verts = malloc(sizeof(VertexBuffer));
	init_VertexBuffer(verts);
	return verts;
	}

void init_VertexBuffer(VertexBuffer* verts)
	{
	memset(verts, 0, sizeof(VertexBuffer));
	}

void allocate_vertex_data(VertexBuffer* verts, unsigned v_ct)
	{
	VertexFormat* format = &verts->format;
	if (!format->packed)
		pack(format);

	verts->vertex_ct = v_ct;

	// Data initially lives in main memory. Will be transferred to VRAM when we "prime" it.
	verts->data = malloc(vertex_buffer_size(format, v_ct));
	}

void setAttrib(VertexBuffer* verts, unsigned a_idx, unsigned v_idx, const void* data)
	{
	const VertexFormat* format = &verts->format;
	const Attrib* a = format->attribs + a_idx;

#if TRUST_NO_ONE
	assert(a_idx < format->attrib_ct);
	assert(v_idx < verts->vertex_ct);
	assert(verts->data != NULL); // data must be in main mem
#endif

	memcpy((GLubyte*)verts->data + a->offset + v_idx * format->stride, data, a->sz);
	}

void fillAttrib(VertexBuffer* verts, unsigned a_idx, const void* data)
	{
	const VertexFormat* format = &verts->format;
	const Attrib* a = format->attribs + a_idx;

#if TRUST_NO_ONE
	assert(a_idx < format->attrib_ct);
#endif

	const unsigned stride = a->sz; // tightly packed input data

	fillAttribStride(verts, a_idx, stride, data);
	}

void fillAttribStride(VertexBuffer* verts, unsigned a_idx, unsigned stride, const void* data)
	{
	const VertexFormat* format = &verts->format;
	const Attrib* a = format->attribs + a_idx;

#if TRUST_NO_ONE
	assert(a_idx < format->attrib_ct);
	assert(verts->data != NULL); // data must be in main mem
#endif

	const unsigned vertex_ct = verts->vertex_ct;

	if (format->attrib_ct == 1 && stride == format->stride)
		{
		// we can copy it all at once
		memcpy(verts->data, data, vertex_ct * a->sz);
		}
	else
		{
		// we must copy it per vertex
		for (unsigned v = 0; v < vertex_ct; ++v)
			memcpy((GLubyte*)verts->data + a->offset + v * format->stride, (const GLubyte*)data + v * stride, a->sz);
		}
	}