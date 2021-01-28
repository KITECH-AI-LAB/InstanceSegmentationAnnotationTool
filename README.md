# InstanceSegmentationAnnotationTool

## 개요
#### 2021.01.28 [ver.1.3]
- 한글 경로 지원
- 버그 수정
  - manual mode undo 기능 보완
  - 같은 이미지를 여러 번 조회 시 결과 mask가 변경되는 문제 해결
  - dat 파일 시각화 시 outlier 제거
  
#### 2021.01.11 [ver.1.2]
- 시각화 기능 개선
  - bounding box 추가, contour 추가
<div align="center">
<img width="612" alt="0-2" src="https://user-images.githubusercontent.com/54341546/104527896-3531f580-5649-11eb-855e-477d844c148e.PNG">
<div align="left">

#### 2020.09.10 [ver.1.1]
- 버그 수정
  - manual mode 수정 사항 undo 적용 안됨 해결
  - 이미지 작업 중 다른 directory 불러올 시 이전 작업 내용으로 덮어써짐 해결
  - 1400 x 900 보다 큰 이미지 작업 시 label이 아닌 값이 mask에 저장 됨 해결
  - .txt 파일에 저장된 label이 누락되어 표시됨 해결
  - .mask 파일에 Background에 해당된 픽셀 값 0으로 저장
- 단축키 추가
  - M: manual mode on/off
  - N: watershed mask on/off
  - P: label 추가('+'버튼과 동일)
  - 키보드 방향키 위,아래를 통해 label간 이동 가능
- 첫 번째/마지막 이미지에서 이전/다음 이미지로 넘어갈 시 작업내용 저장 후 첫 번째/마지막 이미지임을 알림
- 결과 저장 시 label, mask ID의 정렬
- 랜덤 색상 배정시 무작위성 높임

#### 2020.04.22
- sort.py 추가
  - 저장된 mask, label 정렬
  - background에 할당된 픽셀 값 0으로 변경, label에서 background 제거
  - 사용법: sort.py의 89~90 줄 저장된 데이터 경로로 수정 후 python sort.py 명령어 실행

#### 2020.04.17
- 마우스 커서 위치 변경
- 데이터 저장 시 알림 추가

#### 2020.04.13 [ver.1]
- deep learning based instance segmentation model 학습용 mask 제작을 위해 만든 annotation tool
- watershed 알고리즘 이용(직접 결과 mask 그리는 기능 사용 가능)
- undo, redo 추가

## 빌드 방법
#### CMakeLists.txt의 5~6번째 줄을 수정하여 Qt, OpenCV 디렉토리 설정
<div align="center">
<img width="568" alt="0" src="https://user-images.githubusercontent.com/54341546/77394720-648a2400-6de3-11ea-9427-c00af4411601.png">
<div align="left">

#### CMake GUI를 이용하여 프로젝트, 솔루션 파일 생성
- Where is the source code: CMakeLists.txt가 있는 경로 입력(본 repository의 root 폴더)
- Where to build the binaries: 프로젝트, 솔루션을 저장할 경로 입력(없으면 자동 생성)
- Configure 버튼 클릭 -> Yes -> Optinal platform for generator(if empty, generator uses: Win 32) x64 선택 -> Finish
<div align="center">
<img width="612" alt="0-1" src="https://user-images.githubusercontent.com/54341546/77395290-9780e780-6de4-11ea-9c12-c8c7f08182eb.png">
<img width="612" alt="0-2" src="https://user-images.githubusercontent.com/54341546/77395303-9ea7f580-6de4-11ea-8ee2-60e370fdb4d2.png">
<div align="left">

- Generate 버튼 클릭 -> Open Project 버튼 클릭
<div align="center">
<img width="614" alt="0-3" src="https://user-images.githubusercontent.com/54341546/77395672-74a30300-6de5-11ea-86cf-8842b3a9bd25.png">
<div align="left">

