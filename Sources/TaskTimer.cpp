#include "TaskTimer.h"
#include "Includes.h"
#include "stdio.h"

/*
const float TaskTimer::baseTimeTable_ [NB_TASKS][NB_SUB_TASKS] =
{
  {0.0f, 0.000f,   0.000f, 0.000f, 0.000f, 0.000f, 0.000f, 0.000f, 0.000f, 0.000f, 0.000f},
  {0.0f, 0.000f,   0.000f, 0.000f, 0.000f, 0.000f, 0.000f, 0.000f, 0.000f, 0.000f, 0.016f},
  {0.0f, 0.015f,   0.000f, 0.047f, 0.000f, 0.016f, 0.000f, 0.000f, 0.000f, 0.015f, 0.000f},
  {0.0f, 0.140f,   0.016f, 0.156f, 0.000f, 0.094f, 0.000f, 0.000f, 0.000f, 0.016f, 0.000f},
  {0.0f, 0.610f,   0.031f, 0.594f, 0.015f, 0.469f, 0.016f, 0.015f, 0.016f, 0.031f, 0.016f},
  {0.0f, 1.765f,   0.141f, 0.156f, 0.047f, 1.656f, 0.078f, 0.032f, 0.062f, 0.141f, 0.031f},
  {0.0f, 0.860f,   0.562f, 1.922f, 0.156f, 1.407f, 0.281f, 0.172f, 0.390f, 0.532f, 0.140f},
  {0.0f, 3.407f,   2.218f, 3.407f, 0.656f, 2.516f, 1.203f, 1.031f, 1.531f, 2.203f, 0.641f},
  {0.0f, 23.71f,     8.9f,  10.0f,   0.1f,  10.0f,   5.0f,   6.2f,   1.5f,   8.6f,   2.6f},
  {0.0f, 95.34f,    35.9f,  40.1f,   0.5f,  40.3f,  13.2f,  40.8f,   3.1f,  34.7f,  10.4f},
  {0.0f, 380.0f,   140.0f, 160.0f,   2.0f, 160.0f,  52.0f, 160.0f,  12.0f, 136.0f,  40.0f},
  {0.0f, 1520.0f,  560.0f, 640.0f,   8.0f, 640.0f, 208.0f, 240.0f,  48.0f, 544.0f, 160.0f}};*/
/*
const float TaskTimer::baseTimeTable_ [NB_TASKS][NB_SUB_TASKS] = {
  {0.0f, 0.000f, 0.000f, 0.000f, 0.000f, 0.000f, 0.000f, 0.000f, 0.000f, 0.000f, 0.000f},
  {0.0f, 0.000f, 0.000f, 0.000f, 0.000f, 0.000f, 0.000f, 0.000f, 0.000f, 0.000f, 0.000f},
  {0.0f, 0.016f, 0.000f, 0.031f, 0.000f, 0.015f, 0.000f, 0.000f, 0.000f, 0.000f, 0.000f},
  {0.0f, 0.157f, 0.015f, 0.156f, 0.000f, 0.094f, 0.000f, 0.000f, 0.000f, 0.000f, 0.000f},
  {0.0f, 0.328f, 0.031f, 0.312f, 0.016f, 0.266f, 0.031f, 0.000f, 0.016f, 0.000f, 0.015f},
  {0.0f, 0.828f, 0.141f, 0.109f, 0.047f, 0.828f, 0.125f, 0.078f, 0.047f, 0.000f, 0.047f},
  {0.0f, 0.609f, 0.563f, 1.609f, 0.156f, 1.188f, 0.344f, 0.828f, 0.375f, 0.000f, 0.140f},
  {0.0f, 2.563f, 2.218f, 2.657f, 0.640f, 0.000f, 1.453f, 5.297f, 1.500f, 0.000f, 0.625f},
  {0.0f, 18.110f, 8.781f, 0.000f, 0.109f, 0.000f, 5.953f, 29.750f, 1.547f, 0.000f, 2.485f},
  {0.0f, 75.906f, 35.297f, 0.000f, 0.468f, 0.000f, 14.641f, 26.250f, 3.141f, 0.000f, 10.047f},
  {0.0f, 380.0f,   140.0f, 160.0f,   2.0f, 160.0f,  52.0f, 160.0f,  12.0f, 136.0f,  40.0f},
  {0.0f, 1520.0f,  560.0f, 640.0f,   8.0f, 640.0f, 208.0f, 240.0f,  48.0f, 544.0f, 160.0f}};
  */
