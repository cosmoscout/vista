/*============================================================================*/
/*       1         2         3         4         5         6         7        */
/*3456789012345678901234567890123456789012345678901234567890123456789012345678*/
/*============================================================================*/
/*                                             .                              */
/*                                               RRRR WW  WW   WTTTTTTHH  HH  */
/*                                               RR RR WW WWW  W  TT  HH  HH  */
/*      FileName :                               RRRR   WWWWWWWW  TT  HHHHHH  */
/*                                               RR RR   WWW WWW  TT  HH  HH  */
/*      Module   :  KERNEL                       RR  R    WW  WW  TT  HH  HH  */
/*                                                                            */
/*      Project  :  ViSTA                          Rheinisch-Westfaelische    */
/*                                               Technische Hochschule Aachen */
/*      Purpose  :                                                            */
/*                                                                            */
/*                                                 Copyright (c)  1998-2016   */
/*                                                 by  RWTH-Aachen, Germany   */
/*                                                 All rights reserved.       */
/*                                             .                              */
/*============================================================================*/

#include "OpenALSoundContext.h"
#include <algorithm>
#include <exception>
#include <iostream>

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

namespace {
class _cNoContextException : public std::exception {
 public:
  _cNoContextException(const char* pcDescription)
      : m_pcDesc(pcDescription) {
  }

  virtual const char* what() const throw() {
    return m_pcDesc;
  }

 private:
  const char* m_pcDesc;
};

std::list<std::string> getTokenizedList(const char* stringlist) {
  std::list<std::string> l;
  while (*stringlist) {
    std::string dev;
    while (*stringlist)
      dev.push_back(*stringlist++);
    l.push_back(dev);

    if (*(stringlist + 1) == 0x0)
      break;      // leave while loop, we reached the last entry (double 0)
    ++stringlist; // else: advance again to the next string.
  }
  return l;
}
} // namespace

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/

OpenALSoundContext::OpenALSoundContext()
    : m_bGetEnv(false) {
  m_pALCDevice = alcOpenDevice(NULL);
  if (!m_pALCDevice)
    throw _cNoContextException("could not open sound device.");
}

OpenALSoundContext::~OpenALSoundContext() {
  for (std::map<std::string, OpenALSoundContext::OpenALSoundEnvironment*>::iterator it =
           m_mpEnvs.begin();
       it != m_mpEnvs.end(); ++it) {
    delete (*it).second;
  }

  for (std::map<std::string, OpenALBuffer*>::iterator bit = m_mpBuffers.begin();
       bit != m_mpBuffers.end(); ++bit) {
    delete (*bit).second;
  }

  if (m_pALCDevice)
    alcCloseDevice(m_pALCDevice);
}

std::string OpenALSoundContext::GetVendor() const {
  return m_pcVendor;
}

std::string OpenALSoundContext::GetVersion() const {
  return m_pcVersion;
}

std::string OpenALSoundContext::GetRenderer() const {
  return m_pcRenderer;
}

std::string OpenALSoundContext::GetExtensions() const {
  return m_pcExtensions;
}

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/

OpenALSoundContext::OpenALSoundEnvironment* OpenALSoundContext::CreateAndAddSoundEnv(
    const std::string& sEnvName) {
  if (m_mpEnvs.find(sEnvName) != m_mpEnvs.end()) {
    return NULL;
  }

  OpenALSoundEnvironment* env = new OpenALSoundEnvironment(m_pALCDevice);
  m_mpEnvs[sEnvName]          = env;

  if (!m_bGetEnv) {
    m_bGetEnv = true;
    alcMakeContextCurrent(env->m_pALCContext);

    m_pcVendor     = alGetString(AL_VENDOR);
    m_pcVersion    = alGetString(AL_VERSION);
    m_pcRenderer   = alGetString(AL_RENDERER);
    m_pcExtensions = alGetString(AL_EXTENSIONS);
  }

  return env;
}

OpenALSoundContext::OpenALSoundEnvironment* OpenALSoundContext::GetSoundEnvironment(
    const std::string& sEnvName) const {
  std::map<std::string, OpenALSoundEnvironment*>::const_iterator cit;
  if ((cit = m_mpEnvs.find(sEnvName)) == m_mpEnvs.end()) {
    return NULL;
  }

  return (*cit).second;
}

