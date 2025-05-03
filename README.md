# AUE3034-project-group8
미래자동차공학과 [AUE3034] 마이크로프로세서 과목 프로젝트

# Team Members
- 김동협 2020027301
- 김민성 2021011803
- 조민건 2021083318

# Target: 필승 짱깸뽀
- [[wiki/마프 프로젝트 제안서|마프 프로젝트 제안서]]
## 디렉터리 구조

```
project_root/
├── uno/                        # Arduino Uno 관련 코드
│   ├── include/
│   │   ├── rulette.h           # 룰렛 모터 제어
│   │   ├── display.h           # LED 화면 관련
│   │   └── sound.h             # 소리 출력
│   ├── src/
│   │   ├── rulette.cpp
│   │   ├── display.cpp
│   │   └── sound.cpp
│   └── uno_main.cpp
│
├── nano/                       # Arduino Nano 관련 코드
│   ├── include/
│   │   ├── emg.h               # emg 관련 모듈, 후처리를 분리할지는 추후 고민
│   │   └── (module_02.h)
│   ├── src/
│   │   ├── emg.cpp
│   │   └── (module_02.cpp)
│   └── nano_main.cpp
│
├── common/                     # Uno와 Nano 공용 헤더 또는 설정
│   ├── include/
│   │   └── communication.h
│   └── src/
│       └── communication.cpp
│
├── wiki/                       # 시스템 문서, 메모, 회로도 설명 등
│   ├── rulette.md
│   ├── emg.md
│   └── game_system.md
│
└── README.md
```

## 참고

[cpp 파일 구조 / 헤더 관리](wiki/cpp-header-structure.md)
