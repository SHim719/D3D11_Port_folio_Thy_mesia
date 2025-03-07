#pragma once

enum TwinBladeKnightAnim
{
	LArmor_TwinSwords_ComboA02,
	LArmor_TwinSwords_ComboA03,
	LArmor_TwinSwords_ComboB01_1,
	LArmor_TwinSwords_ComboB01_2,
	LArmor_TwinSwords_ComboC01,
	LArmor_TwinSwords_ComboC02,
	LArmor_TwinSwords_ComboC03,
	LArmor_TwinSwords_Dodge_B,
	LArmor_TwinSwords_HurtCounter,
	LArmor_TwinSwords_HurtKnockBack,
	LArmor_TwinSwords_HurtL_F,
	LArmor_TwinSwords_HurtLieGetup_FaceUp,
	LArmor_TwinSwords_HurtM_FL,
	LArmor_TwinSwords_HurtM_FR,
	LArmor_TwinSwords_HurtS_FL,
	LArmor_TwinSwords_HurtS_FR,
	LArmor_TwinSwords_HurtStunEnd,
	LArmor_TwinSwords_HurtStunLoop,
	LArmor_TwinSwords_HurtStunStart,
	LArmor_TwinSwords_Idle,
	LArmor_TwinSwords_IdleN,
	LArmor_TwinSwords_IdleN2Fight,
	LArmor_TwinSwords_RaidAttack1,
	LArmor_TwinSwords_Run_F,
	LArmor_TwinSwords_SP_Attack01_End,
	LArmor_TwinSwords_SP_Attack01_Loop,
	LArmor_TwinSwords_SP_Attack01_Start,
	LArmor_TwinSwords_SP_Idle01,
	LArmor_TwinSwords_TurnL90,
	LArmor_TwinSwords_TurnR90,
	LArmor_TwinSwords_WalkB,
	LArmor_TwinSwords_WalkF,
	LArmor_TwinSwords_WalkL,
	LArmor_TwinSwords_WalkR,
	HArmorLV1_Halberds_VS_TakeExecution,
	LArmor_TwinSwords_ComboA01,
	TwinBladeKnightAnim_End,
};

enum class TwinBladeKnight_State
{
	State_Idle,
	State_Walk,
	State_Hit,
	State_ComboA,
	State_ComboB,
	State_ComboC,
	State_ParryAttack,
	State_Dodge,
	State_Stunned_Start,
	State_Stunned_Loop,
	State_Executed_Start,
	State_Death,
	State_End,
};