bool OpenALSoundContext::PrintState() const {
  return true;
}

bool OpenALSoundContext::AddBuffer(OpenALBuffer* buf) {
  if (!buf)
    return false;

  if (buf->GetBufferName().empty())
    return false;
  if (m_mpBuffers.find(buf->GetBufferName()) == m_mpBuffers.end()) {
    m_mpBuffers.insert(std::map<std::string, OpenALBuffer*>::value_type(buf->GetBufferName(), buf));
    return true;
  }

  return false;
}

bool OpenALSoundContext::RemBuffer(OpenALBuffer* buf) {
  std::map<std::string, OpenALBuffer*>::iterator it;
  if ((it = m_mpBuffers.find(buf->GetBufferName())) == m_mpBuffers.end()) {
    return true;
  }
  delete (*it).second;

  m_mpBuffers.erase(it);
  return true;
}

OpenALSoundContext::OpenALBuffer* OpenALSoundContext::GetBuffer(const std::string& sName) const {
  std::map<std::string, OpenALBuffer*>::const_iterator it;
  if ((it = m_mpBuffers.find(sName)) == m_mpBuffers.end()) {
    return NULL;
  }

  return (*it).second;
}

OpenALSoundContext::OpenALBuffer* OpenALSoundContext::GetBuffer(ALuint nBufferId) const {
  for (std::map<std::string, OpenALBuffer*>::const_iterator cit = m_mpBuffers.begin();
       cit != m_mpBuffers.end(); ++cit) {
    if ((*cit).second->GetBufferId() == nBufferId)
      return (*cit).second;
  }
  return NULL;
}

// ###################################################################

OpenALSoundContext::OpenALSoundEnvironment::OpenALSoundEnvironment(ALCdevice* pDev) {
  m_pALCContext = alcCreateContext(pDev, NULL);
  if (!m_pALCContext)
    throw _cNoContextException("could not create sound context.\n");
  m_pListener = new OpenALVirtualListener;
  m_pListener->SetEnvironment(this);
}

OpenALSoundContext::OpenALSoundEnvironment::~OpenALSoundEnvironment() {
  delete m_pListener;
  for (std::vector<OpenALVirtualSound*>::iterator it = m_mpSounds.begin(); it != m_mpSounds.end();
       ++it) {
    delete *it;
  }

  if (m_pALCContext) {
    alcMakeContextCurrent(NULL);
    alcDestroyContext(m_pALCContext);
  }
}

OpenALSoundContext::OpenALVirtualListener*
OpenALSoundContext::OpenALSoundEnvironment::GetListener() const {
  return m_pListener;
}

bool OpenALSoundContext::OpenALSoundEnvironment::MakeCurrent() {
  if (alcGetCurrentContext() == m_pALCContext)
    return true;

  alcMakeContextCurrent(m_pALCContext);

  return true;
}

OpenALSoundContext::OpenALVirtualSound*
OpenALSoundContext::OpenALSoundEnvironment::CreateAndAddSound() {
  m_mpSounds.push_back(new OpenALVirtualSound(this));
  return m_mpSounds.back();
}

bool OpenALSoundContext::OpenALSoundEnvironment::DeleteSound(OpenALVirtualSound* vs) {
  std::vector<OpenALVirtualSound*>::iterator it =
      std::find(m_mpSounds.begin(), m_mpSounds.end(), vs);
  if (it == m_mpSounds.end())
    return false;

  m_mpSounds.erase(it);
  delete vs;
  return true;
}

bool OpenALSoundContext::OpenALSoundEnvironment::RemoveSound(OpenALVirtualSound* vs) {
  std::vector<OpenALVirtualSound*>::iterator it =
      std::find(m_mpSounds.begin(), m_mpSounds.end(), vs);
  if (it == m_mpSounds.end())
    return false;

  m_mpSounds.erase(it);
  return true;
}

bool OpenALSoundContext::OpenALSoundEnvironment::AddSound(OpenALVirtualSound* vs) {
  // migrating sounds
  vs->SetEnvironment(this);
  m_mpSounds.push_back(vs);
  return true;
}

