/*******************************************************************************
Copyright (c) 1983-2016 Advantech Co., Ltd.
********************************************************************************
THIS IS AN UNPUBLISHED WORK CONTAINING CONFIDENTIAL AND PROPRIETARY INFORMATION
WHICH IS THE PROPERTY OF ADVANTECH CORP., ANY DISCLOSURE, USE, OR REPRODUCTION,
WITHOUT WRITTEN AUTHORIZATION FROM ADVANTECH CORP., IS STRICTLY PROHIBITED.

================================================================================
REVISION HISTORY
--------------------------------------------------------------------------------
$Log:  $
--------------------------------------------------------------------------------
$NoKeywords:  $
*/
/******************************************************************************
*
* Windows Example:
*    StreamingAI.cpp
*
* Example Category:
*    AI
*
* Description:
*    This example demonstrates how to use Streaming AI function.
*
* Instructions for Running:
*    1. Set the 'deviceDescription' which can get from system device manager for opening the device.
*	   2. Set the 'profilePath' to save the profile path of being initialized device.
*    3. Set the 'startChannel' as the first channel for scan analog samples
*    4. Set the 'channelCount' to decide how many sequential channels to scan analog samples.
*    5. Set the 'sectionLength' as the length of data section for Buffered AI.
*	   6. Set the 'sectionCount' as the count of data section for Buffered AI.
*	   7. Set the 'clockRate' as the sample rate of data section for Buffered AI.
*
* I/O Connections Overview:
*    Please refer to your hardware reference manual.
*
******************************************************************************/
#include <stdlib.h>
#include <stdio.h>
#include "../inc/compatibility.h"
#include "../../../inc/bdaqctrl.h"
#include <mysql/mysql.h>
#include <sys/time.h>
#include <pthread.h>
#include <unistd.h>
#include <iostream>
#include <sstream>
#include <math.h>
using namespace Automation::BDaq;
using namespace std;
//-----------------------------------------------------------------------------------
// Configure the following parameters before running the demo
//-----------------------------------------------------------------------------------
#define       deviceDescription  L"MIC-1810,BID#15"
const wchar_t* profilePath = L"../../profile/DemoDevice.xml";

int32         startChannel = 0;
const int32   channelCount = 2;
const int32   sectionLength = 4000; //6500
const int32   sectionCount = 150000; //200000
const int32   clockRate = 100000;

int secPerShot = 2; // Set how many SECS per SHOT
int everyPoints = 2500; //Set the downsampling data count

//-----------------------------------------------------------------------------------
// Configure the above parameters before running the demo
//-----------------------------------------------------------------------------------

int channelNum = startChannel + channelCount; // the total number of channels
int insertCount = 0;
int terminatorCount = 0;
float terminator = 999;

#define       USER_BUFFER_SIZE   channelCount * sectionLength
#define       MAX channelCount * sectionLength * 20000

int QueueSize = channelCount * sectionLength * 20000;
double userDataBuffer[USER_BUFFER_SIZE];
float  circularQueue[channelCount][MAX];

int front = -1;
int rear = -1;

string       timestampQueue[channelCount][1000000];
int timestamp_front = -1;
int timestamp_rear = -1;

bool isFull()
{
    if( (front == rear + 1) || (front == 0 && rear == MAX - 1)) return true;
    return false;
}

bool isEmpty()
{
    if(front == rear) return true;
    return false;
}




string float2hexstr(float a)
{
  int* q = (int*)&a;
  stringstream my_ss;
  my_ss << hex << *q;
  string res1 = "0x" + my_ss.str() ;
  return res1;
}

//time
enum { NS_PER_SECOND = 1000000000 };
struct timespec start, finish, middle1, middle2, delta;
void sub_timespec(struct timespec t1, struct timespec t2, struct timespec *td)
{
    td->tv_nsec = t2.tv_nsec - t1.tv_nsec;
    td->tv_sec  = t2.tv_sec - t1.tv_sec;
    if (td->tv_sec > 0 && td->tv_nsec < 0)
    {
        td->tv_nsec += NS_PER_SECOND;
        td->tv_sec--;
    }
    else if (td->tv_sec < 0 && td->tv_nsec > 0)
    {
        td->tv_nsec -= NS_PER_SECOND;
        td->tv_sec++;
    }
}


