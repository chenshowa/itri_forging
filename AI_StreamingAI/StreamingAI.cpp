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
*    StreamingAIwithTrigger.cpp
*
* Example Category:
*    AI
*
* Description:
*    This example demonstrates how to use Streaming AI with Trigger Delay to Start function.
*
* Instructions for Running:
*    1. Set the 'DEVICE_DESCRIPTION' for opening the device.
*	  2. Set the 'profilePath' to save the profile path of being initialized device. 
*    3. Set the 'START_CHANNEL' as the first channel for scan analog samples  
*    4. Set the 'CHANNEL_COUNT' to decide how many sequential channels to scan analog samples.
*    5. Set the 'SECTION_LENGTH' as the length of data section for Buffered AI.
*	  6. Set the 'SECTION_COUNT' as the count of data section for Buffered AI.
*    7. Set the 'trigger parameters' to decide trigger property.
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
#define       DEVICE_DESCRIPTION  L"MIC-1810,BID#15"
const wchar_t* profilePath = L"../../profile/DemoDevice.xml";
// 100ms -> 5000
// 50ms  -> 2500
// 40ms  -> 2000
// 30ms  -> 1500
// 20ms  -> 1000
// 10ms  ->  500

int intMaxInsertShot = 400;
int intDelayUSeconds = 20000;
#define START_CHANNEL  0    // startChannel
#define CHANNEL_COUNT  8
#define SECTION_LENGTH 45    //4000
#define SECTION_COUNT  0
#define CLOCK_RATE     5000    // specify sampling frequency of each channel
#define SEC_PER_SHOT 0.1        // Set max SECS per SHOT
#define DOWN_SAMPLING_NUM 100    //Set the downsampling data count for gragh
#define MAX_ENQUEUE_NUMBER CLOCK_RATE*SEC_PER_SHOT
#define USER_BUFFER_SIZE CHANNEL_COUNT*SECTION_LENGTH 
double  Data[USER_BUFFER_SIZE];

#define MYSQL_SERVER_IP "127.0.0.1"
#define MYSQL_USER_NAME "root"
#define MYSQL_PASSWORD "rootroot"
#define MYSQL_DB_NAME "forging"


// Set trigger parameters
TriggerAction triggerAction     = DelayToStart;
ActiveSignal  triggerEdge       = FallingEdge;
int           triggerDelayCount = 0;
double        triggerLevel      = 0.2;


//for trigger1 parameters 
TriggerAction trigger1Action     = DelayToStop;
ActiveSignal  trigger1Edge       = RisingEdge;
int           trigger1DelayCount = 0;
double        trigger1Level      = 0.2;


//-----------------------------------------------------------------------------------
// Configure the above parameters before running the demo
//-----------------------------------------------------------------------------------

bool boolTrigger = true;


#define TERMINATOR 999.0
#define END_CHANNEL START_CHANNEL + CHANNEL_COUNT
unsigned int intMax_ENQUEUE_NUMBER = (CLOCK_RATE*SEC_PER_SHOT) ;
unsigned int intEnQueueCount = 0;
unsigned int intNumOfShot = 0;
unsigned int intNumOfInsertShot;

char charErrorStatement[300];
int intStatus = 0;
bool boolEnQueueThreadErrorFlag = false;
bool boolDeQueueThreadErrorFlag = false;


#define MAX_QUEUE_SIZE CHANNEL_COUNT * SECTION_LENGTH * 5000  //2000
unsigned int  intMaxQueueSize = CHANNEL_COUNT * SECTION_LENGTH * 5000; //2000

// 存放取樣數值的Queue
float  floatSensorValueQueueData[CHANNEL_COUNT][MAX_QUEUE_SIZE];
int intQueueHeadIndex = -1;
int intQueueTailIndex = -1;
int intTempQueueHeadIndex = 0;
int intTheLastShotTailIndex = 0;

// 存放每一shot取樣時間的Queue
#define MAX_TIME_QUEUE_SIZE  10000
int intMaxTimeQueueSize = MAX_TIME_QUEUE_SIZE;
string stringTimeQueue[MAX_TIME_QUEUE_SIZE];
int intTimeQueueHeadIndex = -1;
int intTimeQueueTailIndex = -1;
int intTheLastTimeTailIndex = 0;


