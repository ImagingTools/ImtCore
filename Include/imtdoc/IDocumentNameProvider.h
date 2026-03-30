// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <istd/IPolymorphic.h>

// Qt includes
#include <QtCore/QByteArray>
#include <QtCore/QString>


namespace istd
{
class IChangeable;
}


namespace imtdoc
{


class IDocumentNameProvider: virtual public istd::IPolymorphic
{
public:
	virtual QString GetDefaultDocumentName(const QByteArray& objectId, const istd::IChangeable& document) const = 0;
};


} // namespace imtdoc