bool OpenALSoundContext::OpenALSoundEnvironment::HasBuffer(OpenALBuffer* pBuf) const {
  for (std::vector<OpenALVirtualSound*>::const_iterator cit = m_mpSounds.begin();
       cit != m_mpSounds.end(); ++cit) {
    if ((*cit)->GetBufferId() == pBuf->GetBufferId())
      return true;
  }

  return false;
}

// ###################################################################

bool OpenALSoundContext::OpenALVirtualListener::UpdateListenerWCS(
    float pos[3], float view[3], float up[3]) {
  return UpdateListenerWCS(pos[0], pos[1], pos[2], view[0], view[1], view[2], up[0], up[1], up[2]);
}

bool OpenALSoundContext::OpenALVirtualListener::UpdateListenerWCSPos(float pos[3]) {
  return UpdateListenerWCSPos(pos[0], pos[1], pos[2]);
}

bool OpenALSoundContext::OpenALVirtualListener::UpdateListenerWCSView(float view[3]) {
  return UpdateListenerWCSView(view[0], view[1], view[2]);
}

bool OpenALSoundContext::OpenALVirtualListener::UpdateListenerWCSUp(float up[3]) {
  return UpdateListenerWCSUp(up[0], up[1], up[2]);
}

bool OpenALSoundContext::OpenALVirtualListener::UpdateListenerWCS(float x, float y, float z,
    float viewx, float viewy, float viewz, float upx, float upy, float upz) {
  m_pEnv->MakeCurrent();
  alListener3f(AL_POSITION, x, y, z);

  float ori[6];
  ori[0] = viewx;
  ori[1] = viewy;
  ori[2] = viewz;
  ori[3] = upx;
  ori[4] = upy;
  ori[5] = upz;

  alListenerfv(AL_ORIENTATION, ori);

  return true;
}

bool OpenALSoundContext::OpenALVirtualListener::UpdateListenerWCSPos(float x, float y, float z) {
  m_pEnv->MakeCurrent();
  alListener3f(AL_POSITION, x, y, z);
  return true;
}

bool OpenALSoundContext::OpenALVirtualListener::UpdateListenerWCSView(
    float viewx, float viewy, float viewz) {
  float up[6];
  m_pEnv->MakeCurrent();
  alGetListenerfv(AL_ORIENTATION, up);
  up[0] = viewx;
  up[1] = viewy;
  up[2] = viewz;

  alListenerfv(AL_ORIENTATION, up);

  return true;
}

bool OpenALSoundContext::OpenALVirtualListener::UpdateListenerWCSUp(
    float upx, float upy, float upz) {
  float up[6];
  m_pEnv->MakeCurrent();
  alGetListenerfv(AL_ORIENTATION, up);
  up[3] = upx;
  up[4] = upy;
  up[5] = upz;

  alListenerfv(AL_ORIENTATION, up);
  return true;
}

bool OpenALSoundContext::OpenALVirtualListener::UpdateListenerVel(
    float velx, float vely, float velz) {
  m_pEnv->MakeCurrent();
  alListener3f(AL_VELOCITY, velx, vely, velz);
  return true;
}

bool OpenALSoundContext::OpenALVirtualListener::UpdateListenerVel(float vel[3]) {
  return UpdateListenerVel(vel[0], vel[1], vel[2]);
}

void OpenALSoundContext::OpenALVirtualListener::SetListenerGain(float fGain) {
  m_pEnv->MakeCurrent();
  alListenerf(AL_GAIN, fGain);
}

float OpenALSoundContext::OpenALVirtualListener::GetListenerGain() const {
  ALfloat fGain = 0.0f;
  m_pEnv->MakeCurrent();
  alGetListenerf(AL_GAIN, &fGain);
  return fGain;
}

void OpenALSoundContext::OpenALVirtualListener::SetEnvironment(OpenALSoundEnvironment* pEnv) {
  m_pEnv = pEnv;
}

OpenALSoundContext::OpenALSoundEnvironment*
OpenALSoundContext::OpenALVirtualListener::GetEnvironment() const {
  return m_pEnv;
}

OpenALSoundContext::OpenALVirtualListener::OpenALVirtualListener() {
  m_pEnv = NULL;
}

OpenALSoundContext::OpenALVirtualListener::~OpenALVirtualListener() {
  m_pEnv = NULL;
}

