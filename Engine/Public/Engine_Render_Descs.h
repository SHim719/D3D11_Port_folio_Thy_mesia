#pragma once

namespace Engine
{
	class CGameObject;
}


typedef struct tagRadialBlurDescs
{
	Engine::CGameObject*		pActor;
	float						fBlurStrength;
	float						fBlurRadius;
} RADIALBLUR_DESCS;