int geTimeCount()
{
   if(intTimeQueueTailIndex > intTimeQueueHeadIndex)
   {
      return (intTimeQueueTailIndex - intTimeQueueHeadIndex) +1;
   }
   else if (intTimeQueueTailIndex == intTimeQueueHeadIndex)
   {
      return 1;
   }
   else
   {
      return (intMaxTimeQueueSize - intTimeQueueHeadIndex + intTimeQueueTailIndex);
   }
}
// void C_getTime(char *a)
// {
// 	struct timeval tv;
// 	struct tm *ptm;
// 	char time_string[19];
// 	char milliseconds[6];

// 	/* Obtain the time of day, and convert it to a tm struct. */
// 	gettimeofday (&tv, NULL);
// 	ptm = localtime (&tv.tv_sec);
	
// 	/* Format the date and time, down to a single second. */
// 	strftime(time_string, 21, "%Y-%m-%d %H:%M:%S", ptm);
	
// 	snprintf(a, 27, "%s.%ld", time_string, tv.tv_usec);
   
//    // printf("%s",a);

// // use the method

// // char charDatatime[27];
// // C_getTime(charDatatime);
// }

string getTime()
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


   milliseconds = ".";
   for(int i=0;  i<(6 - to_string(tv.tv_usec).length()); i++)
   {
      milliseconds = milliseconds + '0';
   }
   milliseconds = milliseconds + to_string((tv.tv_usec));



   /* Print the formatted time, in seconds, followed by a decimal point
      and the milliseconds. */

   timestamp = time_string + milliseconds;

   return timestamp;
}

// void writeErrorLog(char *statement){   // C-code
void writeErrorLog(string statement){

   printf("%s", statement.c_str());

   // char charDatatime[27];
   // C_getTime(charDatatime);

   string charDatatime = getTime();


   FILE *f;
   f = fopen("error.log", "a+"); // a+ (create + append) option will allow appending which is useful in a log file
   if (f == NULL) { printf(" Open FILE error!!\n");/* Something is wrong   */}

   
   fprintf(f,"\n%s %s ", charDatatime.c_str(), statement.c_str());
   fprintf(f,"Num of shot waiting for insert: %d    left in Queue: %d    Time: %d   ", intNumOfInsertShot, intNumOfShot, geTimeCount()-1);

   fclose(f);

}




// 判斷ValueQueue是否已滿
bool QueueIsFull()
{
   if((intQueueHeadIndex == intQueueTailIndex + 1) || (intQueueHeadIndex == 0 && intQueueTailIndex == intMaxQueueSize - 1)) return true;
   return false;
}

// 判斷ValueQueue是否為空
bool QueueIsEmpty()
{
   if(intQueueHeadIndex == -1) return true;
   return false;
}

// 存入terminator旗標至每一個channel
void enValueQueue_TERMINATOR()
{

   if(QueueIsFull())
   {
      writeErrorLog("\n[enValueQueue_TERMINATOR] Fail to insert TERMINATOR into Queue while Queue is full \n");
   } 
   else
   {
      // if(intQueueHeadIndex == -1) intQueueHeadIndex = 0;
      intQueueTailIndex = (intQueueTailIndex + 1) % intMaxQueueSize;

      for(int channel = START_CHANNEL; channel < END_CHANNEL; channel++)
      {
         floatSensorValueQueueData[channel][intQueueTailIndex] = TERMINATOR;
      }
   }
}

// 釋放Queue的空間
float deValueQueue() 
{
   float element;
   if(QueueIsEmpty())
   {
      writeErrorLog("\n[deValueQueue] Fail to release Queue because Queue is Empty \n");
   }
   else
   {
      intQueueHeadIndex = intTempQueueHeadIndex ;

      if (intQueueHeadIndex == intQueueTailIndex)
      {
         intQueueHeadIndex = -1;
         intQueueTailIndex = -1;
         intTempQueueHeadIndex = 0;
      }
      
      return element;
   }
}



// 取出Queue的值
float getValueQueue(int channel)
{

   float element;
   if(QueueIsEmpty())  
   {
      writeErrorLog("\n [getValueQueue] Fail to get value from Queue while Queue is Empty \n");
   }
   else
   {
      element = floatSensorValueQueueData[channel][intTempQueueHeadIndex];

      intTempQueueHeadIndex = (intTempQueueHeadIndex + 1) % intMaxQueueSize;

      return element;
   }
}




