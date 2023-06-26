/*============================================================================*/
/*       1         2         3         4         5         6         7        */
/*3456789012345678901234567890123456789012345678901234567890123456789012345678*/
/*============================================================================*/
/*                                             .                              */
/*                                               RRRR WW  WW   WTTTTTTHH  HH  */
/*                                               RR RR WW WWW  W  TT  HH  HH  */
/*      Header   :  OPENALAUDIOMANAGER.h      RRRR   WWWWWWWW  TT  HHHHHH  */
/*                                               RR RR   WWW WWW  TT  HH  HH  */
/*      Module   :  OPENALAUDIOMANAGER        RR  R    WW  WW  TT  HH  HH  */
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
/*                                                                            */
/*    THIS SOFTWARE IS PROVIDED 'AS IS'. ANY WARRANTIES ARE DISCLAIMED. IN    */
/*    NO CASE SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE FOR ANY DAMAGES.    */
/*    REDISTRIBUTION AND USE OF THE NON MODIFIED TOOLKIT IS PERMITTED. OWN    */
/*    DEVELOPMENTS BASED ON THIS TOOLKIT MUST BE CLEARLY DECLARED AS SUCH.    */
/*                                                                            */
/*============================================================================*/
/*                                                                            */
/*      CLASS DEFINITIONS:                                                    */
/*                                                                            */
/*        - ...               :   ...                                         */
/*                                                                            */
/*============================================================================*/

#ifndef __OPENALSOUNDCONTEXT_H
#define __OPENALSOUNDCONTEXT_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/

#include <list>
#include <map>
#include <string>
#include <vector>

#include <al.h>
#include <alc.h>
#include <alut.h>

#ifdef WIN32
#pragma warning(disable : 4996)
#endif

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

class OpenALSoundContext {
 public:
  class OpenALVirtualListener;
  class OpenALVirtualSound;
  class OpenALSoundEnvironment;
  class OpenALSound;

  OpenALSoundContext();
  virtual ~OpenALSoundContext();

  // #######################
  // BUFFERS
  // #######################

  class OpenALBuffer {
    friend class OpenALSoundContext;
    friend class OpenALSoundContext::OpenALSound;

   public:
    int GetSampleRate() const;
    int GetBitDepth() const;
    int GetChannels() const;
    int GetSize() const;

    bool GetIsStreamingBuffer() const;
    void SetIsStreamingBuffer(bool bIsStreamingBuffer);

    ALuint GetBufferId() const;

    std::string GetBufferName() const;
    void        SetBufferName(const std::string& sName);
    OpenALBuffer(const OpenALBuffer& mb);
    OpenALBuffer& operator=(const OpenALBuffer&);

    bool FillBuffer(ALuint nSize, ALuint freq, const ALvoid* pvBufferData, ALenum eFormat);

    static OpenALBuffer* ReadFromWav(const std::string& sFileName);
    static OpenALBuffer* CreateFromBuffer(const std::string& sName, ALuint nSize, ALuint nFreq,
        ALenum eFormat, const ALvoid* pvBufferData);

   protected:
   private:
    OpenALBuffer();
    ~OpenALBuffer();

    ALuint      m_nBufId;
    ALuint      m_nFreq;
    ALuint      m_nFormat;
    std::string m_sName;
    bool        m_bStreamingBuffer;
  };

  bool          AddBuffer(OpenALBuffer* buf);
  bool          RemBuffer(OpenALBuffer* buf);
  OpenALBuffer* GetBuffer(const std::string& sName) const;
  OpenALBuffer* GetBuffer(ALuint nBufferId) const;

  // #######################
  // SOUNDS
  // #######################
  class OpenALSound {
    friend class OpenALSoundContext;
    friend class OpenALSoundContext::OpenALSoundEnvironment;

   public:
    enum ePlayState { PS_NONE = -1, PS_IDLE, PS_PLAY, PS_PAUSE, PS_REWIND, PS_MUTE };

    bool       SetPlaystate(ePlayState eState);
    ePlayState GetPlaystate() const;

    float GetPitch() const;
    void  SetPitch(float);

    float GetGain() const;
    void  SetGain(float);

    bool GetIsLooping() const;
    void SetIsLooping(bool);

    enum eState { STATE_NONE = -1, STATE_PLAYING = 0, STATE_STOPPED, STATE_PAUSED };

    eState GetSoundState() const;

    OpenALSoundEnvironment* GetEnvironment() const;

    bool   AttachBuffer(const OpenALBuffer&);
    bool   DetachBuffer(const OpenALBuffer* pBuffer = NULL);
    ALuint GetBufferId() const;

    int GetNumBuffersProcessed() const;

    OpenALSound(const OpenALSound&);
    OpenALSound& operator=(const OpenALSound&);

   protected:
    void SetEnvironment(OpenALSoundEnvironment*);

    float GetFloatValue(int nId) const;
    void  SetFloatValue(int nId, float fValue);
    void  SetFloat3Array(int nId, float val[3]);
    void  SetFloat3Array(int nId, float f1, float f2, float f3);

    void SetFloat6Array(int nId, float val[6]);
    void GetFloat3Array(int nId, float val[3]) const;
    void GetFloat3Array(int nId, float& f1, float& f2, float& f3) const;
    void GetFloat6Array(int nId, float val[6]) const;

    bool GetBoolValue(int nId) const;
    void SetBoolValue(int nId, bool bValue);

    int  GetIntValue(int nId) const;
    void SetIntValue(int nId, int nValue);

    OpenALSound();
    OpenALSound(OpenALSoundEnvironment*);
    virtual ~OpenALSound();

