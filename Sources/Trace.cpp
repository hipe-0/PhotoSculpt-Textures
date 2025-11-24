#include "Trace.h"
#include <string.h>
#include <stdio.h>
#include <windows.h>

#ifdef TRACE_TO_FILE

void Trace::write(char * content)
{
  char path[_MAX_PATH] = "";
  GetTempPath( _MAX_PATH, path );
  strcat_s(path, _MAX_PATH, (DEBUG_FILE_NAME));

  FILE * pFile;
  int isError = fopen_s (&pFile, path, "a");

  if (isError)
    return;

  fprintf (pFile, "%s\n", content );
  printf  (       "%s\n", content );

  fclose(pFile);
}

void Trace::write(char * content, int content2)
{
  char path[_MAX_PATH] = "";
  GetTempPath( _MAX_PATH, path );
  strcat_s(path, _MAX_PATH, (DEBUG_FILE_NAME));

  FILE * pFile;
  int isError = fopen_s (&pFile, path, "a");

  if (isError)
    return;

  fprintf (pFile, "%s=%d\n", content , content2 );
  printf (        "%s=%d\n", content , content2 );

  fclose(pFile);
}
void Trace::write(char * content, float content2)
{
  char path[_MAX_PATH] = "";
  GetTempPath( _MAX_PATH, path );
  strcat_s(path, _MAX_PATH, (DEBUG_FILE_NAME));

  FILE * pFile;
  int isError = fopen_s (&pFile, path, "a");

  if (isError)
    return;

  fprintf (pFile, "%s=%f\n", content, content2 );
  printf  (       "%s=%f\n", content, content2 );

  fclose(pFile);
}

void Trace::write(char* content, char* content2)
{
  char path[_MAX_PATH] = "";
  GetTempPath( _MAX_PATH, path );
  strcat_s(path, _MAX_PATH, (DEBUG_FILE_NAME));

  FILE * pFile;
  int isError = fopen_s (&pFile, path, "a");

  if (isError)
    return;

  fprintf (pFile, "%s=%s\n", content, content2);
  printf (        "%s=%s\n", content, content2);

  fclose(pFile);
}

Trace::Trace(char * functionname, int counter, float * secondscounter)
{
  strcpy_s(function_, 100, functionname);
  char tmp[200];
  sprintf_s(tmp, 200, "Entering %s (%dth call)", function_, counter);
  write(tmp);
  timer_.reset();
  secondsCounter_ = secondscounter;
}

Trace::~Trace()
{
  float seconds = timer_.getSeconds();
  *secondsCounter_ += seconds;
  char tmp[200];
  sprintf_s(tmp, 200, "Exiting %s (%.3fs, cumulated %.3fs)", function_, seconds, *secondsCounter_);
  write(tmp);
}

void Trace::cleanFile()
{
  char path[_MAX_PATH] = "";
  GetTempPath( _MAX_PATH, path );
  strcat_s(path, _MAX_PATH, (DEBUG_FILE_NAME));
  remove(path);
}

#endif