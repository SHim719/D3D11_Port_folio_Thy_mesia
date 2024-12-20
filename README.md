<div align="center">
  
# 티메시아 모작(DirectX 11, 개인프로젝트)

<img src="https://i.namu.wiki/i/4VDxvXKYKcyXqsQfhMk2fJGnk6ha388DZEOQ_RRWF62bmAnaAb5rFsKbfKQ8db47fc4ooCQpj4GXUIHUyozo2dyVheiR5DX5ZKWO1dCfBQVLhVgNUcqQ4l5Du9xgisyblmaz0cv3FUW9Qtv_Oq8ZhQ.webp" alt="이미지" width="400">

2024.05.15 ~ 2025.08.23
</div>

# 동영상
https://www.youtube.com/watch?v=F7a7G8MvuqQ

# 프로젝트 설명

## 프레임워크

### 이벤트 매니져

- 함수 호출 시점에 의하여 발생하는 문제를 해결하기 위해 이벤트 매니저를 설계했습니다. 이를 통해 호출 시점으로 인한 문제들을 효과적으로 해결할 수 있었습니다.
- std::function<void()>를 이용해서 void 함수를 바인딩해 주어서 이벤트 매니져에 넘깁니다. 이벤트 매니져는 list 컨테이너에 바인딩 된 함수들을 저장합니다. 
- 다음 프레임 게임 루프(PriorityTick, Tick, LateTick) 시작 전에 바인딩 된 함수들을 호출합니다.

### 애니메이션 키 프레임 이벤트

- 애니메이션 재생 중 특정 동작에서 효과음 및 이펙트를 활성화하고 콜라이더를 활성화하거나 비활성화하는 등의 처리를 효율적으로 하기 위해 키 프레임 이벤트를 구현했습니다.
- std::unordered_multimap을 이용해서 특정 키 프레임에 여러 개의 이벤트를 호출할 수 있도록 설계했습니다.
- 객체 초기화 시점에서  std::function<void()>를 이용해서 void 함수를 키 프레임에 바인딩해 주었습니다.

## 최적화

### FBX To Bin

- Assimp로 FBX 모델을 로드할 때 로드 시간이 매우 느렸습니다. 이를 해결하기 위해 FBX를 로드한 후 필요한 정보만 추출하여 Bin 파일로 변환하고, 이후 변환된 Bin 모델 데이터만을 사용했습니다.
- 이 덕분에 모델 로드 시간이 대략 1분에서 3초 정도로 대폭 개선되었습니다.

### Frustum Culling

- 카메라의 프러스텀에 포함되지 않는 객체를 렌더링하는 것은 자원 낭비라고 생각했습니다. 그래서 뷰 프러스텀에 포함되지 않은 객체는 렌더링하지 않도록 프러스텀 컬링을 구현했습니다.
- 객체마다 일정 값의 반지름을 할당하여, 해당 반지름의 구가 프러스텀에 포함되지 않으면 객체를 그리지 않도록 했습니다.

### Instancing
<img src="https://github.com/SHim719/Image/blob/main/%EC%9D%B8%EC%8A%A4%ED%84%B4%EC%8B%B11.png" alt="이미지" width="450" height="350"> <img src="https://github.com/SHim719/Image/blob/main/%EC%9D%B8%EC%8A%A4%ED%84%B4%EC%8B%B12.png" alt="이미지" width="450">

- 바닥, 울타리, 다리, 파티클 등의 오브젝트 들이 똑같은 메쉬 데이터를 쓰고 있었고, 적어도 100개 이상이 한번에 그려져서 드로우 콜이 기하급수적으로 증가하는 문제가 있었습니다.
- DirectX에서 지원하는 하드웨어 인스턴싱을 이용해 한 번의 드로우 콜로 여러 개의 동일한 오브젝트를 그려, 최적화를 시켜주었습니다.


## 플레이어, 몬스터

### 상속구조
<img src="https://github.com/SHim719/Image/blob/main/%EC%BA%90%EB%A6%AD%ED%84%B0%20%EA%B5%AC%EC%A1%B0.webp" alt="이미지" width="500">

