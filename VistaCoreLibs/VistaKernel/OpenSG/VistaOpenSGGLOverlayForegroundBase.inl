#ifdef WIN32
// disable warnings from OpenSG
#pragma warning(push)
#pragma warning(disable: 4231)
#pragma warning(disable: 4267)
#endif

#include <OpenSG/OSGConfig.h>

#ifdef WIN32
#pragma warning(pop)
#endif

OSG_BEGIN_NAMESPACE

//! access the type of the class
inline
OSG::FieldContainerType & VistaOpenSGGLOverlayForegroundBase::getClassType(void)
{
    return _type; 
} 

//! access the numerical type of the class
inline
OSG::UInt32 VistaOpenSGGLOverlayForegroundBase::getClassTypeId(void) 
{
    return _type.getId(); 
} 

//! create a new instance of the class
inline
VistaOpenSGGLOverlayForegroundPtr VistaOpenSGGLOverlayForegroundBase::create(void) 
{
    VistaOpenSGGLOverlayForegroundPtr fc; 

    if(getClassType().getPrototype() != OSG::NullFC) 
    {
        fc = VistaOpenSGGLOverlayForegroundPtr::dcast(
            getClassType().getPrototype()-> shallowCopy()); 
    }
    
    return fc; 
}

//! create an empty new instance of the class, do not copy the prototype
inline
VistaOpenSGGLOverlayForegroundPtr VistaOpenSGGLOverlayForegroundBase::createEmpty(void) 
{ 
    VistaOpenSGGLOverlayForegroundPtr returnValue; 
    
    newPtr(returnValue); 

    return returnValue; 
}


/*------------------------------ get -----------------------------------*/

//! Get the TextForeground::_mfFormats field.
inline
MField<void*> *VistaOpenSGGLOverlayForegroundBase::getMFGLOverlays(void)
{
    return &m_mfGLOverlays;
}

//! Get the value of the \a index element the TextForeground::_mfFormats field.
inline
void* VistaOpenSGGLOverlayForegroundBase::getGLOverlays(const UInt32 index)
{
    return m_mfGLOverlays[index];
}

//! Get the VistaOpenSGTextForeground::_mfFormats field.
inline
MField<void*> &VistaOpenSGGLOverlayForegroundBase::getGLOverlays(void)
{
    return m_mfGLOverlays;
}

//! Get the TextForeground::_mfFormats field.
inline
const MField<void*> &VistaOpenSGGLOverlayForegroundBase::getGLOverlays(void) const
{
    return m_mfGLOverlays;
}

OSG_END_NAMESPACE