// Operation of time Queue
bool TimeQueueIsFull()
{
    if((intTimeQueueHeadIndex == intTimeQueueTailIndex + 1) || (intTimeQueueHeadIndex == 0 && intTimeQueueTailIndex == intMaxTimeQueueSize - 1)) return true;
    return false;
}
bool TimeQueueIsEmpty()
{
    if(intTimeQueueHeadIndex  == -1 ) return true;
    return false;
}

void enTimeQueue( string element)
{
    if(TimeQueueIsFull()) 
    {
       writeErrorLog("\n [enTimeQueue] Time Queue is full!! \n");
    }
    else
    {
        if(intTimeQueueHeadIndex == -1) intTimeQueueHeadIndex = 0;
        intTimeQueueTailIndex = (intTimeQueueTailIndex + 1) % intMaxTimeQueueSize;
        stringTimeQueue[intTimeQueueTailIndex] = element;
        
    }
}

string deTimeQueue()
{
    string element;
    if(TimeQueueIsEmpty())
    {
       writeErrorLog("\n[deTimeQueue] Fail to deTimeQueue while Time Queue is Empty \n");
      return "-1";
    }
    else
    {
        element = stringTimeQueue[intTimeQueueHeadIndex];
        if (intTimeQueueHeadIndex == intTimeQueueTailIndex)
        {
            intTimeQueueHeadIndex = -1;
            intTimeQueueTailIndex = -1;
        }
        else
        {
            intTimeQueueHeadIndex = (intTimeQueueHeadIndex + 1) % intMaxTimeQueueSize;
        }
        return(element);
    }
}


// convert float type value into hex str
string float2hexstr(float a)
{
  int* q = (int*)&a;
  stringstream my_ss;
  my_ss << hex << *q;
  string res1 = my_ss.str() ;
  return res1;
}

// 得出最大、最小、加總值
void MaxMinSum(float *val,float *max, float *min, float *avg)
{
   *avg += *val;

   if(*val > *max)
   {
      *max = *val;
   }
   if(*val < *min)
   {
      *min = *val;
   }
}





struct timespec start, finish, middle1, middle2, delta;
enum { NS_PER_SECOND = 1000000000 };
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

void* simulateTrigger(void* data)
{ 
   while(1)
   {
      
      boolTrigger = true;
      usleep(intDelayUSeconds);
      
      boolTrigger = false;
      // enValueQueue_TERMINATOR();
      // intNumOfShot++;
      // intEnQueueCount = 0;

      usleep(intDelayUSeconds/2);
   }
}