void* child(void* data)
{
  MYSQL* conn;
  conn = mysql_init(0);
  conn = mysql_real_connect(conn, "127.0.0.1", "root2", "rootroot", "forging", 0, NULL, 0);
  sleep(1);

  int len = clockRate * secPerShot;
  string insertDB;
  string tmpGragh; //forGragh
  string tmp;
  float takeValue;
  float max = 0, min = 0, avg = 0;
  int deQueueCount = 0;
  bool check = true;
  int intialFrontIndex = 0;
  int postDeQueueFrontIndex = 0;
  int GraghCount = 0;


    do
    {

      for(int i = startChannel; i < channelNum; i++)
      {

        insertDB = "INSERT INTO customSenorSN1_TableSN" + to_string((i + 1)) + " (col, shotMax, shotMin, shotAvg, TimeStamp, GraghData) VALUES ('";
        tmpGragh = " "; //forGragh

        while(1)
        {
          if(isEmpty())
          {
            if(takeValue == 999) takeValue = -1;
            sleep(0.25);
          }
          else
          {
              takeValue = circularQueue[i][front];
              // cout << takeValue << endl;
              front = (front + 1) % QueueSize;
              deQueueCount++;
          }

          if(takeValue == terminator)
          {
            break;
          }


          tmp = float2hexstr(takeValue);
          insertDB += (tmp + " ");
          avg += takeValue;

          if(check)
          {
            max = takeValue;
            min = max;
            check = false;
          }
          else
          {
            if(takeValue > max)
            {
              max = takeValue;
            }
            else if(takeValue < min)
            {
              min = takeValue;
            }
          }

          // forGragh
          GraghCount++;
          if(( GraghCount % everyPoints ) == 0 )
          {
            tmpGragh += (to_string(takeValue) + " "); 
          }
        }

        // insert data
        if((takeValue == terminator) && (takeValue != -1))
        {

          if(timestamp_front < timestamp_rear)
          {
            avg = avg / float((deQueueCount - 1));

          // cout << max << "\t" << min << "\t" << avg << endl << ", deQueueCount = " << deQueueCount - 1 << endl;

            insertDB += "', '" + to_string(max) + "', '" + to_string(min) + "', '" + to_string(avg) + "', '" + timestampQueue[i][++timestamp_front] + "', '" + tmpGragh + "')"; //   " + tmpGragh + "
            
          // cout << "channel = " << i << ", front = " << timestamp_front << ", rear = " << timestamp_rear << endl;

            mysql_query(conn, insertDB.c_str());
            insertCount++;
            cout << "the " << insertCount << " mysql inserted.\n";
          }
          if(channelNum != 1) postDeQueueFrontIndex = front;
        }

        if(channelNum != 1) front = intialFrontIndex;
        check = true;
        max = 0;
        min = 0;
        avg = 0;
        deQueueCount = 0;
        // if(channelNum != 1) cout << "front = " << front << ", intial Front Index = " << intialFrontIndex << ", after DeQueue Front Index = " << postDeQueueFrontIndex << endl;
      }
      if(channelNum != 1) intialFrontIndex = postDeQueueFrontIndex;
   }while(1);


  // mysql_close(conn);

  pthread_exit(NULL); // 離開子執行緒
}


inline void waitAnyKey(){

   do{SLEEP(1);} while(!kbhit());

}

string getTime ()
{
 struct timeval tv;
 struct tm* ptm;
 char time_string[40];
 string milliseconds;
 string timestamp;
 double us = 0;
 double secs = 0;
 secs = tv.tv_sec;

 // printf("%f\n", secs);

 /* Obtain the time of day, and convert it to a tm struct. */
 gettimeofday (&tv, NULL);
 ptm = localtime (&tv.tv_sec);
 // cout << tv.tv_sec << endl;
 // cout << tv.tv_usec/1000000.0 << endl;

 /* Format the date and time, down to a single second. */
 strftime (time_string, sizeof (time_string), "%Y-%m-%d %H:%M:%S", ptm);

 /* Compute milliseconds from microseconds. */
 milliseconds = "." + to_string(tv.tv_usec);

 /* Print the formatted time, in seconds, followed by a decimal point
   and the milliseconds. */

 timestamp = time_string + milliseconds;
 return timestamp;
}

// This function is used to deal with 'DataReady' Event.
void BDAQCALL OnDataReadyEvent(void * sender, BfdAiEventArgs * args, void *userParam)
{
	int32 returnedCount = 0;
  WaveformAiCtrl * waveformAiCtrl = NULL;
  waveformAiCtrl = (WaveformAiCtrl *)sender;
  int32 getDataCount = MinValue(USER_BUFFER_SIZE, args->Count);
	waveformAiCtrl->GetData(getDataCount, userDataBuffer, 0, &returnedCount);
  // in this demo, we show only the first sample of each channel's new data
  // printf("the first sample for each Channel are:\n");
  // printf("%d\n",args->Count);

    for(int32 i = 0; i < USER_BUFFER_SIZE; i += channelCount)
    {
      // printf("i = %d\n",i);
      if(!isFull())
      {
        if(front == -1) front = 0;
        rear = (rear + 1) % QueueSize;
        for(int j = startChannel; j < channelNum; j++)
        {
          if(terminatorCount == 0)
          {
            timestampQueue[j][++timestamp_rear] = getTime();
            // cout << "timestamp_rear = " << timestamp_rear << endl;
          }
          circularQueue[j][rear] = userDataBuffer[i + (j - startChannel)];
          // printf("%f\n",circularQueue[j][rear]);
        }
      }
      // printf("InsertTime: %d:%d:%d %ld \n", tm->tm_hour, tm->tm_min, tm->tm_sec, tv.tv_usec);

      // enQueue all this time
      terminatorCount++;

      // input a Terminater in Queue
      if(terminatorCount == ((clockRate * secPerShot) / channelCount))
      {
        rear = (rear + 1) % QueueSize;
        for(int j = startChannel; j < channelNum; j++)
        {
          circularQueue[j][rear] = terminator;
        }

        terminatorCount = 0;
        // printf("Insert a terminator intot Queue %d\n",terminator);
      }
      // printf("%f\n", userDataBuffer[i] );
    }

    // THE SAMPLE CODE
    // printf("%d\n",returnedCount);
    // for(int32 i = 0; i < channelCount; ++i)
    // {
    //     // printf("channel %d:%10.6f \n",(i + startChannel), userDataBuffer[i]);
    // }
}


