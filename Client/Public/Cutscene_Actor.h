#pragma once

#include "Client_Defines.h"

namespace Client
{
	class CCutscene_Actor abstract
	{
	public:
		virtual void OnEnter_Cutscene() PURE;
		virtual void OnStart_Cutscene(CUTSCENE_NUMBER eCutsceneNumber) PURE;
		virtual void OnEnd_Cutscene() PURE;
	};

}

