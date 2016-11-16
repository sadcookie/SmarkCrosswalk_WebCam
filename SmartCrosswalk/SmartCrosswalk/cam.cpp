
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/video/tracking.hpp>

#include <iostream>
#include <string>

using namespace cv;
using namespace std;

//int histSize[] = { 256,2 };
int histSize[] = { 64,64 };
int channels[] = { 0,1 };
float range[] = { 0, 255 };
const float* ranges[] = { range , range};

class Noname {
private:
	int x, y, width, height;
	Rect rect(int x,int y,int width,int height);
public:
	Noname(int x, int y, int width, int height){
		this->x = x;
		this->y = y;
		this->width = width;
		this->height = height;
	}

	Rect setNgetRectangle(){//rectangle 반환
		return rect(x, y, width, height);
	}

	int getX() { return x; }//x좌표 반환
	int getY() { return y; }//y
	int getWidth(){ return width; }//width
	int getheight() { return height; }//height
};


Mat calc_HSV_Hist(String image_path) {

	Mat hist, image, hsv;
	
	image = imread(image_path);
	cvtColor(image, hsv, COLOR_BGR2HSV);
	
	calcHist(&hsv, 1, channels, Mat(), hist, 2, histSize, ranges, true, false);
	normalize(hist, hist, 0, 255, NORM_MINMAX, -1, Mat());

	namedWindow(image_path, CV_WINDOW_NORMAL);
	cv::imshow(image_path,image);

	return hist;
}

Mat Backproj(Mat background, Mat ROI_hist)
{
	/// Get Backprojection
	MatND backproj;
	calcBackProject(&background,1, channels, ROI_hist, backproj, ranges,1,true);
	normalize(backproj, backproj, 0, 255, NORM_MINMAX);
	erode(backproj, backproj, getStructuringElement(MORPH_ELLIPSE, Size(2, 2)));
	dilate(backproj, backproj, getStructuringElement(MORPH_ELLIPSE, Size(2, 2)));
	
	return backproj;
}


/*
track : 1번째 트랙 사각 윈도우
track2 : 2번째 트랙 사각 윈도우
image : 캠이미지
roi_1 : 첫번재 ROI
roi_2 : 두번째 ROI
hsv : image 를 hsv로 변환 저장 값*/

