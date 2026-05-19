// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QJsonObject>
#include <QtCore/QVariant>

// ImtCore includes
#include <imtbase/CFilter.h>


namespace imtbase
{


/**
 * @brief Converts the SDL/QML transport representation of a standalone filter
 *        into the C++ CFilter model used on the server.
 *
 * The converter is a semantic SDL/QML boundary.  The actual JSON/query-string
 * mapping is shared with CFilterSerializer so URL/API transport and SDL
 * transport stay aligned.
 */
class CFilterSdlConverter
{
public:
    static QJsonObject ToSdlJson(const CFilter& filter);
    static bool FromSdlJson(const QJsonObject& filterModel, CFilter& filter);
    static bool FromQmlModel(const QVariant& filterModel, CFilter& filter);
};


} // namespace imtbase

