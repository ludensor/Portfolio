# EQS Navigation System
## 목차
* 개요
* 사용 방법
* 데모

## 개요
UE5(Ver 5.0)로 제작된 EQS(환경 쿼리)기반 내비게이션 시스템입니다.

언리얼엔진에 내장된 EnvironmentQuerySystem을 이용하여 길 찾기를 수행하는 모듈입니다.

## 사용 방법
1. AI의 위치에서 목적지가 보이는지 검사합니다.
2. 목적지가 보이면 그대로 이동하고 보이지 않으면, EQS를 실행합니다.
3. 복셀 생성기에서 각 아이템의 필터링과 점수를 평가하여 최종 아이템 위치 좌표를 얻습니다.
4. 아이템 위치 좌표로 AI를 이동시킵니다.
5. 2번 내용을 수행합니다.
6. 목적지에 도착하면 EQSNavMoveTo BTTask를 종료합니다.
<img src="https://github.com/ludensor/EQSNavigationSystem/assets/76856672/5a9b730d-d78a-430a-a1bc-d79bdf9f4b89.png" width="400" height="400"/>
<img src="https://github.com/ludensor/EQSNavigationSystem/assets/76856672/debd033b-ed23-4c94-a578-a4046a69e76a.png" width="400" height="400"/>

## 데모
특정 위치로 길 찾는 영상과, 특정 액터를 지속적으로 길 찾기를 하면서 따라가는 영상입니다.
<img src="https://github.com/ludensor/EQSNavigationSystem/assets/76856672/a85b853a-4393-4042-b6dd-4da178681d79.gif" width="400" height="400"/>
<img src="https://github.com/ludensor/EQSNavigationSystem/assets/76856672/d2e28186-82e7-4a60-98bc-7fb9627c4387.gif" width="400" height="400"/>