#### Open Project 버튼을 통해 열린 솔루션 빌드
- Debug, Release에 대하여 각각 빌드 필요
- 빌드 후 아래의 DLL을 실행 파일 경로에 복사
    - Debug: qwindowsd.dll, Qt5Cored.dll, Qt5Guid.dll, Qt5Widgetsd.dll, opencv_world***d.dll
	- Release: qwindows.dll, Qt5Core.dll, Qt5Gui.dll, Qt5Widgets.dll, opencv_world***.dll
	- qwindows*.dll은 Qt 경로의 msvc2017_64/plugins/platforms에 있음, platforms/qwindows*.dll 형태로 복사
	- Qt5*.dll은 Qt 경로의 msvc2017_64/bin에 있음
	- opencv_world***.dll은 opencv-x.x.x/build/x64/vc15/bin에 있음

## 사용 방법
- UI
<div align="center">
<img width="957" alt="UI" src="https://user-images.githubusercontent.com/54341546/78971309-3aa46180-7b46-11ea-8ec9-f1ca899d5c59.PNG">
<div align="left">

    - Set image directory 버튼
        - 선택한 폴더 하위의 모든 이미지를 불러오는 기능 수행
        - 기능 수행 후 class 정보가 담긴 텍스트 파일을 불러온 상태가 아니라면 자동으로 Set class 버튼 실행

    - Set class 버튼
        - class 정보가 담긴 텍스트 파일 또는 names 파일을 불러오는 기능 수행

    - Circle size
        - mask를 그리는 펜 사이즈 변경 기능 수행
		
	- Draw mode
	    - mask 그리기 모드 변경 기능 수행
        - manual 체크 시: watershed 알고리즘 결과 mask 직접 그리기 모드, watershed 알고리즘 적용 후 사용 가능
		- manual 체크 해제 시: watershed 알고리즘 수행을 위한 mask 그리기 모드

    - Visualize
	    - manual mask: watershed 알고리즘 수행을 위한 mask
		- watershed mask: watershed 알고리즘 결과 mask
	
	- Change # of object
	    - 각 Class 하위 객체 추가 또는 삭제, Background 객체에는 사용 불가능(삭제는 가능)
	
    - Previous 버튼
        - 이전 이미지로 이동
        - 단축키: 왼쪽 방향키
    
    - Next 버튼
	    - 다음 이미지로 이동
		- 단축키: 오른쪽 방향키
	
	- Watershed 버튼
	    - watershed 알고리즘 수행
		- 단축키: space
	
	- Save 버튼
	    - 결과 저장
		- 단축키: ctrl+s

- 기타
    - 프로그램 실행 후 Set image directory 버튼과 Set class 버튼을 통해 이미지 경로와 class 정보가 초기화 필요
    - undo: ctrl+z
    - redo: ctrl+y
	- 저장되는 파일
	    - *.dat: watershed 알고리즘 수행을 위한 mask
		- *.mask: watershed 알고리즘 결과 mask
		- *.txt: *.mask에 대한 label

-작업 예시
<div align="center">
<img width="957" alt="UI" src="https://user-images.githubusercontent.com/68308047/92672801-7beaa880-f354-11ea-96a8-19373f7a03eb.png">
<div align="left">

    - 작업 희망 폴더 및 class 정보 텍스트 파일 불러옴
    - 우측 class tree에서 작업 희망 label 선택 후 이미지 위에서 드래그 하여 watershed 알고리즘의 가이드 라인 생성
       (Background의 경우 '+'버튼 사용 안함, 다른 label들은 객체별로 '+'를 통해 No. 생성 후 작업)
       (가이드 라인은 추후 딥러닝 모델에 사용되지 않음)

<div align="center">
<img width="957" alt="UI" src="https://user-images.githubusercontent.com/68308047/92673058-26fb6200-f355-11ea-9a1b-47c2aa455805.png">
<div align="left">

    - 모든 가이드 라인 작업 후 'watershed' 버튼을 통해 mask 생성

<div align="center">
<img width="957" alt="UI" src="https://user-images.githubusercontent.com/68308047/92673434-0da6e580-f356-11ea-9dee-7a3dc59d0ac8.png">
<div align="left">

    - manual 체크박스 체크 후 드래그하여 mask 수정

## 개발 환경
- Windows 10
- Visual studio 2017
- OpenCV 4.2.0
- Qt 5.14.1
- python 3.7

## Reference
- https://github.com/abreheret/PixelAnnotationTool
- https://github.com/developer0hye/Yolo_Label