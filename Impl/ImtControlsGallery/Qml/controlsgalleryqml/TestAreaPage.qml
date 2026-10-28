import QtQuick 2.12

import Acf 1.0
import imtcontrols 1.0
import imtguigql 1.0
import controlsgalleryTestAreaSdl 1.0

Rectangle {
	id: testAreaPage

	anchors.fill: parent
	clip: true
	color: Style.baseColor

	property int requestIndex: 0
	property real requestStartedAt: 0
	property bool awaitingLayout: false

	function mark(step) {
		var elapsed = testAreaPage.requestStartedAt > 0 ? Date.now() - testAreaPage.requestStartedAt : 0
		console.log("[TestArea]", testAreaPage.requestIndex, step, elapsed, "ms")
	}
   
	RemoteCollectionView {  
		id: collectionView 

		anchors.fill: parent
		collectionId: "TestArea"
		gqlGetListCommandId: "TestAreaList"
		listPayloadComp: Component { TestAreaListPayload {} }
		commandsPanelVisible: false
		tableViewParamsStoredServer: false
		showRemoteChangesAlert: false

	}

	Connections {
		target: collectionView.dataController

		function onBeginUpdate() {
			testAreaPage.requestIndex = testAreaPage.requestIndex + 1
			testAreaPage.requestStartedAt = Date.now()
			testAreaPage.awaitingLayout = true
			console.log("[TestArea]", testAreaPage.requestIndex, "request started")
		}

		function onHeadersReceived(headers) {
			var count = headers ? headers.getItemsCount() : 0
			testAreaPage.mark("headers received, columns " + count)
		}

		function onElementsReceived(elements) {
			var count = elements ? elements.getItemsCount() : 0
			testAreaPage.mark("payload received, rows " + count)
			layoutTimer.restart()
		}

		function onEndUpdate() {
			testAreaPage.mark("request finished")
		}
	}

	Connections {
		target: collectionView.table

		function onSortingChanged(headerId, sortOrder) {
			var info = collectionView.collectionFilter.getSortingInfo()
			var fieldId = info ? info.m_fieldId : ""
			var order = info ? info.m_sortingOrder : ""
			console.log("[TestArea] sort click", headerId, sortOrder, "stored", fieldId, order)
		}

		function onElementsChanged() {
			testAreaPage.mark("table model assigned, rows " + collectionView.table.elementsCount)
			layoutTimer.restart()
		}
	}

	Timer {
		id: layoutTimer
		interval: 0

		onTriggered: {
			if (!testAreaPage.awaitingLayout) {
				return
			}

			testAreaPage.awaitingLayout = false
			testAreaPage.mark("rows laid out, contentHeight " + collectionView.table.contentHeight)
		}
	}
}