void* WriteSensorQueuetoDatabase(void* data)
{   
   bool boolFirstGetValueFlag = true;
   string stringSQLStatement;
   string stringDownSampleSensorValue;
   string stringSensorValue;
   float floatSensorValue;
   float floatMax , floatMin , floatAvg;
   int intGetValueCount = 0;
   int intfloatSensorValueBufferSize = (CLOCK_RATE*SEC_PER_SHOT) + SECTION_LENGTH;
   float floatSensorValueBuffer[intfloatSensorValueBufferSize] = {0};
   
   int intRetval;
   bool boolInsert = true;


	MYSQL mysqlCon;
   // mysql_init(&mysqlCon);
   // mysql_real_connect(&mysqlCon, MYSQL_SERVER_IP, MYSQL_USER_NAME, MYSQL_PASSWORD, MYSQL_DB_NAME, 0, NULL, CLIENT_MULTI_STATEMENTS);

	// MYSQL* mysqlCon;
   // mysqlCon = mysql_init(0);
   // mysqlCon = mysql_real_connect(mysqlCon, MYSQL_SERVER_IP, MYSQL_USER_NAME, MYSQL_PASSWORD, MYSQL_DB_NAME, 0, NULL, CLIENT_MULTI_STATEMENTS);



   // keep dequeuing
   while(1)
   {
      // there is shot in Queue
      if(intNumOfShot!=0) 
      {


      if(boolInsert){
      // cout << "\nNumber of shot left in Queue: " << intNumOfShot << "   "  ;
      boolInsert = false;

      }

         // prepare one shot of each channel
         for(int i = START_CHANNEL; i < END_CHANNEL; i++) 
         {
            
            stringSQLStatement += "INSERT INTO customSensorSN1_TableSN" + to_string((i + 1)) + " (col, shotMax, shotMin, shotAvg, TimeStamp, GraghData) VALUES ('";
            
            stringDownSampleSensorValue = " ";

            boolFirstGetValueFlag = true;

            floatAvg = 0;
            intGetValueCount = 0;
            
            intTempQueueHeadIndex = (intQueueHeadIndex + 1); 

            // get each value of the shot
            while(floatSensorValueQueueData[i][intTempQueueHeadIndex] != TERMINATOR)
            {

               //  prepare data and buffer with the sensor value
               floatSensorValue = getValueQueue(i);

               floatSensorValueBuffer[intGetValueCount] = floatSensorValue;
               intGetValueCount++;

               // convert sensor value to hex string
               stringSensorValue = float2hexstr(floatSensorValue);
               stringSQLStatement += (stringSensorValue + " ");

               // Calculate Max, Min, Sum value
               if(boolFirstGetValueFlag) 
               {
                  floatMax = floatSensorValue;
                  floatMin = floatSensorValue;
                  boolFirstGetValueFlag = false;
               }
               else
               {
                  MaxMinSum(&floatSensorValue, &floatMax, &floatMin, &floatAvg);
               }


               // downsampling
               if(( intGetValueCount % DOWN_SAMPLING_NUM) == 0 )
               {
                  stringDownSampleSensorValue += (to_string(floatSensorValue) + " "); 
               }                       
            }
            
            // Calculate feature "Avg" value
            floatAvg = floatAvg / float(intGetValueCount);

            // Prepare SQL for one channel
            stringSQLStatement += "', '" + to_string(floatMax) + "', '" + to_string(floatMin) + "', '" + to_string(floatAvg) + "', '" + stringTimeQueue[intTimeQueueHeadIndex] + "', '" + stringDownSampleSensorValue + "'); ";
         
         }

         
      intNumOfInsertShot++;
      intNumOfShot--;


      // Execute SQL for all channels
      // if(intNumOfShot == 0 || intNumOfInsertShot == intMaxInsertShot)
      if(intNumOfInsertShot == 40 || intNumOfInsertShot == intMaxInsertShot)
      {
         printf("Num of shot waiting for insert: %d    left in Queue: %d    Time: %d   ", intNumOfInsertShot, intNumOfShot, geTimeCount()-1);
         mysql_init(&mysqlCon);

         if (!mysql_real_connect(&mysqlCon, MYSQL_SERVER_IP, MYSQL_USER_NAME, MYSQL_PASSWORD, MYSQL_DB_NAME, 0, NULL, CLIENT_MULTI_STATEMENTS))
         {
            writeErrorLog("\nMYSQL connection failed.");
            if (mysql_errno(&mysqlCon))
            {
	            snprintf(charErrorStatement, 300, "\nMYSQL connection error %d: %s", mysql_errno(&mysqlCon), mysql_error(&mysqlCon));
               writeErrorLog(charErrorStatement);
               // printf( "\nMYSQL connection error %d: %s", mysql_errno(&mysqlCon), mysql_error(&mysqlCon));
            }
         }
         
         intRetval = mysql_query(&mysqlCon, stringSQLStatement.c_str());
      //   intRetval = mysql_query(mysqlCon, stringSQLStatement.c_str());

         // if (intRetval != EXIT_SUCCESS)
         if (intRetval)
         {
            snprintf(charErrorStatement, 150, "\nError %u: %s\n", mysql_errno(&mysqlCon), mysql_error(&mysqlCon));
            // snprintf(charErrorStatement, 150, "\nError %u: %s\n", mysql_errno(mysqlCon), mysql_error(mysqlCon));
            
            writeErrorLog(charErrorStatement);
         }
         else
         {
            printf("%lld rows affected\n", mysql_affected_rows(&mysqlCon));
            // printf("%lld rows affected\n", mysql_affected_rows(mysqlCon));

            // printf("   Insert data success");
         }
         
         mysql_close(&mysqlCon);


         // cout << '\n' << stringSQLStatement.c_str() << '\n';
         stringSQLStatement = "";

         boolInsert = true;
         intNumOfInsertShot = 0;
      }

         deTimeQueue();
         deValueQueue();
      }
      else
      {
         // sleep(0.1);
         continue;
      }
   }
}




