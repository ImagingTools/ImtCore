// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QString>

// ACF includes
#include <iser/ISerializable.h>

// ImtCore includes
#include <imtsdl/imtsdl.h>
#include <imtsdl/CSdlEntryBase.h>


namespace imtsdl
{


class CSdlUnion;
typedef QList<CSdlUnion> SdlUnionList;


class CSdlUnion: virtual public CSdlEntryBase
{

public:
	typedef CSdlEntryBase BaseClass;

	/// \todo rename it to _TypeName and add methods , that returns \c CSdlType
	[[nodiscard]] QList<QString> GetTypes() const;
	void SetTypes(const QList<QString>& types);
	void AddType(const QString& type);

	// reimplemented(iser::ISerializable)
	virtual bool Serialize(iser::IArchive& archive) override;

	// operators
	[[nodiscard]] bool operator==(const CSdlUnion& other) const;
	[[nodiscard]] bool operator!=(const CSdlUnion& other) const { return !(operator==(other)); }

private:
	QList<QString> m_types;
};


}
