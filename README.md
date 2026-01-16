<div align="center">
  
# 티메시아 모작(개인 프로젝트)

<img src="https://github.com/SHim719/Image/blob/main/%ED%8B%B0%EB%A9%94%EC%8B%9C%EC%95%84%EC%8D%B8%EB%84%A4%EC%9D%BC.png" alt="이미지" width="500">  

**C++, Direct X 11, ImGUI, Assimp, Fmod, Github Desktop**

</div>

# 동영상
https://youtu.be/3FAKisv6opQ

# 프로젝트 구현 사항

## 쉐이더

### 디퍼드 렌더링
<img src="https://github.com/SHim719/Image/blob/main/Diffuse.png" alt="이미지" width="250" height="250"> <img src="https://github.com/SHim719/Image/blob/main/Normal.png" alt="이미지" width="250" height="250"> <img src="https://github.com/SHim719/Image/blob/main/Depth.png" alt="이미지" width="250" height="250"> <img src="https://github.com/SHim719/Image/blob/main/Shade.png" alt="이미지" width="250" height="250"> <img src="https://github.com/SHim719/Image/blob/main/Deferred.png" alt="이미지" width="250" height="250">

(차례로 Diffuse, Normal, Depth, Shade, Deferred)

- 기존 물체 렌더링 방식인 포워드 렌더링 방식을 사용하면 모든 물체에 대해 조명 연산을 진행하기 때문에 다중 조명 연산에 있어서 계산량이 높습니다. 그래서 다중 조명 연산에 최적화 된 디퍼드 렌더링을 구현했습니다.
- Diffuse, Normal, Depth 등의 정보를 G-버퍼에 저장하고, G-버퍼의 정보를 이용해 조명 연산을 해주었습니다. 또한 G-버퍼에 저장된 정보를 활용하여 후처리 효과를 보다 효율적으로 구현할 수 있었습니다.

## 최적화

### 바이너리 직렬화를 통한 모델 로딩 속도 최적화

- **문제**: Assimp 라이브러리의 런타임 파싱 비용으로 인해 모델 로딩 시 심각한 병목 현상(약 3분 소요)이 발생했습니다.
- **해결**: 모델 데이터를 파싱한 후, 즉시 로드 가능한 **바이너리 포맷으로 직렬화**해서 저장하는 전처리 과정을 도입했습니다.
- **성과**: 모델들의 로드 시간을 약 3분에서 5초 정도 대폭 단축했습니다.

### Frustum Culling

- **목표**: 카메라 시야 밖의 객체까지 렌더링 파이프라인에 들어가는 비효율을 방지하고자 했습니다.
- **구현** : 각 객체에 반지름을 세팅하고, 카메라의 6개 평면과의 충돌 여부를 판별했습니다.
- **결과** : 시야에 보이지 않는 객체는 드로우 콜 제출 단계 이전에 조기 제외시켜 GPU 연산 부하를 효과적으로 줄였습니다.

### Instancing
<img src="https://github.com/SHim719/Image/blob/main/%EC%9D%B8%EC%8A%A4%ED%84%B4%EC%8B%B12.png" alt="이미지" width="450">

- **문제** : 바닥, 울타리, 파티클 등 동일한 지오메트리를 가진 다수의 오브젝트 렌더링 시, 드로우 콜의 과도한 발생으로 오버헤드가 생기는 문제가 있었습니다.
- **해결** : DirectX의 하드웨어 인스턴싱 기법을 적용하여, 인스턴스 버퍼를 통해 월드 행렬 등의 개별 데이터를 전달했습니다.
- **성과** : 수백 개의 오브젝트 렌더링을 한 번의 드로우 콜로 처리하여 렌더링 성능을 최적화했습니다.

### Geometry Shader

- **구현**: 파티클 및 UI 렌더링 시, CPU에서는 정점(Point) 하나만 넘겨주고 Geometry Shader 단계에서 사각형으로 확장하는 기법을 사용했습니다.
- **효과**: 정점 4개 대신 1개만 전송함으로써 CPU-GPU 간 데이터 전송 병목을 줄여서, 파이프라인 효율을 높였습니다.

### 렌더 타겟을 활용한 맵 오브젝트 피킹
-  **문제** : 씬에 배치된 오브젝트가 많아질수록, 기존 레이캐스팅 방식의 피킹 연산 비용이 선형적으로 증가하는 문제가 있었습니다.
-  **해결** :각 오브젝트의 고유 ID를 색상 값으로 매핑하여 별도의 렌더 타겟에 그리는 방식을 이용했습니다.
-  **성과** : 마우스 클릭 시 해당 픽셀의 색상 값을 읽어오는 것만으로 즉시 객체를 식별할 수 있게 되어, 오브젝트 개수와 무관한 O(1)의 피킹 성능을 확보할 수 있었습니다.

