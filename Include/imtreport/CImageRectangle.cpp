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
	static iser::CArchiveTag imagePathTag("ImagePath", "The image path");

	istd::CChangeNotifier notifier(archive.IsStoring()? NULL: this);

	bool retVal = true;

	retVal = retVal && BaseClass::Serialize(archive);

	retVal = retVal && archive.BeginTag(imagePathTag);
	retVal = retVal && archive.Process(m_imagePath);
	retVal = retVal && archive.EndTag(imagePathTag);


	return retVal;
}


// reimplemented (istd::IChangeable)

int CImageRectangle::GetSupportedOperations() const
{
	return SO_COPY | SO_CLONE;
}


bool CImageRectangle::CopyFrom(const IChangeable& object, CompatibilityMode mode)
{
	const CImageRectangle* labelPtr = dynamic_cast<const CImageRectangle*>(&object);

	if (labelPtr != NULL)
	{
		istd::CChangeNotifier notifier(this);

		m_imagePath = labelPtr->GetImagePath();

		BaseClass::CopyFrom(object, mode);

		return true;
	}

	return false;
}


istd::IChangeable* CImageRectangle::CloneMe(CompatibilityMode mode) const 
{
	istd::TDelPtr<CImageRectangle> clonePtr(new CImageRectangle);

	if (clonePtr->CopyFrom(*this, mode))
	{
		return clonePtr.PopPtr();
	}

	return NULL;
}


} // namespace imtreport


