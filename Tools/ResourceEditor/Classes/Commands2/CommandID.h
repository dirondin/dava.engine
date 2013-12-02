/*==================================================================================
    Copyright (c) 2008, binaryzebra
    All rights reserved.

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
    notice, this list of conditions and the following disclaimer in the
    documentation and/or other materials provided with the distribution.
    * Neither the name of the binaryzebra nor the
    names of its contributors may be used to endorse or promote products
    derived from this software without specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE binaryzebra AND CONTRIBUTORS "AS IS" AND
    ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
    WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
    DISCLAIMED. IN NO EVENT SHALL binaryzebra BE LIABLE FOR ANY
    DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
    (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
    LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
    ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
    (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
    SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
=====================================================================================*/



#ifndef __COMMAND_ID_H__
#define __COMMAND_ID_H__

enum CommandID
{
	CMDID_UNKNOWN	= -1,
	CMDID_BATCH		=  0,

	CMDID_TRANSFORM,

	CMDID_ENTITY_ADD,
	CMDID_ENTITY_REMOVE,
	CMDID_ENTITY_CHANGE_PARENT,
	CMDID_PARTICLE_LAYER_REMOVE,
	CMDID_PARTICLE_LAYER_MOVE,
	CMDID_PARTICLE_FORCE_REMOVE,
	CMDID_PARTICLE_FORCE_MOVE,
	CMDID_GROUP_ENTITIES_FOR_MULTISELECT,
	CMDID_LANDSCAPE_SET_TEXTURE,
	CMDID_LANDSCAPE_SET_HEIGHTMAP,

	CMDID_META_OBJ_MODIFY,
	CMDID_INSP_MEMBER_MODIFY,

	CMDID_KEYEDARCHIVE_ADD_KEY,
	CMDID_KEYEDARCHIVE_REM_KEY,
	CMDID_KEYEDARCHIVE_SET_KEY,

	CMDID_CUSTOM_COLORS_ENABLE,
	CMDID_CUSTOM_COLORS_DISABLE,
	CMDID_CUSTOM_COLORS_MODIFY,
	CMDID_VISIBILITY_TOOL_ENABLE,
	CMDID_VISIBILITY_TOOL_DISABLE,
	CMDID_VISIBILITY_TOOL_SET_POINT,
	CMDID_VISIBILITY_TOOL_SET_AREA,
	CMDID_HEIGHTMAP_EDITOR_ENABLE,
	CMDID_HEIGHTMAP_EDITOR_DISABLE,
	CMDID_HEIGHTMAP_MODIFY,
	CMDID_TILEMASK_EDITOR_ENABLE,
	CMDID_TILEMASK_EDITOR_DISABLE,
	CMDID_TILEMASK_MODIFY,
	CMDID_RULER_TOOL_ENABLE,
	CMDID_RULER_TOOL_DISABLE,
	CMDID_NOT_PASSABLE_TERRAIN_ENABLE,
	CMDID_NOT_PASSABLE_TERRAIN_DISABLE,

	CMDID_PARTICLE_EFFECT_UPDATE,
	CMDID_PARTICLE_EMITTER_UPDATE,
	CMDID_PARTICLE_LAYER_UPDATE,
	CMDID_PARTILCE_LAYER_UPDATE_TIME,
	CMDID_PARTICLE_LAYER_UPDATE_ENABLED,
	CMDID_PARTICLE_LAYER_UPDATE_LODS,
	CMDID_PARTICLE_FORCE_UPDATE,

	CMDID_PARTICLE_EMITTER_ADD,

	CMDID_PARTICLE_EFFECT_START_STOP,
	CMDID_PARTICLE_EFFECT_RESTART,

	CMDID_PARTICLE_EMITTER_LAYER_ADD,
	CMDID_PARTICLE_EMITTER_LAYER_REMOVE,
	CMDID_PARTICLE_EMITTER_LAYER_CLONE,
	CMDID_PARTICLE_EMITTER_FORCE_ADD,
	CMDID_PARTICLE_EMITTER_FORCE_REMOVE,
	
	CMDID_PARTICLE_EMITTER_LOAD_FROM_YAML,
	CMDID_PARTICLE_EMITTER_SAVE_TO_YAML,

	CMDID_DAE_CONVERT,
	CMDID_ENTITY_SAVE_AS,
    
    CMDID_CONVERT_TO_SHADOW,

	CMDID_LOD_DISTANCE_CHANGE,

	CMDID_BEAST,

	CMDID_COMPONENT_ADD,
	CMDID_COMPONENT_REMOVE,

	CMDID_DYNAMIC_SHADOW_CHANGE_COLOR,
	CMDID_DYNAMIC_SHADOW_CHANGE_MODE,
    
    CMDID_MATERIAL_ASSIGN,

	CMDID_USER		= 0xF000
};

#endif // __COMMAND_ID_H__
