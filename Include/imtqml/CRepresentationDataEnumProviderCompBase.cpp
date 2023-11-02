#include <imtqml/CRepresentationDataEnumProviderCompBase.h>


namespace imtqml
{


// reimplemented (imtbase::IBinaryDataProvider)

bool CRepresentationDataEnumProviderCompBase::GetData(
			QByteArray& data,
			const QByteArray& dataId,
			qint64 /*readFromPosition*/,
			qint64 /*readMaxLength*/) const
{
	bool retVal = false;
	if (m_dataIdAttrPtr.IsValid() && *m_dataIdAttrPtr == dataId){
		imtrest::QueryParams enums;
		GetEnums(enums);

		data = "//pragma Singleton\n"
			"import QtQuick 2.0\n"
			"QtObject {\n";
		for (const QByteArray& key : enums.keys()){
			data += " property string " + key + ": \"" + enums.value(key) + "\"\n";
			retVal = true;
		}
		data += "\n}\n";
	}
	return retVal;
}


} // namespace imtqml


