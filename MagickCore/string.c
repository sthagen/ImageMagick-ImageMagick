/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%                  SSSSS   TTTTT  RRRR   IIIII  N   N   GGGG                  %
%                  SS        T    R   R    I    NN  N  G                      %
%                   SSS      T    RRRR     I    N N N  G GGG                  %
%                     SS     T    R R      I    N  NN  G   G                  %
%                  SSSSS     T    R  R   IIIII  N   N   GGGG                  %
%                                                                             %
%                                                                             %
%                        MagickCore String Methods                            %
%                                                                             %
%                             Software Design                                 %
%                                  Cristy                                     %
%                               August 2003                                   %
%                                                                             %
%                                                                             %
%  Copyright @ 1999 ImageMagick Studio LLC, a non-profit organization         %
%  dedicated to making software imaging solutions freely available.           %
%                                                                             %
%  You may not use this file except in compliance with the license.  You may  %
%  obtain a copy of the license at                                            %
%                                                                             %
%    https://imagemagick.org/script/license.php                               %
%                                                                             %
%  unless required by applicable law or agreed to in writing, software        %
%  distributed under the license is distributed on an "as is" basis,          %
%  without warranties or conditions of any kind, either express or implied.   %
%  See the license for the specific language governing permissions and        %
%  limitations under the license.                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%
*/

/*
  Include declarations.
*/
#include "MagickCore/studio.h"
#include "MagickCore/blob.h"
#include "MagickCore/blob-private.h"
#include "MagickCore/exception.h"
#include "MagickCore/exception-private.h"
#include "MagickCore/image-private.h"
#include "MagickCore/list.h"
#include "MagickCore/locale_.h"
#include "MagickCore/log.h"
#include "MagickCore/memory_.h"
#include "MagickCore/memory-private.h"
#include "MagickCore/nt-base-private.h"
#include "MagickCore/property.h"
#include "MagickCore/policy.h"
#include "MagickCore/resource_.h"
#include "MagickCore/resource-private.h"
#include "MagickCore/signature-private.h"
#include "MagickCore/string_.h"
#include "MagickCore/string-private.h"
#include "MagickCore/utility-private.h"