//The function is used to deal with 'OverRun' Event.
void BDAQCALL OnOverRunEvent(void * sender, BfdAiEventArgs * args, void *userParam)
{
   printf("Streaming AI Overrun: offset = %d, count = %d\n", args->Offset, args->Count);
}

//The function is used to deal with 'CacheOverflow' Event.
void BDAQCALL OnCacheOverflowEvent(void * sender, BfdAiEventArgs * args, void *userParam)
{
   printf("Streaming AI Cache Overflow: offset = %d, count = %d\n", args->Offset, args->Count);
}
//The function is used to deal with 'Stopped' Event.
void BDAQCALL OnStoppedEvent(void * sender, BfdAiEventArgs * args, void *userParam)
{
   printf("Streaming AI stopped: offset = %d, count = %d\n", args->Offset, args->Count);
   // printf("count = %d\n",count);

   // input a Terminater for programing stop
   rear = (rear + 1) % QueueSize;
   for(int i = startChannel; i < channelNum; i++)
   {
     circularQueue[i][rear] = terminator;
   }
}


int main(int argc, char* argv[])
{
  // printf("Start");
   ErrorCode        ret = Success;
   // Step 1: Create a 'WaveformAiCtrl' for buffered AI function.
   WaveformAiCtrl * wfAiCtrl = WaveformAiCtrl::Create();


	// Step 2: Set the notification event Handler by which we can known the state of operation effectively.
   wfAiCtrl->addDataReadyHandler(OnDataReadyEvent, NULL);
   wfAiCtrl->addOverrunHandler(OnOverRunEvent, NULL);
   wfAiCtrl->addCacheOverflowHandler(OnCacheOverflowEvent, NULL);
   wfAiCtrl->addStoppedHandler(OnStoppedEvent, NULL);
	do
	{
		 // Step 3: Select a device by device number or device description and specify the access mode.
		 // in this example we use ModeWrite mode so that we can fully control the device, including configuring, sampling, etc.
		  DeviceInformation devInfo(deviceDescription);
		  ret = wfAiCtrl->setSelectedDevice(devInfo);
		  CHK_RESULT(ret);
      ret = wfAiCtrl->LoadProfile(profilePath);//Loads a profile to initialize the device.
      CHK_RESULT(ret);

		  // Step 4: Set necessary parameters.
      Conversion * conversion = wfAiCtrl->getConversion();
      ret = conversion->setChannelStart(startChannel);
      CHK_RESULT(ret);
      ret = conversion->setChannelCount(channelCount);
      CHK_RESULT(ret);
      ret = conversion->setClockRate(clockRate);
      CHK_RESULT(ret);
		  Record * record = wfAiCtrl->getRecord();
      ret = record->setSectionCount(sectionCount);//The 0 means setting 'streaming' mode.
      CHK_RESULT(ret);
      ret = record->setSectionLength(sectionLength);
      CHK_RESULT(ret);

      // Step 5: The operation has been started.
      ret = wfAiCtrl->Prepare();
		  CHK_RESULT(ret);
		  ret = wfAiCtrl->Start();
      CHK_RESULT(ret);


      // Step 6: The device is acquiring data.
      printf("Streaming AI is in progress.\nplease wait...  any key to quit!\n\n");
      pthread_t t; // 宣告 pthread 變數
      pthread_create(&t, NULL, child, NULL); // 建立子執行緒

      // you can code something Here


      do
      {
        SLEEP(1);
      }	while(!kbhit());

      // step 7: Stop the operation if it is running.
      ret = wfAiCtrl->Stop();
      CHK_RESULT(ret);

   }while(false);


   // Step 8: Close device, release any allocated resource.
   wfAiCtrl->Dispose();

	 // If something wrong in this execution, print the error code on screen for tracking.
   if(BioFailed(ret))
   {
      wchar_t enumString[256];
      AdxEnumToString(L"ErrorCode", (int32)ret, 256, enumString);
      printf("Some error occurred. And the last error code is 0x%X. [%ls]\n", ret, enumString);
      waitAnyKey();// wait any key to quit!
   }
   return 0;
}

// all:
// 	g++ -o StreamingAI StreamingAI.cpp -lbiodaq -L/usr/include/mysql -lmysqlclient -I/usr/include/mysql -lpthread
// clean:
// 	rm StreamingAI
