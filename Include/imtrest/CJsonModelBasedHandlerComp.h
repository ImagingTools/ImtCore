#pragma once


// ACF includes
//#include <ifile/IFileNameParam.h>
#include <ilog/TLoggerCompWrap.h>

// ImtCore includes
#include <imtrest/IRequestHandler.h>

//// Qt includes
//#include <QtCore/QDir>
//#include <QtCore/QFile>
//#include <QtCore/QTextStream>

//// ImtCore includes
//#include <imtrest/IRequest.h>
//#include <imtrest/IResponder.h>
//#include <imtrest/IResponse.h>
#include <imtrest/IProtocolEngine.h>




//------------------------------------------------------------------mem test
// Qt includes
//#include <QtWidgets/QAction>
//#include <QtCore/QJsonDocument>


//// ACF includes
////#include <iqtgui/TDesignerGuiObserverCompBase.h>
//#include <iqtgui/IGuiObject.h>
//#include <imod/TSingleModelObserverBase.h>
//#include <ilog/TLoggerCompWrap.h>

// ImtCore includes
//#include <imtauth/IContactInfo.h>
//#include <GeneratedFiles/imtauthgui/ui_CContactInfoEditorComp.h>
//#include <imtauth/IContactInfo.h>
//#include <imtbase/IItemBasedRepresentationProvider.h>
//#include <imtbase/CTreeItemModel.h>

// ACF includes
//#include <istd/CChangeGroup.h>
#include <iser/CJsonStringWriteArchive.h>

// ImtCore includes
//#include <imtbase/ICollectionInfo.h>
//#include <imtauth/IAddressManager.h>
//#include <imtauth/CAddress.h>
#include <imtrest/IRepresentationDataProvider.h>
//------------------------------------------------------------------

namespace imtrest
{


/**
 * \brief The CJsonModelBasedHandlerComp class generates JSON-model
 */
class CJsonModelBasedHandlerComp:
			public ilog::CLoggerComponentBase,
			virtual public IRequestHandler
{
public:
	typedef ilog::CLoggerComponentBase BaseClass;

	CJsonModelBasedHandlerComp();

	I_BEGIN_COMPONENT(CJsonModelBasedHandlerComp);
		I_REGISTER_INTERFACE(IRequestHandler);
		I_ASSIGN(m_representationDataProvider, "RepresentationDataProvider", "Representation data provider", false, "RepresentationDataProvider");
		I_ASSIGN(m_commandIdAttrPtr, "CommandId", "List of command-ID used with corresponded handlers", true, "");
	I_END_COMPONENT


	// reimplemented (IRequestHandler)
	virtual ConstResponsePtr ProcessRequest(const IRequest& request) const override;
	virtual QByteArray GetSupportedCommandId() const override;

private:
	I_REF(imtrest::IRepresentationDataProvider, m_representationDataProvider);
	I_ATTR(QByteArray, m_commandIdAttrPtr);

};


} // namespace imtrest