int main()
{
	Rect track, track2,track3,track4,track5, track6, track7, track8, track9, track10;
	Mat image, roi_1,roi_2,roi_3,hsv;
	Mat result;
	RotatedRect trackBox, trackBox2;
	int choose = 1;//
	int tracking_buffer[15] = { 0, };//멈춤을 판단하는 버퍼
	int x = 0, y = 0, x1 = 0, y1 = 0, flag = 0, ll = 0;
	Point2f point;
	bool tracking = false;

	result = imread("result.png");
	cv::namedWindow("보행자 상태",CV_WINDOW_NORMAL);
	cv::resizeWindow("보행자 상태", 400, 100);

	VideoCapture cap(0);
	cap.set(CAP_PROP_FRAME_WIDTH, 640);
	cap.set(CAP_PROP_FRAME_HEIGHT, 320);
	
	/*
	Rect을 위해 4가지 경우
	first : 왼쪽 (좌표 0,0, 길이 10, 높이 620)
	second: 오른쪽
	third  : 위쪽
	fouth : 아래쪽
	*/
	Noname first(0, 0, 10, 620);
	Noname second(600, 0, 10, 620);
	Noname third(0, 0, 620, 10);
	Noname fouth(0, 600, 620, 10);	

	if (!cap.isOpened()) {
		cout << " 웹캠이 열려있지 않습니다" << endl;
		return -1;
	}

	/*
	트랙킹 위치 초기화
	1,2는 왼쪽에서 시작
	3,4는 오른쪽
	5,6 은 위쪽
	7,8 은 아래
	*/
	track.x = first.getX();
	track.y = first.getY();
	track.width = first.getWidth();
	track.height = first.getheight();
	track2.x = first.getX();
	track2.y = first.getY();
	track2.width = first.getWidth();
	track2.height = first.getheight();
	track3.x = second.getX();
	track3.y = second.getY();
	track3.width = second.getWidth();
	track3.height = second.getheight();
	track4.x = second.getX();
	track4.y = second.getY();
	track4.width = second.getWidth();
	track4.height = second.getheight();
	track5.x = third.getX();
	track5.y = third.getY();
	track5.width = third.getWidth();
	track5.height = third.getheight();
	track6.x = third.getX();
	track6.y = third.getY();
	track6.width = third.getWidth();
	track6.height = third.getheight();
	track7.x = fouth.getX();
	track7.y = fouth.getY();
	track7.width = fouth.getWidth();
	track7.height = fouth.getheight();
	track8.x = fouth.getX();
	track8.y = fouth.getY();
	track8.width = fouth.getWidth();
	track8.height = fouth.getheight();

	/*
	현재 case 1번만 정상작동
	case 2,3,4는 1번과 동일하게 작동하여 판단이 쉽게 불가
	수정필요
	*/

	int buffer_count = 0;
	for (;;) {
		cap.read(image);
		cvtColor(image, hsv, COLOR_BGR2HSV);
		if (image.empty())
		{
			cout << "Could not open or find the image" << std::endl;
			return -1;
		}
		/*
		roi 이미지 가져온다
		*/
		roi_1 = Backproj(hsv, calc_HSV_Hist("ROI_jyj.jpg"));
		roi_2 = Backproj(hsv, calc_HSV_Hist("ROI2.jpg"));

		switch (choose) {
		case 1://1번 위치에서 값이 들어왔을 경우
			if (tracking == true)
			{
				/*수정 필요함 */
				trackBox = CamShift(roi_1, track,
					TermCriteria(TermCriteria::EPS | TermCriteria::COUNT, 10, 1));

				trackBox2 = CamShift(roi_2, track2,
					TermCriteria(TermCriteria::EPS | TermCriteria::COUNT, 10, 1));
				/*
				cout << "ROI1의 x : : " << track.x << " |||  ROI1의 y : : " << track.y << endl;
				cout << "ROI1의 width : : " << track.width << " |||  ROI1의 y : : " << track.height << endl;
				
				cout << "ROI2의 width : : " << track2.width << " |||  ROI2의 y : : " << track2.height << endl;
				*/
				if (( track.width > 10 && track.width < 70 ) && ( track.height < 80 && track.height > 20 )) {//정확히 트랙킹이 되는 경우
					ellipse(image, trackBox, Scalar(0, 255, 0), 5, LINE_AA);//그린
					x = trackBox.boundingRect().x;
					y = trackBox.boundingRect().y;
					if (flag == 0) {
						cout << "1번 위치에서 움직임이 감지 되었습니다." << endl;
						x1 = trackBox.boundingRect().x;
						y1 = trackBox.boundingRect().y;
						flag = 1;
					}
					//필요 없는 부분 곧 수동이 아닌 코드 내부에서 자동적으로 돌아가야함
					if (ll == 1) {
						result = imread("result.png");
						cv::putText(result, "Walking..", Point(50, 300), FONT_HERSHEY_COMPLEX, 10, Scalar(0, 0, 255), 7);
					}
					else if (ll == 0) {
						result = imread("result.png");
						cv::putText(result, "Stop..", Point(50, 300), FONT_HERSHEY_COMPLEX, 10, Scalar(0, 0, 255), 7);
					}
					
					if (-600 > x1 - trackBox.boundingRect().x || x1 - trackBox.boundingRect().x > 650) { //보행자가 다 건넜는지 판단 지표
						cout << "보행자가 길을 다 건넜습니다~~~~~~~~~~~`" << endl;
						flag = 0;
						tracking = false;
					}
					tracking_buffer[buffer_count] = trackBox.boundingRect().x;
				}

				else {//트랙킹이 되지 않는 경우
					cout << "ROI1의 객체가 사라졌습니다!! 재탐색을 시작합니다" << endl;
					cv::putText(image, "ROI 1 is gone.", Point(10, 80), FONT_HERSHEY_COMPLEX, 1, Scalar(0, 0, 255), 3);
					//ellipse(image, trackBox, Scalar(0, 255, 0), 5, LINE_AA);//그린, 보행자가 사라져도 원형을 그리면서 

					if (-50 > x - trackBox.boundingRect().x || x - trackBox.boundingRect().x > 50) {//급격한 변화가 존재하는 경우 x좌표의 차이가 50이상 나는 경우 이전 단계로 컴백
						cout << "보행자검색중 ..." << endl;
						for (int i = 0; i < 10; i++) {
							if (track.x = 0 || track.y == 0) {
								track.x = x;
								track.y = y;
							}
							else {
								track.x = x;
								track.y = y;
							}
						}//for
					}//보행자 검색 if 문
				}

				buffer_count += 1;

				if (buffer_count == 9) { //버퍼 카운터가 9이면
					if(-5>tracking_buffer[0] - tracking_buffer[9] || tracking_buffer[0] - tracking_buffer[9] < 5);//첫번째와 마지막 번지의 위치 차이가 없을시
					//cv::putText(result, "Stop", Point(350, 300), FONT_HERSHEY_COMPLEX, 10, Scalar(0, 0, 255), 6);//보행자가 멈춤을 확인시켜 준다.	

					for (int i = 0; i < 10; i++) {//버퍼 0에서 9까지 초기화 시킴
						tracking_buffer[i] = 0;//버퍼 0으로 초기화
					}
					buffer_count = 0;//버퍼 카운터 초기화

				}
				/* 객체 2번 추적
				if (track2.width < 35 || track2.width >100) {
					cout << "ROI2의 객체가 사라졌습니다!! 재탐색을 시작합니다" << endl;
					cv::putText(image, "ROI 2 is gone.", Point(10, 100), FONT_HERSHEY_COMPLEX, 1, Scalar(0, 0, 255), 3);
				}
				else {
					ellipse(image, trackBox2, Scalar(0, 0, 255), 5, LINE_AA);//레드
				}
				*/
			}
			break;
		case 2://2번째 센서 위치 케이스
			if (tracking == true)
			{

				trackBox = CamShift(roi_1, track,
					TermCriteria(TermCriteria::EPS | TermCriteria::COUNT, 10, 1));

				trackBox2 = CamShift(roi_2, track2,
					TermCriteria(TermCriteria::EPS | TermCriteria::COUNT, 10, 1));
				/*
				cout << "ROI1의 width : : " << track.width << " |||  ROI1의 y : : " << trackBox.boundingRect().x << endl;
				cout << "ROI2의 width : : " << track2.width << " |||  ROI2의 y : : " << track2.height << endl;
				*/
				if (track.width > 20 && track.width <70) {//추적하는 크기가 20이상 70이하인 경우 정상 추적으로 판단한다
					ellipse(image, trackBox, Scalar(0, 255, 0), 5, LINE_AA);//그린
					x = trackBox.boundingRect().x;
					y = trackBox.boundingRect().y;
					if (flag == 0) {//처음 추적되는 위치 저장
						cout << "2번 위치에서 움직임이 감지 되었습니다." << endl;
						x1 = trackBox.boundingRect().x;
						y1 = trackBox.boundingRect().y;
						flag = 1;
					}
				}
				else {//객체를 추적하지 못하는 경우 진입
					cout << "ROI1의 객체가 사라졌습니다!! 재탐색을 시작합니다" << endl;
					cv::putText(image, "ROI 1 is gone.", Point(10, 80), FONT_HERSHEY_COMPLEX, 1, Scalar(0, 0, 255), 3);

					if (-50 > x - trackBox.boundingRect().x || x - trackBox.boundingRect().x > 50) {//급격한 변화가 존재하는 경우 x좌표의 차이가 50이상 나는 경우 이전 단계로 컴백
						cout << "보행자검색중 ..." << endl;
						if (track.x = 0 || track.y == 0) {//위치를 찾지 못하는 경우(x 또는 y 좌표가 0으로 잡지 못하는 경우 -> 이전 좌표로 값을 되돌린다.
							track.x = x;
							track.y = y;
						}
						else {//왜 넣어쓴지 판단 불가
							track.x = x;
							track.y = y;
						}
					}

					if (-500 > x1 - trackBox.boundingRect().x || x1 - trackBox.boundingRect().x > 500) { //보행자가 다 건넜는지 판단 지표
						cout << "보행자가 길을 다 건넜습니다~~~~~~~~~~~`" << endl;
						flag = 0;//다건너는 경우 트렉킹을 끝내고 flag도 0으로 만든다
						/*
						track.x = 0;
						track.y = 0;
						track.width = 10;
						track.height = 0;
						*/
						tracking = false;
					}
				}

				if (track2.width < 35 || track2.width >100) {//아직 구현 x
					cout << "ROI2의 객체가 사라졌습니다!! 재탐색을 시작합니다" << endl;
					cv::putText(image, "ROI 2 is gone.", Point(10, 100), FONT_HERSHEY_COMPLEX, 1, Scalar(0, 0, 255), 3);
				}
				else {
					ellipse(image, trackBox2, Scalar(0, 0, 255), 5, LINE_AA);//레드
				}
			}
			break;
		case 3://3번째 위치에서 신호 발생
			if (tracking == true)
			{
				trackBox = CamShift(roi_1, track,
					TermCriteria(TermCriteria::EPS | TermCriteria::COUNT, 10, 1));

				trackBox2 = CamShift(roi_2, track2,
					TermCriteria(TermCriteria::EPS | TermCriteria::COUNT, 10, 1));
				/*
				cout << "ROI1의 width : : " << track.width << " |||  ROI1의 y : : " << trackBox.boundingRect().x << endl;
				cout << "ROI2의 width : : " << track2.width << " |||  ROI2의 y : : " << track2.height << endl;
				*/
				if (track.width > 20 && track.width <70) {
					ellipse(image, trackBox, Scalar(0, 255, 0), 5, LINE_AA);//그린
					x = trackBox.boundingRect().x;
					y = trackBox.boundingRect().y;
					if (flag == 0) {
						cout << "3번 위치에서 움직임이 감지 되었습니다." << endl;
						x1 = trackBox.boundingRect().x;
						y1 = trackBox.boundingRect().y;
						flag = 1;
					}
				}
				else {
					cout << "ROI1의 객체가 사라졌습니다!! 재탐색을 시작합니다" << endl;
					cv::putText(image, "ROI 1 is gone.", Point(10, 80), FONT_HERSHEY_COMPLEX, 1, Scalar(0, 0, 255), 3);

					if (-50 > x - trackBox.boundingRect().x || x - trackBox.boundingRect().x > 50) {//급격한 변화가 존재하는 경우 x좌표의 차이가 50이상 나는 경우 이전 단계로 컴백
						cout << "보행자검색중 ..." << endl;
						if (track.x = 0 || track.y == 0) {
							track.x = x;
							track.y = y;
						}
						else {
							track.x = x;
							track.y = y;
						}
					}

					if (-500 > x1 - trackBox.boundingRect().x || x1 - trackBox.boundingRect().x > 500) { //보행자가 다 건넜는지 판단 지표
						cout << "보행자가 길을 다 건넜습니다~~~~~~~~~~~`" << endl;
						flag = 0;
						/*
						track.x = 0;
						track.y = 0;
						track.width = 10;
						track.height = 0;
						*/
						tracking = false;
					}
				}

				if (track2.width < 35 || track2.width >100) {
					cout << "ROI2의 객체가 사라졌습니다!! 재탐색을 시작합니다" << endl;
					cv::putText(image, "ROI 2 is gone.", Point(10, 100), FONT_HERSHEY_COMPLEX, 1, Scalar(0, 0, 255), 3);
				}
				else {
					ellipse(image, trackBox2, Scalar(0, 0, 255), 5, LINE_AA);//레드
				}
			}
			break;
		case 4: //4번째 센서에서 신호발생
			if (tracking == true)
			{
				trackBox = CamShift(roi_1, track,
					TermCriteria(TermCriteria::EPS | TermCriteria::COUNT, 10, 1));

				trackBox2 = CamShift(roi_2, track2,
					TermCriteria(TermCriteria::EPS | TermCriteria::COUNT, 10, 1));
				/*
				cout << "ROI1의 width : : " << track.width << " |||  ROI1의 y : : " << trackBox.boundingRect().x << endl;
				cout << "ROI2의 width : : " << track2.width << " |||  ROI2의 y : : " << track2.height << endl;
				*/
				if (track.width > 20 && track.width <70) {
					ellipse(image, trackBox, Scalar(0, 255, 0), 5, LINE_AA);//그린
					x = trackBox.boundingRect().x;
					y = trackBox.boundingRect().y;
					if (flag == 0) {
						cout << "4번 위치에서 움직임이 감지 되었습니다." << endl;
						x1 = trackBox.boundingRect().x;
						y1 = trackBox.boundingRect().y;
						flag = 1;
					}
				}
				else {
					cout << "ROI1의 객체가 사라졌습니다!! 재탐색을 시작합니다" << endl;
					cv::putText(image, "ROI 1 is gone.", Point(10, 80), FONT_HERSHEY_COMPLEX, 1, Scalar(0, 0, 255), 3);

					if (-50 > x - trackBox.boundingRect().x || x - trackBox.boundingRect().x > 50) {//급격한 변화가 존재하는 경우 x좌표의 차이가 50이상 나는 경우 이전 단계로 컴백
						cout << "보행자검색중 ..." << endl;
						if (track.x = 0 || track.y == 0) {
							track.x = x;
							track.y = y;
						}
						else {
							track.x = x;
							track.y = y;
						}
					}

					if (-500 > x1 - trackBox.boundingRect().x || x1 - trackBox.boundingRect().x > 500) { //보행자가 다 건넜는지 판단 지표
						cout << "보행자가 길을 다 건넜습니다~~~~~~~~~~~`" << endl;
						flag = 0;
						/*
						track.x = 0;
						track.y = 0;
						track.width = 10;
						track.height = 0;
						*/
						tracking = false;
					}
				}

				if (track2.width < 35 || track2.width >100) {
					cout << "ROI2의 객체가 사라졌습니다!! 재탐색을 시작합니다" << endl;
					cv::putText(image, "ROI 2 is gone.", Point(10, 100), FONT_HERSHEY_COMPLEX, 1, Scalar(0, 0, 255), 3);
				}
				else {
					ellipse(image, trackBox2, Scalar(0, 0, 255), 5, LINE_AA);//레드
				}
			}
			break;
		}


		char ch = (char)waitKey(1);

		if (ch == 27)
			break;

		/*
		p : 일시정지
		s : 재시작& 시작 버튼
		r : 경고문구 뛰어줌(삭제예정, 자동으로 되게 할 예정)
		t : 경고문구 지움(삭제예정, 자동으로 되게 할 예정)
		*/
		switch (ch)
		{
		case 'p' :
			tracking = false;
			cout << "pause tracking " << endl;
			break;
		case 's' :
			tracking = true;
			cout << "restart tracking " << endl;
			break;
		case 'r':
			ll = 1;
			break;
		case 't':
			ll = 0;
			break;
		}
		/*
		roi이미지 보여주고
		결과 이미지 보여준다.
		*/
		cv::imshow("보행자 상태", result);
		cv::imshow("roi_1", roi_1);
		cv::imshow("roi_2", roi_2);
		cv::imshow("result", image);
	}

	return 0;
}
