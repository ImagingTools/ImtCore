// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtcontrols 1.0

/**
 * PermissionDetailsPanel
 *
 * Compact strip under a permission tree or table: the path of the selected permission with its
 * mark, where it comes from and what it requires. Every line is always laid out, so the strip
 * keeps one height while the selection moves; it grows only while a long line is expanded.
 */
Rectangle {
	id: panel

	property string path: ""
	property string badge: ""
	property string description: ""
	property string source: ""
	property string requires: ""
	property bool showDescription: true
	property bool showRequires: true
	property string placeholderText: qsTr("Select a permission to see where it comes from")
	property bool __pathCopied: false

	onPathChanged: panel.__pathCopied = false

	function copyPath() {
		if (panel.path === "")
			return
		clipboardProxy.text = panel.path
		clipboardProxy.selectAll()
		clipboardProxy.copy()
		clipboardProxy.select(0, 0)
		panel.__pathCopied = true
		copiedTimer.restart()
	}

	TextEdit {
		id: clipboardProxy
		width: 0
		height: 0
		visible: false
		readOnly: true
	}

	Timer {
		id: copiedTimer
		interval: 1500
		onTriggered: panel.__pathCopied = false
	}

	readonly property real __labelWidth: Math.max(sourceRow.labelImplicitWidth, requiresRow.labelImplicitWidth, descriptionRow.labelImplicitWidth)

	height: contentColumn.height + 2 * Style.marginM
	radius: Style.radiusM
	color: Style.baseColor
	border.color: Style.borderColor
	border.width: 1

	Column {
		id: contentColumn
		anchors.top: parent.top
		anchors.topMargin: Style.marginM
		anchors.left: parent.left
		anchors.leftMargin: Style.marginL
		anchors.right: parent.right
		anchors.rightMargin: Style.marginL
		spacing: Style.spacingXS

		Item {
			width: contentColumn.width
			height: Math.max(pathText.implicitHeight, badgeItem.height, copyButton.height)

			BaseText {
				id: pathText
				objectName: "PermissionDetailsPath"
				anchors.left: parent.left
				anchors.right: copyButton.visible ? copyButton.left : (badgeItem.visible ? badgeItem.left : parent.right)
				anchors.rightMargin: copyButton.visible || badgeItem.visible ? Style.marginS : 0
				anchors.verticalCenter: parent.verticalCenter
				text: panel.path !== "" ? panel.path : panel.placeholderText
				font.family: panel.path !== "" ? Style.fontFamilyBold : Style.fontFamily
				font.pixelSize: Style.fontSizeM
				color: panel.path !== "" ? Style.textColor : Style.inactiveTextColor
				// The tail of a path tells it apart from its siblings.
				elide: Text.ElideLeft
			}

			ToolButton {
				id: copyButton
				objectName: "PermissionDetailsCopyPath"
				anchors.right: badgeItem.visible ? badgeItem.left : parent.right
				anchors.rightMargin: badgeItem.visible ? Style.marginM : 0
				anchors.verticalCenter: parent.verticalCenter
				width: Style.buttonHeightS
				height: width
				visible: panel.path !== ""
				iconSource: panel.__pathCopied
					? "qrc:/" + Style.getIconPath("Icons/Ok", Icon.State.On, Icon.Mode.Normal)
					: "qrc:/" + Style.getIconPath("Icons/Copy", Icon.State.On, Icon.Mode.Normal)
				tooltipText: panel.__pathCopied ? qsTr("The path is copied") : qsTr("Copy the full path")
				onClicked: panel.copyPath()
			}

			StatusBadge {
				id: badgeItem
				anchors.right: parent.right
				anchors.verticalCenter: parent.verticalCenter
				visible: panel.path !== "" && panel.badge !== ""
				text: panel.badge
				textColor: Style.inactiveTextColor
			}
		}

		PermissionDetailsRow {
			id: descriptionRow
			objectName: "PermissionDetailsDescription"
			width: contentColumn.width
			visible: panel.showDescription
			labelWidth: panel.__labelWidth
			label: qsTr("Description")
			text: panel.path !== "" ? panel.description : ""
		}

		PermissionDetailsRow {
			id: sourceRow
			objectName: "PermissionDetailsSource"
			width: contentColumn.width
			labelWidth: panel.__labelWidth
			label: qsTr("Source")
			text: panel.path !== "" ? panel.source : ""
			separator: "; "
		}

		PermissionDetailsRow {
			id: requiresRow
			objectName: "PermissionDetailsRequires"
			width: contentColumn.width
			visible: panel.showRequires
			labelWidth: panel.__labelWidth
			label: qsTr("Requires")
			text: panel.path !== "" ? panel.requires : ""
			separator: ", "
		}
	}
}