// #########################################################################
OpenALSoundContext::OpenALSound::OpenALSound(OpenALSoundEnvironment* pEnv)
    : m_pEnv(pEnv)
    , m_nMuteGain(-1)
    , m_ePlayState(PS_NONE) {
  m_pEnv->MakeCurrent();
  alGenSources(1, &m_nSndId);
}

OpenALSoundContext::OpenALSound::~OpenALSound() {
  m_pEnv->MakeCurrent();
  alSourcei(m_nSndId, AL_BUFFER, 0); // detach from any sound buffer
  alDeleteSources(1, &m_nSndId);
}

OpenALSoundContext::OpenALSound::OpenALSound(const OpenALSound& other) {
  m_pEnv       = other.m_pEnv;
  m_nSndId     = other.m_nSndId;
  m_nMuteGain  = other.m_nMuteGain;
  m_ePlayState = other.m_ePlayState;
}

OpenALSoundContext::OpenALSound& OpenALSoundContext::OpenALSound::operator=(
    const OpenALSoundContext::OpenALSound& other) {
  m_pEnv       = other.m_pEnv;
  m_nSndId     = other.m_nSndId;
  m_nMuteGain  = other.m_nMuteGain;
  m_ePlayState = other.m_ePlayState;

  return *this;
}

bool OpenALSoundContext::OpenALSound::GetBoolValue(int nId) const {
  return GetIntValue(nId) == AL_TRUE ? true : false;
}

void OpenALSoundContext::OpenALSound::SetBoolValue(int nId, bool bValue) {
  SetIntValue(nId, bValue ? AL_TRUE : AL_FALSE);
}

int OpenALSoundContext::OpenALSound::GetNumBuffersProcessed() const {
  return GetIntValue(AL_BUFFERS_PROCESSED);
}

int OpenALSoundContext::OpenALSound::GetIntValue(int nId) const {
  m_pEnv->MakeCurrent();
  ALint n = 0;
  alGetSourcei(m_nSndId, nId, &n);
  return n;
}

void OpenALSoundContext::OpenALSound::SetIntValue(int nId, int nValue) {
  m_pEnv->MakeCurrent();
  alSourcei(m_nSndId, nId, nValue);
}

float OpenALSoundContext::OpenALSound::GetFloatValue(int nId) const {
  m_pEnv->MakeCurrent();
  float f = 0.0f;
  alGetSourcef(m_nSndId, nId, &f);
  return f;
}

bool OpenALSoundContext::OpenALSound::SetPlaystate(ePlayState eState) {
  switch (eState) {
  case PS_PLAY: {
    alSourcePlay(m_nSndId);
    m_ePlayState = eState;
    break;
  }
  case PS_PAUSE: {
    alSourcePause(m_nSndId);
    m_ePlayState = eState;
    break;
  }
  case PS_REWIND: {
    alSourceRewind(m_nSndId);
    m_ePlayState = eState;
    break;
  }
  case PS_MUTE: {
    if (m_ePlayState == PS_MUTE) {
      // unmute
      SetGain(m_nMuteGain);
      m_nMuteGain = -1.0f;
    } else {
      // we are not muting, get gain
      m_nMuteGain = GetGain();
      SetGain(0); // mute
    }
    m_ePlayState = eState;
    break;
  }
  default:
    break;
  }

  return true;
}

OpenALSoundContext::OpenALSound::ePlayState OpenALSoundContext::OpenALSound::GetPlaystate() const {
  if (m_ePlayState == PS_MUTE)
    return PS_MUTE;

  eState nState = GetSoundState();
  switch (nState) {
  case STATE_PLAYING:
    return PS_PLAY;
  case STATE_PAUSED:
    return PS_PAUSE;
  case STATE_STOPPED:
    return PS_IDLE;
  default:
    return PS_NONE;
  }
}

void OpenALSoundContext::OpenALSound::SetFloatValue(int nId, float fValue) {
  m_pEnv->MakeCurrent();
  alSourcef(m_nSndId, nId, fValue);
}

void OpenALSoundContext::OpenALSound::SetFloat3Array(int nId, float val[3]) {
  SetFloat3Array(nId, val[0], val[1], val[2]);
}