inline void waitAnyKey()
{
   do{SLEEP(1);} while(!kbhit());
} 
// This function is used to deal with 'DataReady' Event.


int intFailCount=0;
void BDAQCALL OnDataReadyEvent(void * sender, BfdAiEventArgs * args, void *userParam)
{
	int32 returnedCount = 0;
	WaveformAiCtrl * waveformAiCtrl = NULL;
   waveformAiCtrl = (WaveformAiCtrl *)sender;
   int32 getDataCount = MinValue(USER_BUFFER_SIZE, args->Count);
   waveformAiCtrl->GetData(getDataCount, Data, 0, &returnedCount);
  
   // 將section的數值放入Queue
   for(int32 i = 0; i < USER_BUFFER_SIZE; i += CHANNEL_COUNT)
   {
      if(boolTrigger)
      {
         // 將shot的取得時間放入TimeQueue
         if(!TimeQueueIsFull() && intEnQueueCount == 0) 
         {
            enTimeQueue(getTime());
            if(TimeQueueIsFull())
            {
               writeErrorLog("[EnQueue thread] Fail to save Timestamp into TimeQueue while TimeQueue is full \n");
               boolEnQueueThreadErrorFlag = true;
            }
         }


         if(intEnQueueCount > (MAX_ENQUEUE_NUMBER)) 
         {
            if(!boolEnQueueThreadErrorFlag)writeErrorLog("[EnQueue thread] Number of shot out of contraint \n");
            boolEnQueueThreadErrorFlag = true;
         }
         else{

            // 將各通道的取樣值放入Queue
            if(!QueueIsFull())
            {
               if(intQueueHeadIndex == -1) intQueueHeadIndex = 0;
               intQueueTailIndex = (intQueueTailIndex + 1) % intMaxQueueSize;
               
               for(int channel = START_CHANNEL; channel < END_CHANNEL; channel++)
               {
                  floatSensorValueQueueData[channel][intQueueTailIndex] = Data[i + (channel - START_CHANNEL)];
               }
               intEnQueueCount++;
            } 
            else
            {
               if(!boolEnQueueThreadErrorFlag)
               {
                  if (intFailCount==230)
                  {
                     intFailCount=0;
                  }
                  if(intFailCount==0)
                  {
                     writeErrorLog("[EnQueue thread] Fail to save sensor value into Queue while Queue is full \n");
                  }
                  intFailCount++;
               }
               boolEnQueueThreadErrorFlag = true;
            }
         }

         // Shot max time constraint
         if(intEnQueueCount == MAX_ENQUEUE_NUMBER ) 
         {
            // for time constraint
            enValueQueue_TERMINATOR();
            intNumOfShot++;
            intEnQueueCount = 0;
         }
      }
   }


}
// This function is used to deal with 'Overrun' Event.
void BDAQCALL OnOverRunEvent(void * sender, BfdAiEventArgs * args, void *userParam)
{
   printf("Streaming AI Overrun: offset = %d, count = %d\n", args->Offset, args->Count);
   writeErrorLog("Streaming AI Overrun: \n");
}
// This function is used to deal with 'CacheOverflow' Event.
void BDAQCALL OnOverCacheOverflowEvent(void * sender, BfdAiEventArgs * args, void *userParam)
{
   printf(" Streaming AI Cache Overflow: offset = %d, count = %d\n", args->Offset, args->Count);
   writeErrorLog(" Streaming AI Cache Overflow: \n");

}
// This function is used to deal with 'Stopped' Event.
void BDAQCALL OnStoppedEvent(void * sender, BfdAiEventArgs * args, void *userParam)
{


   // printf("Streaming AI stopped: offset = %d, count = %d\n", args->Offset, args->Count);
   intStatus=1;

   if(intEnQueueCount == 0) 
   {
      writeErrorLog("[OnStoppedEvent] No value be sampled but stopEvent occur\n");
      intStatus=1;
   }
   else
   {
      if(boolEnQueueThreadErrorFlag)
      {
         if(intNumOfShot == 0)
         {
            intQueueHeadIndex = -1;
            intQueueTailIndex = -1;
         }
         else
         {
            intQueueTailIndex = intTheLastShotTailIndex;
            intTimeQueueTailIndex = intTheLastTimeTailIndex;
         }
      }
      else
      {
         intNumOfShot++;
         enValueQueue_TERMINATOR();
         intTheLastShotTailIndex = intQueueTailIndex;
         intTheLastTimeTailIndex = intTimeQueueTailIndex;
      }
   }
   intEnQueueCount = 0;
   boolEnQueueThreadErrorFlag = false;

}

