
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

	Rect setNgetRectangle(){//rectangle ��ȯ
		return rect(x, y, width, height);
	}

	int getX() { return x; }//x��ǥ ��ȯ
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
track : 1��° Ʈ�� �簢 ������
track2 : 2��° Ʈ�� �簢 ������
image : ķ�̹���
roi_1 : ù���� ROI
roi_2 : �ι�° ROI
hsv : image �� hsv�� ��ȯ ���� ��*/

int main()
{
	Rect track, track2,track3,track4,track5, track6, track7, track8, track9, track10;
	Mat image, roi_1,roi_2,roi_3,hsv;
	RotatedRect trackBox, trackBox2;
	int choose = 1;
	int x = 0, y = 0, x1 = 0, y1 = 0, flag = 0, ll = 0;
	Point2f point;
	bool tracking = false;

	VideoCapture cap(1);
	cap.set(CAP_PROP_FRAME_WIDTH, 640);
	cap.set(CAP_PROP_FRAME_HEIGHT, 320);
	
	/*
	Rect�� ���� 4���� ���
	first : ���� (��ǥ 0,0, ���� 10, ���� 620)
	second: ������
	third  : ����
	fouth : �Ʒ���
	*/
	Noname first(0, 0, 10, 620);
	Noname second(600, 0, 10, 620);
	Noname third(0, 0, 620, 10);
	Noname fouth(0, 600, 620, 10);	

	if (!cap.isOpened()) {
		cout << " ��ķ�� �������� �ʽ��ϴ�" << endl;
		return -1;
	}

	/*
	Ʈ��ŷ ��ġ �ʱ�ȭ
	1,2�� ���ʿ��� ����
	3,4�� ������
	5,6 �� ����
	7,8 �� �Ʒ�
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
	���� case 1���� �����۵�
	case 2,3,4�� 1���� �����ϰ� �۵��Ͽ� �Ǵ��� ���� �Ұ�
	�����ʿ�
	*/
	for (;;) {
		cap.read(image);
		cvtColor(image, hsv, COLOR_BGR2HSV);
		if (image.empty())
		{
			cout << "Could not open or find the image" << std::endl;
			return -1;
		}
		/*
		roi �̹��� �����´�
		*/
		roi_1 = Backproj(hsv, calc_HSV_Hist("ROI.jpg"));
		roi_2 = Backproj(hsv, calc_HSV_Hist("ROI2.jpg"));

		switch (choose) {
		case 1://1�� ��ġ���� ���� ������ ���
			if (tracking == true)
			{
				
				trackBox = CamShift(roi_1, track,
					TermCriteria(TermCriteria::EPS | TermCriteria::COUNT, 10, 1));

				trackBox2 = CamShift(roi_2, track2,
					TermCriteria(TermCriteria::EPS | TermCriteria::COUNT, 10, 1));
				/*
				cout << "ROI1�� x : : " << track.x << " |||  ROI1�� y : : " << track.y << endl;
				cout << "ROI1�� width : : " << track.width << " |||  ROI1�� y : : " << track.height << endl;
				
				cout << "ROI2�� width : : " << track2.width << " |||  ROI2�� y : : " << track2.height << endl;
				*/
				if (( track.width > 10 && track.width < 70 ) && ( track.height < 80 && track.height > 20 )) {//��Ȯ�� Ʈ��ŷ�� �Ǵ� ���
					ellipse(image, trackBox, Scalar(0, 255, 0), 5, LINE_AA);//�׸�
					x = trackBox.boundingRect().x;
					y = trackBox.boundingRect().y;
					if (flag == 0) {
						cout << "1�� ��ġ���� �������� ���� �Ǿ����ϴ�." << endl;
						x1 = trackBox.boundingRect().x;
						y1 = trackBox.boundingRect().y;
						flag = 1;
					}
					if (ll == 1) {
						cv::putText(image, "Warning", Point(50, 100), FONT_HERSHEY_COMPLEX, 2, Scalar(0, 0, 255), 3);
					}

					if (-500 > x1 - trackBox.boundingRect().x || x1 - trackBox.boundingRect().x > 500) { //�����ڰ� �� �ǳԴ��� �Ǵ� ��ǥ
						cout << "�����ڰ� ���� �� �ǳԽ��ϴ�~~~~~~~~~~~`" << endl;
						flag = 0;
						tracking = false;
					}
				}
				else {//Ʈ��ŷ�� ���� �ʴ� ���
					cout << "ROI1�� ��ü�� ��������ϴ�!! ��Ž���� �����մϴ�" << endl;
					cv::putText(image, "ROI 1 is gone.", Point(10, 80), FONT_HERSHEY_COMPLEX, 1, Scalar(0, 0, 255), 3);

					if (-50 > x - trackBox.boundingRect().x || x - trackBox.boundingRect().x > 50) {//�ް��� ��ȭ�� �����ϴ� ��� x��ǥ�� ���̰� 50�̻� ���� ��� ���� �ܰ�� �Ĺ�
						cout << "�����ڰ˻��� ..." << endl;
						if (track.x = 0 || track.y == 0) {
							track.x = x;
							track.y = y;
						}
						else {
							track.x = x;
							track.y = y;
						}
					}
				}
				/*
				if (track2.width < 35 || track2.width >100) {
					cout << "ROI2�� ��ü�� ��������ϴ�!! ��Ž���� �����մϴ�" << endl;
					cv::putText(image, "ROI 2 is gone.", Point(10, 100), FONT_HERSHEY_COMPLEX, 1, Scalar(0, 0, 255), 3);
				}
				else {
					ellipse(image, trackBox2, Scalar(0, 0, 255), 5, LINE_AA);//����
				}
				*/
			}
			break;
		case 2://2��° ���� ��ġ ���̽�
			if (tracking == true)
			{

				trackBox = CamShift(roi_1, track,
					TermCriteria(TermCriteria::EPS | TermCriteria::COUNT, 10, 1));

				trackBox2 = CamShift(roi_2, track2,
					TermCriteria(TermCriteria::EPS | TermCriteria::COUNT, 10, 1));
				/*
				cout << "ROI1�� width : : " << track.width << " |||  ROI1�� y : : " << trackBox.boundingRect().x << endl;
				cout << "ROI2�� width : : " << track2.width << " |||  ROI2�� y : : " << track2.height << endl;
				*/
				if (track.width > 20 && track.width <70) {//�����ϴ� ũ�Ⱑ 20�̻� 70������ ��� ���� �������� �Ǵ��Ѵ�
					ellipse(image, trackBox, Scalar(0, 255, 0), 5, LINE_AA);//�׸�
					x = trackBox.boundingRect().x;
					y = trackBox.boundingRect().y;
					if (flag == 0) {//ó�� �����Ǵ� ��ġ ����
						cout << "2�� ��ġ���� �������� ���� �Ǿ����ϴ�." << endl;
						x1 = trackBox.boundingRect().x;
						y1 = trackBox.boundingRect().y;
						flag = 1;
					}
				}
				else {//��ü�� �������� ���ϴ� ��� ����
					cout << "ROI1�� ��ü�� ��������ϴ�!! ��Ž���� �����մϴ�" << endl;
					cv::putText(image, "ROI 1 is gone.", Point(10, 80), FONT_HERSHEY_COMPLEX, 1, Scalar(0, 0, 255), 3);

					if (-50 > x - trackBox.boundingRect().x || x - trackBox.boundingRect().x > 50) {//�ް��� ��ȭ�� �����ϴ� ��� x��ǥ�� ���̰� 50�̻� ���� ��� ���� �ܰ�� �Ĺ�
						cout << "�����ڰ˻��� ..." << endl;
						if (track.x = 0 || track.y == 0) {//��ġ�� ã�� ���ϴ� ���(x �Ǵ� y ��ǥ�� 0���� ���� ���ϴ� ��� -> ���� ��ǥ�� ���� �ǵ�����.
							track.x = x;
							track.y = y;
						}
						else {//�� �־�� �Ǵ� �Ұ�
							track.x = x;
							track.y = y;
						}
					}

					if (-500 > x1 - trackBox.boundingRect().x || x1 - trackBox.boundingRect().x > 500) { //�����ڰ� �� �ǳԴ��� �Ǵ� ��ǥ
						cout << "�����ڰ� ���� �� �ǳԽ��ϴ�~~~~~~~~~~~`" << endl;
						flag = 0;//�ٰǳʴ� ��� Ʈ��ŷ�� ������ flag�� 0���� �����
						/*
						track.x = 0;
						track.y = 0;
						track.width = 10;
						track.height = 0;
						*/
						tracking = false;
					}
				}

				if (track2.width < 35 || track2.width >100) {//���� ���� x
					cout << "ROI2�� ��ü�� ��������ϴ�!! ��Ž���� �����մϴ�" << endl;
					cv::putText(image, "ROI 2 is gone.", Point(10, 100), FONT_HERSHEY_COMPLEX, 1, Scalar(0, 0, 255), 3);
				}
				else {
					ellipse(image, trackBox2, Scalar(0, 0, 255), 5, LINE_AA);//����
				}
			}
			break;
		case 3://3��° ��ġ���� ��ȣ �߻�
			if (tracking == true)
			{
				trackBox = CamShift(roi_1, track,
					TermCriteria(TermCriteria::EPS | TermCriteria::COUNT, 10, 1));

				trackBox2 = CamShift(roi_2, track2,
					TermCriteria(TermCriteria::EPS | TermCriteria::COUNT, 10, 1));
				/*
				cout << "ROI1�� width : : " << track.width << " |||  ROI1�� y : : " << trackBox.boundingRect().x << endl;
				cout << "ROI2�� width : : " << track2.width << " |||  ROI2�� y : : " << track2.height << endl;
				*/
				if (track.width > 20 && track.width <70) {
					ellipse(image, trackBox, Scalar(0, 255, 0), 5, LINE_AA);//�׸�
					x = trackBox.boundingRect().x;
					y = trackBox.boundingRect().y;
					if (flag == 0) {
						cout << "3�� ��ġ���� �������� ���� �Ǿ����ϴ�." << endl;
						x1 = trackBox.boundingRect().x;
						y1 = trackBox.boundingRect().y;
						flag = 1;
					}
				}
				else {
					cout << "ROI1�� ��ü�� ��������ϴ�!! ��Ž���� �����մϴ�" << endl;
					cv::putText(image, "ROI 1 is gone.", Point(10, 80), FONT_HERSHEY_COMPLEX, 1, Scalar(0, 0, 255), 3);

					if (-50 > x - trackBox.boundingRect().x || x - trackBox.boundingRect().x > 50) {//�ް��� ��ȭ�� �����ϴ� ��� x��ǥ�� ���̰� 50�̻� ���� ��� ���� �ܰ�� �Ĺ�
						cout << "�����ڰ˻��� ..." << endl;
						if (track.x = 0 || track.y == 0) {
							track.x = x;
							track.y = y;
						}
						else {
							track.x = x;
							track.y = y;
						}
					}

					if (-500 > x1 - trackBox.boundingRect().x || x1 - trackBox.boundingRect().x > 500) { //�����ڰ� �� �ǳԴ��� �Ǵ� ��ǥ
						cout << "�����ڰ� ���� �� �ǳԽ��ϴ�~~~~~~~~~~~`" << endl;
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
					cout << "ROI2�� ��ü�� ��������ϴ�!! ��Ž���� �����մϴ�" << endl;
					cv::putText(image, "ROI 2 is gone.", Point(10, 100), FONT_HERSHEY_COMPLEX, 1, Scalar(0, 0, 255), 3);
				}
				else {
					ellipse(image, trackBox2, Scalar(0, 0, 255), 5, LINE_AA);//����
				}
			}
			break;
		case 4: //4��° �������� ��ȣ�߻�
			if (tracking == true)
			{
				trackBox = CamShift(roi_1, track,
					TermCriteria(TermCriteria::EPS | TermCriteria::COUNT, 10, 1));

				trackBox2 = CamShift(roi_2, track2,
					TermCriteria(TermCriteria::EPS | TermCriteria::COUNT, 10, 1));
				/*
				cout << "ROI1�� width : : " << track.width << " |||  ROI1�� y : : " << trackBox.boundingRect().x << endl;
				cout << "ROI2�� width : : " << track2.width << " |||  ROI2�� y : : " << track2.height << endl;
				*/
				if (track.width > 20 && track.width <70) {
					ellipse(image, trackBox, Scalar(0, 255, 0), 5, LINE_AA);//�׸�
					x = trackBox.boundingRect().x;
					y = trackBox.boundingRect().y;
					if (flag == 0) {
						cout << "4�� ��ġ���� �������� ���� �Ǿ����ϴ�." << endl;
						x1 = trackBox.boundingRect().x;
						y1 = trackBox.boundingRect().y;
						flag = 1;
					}
				}
				else {
					cout << "ROI1�� ��ü�� ��������ϴ�!! ��Ž���� �����մϴ�" << endl;
					cv::putText(image, "ROI 1 is gone.", Point(10, 80), FONT_HERSHEY_COMPLEX, 1, Scalar(0, 0, 255), 3);

					if (-50 > x - trackBox.boundingRect().x || x - trackBox.boundingRect().x > 50) {//�ް��� ��ȭ�� �����ϴ� ��� x��ǥ�� ���̰� 50�̻� ���� ��� ���� �ܰ�� �Ĺ�
						cout << "�����ڰ˻��� ..." << endl;
						if (track.x = 0 || track.y == 0) {
							track.x = x;
							track.y = y;
						}
						else {
							track.x = x;
							track.y = y;
						}
					}

					if (-500 > x1 - trackBox.boundingRect().x || x1 - trackBox.boundingRect().x > 500) { //�����ڰ� �� �ǳԴ��� �Ǵ� ��ǥ
						cout << "�����ڰ� ���� �� �ǳԽ��ϴ�~~~~~~~~~~~`" << endl;
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
					cout << "ROI2�� ��ü�� ��������ϴ�!! ��Ž���� �����մϴ�" << endl;
					cv::putText(image, "ROI 2 is gone.", Point(10, 100), FONT_HERSHEY_COMPLEX, 1, Scalar(0, 0, 255), 3);
				}
				else {
					ellipse(image, trackBox2, Scalar(0, 0, 255), 5, LINE_AA);//����
				}
			}
			break;
		}


		char ch = (char)waitKey(1);

		if (ch == 27)
			break;

		/*
		p : �Ͻ�����
		s : �����
		r : ����� �پ���(��������, �ڵ����� �ǰ� �� ����)
		t : ����� ����(��������, �ڵ����� �ǰ� �� ����)
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
		roi�̹��� �����ְ�
		��� �̹��� �����ش�.
		*/
		cv::imshow("roi_1", roi_1);
		cv::imshow("roi_2", roi_2);
		cv::imshow("result", image);
	}

	return 0;
}
