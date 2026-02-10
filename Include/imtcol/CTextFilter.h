// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtcol/ITextFilter.h>


namespace imtcol
{


class CTextFilter : virtual public ITextFilter
{
public:
	// reimplemented (imtcol::ITextFilter)
	virtual QByteArrayList GetFilteringFieldIds() const override;
	virtual void SetFilteringFieldIds(const QByteArrayList& fieldIds) override;
	virtual QString GetFilterText() const override;
	virtual void SetFilterText(const QString& text) override;

	// reimplemented (iser::ISerializable)
	virtual bool Serialize(iser::IArchive& archive) override;

	// reimplemented (istd::IChangeable)
	virtual int GetSupportedOperations() const override;
	virtual bool CopyFrom(const IChangeable& object, CompatibilityMode mode = CM_WITHOUT_REFS) override;
	virtual istd::IChangeableUniquePtr CloneMe(CompatibilityMode mode = CM_WITHOUT_REFS) const override;
	virtual bool ResetData(CompatibilityMode mode = CM_WITHOUT_REFS) override;

private:
	QByteArrayList m_fieldIds;
	QString m_text;
};


} // namespace imtcol