   private:
    OpenALSoundEnvironment* m_pEnv;
    ALuint                  m_nSndId;

    float      m_nMuteGain;
    ePlayState m_ePlayState;
  };

  class OpenALVirtualSound : public OpenALSound {
    friend class OpenALSoundContext;
    friend class OpenALSoundContext::OpenALSoundEnvironment;

   public:
    float GetMaxDistance() const;
    void  SetMaxDistance(float);

    float GetRolloffFactor() const;
    void  SetRolloffFactor(float);

    float GetReferenceDistance() const;
    void  SetReferenceDistance(float);

    float GetMinGain() const;
    void  SetMinGain(float);

    float GetMaxGain() const;
    void  SetMaxGain(float);

    float GetConeOuterGain() const;
    void  SetConeOuterGain(float);

    void GetPosition(float& x, float& y, float& z) const;
    void GetPosition(float pos[3]);
    void SetPosition(float x, float y, float z);
    void SetPosition(float pos[3]);

    void GetVelocity(float& x, float& y, float& z) const;
    void GetVelocity(float pos[3]);
    void SetVelocity(float x, float y, float z);
    void SetVelocity(float pos[3]);

    void GetDirection(float& x, float& y, float& z) const;
    void GetDirection(float pos[3]);
    void SetDirection(float x, float y, float z);
    void SetDirection(float pos[3]);

    bool GetSourceIsRelative() const;
    void SetSourceIsRelative(bool);

    int  GetConeInnerAngle() const;
    void SetConeInnerAngle(int);

    int  GetConeOuterAngle() const;
    void SetConeOuterAngle(int);

    OpenALVirtualSound(const OpenALVirtualSound&);
    OpenALVirtualSound& operator=(const OpenALVirtualSound&);

   protected:
   private:
    OpenALVirtualSound(OpenALSoundEnvironment*);
    virtual ~OpenALVirtualSound();
  };

  // #######################
  // ENVIRONMENT
  // #######################
  class OpenALSoundEnvironment {
    friend class OpenALSoundContext;

   public:
    virtual ~OpenALSoundEnvironment();

    OpenALVirtualListener* GetListener() const;

    bool MakeCurrent();

    OpenALVirtualSound* CreateAndAddSound();
    bool                DeleteSound(OpenALVirtualSound*);
    bool                RemoveSound(OpenALVirtualSound*);
    bool                AddSound(OpenALVirtualSound*);

    bool HasBuffer(OpenALBuffer* pBuf) const;

   protected:
   private:
    OpenALVirtualListener* m_pListener;
    OpenALSoundEnvironment(ALCdevice* pDev);
    ALCcontext*                      m_pALCContext;
    std::vector<OpenALVirtualSound*> m_mpSounds;
  };

  // #######################
  // LISTENER
  // #######################
  class OpenALVirtualListener {
    friend class OpenALSoundContext;
    friend class OpenALSoundContext::OpenALSoundEnvironment;

   public:
    bool UpdateListenerWCS(float pos[3], float view[3], float up[3]);

    bool UpdateListenerWCSPos(float pos[3]);
    bool UpdateListenerWCSView(float view[3]);
    bool UpdateListenerWCSUp(float up[3]);

    bool UpdateListenerVel(float vel[3]);

    bool UpdateListenerWCS(float x, float y, float z, float viewx, float viewy, float viewz,
        float upx, float upy, float upz);

    bool UpdateListenerWCSPos(float x, float y, float z);
    bool UpdateListenerWCSView(float viewx, float viewy, float viewz);
    bool UpdateListenerWCSUp(float upx, float upy, float upz);

    bool UpdateListenerVel(float velx, float vely, float velz);

    void  SetListenerGain(float fGain);
    float GetListenerGain() const;

    OpenALSoundEnvironment* GetEnvironment() const;

   protected:
    void SetEnvironment(OpenALSoundEnvironment*);

   private:
    OpenALVirtualListener();
    virtual ~OpenALVirtualListener();
    OpenALSoundEnvironment* m_pEnv;
  };

  class OpenALCaptureContext {
   public:
    OpenALCaptureContext(size_t freq, unsigned int format, size_t captureBufferSizeInSamples,
        const std::string& captureDevice = "");

    ~OpenALCaptureContext();

    bool start();
    bool stop();
    bool doesCapture() const;

    size_t captureTo(void* buffer, size_t maxSize);
    size_t available() const;

    static std::list<std::string> getCaptureDeviceNames();
    static std::string            getDefaultCaptureDevice();

   private:
    ALCdevice* m_pCapture;
    bool       m_bDoesCapture;
  };

  // USER API
  bool                                        PrintState() const;
  OpenALSoundContext::OpenALSoundEnvironment* CreateAndAddSoundEnv(const std::string& sEnvName);
  OpenALSoundContext::OpenALSoundEnvironment* GetSoundEnvironment(
      const std::string& sEnvName) const;

  std::string GetVendor() const;
  std::string GetVersion() const;
  std::string GetRenderer() const;
  std::string GetExtensions() const;

 protected:
 private:
  // OpenAL members
  ALCdevice* m_pALCDevice;

  std::string m_pcVendor, m_pcVersion, m_pcRenderer, m_pcExtensions;

  bool m_bGetEnv;

  std::map<std::string, OpenALSoundEnvironment*> m_mpEnvs;
  std::map<std::string, OpenALBuffer*>           m_mpBuffers;
};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

/*============================================================================*/
/* END OF FILE                                                                */
/*============================================================================*/

#endif // __OPENALSOUNDCONTEXT_H
