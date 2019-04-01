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
OSG::FieldContainerType & VistaOpenSGTextForegroundBase::getClassType(void)
{
    return _type; 
} 

//! access the numerical type of the class
inline
OSG::UInt32 VistaOpenSGTextForegroundBase::getClassTypeId(void) 
{
    return _type.getId(); 
} 

//! create a new instance of the class
inline
VistaOpenSGTextForegroundPtr VistaOpenSGTextForegroundBase::create(void) 
{
    VistaOpenSGTextForegroundPtr fc; 

    if(getClassType().getPrototype() != OSG::NullFC) 
    {
        fc = VistaOpenSGTextForegroundPtr::dcast(
            getClassType().getPrototype()-> shallowCopy()); 
    }
    
    return fc; 
}

//! create an empty new instance of the class, do not copy the prototype
inline
VistaOpenSGTextForegroundPtr VistaOpenSGTextForegroundBase::createEmpty(void) 
{ 
    VistaOpenSGTextForegroundPtr returnValue; 
    
    newPtr(returnValue); 

    return returnValue; 
}


/*------------------------------ get -----------------------------------*/

//! Get the TextForeground::_mfFormats field.
inline
MField<void*> *VistaOpenSGTextForegroundBase::getMFTexts(void)
{
    return &m_mfTexts;
}

//! Get the value of the \a index element the TextForeground::_mfFormats field.
inline
void* VistaOpenSGTextForegroundBase::getTexts(const UInt32 index)
{
    return m_mfTexts[index];
}

//! Get the VistaOpenSGTextForeground::_mfFormats field.
inline
MField<void*> &VistaOpenSGTextForegroundBase::getTexts(void)
{
    return m_mfTexts;
}

//! Get the TextForeground::_mfFormats field.
inline
const MField<void*> &VistaOpenSGTextForegroundBase::getTexts(void) const
{
    return m_mfTexts;
}

OSG_END_NAMESPACE
