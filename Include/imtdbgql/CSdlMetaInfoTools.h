// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QByteArray>


// ACF includes
#include <istd/TSharedNullable.h>
#include <idoc/IDocumentMetaInfo.h>


namespace imtdbgql
{

/**
	This is a comfort class for transferring metadata to a \c istd::TSharedNullable container from \c idoc::IDocumentMetaInfo.
*/
class CSdlMetaInfoTools
{
public:
	static void GetMetaInfo(const QVariant& metaInfoData, istd::TSharedNullable<bool>& output);
	static void GetMetaInfo(const QVariant& metaInfoData, istd::TSharedNullable<int>& output);
	static void GetMetaInfo(const QVariant& metaInfoData, istd::TSharedNullable<long>& output);
	static void GetMetaInfo(const QVariant& metaInfoData, istd::TSharedNullable<long long>& output);
	static void GetMetaInfo(const QVariant& metaInfoData, istd::TSharedNullable<float>& output);
	static void GetMetaInfo(const QVariant& metaInfoData, istd::TSharedNullable<double>& output);
	static void GetMetaInfo(const QVariant& metaInfoData, istd::TSharedNullable<QString>& output);
	static void GetMetaInfo(const QVariant& metaInfoData, istd::TSharedNullable<QByteArray>& output);

	template<class T>
	static void GetMetaInfo(
			const idoc::IDocumentMetaInfo& metaInfo,
			int metaInfoType,
			istd::TSharedNullable<T>& output,
			bool setNullIfNotExists = true);
};


// inline methods
template<class T>
inline void CSdlMetaInfoTools::GetMetaInfo(const idoc::IDocumentMetaInfo& metaInfo, int metaInfoType, istd::TSharedNullable<T>& output, bool setNullIfNotExists)
{
	QVariant metaInfoData = metaInfo.GetMetaInfo(metaInfoType);
	if (metaInfoData.isNull()) {
		if (setNullIfNotExists) {
			output.SetNull();
		}
		return;
	}

	GetMetaInfo(metaInfoData, output);
}




} // namespace imtdbgql