void OpenALSoundContext::OpenALSound::SetFloat3Array(int nId, float f1, float f2, float f3) {
  m_pEnv->MakeCurrent();
  alSource3f(m_nSndId, nId, f1, f2, f3);
}

void OpenALSoundContext::OpenALSound::SetFloat6Array(int nId, float val[6]) {
  m_pEnv->MakeCurrent();
  alSourcefv(m_nSndId, nId, val);
}

void OpenALSoundContext::OpenALSound::GetFloat3Array(int nId, float val[3]) const {
  m_pEnv->MakeCurrent();
  alGetSource3f(m_nSndId, nId, &val[0], &val[1], &val[2]);
}

void OpenALSoundContext::OpenALSound::GetFloat6Array(int nId, float val[6]) const {
  m_pEnv->MakeCurrent();
  alGetSourcefv(m_nSndId, nId, val);
}

void OpenALSoundContext::OpenALSound::GetFloat3Array(
    int nId, float& f1, float& f2, float& f3) const {
  m_pEnv->MakeCurrent();
  alGetSource3f(m_nSndId, nId, &f1, &f2, &f3);
}

float OpenALSoundContext::OpenALSound::GetPitch() const {
  return GetFloatValue(AL_PITCH);
}

void OpenALSoundContext::OpenALSound::SetPitch(float fP) {
  SetFloatValue(AL_PITCH, fP);
}

float OpenALSoundContext::OpenALSound::GetGain() const {
  return GetFloatValue(AL_GAIN);
}

void OpenALSoundContext::OpenALSound::SetGain(float fGain) {
  SetFloatValue(AL_GAIN, fGain);
}

bool OpenALSoundContext::OpenALSound::GetIsLooping() const {
  return GetBoolValue(AL_LOOPING);
}

void OpenALSoundContext::OpenALSound::SetIsLooping(bool bLoop) {
  SetBoolValue(AL_LOOPING, bLoop);
}

OpenALSoundContext::OpenALVirtualSound::eState
OpenALSoundContext::OpenALSound::GetSoundState() const {
  int nState = GetIntValue(AL_SOURCE_STATE);
  switch (nState) {
  case AL_STOPPED: {
    return STATE_STOPPED;
  }
  case AL_PAUSED: {
    return STATE_PAUSED;
  }
  case AL_PLAYING: {
    return STATE_PLAYING;
  }
  default:
    return STATE_NONE;
  }
}

OpenALSoundContext::OpenALSoundEnvironment*
OpenALSoundContext::OpenALSound::GetEnvironment() const {
  return m_pEnv;
}

void OpenALSoundContext::OpenALSound::SetEnvironment(OpenALSoundEnvironment* pEnv) {
  m_pEnv = pEnv;
}

bool OpenALSoundContext::OpenALSound::AttachBuffer(const OpenALBuffer& buffer) {
  if (buffer.GetIsStreamingBuffer()) {
    ALuint b[1];
    b[0] = buffer.m_nBufId;

    alSourceQueueBuffers(m_nSndId, 1, b);
  } else {
    if (GetIntValue(AL_BUFFER) == 0) {
      SetIntValue(AL_BUFFER, buffer.m_nBufId);
      return true;
    }
  }
  return false;
}

bool OpenALSoundContext::OpenALSound::DetachBuffer(const OpenALBuffer* pBuffer) {
  if (pBuffer && pBuffer->GetIsStreamingBuffer()) {
    ALuint b[1];
    b[0] = (*pBuffer).m_nBufId;
    alSourceUnqueueBuffers(m_nSndId, 1, b);
  } else
    SetIntValue(AL_BUFFER, 0);
  return true;
}

ALuint OpenALSoundContext::OpenALSound::GetBufferId() const {
  return GetIntValue(AL_BUFFER);
}

// ##########################################################################

OpenALSoundContext::OpenALVirtualSound::OpenALVirtualSound(OpenALSoundEnvironment* pEnv)
    : OpenALSoundContext::OpenALSound(pEnv) {
}

OpenALSoundContext::OpenALVirtualSound::~OpenALVirtualSound() {
}

OpenALSoundContext::OpenALVirtualSound::OpenALVirtualSound(const OpenALVirtualSound& other)
    : OpenALSound(other) {
}

