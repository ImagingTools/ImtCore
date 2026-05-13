// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtdbgql/CSdlMetaInfoTools.h>


namespace imtdbgql
{


void CSdlMetaInfoTools::GetMetaInfo(const QVariant& metaInfoData, istd::TSharedNullable<bool>& output)
{
	bool metaInfoValue = metaInfoData.toBool();
	output = metaInfoValue;
}


void CSdlMetaInfoTools::GetMetaInfo(const QVariant& metaInfoData, istd::TSharedNullable<int>& output)
{
	bool ok = false;
	int metaInfoValue = metaInfoData.toInt(&ok);
	if (ok){
		output = metaInfoValue;
	}
}


void CSdlMetaInfoTools::GetMetaInfo(const QVariant& metaInfoData, istd::TSharedNullable<long>& output)
{
	bool ok = false;
	long metaInfoValue = metaInfoData.toLongLong(&ok);
	if (ok){
		output = metaInfoValue;
	}
}


void CSdlMetaInfoTools::GetMetaInfo(const QVariant& metaInfoData, istd::TSharedNullable<long long>& output)
{
	bool ok = false;
	long long metaInfoValue = metaInfoData.toLongLong(&ok);
	if (ok){
		output = metaInfoValue;
	}
}


void CSdlMetaInfoTools::GetMetaInfo(const QVariant& metaInfoData, istd::TSharedNullable<float>& output)
{
	bool ok = false;
	float metaInfoValue = metaInfoData.toFloat(&ok);
	if (ok){
		output = metaInfoValue;
	}
}


void CSdlMetaInfoTools::GetMetaInfo(const QVariant& metaInfoData, istd::TSharedNullable<double>& output)
{
	bool ok = false;
	double metaInfoValue = metaInfoData.toDouble(&ok);
	if (ok){
		output = metaInfoValue;
	}
}


void CSdlMetaInfoTools::GetMetaInfo(const QVariant& metaInfoData, istd::TSharedNullable<QString>& output)
{
	QString metaInfoValue = metaInfoData.toString();
	output = std::move(metaInfoValue);
}


void CSdlMetaInfoTools::GetMetaInfo(const QVariant& metaInfoData, istd::TSharedNullable<QByteArray>& output)
{
	QByteArray metaInfoValue = metaInfoData.toByteArray();
	output = std::move(metaInfoValue);
}


} // namespace imtdbgql


