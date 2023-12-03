# EQS Navigation System
## 목차

<b>

- [개요](#프로젝트-개요)
- [사용 방법](#사용-방법)
- [데모](#데모)

</b>

## 프로젝트 개요
> **설명:** 언리얼엔진의 EQS(환경 쿼리)를 이용한 3차원 내비게이션 시스템 모듈입니다.
>
> **개발 환경:** UnrealEngine 5.0
> 
> **제작 기간:** 2023.11.19 ~ 2023.11.30

## 사용 방법
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


## 데모

<img src="https://github.com/ludensor/EQSNavigationSystem/assets/76856672/d7b9dfd9-5e0b-42cb-a049-1a6e524899a9.gif" width="400" height="400"/>
<img src="https://github.com/ludensor/EQSNavigationSystem/assets/76856672/7551d46d-4174-430a-b4f9-724ed1aaffa4.gif" width="400" height="400"/>
