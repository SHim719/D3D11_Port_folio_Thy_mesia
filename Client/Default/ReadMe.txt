﻿2024-03-29 - Ver.00

키매니져사용
if (m_pGameInstance->GetKeyDown(ekeyCode::W))
	// 샬라샬라
이런 느낌
GetKeyDown => 처음누를 때 
GetKey => 누르고있을 때
GetKeyUp => 막 땟을 때
GetKeyNone => 안누를 때

CGameObject에 
- CTransform 디폴트로 추가
- Get_Transform() 추가

- 디폴트 백버퍼 색깔 = 검은색 => 경사로 구간에서 배경이 검은색이더라

- 유니티 빌드 + 다중 프로세서 컴파일추가 (한글 경로 없어야함)

- Terrain 이런거 우리 게임에 안쓸 것 같아서 일단 안넣었음. 필요하면 넣겠음
- 카메라 일부러 안넣었음 민혁형이 만든거 쓸거임
- UpdateLib 안돌려도됨.


2024-03-31
VIBuffer쪽 수정.

2024-04-02 Ver 0.1

Level 클래스에 Load_MapObject 추가
Transform에 Set_WorldMatrix 추가
Engine_Defines에 assert헤더 포함

1차 머지


UI_Active 목록

- Ui_CrossHair
- Ui_Pistol_Right_Hand

