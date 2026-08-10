#include <imtcore/CImtCoreFusionBaseStyleInitializer.h>

// Qt includes
#include <QtWidgets/QApplication>
#include <QtWidgets/QStyleFactory>

// ImtCore includes
#include <imtstyle/CImtStyle.h>


namespace imtcore
{


void CImtCoreFusionBaseStyleInitializer::Initialize()
{
	auto* imtStylePtr = imtstyle::CImtStyle::GetInstance();
	Q_ASSERT(imtStylePtr != nullptr);

	imtStylePtr->setBaseStyle(QStyleFactory::create("fusion"));
	QApplication::setStyle(imtStylePtr);
}


} // namespace imtcore
