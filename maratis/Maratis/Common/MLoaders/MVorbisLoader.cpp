/////////////////////////////////////////////////////////////////////////////////////////////////////////
// MaratisCommon
// MVorbisLoader.cpp
//
// Vorbis Sound loader
/////////////////////////////////////////////////////////////////////////////////////////////////////////

//========================================================================
// Copyright (c) 2003-2011 Anael Seghezzi <www.maratis3d.com>
//
// This software is provided 'as-is', without any express or implied
// warranty. In no event will the authors be held liable for any damages
// arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented; you must not
//    claim that you wrote the original software. If you use this software
//    in a product, an acknowledgment in the product documentation would
//    be appreciated but is not required.
//
// 2. Altered source versions must be plainly marked as such, and must not
//    be misrepresented as being the original software.
//
// 3. This notice may not be removed or altered from any source
//    distribution.
//
//========================================================================

#include "vorbis/codec.h"
#include "vorbis/vorbisfile.h"

#include <MCore.h>
#include "MVorbisLoader.h"

size_t M_VorbisRead(void *dest, size_t size, size_t count, void *file)
{
	return M_fread(dest, size, count, (MFile*)file);
}

int M_VorbisSeek(void *file, ogg_int64_t offset, int whence)
{
	return M_fseek((MFile*)file, offset, whence);
}

int M_VorbisClose(void *file)
{
	return M_fclose((MFile*)file);
}

long M_VorbisTell(void *file)
{
	return M_ftell((MFile*)file);
}

bool M_loadVorbisSound(const char * filename, void * data)
{
	if((!data) || (!filename))
		return false;

	MFile* file = M_fopen(filename, "rb");
	if (!file)
	{
		fprintf(stderr, "ERROR Load Vorbis : unable to open %s\n", filename);
		return false;
	}

	OggVorbis_File vf;

	ov_callbacks io;
	io.read_func = &M_VorbisRead;
	io.seek_func = &M_VorbisSeek;
	io.close_func = &M_VorbisClose;
	io.tell_func = &M_VorbisTell;

	if(ov_open_callbacks(file, &vf, NULL, 0, io) < 0)
	{
		fprintf(stderr,"ERROR Load Vorbis : input does not appear to be an ogg vorbis bitstream\n");
		return false;
	}

	vorbis_info * vi = ov_info(&vf, -1);
	unsigned int size = ov_pcm_total(&vf, -1) * vi->channels * 2;

	M_SOUND_FORMAT format;
	switch(vi->channels)
	{
		case 1:
			format = M_SOUND_FORMAT_MONO16;
			break;
		case 2:
			format = M_SOUND_FORMAT_STEREO16;
			break;
		default:
			fprintf(stderr, "ERROR Load Vorbis : unsupported format\n");
			return false;
	}

	MSound * sound = (MSound *)data;
	sound->create(format, size, (unsigned int)vi->rate);

	int current_section;
	char pcmout[4096];
	unsigned int offset = 0;

	while(true)
	{
		if (ov_read(&vf, pcmout, sizeof(pcmout), 0, 2, 1, &current_section) == 0)
			break;

		memcpy((char*)sound->getData() + offset, pcmout, sizeof(pcmout));
		offset += sizeof(pcmout);
	}

	ov_clear(&vf);
	M_fclose(file);

	return true;
}
