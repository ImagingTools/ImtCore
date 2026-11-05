// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
import QtQuick 2.12

/**
 * ProductPermissionStatusProvider
 *
 * Abstract contract telling whether a product published its permission tree.
 * This file MUST NOT import imtguigql or any SDL module.
 */
QtObject {
	signal statusReceived(bool isPublished, string version, string publishedAt)
	signal requestFailed(string message)

	function requestStatus(productId) {}
}
