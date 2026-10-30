import QtQuick 2.12
import Acf 1.0
import imtcontrols 1.0

Item {
	id: root

	property bool initialItemTitleVisible: true

	// Width the headers take without collapsing, for a page that places its commands next to them
	readonly property real implicitContentWidth: (buttonPanel.visible ? buttonPanel.width + Style.marginXS : 0) + root.__fullWidth

	// The headers that do not fit are collapsed into "...": the one at __collapsedFrom shows it,
	// the ones after it up to __collapsedTo are hidden; -1 while everything fits
	property int __collapsedFrom: -1
	property int __collapsedTo: -1
	property real __fullWidth: 0
	// Width left for the name of the current header, which is elided when even the collapsed headers do not fit
	property real __currentMaxWidth: -1
	readonly property int __firstShownIndex: root.initialItemTitleVisible ? 0 : 1
	readonly property int __lastIndex: headersModel.count - 1

	signal closeClicked()
	signal headerItemClicked(int index, string headerId)

	onInitialItemTitleVisibleChanged: root.__scheduleLayout()

	ListModel {
		id: headersModel
	}

	function addHeader(id, name){
		headersModel.append({id: id, name: name, dirty: false})
		root.__scheduleLayout()
	}

	function getIndexById(id){
		for (let i = 0; i < headersModel.count; i++){
			if (headersModel.get(i).id === id){
				return i
			}
		}

		return -1
	}

	function popHeader(){
		if (headersModel.count > 0){
			removeHeader(headersModel.get(headersModel.count - 1).id)
		}
	}

	function removeHeader(id){
		let index = root.getIndexById(id)
		if (index >= 0){
			headersModel.remove(index)
			root.__scheduleLayout()
		}
	}

	function setHeaderName(id, name){
		let index = root.getIndexById(id)
		if (index >= 0){
			headersModel.setProperty(index, "name", name)
			root.__scheduleLayout()
		}
	}

	function getHeaderName(id){
		let index = root.getIndexById(id)
		if (index >= 0){
			return headersModel.get(index).name
		}

		return ""
	}

	// Marks a header as having unsaved changes
	function setHeaderDirty(id, dirty){
		let index = root.getIndexById(id)
		if (index >= 0){
			headersModel.setProperty(index, "dirty", dirty)
			root.__scheduleLayout()
		}
	}

	function getHeaderDirty(id){
		let index = root.getIndexById(id)
		if (index >= 0){
			return headersModel.get(index).dirty
		}

		return false
	}

	function __scheduleLayout(){
		layoutTimer.restart()
	}

	// Waits for the measuring copies to take the new names
	Timer {
		id: layoutTimer
		interval: 0
		onTriggered: {
			root.__updateLayout()
		}
	}

	function __crumbWidth(index){
		let measure = measureRepeater.itemAt(index)
		if (!measure){
			return 0
		}

		let textWidth = index === root.__lastIndex ? measure.currentWidth : measure.ancestorWidth
		let dotWidth = headersModel.get(index).dirty ? Style.marginS + Style.marginXS : 0
		return textWidth + dotWidth + 2 * Style.marginXS
	}

	// Width of the shown headers with the ones from `from` to `to` collapsed into "..."
	function __rowWidth(from, to){
		let width = 0
		let shown = 0
		for (let i = root.__firstShownIndex; i <= root.__lastIndex; i++){
			if (from >= 0 && i > from && i <= to){
				continue
			}
			width += i === from ? ellipsisMeasure.implicitWidth + 2 * Style.marginXS : root.__crumbWidth(i)
			shown++
		}

		return width + Math.max(0, shown - 1) * (Style.iconSizeXS + 2 * Style.marginXXS)
	}

	// The middle headers collapse first, keeping the first one and the last two; then all but the current one
	function __updateLayout(){
		let first = root.__firstShownIndex
		let last = root.__lastIndex
		let available = crumbsArea.width
		let from = -1
		let to = -1

		root.__fullWidth = last >= first ? root.__rowWidth(-1, -1) : 0
		if (last > first && root.__fullWidth > available){
			for (let end = first + 1; end <= last - 2; end++){
				if (root.__rowWidth(first + 1, end) <= available){
					from = first + 1
					to = end
					break
				}
			}
			if (from < 0){
				from = first
				to = last - 1
			}
		}

		root.__collapsedFrom = from
		root.__collapsedTo = to
		root.__currentMaxWidth = last >= first ? available - (root.__rowWidth(from, to) - root.__crumbWidth(last)) : -1
	}

	function __activate(index, item){
		if (index === root.__collapsedFrom){
			root.__openCollapsedMenu(item)
			return
		}

		root.headerItemClicked(index, headersModel.get(index).id)
	}

	ListModel {
		id: collapsedModel
	}

	function __openCollapsedMenu(item){
		collapsedModel.clear()
		for (let i = root.__collapsedFrom; i <= root.__collapsedTo; i++){
			collapsedModel.append({"id": String(i), "name": headersModel.get(i).name, "icon": "", "isEnabled": true})
		}

		let point = item.mapToItem(null, 0, item.height)
		ModalDialogManager.openDialog(collapsedMenuComp, {"x": point.x, "y": point.y, "model": collapsedModel})
	}

	Component {
		id: collapsedMenuComp

		PopupMenuDialog {
			onFinished: {
				let index = parseInt(commandId)
				if (index >= 0 && index < headersModel.count){
					root.headerItemClicked(index, headersModel.get(index).id)
				}
			}
		}
	}

	Item {
		id: buttonPanel
		anchors.verticalCenter: parent.verticalCenter
		anchors.left: parent.left
		width: visible ? closeButton.width : 0
		height: closeButton.height
		visible: headersModel.count > 1

		ToolButton {
			id: closeButton
			anchors.centerIn: buttonPanel
			width: Style.controlHeightM
			height: width
			iconSource: "../../../" + Style.getIconPath("Icons/Left", Icon.State.On, Icon.Mode.Normal)
			onClicked: {
				root.closeClicked()
			}
		}
	}

	// The names laid out unelided in both fonts, to measure them; transparent, as a hidden item is not laid out
	Row {
		id: measureRow
		opacity: 0
		enabled: false

		Repeater {
			id: measureRepeater
			model: headersModel
			delegate: Item {
				readonly property real ancestorWidth: ancestorMeasure.implicitWidth
				readonly property real currentWidth: currentMeasure.implicitWidth

				Text {
					id: ancestorMeasure
					font.pixelSize: Style.fontSizeL
					font.family: Style.fontFamily
					text: model.name
				}

				Text {
					id: currentMeasure
					font.pixelSize: Style.fontSizeXL
					font.family: Style.fontFamily
					font.bold: true
					text: model.name
				}
			}
		}
	}

	Text {
		id: ellipsisMeasure
		opacity: 0
		font.pixelSize: Style.fontSizeL
		font.family: Style.fontFamily
		text: "…"
	}

	Item {
		id: crumbsArea
		anchors.top: parent.top
		anchors.bottom: parent.bottom
		anchors.left: buttonPanel.right
		anchors.leftMargin: buttonPanel.visible ? Style.marginXS : 0
		anchors.right: parent.right
		clip: true

		onWidthChanged: {
			root.__scheduleLayout()
		}

		Row {
			id: crumbsRow
			anchors.verticalCenter: parent.verticalCenter
			spacing: Style.marginXXS

			Repeater {
				model: headersModel
				delegate: Row {
					id: crumbDelegate

					readonly property bool isCurrent: model.index === root.__lastIndex
					readonly property bool isEllipsis: model.index === root.__collapsedFrom

					visible: model.index >= root.__firstShownIndex
						&& !(root.__collapsedFrom >= 0 && model.index > root.__collapsedFrom && model.index <= root.__collapsedTo)
					spacing: Style.marginXXS

					Item {
						id: crumb
						anchors.verticalCenter: parent.verticalCenter
						width: crumbContent.width + 2 * Style.marginXS
						height: Style.controlHeightS + Style.marginXS
						activeFocusOnTab: !crumbDelegate.isCurrent

						Keys.onReturnPressed: {
							root.__activate(model.index, crumb)
						}
						Keys.onSpacePressed: {
							root.__activate(model.index, crumb)
						}

						Rectangle {
							anchors.fill: parent
							radius: Style.radiusS
							color: crumbMouse.containsMouse && !crumbDelegate.isCurrent ? Style.menuPanelItemHoverColor : "transparent"
							border.width: crumb.activeFocus ? 2 : 0
							border.color: Style.linkColor
						}

						Row {
							id: crumbContent
							x: Style.marginXS
							anchors.verticalCenter: parent.verticalCenter
							spacing: Style.marginXS

							Text {
								id: crumbText
								anchors.verticalCenter: parent.verticalCenter
								width: crumbDelegate.isCurrent && root.__currentMaxWidth >= 0
									? Math.max(0, Math.min(implicitWidth, root.__currentMaxWidth - 2 * Style.marginXS - (dot.visible ? dot.width + Style.marginXS : 0)))
									: implicitWidth
								elide: Text.ElideRight
								font.pixelSize: crumbDelegate.isCurrent ? Style.fontSizeXL : Style.fontSizeL
								font.family: Style.fontFamily
								font.bold: crumbDelegate.isCurrent
								color: crumbDelegate.isCurrent || crumbMouse.containsMouse ? Style.titleColor : Style.subtitleColor
								text: crumbDelegate.isEllipsis ? "…" : model.name
							}

							Rectangle {
								id: dot
								anchors.verticalCenter: parent.verticalCenter
								width: Style.marginS
								height: width
								radius: width / 2
								color: Style.linkColor
								visible: model.dirty && !crumbDelegate.isEllipsis
							}
						}

						MouseArea {
							id: crumbMouse
							anchors.fill: parent
							hoverEnabled: true
							enabled: !crumbDelegate.isCurrent
							cursorShape: Qt.PointingHandCursor
							onClicked: {
								root.__activate(model.index, crumb)
							}
						}
					}

					Image {
						anchors.verticalCenter: parent.verticalCenter
						width: Style.iconSizeXS
						height: width
						sourceSize.width: width
						sourceSize.height: height
						visible: !crumbDelegate.isCurrent
						source: "../../../" + Style.getIconPath("Icons/Right", Icon.State.Off, Icon.Mode.Disabled)
					}
				}
			}
		}
	}
}