int main(int argc, char* argv[])
{

   ErrorCode        ret = Success;

   // Step 1: Create a 'WaveformAiCtrl' for buffered AI function.
   WaveformAiCtrl * wfAiCtrl = WaveformAiCtrl::Create();

	// Step 2: Set the notification event Handler by which we can known the state of operation effectively.
   wfAiCtrl->addDataReadyHandler(OnDataReadyEvent, NULL);
   wfAiCtrl->addOverrunHandler(OnOverRunEvent, NULL);
   wfAiCtrl->addCacheOverflowHandler(OnOverRunEvent, NULL);
   wfAiCtrl->addStoppedHandler(OnStoppedEvent, NULL);
   do
   {
      // Step 3: Select a device by device number or device description and specify the access mode.
      // in this example we use ModeWrite mode so that we can fully control the device, including configuring, sampling, etc.
      DeviceInformation devInfo(DEVICE_DESCRIPTION);
      ret = wfAiCtrl->setSelectedDevice(devInfo);
      CHK_RESULT(ret);
      ret = wfAiCtrl->LoadProfile(profilePath);//Loads a profile to initialize the device.
      CHK_RESULT(ret);

		// Step 4: Set necessary parameters. 
		Conversion * conversion = wfAiCtrl->getConversion();
		ret = conversion->setChannelStart(START_CHANNEL);
		CHK_RESULT(ret);
		ret = conversion->setChannelCount(CHANNEL_COUNT);
		CHK_RESULT(ret);
      ret = conversion->setClockRate(CLOCK_RATE);
      CHK_RESULT(ret);
		Record * record = wfAiCtrl->getRecord();
		ret = record->setSectionCount(SECTION_COUNT);//The 0 means setting 'streaming' mode.
		CHK_RESULT(ret);
		ret = record->setSectionLength(SECTION_LENGTH);
		CHK_RESULT(ret);

// Step 5: The operation has been started.
ret = wfAiCtrl->Prepare();
CHK_RESULT(ret);


ret = wfAiCtrl->Start();
CHK_RESULT(ret);

// Step 6: The device is acquiring data.
printf("Streaming AI is in progress.\nplease wait...  any key to quit!\n\n");


      pthread_t threadWriteSensorQueuetoDatabase; // 宣告 pthread 變數
      pthread_create(&threadWriteSensorQueuetoDatabase, NULL, WriteSensorQueuetoDatabase, NULL); // 建立子執行緒
      
      pthread_t threadsimulateTrigger; // 宣告 test pthread 變數
      pthread_create(&threadsimulateTrigger, NULL, simulateTrigger, NULL); 




   do
   {
      
// if(intStatus==1) 
// {
//    ret = wfAiCtrl->Prepare();
//    CHK_RESULT(ret);
//    ret = wfAiCtrl->Start();
//    CHK_RESULT(ret);
//    intStatus=0;
// }



   usleep(intDelayUSeconds);
   
ret = wfAiCtrl->Stop();
CHK_RESULT(ret);

   usleep(intDelayUSeconds/2);

ret = wfAiCtrl->Prepare();
CHK_RESULT(ret);
ret = wfAiCtrl->Start();
CHK_RESULT(ret);


         // SLEEP(1);
      }	while(!kbhit());
      printf("Leave Loop\n");

      // be command
      // do
      // {
      //    SLEEP(1);
      // }	while(!kbhit());

      // step 8: Stop the operation if it is running.
      ret = wfAiCtrl->Stop(); 
      CHK_RESULT(ret);
   }while(false);

	// Step 9: Close device, release any allocated resource.
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