OpenALSoundContext::OpenALVirtualSound& OpenALSoundContext::OpenALVirtualSound::operator=(
    const OpenALSoundContext::OpenALVirtualSound& other) {
  OpenALSound::operator=(other);
  return *this;
}

float OpenALSoundContext::OpenALVirtualSound::GetMaxDistance() const {
  return GetFloatValue(AL_MAX_DISTANCE);
}

void OpenALSoundContext::OpenALVirtualSound::SetMaxDistance(float fMd) {
  SetFloatValue(AL_MAX_DISTANCE, fMd);
}

float OpenALSoundContext::OpenALVirtualSound::GetRolloffFactor() const {
  return GetFloatValue(AL_ROLLOFF_FACTOR);
}

void OpenALSoundContext::OpenALVirtualSound::SetRolloffFactor(float fRf) {
  SetFloatValue(AL_ROLLOFF_FACTOR, fRf);
}

float OpenALSoundContext::OpenALVirtualSound::GetReferenceDistance() const {
  return GetFloatValue(AL_REFERENCE_DISTANCE);
}

void OpenALSoundContext::OpenALVirtualSound::SetReferenceDistance(float fRd) {
  SetFloatValue(AL_REFERENCE_DISTANCE, fRd);
}

float OpenALSoundContext::OpenALVirtualSound::GetMinGain() const {
  return GetFloatValue(AL_MIN_GAIN);
}

void OpenALSoundContext::OpenALVirtualSound::SetMinGain(float fMg) {
  SetFloatValue(AL_MIN_GAIN, fMg);
}

float OpenALSoundContext::OpenALVirtualSound::GetMaxGain() const {
  return GetFloatValue(AL_MAX_GAIN);
}

void OpenALSoundContext::OpenALVirtualSound::SetMaxGain(float fMg) {
  SetFloatValue(AL_MAX_GAIN, fMg);
}

float OpenALSoundContext::OpenALVirtualSound::GetConeOuterGain() const {
  return GetFloatValue(AL_CONE_OUTER_GAIN);
}

void OpenALSoundContext::OpenALVirtualSound::SetConeOuterGain(float fOg) {
  SetFloatValue(AL_CONE_OUTER_GAIN, fOg);
}

void OpenALSoundContext::OpenALVirtualSound::GetPosition(float& x, float& y, float& z) const {
  GetFloat3Array(AL_POSITION, x, y, z);
}

void OpenALSoundContext::OpenALVirtualSound::GetPosition(float pos[3]) {
  GetFloat3Array(AL_POSITION, pos);
}

void OpenALSoundContext::OpenALVirtualSound::SetPosition(float x, float y, float z) {
  SetFloat3Array(AL_POSITION, x, y, z);
}

void OpenALSoundContext::OpenALVirtualSound::SetPosition(float pos[3]) {
  SetFloat3Array(AL_POSITION, pos);
}

void OpenALSoundContext::OpenALVirtualSound::GetVelocity(float& x, float& y, float& z) const {
  GetFloat3Array(AL_VELOCITY, x, y, z);
}

void OpenALSoundContext::OpenALVirtualSound::GetVelocity(float pos[3]) {
  GetFloat3Array(AL_VELOCITY, pos);
}

void OpenALSoundContext::OpenALVirtualSound::SetVelocity(float x, float y, float z) {
  SetFloat3Array(AL_VELOCITY, x, y, z);
}

void OpenALSoundContext::OpenALVirtualSound::SetVelocity(float pos[3]) {
  SetFloat3Array(AL_VELOCITY, pos);
}

void OpenALSoundContext::OpenALVirtualSound::GetDirection(float& x, float& y, float& z) const {
  GetFloat3Array(AL_DIRECTION, x, y, z);
}

void OpenALSoundContext::OpenALVirtualSound::GetDirection(float pos[3]) {
  GetFloat3Array(AL_DIRECTION, pos);
}

void OpenALSoundContext::OpenALVirtualSound::SetDirection(float x, float y, float z) {
  SetFloat3Array(AL_DIRECTION, x, y, z);
}

void OpenALSoundContext::OpenALVirtualSound::SetDirection(float pos[3]) {
  SetFloat3Array(AL_DIRECTION, pos);
}

