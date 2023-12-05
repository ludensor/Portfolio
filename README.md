# Portfolio
## 목차

<b>

- [EQS Navigation System(플러그인)](#EQS-Navigation-System)
  - [개요](#프로젝트-개요)
  - [사용 방법](#사용-방법)
  - [데모](#데모)
- [Souls Like(개인 프로젝트)](#Souls-Like)
  - [개요](#프로젝트-개요)
  - [콘텐츠](#콘텐츠)
  - [시연 영상](https://www.youtube.com/watch?v=An0XZp3zEA8)

</b>

## EQS Navigation System
### 프로젝트 개요
> **설명:** 언리얼엔진의 EQS(환경 쿼리)를 이용한 3차원 내비게이션 시스템 모듈입니다.
>
> **개발 환경:** UnrealEngine 5.0
> 
> **제작 기간:** 2023.11.19 ~ 2023.11.30 (12일)
>
> **[소스 코드](https://github.com/ludensor/Portfolio/tree/master/EQSNavigationSystem/Plugins/EQSNavigationSystem/Source/EQSNavigationSystem)**

### 사용 방법
<img src="https://github.com/ludensor/EQSNavigationSystem/assets/76856672/32c08b74-0e9f-406c-b7cf-08f6bcaa4816.png" width="400" height="400"/>
<img src="https://github.com/ludensor/EQSNavigationSystem/assets/76856672/63825963-9566-4ab3-9eda-195419dc4b7b.png" width="400" height="400"/>

1. AI의 위치에서 목적지가 보이는지 검사합니다.
2. 목적지가 보이면 그대로 이동하고 보이지 않으면, EQS를 실행합니다.
3. 복셀 생성기에서 각 아이템의 필터링과 점수를 평가하여 최종 아이템 위치 좌표를 얻습니다.
4. 아이템 위치 좌표로 AI를 이동시킵니다.
5. 2번 내용을 수행합니다.
6. 목적지에 도착하면 EQSNavMoveTo BTTask를 종료합니다.

<img src="https://github.com/ludensor/EQSNavigationSystem/assets/76856672/c3cb9816-3cb4-4134-b4eb-e8bc5870e674.png">

- 길 찾기에 사용되는 EnvQuery 에셋


### 데모

<img src="https://github.com/ludensor/EQSNavigationSystem/assets/76856672/d7b9dfd9-5e0b-42cb-a049-1a6e524899a9.gif" width="400" height="400"/>
<img src="https://github.com/ludensor/EQSNavigationSystem/assets/76856672/7551d46d-4174-430a-b4f9-724ed1aaffa4.gif" width="400" height="400"/>

## Souls Like
### 프로젝트 개요
> **설명:** 소울류 장르의 포트폴리오입니다.
>
> **개발 환경:** UnrealEngine 4.26
>
> **제작 기간:** 2021.03.29 ~ 2021.06.30 (94일)
>
> **[시연 영상](https://www.youtube.com/watch?v=An0XZp3zEA8)**

### 콘텐츠
#### 락온을 이용한 대상 추적
<img src="https://github.com/ludensor/Portfolio/assets/76856672/7e0356d5-429b-4822-8f88-05a0553cff68.gif">

```cpp
void ASLPlayerController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	APawn* OwningPawn = GetPawn();
	if (OwningPawn && LockableTarget)
	{
		const FVector PawnLocation = OwningPawn->GetActorLocation();
		const FVector TargetLocation = LockableTarget->GetLockOnSocketLocation();
		const FVector ToTarget = TargetLocation - PawnLocation;

		const FRotator CurrentRotation = GetControlRotation();
		const FRotator TargetRotation = ToTarget.GetSafeNormal().Rotation();
		const float DistanceSquared = ToTarget.SizeSquared();
		const float Distance = FMath::Sqrt(DistanceSquared);

		FRotator DesiredRotation = FMath::RInterpTo(CurrentRotation, TargetRotation, DeltaSeconds, ControllerRotationLagSpeed);

		// 거리에 따른 Pitch 보간
		DesiredRotation.Pitch -= LockableDistanceFactor * FMath::InvSqrt(Distance);
		SetControlRotation(DesiredRotation);

		if (DistanceSquared > FMath::Square(LockableTargetDistance))
		{
			UnlockOnTarget();
		}
	}
}
```

- 카메라가 적 위치를 자동으로 포착하도록 기능을 구현하여 전투의 몰입도를 높였습니다.

#### Timeline을 이용한 박스 트리깅
<img src="https://github.com/ludensor/Portfolio/assets/76856672/6f2af63d-7438-49cc-891f-d6cf923f05fd.gif">
<img src="https://github.com/ludensor/Portfolio/assets/76856672/6d5f3c2a-eaaa-43b2-8515-14393be075e9.png" height = "300"/>

- 플레이어와 트리거 박스를 상호작용하여 문의 움직임을 Timeline 기능을 이용하여 제어하였습니다.

#### 피격 방향에 따른 히트 모션 다양화
<img src="https://github.com/ludensor/Portfolio/assets/76856672/e98386ad-1c5c-46f3-be22-63bfa7f0a1e1.gif">

```cpp
int32 USLGameStatics::ConvertYawRotationToIndex(FRotator Rotation, FRotator BaseRotation /*= FRotator::ZeroRotator*/, int32 Slice /*= 4*/)
{
	FVector BaseDirection = BaseRotation.Vector();
	const float OffsetAngle = 360.0f / static_cast<float>(Slice * 2);
	BaseDirection = BaseDirection.RotateAngleAxis(-OffsetAngle, FVector::UpVector);

	const FVector RightDirection = FVector(-BaseDirection.Y, BaseDirection.X, 0.0f);

	FVector TargetDirection = Rotation.Vector();
	TargetDirection = TargetDirection.GetSafeNormal2D();

	float Angle = FMath::Acos(FVector::DotProduct(BaseDirection, TargetDirection));
	const float CompareCos = FVector::DotProduct(RightDirection, TargetDirection);

	if (CompareCos < 0.0f)
	{
		Angle = 2.0f * PI - Angle;
	}

	const float Degrees = FMath::RadiansToDegrees(Angle);
	const int32 DesiredIndex = static_cast<int32>(Degrees * static_cast<float>(Slice / 360.0f));

	return DesiredIndex;
}
```

- 몬스터와 플레이어의 위치에 대한 방향벡터와 플레이어의 전방 벡터의 사이 각도를 인덱스화하여 피격 애니메이션을 재생하도록 구현하였습니다.