/*
const float TaskTimer::baseTimeTable_ [NB_TASKS][NB_SUB_TASKS] =
  {{0.000f, 0.016f, 0.000f, 0.000f, 0.000f, 0.000f, 0.000f, 0.000f, 0.000f, 0.000f, 0.000f},
  {0.000f, 0.000f, 0.000f, 0.000f, 0.000f, 0.000f, 0.000f, 0.000f, 0.000f, 0.000f, 0.000f},
  {0.000f, 0.015f, 0.000f, 0.032f, 0.000f, 0.015f, 0.000f, 0.000f, 0.000f, 0.000f, 0.000f},
  {0.000f, 0.094f, 0.000f, 0.125f, 0.000f, 0.093f, 0.000f, 0.016f, 0.000f, 0.000f, 0.000f},
  {0.000f, 0.202f, 0.032f, 0.156f, 0.015f, 0.125f, 0.016f, 0.000f, 0.015f, 0.016f, 0.000f},
  {0.000f, 0.593f, 0.078f, 0.062f, 0.031f, 0.515f, 0.078f, 0.062f, 0.047f, 0.000f, 0.000f},
  {0.000f, 0.405f, 0.281f, 1.030f, 0.109f, 0.702f, 0.281f, 0.265f, 0.140f, 0.000f, 0.000f},
  {0.000f, 1.716f, 1.123f, 1.716f, 0.452f, 0.000f, 1.233f, 1.638f, 0.327f, 0.000f, 0.000f},
  {0.000f, 11.76f, 4.525f, 0.000f, 0.234f, 0.000f, 3.088f, 12.262f, 1.357f, 0.000f, 0.000f},
  {0.000f, 47.97f, 12.69f, 0.000f, 0.624f, 0.000f, 8.237f, 16.536f, 3.884f, 0.000f, 0.000f},
  {0.0f,   188.0,  48.00f, 0.0f,     2.4f,   0.0f,  32.0f,   66.0f,  15.0f,   0.0f,   0.0f},
  {0.0f,   752.0f, 192.0f, 0.0f,     9.9f,   0.0f, 131.0f,  264.0f,  60.8f,   0.0f,   0.0f}};*/

const float TaskTimer::baseTimeTable_ [NB_TASKS][NB_SUB_TASKS] =
  {{0.000f, 0.016f, 0.016f, 0.015f, 0.000f, 0.031f, 0.000f, 0.016f, 0.000f, 0.016f, 0.015f},
  {0.000f, 0.031f, 0.000f, 0.047f, 0.016f, 0.031f, 0.000f, 0.062f, 0.000f, 0.000f, 0.062f},
  {0.000f, 0.047f, 0.000f, 0.046f, 0.016f, 0.031f, 0.000f, 0.016f, 0.000f, 0.000f, 1.046f},
  {0.000f, 0.141f, 0.015f, 0.172f, 0.000f, 0.109f, 0.015f, 0.016f, 0.000f, 0.000f, 0.515f},
  {0.000f, 0.312f, 0.015f, 0.250f, 0.000f, 0.203f, 0.015f, 0.016f, 0.000f, 0.000f, 0.717f},
  {0.000f, 0.780f, 0.032f, 0.078f, 0.015f, 0.531f, 0.031f, 0.031f, 0.016f, 0.000f, 3.104f},
  {0.000f, 0.436f, 0.094f, 0.577f, 0.047f, 0.515f, 0.140f, 0.047f, 0.062f, 0.000f, 0.921f},
  {0.000f, 1.264f, 0.437f, 1.279f, 0.171f, 0.000f, 0.375f, 0.203f, 0.124f, 0.000f, 1.467f},
  {0.000f, 9.469f, 1.482f, 0.000f, 0.156f, 0.016f, 1.684f, 0.905f, 0.577f, 0.016f, 1.108f},
  {0.000f, 36.286f, 4.430f, 0.016f, 0.514f, 0.016f, 4.555f, 2.980f, 1.326f, 0.000f, 4.150f},
  {0.000f, 36.286f*2, 4.430f*2, 0.016f*2, 0.514f*2, 0.016f*2, 4.555f*2, 2.980f*2, 1.326f*2, 0.000f*2, 4.150f*2},
  {0.000f, 36.286f*4, 4.430f*4, 0.016f*4, 0.514f*4, 0.016f*4, 4.555f*4, 2.980f*4, 1.326f*4, 0.000f*4, 4.150f*4}};

const float TaskTimer::MINIMUM_ESTIMATE_TIME = 2.0f; // 2 seconds
 