## 애니메이션

### 애니메이션 키 프레임 이벤트
<img src="https://github.com/SHim719/Image/blob/main/%ED%82%A4%ED%94%84%EB%A0%88%EC%9E%84%ED%88%B4.png" alt="이미지" width="300" height="450"><img src="https://github.com/SHim719/Image/blob/main/%ED%82%A4%ED%94%84%EB%A0%88%EC%9E%84%EC%9D%B4%EB%B2%A4%ED%8A%B8%EC%B2%B4%ED%81%AC.png" alt="이미지" width="400" height="600">
<img src="https://github.com/SHim719/Image/blob/main/%ED%82%A4%ED%94%84%EB%A0%88%EC%9E%84%EC%9D%B4%EB%B2%A4%ED%8A%B8%EB%B0%94%EC%9D%B8%EB%94%A9.png" alt="이미지" width="600">

- **구현 의도**: 애니메이션과 게임 로직(이펙트, 사운드, 피격 판정)의 정확한 동기화를 위해 키 프레임 이벤트 시스템을 구축했습니다.
- **구현 방법**: std::function을 활용한 콜백 방식으로 설계하여, 애니메이션 시스템이 구체적인 게임 로직을 알 필요 없도록 결합도를 낮췄습니다. 이를 통해 특정 애니메이션 키 프레임에 여러가지의 이벤트를 유연하게 바인딩할 수 있었습니다.

### 애니메이션 블렌딩 
<img src="https://github.com/SHim719/Image/blob/main/%EB%B3%B4%EA%B0%84%EC%A0%84.gif" alt="이미지" width="300"> <img src="https://github.com/SHim719/Image/blob/main/%EB%B3%B4%EA%B0%84%ED%9B%84.gif" alt="이미지" width="300">

#### &emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;적용 전&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;적용 후

<img src="https://github.com/SHim719/Image/blob/main/%ED%82%A4%ED%94%84%EB%A0%88%EC%9E%84%EB%B3%B4%EA%B0%80.png" alt="이미지" width="600">

- **문제**: 애니메이션 전환 시 툭 끊기게 전환되는 문제를 해결하고자 했습니다.
- **구현 방법**: 애니메이션 변경 당시의 키 프레임의 Transform값과 새로 재생할 애니메이션의 시작 프레임의 Transform 값을 일정 시간 동안 선형 보간 해주어서 자연스러운 애니메이션 전환을 구현했습니다.

### 루트 모션
<img src="https://github.com/SHim719/Image/blob/main/%EB%A3%A8%ED%8A%B8%EC%95%A0%EB%8B%88%EB%A9%94%EC%9D%B4%EC%85%98%EC%A0%81%EC%9A%A9%EC%A0%84.gif" alt="이미지" width="350"><img src="https://github.com/SHim719/Image/blob/main/%EB%A3%A8%ED%8A%B8%EB%AA%A8%EC%85%98.gif" alt="이미지" width="350">
#### &emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;적용 전&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;적용 후

- 애니메이션의 보폭과 실제 이동 거리를 일치시키기 위해 루트 모션을 도입했습니다.
- 루트 본의 프레임별 이동 변화량을 추출하여 월드 좌표계에 반영함으로써, 애니메이션과 실제 이동이 동기화된 움직임을 구현했습니다.

## 카메라

### 카메라 스무딩
<img src="https://github.com/SHim719/Image/blob/main/CameraLerp.gif" alt="이미지" width="500">
<img src="https://github.com/SHim719/Image/blob/main/%EC%B9%B4%EB%A9%94%EB%9D%BC%EB%B3%B4%EA%B0%84.png" alt="이미지" width="500">

- **구현 의도**: 카메라가 캐릭터에 고정되는 부자연스러움을 줄이고, 부드러운 조작감을 주기 위해 구현했습니다.
- **구현 방법**: 현재 카메라 위치와 플레이어의 목표 위치를 선형 보간 하여, 카메라가 뒤따라가는 듯한 지연 효과를 주어 자연스러운 움직임을 연출했습니다.

