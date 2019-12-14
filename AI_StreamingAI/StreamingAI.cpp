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

#define       deviceDescription  L"MIC-1810,BID#15"
const wchar_t* profilePath = L"../../profile/DemoDevice.xml";

int32         startChannel = 0;
//-----------------------------------------------------------------------------------
// Configure the following parameters before running the demo
//-----------------------------------------------------------------------------------

const int32   channelCount = 4;
const int32   sectionLength = 20000; 
const int32   sectionCount = 0; 
const int32   clockRate = 200000;

int secPerShot = 2; // Set how many SECS per SHOT
int downsamplingNum = 2500; //Set the downsampling data count for gragh

//-----------------------------------------------------------------------------------
// Configure the above parameters before running the demo
//-----------------------------------------------------------------------------------

int channelAmount = startChannel + channelCount; // the total number of channels
int insertCount = 0;
int terminatorCount = 0;
float terminator = 999;

#define       USER_BUFFER_SIZE   channelCount * sectionLength
#define       MAX_CACHE_SIZE channelCount * sectionLength * 200
int         maxCacheSize = channelCount * sectionLength * 200;

double userDataBuffer[USER_BUFFER_SIZE];
float  floatSensorValueCacheData[channelCount][MAX_CACHE_SIZE];

int intCacheHeadIndex = -1;
int intCacheTailIndex = -1;

string       timestampQueue[channelCount][1000000];
int timestampHeadIndex = -1;
int timestampTailIndex = -1;

// 判斷circleQueue是否已滿
bool isFull()
{
    if( (intCacheHeadIndex == intCacheTailIndex + 1) || (intCacheHeadIndex == 0 && intCacheTailIndex == maxCacheSize - 1)) return true;
    return false;
}

// 判斷circleQueue是否為空
bool isEmpty()
{
    if(intCacheHeadIndex == intCacheTailIndex) return true;
    return false;
}


//將float type的數值轉換為十六進位的字串
string float2hexstr(float a)
{
  int* q = (int*)&a;
  stringstream my_ss;
  my_ss << hex << *q;
  string res1 = "0x" + my_ss.str() ;
  return res1;
}



// 執行緒:將circleQueue的資料存入Database
void* WriteSensorCachetoDatabase(void* data)
{
  MYSQL* conn;
  conn = mysql_init(0);
  conn = mysql_real_connect(conn, "127.0.0.1", "root2", "rootroot", "forging", 0, NULL, 0);
  sleep(1);


  string SQLinsertDB;
  string tmpGragh;
  string tmpValue;
  float sampleValue;
  float max = 0, min = 0, avg = 0;
  int deQueueCount = 0;
  bool firstDeQueueFlag = true;
  int intialCacheHeadIndex = 0;
  int postDecacheHeadIndex = 0;
  int GraghCount = 0;


    do
    {
<<<<<<< Updated upstream

      for(int i = startChannel; i < channelNum; i++)
=======
      // 將各個通道存在Queue的值串接成SQL指令, 各別存入進去各通道之table
      for(int i = startChannel; i < channelAmount; i++)
>>>>>>> Stashed changes
      {
        //
        SQLinsertDB = "INSERT INTO customSenorSN1_TableSN" + to_string((i + 1)) + " (col, shotMax, shotMin, shotAvg, TimeStamp, GraghData) VALUES ('";
        tmpGragh = " "; 

        while(1)
        {
          if(isEmpty())
          {
            if(sampleValue == terminator) sampleValue = -1;
            sleep(0.25);
          }
          else
          {
              // 取出存放在Queue裡面的值
              sampleValue = floatSensorValueCacheData[i][intCacheHeadIndex];
              intCacheHeadIndex = (intCacheHeadIndex + 1) % maxCacheSize;
              deQueueCount++;
          }

          // 若接收到shot的終止flag,則跳出迴圈進行insert資料庫的動作
          if(sampleValue == terminator)
          {
            break;
          }

          // 將資料轉換為十六進位的字串
          tmpValue = float2hexstr(sampleValue);
          SQLinsertDB += (tmpValue + " ");
          avg += sampleValue;

          // 判斷最大值, 最小值
          if(firstDeQueueFlag)
          {
            max = sampleValue;
            min = max;
            firstDeQueueFlag = false;
          }
          else
          {
            if(sampleValue > max)
            {
              max = sampleValue;
            }
            else if(sampleValue < min)
            {
              min = sampleValue;
            }
          }

          // 串接繪圖用的字串
          GraghCount++;
          if(( GraghCount % downsamplingNum ) == 0 )
          {
            tmpGragh += (to_string(sampleValue) + " ");
          }
        }

        // 計算平均值，串接insert指令
        // insert data至資料庫
        if((sampleValue == terminator) && (sampleValue != -1))
        {

          if(timestampHeadIndex < timestampTailIndex)
          {
            avg = avg / float((deQueueCount - 1));

            SQLinsertDB += "', '" + to_string(max) + "', '" + to_string(min) + "', '" + to_string(avg) + "', '" + timestampQueue[i][++timestampHeadIndex] + "', '" + tmpGragh + "')"; //   " + tmpGragh + "

            mysql_query(conn, SQLinsertDB.c_str());
            insertCount++;
          }
          if(channelAmount != 1) postDecacheHeadIndex = intCacheHeadIndex;
        }

        if(channelAmount != 1) intCacheHeadIndex = intialCacheHeadIndex;
        firstDeQueueFlag = true;
        max = 0;
        min = 0;
        avg = 0;
        deQueueCount = 0;
      }
      if(channelAmount != 1) intialCacheHeadIndex = postDecacheHeadIndex;
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

 /* Obtain the time of day, and convert it to a tm struct. */
 gettimeofday (&tv, NULL);
 ptm = localtime (&tv.tv_sec);

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
  // printf("%d\n",getDataCount);


  // 將各通道存在userDataBuffer的值放入Queue
    for(int32 i = 0; i < USER_BUFFER_SIZE; i += channelCount)
    {
      if(!isFull())
      {
        if(intCacheHeadIndex == -1) intCacheHeadIndex = 0;
        intCacheTailIndex = (intCacheTailIndex + 1) % maxCacheSize;
        
        for(int j = startChannel; j < channelAmount; j++)
        {
          // Shot開始時，將第一筆資料取得時間放入timestampQueue
          if(terminatorCount == 0)
          {
            timestampQueue[j][++timestampTailIndex] = getTime();
          }

          floatSensorValueCacheData[j][intCacheTailIndex] = userDataBuffer[i + (j - startChannel)];
        }
      }

      terminatorCount++;

      // Shot結束時，在Queue存入一個terminator，作為Shot結束的flag
      if(terminatorCount == ((clockRate * secPerShot) / channelCount))
      {
        intCacheTailIndex = (intCacheTailIndex + 1) % maxCacheSize;
        for(int j = startChannel; j < channelAmount; j++)
        {
          floatSensorValueCacheData[j][intCacheTailIndex] = terminator;
        }

        terminatorCount = 0;
      }
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

   // input a Terminater for programing stop
   intCacheTailIndex = (intCacheTailIndex + 1) % maxCacheSize;
   for(int i = startChannel; i < channelAmount; i++)
   {
     floatSensorValueCacheData[i][intCacheTailIndex] = terminator;
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
      pthread_t threadWriteSensorCachetoDatabase; // 宣告 pthread 變數
      pthread_create(&threadWriteSensorCachetoDatabase, NULL, WriteSensorCachetoDatabase, NULL); // 建立子執行緒

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
