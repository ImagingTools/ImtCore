// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once

#if __has_include(<rhi/qrhi.h>)
#include <rhi/qrhi.h>
#elif __has_include(<QtGui/qrhi.h>)
#include <QtGui/qrhi.h>
#elif __has_include(<QtGui/private/qrhi_p.h>)
#include <QtGui/private/qrhi_p.h>
#else
#error "QRhi header not found"
#endif
