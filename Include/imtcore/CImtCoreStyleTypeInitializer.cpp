#include <imtcore/CImtCoreStyleTypeInitializer.h>

// Qt includes
#include <QtCore/qglobal.h>

// ImtCore includes
#include <imtstyle/CImtStyle.h>


namespace imtcore
{


void CImtCoreStyleTypeInitializer::Initialize()
{
	auto* imtStylePtr = imtstyle::CImtStyle::GetInstance();
	Q_ASSERT(imtStylePtr != nullptr);
	imtStylePtr->SetStyleType(imtstyle::CImtStyle::ST_IMAGINGTOOLS);
}


} // namespace imtcore
