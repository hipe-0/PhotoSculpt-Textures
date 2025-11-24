/* TaskTimer.h
 *
 * Created on: 
 *      Author: Hippolyte Mounier, Copyright 2011 All rights reserved
 */
// 80 char per line standard :
/////////_/////////_/////////_/////////_/////////_/////////_/////////_/////////_
#pragma once

#include "Timer.h"

#define TASKTIMER_CODE_NO_DATA              -2.0f
#define TASKTIMER_CODE_CURRENTLY_ESTIMATING -1.0f

class TaskTimer
{
public:
  TaskTimer                ();
  void  setNbTasks         (int task             );
  void  startRecordTime    (int task, int subtask);
  float estimateSecondsNow ();  // returns the time (in seconds) remaining now until completion
  void  stopRecordTime     ();
  void  interruptRecording ();
  void  dumpTimesToDisk    (char * fileName      );

private:
  static const int   NB_TASKS=12 , NB_SUB_TASKS=11;
  static const float MINIMUM_ESTIMATE_TIME;  // time in seconds

  static const float baseTimeTable_    [NB_TASKS] [NB_SUB_TASKS];
  float              currentTimeTable_ [NB_TASKS] [NB_SUB_TASKS];

  Timer time_         ;
  float startTime_    ;
  int   finalTask_    ;
  bool  isRecording_  ;
  float totalRecordedTime_ , totalPreviouslyRecordedTime_ ; 
  int   currentTask_, currentSubTask_;
};