bool OpenALSoundContext::OpenALVirtualSound::GetSourceIsRelative() const {
  return GetBoolValue(AL_SOURCE_RELATIVE);
}

void OpenALSoundContext::OpenALVirtualSound::SetSourceIsRelative(bool bVal) {
  SetBoolValue(AL_SOURCE_RELATIVE, bVal);
}

int OpenALSoundContext::OpenALVirtualSound::GetConeInnerAngle() const {
  return GetIntValue(AL_CONE_INNER_ANGLE);
}

void OpenALSoundContext::OpenALVirtualSound::SetConeInnerAngle(int nValue) {
  SetIntValue(AL_CONE_INNER_ANGLE, nValue);
}

int OpenALSoundContext::OpenALVirtualSound::GetConeOuterAngle() const {
  return GetIntValue(AL_CONE_OUTER_ANGLE);
}

void OpenALSoundContext::OpenALVirtualSound::SetConeOuterAngle(int nValue) {
  SetIntValue(AL_CONE_OUTER_ANGLE, nValue);
}

// ###########################################################################

OpenALSoundContext::OpenALBuffer::OpenALBuffer(const OpenALBuffer& mb) {
  m_nBufId           = mb.m_nBufId;
  m_bStreamingBuffer = mb.m_bStreamingBuffer;
  m_nFreq            = mb.m_nFreq;
  m_nFormat          = mb.m_nFormat;
}

OpenALSoundContext::OpenALBuffer& OpenALSoundContext::OpenALBuffer::operator=(
    const OpenALBuffer& other) {
  if (this == &other)
    return *this;

  m_nBufId           = other.m_nBufId;
  m_bStreamingBuffer = other.m_bStreamingBuffer;
  m_nFreq            = other.m_nFreq;
  m_nFormat          = other.m_nFormat;

  return *this;
}

OpenALSoundContext::OpenALBuffer::OpenALBuffer()
    : m_bStreamingBuffer(false)
    , m_nFormat(~0)
    , m_nFreq(~0) {
  alGenBuffers(1, &m_nBufId);
}

OpenALSoundContext::OpenALBuffer::~OpenALBuffer() {
  alDeleteBuffers(1, &m_nBufId);
  ALenum error;
  if ((error = alGetError()) != AL_NO_ERROR) {
    std::cerr << "error during buffer delete: ";
    if (error == AL_INVALID_OPERATION) {
      std::cerr << "buffer might still be allocated.\n";
    }
  }
}

bool OpenALSoundContext::OpenALBuffer::GetIsStreamingBuffer() const {
  return m_bStreamingBuffer;
}

void OpenALSoundContext::OpenALBuffer::SetIsStreamingBuffer(bool bIsStreamingBuffer) {
  m_bStreamingBuffer = bIsStreamingBuffer;
}

int OpenALSoundContext::OpenALBuffer::GetSampleRate() const {
  int n = 0;
  alGetBufferi(m_nBufId, AL_FREQUENCY, &n);
  return n;
}

int OpenALSoundContext::OpenALBuffer::GetBitDepth() const {
  int n = 0;
  alGetBufferi(m_nBufId, AL_BITS, &n);
  return n;
}

int OpenALSoundContext::OpenALBuffer::GetChannels() const {
  int n = 0;
  alGetBufferi(m_nBufId, AL_CHANNELS, &n);
  return n;
}

int OpenALSoundContext::OpenALBuffer::GetSize() const {
  int n = 0;
  alGetBufferi(m_nBufId, AL_SIZE, &n);
  return n;
}

ALuint OpenALSoundContext::OpenALBuffer::GetBufferId() const {
  return m_nBufId;
}

OpenALSoundContext::OpenALBuffer* OpenALSoundContext::OpenALBuffer::ReadFromWav(
    const std::string& sFileName) {
  ALsizei   alSize, alFreq;
  ALenum    alFormat;
  ALvoid*   pRawSound = NULL;
  ALboolean alLoop;
  alutLoadWAVFile((ALbyte*)sFileName.c_str(), &alFormat, &pRawSound, &alSize, &alFreq, &alLoop);

  if (pRawSound) {
    OpenALBuffer* b = new OpenALBuffer;
    b->SetBufferName(sFileName);

    alBufferData(b->m_nBufId, alFormat, pRawSound, alSize, alFreq);

    alutUnloadWAV(alFormat, pRawSound, alSize, alFreq);

    b->SetIsStreamingBuffer(false);
    b->m_nFormat = alFormat;
    b->m_nFreq   = alFreq;

    return b;
  } else
    return NULL; // load error
}

