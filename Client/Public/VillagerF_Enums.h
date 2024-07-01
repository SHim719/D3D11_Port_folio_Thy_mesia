#pragma once

enum VillagerFAnim
{
	LV1Villager_F_Attack01,
	LV1Villager_F_Attack02,
	LV1Villager_F_Attack03,
	LV1Villager_F_Attack04,
	LV1Villager_F_Die01,
	LV1Villager_F_Die02,
	LV1Villager_F_HurtCounter,
	LV1Villager_F_HurtFall_End_FaceUp,
	LV1Villager_F_HurtKnockBack,
	LV1Villager_F_HurtKnockDownF,
	LV1Villager_F_HurtKnockUp_Sart,
	LV1Villager_F_HurtL_F,
	LV1Villager_F_HurtLieGetup_FaceDown,
	LV1Villager_F_HurtLieGetup_FaceUp,
	LV1Villager_F_HurtLieLoop_FaceDown,
	LV1Villager_F_HurtLieLoop_FaceUp,
	LV1Villager_F_HurtM_FL,
	LV1Villager_F_HurtM_FR,
	LV1Villager_F_HurtStunEnd,
	LV1Villager_F_HurtStunHurt,
	LV1Villager_F_HurtStunLoop,
	LV1Villager_F_HurtStunStart,
	LV1Villager_F_HurtXL_F,
	LV1Villager_F_Idle,
	LV1Villager_F_Run_N,
	LV1Villager_F_TurnL90,
	LV1Villager_F_TurnR90,
	LV1Villager_F_WalkB,
	LV1Villager_F_WalkF,
	LV1Villager_F_WalkL,
	LV1Villager_F_WalkR,
};


enum class VillagerF_State
{
	State_Idle,
	State_Walk,
	State_Hit,
	State_Attack1,
	State_Attack2,
	State_Attack3,
	State_Stunned_Start,
	State_Stunned_Loop,
	State_Death,
	State_End,
};