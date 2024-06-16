#pragma once

enum OdurAnim
{
	Magician_Appear_B,
	Magician_Appear_F,
	Magician_Appear_L,
	Magician_Appear_R,
	Magician_CaneAttack1Appear,
	Magician_CaneAttack2,
	Magician_CaneAttack03,
	Magician_Disappear,
	Magician_DisappearMove_B,
	Magician_DisappearMove_BShort,
	Magician_DisappearMove_F,
	Magician_DisappearMove_L,
	Magician_DisappearMove_LShort,
	Magician_DisappearMove_R,
	Magician_DisappearMove_RShort,
	Magician_DisppearIdle,
	Magician_HurtCounter,
	Magician_HurtDisappear,
	Magician_HurtFL,
	Magician_HurtFR,
	Magician_HurtSL,
	Magician_HurtSR,
	Magician_Idle,
	Magician_Idle2,
	Magician_JumpAppear,
	Magician_KickCombo,
	Magician_Parry01,
	Magician_ParryAttack01,
	Magician_ParryAttack01_2,
	Magician_ParryJump,
	Magician_Shoot_2_V1,
	Magician_Shoot1,
	Magician_Shoot2,
	Magician_Shoot3,
	Magician_ShootCombo1_1,
	Magician_ShootCombo1_2,
	Magician_ShotJump,
	Magician_SPAttack1,
	Magician_SPAttack1_Suc,
	Magician_SPAttack2,
	Magician_SPAttack2_Start,
	Magician_SPAttack2_Suc,
	Magician_Sprinkle,
	Magician_Stage2_Attack02,
	Magician_Stage2_Attack03,
	Magician_Stage2_Attack04,
	Magician_Stage2_Attack05,
	Magician_Stage2_Attakc01,
	Magician_Stage2_Cane2Sword,
	Magician_Stage2_JumpAppear,
	Magician_Stage2_SwrodAttackCombo,
	Magician_StunEnd_Cane,
	Magician_StunEnd_Sword,
	Magician_StunLoop_Cane,
	Magician_StunLoop_Sword,
	Magician_StunStart_Cane,
	Magician_StunStart_Sword,
	Magician_SwordAttack1,
	Magician_SwordAttack1Appear,
	Magician_SwordAttack2,
	Magician_SwordAttack2_V2,
	Magician_SwordAttack2Disappear,
	Magician_VSCorvus_TakeExecution,
	Magician_VSCorvus_TakeExecutionDisappear,
	Magician_Walk2F,
	Magician_Walk2L,
	Magician_Walk2R,
	Magician_WalkBDisappear,
	Magician_WalkFDisappear,
	Magician_WalkLDisappear,
	Magician_WalkRDisappear,
	MagicianLV1_Seq_BossFightStart,
};

enum class OdurState
{
	State_Idle,
	State_Walk,
	State_Hit,
	State_CaneAttack1,
	State_CaneAttack2,
	State_KickCombo,
	State_DisappearWalk,
	State_DisappearMove,
	State_Appear,
	State_Parry,
	State_End,
};