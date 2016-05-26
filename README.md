#[2016_한이음] 3팀_IoT기반 국도 횡단보도 보행자 지킴이

멘토
+ 지도 멘토 : 윤정훈 멘토 (yoonjunghun@naver.com)
+ 서브 멘토 : 김유정 멘토 (bluehorn@cu.ac.kr)

멘티
+ *안광은 멘티 (yooer10@cu.ac.kr)
+ 정영주 멘티 (vertex50@cu.ac.kr)
+ 이성원 멘티 (lee12041993@cu.ac.kr)
+ 이민우 멘티 (dnj011@cu.ac.kr)
+ 이득희 멘티 (te03074@cu.ac.kr)

# 설명
라즈베리파이에서 웹캠을 운용하기 위한 프로그램입니다.

# 시작 전 참고

현재 개발 주력 IDE는 Eclipse이며 프로그램 버전 관리는 Github로 이루어 집니다. 개발 시작 전 Eclipse 사용법과 Git개념 및 Github사용법 숙지는 필수입니다.

개발 도구인 Eclipse와 Git의 사용방법은 아래 링크에서 다운받으면 됩니다.
+ 안광은_EGit, Git for Eclipse.pdf : [다운로드](http://203.250.32.155:5000/fbsharing/lwCfWNUq)
+ Github 설명 : [소개_국문](http://kr.discovermeteor.com/chapters/github/) / [책_영문](https://git-scm.com/book/en/v2)

개발 언어는 당분간 Python, C/C++만 허용합니다. 장치간 메세지 교환방식에 대하여 결정이 된다면 언어 제한이 풀릴 수 있습니다.

+ 개인 브랜치 생성해서 작업하세요. 작업 후 Master에 Merge요청 하시면 됩니다.

아두이노를 Visual Studio IDE이외에 이클립스로 사용하는 방법을 시간내서 문서화 작업할겁니다.
+ 아두이노_이클립스연동 : [링크](http://playground.arduino.cc/Code/Eclipse)

자료 공유는 Dropbox를 사용하고 있습니다. 단, 이 곳에는 소스코드가 있지 않습니다.
+ 작업 기록 자료실 : [링크](https://www.dropbox.com/sh/7vmnmwyq8xp75xb/AADxzAP9nBhgIPdVGKtEdNQ4a?dl=0)

이전에 작업된 샘플코드들은 저작권이 있는 자료입니다. 하위 목록은 각 파트 대표 개발자이며 프로젝트 이외의 사용에는 문의가 필요할 수 있습니다. 
+ 아두이노-LED 신호처리 : 정영주(vertex50@cu.ac.kr), 대구가톨릭대학교
+ 아두이노-광수신 신호처리 : 최홍준(ghdwns7121@naver.com), 대구가톨릭대학교
+ 아두이노-PC 신호처리 : 정세연(best0937@naver.com), 대구가톨릭대학교
+ PC-웹캠 영상처리 : 박재성(sharkel@naver.com), 대구가톨릭대학교 
* 현재 등재된 버전의 PC-웹캠 영상처리 프로그램 소스의 실 소유주는 아니므로 이 프로그램을 다른 목적으로 사용하는 경우 컴퓨터 프로그램 보호법에 위배됩니다. 웹캠 프로그램 담당자 : 안광은(yooer10@cu.ac.kr)

프로젝트가 목적에 따라 레포지토리 기준으로 분류되었습니다.
