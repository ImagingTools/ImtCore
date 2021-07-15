#pragma once


// Qt includes
#include <QtCore/QObject>
#include <QtQml/QJSEngine>
#include <QtQml/QQmlEngine>

// ACF includes
#include <icomp/CComponentBase.h>
#include <iser/CJsonStringWriteArchive.h>

// ImtCore includes
//#include <imtbase/IItemBasedRepresentationDataProvider.h>
//#include <imtrest/IRepresentationDataProvider.h>
//#include <imtbase/IItemBasedRepresentationDataProvider.h>
#include <imtbase/IBinaryDataProvider.h>
#include <imtrest/imtrest.h>


namespace imtqml
{


class CRepresentationDataEnumProviderCompBase :
		public icomp::CComponentBase,
		public imtbase::IBinaryDataProvider
{

public:
	typedef icomp::CComponentBase BaseClass;

	I_BEGIN_COMPONENT(CRepresentationDataEnumProviderCompBase);
		I_REGISTER_INTERFACE(imtbase::IBinaryDataProvider);
		I_ASSIGN(m_dataIdAttrPtr, "Data Id", "Data Id", true, "");
	I_END_COMPONENT;

	CRepresentationDataEnumProviderCompBase() : BaseClass() {}

	// reimplemented (imtbase::IBinaryDataProvider)
	virtual bool GetData(QByteArray& data, const QByteArray& dataId) const override
	{
		bool retVal = false;
		if (m_dataIdAttrPtr.IsValid() && *m_dataIdAttrPtr == dataId){
			imtrest::QweryParams enums;
			GetEnums(enums);

			data = "//pragma Singleton\n"
				"import QtQuick 2.0\n"
				"QtObject {\n";
			for (const QByteArray& key: enums.keys()){
				data += " property string " + key + ": \"" + enums.value(key) + "\"\n";
				retVal = true;
			}
			data += "\n}\n";
		}
		return retVal;
	}

protected:
	I_ATTR(QByteArray, m_dataIdAttrPtr);

	virtual void GetEnums(imtrest::QweryParams& enums) const { }

};


} // namespace imtqml

