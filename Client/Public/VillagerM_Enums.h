#pragma once

enum VillagerMAnim
{
	DemoM02_RunF1,
	LV1Villager_M_Attack01,
	LV1Villager_M_Attack02,
	LV1Villager_M_Attack03,
	LV1Villager_M_Attack05,
	LV1Villager_M_Die01,
	LV1Villager_M_Fall_End,
	LV1Villager_M_Fall_Loop,
	LV1Villager_M_FallGround,
	LV1Villager_M_FallHurt_B,
	LV1Villager_M_HurtCounter,
	LV1Villager_M_HurtFall_End_FaceUp,
	LV1Villager_M_HurtKnockBack,
	LV1Villager_M_HurtKnockDownB,
	LV1Villager_M_HurtKnockUp_Sart,
	LV1Villager_M_HurtL_F,
	LV1Villager_M_HurtLieGetup_FaceDown,
	LV1Villager_M_HurtLieGetup_FaceUp,
	LV1Villager_M_HurtLieLoop_FaceDown,
	LV1Villager_M_HurtLieLoop_FaceUp,
	LV1Villager_M_HurtS_FL,
	LV1Villager_M_HurtS_FR,
	LV1Villager_M_HurtStunEnd,
	LV1Villager_M_HurtStunLoop,
	LV1Villager_M_HurtStunStart,
	LV1Villager_M_IdleGeneral,
	LV1Villager_M_Sit_cough,
	LV1Villager_M_Sit_Idle,
	LV1Villager_M_SitToIdle,
	LV1Villager_M_SP_Idle1,
	LV1Villager_M_SP_Idle1_End,
	LV1Villager_M_SP01,
	LV1Villager_M_SP01_Attack01,
	LV1Villager_M_VSAmbushedDead,
	LV1Villager_M_VSTakeExecution,
	LV1Villager_M_VSTakeExecutionDead,
	LV1Villager_M_WalkB,
	LV1Villager_M_WalkF,
	LV1Villager_M_WalkL,
	LV1Villager_M_WalkR,
	LV1Villager_Parry,
};


enum class VillagerM_State
{
	State_Idle,
	State_Walk,
	State_Hit,
	State_Death,
	State_Attack,
	State_Stunned_Start,
	State_Stunned_Loop,
	State_End,
};