std::string OpenALSoundContext::OpenALBuffer::GetBufferName() const {
  return m_sName;
}

void OpenALSoundContext::OpenALBuffer::SetBufferName(const std::string& sName) {
  m_sName = sName;
}

bool OpenALSoundContext::OpenALBuffer::FillBuffer(
    ALuint nSize, ALuint freq, const ALvoid* pvBufferData, ALenum eFormat) {
  if (m_nFreq == ~0)
    m_nFreq = freq;
  else if (m_nFreq != freq)
    return false;

  if (m_nFormat == ~0)
    m_nFormat = eFormat;
  else if (m_nFormat != eFormat)
    return false;

  alBufferData(m_nBufId, eFormat, pvBufferData, nSize, freq);
  return true;
}

OpenALSoundContext::OpenALBuffer* OpenALSoundContext::OpenALBuffer::CreateFromBuffer(
    const std::string& sBuffer, ALuint nSize, ALuint nFreq, ALenum eFormat,
    const ALvoid* pvBufferData) {
  OpenALBuffer* b = new OpenALBuffer;
  b->SetBufferName(sBuffer);

  b->FillBuffer(nSize, nFreq, pvBufferData, eFormat);

  return b;
}

// ############################################################################
// ### CAPTURE
// ############################################################################

OpenALSoundContext::OpenALCaptureContext::OpenALCaptureContext(size_t freq, unsigned int format,
    size_t captureBufferSizeInSamples, const std::string& captureDevice)
    : m_bDoesCapture(false) {
  m_pCapture = alcCaptureOpenDevice((captureDevice.empty() ? NULL : captureDevice.c_str()),
      int(freq), int(format), int(captureBufferSizeInSamples));
}

OpenALSoundContext::OpenALCaptureContext::~OpenALCaptureContext() {
  if (m_pCapture)
    alcCaptureCloseDevice(m_pCapture);
}

size_t OpenALSoundContext::OpenALCaptureContext::captureTo(void* buffer, size_t maxSize) {
  if (m_pCapture) {
    ALint iSamplesAvailable = (ALint)available();
    ALint numSamples        = std::min<ALint>((ALint)maxSize, iSamplesAvailable);
    //		std::cout << "available: " << iSamplesAvailable << std::endl
    //				  << "taking: " << numSamples << std::endl;

    if (numSamples) {
      alcCaptureSamples(m_pCapture, buffer, numSamples);
    }

    return numSamples;
  }

  return 0;
}

size_t OpenALSoundContext::OpenALCaptureContext::available() const {
  ALint iSamplesAvailable = 0;
  alcGetIntegerv(m_pCapture, ALC_CAPTURE_SAMPLES, 1, &iSamplesAvailable);
  return iSamplesAvailable;
}

bool OpenALSoundContext::OpenALCaptureContext::start() {
  alcCaptureStart(m_pCapture);
  m_bDoesCapture = true;
  return true;
}

bool OpenALSoundContext::OpenALCaptureContext::stop() {
  alcCaptureStop(m_pCapture);
  m_bDoesCapture = false;
  return true;
}

bool OpenALSoundContext::OpenALCaptureContext::doesCapture() const {
  return m_bDoesCapture;
}

std::list<std::string> OpenALSoundContext::OpenALCaptureContext::getCaptureDeviceNames() {
  const ALCchar* devlist = alcGetString(NULL, ALC_CAPTURE_DEVICE_SPECIFIER);
  return getTokenizedList(devlist);
}

std::string OpenALSoundContext::OpenALCaptureContext::getDefaultCaptureDevice() {
  const ALCchar* devlist = alcGetString(NULL, ALC_CAPTURE_DEFAULT_DEVICE_SPECIFIER);
  if (!devlist)
    return std::string();
  else
    return std::string(devlist);
}

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

/*============================================================================*/
/* END OF FILE                                                                */
/*============================================================================*/

/************************** CR / LF nicht vergessen! **************************/
