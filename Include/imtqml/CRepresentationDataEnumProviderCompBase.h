#pragma once


// Qt includes
#include <QtCore/QObject>
#include <QtQml/QJSEngine>
#include <QtQml/QQmlEngine>

// ACF includes
#include <icomp/CComponentBase.h>
#include <iser/CJsonStringWriteArchive.h>

// ImtCore includes
#include <imtbase/IItemBasedRepresentationDataProvider.h>
#include <imtrest/IRepresentationDataProvider.h>
#include <imtbase/IItemBasedRepresentationDataProvider.h>


namespace imtqml
{


class CRepresentationDataEnumProviderCompBase :
		public icomp::CComponentBase,
		public imtrest::IRepresentationDataProvider
{

public:
	typedef icomp::CComponentBase BaseClass;

	I_BEGIN_COMPONENT(CRepresentationDataEnumProviderCompBase);
		I_REGISTER_INTERFACE(imtrest::IRepresentationDataProvider);
	I_END_COMPONENT;

	CRepresentationDataEnumProviderCompBase() : BaseClass() {}


	// reimplemented (imtauthgui::IRepresentationDataProvider)
	virtual bool GetRepresentationData(
				imtrest::IRepresentationDataProvider::Format format,
				QByteArray& representationData,
				const QByteArray& commandId = "") override
	{
		bool retVal = false;

		istd::IChangeable::ChangeInfoMap params;

		{
			imtbase::CTreeItemModel rootModel;
			SetEnums(rootModel);
			iser::CJsonStringWriteArchive archive(representationData);
			retVal = rootModel.Serialize(archive);
		}

		return retVal;
	}
	virtual bool SetRepresentationData(
				imtrest::IRepresentationDataProvider::Format format,
				QByteArray& representationData,
				const QByteArray& commandId = "") override
	{
		return false;
	}

protected:
	virtual void SetEnums(imtbase::CTreeItemModel& rootModel) {}

};


} // namespace imtqml

