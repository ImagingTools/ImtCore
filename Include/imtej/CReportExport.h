// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QMap>

// ImtCore includes
#include <imtej/IReportExport.h>


namespace imtej
{


class CReportExport: virtual public IReportExport
{
public:
	virtual bool exportReport(EjDocument* ejDocument, QString const& file_name) override;
private:


};


} // namespace imtej