### 타겟 락온(Lock-On) 카메라 보간
<img src="https://github.com/SHim719/Image/blob/main/%EB%9D%BD%EC%98%A8%EC%B9%B4%EB%A9%94%EB%9D%BC.gif" alt="이미지" width="500">
<img src="https://github.com/SHim719/Image/blob/main/%EB%9D%BD%EC%98%A8%EB%B3%B4%EA%B0%84.png" alt="이미지" width="500">

- **문제 해결**: 락온 시 시점이 순간적으로 튀는 현상을 방지하여, 시점 전환의 어색함을 개선해습니다.
- **구현 방법**: 락온 시 현재 카메라의 Forward 벡터와 타겟을 향한 Look 벡터를 매 프레임 보간하여, 시점이 부드럽게 회전하며 타겟을 조준하도록 구현했습니다.


## 플레이어, 몬스터

### 계층적 캐릭터 클래스
<img src="https://github.com/SHim719/Image/blob/main/%EC%BA%90%EB%A6%AD%ED%84%B0%20%EA%B5%AC%EC%A1%B0.webp" alt="이미지" width="500">

- 플레이어와 몬스터가 공유하는 핵심 로직(이동, 메쉬, 무기 시스템)을 추상 부모 클래스(Character)로 일반화 코드 재사용성과 유지보수성을 확보했습니다.
- 이를 통해 신규 몬스터 추가 시, 기본 로직은 상속받고 고유 패턴만 오버라이딩하여 빠르게 구현할 수 있는 확장성 있는 구조를 갖췄습니다.

### 전투 이벤트 정보의 규격화 (AttackDesc)
<img src="https://github.com/SHim719/Image/blob/main/AttackDesc%EC%BD%94%EB%93%9C.png" alt="이미지" width="500">

- 넉백, 경직, 방어 불가 등 다양한 피격 리액션을 처리하기 위해, 함수 인자가 아닌 구조체(AttackDesc) 기반의 메시지 전달 방식을 채택했습니다.
- 공격 주체가 공격의 정보를 구조체에 담아 전달하면, 피격 객체가 이를 해석하여 적절한 상태로 전이되도록 전투 로직을 데이터 중심으로 단순화했습니다.

### FSM 기반의 행동 제어
<img src="https://github.com/SHim719/Image/blob/main/%EC%83%81%ED%83%9C%ED%8C%A8%ED%84%B4%EC%83%81%EC%86%8D%EA%B5%AC%EC%A1%B0.png" alt="이미지" width="500">

- 캐릭터의 복잡한 행동과 애니메이션 동기화를 제어하기 위해 유한 상태 머신(FSM) 구조를 도입했습니다.
- Enter(Start), Exit(End), Update로 상태의 수명 주기를 명확히 분리하여, 상태 전이 시 초기화와 정리 작업이 누락 없이 수행되도록 안전성을 높였습니다.

## 툴

### 이펙트 툴
<img src="https://github.com/SHim719/Image/blob/main/%EC%9D%B4%ED%8E%99%ED%8A%B8%ED%88%B41.gif" alt="이미지" width="400"> <img src="https://github.com/SHim719/Image/blob/main/%EC%9D%B4%ED%8E%99%ED%8A%B8%ED%88%B42.gif" alt="이미지" width="400">

- **실시간 미리보기**: 파티클, 트레일 등 다양한 이펙트의 속성을 실시간으로 조정하고 시각적으로 확인하며 작업 효율을 높였습니다.  
- **바이너리 직렬화**: 이펙트 속성 데이터를 엔진에서 즉시 로드 가능한 바이너리 포맷으로 저장하여 로딩 속도를 최적화했습니다.  
- **오브젝트 풀링 최적화**: 런타임 생성 비용을 줄이기 위해, 게임 로딩 시점에 이펙트 객체와 미리 생성하여 이펙트 생성에 의한 프레임 드랍을 방지했습니다.

### 맵 툴
<img src="https://github.com/SHim719/Image/blob/main/%EB%A7%B5%ED%88%B41.gif" alt="이미지" width="400"> <img src="https://github.com/SHim719/Image/blob/main/%EB%A7%B5%ED%88%B42.gif" alt="이미지" width="400">  
- **레벨 편집 효율화**: 월드 상에 메쉬와 기믹 오브젝트(문, 체크포인트 등)를 직관적으로 배치하고 수정할 수 있는 에디터를 제작했습니다.  
- **오브젝트 배치 및 관리**: 배경을 구성하는 정적 메쉬뿐만 아니라 문, 체크포인트 등 로직이 포함된 기믹 오브젝트의 배치와 데이터 저장을 지원하여 원활한 레벨 디자인이 가능하도록 했습니다.  


