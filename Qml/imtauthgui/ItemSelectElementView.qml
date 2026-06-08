// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtcontrols 1.0
import imtcolgui 1.0
import imtguigql 1.0

/**
 * ItemSelectElementView (imtauthgui compatibility wrapper)
 *
 * This is a backward-compatible wrapper that re-exports GqlBasedItemSelectElementView
 * from imtguigql. New code should use either:
 *   - imtgui::ItemSelectElementView (base, no GQL dependency — provide your own dataProvider)
 *   - imtguigql::GqlBasedItemSelectElementView (GQL name resolution via collectionId)
 */
GqlBasedItemSelectElementView {
	id: itemSelectElementView
}
