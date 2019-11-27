#include <imtreport/CImageRectangle.h>


// ACF includes
#include "istd/TDelPtr.h"
#include "istd/CChangeNotifier.h"
#include "iser/CArchiveTag.h"


namespace imtreport
{


// public methods

const QString& CImageRectangle::GetImagePath() const
{
	return m_imagePath;
}


void CImageRectangle::SetImagePath(const QString& imagePath)
{
	if (m_imagePath != imagePath)
	{
		istd::CChangeNotifier changeNotifier(this);

		m_imagePath = imagePath;
	}
}


// reimplemented (iser::ISerializable)

bool CImageRectangle::Serialize(iser::IArchive& archive)
{
	istd::CChangeNotifier notifier(archive.IsStoring()? NULL: this);

	bool retVal = true;

	static iser::CArchiveTag imagePathTag("ImagePath", "The image path", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(imagePathTag);
	retVal = retVal && archive.Process(m_imagePath);
	retVal = retVal && archive.EndTag(imagePathTag);

	retVal = retVal && BaseClass::Serialize(archive);

	return retVal;
}


// reimplemented (istd::IChangeable)

int CImageRectangle::GetSupportedOperations() const
{
	return SO_COPY | SO_CLONE | SO_COMPARE | SO_RESET;
}


bool CImageRectangle::CopyFrom(const IChangeable& object, CompatibilityMode mode)
{
	const CImageRectangle* labelPtr = dynamic_cast<const CImageRectangle*>(&object);

	if (labelPtr != nullptr){
		istd::CChangeNotifier notifier(this);

		if (BaseClass::GetSupportedOperations() & SO_COPY){
			if (!BaseClass::CopyFrom(object, mode)){
				return false;
			}
		}

		m_imagePath = labelPtr->GetImagePath();
		
		return true;
	}

	return false;
}

bool CImageRectangle::IsEqual(const IChangeable & object) const
{
	const CImageRectangle* sourcePtr = dynamic_cast<const CImageRectangle*>(&object);

	if (sourcePtr != nullptr){
		if (BaseClass::GetSupportedOperations() & SO_COMPARE){
			if (!BaseClass::IsEqual(object)){
				return false;
			}
		}

		return (m_imagePath == sourcePtr->m_imagePath);
	}

	return false;
}


istd::IChangeable* CImageRectangle::CloneMe(CompatibilityMode mode) const 
{
	istd::TDelPtr<CImageRectangle> clonePtr(new CImageRectangle);

	if (clonePtr->CopyFrom(*this, mode)){
		return clonePtr.PopPtr();
	}

	return nullptr;
}


bool CImageRectangle::ResetData(CompatibilityMode mode)
{
	istd::CChangeNotifier changeNotifier(this);

	if (BaseClass::GetSupportedOperations() & SO_RESET){
		if (!BaseClass::ResetData(mode)){
			return false;
		}
	}
	
	m_imagePath = QString();

	return true;
}

} // namespace imtreport


