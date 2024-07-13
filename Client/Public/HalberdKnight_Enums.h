#pragma once


enum HalberdKnightAnim
{
	HArmorLV1_Halberds_ComboA01,
	HArmorLV1_Halberds_ComboA02,
	HArmorLV1_Halberds_ComboB01,
	HArmorLV1_Halberds_ComboC01,
	HArmorLV1_Halberds_ComboC02,
	HArmorLV1_Halberds_ComboC03,
	HArmorLV1_Halberds_Dead,
	HArmorLV1_Halberds_HurtCounter,
	HArmorLV1_Halberds_HurtKnockBack,
	HArmorLV1_Halberds_HurtKnockDownB,
	HArmorLV1_Halberds_HurtKnockUp_Sart,
	HArmorLV1_Halberds_HurtL_F,
	HArmorLV1_Halberds_HurtM_FL,
	HArmorLV1_Halberds_HurtM_FR,
	HArmorLV1_Halberds_HurtS_FL,
	HArmorLV1_Halberds_HurtS_FR,
	HArmorLV1_Halberds_HurtStunEnd,
	HArmorLV1_Halberds_HurtStunLoop,
	HArmorLV1_Halberds_HurtStunStart,
	HArmorLV1_Halberds_HurtXL_F,
	HArmorLV1_Halberds_Idle,
	HArmorLV1_Halberds_Run_F,
	HArmorLV1_Halberds_TurnL90,
	HArmorLV1_Halberds_TurnR90,
	HArmorLV1_Halberds_WalkB,
	HArmorLV1_Halberds_WalkF,
	HArmorLV1_Halberds_WalkL,
	HArmorLV1_Halberds_WalkR,
};

enum class HalberdKnight_State
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