- 플레이어와 몬스터는 공통된 객체와 로직이 많아 동일한 코드를 반복적으로 작성해야 하는 상황이 발생했습니다. 
- 그래서 상태, 모델, 네비게이션, 무기 등 공통된 객체와 이를 처리하는 메서드를 포함한 Character라는 추상 부모 클래스를 설계하고 상속받게 했습니다.

### 전투 로직
- 티메시아는 짧은 넉백, 긴 넉백, 방어 불가 등 여러가지 공격이 있습니다. 이러한 여러가지의 공격에 대한 정보를 효과적으로 전달할 수 있는 방법이 뭘까 생각했습니다.
- 데미지, 공격 종류등의 정보를 담을 수 있는 ATTACKDESC 이라는 구조체를 만들었습니다. 이를 통해 공격 정보를 읽어 해당 공격에 적합한 히트 처리를 편리하게 구현할 수 있었습니다.

### 상태 패턴
<img src="https://github.com/SHim719/Image/blob/main/%EC%83%81%ED%83%9C%ED%8C%A8%ED%84%B4%EC%83%81%EC%86%8D%EA%B5%AC%EC%A1%B0.png" alt="이미지" width="500">

- 플레이어, 몬스터의 제어는 기본적으로 상태 패턴으로 구현했습니다. 상태 패턴이 객체 상태에 따른 애니메이션 변경 등 각종 로직을 처리하기에 용이하다고 생각했습니다.
- State_Base라는 추상 클래스를 만들고, OnState_Start와 OnState_End라는 순수 가상 함수를 선언했습니다. 이를 각 상태에서 오버라이딩하여 상태 전환에 따른 처리를 구현했습니다.
- 각 상태에서 Update와 Late_Update를 오버라이딩하여 상태별 로직을 구현했습니다.
  
## 애니메이션

### 키 프레임 선형 보간
<img src="https://github.com/SHim719/Image/blob/main/%EC%95%A0%EB%8B%88%EB%A9%94%EC%9D%B4%EC%85%98%EC%84%A0%ED%98%95%EB%B3%B4%EA%B0%84.gif" alt="이미지" width="500">

- 애니메이션을 전환할 때 툭 끊기는 느낌이 들어서 부자연스러워 보입니다.
- 애니메이션 변경 당시의 키 프레임의 Position, Rotation, Scale과 새로 재생할 애니메이션의 시작 프레임을 일정 시간 동안 선형 보간 해주어서 자연스러운 애니메이션 전환을 구현했습니다.

### 루트 애니메이션
<img src="https://github.com/SHim719/Image/blob/main/%EB%A3%A8%ED%8A%B8%EB%AA%A8%EC%85%98.gif" alt="이미지" width="500">

- 애니메이션을 재생할 때, 루트 본의 움직임을 그대로 월드 공간에 반영하고 싶었습니다.
- 이전 프레임의 루트 본과 현재 프레임의 루트 본의 Position 값의 차이를 저장하고 그 변화량만큼 객체를 이동해 주었습니다.


## 카메라

### 카메라 위치 보간
<img src="https://github.com/SHim719/Image/blob/main/CameraLerp.gif" alt="이미지" width="500">

- 카메라가 플레이어에 대해 고정되어 있으면 부자연스러워 보입니다. 그래서 카메라가 플레이어를 자연스럽게 따라가도록 구현했습니다. 
- 이전 프레임 플레이어의 위치와 현재 플레이어의 위치를 보간하여, 보간된 위치를 기준으로 카메라의 위치를 확정해 줬습니다.

### 락온
<img src="https://github.com/SHim719/Image/blob/main/%EB%9D%BD%EC%98%A8%EC%B9%B4%EB%A9%94%EB%9D%BC.gif" alt="이미지" width="500">

- 카메라의 Look을 바로 타겟한테 옮기면 시점이 갑자기 달라져서 부자연스러워 보입니다.
- 이를 해결하기 위해 현재 카메라의 Look 방향과 타겟의 Look 방향에 대한 회전 행렬을 이용해 쿼터니언을 계산한 후, Slerp를 사용해 두 쿼터니언을 보간하여 카메라의 Look 방향을 결정했습니다.

## 쉐이더




## 툴