TaskTimer::TaskTimer()
{
  finalTask_    = -1;
  isRecording_  = false;
  totalRecordedTime_ = totalPreviouslyRecordedTime_ = 0.0f ;
  currentTask_ = currentSubTask_ = -1;
  for (int task=0; task<NB_TASKS ; task++)
    for (int sub=0; sub<NB_SUB_TASKS ; sub++)
      currentTimeTable_[task][sub] = 0.0f;
}

void TaskTimer::setNbTasks(int nbtasks)
{
  finalTask_ = nbtasks - 1;
}

void TaskTimer::startRecordTime(int task, int subtask)
{
  if (task < 0    || task >= NB_TASKS       )    return;
  if (subtask < 0 || subtask >= NB_SUB_TASKS)    return;

  isRecording_    = true; // not a problem if it was true already
  startTime_      = time_.getSeconds();
  currentTask_    = task;
  currentSubTask_ = subtask;
}

void TaskTimer::stopRecordTime()
{
  isRecording_ = false;  
  
  if (currentTask_ < 0    || currentTask_ >= NB_TASKS       )    return;
  if (currentSubTask_ < 0 || currentSubTask_ >= NB_SUB_TASKS)    return;

  float time = time_.getSeconds() - startTime_ ;
  currentTimeTable_ [currentTask_][currentSubTask_] = time;

  totalRecordedTime_                += time;
  totalPreviouslyRecordedTime_      += baseTimeTable_ [currentTask_][currentSubTask_] ;
}

void TaskTimer::interruptRecording()
{
  isRecording_    = false;
  currentSubTask_ = 0;
}

float TaskTimer::estimateSecondsNow()
{
  if ( totalPreviouslyRecordedTime_ <= 0.0f )                      return TASKTIMER_CODE_NO_DATA;
  if ( finalTask_ < 0 || currentTask_ < 0 || currentSubTask_ < 0 ) return TASKTIMER_CODE_NO_DATA;

  if ( totalRecordedTime_ < MINIMUM_ESTIMATE_TIME )  // not enough estimation done   
    return TASKTIMER_CODE_CURRENTLY_ESTIMATING;      // error code saying it's estimating time now

  float remainingJob = 0.0f;

  if (currentTask_ < finalTask_)
    // count how many jobs are remaining
    for (int task = currentTask_ + 1; task <= finalTask_ ; task ++ ) // exclude current task
      for (int sub=0; sub<NB_SUB_TASKS ; sub++) 
        remainingJob += baseTimeTable_[task][sub];

  if (currentSubTask_ < NB_SUB_TASKS)
    for (int sub = currentSubTask_ ; sub < NB_SUB_TASKS ; sub ++ )
      remainingJob += baseTimeTable_[currentTask_][sub];

  float estimate  = remainingJob * totalRecordedTime_ / totalPreviouslyRecordedTime_;
  
  float extratime = 0.0f;
  if (isRecording_)
  {
    float currentTask = baseTimeTable_[currentTask_][currentSubTask_] 
                        * totalRecordedTime_ / totalPreviouslyRecordedTime_; 
    extratime = time_.getSeconds() - startTime_;
    if (extratime < 0.0f)
      extratime = 0.0f;
    if (extratime > currentTask )
      extratime = currentTask;
  }
  
  const float EXAGERATION = 1.5f ;

  float totalestimate = estimate * EXAGERATION - extratime;
  if (totalestimate < 0.0f)
    totalestimate = 0.0f;

  return totalestimate;
}

void TaskTimer::dumpTimesToDisk(char * fileName)
{
  FILE * pFile;
  int isError = fopen_s (&pFile, fileName,"w");

  if (isError)
    return;

  float total = 0.0f;

  fprintf (pFile, "const float TaskTimer::baseTimeTable_ [NB_TASKS][NB_SUB_TASKS] =\n  {{" );

  for (int task=0; task<NB_TASKS ; task++)
  {
    for (int sub=0; sub<NB_SUB_TASKS ; sub++)
    {
      float time = currentTimeTable_[task][sub];
      fprintf (pFile, "%.3ff", time);
      total += time ;
      if (sub<NB_SUB_TASKS-1)
        fprintf (pFile, ", ");
    }

    if (task<NB_TASKS-1)
      fprintf (pFile, "},\n  {" );
    else
      fprintf (pFile, "}};\n" );
  }

  fprintf (pFile, "// total = %.3ff\n", total );

  fclose(pFile);
}
