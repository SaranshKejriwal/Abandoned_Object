#include<opencv2/core/core.hpp>
#include<opencv2/highgui/highgui.hpp>
#include<opencv2/imgproc/imgproc.hpp>
#include<iostream>
#include<ctime>
#include<queue>

using namespace cv;
using namespace std;

void myGMM(Mat src,Mat bgsub,int npixel);
int main()
{
	Mat img,diff,back,aban(200,200,CV_8UC1,Scalar(0)),sub;
	VideoCapture cap("aban3.mp4");
	//VideoWriter outp;//to save a video
	//outp.open("aboutput.wmv",CV_FOURCC('M','P','4','2'),10,Size(160,160),false);//20 is fps
	
	int i=0,j=0;
	
	int count=0;//to count number of frames
	int npixel=100;//change this value to increase number of averaged pixels
	queue<int>colq;
	int col2[160][160];
	int sum2[160][160]={0};
	float avgq2[160][160]={0};
	queue<int>colq2[160][160];
	int flag=0;
	clock_t clk=clock()/CLOCKS_PER_SEC;//hold time in seconds b/w excecution and this statement
	for(;;)
	{
		cap>>img;
        imshow("pure input",img);
		      
        Rect r(0,65,img.cols,img.rows-65);
        img=img(r);
		resize(img,img,Size(160,160));
		
		
		count++;
		cvtColor(img,img,cv::COLOR_BGR2GRAY);
		//outp.write(img);
		diff=img.clone();
		absdiff(img,img,diff);
		back=img.clone();
		absdiff(back,back,back);
		
		//Implementation on whole image___________________________________________________________
		for(i=0;i<160;i++)
			{
				for(j=0;j<160;j++)
					{
						col2[i][j]=img.at<uchar>(Point(i,j));//save latest value
						colq2[i][j].push(col2[i][j]);//push latest value
						if(count<npixel)
							{
								sum2[i][j]=sum2[i][j]+col2[i][j];
								avgq2[i][j]=0;	//avg zero till queue is full
							}
						if(count>=npixel)//Background acquisition complete
							{ 
								colq2[i][j].pop();//take latest pixel
								sum2[i][j]=sum2[i][j]+col2[i][j]-colq2[i][j].front();//take sum of latest 150 pixels
								avgq2[i][j]=sum2[i][j]/npixel;
								
							}
							back.at<uchar>(Point(i,j))=avgq2[i][j];//get averaged background
							if(col2[i][j]-avgq2[i][j]>10 || col2[i][j]-avgq2[i][j]<(-10))
							{
								diff.at<uchar>(Point(i,j))=col2[i][j];// BG modelling step
							
							}

					}
			}

		if(count<npixel)
			{
				
				putText(back,"Please wait...",Point(10,30),FONT_HERSHEY_SIMPLEX,0.5,Scalar(255),1);
				putText(back,"Acquiring Background",Point(10,50),FONT_HERSHEY_SIMPLEX,0.4,Scalar(255),1);
			}
		//Background Subtraction ends here__________________________________________________
		
		int interval;
		//createTrackbar("Interval","Abandoned Objects",&interval,600);//max interval of 10 minutes
		if(flag==0)//initial block in verilog, with delay
			{
				 absdiff(back,back,aban);//initialize aban once only
				 interval=300; //to initialize trackbar position once at 60 seconds
				 flag=10 ;
			 }
	
		if(flag==10 && count>=npixel )//this loop runs once
			 {
				 aban=back.clone();//extra loop run to ensure stable initial background
				 flag =20;
			 }
		//cout<<"interval = "<<clock()/CLOCKS_PER_SEC-clk<<"\n";
		 if(clock()/CLOCKS_PER_SEC-clk>=interval )//interval can vary from 0 to 10 minutes; infinite loop
		{
			cout<<"New Interval \n";
			aban=back.clone();//not aban=back as they'll become pointers pointing to same address
			clk=clock()/CLOCKS_PER_SEC;
	    }
		 absdiff(back,aban,sub);
		//outp.write(sub);//to write on saved video


		resize(diff,diff,Size(400,400));
		resize(back,back,Size(400,400));
		resize(img,img,Size(400,400));
		resize(sub,sub,Size(400,400));
		moveWindow("img",0,0);
		moveWindow("back",400,0);
       		moveWindow("pure input",10,500);
		moveWindow("Abandoned Objects",800,0);
		imshow("img",img);
		imshow("back",back);
		imshow("Abandoned Objects",sub);
		char c=waitKey(10);
		if(c=='b')
		{break;}//break when 'b' is pressed
	
	}


	return 0;
}