/*
  Define declarations.
*/
#define CharsPerLine  0x14

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   A c q u i r e S t r i n g                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  AcquireString() returns an new extended string, containing a clone of the
%  given string.
%
%  An extended string is the string length, plus an extra MagickPathExtent space
%  to allow for the string to be actively worked on.
%
%  The returned string should be freed using DestroyString().
%
%  The format of the AcquireString method is:
%
%      char *AcquireString(const char *source)
%
%  A description of each parameter follows:
%
%    o source: A character string.
%
*/
MagickExport char *AcquireString(const char *source)
{
  char
    *destination;

  size_t
    length;

  length=0;
  if (source != (char *) NULL)
    length+=strlen(source);
  if (~length < MagickPathExtent)
    ThrowFatalException(ResourceLimitFatalError,"UnableToAcquireString");
  destination=(char *) AcquireQuantumMemory(length+MagickPathExtent,
    sizeof(*destination));
  if (destination == (char *) NULL)
    ThrowFatalException(ResourceLimitFatalError,"UnableToAcquireString");
  if (source != (char *) NULL)
    (void) memcpy(destination,source,length*sizeof(*destination));
  destination[length]='\0';
  return(destination);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   A c q u i r e S t r i n g I n f o                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  AcquireStringInfo() allocates the StringInfo structure.
%
%  The format of the AcquireStringInfo method is:
%
%      StringInfo *AcquireStringInfo(const size_t length)
%
%  A description of each parameter follows:
%
%    o length: the string length.
%
*/

static StringInfo *AcquireStringInfoContainer(void)
{
  StringInfo
    *string_info;

  string_info=(StringInfo *) AcquireCriticalMemory(sizeof(*string_info));
  (void) memset(string_info,0,sizeof(*string_info));
  string_info->signature=MagickCoreSignature;
  return(string_info);
}

MagickExport StringInfo *AcquireStringInfo(const size_t length)
{
  StringInfo
    *string_info;

  string_info=AcquireStringInfoContainer();
  string_info->length=length;
  if (~string_info->length >= (MagickPathExtent-1))
    string_info->datum=(unsigned char *) AcquireQuantumMemory(
      string_info->length+MagickPathExtent,sizeof(*string_info->datum));
  if (string_info->datum == (unsigned char *) NULL)
    ThrowFatalException(ResourceLimitFatalError,"MemoryAllocationFailed");
  (void) memset(string_info->datum,0,(length+MagickPathExtent)*
    sizeof(*string_info->datum));
  return(string_info);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   B l o b T o S t r i n g I n f o                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  BlobToStringInfo() returns the contents of a blob as a StringInfo structure
%  with MagickPathExtent extra space.
%
%  The format of the BlobToStringInfo method is:
%
%      StringInfo *BlobToStringInfo(const void *blob,const size_t length)
%
%  A description of each parameter follows:
%
%    o blob: the blob.
%
%    o length: the length of the blob.
%
*/
MagickExport StringInfo *BlobToStringInfo(const void *blob,const size_t length)
{
  StringInfo
    *string_info;

  if (~length < MagickPathExtent)
    ThrowFatalException(ResourceLimitFatalError,"MemoryAllocationFailed");
  string_info=AcquireStringInfoContainer();
  string_info->length=length;
  string_info->datum=(unsigned char *) AcquireQuantumMemory(length+
    MagickPathExtent,sizeof(*string_info->datum));
  if (string_info->datum == (unsigned char *) NULL)
    {
      string_info=DestroyStringInfo(string_info);
      return((StringInfo *) NULL);
    }
  if (blob != (const void *) NULL)
    (void) memcpy(string_info->datum,blob,length);
  else
    (void) memset(string_info->datum,0,length*sizeof(*string_info->datum));
  (void) memset(string_info->datum+length,0,MagickPathExtent*
    sizeof(*string_info->datum));
  return(string_info);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   C l o n e S t r i n g                                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  CloneString() replaces or frees the destination string to make it
%  a clone of the input string plus MagickPathExtent more space so the string
%  may be worked on.
%
%  If source is a NULL pointer the destination string will be freed and set to
%  a NULL pointer.  A pointer to the stored in the destination is also returned.
%
%  When finished the non-NULL string should be freed using DestroyString()
%  or using CloneString() with a NULL pointed for the source.
%
%  The format of the CloneString method is:
%
%      char *CloneString(char **destination,const char *source)
%
%  A description of each parameter follows:
%
%    o destination:  A pointer to a character string.
%
%    o source: A character string.
%
*/
MagickExport char *CloneString(char **destination,const char *source)
{
  size_t
    length;

  assert(destination != (char **) NULL);
  if (source == (const char *) NULL)
    {
      if (*destination != (char *) NULL)
        *destination=DestroyString(*destination);
      return(*destination);
    }
  if (*destination == (char *) NULL)
    {
      *destination=AcquireString(source);
      return(*destination);
    }
  length=strlen(source);
  if (~length < MagickPathExtent)
    ThrowFatalException(ResourceLimitFatalError,"UnableToAcquireString");
  *destination=(char *) ResizeQuantumMemory(*destination,length+
    MagickPathExtent,sizeof(**destination));
  if (*destination == (char *) NULL)
    ThrowFatalException(ResourceLimitFatalError,"UnableToAcquireString");
  if (length != 0)
    (void) memcpy(*destination,source,length*sizeof(**destination));
  (*destination)[length]='\0';
  return(*destination);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   C l o n e S t r i n g I n f o                                             %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  CloneStringInfo() clones a copy of the StringInfo structure.
%
%  The format of the CloneStringInfo method is:
%
%      StringInfo *CloneStringInfo(const StringInfo *string_info)
%
%  A description of each parameter follows:
%
%    o string_info: the string info.
%
*/
MagickExport StringInfo *CloneStringInfo(const StringInfo *string_info)
{
  StringInfo
    *clone_info;

  assert(string_info != (StringInfo *) NULL);
  assert(string_info->signature == MagickCoreSignature);
  clone_info=AcquireStringInfo(string_info->length);
  (void) CloneString(&clone_info->path,string_info->path);
  (void) CloneString(&clone_info->name,string_info->name);
  if (string_info->length != 0)
    (void) memcpy(clone_info->datum,string_info->datum,string_info->length+1);
  return(clone_info);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   C o m p a r e S t r i n g I n f o                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  CompareStringInfo() compares the two datums target and source.  It returns
%  an integer less than, equal to, or greater than zero if target is found,
%  respectively, to be less than, to match, or be greater than source.
%
%  The format of the CompareStringInfo method is:
%
%      int CompareStringInfo(const StringInfo *target,const StringInfo *source)
%
%  A description of each parameter follows:
%
%    o target: the target string.
%
%    o source: the source string.
%
*/

MagickExport int CompareStringInfo(const StringInfo *target,
  const StringInfo *source)
{
  int
    status;

  assert(target != (StringInfo *) NULL);
  assert(target->signature == MagickCoreSignature);
  assert(source != (StringInfo *) NULL);
  assert(source->signature == MagickCoreSignature);
  status=memcmp(target->datum,source->datum,MagickMin(target->length,
    source->length));
  if (status != 0)
    return(status);
  if (target->length == source->length)
    return(0);
  return(target->length < source->length ? -1 : 1);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   C o n c a t e n a t e M a g i c k S t r i n g                             %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  ConcatenateMagickString() concatenates the source string to the destination
%  string.  The destination buffer is always null-terminated even if the
%  string must be truncated.
%
%  The format of the ConcatenateMagickString method is:
%
%      size_t ConcatenateMagickString(char *magick_restrict destination,
%        const char *magick_restrict source,const size_t length)
%
%  A description of each parameter follows:
%
%    o destination: the destination string.
%
%    o source: the source string.
%
%    o length: the length of the destination string.
%
*/
MagickExport size_t ConcatenateMagickString(char *magick_restrict destination,
  const char *magick_restrict source,const size_t length)
{
  char
    *magick_restrict q;

  const char
    *magick_restrict p;

  size_t
    count,
    i;

  assert(destination != (char *) NULL);
  assert(source != (const char *) NULL);
  assert(length >= 1);
  p=source;
  q=destination;
  i=length;
  while ((i-- != 0) && (*q != '\0'))
    q++;
  count=(size_t) (q-destination);
  i=length-count;
  if (i == 0)
    return(count+strlen(p));
  while (*p != '\0')
  {
    if (i != 1)
      {
        *q++=(*p);
        i--;
      }
    p++;
  }
  *q='\0';
  return(count+(size_t) (p-source));
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   C o n c a t e n a t e S t r i n g                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  ConcatenateString() appends a copy of string source, including the
%  terminating null character, to the end of string destination.
%
%  The format of the ConcatenateString method is:
%
%      MagickBooleanType ConcatenateString(char **magick_restrict destination,
%        const char *magick_restrict source)
%
%  A description of each parameter follows:
%
%    o destination:  A pointer to a character string.
%
%    o source: A character string.
%
*/
MagickExport MagickBooleanType ConcatenateString(
  char **magick_restrict destination,const char *magick_restrict source)
{
  size_t
    destination_length,
    length,
    source_length;

  assert(destination != (char **) NULL);
  if (source == (const char *) NULL)
    return(MagickTrue);
  if (*destination == (char *) NULL)
    {
      *destination=AcquireString(source);
      return(MagickTrue);
    }
  destination_length=strlen(*destination);
  source_length=strlen(source);
  length=destination_length;
  if (~length < source_length)
    ThrowFatalException(ResourceLimitFatalError,"UnableToConcatenateString");
  length+=source_length;
  if (~length < MagickPathExtent)
    ThrowFatalException(ResourceLimitFatalError,"UnableToConcatenateString");
  *destination=(char *) ResizeQuantumMemory(*destination,
    OverAllocateMemory(length+MagickPathExtent),sizeof(**destination));
  if (*destination == (char *) NULL)
    ThrowFatalException(ResourceLimitFatalError,"UnableToConcatenateString");
  if (source_length != 0)
    (void) memcpy((*destination)+destination_length,source,source_length);
  (*destination)[length]='\0';
  return(MagickTrue);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   C o n c a t e n a t e S t r i n g I n f o                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  ConcatenateStringInfo() concatenates the source string to the destination
%  string.
%
%  The format of the ConcatenateStringInfo method is:
%
%      void ConcatenateStringInfo(StringInfo *string_info,
%        const StringInfo *source)
%
%  A description of each parameter follows:
%
%    o string_info: the string info.
%
%    o source: the source string.
%
*/
MagickExport void ConcatenateStringInfo(StringInfo *string_info,
  const StringInfo *source)
{
  size_t
    length;

  assert(string_info != (StringInfo *) NULL);
  assert(string_info->signature == MagickCoreSignature);
  assert(source != (const StringInfo *) NULL);
  length=string_info->length;
  if (~length < source->length)
    ThrowFatalException(ResourceLimitFatalError,"UnableToConcatenateString");
  length+=source->length;
  if (~length < MagickPathExtent)
    ThrowFatalException(ResourceLimitFatalError,"MemoryAllocationFailed");
  if (string_info->datum == (unsigned char *) NULL)
    string_info->datum=(unsigned char *) AcquireQuantumMemory(length+
      MagickPathExtent,sizeof(*string_info->datum));
  else
    string_info->datum=(unsigned char *) ResizeQuantumMemory(
      string_info->datum,OverAllocateMemory(length+MagickPathExtent),
      sizeof(*string_info->datum));
  if (string_info->datum == (unsigned char *) NULL)
    ThrowFatalException(ResourceLimitFatalError,"MemoryAllocationFailed");
  (void) memcpy(string_info->datum+string_info->length,source->datum,
    source->length);
  string_info->length=length;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   C o n f i g u r e F i l e T o S t r i n g I n f o                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  ConfigureFileToStringInfo() returns the contents of a configure file as a
%  string.
%
%  The format of the ConfigureFileToStringInfo method is:
%
%      StringInfo *ConfigureFileToStringInfo(const char *filename)
%        ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o filename: the filename.
%
*/
MagickExport StringInfo *ConfigureFileToStringInfo(const char *filename)
{
  char
    *string;

  int
    file;

  MagickOffsetType
    offset;

  size_t
    length;

  StringInfo
    *string_info;

  void
    *map;

  assert(filename != (const char *) NULL);
  file=open_utf8(filename,O_RDONLY | O_BINARY,0);
  if (file == -1)
    return((StringInfo *) NULL);
  offset=(MagickOffsetType) lseek(file,0,SEEK_END);
  if ((offset < 0) || (offset != (MagickOffsetType) ((ssize_t) offset)))
    {
      file=close_utf8(file)-1;
      return((StringInfo *) NULL);
    }
  length=(size_t) offset;
  string=(char *) NULL;
  if (~length >= (MagickPathExtent-1))
    string=(char *) AcquireQuantumMemory(length+MagickPathExtent,
      sizeof(*string));
  if (string == (char *) NULL)
    {
      file=close_utf8(file)-1;
      return((StringInfo *) NULL);
    }
  map=MapBlob(file,ReadMode,0,length);
  if (map != (void *) NULL)
    {
      (void) memcpy(string,map,length);
      (void) UnmapBlob(map,length);
    }
  else
    {
      size_t
        i;

      ssize_t
        count;

      (void) lseek(file,0,SEEK_SET);
      for (i=0; i < length; i+=(size_t) count)
      {
        count=read(file,string+i,(size_t) MagickMin(length-i,(size_t)
          MagickMaxBufferExtent));
        if (count <= 0)
          {
            count=0;
            if (errno != EINTR)
              break;
          }
      }
      if (i < length)
        {
          file=close_utf8(file)-1;
          string=DestroyString(string);
          return((StringInfo *) NULL);
        }
    }
  string[length]='\0';
  file=close_utf8(file)-1;
  string_info=AcquireStringInfoContainer();
  string_info->path=ConstantString(filename);
  string_info->length=length;
  string_info->datum=(unsigned char *) string;
  return(string_info);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   C o n s t a n t S t r i n g                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  ConstantString() allocates exactly the needed memory for a string and
%  copies the source string to that memory location.  A NULL string pointer
%  will allocate an empty string containing just the NUL character.
%
%  When finished the string should be freed using DestroyString()
%
%  The format of the ConstantString method is:
%
%      char *ConstantString(const char *source)
%
%  A description of each parameter follows:
%
%    o source: A character string.
%
*/
MagickExport char *ConstantString(const char *source)
{
  char
    *destination;

  size_t
    length;

  length=0;
  if (source != (char *) NULL)
    length+=strlen(source);
  destination=(char *) NULL;
  if (~length >= 1UL)
    destination=(char *) AcquireQuantumMemory(length+1UL,sizeof(*destination));
  if (destination == (char *) NULL)
    ThrowFatalException(ResourceLimitFatalError,"UnableToAcquireString");
  if (source != (char *) NULL)
    (void) memcpy(destination,source,length*sizeof(*destination));
  destination[length]='\0';
  return(destination);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   C o p y M a g i c k S t r i n g                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  CopyMagickString() copies the source string to the destination string, with
%  out exceeding the given pre-declared length.
%
%  The destination buffer is always null-terminated even if the string must be
%  truncated.  The return value is the length of the string.
%
%  The format of the CopyMagickString method is:
%
%      size_t CopyMagickString(const char *magick_restrict destination,
%        char *magick_restrict source,const size_t length)
%
%  A description of each parameter follows:
%
%    o destination: the destination string.
%
%    o source: the source string.
%
%    o length: the length of the destination string.
%
*/
MagickExport size_t CopyMagickString(char *magick_restrict destination,
  const char *magick_restrict source,const size_t length)
{
  char
    *magick_restrict q;

  const char
    *magick_restrict p;

  size_t
    n;

  p=source;
  q=destination;
  for (n=length; n > 4; n-=4)
  {
    if (((*q++)=(*p++)) == '\0')
      return((size_t) (p-source-1));
    if (((*q++)=(*p++)) == '\0')
      return((size_t) (p-source-1));
    if (((*q++)=(*p++)) == '\0')
      return((size_t) (p-source-1));
    if (((*q++)=(*p++)) == '\0')
      return((size_t) (p-source-1));
  }
  if (length != 0)
    {
      while (--n != 0)
        if (((*q++)=(*p++)) == '\0')
          return((size_t) (p-source-1));
      *q='\0';
    }
  return((size_t) (p-source));
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D e s t r o y S t r i n g                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DestroyString() destroys memory associated with a string.
%
%  The format of the DestroyString method is:
%
%      char *DestroyString(char *string)
%
%  A description of each parameter follows:
%
%    o string: the string.
%
*/
MagickExport char *DestroyString(char *string)
{
  return((char *) RelinquishMagickMemory(string));
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D e s t r o y S t r i n g I n f o                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DestroyStringInfo() destroys memory associated with the StringInfo structure.
%
%  The format of the DestroyStringInfo method is:
%
%      StringInfo *DestroyStringInfo(StringInfo *string_info)
%
%  A description of each parameter follows:
%
%    o string_info: the string info.
%
*/
MagickExport StringInfo *DestroyStringInfo(StringInfo *string_info)
{
  assert(string_info != (StringInfo *) NULL);
  assert(string_info->signature == MagickCoreSignature);
  if (string_info->datum != (unsigned char *) NULL)
    string_info->datum=(unsigned char *) RelinquishMagickMemory(
      string_info->datum);
  if (string_info->name != (char *) NULL)
    string_info->name=DestroyString(string_info->name);
  if (string_info->path != (char *) NULL)
    string_info->path=DestroyString(string_info->path);
  string_info->signature=(~MagickCoreSignature);
  string_info=(StringInfo *) RelinquishMagickMemory(string_info);
  return(string_info);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D e s t r o y S t r i n g L i s t                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DestroyStringList() zeros memory associated with a string list.
%
%  The format of the DestroyStringList method is:
%
%      char **DestroyStringList(char **list)
%
%  A description of each parameter follows:
%
%    o list: the string list.
%
*/
MagickExport char **DestroyStringList(char **list)
{
  ssize_t
    i;

  assert(list != (char **) NULL);
  for (i=0; list[i] != (char *) NULL; i++)
    list[i]=DestroyString(list[i]);
  list=(char **) RelinquishMagickMemory(list);
  return(list);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   E s c a p e S t r i n g                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  EscapeString() allocates memory for a backslash-escaped version of a
%  source text string, copies the escaped version of the text to that
%  memory location while adding backslash characters, and returns the
%  escaped string.
%
%  The format of the EscapeString method is:
%
%      char *EscapeString(const char *source,const char escape)
%
%  A description of each parameter follows:
%
%    o allocate_string:  Method EscapeString returns the escaped string.
%
%    o source: A character string.
%
%    o escape: the quoted string termination character to escape (e.g. '"').
%
*/
MagickExport char *EscapeString(const char *source,const char escape)
{
  char
    *destination;

  char
    *q;

  const char
    *p;

  size_t
    length;

  assert(source != (const char *) NULL);
  length=0;
  for (p=source; *p != '\0'; p++)
  {
    if ((*p == '\\') || (*p == escape))
      {
        if (~length < 1)
          ThrowFatalException(ResourceLimitFatalError,"UnableToEscapeString");
        length++;
      }
    length++;
  }
  destination=(char *) NULL;
  if (~length >= (MagickPathExtent-1))
    destination=(char *) AcquireQuantumMemory(length+MagickPathExtent,
      sizeof(*destination));
  if (destination == (char *) NULL)
    ThrowFatalException(ResourceLimitFatalError,"UnableToEscapeString");
  *destination='\0';
  q=destination;
  for (p=source; *p != '\0'; p++)
  {
    if ((*p == '\\') || (*p == escape))
      *q++='\\';
    *q++=(*p);
  }
  *q='\0';
  return(destination);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   F i l e T o S t r i n g                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  FileToString() returns the contents of a file as a string.
%
%  The format of the FileToString method is:
%
%      char *FileToString(const char *filename,const size_t extent,
%        ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o filename: the filename.
%
%    o extent: Maximum length of the string.
%
%    o exception: return any errors or warnings in this structure.
%
*/
MagickExport char *FileToString(const char *filename,const size_t extent,
  ExceptionInfo *exception)
{
  const char
    *p;

  size_t
    length;

  assert(filename != (const char *) NULL);
  assert(exception != (ExceptionInfo *) NULL);
  if (IsEventLogging() != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",filename);
  p=filename;
  if ((*filename == '@') && (strlen(filename) > 1))
    {
      MagickBooleanType
        status;

      status=IsRightsAuthorized(PathPolicyDomain,ReadPolicyRights,filename);
      if (status == MagickFalse)
        {
          errno=EPERM;
          (void) ThrowMagickException(exception,GetMagickModule(),PolicyError,
            "NotAuthorized","`%s'",filename);
          return((char *) NULL);
        }
      p=filename+1;
    }
  return((char *) FileToBlob(p,extent,&length,exception));
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   F i l e T o S t r i n g I n f o                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  FileToStringInfo() returns the contents of a file as a string.
%
%  The format of the FileToStringInfo method is:
%
%      StringInfo *FileToStringInfo(const char *filename,const size_t extent,
%        ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o filename: the filename.
%
%    o extent: Maximum length of the string.
%
%    o exception: return any errors or warnings in this structure.
%
*/
MagickExport StringInfo *FileToStringInfo(const char *filename,
  const size_t extent,ExceptionInfo *exception)
{
  StringInfo
    *string_info;

  assert(filename != (const char *) NULL);
  assert(exception != (ExceptionInfo *) NULL);
  if (IsEventLogging() != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",filename);
  string_info=AcquireStringInfoContainer();
  string_info->path=ConstantString(filename);
  string_info->datum=(unsigned char *) FileToBlob(filename,extent,
    &string_info->length,exception);
  if (string_info->datum == (unsigned char *) NULL)
    {
      string_info=DestroyStringInfo(string_info);
      return((StringInfo *) NULL);
    }
  return(string_info);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%  F o r m a t M a g i c k S i z e                                            %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  FormatMagickSize() converts a size to a human readable format, for example,
%  14k, 234m, 2.7g, or 3.0t.  Scaling is done by repetitively dividing by
%  1000.
%
%  The format of the FormatMagickSize method is:
%
%      ssize_t FormatMagickSize(const MagickSizeType size,const char *suffix,
%        const size_t length,char *format)
%
%  A description of each parameter follows:
%
%    o size:  convert this size to a human readable format.
%
%    o bi:  use power of two rather than power of ten.
%
%    o suffix:  append suffix, typically B or P.
%
%    o length: the maximum length of the string.
%
%    o format:  human readable format.
%
*/
MagickExport ssize_t FormatMagickSize(const MagickSizeType size,
  const MagickBooleanType bi,const char *suffix,const size_t length,
  char *format)
{
  const char
    **units;

  double
    bytes,
    extent;

  ssize_t
    i;

  ssize_t
    count;

  static const char
    *bi_units[] =
    {
      "", "Ki", "Mi", "Gi", "Ti", "Pi", "Ei", "Zi", "Yi", "Ri", "Qi", (char *) NULL
    },
    *traditional_units[] =
    {
      "", "K", "M", "G", "T", "P", "E", "Z", "Y", "R", "Q", (char *) NULL
    };

  bytes=1000.0;
  units=traditional_units;
  if (bi != MagickFalse)
    {
      bytes=1024.0;
      units=bi_units;
    }
  extent=(double) size;
  (void) FormatLocaleString(format,MagickFormatExtent,"%.*g",
    GetMagickPrecision(),extent);
  if (strstr(format,"e+") == (char *) NULL)
    {
      if (suffix == (const char *) NULL)
        count=FormatLocaleString(format,length,"%.20g%s",extent,units[0]);
      else
        count=FormatLocaleString(format,length,"%.20g%s%s",extent,units[0],
          suffix);
      return(count);
    }
  for (i=0; (extent >= bytes) && (units[i+1] != (const char *) NULL); i++)
    extent/=bytes;
  if (suffix == (const char *) NULL)
    count=FormatLocaleString(format,length,"%.*g%s",GetMagickPrecision(),
      extent,units[i]);
  else
    count=FormatLocaleString(format,length,"%.*g%s%s",GetMagickPrecision(),
      extent,units[i],suffix);
  return(count);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   G e t E n v i r o n m e n t V a l u e                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  GetEnvironmentValue() returns the environment string that matches the
%  specified name.
%
%  The format of the GetEnvironmentValue method is:
%
%      char *GetEnvironmentValue(const char *name)
%
%  A description of each parameter follows:
%
%    o name: the environment name.
%
*/
MagickExport char *GetEnvironmentValue(const char *name)
{
#if defined(MAGICKCORE_WINDOWS_SUPPORT)
  return(NTGetEnvironmentValue(name));
#else
  const char
    *environment;

  environment=getenv(name);
  if (environment == (const char *) NULL)
    return((char *) NULL);
  return(ConstantString(environment));
#endif
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   G e t S t r i n g I n f o D a t u m                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  GetStringInfoDatum() returns the datum associated with the string.
%
%  The format of the GetStringInfoDatum method is:
%
%      unsigned char *GetStringInfoDatum(const StringInfo *string_info)
%
%  A description of each parameter follows:
%
%    o string_info: the string info.
%
*/
MagickExport unsigned char *GetStringInfoDatum(const StringInfo *string_info)
{
  assert(string_info != (StringInfo *) NULL);
  assert(string_info->signature == MagickCoreSignature);
  return(string_info->datum);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   G e t S t r i n g I n f o L e n g t h                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  GetStringInfoLength() returns the string length.
%
%  The format of the GetStringInfoLength method is:
%
%      size_t GetStringInfoLength(const StringInfo *string_info)
%
%  A description of each parameter follows:
%
%    o string_info: the string info.
%
*/
MagickExport size_t GetStringInfoLength(const StringInfo *string_info)
{
  assert(string_info != (StringInfo *) NULL);
  assert(string_info->signature == MagickCoreSignature);
  return(string_info->length);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   G e t S t r i n g I n f o N a m e                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  GetStringInfoName() returns the name associated with the string.
%
%  The format of the GetStringInfoName method is:
%
%      const char *GetStringInfoName(const StringInfo *string_info)
%
%  A description of each parameter follows:
%
%    o string_info: the string info.
%
*/
MagickExport const char *GetStringInfoName(const StringInfo *string_info)
{
  assert(string_info != (StringInfo *) NULL);
  assert(string_info->signature == MagickCoreSignature);
  return(string_info->name);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   G e t S t r i n g I n f o P a t h                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  GetStringInfoPath() returns the path associated with the string.
%
%  The format of the GetStringInfoPath method is:
%
%      const char *GetStringInfoPath(const StringInfo *string_info)
%
%  A description of each parameter follows:
%
%    o string_info: the string info.
%
*/
MagickExport const char *GetStringInfoPath(const StringInfo *string_info)
{
  assert(string_info != (StringInfo *) NULL);
  assert(string_info->signature == MagickCoreSignature);
  return(string_info->path);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   I n t e r p r e t S i P r e f i x V a l u e                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  InterpretSiPrefixValue() converts the initial portion of the string to a
%  double representation.  It also recognizes SI prefixes (e.g. B, KB, MiB,
%  etc.).
%
%  The format of the InterpretSiPrefixValue method is:
%
%      double InterpretSiPrefixValue(const char *value,char **sentinel)
%
%  A description of each parameter follows:
%
%    o value: the string value.
%
%    o sentinel:  if sentinel is not NULL, return a pointer to the character
%      after the last character used in the conversion.
%
*/
MagickExport double InterpretSiPrefixValue(const char *magick_restrict string,
  char **magick_restrict sentinel)
{
  char
    *q;

  double
    value;

  value=InterpretLocaleValue(string,&q);
  if (q != string)
    {
      if ((*q >= 'E') && (*q <= 'z'))
        {
          double
            e;

          switch ((int) ((unsigned char) *q))
          {
            case 'q': e=(-30.0); break;
            case 'r': e=(-27.0); break;
            case 'y': e=(-24.0); break;
            case 'z': e=(-21.0); break;
            case 'a': e=(-18.0); break;
            case 'f': e=(-15.0); break;
            case 'p': e=(-12.0); break;
            case 'n': e=(-9.0); break;
            case 'u': e=(-6.0); break;
            case 'm': e=(-3.0); break;
            case 'c': e=(-2.0); break;
            case 'd': e=(-1.0); break;
            case 'h': e=2.0; break;
            case 'k': e=3.0; break;
            case 'K': e=3.0; break;
            case 'M': e=6.0; break;
            case 'G': e=9.0; break;
            case 'T': e=12.0; break;
            case 'P': e=15.0; break;
            case 'E': e=18.0; break;
            case 'Z': e=21.0; break;
            case 'Y': e=24.0; break;
            case 'R': e=27.0; break;
            case 'Q': e=30.0; break;
            default: e=0.0; break;
          }
          if (e >= MagickEpsilon)
            {
              if (q[1] == 'i')
                {
                  value*=pow(2.0,e/0.3);
                  q+=(ptrdiff_t) 2;
                }
              else
                {
                  value*=pow(10.0,e);
                  q++;
                }
            }
        }
      if ((*q == 'B') || (*q == 'P'))
        q++;
    }
  if (sentinel != (char **) NULL)
    *sentinel=q;
  return(value);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   I s S t r i n g T r u e                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  IsStringTrue() returns MagickTrue if the value is "true", "on", "yes" or
%  "1". Any other string or undefined returns MagickFalse.
%
%  Typically this is used to look at strings (options or artifacts) which
%  has a default value of "false", when not defined.
%
%  The format of the IsStringTrue method is:
%
%      MagickBooleanType IsStringTrue(const char *value)
%
%  A description of each parameter follows:
%
%    o value: Specifies a pointer to a character array.
%
*/
MagickExport MagickBooleanType IsStringTrue(const char *value)
{
  if (value == (const char *) NULL)
    return(MagickFalse);
  if (LocaleCompare(value,"true") == 0)
    return(MagickTrue);
  if (LocaleCompare(value,"on") == 0)
    return(MagickTrue);
  if (LocaleCompare(value,"yes") == 0)
    return(MagickTrue);
  if (LocaleCompare(value,"1") == 0)
    return(MagickTrue);
  return(MagickFalse);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   I s S t r i n g F a l s e                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  IsStringFalse() returns MagickTrue if the value is "false", "off", "no" or
%  "0". Any other string or undefined returns MagickFalse.
%
%  Typically this is used to look at strings (options or artifacts) which
%  has a default value of "true", when it has not been defined.
%
%  The format of the IsStringFalse method is:
%
%      MagickBooleanType IsStringFalse(const char *value)
%
%  A description of each parameter follows:
%
%    o value: Specifies a pointer to a character array.
%
*/
MagickExport MagickBooleanType IsStringFalse(const char *value)
{
  if (value == (const char *) NULL)
    return(MagickFalse);
  if (LocaleCompare(value,"false") == 0)
    return(MagickTrue);
  if (LocaleCompare(value,"off") == 0)
    return(MagickTrue);
  if (LocaleCompare(value,"no") == 0)
    return(MagickTrue);
  if (LocaleCompare(value,"0") == 0)
    return(MagickTrue);
  return(MagickFalse);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   P r i n t S t r i n g I n f o                                             %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  PrintStringInfo() prints the string.
%
%  The format of the PrintStringInfo method is:
%
%      void PrintStringInfo(FILE *file,const char *id,
%        const StringInfo *string_info)
%
%  A description of each parameter follows:
%
%    o file: the file, typically stdout.
%
%    o id: the string id.
%
%    o string_info: the string info.
%
*/
MagickExport void PrintStringInfo(FILE *file,const char *id,
  const StringInfo *string_info)
{
  const char
    *p;

  size_t
    i,
    j;

  assert(id != (const char *) NULL);
  assert(string_info != (StringInfo *) NULL);
  assert(string_info->signature == MagickCoreSignature);
  p=(char *) string_info->datum;
  for (i=0; i < string_info->length; i++)
  {
    if (((int) ((unsigned char) *p) < 32) &&
        (isspace((int) ((unsigned char) *p)) == 0))
      break;
    p++;
  }
  (void) FormatLocaleFile(file,"%s(%.20g):\n",id,(double) string_info->length);
  if (i == string_info->length)
    {
      for (i=0; i < string_info->length; i++)
        (void) fputc(string_info->datum[i],file);
      (void) fputc('\n',file);
      return;
    }
  /*
    Convert string to a HEX list.
  */
  p=(char *) string_info->datum;
  for (i=0; i < string_info->length; i+=CharsPerLine)
  {
    (void) FormatLocaleFile(file,"0x%08lx: ",(unsigned long) (CharsPerLine*i));
    for (j=1; j <= MagickMin(string_info->length-i,CharsPerLine); j++)
    {
      (void) FormatLocaleFile(file,"%02lx",(unsigned long) (*(p+j)) & 0xff);
      if ((j % 0x04) == 0)
        (void) fputc(' ',file);
    }
    for ( ; j <= CharsPerLine; j++)
    {
      (void) fputc(' ',file);
      (void) fputc(' ',file);
      if ((j % 0x04) == 0)
        (void) fputc(' ',file);
    }
    (void) fputc(' ',file);
    for (j=1; j <= MagickMin(string_info->length-i,CharsPerLine); j++)
    {
      if (isprint((int) ((unsigned char) *p)) != 0)
        (void) fputc(*p,file);
      else
        (void) fputc('-',file);
      p++;
    }
    (void) fputc('\n',file);
  }
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e s e t S t r i n g I n f o                                             %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  ResetStringInfo() reset the string to all null bytes.
%
%  The format of the ResetStringInfo method is:
%
%      void ResetStringInfo(StringInfo *string_info)
%
%  A description of each parameter follows:
%
%    o string_info: the string info.
%
*/
MagickExport void ResetStringInfo(StringInfo *string_info)
{
  assert(string_info != (StringInfo *) NULL);
  assert(string_info->signature == MagickCoreSignature);
  (void) memset(string_info->datum,0,string_info->length);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   S a n t i z e S t r i n g                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  SanitizeString() returns a new string with all characters removed except
%  letters, digits and !#$%&'*+-=?^_`{|}~@.[].
%
%  Free the sanitized string with DestroyString().
%
%  The format of the SanitizeString method is:
%
%      char *SanitizeString(const char *source)
%
%  A description of each parameter follows:
%
%    o source: A character string.
%
*/
MagickExport char *SanitizeString(const char *source)
{
  char
    *sanitize_source;

  const char
    *q;

  char
    *p;

  static char
    allowlist[] =
      "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789 "
      "$-_.+!*'(),{}|\\^~[]`\"><#%;/?:@&=";

  sanitize_source=AcquireString(source);
  p=sanitize_source;
  q=sanitize_source+strlen(sanitize_source);
  for (p+=strspn(p,allowlist); p != q; p+=(ptrdiff_t) strspn(p,allowlist))
    *p='_';
  return(sanitize_source);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   S e t S t r i n g I n f o                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  SetStringInfo() copies the source string to the destination string.
%
%  The format of the SetStringInfo method is:
%
%      void SetStringInfo(StringInfo *string_info,const StringInfo *source)
%
%  A description of each parameter follows:
%
%    o string_info: the string info.
%
%    o source: the source string.
%
*/
MagickExport void SetStringInfo(StringInfo *string_info,
  const StringInfo *source)
{
  assert(string_info != (StringInfo *) NULL);
  assert(string_info->signature == MagickCoreSignature);
  assert(source != (StringInfo *) NULL);
  assert(source->signature == MagickCoreSignature);
  if (string_info->length == 0)
    return;
  (void) memset(string_info->datum,0,string_info->length);
  (void) memcpy(string_info->datum,source->datum,MagickMin(string_info->length,
    source->length));
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   S e t S t r i n g I n f o D a t u m                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  SetStringInfoDatum() copies bytes from the source string for the length of
%  the destination string.
%
%  The format of the SetStringInfoDatum method is:
%
%      void SetStringInfoDatum(StringInfo *string_info,
%        const unsigned char *source)
%
%  A description of each parameter follows:
%
%    o string_info: the string info.
%
%    o source: the source string.
%
*/
MagickExport void SetStringInfoDatum(StringInfo *string_info,
  const unsigned char *source)
{
  assert(string_info != (StringInfo *) NULL);
  assert(string_info->signature == MagickCoreSignature);
  if (string_info->length != 0)
    (void) memcpy(string_info->datum,source,string_info->length);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   S e t S t r i n g I n f o L e n g t h                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  SetStringInfoLength() set the string length to the specified value.
%
%  The format of the SetStringInfoLength method is:
%
%      void SetStringInfoLength(StringInfo *string_info,const size_t length)
%
%  A description of each parameter follows:
%
%    o string_info: the string info.
%
%    o length: the string length.
%
*/
MagickExport void SetStringInfoLength(StringInfo *string_info,
  const size_t length)
{
  assert(string_info != (StringInfo *) NULL);
  assert(string_info->signature == MagickCoreSignature);
  if (string_info->length == length)
    return;
  if (~length < MagickPathExtent)
    ThrowFatalException(ResourceLimitFatalError,"MemoryAllocationFailed");
  string_info->length=length;
  if (string_info->datum == (unsigned char *) NULL)
    string_info->datum=(unsigned char *) AcquireQuantumMemory(length+
      MagickPathExtent,sizeof(*string_info->datum));
  else
    string_info->datum=(unsigned char *) ResizeQuantumMemory(string_info->datum,
      length+MagickPathExtent,sizeof(*string_info->datum));
  if (string_info->datum == (unsigned char *) NULL)
    ThrowFatalException(ResourceLimitFatalError,"MemoryAllocationFailed");
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   S e t S t r i n g I n f o N a m e                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  SetStringInfoName() sets the name associated with the string.
%
%  The format of the SetStringInfoName method is:
%
%      void SetStringInfoName(StringInfo *string_info,const char *name)
%
%  A description of each parameter follows:
%
%    o string_info: the string info.
%
%    o name: the name.
%
*/
MagickExport void SetStringInfoName(StringInfo *string_info,const char *name)
{
  assert(string_info != (StringInfo *) NULL);
  assert(string_info->signature == MagickCoreSignature);
  assert(name != (const char *) NULL);
  string_info->name=ConstantString(name);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   S e t S t r i n g I n f o P a t h                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  SetStringInfoPath() sets the path associated with the string.
%
%  The format of the SetStringInfoPath method is:
%
%      void SetStringInfoPath(StringInfo *string_info,const char *path)
%
%  A description of each parameter follows:
%
%    o string_info: the string info.
%
%    o path: the path.
%
*/
MagickExport void SetStringInfoPath(StringInfo *string_info,const char *path)
{
  assert(string_info != (StringInfo *) NULL);
  assert(string_info->signature == MagickCoreSignature);
  assert(path != (const char *) NULL);
  string_info->path=ConstantString(path);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   S p l i t S t r i n g I n f o                                             %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  SplitStringInfo() splits a string into two and returns it.
%
%  The format of the SplitStringInfo method is:
%
%      StringInfo *SplitStringInfo(StringInfo *string_info,const size_t offset)
%
%  A description of each parameter follows:
%
%    o string_info: the string info.
%
*/
MagickExport StringInfo *SplitStringInfo(StringInfo *string_info,
  const size_t offset)
{
  StringInfo
    *split_info;

  assert(string_info != (StringInfo *) NULL);
  assert(string_info->signature == MagickCoreSignature);
  if (offset > string_info->length)
    return((StringInfo *) NULL);
  split_info=AcquireStringInfo(offset);
  SetStringInfo(split_info,string_info);
  (void) memmove(string_info->datum,string_info->datum+offset,
    string_info->length-offset+MagickPathExtent);
  SetStringInfoLength(string_info,string_info->length-offset);
  return(split_info);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   S t r i n g I n f o T o D i g e s t                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  StringInfoToDigest() converts a string info string to a hex digest.
%
%  The format of the StringInfoToString method is:
%
%      char *StringInfoToDigest(const StringInfo *signature)
%
%  A description of each parameter follows:
%
%    o string_info: the string.
%
*/
MagickExport char *StringInfoToDigest(const StringInfo *signature)
{
  char
    *digest;

  SignatureInfo
    *signature_info;

  signature_info=AcquireSignatureInfo();
  UpdateSignature(signature_info,signature);
  FinalizeSignature(signature_info);
  digest=StringInfoToHexString(GetSignatureDigest(signature_info));
  signature_info=DestroySignatureInfo(signature_info);
  return(digest);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   S t r i n g I n f o T o H e x S t r i n g                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  StringInfoToHexString() converts a string info string to a C string.
%
%  The format of the StringInfoToHexString method is:
%
%      char *StringInfoToHexString(const StringInfo *string_info)
%
%  A description of each parameter follows:
%
%    o string_info: the string.
%
*/
MagickExport char *StringInfoToHexString(const StringInfo *string_info)
{
  char
    *string;

  const unsigned char
    *p;

  ssize_t
    i;

  unsigned char
    *q;

  size_t
    length;

  unsigned char
    hex_digits[16];

  length=string_info->length;
  if (~length < MagickPathExtent)
    ThrowFatalException(ResourceLimitFatalError,"UnableToAcquireString");
  string=(char *) AcquireQuantumMemory(length+MagickPathExtent,2*
    sizeof(*string));
  if (string == (char *) NULL)
    ThrowFatalException(ResourceLimitFatalError,"UnableToAcquireString");
  hex_digits[0]='0';
  hex_digits[1]='1';
  hex_digits[2]='2';
  hex_digits[3]='3';
  hex_digits[4]='4';
  hex_digits[5]='5';
  hex_digits[6]='6';
  hex_digits[7]='7';
  hex_digits[8]='8';
  hex_digits[9]='9';
  hex_digits[10]='a';
  hex_digits[11]='b';
  hex_digits[12]='c';
  hex_digits[13]='d';
  hex_digits[14]='e';
  hex_digits[15]='f';
  p=string_info->datum;
  q=(unsigned char *) string;
  for (i=0; i < (ssize_t) string_info->length; i++)
  {
    *q++=hex_digits[(*p >> 4) & 0x0f];
    *q++=hex_digits[*p & 0x0f];
    p++;
  }
  *q='\0';
  return(string);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   S t r i n g I n f o T o S t r i n g                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  StringInfoToString() converts a string info string to a C string.
%
%  The format of the StringInfoToString method is:
%
%      char *StringInfoToString(const StringInfo *string_info)
%
%  A description of each parameter follows:
%
%    o string_info: the string.
%
*/
MagickExport char *StringInfoToString(const StringInfo *string_info)
{
  char
    *string;

  size_t
    length;

  string=(char *) NULL;
  length=string_info->length;
  if (~length >= (MagickPathExtent-1))
    string=(char *) AcquireQuantumMemory(length+MagickPathExtent,
      sizeof(*string));
  if (string == (char *) NULL)
    return((char *) NULL);
  (void) memcpy(string,(char *) string_info->datum,length*sizeof(*string));
  string[length]='\0';
  return(string);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%  S t r i n g T o A r g v                                                    %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  StringToArgv() converts a text string into command line arguments.
%  The 'argv' array of arguments, is returned while the number of arguments
%  is returned via the provided integer variable pointer.
%
%  Simple 'word' tokenizer, which allows for each word to be optionally
%  quoted.  However it will not allow use of partial quotes, or escape
%  characters.
%
%  The format of the StringToArgv method is:
%
%      char **StringToArgv(const char *text,int *argc)
%
%  A description of each parameter follows:
%
%    o argv:  Method StringToArgv returns the string list unless an error
%      occurs, otherwise NULL.
%
%    o text:  Specifies the string to segment into a list.
%
%    o argc:  This integer pointer returns the number of arguments in the
%      list.
%
*/
MagickExport char **StringToArgv(const char *text,int *argc)
{
  char
    **argv;

  const char
    *p,
    *q;

  ssize_t
    i;

  *argc=0;
  if (text == (char *) NULL)
    return((char **) NULL);
  /*
    Determine the number of arguments.
  */
  for (p=text; *p != '\0'; )
  {
    while (isspace((int) ((unsigned char) *p)) != 0)
      p++;
    if (*p == '\0')
      break;
    (*argc)++;
    if (*p == '"')
      for (p++; (*p != '"') && (*p != '\0'); p++) ;
    if (*p == '\'')
      for (p++; (*p != '\'') && (*p != '\0'); p++) ;
    while ((isspace((int) ((unsigned char) *p)) == 0) && (*p != '\0'))
      p++;
  }
  (*argc)++;
  argv=(char **) AcquireQuantumMemory((size_t) *argc+1UL,sizeof(*argv));
  if (argv == (char **) NULL)
    ThrowFatalException(ResourceLimitFatalError,"UnableToConvertStringToARGV");
  /*
    Convert string to an ASCII list.
  */
  argv[0]=AcquireString("magick");
  p=text;
  for (i=1; i < (ssize_t) *argc; i++)
  {
    while (isspace((int) ((unsigned char) *p)) != 0)
      p++;
    q=p;
    if (*q == '"')
      {
        p++;
        for (q++; (*q != '"') && (*q != '\0'); q++) ;
      }
    else
      if (*q == '\'')
        {
          p++;
          for (q++; (*q != '\'') && (*q != '\0'); q++) ;
        }
      else
        while ((isspace((int) ((unsigned char) *q)) == 0) && (*q != '\0'))
          q++;
    argv[i]=(char *) AcquireQuantumMemory((size_t) (q-p)+MagickPathExtent,
      sizeof(**argv));
    if (argv[i] == (char *) NULL)
      {
        for (i--; i >= 0; i--)
          argv[i]=DestroyString(argv[i]);
        argv=(char **) RelinquishMagickMemory(argv);
        ThrowFatalException(ResourceLimitFatalError,
          "UnableToConvertStringToARGV");
      }
    (void) memcpy(argv[i],p,(size_t) (q-p));
    argv[i][q-p]='\0';
    p=q;
    while ((isspace((int) ((unsigned char) *p)) == 0) && (*p != '\0'))
      p++;
  }
  argv[i]=(char *) NULL;
  return(argv);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   S t r i n g T o A r r a y O f D o u b l e s                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  StringToArrayOfDoubles() converts a string of space or comma separated
%  numbers into array of floating point numbers (doubles). Any number that
%  fails to parse properly will produce a syntax error. As will two commas
%  without a  number between them.  However a final comma at the end will
%  not be regarded as an error so as to simplify automatic list generation.
%
%  A NULL value is returned on syntax or memory errors.
%
%  Use RelinquishMagickMemory() to free returned array when finished.
%
%  The format of the StringToArrayOfDoubles method is:
%
%     double *StringToArrayOfDoubles(const char *string,size_t *count,
%       ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o string: the string containing the comma/space separated values.
%
%    o count: returns number of arguments in returned array
%
%    o exception: return any errors or warnings in this structure.
%
*/
MagickExport double *StringToArrayOfDoubles(const char *string,ssize_t *count,
  ExceptionInfo *exception)
{
  char
    *q;

  const char
    *p;

  double
    *array;

  ssize_t
    i;

  /*
    Determine count of values, and check syntax.
  */
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickCoreSignature);
  *count=0;
  if (string == (char *) NULL)
    return((double *) NULL);  /* no value found */
  i=0;
  p=string;
  while (*p != '\0')
  {
    (void) StringToDouble(p,&q);  /* get value - ignores leading space */
    if (p == q)
      return((double *) NULL);  /* no value found */
    p=q;
    i++;  /* increment value count */
    while (isspace((int) ((unsigned char) *p)) != 0)
      p++;  /* skip spaces */
    if (*p == ',')
      p++;  /* skip comma */
    while (isspace((int) ((unsigned char) *p)) != 0)
      p++;  /* and more spaces */
  }
  /*
    Allocate floating point argument list.
  */
  *count=i;
  array=(double *) AcquireQuantumMemory((size_t) i,sizeof(*array));
  if (array == (double *) NULL)
    {
      (void) ThrowMagickException(exception,GetMagickModule(),
        ResourceLimitError,"MemoryAllocationFailed","`%s'","");
      return((double *) NULL);
    }
  /*
    Fill in the floating point values.
  */
  i=0;
  p=string;
  while ((*p != '\0') && (i < *count))
  {
    array[i++]=StringToDouble(p,&q);
    p=q;
    while ((isspace((int) ((unsigned char) *p)) != 0) || (*p == ','))
      p++;
  }
  return(array);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   S t r i n g T o k e n                                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  StringToken() looks for any one of given delimiters and splits the string
%  into two separate strings by replacing the delimiter character found with a
%  null character.
%
%  The given string pointer is changed to point to the string following the
%  delimiter character found, or NULL.  A pointer to the start of the
%  string is returned, representing the token before the delimiter.
%
%  StringToken() is similar to the strtok() C library method, but with
%  multiple delimiter characters rather than a delimiter string.
%
%  The format of the StringToken method is:
%
%      char *StringToken(const char *delimiters,char **string)
%
%  A description of each parameter follows:
%
%    o delimiters: one or more delimiters.
%
%    o string: return the first token in the string.  If none is found, return
%      NULL.
%
*/
MagickExport char *StringToken(const char *delimiters,char **string)
{
  char
    *q;

  char
    *p;

  const char
    *r;

  int
    c,
    d;

  p=(*string);
  if (p == (char *) NULL)
    return((char *) NULL);
  q=p;
  for ( ; ; )
  {
    c=(*p++);
    r=delimiters;
    do
    {
      d=(*r++);
      if (c == d)
        {
          if (c == '\0')
            p=(char *) NULL;
          else
            p[-1]='\0';
          *string=p;
          return(q);
        }
    } while (d != '\0');
  }
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%  S t r i n g T o L i s t                                                    %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  StringToList() converts a text string into a list by segmenting the text
%  string at each carriage return discovered.  The list is converted to HEX
%  characters if any control characters are discovered within the text string.
%
%  The format of the StringToList method is:
%
%      char **StringToList(const char *text)
%
%  A description of each parameter follows:
%
%    o text:  Specifies the string to segment into a list.
%
*/
MagickExport char **StringToList(const char *text)
{
  return(StringToStrings(text,(size_t *) NULL));
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%  S t r i n g T o S t r i n g s                                              %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  StringToStrings() converts a text string into a list by segmenting the text
%  string at each carriage return discovered.  The list is converted to HEX
%  characters if any control characters are discovered within the text string.
%
%  The format of the StringToList method is:
%
%      char **StringToList(const char *text,size_t *lines)
%
%  A description of each parameter follows:
%
%    o text:  Specifies the string to segment into a list.
%
%    o count: Return value for the number of items in the list.
%
*/
MagickExport char **StringToStrings(const char *text,size_t *count)
{
  char
    **textlist;

  const char
    *p;

  ssize_t
    i;

  size_t
    lines;

  if (text == (char *) NULL)
    {
      if (count != (size_t *) NULL)
        *count=0;
      return((char **) NULL);
    }
  for (p=text; *p != '\0'; p++)
    if (((int) ((unsigned char) *p) < 32) &&
        (isspace((int) ((unsigned char) *p)) == 0))
      break;
  if (*p == '\0')
    {
      const char
        *q;

      /*
        Convert string to an ASCII list.
      */
      lines=1;
      for (p=text; *p != '\0'; p++)
        if (*p == '\n')
          lines++;
      textlist=(char **) AcquireQuantumMemory((size_t) lines+1UL,
        sizeof(*textlist));
      if (textlist == (char **) NULL)
        ThrowFatalException(ResourceLimitFatalError,"UnableToConvertText");
      p=text;
      for (i=0; i < (ssize_t) lines; i++)
      {
        for (q=p; *q != '\0'; q++)
          if ((*q == '\r') || (*q == '\n'))
            break;
        textlist[i]=(char *) AcquireQuantumMemory((size_t) (q-p)+1,
          sizeof(**textlist));
        if (textlist[i] == (char *) NULL)
          ThrowFatalException(ResourceLimitFatalError,"UnableToConvertText");
        (void) memcpy(textlist[i],p,(size_t) (q-p));
        textlist[i][q-p]='\0';
        if (*q == '\r')
          q++;
        p=q+1;
      }
    }
  else
    {
      char
        hex_string[MagickPathExtent];

      char
        *q;

      ssize_t
        j;

      /*
        Convert string to a HEX list.
      */
      lines=(size_t) (strlen(text)/CharsPerLine)+1;
      textlist=(char **) AcquireQuantumMemory((size_t) lines+1UL,
        sizeof(*textlist));
      if (textlist == (char **) NULL)
        ThrowFatalException(ResourceLimitFatalError,"UnableToConvertText");
      p=text;
      for (i=0; i < (ssize_t) lines; i++)
      {
        size_t
          length;

        textlist[i]=(char *) AcquireQuantumMemory(2UL*MagickPathExtent,
          sizeof(**textlist));
        if (textlist[i] == (char *) NULL)
          ThrowFatalException(ResourceLimitFatalError,"UnableToConvertText");
        (void) FormatLocaleString(textlist[i],MagickPathExtent,"0x%08lx: ",
          (long) (CharsPerLine*i));
        q=textlist[i]+strlen(textlist[i]);
        length=strlen(p);
        for (j=1; j <= (ssize_t) MagickMin(length,CharsPerLine); j++)
        {
          (void) FormatLocaleString(hex_string,MagickPathExtent,"%02x",*(p+j));
          (void) CopyMagickString(q,hex_string,MagickPathExtent);
          q+=(ptrdiff_t) 2;
          if ((j % 0x04) == 0)
            *q++=' ';
        }
        for ( ; j <= CharsPerLine; j++)
        {
          *q++=' ';
          *q++=' ';
          if ((j % 0x04) == 0)
            *q++=' ';
        }
        *q++=' ';
        for (j=1; j <= (ssize_t) MagickMin(length,CharsPerLine); j++)
        {
          if (isprint((int) ((unsigned char) *p)) != 0)
            *q++=(*p);
          else
            *q++='-';
          p++;
        }
        *q='\0';
        textlist[i]=(char *) ResizeQuantumMemory(textlist[i],(size_t) (q-
          textlist[i]+1),sizeof(**textlist));
        if (textlist[i] == (char *) NULL)
          ThrowFatalException(ResourceLimitFatalError,"UnableToConvertText");
      }
    }
  if (count != (size_t *) NULL)
    *count=lines;
  textlist[i]=(char *) NULL;
  return(textlist);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   S t r i n g T o S t r i n g I n f o                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  StringToStringInfo() converts a string to a StringInfo type.
%
%  The format of the StringToStringInfo method is:
%
%      StringInfo *StringToStringInfo(const char *string)
%
%  A description of each parameter follows:
%
%    o string:  The string.
%
*/
MagickExport StringInfo *StringToStringInfo(const char *string)
{
  StringInfo
    *string_info;

  assert(string != (const char *) NULL);
  string_info=AcquireStringInfo(strlen(string));
  SetStringInfoDatum(string_info,(const unsigned char *) string);
  return(string_info);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   S t r i p M a g i c k S t r i n g                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  StripMagickString() strips any whitespace or quotes from the beginning and
%  end of a string of characters. It returns the stripped string length.
%
%  The format of the StripMagickString method is:
%
%      size_t StripMagickString(char *message)
%
%  A description of each parameter follows:
%
%    o message: Specifies an array of characters.
%
*/

MagickExport void StripString(char *message)
{
  (void) StripMagickString(message);
}

MagickExport size_t StripMagickString(char *message)
{
  char
    *p,
    *q;

  size_t
    length;

  assert(message != (char *) NULL);
  if (*message == '\0')
    return(0);
  length=strlen(message);
  if (length == 1)
    return(1);
  p=message;
  while (isspace((int) ((unsigned char) *p)) != 0)
    p++;
  if ((*p == '\'') || (*p == '"'))
    p++;
  q=message+length-1;
  while ((isspace((int) ((unsigned char) *q)) != 0) && (q > p))
    q--;
  if (q > p)
    if ((*q == '\'') || (*q == '"'))
      q--;
  (void) memmove(message,p,(size_t) (q-p+1));
  message[q-p+1]='\0';
  for (p=message; *p != '\0'; p++)
    if (*p == '\n')
      *p=' ';
  return((size_t) (q-p+1));
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   S u b s t i t u t e S t r i n g                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  SubstituteString() performs string substitution on a string, replacing the
%  string with the substituted version. Buffer must be allocated from the heap.
%  If the string is matched and status, MagickTrue is returned otherwise
%  MagickFalse.
%
%  The format of the SubstituteString method is:
%
%      MagickBooleanType SubstituteString(char **string,const char *search,
%        const char *replace)
%
%  A description of each parameter follows:
%
%    o string: the string to perform replacements on;  replaced with new
%      allocation if a replacement is made.
%
%    o search: search for this string.
%
%    o replace: replace any matches with this string.
%
*/
MagickExport MagickBooleanType SubstituteString(char **string,
  const char *search,const char *replace)
{
  MagickBooleanType
    status;

  char
    *p;

  size_t
    extent,
    replace_extent,
    search_extent;

  ssize_t
    offset;

  status=MagickFalse;
  search_extent=0,
  replace_extent=0;
  for (p=strchr(*string,*search); p != (char *) NULL; p=strchr(p+1,*search))
  {
    if (search_extent == 0)
      search_extent=strlen(search);
    if (strncmp(p,search,search_extent) != 0)
      continue;
    /*
      We found a match.
    */
    status=MagickTrue;
    if (replace_extent == 0)
      replace_extent=strlen(replace);
    if (replace_extent > search_extent)
      {
        /*
          Make room for the replacement string.
        */
        offset=(ssize_t) (p-(*string));
        extent=strlen(*string)+replace_extent-search_extent+1;
        *string=(char *) ResizeQuantumMemory(*string,
          OverAllocateMemory(extent+MagickPathExtent),sizeof(*p));
        if (*string == (char *) NULL)
          ThrowFatalException(ResourceLimitFatalError,"UnableToAcquireString");
        p=(*string)+offset;
      }
    /*
      Replace string.
    */
    if (search_extent != replace_extent)
      (void) memmove(p+replace_extent,p+search_extent,
        strlen(p+search_extent)+1);
    (void) memcpy(p,replace,replace_extent);
    p+=(ptrdiff_t) replace_extent-1;
  }
  return(status);
}
