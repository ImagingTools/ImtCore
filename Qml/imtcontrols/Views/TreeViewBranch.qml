import QtQuick
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtcontrols 1.0


Rectangle {
	id: treeViewBranch

	color: "transparent"

	property int shift: Style.treeBranchOffset
	property int delegateWidth: Style.sizeHintXXS
	property int delegateHeight: Style.controlHeightL
	property int selectedIndex: -1
	property int scrollSize: 12

	property string nameId: "name"
	property string scrollBackgroundColor: ""
	property string scrollIndicatorColor: ""

	property string selectionColor: Style.selectedColor !== undefined ? Style.selectedColor : "lightsteelblue"
	property string hoverColor: Style.hover
	property string textColor: Style.textColor

	property real selectionRadius: 2
	property real selectionOpacity: 1

	property bool hasAddDelegInfo: false
	property bool hasSelection: false

	property var rootRows: null
	property var flatNodes: []

	property alias model: viewModel
	property Component additionalDelegateComp: Component { Item {} }

	signal requestSignal(int index)
	signal clicked(int index)
	signal doubleClicked(int index)
	signal rightButtonMouseClicked(int mX, int mY)
	signal openButtonClicked(int index)
	signal selectionChanged()
	signal openBranch(int index)
	signal closeBranch(int index)
	signal forcedOpen(int index)
	signal inserted(int index)

	BaseModel {
		id: emptyRows
	}

	QtObject {
		id: viewModel

		function getItemsCount() {
			return treeViewBranch.flatNodes ? treeViewBranch.flatNodes.length : 0
		}

		function clear() {
			treeViewBranch.rootRows = null
			treeViewBranch.flatNodes = []
		}

		function getData(key, index) {
			return treeViewBranch.getData(key, index)
		}

		function isValidData(key, index) {
			let data = treeViewBranch.getData(key, index)
			if (data === undefined || data === null) {
				return false
			}
			return true
		}
	}

	onScrollBackgroundColorChanged: {
		if (scrollBackgroundColor !== "") {
			scrollVert.backgroundColor = scrollBackgroundColor
		}
	}
	onScrollIndicatorColorChanged: {
		if (scrollIndicatorColor !== "") {
			scrollVert.indicatorColor = scrollIndicatorColor
		}
	}

	Component {
		id: nodeComp

		Item {
			id: nodeItem

			width: parent ? parent.width : 0
			height: rowItem.height + childList.height

			property var rowNode: model && model.item ? model.item : null
			property int index: rowNode ? rowNode.flatIndex : -1
			property bool isOpen: rowNode ? rowNode.IsOpen__ : false
			property bool hasChildren: treeViewBranch.nodeHasChildren(rowNode)
			property bool hasType: treeViewBranch.itemHasType(rowNode)
			property bool isSelected: index === treeViewBranch.selectedIndex
			property bool isHovered: rowMouse.containsMouse
			property int addItemWidth: 0
			property var additionalItem: null
			property var childRows: rowNode && rowNode.IsOpen__ && rowNode.childrenModel ? rowNode.childrenModel : emptyRows

			Component.onCompleted: {
				if (treeViewBranch.hasAddDelegInfo) {
					additionalItem = treeViewBranch.additionalDelegateComp.createObject(nodeItem)
					nodeItem.addItemWidth = additionalItem.width
				}
			}

			Item {
				id: rowItem

				width: parent.width
				height: treeViewBranch.delegateHeight

				Rectangle {
					id: selectionRect
					anchors.fill: parent
					radius: treeViewBranch.selectionRadius
					opacity: treeViewBranch.selectionOpacity
					color: treeViewBranch.selectionColor
					visible: treeViewBranch.hasSelection && nodeItem.isSelected
					z: -1
				}

				Rectangle {
					id: hoverRect
					anchors.fill: parent
					radius: treeViewBranch.selectionRadius
					opacity: treeViewBranch.selectionOpacity
					color: treeViewBranch.hoverColor
					visible: treeViewBranch.hasSelection && nodeItem.isHovered && !nodeItem.isSelected
					z: -1
				}

				Item {
					id: rowContent

					anchors.left: parent.left
					width: treeViewBranch.delegateWidth
					height: parent.height

					Image {
						id: openButton

						anchors.verticalCenter: parent.verticalCenter
						anchors.left: parent.left
						anchors.leftMargin: 8

						visible: nodeItem.hasChildren
						width: 16
						height: width
						sourceSize.width: width
						sourceSize.height: height
						source: "../../../" + Style.getIconPath(imageName, Icon.State.On, Icon.Mode.Normal)
						property string imageName: nodeItem.isOpen ? "Icons/Down" : "Icons/Right"
					}

					Image {
						id: folderImage

						anchors.verticalCenter: parent.verticalCenter
						anchors.left: openButton.right
						anchors.leftMargin: 8

						width: Style.iconSizeS
						height: width
						sourceSize.width: width
						sourceSize.height: height
						source: !nodeItem.hasType ? "" : treeViewBranch.getIcon(treeViewBranch.fieldValue(nodeItem.rowNode, "typeId__"), nodeItem.isOpen)
					}

					Text {
						id: nameText

						anchors.verticalCenter: parent.verticalCenter
						anchors.left: !nodeItem.hasType ? folderImage.left : folderImage.right
						anchors.leftMargin: !nodeItem.hasType ? 0 : 16

						width: nodeItem.width - x - 20 - nodeItem.addItemWidth
						elide: Text.ElideRight

						font.family: Style.fontFamily
						font.pixelSize: Style.fontSizeXL !== undefined ? Style.fontSizeXL : 18
						color: treeViewBranch.textColor

						text: treeViewBranch.itemText(nodeItem.rowNode)
					}
				}

				MouseArea {
					id: rowMouse

					anchors.fill: parent
					acceptedButtons: Qt.LeftButton | Qt.RightButton
					hoverEnabled: visible
					cursorShape: Qt.PointingHandCursor

					onClicked: {
						if (openButton.visible && mouse.x < openButton.width + 16) {
							treeViewBranch.toggleNode(nodeItem.rowNode)
							return
						}

						treeViewBranch.selectNode(nodeItem.rowNode)

						if (mouse.button === Qt.RightButton) {
							let point = mapToItem(null, mouse.x, mouse.y)
							treeViewBranch.rightButtonMouseClicked(point.x, point.y)
						}
					}

					onDoubleClicked: {
						if (mouse.x > openButton.width + 16) {
							treeViewBranch.doubleClicked(nodeItem.index)
						}
					}
				}
			}

			ListView {
				id: childList

				x: treeViewBranch.shift
				y: rowItem.height
				width: parent.width - x
				visible: nodeItem.childRows && nodeItem.childRows.count > 0
				height: visible ? contentHeight : 0

				interactive: false
				cacheBuffer: 0
				spacing: 0
				clip: false

				model: nodeItem.childRows
				delegate: nodeComp
			}
		}
	}

	ListView {
		id: rootList

		anchors.left: parent.left
		anchors.right: parent.right
		anchors.top: parent.top
		anchors.bottom: parent.bottom
		anchors.rightMargin: treeViewBranch.scrollSize

		clip: true
		boundsBehavior: Flickable.StopAtBounds
		spacing: 0
		model: treeViewBranch.rootRows ? treeViewBranch.rootRows : emptyRows
		delegate: nodeComp
	}

	CustomScrollbar {
		id: scrollVert

		anchors.right: parent.right
		anchors.top: parent.top
		anchors.bottom: parent.bottom

		targetItem: rootList
		secondSize: treeViewBranch.scrollSize
		radius: secondSize
		indicatorRadius: secondSize
		indicatorMargin: 0
	}

	function rowNodeAt(model_, index) {
		if (!model_ || !model_.get) {
			return null
		}
		let row = model_.get(index)
		if (!row) {
			return null
		}
		return row.item
	}

	function rebuild() {
		let flat = []
		let walk = function(model_) {
			if (!model_ || !model_.getItemsCount) {
				return
			}
			let count = model_.getItemsCount()
			for (let i = 0; i < count; i++) {
				let node = treeViewBranch.rowNodeAt(model_, i)
				if (!node) {
					continue
				}
				node.flatIndex = flat.length
				flat.push(node)
				if (node.IsOpen__ && node.childrenModel) {
					walk(node.childrenModel)
				}
			}
		}
		walk(treeViewBranch.rootRows)
		treeViewBranch.flatNodes = flat
	}

	function nodeAt(index) {
		if (!treeViewBranch.flatNodes || index < 0 || index >= treeViewBranch.flatNodes.length) {
			return null
		}
		return treeViewBranch.flatNodes[index]
	}

	function showRoot(model_) {
		treeViewBranch.rootRows = model_
		treeViewBranch.rebuild()
		treeViewBranch.inserted(-1)
	}

	function showChildren(index, model_) {
		let node = treeViewBranch.nodeAt(index)
		if (!node) {
			return
		}
		node.childrenModel = model_
		node.HasBranch__ = true
		node.IsOpen__ = true
		node.OpenState__ = 1
		let added = 0
		if (model_ && model_.getItemsCount) {
			added = model_.getItemsCount()
			node.ChildrenCount__ = added
		}
		if (treeViewBranch.selectedIndex > index) {
			treeViewBranch.selectedIndex = treeViewBranch.selectedIndex + added
		}
		treeViewBranch.rebuild()
		treeViewBranch.inserted(index)
		treeViewBranch.forcedOpen(index)
	}

	function fieldValue(node, key) {
		if (!node || !key) {
			return undefined
		}
		if (node[key] !== undefined) {
			return node[key]
		}
		if (key.indexOf("m_") === 0) {
			let plain = key.slice(2)
			if (node[plain] !== undefined) {
				return node[plain]
			}
		}
		let prop = "m_" + key[0].toLowerCase() + key.slice(1)
		return node[prop]
	}

	function itemText(node) {
		let value = treeViewBranch.fieldValue(node, treeViewBranch.nameId)
		if (value === undefined || value === null) {
			return ""
		}
		return value
	}

	function itemHasType(node) {
		let value = treeViewBranch.fieldValue(node, "typeId__")
		if (value === undefined || value === null || value === "") {
			return false
		}
		return true
	}

	function nodeHasChildren(node) {
		if (!node) {
			return false
		}
		if (node.childrenModel && node.childrenModel.getItemsCount && node.childrenModel.getItemsCount() > 0) {
			return true
		}
		let value = treeViewBranch.fieldValue(node, "hasChildren__")
		if (value === undefined || value === null) {
			return false
		}
		return value
	}

	function getData(key, index) {
		let node = treeViewBranch.nodeAt(index)
		if (!node) {
			return undefined
		}
		return treeViewBranch.fieldValue(node, key)
	}

	function getModelItemsCount(model_) {
		if (!model_ || !model_.getItemsCount) {
			return 0
		}
		return model_.getItemsCount()
	}

	function toggleNode(node) {
		if (!node || !treeViewBranch.nodeHasChildren(node)) {
			return
		}

		if (node.IsOpen__) {
			let parentIndex = node.flatIndex
			let before = treeViewBranch.flatNodes ? treeViewBranch.flatNodes.length : 0
			node.IsOpen__ = false
			node.OpenState__ = 0
			treeViewBranch.rebuild()
			let removed = before - treeViewBranch.flatNodes.length
			if (treeViewBranch.selectedIndex > parentIndex && treeViewBranch.selectedIndex <= parentIndex + removed) {
				treeViewBranch.selectedIndex = parentIndex
			}
			else if (treeViewBranch.selectedIndex > parentIndex + removed) {
				treeViewBranch.selectedIndex = treeViewBranch.selectedIndex - removed
			}
			treeViewBranch.closeBranch(parentIndex)
			treeViewBranch.openButtonClicked(parentIndex)
			return
		}

		if (node.childrenModel && node.childrenModel.getItemsCount && node.childrenModel.getItemsCount() > 0) {
			node.IsOpen__ = true
			node.OpenState__ = 1
			treeViewBranch.rebuild()
			treeViewBranch.openBranch(node.flatIndex)
			treeViewBranch.openButtonClicked(node.flatIndex)
			treeViewBranch.forcedOpen(node.flatIndex)
			return
		}

		node.IsOpen__ = true
		node.OpenState__ = 1
		treeViewBranch.requestSignal(node.flatIndex)
		treeViewBranch.openBranch(node.flatIndex)
		treeViewBranch.openButtonClicked(node.flatIndex)
	}

	function openFunc(index) {
		let node = treeViewBranch.nodeAt(index)
		if (!node) {
			return
		}
		if (!treeViewBranch.nodeHasChildren(node) || node.IsOpen__) {
			treeViewBranch.forcedOpen(index)
			return
		}
		treeViewBranch.toggleNode(node)
	}

	function selectNode(node) {
		if (!node) {
			return
		}
		if (treeViewBranch.selectedIndex !== node.flatIndex) {
			treeViewBranch.selectedIndex = node.flatIndex
			treeViewBranch.selectionChanged()
		}
		treeViewBranch.clicked(node.flatIndex)
	}

	function checkIsOpen(index) {
		let node = treeViewBranch.nodeAt(index)
		if (!node) {
			return false
		}
		return node.IsOpen__
	}

	function findIndexById(id, nameId, startIndex) {
		if (nameId === undefined) {
			nameId = "id"
		}
		if (startIndex === undefined) {
			startIndex = 0
		}
		let count = treeViewBranch.flatNodes ? treeViewBranch.flatNodes.length : 0
		for (let i = startIndex; i < count; i++) {
			let value = treeViewBranch.getData(nameId, i)
			if (value == id) {
				return i
			}
		}
		return -1
	}

	function findParentIndex(index) {
		let node = treeViewBranch.nodeAt(index)
		if (!node) {
			return -1
		}
		let count = treeViewBranch.flatNodes ? treeViewBranch.flatNodes.length : 0
		for (let i = 0; i < count; i++) {
			let parent = treeViewBranch.flatNodes[i]
			if (!parent || !parent.childrenModel || !parent.childrenModel.getItemsCount) {
				continue
			}
			let childCount = parent.childrenModel.getItemsCount()
			for (let c = 0; c < childCount; c++) {
				if (treeViewBranch.rowNodeAt(parent.childrenModel, c) === node) {
					return parent.flatIndex
				}
			}
		}
		return -1
	}

	function moveToElement(index) {
		let node = treeViewBranch.nodeAt(index)
		if (!node) {
			return
		}
		let y = 0
		let found = false
		let walk = function(model_) {
			if (!model_ || !model_.getItemsCount || found) {
				return
			}
			let count = model_.getItemsCount()
			for (let i = 0; i < count; i++) {
				let current = treeViewBranch.rowNodeAt(model_, i)
				if (!current) {
					continue
				}
				if (current === node) {
					found = true
					return
				}
				y = y + treeViewBranch.delegateHeight
				if (current.IsOpen__ && current.childrenModel) {
					walk(current.childrenModel)
					if (found) {
						return
					}
				}
			}
		}
		walk(treeViewBranch.rootRows)
		let maxY = rootList.contentHeight - rootList.height
		if (maxY < 0) {
			maxY = 0
		}
		if (y > maxY) {
			rootList.contentY = maxY
		}
		else {
			rootList.contentY = y
		}
	}

	function getIcon(type, isOpen) {
		let imageName = "Icons/New"
		if (type == "Node") {
			imageName = isOpen ? "Icons/FolderOpened" : "Icons/FolderClosed"
		}
		return "../../../" + Style.getIconPath(imageName, Icon.State.On, Icon.Mode.Normal)
	}
}
