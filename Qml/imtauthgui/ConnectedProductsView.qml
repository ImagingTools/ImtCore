// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
import QtQuick 2.12
import Acf 1.0
import imtdocgui 1.0

/**
 * The page of the connected products. The transport comes from the composing page.
 */
Item {
	id: productsView

	anchors.fill: parent

	property var documentManager: null
	property Component collectionViewComp: null

	MultiDocumentCollectionView {
		anchors.fill: parent
		documentManager: productsView.documentManager
		historyPath: "ConnectedProducts"
		contentColor: Style.baseColor
		pageIcon: "Icons/Product"

		Component.onCompleted: {
			setCollectionViewComp(qsTr("Products"), productsView.collectionViewComp)
		}
	}
}
