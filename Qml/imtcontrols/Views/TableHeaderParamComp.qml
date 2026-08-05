import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtcontrols 1.0


// Column picker of a table, built like the checkable lists that stand next to
// the tree views: header with commands on the right and the rows inside a
// framed pane on the base color.
//
// The list is the order: a row can be dragged to where it belongs, and an
// insertion line shows where it will land. The arrows do the same one step at a
// time, on whichever row is selected.
//
// Every part of the body has a fixed height, so the dialog keeps its size
// whatever is shown inside it.
//
// Nothing is written into the table until Apply. Everything before that happens
// on "entries", a plain ordered copy of the columns.
Dialog {
	id: dialog;

	width: Style.sizeHintL;
	backgroundColor: Style.baseColor;

	title: qsTr("Table configuration");

	// Kept for callers that reach for it; it is filled from "entries" on Apply.
	property TableViewParams tableViewParamsCopied: TableViewParams {};
	property Item tableItem: null;

	// One entry per column: { id, name, visible, size }. Its order is the order
	// of the columns.
	property var entries: [];
	property bool dirty: false;

	// The row the arrows act on, and the one drawn as picked out.
	property int currentRow: -1;
	// Drag in progress: the row being carried, and the gap it would drop into.
	property int dragSourceIndex: -1;
	property int dragTargetIndex: -1;

	property real rowHeight: Style.tableRowHeight;
	readonly property bool reorderEnabled: !dialog.tableItem ? false : dialog.tableItem.canSwapColumns;

	Component.onCompleted: {
		addButton(Enums.apply, qsTr("Apply"), false);
		addButton(Enums.cancel, qsTr("Cancel"), true);
		dialog.rebuildEntries();
	}

	onTableItemChanged: dialog.rebuildEntries();

	onFinished: {
		if (buttonId !== Enums.apply || !dialog.tableItem){
			return;
		}

		dialog.writeParams(dialog.tableViewParamsCopied);

		dialog.tableItem.tableViewParams.copyFrom(dialog.tableViewParamsCopied);
		dialog.tableItem.updateWidthFromViewParams();
		dialog.tableItem.fitToWidthOptional();
		dialog.tableItem.tableViewParamsAccepted();
	}

	// Headers come in two shapes across the code base - objects with "m_" names
	// and plain models - so both are read here rather than at every call site.
	function headerName(index){
		let headers = dialog.tableItem.headers;
		if (headers.containsKey("m_name", index)){
			return headers.getData("m_name", index);
		}
		if (headers.containsKey("name", index)){
			return headers.getData("name", index);
		}

		return dialog.tableItem.getHeaderId(index);
	}

	function rebuildEntries(){
		let rows = [];
		if (dialog.tableItem && dialog.tableItem.headers){
			let params = dialog.tableItem.tableViewParams;
			for (let i = 0; i < dialog.tableItem.headers.getItemsCount(); i++){
				let id = dialog.tableItem.getHeaderId(i);
				if (id === ""){
					continue;
				}

				rows.push({
					"id": id,
					"name": dialog.headerName(i),
					"visible": params.headerIsExists(id) ? params.headerIsVisible(id) : true,
					"size": params.headerIsExists(id) ? params.getHeaderSize(id) : -1
				});
			}
		}

		dialog.entries = rows;
		dialog.currentRow = -1;
		dialog.dirty = false;
		dialog.cancelDrag();
		dialog.updateApplyState();
	}

	function currentName(){
		if (dialog.currentRow < 0 || dialog.currentRow >= dialog.entries.length){
			return "";
		}

		return dialog.entries[dialog.currentRow].name;
	}

	function visibleCount(){
		let count = 0;
		for (let i = 0; i < dialog.entries.length; i++){
			if (dialog.entries[i].visible){
				count++;
			}
		}

		return count;
	}

	function allShown(){
		return dialog.entries.length > 0 && dialog.visibleCount() === dialog.entries.length;
	}

	function updateApplyState(){
		dialog.setButtonEnabled(Enums.apply, dialog.dirty && dialog.visibleCount() > 0);
	}

	// Entries are replaced rather than edited in place: reassigning the list is
	// what re-evaluates the rows that read from it.
	function toggleEntry(index){
		if (index < 0 || index >= dialog.entries.length){
			return;
		}

		let list = dialog.entries.slice();
		let entry = list[index];
		list[index] = {
			"id": entry.id,
			"name": entry.name,
			"visible": !entry.visible,
			"size": entry.size
		};

		dialog.entries = list;
		dialog.dirty = true;
		dialog.updateApplyState();
	}

	function showAll(show){
		let list = [];
		for (let i = 0; i < dialog.entries.length; i++){
			let entry = dialog.entries[i];
			list.push({
				"id": entry.id,
				"name": entry.name,
				"visible": show,
				"size": entry.size
			});
		}

		dialog.entries = list;
		dialog.dirty = true;
		dialog.updateApplyState();
	}

	function moveEntry(from, to){
		if (from < 0 || from >= dialog.entries.length || to < 0 || to >= dialog.entries.length || from === to){
			return;
		}

		let list = dialog.entries.slice();
		let moved = list.splice(from, 1)[0];
		list.splice(to, 0, moved);

		dialog.entries = list;
		dialog.currentRow = to;
		dialog.dirty = true;
		dialog.updateApplyState();
	}

	function moveCurrent(step){
		dialog.moveEntry(dialog.currentRow, dialog.currentRow + step);
	}

	function cancelDrag(){
		dialog.dragSourceIndex = -1;
		dialog.dragTargetIndex = -1;
	}

	// The target is a gap between rows, so dropping below the row it started at
	// lands one place earlier once that row is taken out.
	function dropEntry(){
		let from = dialog.dragSourceIndex;
		let to = dialog.dragTargetIndex;
		dialog.cancelDrag();

		if (from < 0 || to < 0){
			return;
		}

		dialog.moveEntry(from, to > from ? to - 1 : to);
	}

	// The live column order is the row order of this model, and "Order" is what
	// survives a reload, so both are written from the same list.
	function writeParams(params){
		params.clear();
		for (let i = 0; i < dialog.entries.length; i++){
			let entry = dialog.entries[i];
			let size = entry.size;
			// A table that scrolls sideways keeps the width its columns were
			// given; anywhere else the width is shared out again from scratch.
			if (!dialog.tableItem.isFlickable || Number(size) === 0){
				size = entry.visible ? -1 : 0;
			}

			params.setHeaderVisible(entry.id, entry.visible);
			params.setHeaderSize(entry.id, size);
			params.setHeaderOrder(entry.id, i);
		}
	}

	contentComp: Component {
		// The dialog takes its height from what this reports, so the column is
		// the root: its implicit size follows its children on its own.
		Column {
			id: content;

			width: dialog.width;
			topPadding: Style.marginL;
			bottomPadding: Style.marginL;

			spacing: Style.marginM;
			focus: true;

			// Ctrl carries the row itself; on its own the key moves the cursor.
			Keys.onPressed: {
				let carry = dialog.reorderEnabled && (event.modifiers & Qt.ControlModifier);
				if (event.key === Qt.Key_Down){
					if (carry){
						dialog.moveCurrent(1);
					}
					else{
						dialog.currentRow = Math.min(dialog.entries.length - 1, dialog.currentRow + 1);
					}
					event.accepted = true;
				}
				else if (event.key === Qt.Key_Up){
					if (carry){
						dialog.moveCurrent(-1);
					}
					else{
						dialog.currentRow = Math.max(0, dialog.currentRow - 1);
					}
					event.accepted = true;
				}
				else if (event.key === Qt.Key_Space){
					dialog.toggleEntry(dialog.currentRow);
					event.accepted = true;
				}
			}

			Item {
				id: panelHeader;

				anchors.left: parent.left;
				anchors.leftMargin: Style.marginL;
				anchors.right: parent.right;
				anchors.rightMargin: Style.marginL;

				height: Style.controlHeightL;

				BaseText {
					anchors.left: parent.left;
					anchors.right: headerCommands.left;
					anchors.rightMargin: Style.marginS;
					anchors.verticalCenter: parent.verticalCenter;

					text: qsTr("Columns");
					font.family: Style.fontFamilyBold;
					font.pixelSize: Style.fontSizeL;
					color: Style.titleColor;
					elide: Text.ElideRight;
				}

				Row {
					id: headerCommands;

					anchors.right: parent.right;
					anchors.verticalCenter: parent.verticalCenter;

					spacing: Style.spacingXS;

					ToolbarButton {
						objectName: "CheckAllColumnsButton";

						active: dialog.entries.length > 0;
						text: dialog.allShown() ? qsTr("Clear") : qsTr("Show all");

						onClicked: dialog.showAll(!dialog.allShown());
					}

					ToolbarButton {
						objectName: "ResetColumnsButton";

						text: qsTr("Reset");
						tooltipText: qsTr("Reset to default columns");
						iconSource: "qrc:/" + Style.getIconPath("Icons/Restore", Icon.State.On, Icon.Mode.Normal);

						onClicked: ModalDialogManager.openDialog(resetDialog, {});
					}
				}
			}

			Rectangle {
				id: listFrame;

				anchors.left: parent.left;
				anchors.leftMargin: Style.marginL;
				anchors.right: parent.right;
				anchors.rightMargin: Style.marginL;

				// A fixed seven rows and a half: the part-row is what tells the
				// reader the list carries on below.
				height: 2 + Math.round(7.5 * dialog.rowHeight);
				radius: Style.radiusM;
				color: Style.baseColor;
				border.width: 1;
				border.color: Style.borderColor;
				clip: true;

				ListView {
					id: entriesView;

					anchors.fill: parent;
					anchors.margins: 1;
					anchors.rightMargin: 1 + Style.marginM;

					model: dialog.entries;
					clip: true;
					boundsBehavior: Flickable.StopAtBounds;

					delegate: Rectangle {
						id: entryRow;

						width: entriesView.width;
						height: dialog.rowHeight;

						property var entry: index < dialog.entries.length ? dialog.entries[index] : null;
						property bool selected: dialog.currentRow === index;
						property bool carried: dialog.dragSourceIndex === index;

						color: entryRow.selected ? Style.selectedColor
							: rowArea.containsMouse ? Style.hover
							: index % 2 === 1 ? Style.backgroundColor2 : "transparent";
						opacity: entryRow.carried ? 0.5 : 1;

						// The picked-out row wears a bar of its own: a tint alone
						// reads as hover, and the arrows need an unmistakable target.
						Rectangle {
							id: selectionBar;

							anchors.left: parent.left;
							anchors.top: parent.top;
							anchors.bottom: parent.bottom;

							width: Style.spacingXS;
							visible: entryRow.selected || entryRow.carried;
							color: Style.titleColor;
						}

						Item {
							id: grip;

							anchors.left: parent.left;
							anchors.leftMargin: Style.marginM;
							anchors.verticalCenter: parent.verticalCenter;

							width: dialog.reorderEnabled ? Style.iconSizeXS : 0;
							height: Style.iconSizeXS;
							visible: dialog.reorderEnabled;

							Column {
								anchors.centerIn: parent;

								spacing: 2;

								Rectangle {
									width: grip.width;
									height: 2;
									radius: 1;
									color: rowArea.containsMouse ? Style.textColor : Style.inactiveTextColor;
								}
								Rectangle {
									width: grip.width;
									height: 2;
									radius: 1;
									color: rowArea.containsMouse ? Style.textColor : Style.inactiveTextColor;
								}
								Rectangle {
									width: grip.width;
									height: 2;
									radius: 1;
									color: rowArea.containsMouse ? Style.textColor : Style.inactiveTextColor;
								}
							}
						}

						// Its own hit area, above the row's: the box decides whether
						// the column is shown, the row decides which one is picked.
						Item {
							id: checkArea;

							anchors.left: grip.right;
							anchors.leftMargin: Style.marginM;
							anchors.top: parent.top;
							anchors.bottom: parent.bottom;

							width: entryCheck.width + 2 * Style.marginS;
							z: 1;

							CheckBox {
								id: entryCheck;

								anchors.centerIn: parent;

								checkState: !entryRow.entry ? Qt.Unchecked
									: entryRow.entry.visible ? Qt.Checked : Qt.Unchecked;
								mouseArea.enabled: false;
							}

							MouseArea {
								anchors.fill: parent;
								cursorShape: Qt.PointingHandCursor;

								onClicked: {
									content.forceActiveFocus();
									dialog.currentRow = index;
									dialog.toggleEntry(index);
								}
							}
						}

						BaseText {
							anchors.left: checkArea.right;
							anchors.leftMargin: Style.marginS;
							anchors.right: parent.right;
							anchors.rightMargin: Style.marginL;
							anchors.verticalCenter: parent.verticalCenter;

							text: !entryRow.entry ? "" : entryRow.entry.name;
							font.family: entryRow.selected ? Style.fontFamilyBold : Style.fontFamily;
							color: !entryRow.entry ? Style.textColor
								: entryRow.entry.visible ? Style.textColor : Style.inactiveTextColor;
							elide: Text.ElideRight;
						}

						MouseArea {
							id: rowArea;

							anchors.fill: parent;
							hoverEnabled: true;
							cursorShape: !dialog.reorderEnabled ? Qt.ArrowCursor
								: rowArea.carrying ? Qt.ClosedHandCursor : Qt.OpenHandCursor;

							property real pressY: 0;
							property bool carrying: false;

							onPressed: {
								content.forceActiveFocus();
								rowArea.pressY = rowArea.mouseY;
								rowArea.carrying = false;
								dialog.currentRow = index;
							}

							// Guarded by "pressed": hoverEnabled makes this fire for
							// a bare pointer move as well, and a hover must not pick
							// a row up. Travelling far enough does.
							onPositionChanged: {
								if (!dialog.reorderEnabled || !rowArea.pressed){
									return;
								}

								if (!rowArea.carrying){
									if (Math.abs(rowArea.mouseY - rowArea.pressY) < dialog.rowHeight / 3){
										return;
									}

									rowArea.carrying = true;
									dialog.dragSourceIndex = index;
								}

								let point = rowArea.mapToItem(entriesView.contentItem, 0, rowArea.mouseY);
								let gap = Math.round(point.y / dialog.rowHeight);
								dialog.dragTargetIndex = Math.max(0, Math.min(dialog.entries.length, gap));
							}

							onReleased: {
								if (rowArea.carrying){
									rowArea.carrying = false;
									dialog.dropEntry();
								}
							}

							// The grab can be taken away mid-drag - by the dialog
							// closing, or by a press of the other button.
							onCanceled: {
								rowArea.carrying = false;
								dialog.cancelDrag();
							}
						}
					}
				}

				// Where the carried row would land.
				Rectangle {
					x: entriesView.x;
					y: entriesView.y + dialog.dragTargetIndex * dialog.rowHeight - entriesView.contentY;
					z: entriesView.z + 1;

					width: entriesView.width;
					height: 2;
					radius: 1;
					visible: dialog.dragSourceIndex >= 0 && dialog.dragTargetIndex >= 0;
					color: Style.titleColor;
				}

				CustomScrollbar {
					z: entriesView.z + 2;

					anchors.right: parent.right;
					anchors.rightMargin: Style.marginXS;
					anchors.top: entriesView.top;
					anchors.topMargin: Style.marginXS;
					anchors.bottom: entriesView.bottom;
					anchors.bottomMargin: Style.marginXS;

					secondSize: Style.marginS;
					targetItem: entriesView;
					radius: Style.radiusS;
				}

				BaseText {
					anchors.centerIn: parent;

					width: parent.width - 2 * Style.marginXL;
					visible: dialog.entries.length === 0;
					horizontalAlignment: Text.AlignHCenter;
					text: qsTr("This table has no columns to configure");
					color: Style.subtitleColor;
					wrapMode: Text.WordWrap;
				}
			}

			Item {
				id: commandRow;

				anchors.left: parent.left;
				anchors.leftMargin: Style.marginL;
				anchors.right: parent.right;
				anchors.rightMargin: Style.marginL;

				height: Style.controlHeightM;

				Row {
					id: moveCommands;

					anchors.left: parent.left;
					anchors.verticalCenter: parent.verticalCenter;

					spacing: Style.spacingXS;

					visible: dialog.reorderEnabled;

					ToolbarButton {
						id: upButton;

						objectName: "MoveColumnUpButton";

						active: dialog.currentRow > 0;
						text: qsTr("Move up");
						iconSource: "qrc:/" + Style.getIconPath("Icons/Up", Icon.State.On,
							upButton.active ? Icon.Mode.Normal : Icon.Mode.Disabled);

						onClicked: dialog.moveCurrent(-1);
					}

					ToolbarButton {
						id: downButton;

						objectName: "MoveColumnDownButton";

						active: dialog.currentRow >= 0 && dialog.currentRow < dialog.entries.length - 1;
						text: qsTr("Move down");
						iconSource: "qrc:/" + Style.getIconPath("Icons/Down", Icon.State.On,
							downButton.active ? Icon.Mode.Normal : Icon.Mode.Disabled);

						onClicked: dialog.moveCurrent(1);
					}
				}

				// Names what the arrows would move, so the pair is never aimed at
				// a row the reader has lost track of.
				BaseText {
					anchors.left: moveCommands.visible ? moveCommands.right : parent.left;
					anchors.leftMargin: moveCommands.visible ? Style.marginM : 0;
					anchors.right: fitToWidthButton.visible ? fitToWidthButton.left : parent.right;
					anchors.rightMargin: Style.marginM;
					anchors.verticalCenter: parent.verticalCenter;

					visible: dialog.reorderEnabled && dialog.entries.length > 0;
					text: dialog.currentRow < 0 ? qsTr("Select a column to move it") : dialog.currentName();
					font.family: dialog.currentRow < 0 ? Style.fontFamily : Style.fontFamilyBold;
					color: Style.subtitleColor;
					elide: Text.ElideRight;
				}

				ToolbarButton {
					id: fitToWidthButton;

					objectName: "FitToWidthButton";

					anchors.right: parent.right;
					anchors.verticalCenter: parent.verticalCenter;

					visible: !dialog.tableItem ? false : dialog.tableItem.isFlickable;
					text: qsTr("Fit to width");
					tooltipText: qsTr("Fit columns to available width");
					iconSource: "qrc:/" + Style.getIconPath("Icons/FitToWidth", Icon.State.On, Icon.Mode.Normal);

					onClicked: {
						if (dialog.tableItem){
							dialog.tableItem.fitToWidth();
							dialog.accepted();
						}
					}
				}
			}

			// Holds its height whether or not it has anything to say.
			Item {
				anchors.left: parent.left;
				anchors.leftMargin: Style.marginL;
				anchors.right: parent.right;
				anchors.rightMargin: Style.marginL;

				height: Style.controlHeightS;

				BaseText {
					anchors.fill: parent;

					verticalAlignment: Text.AlignVCenter;
					visible: dialog.visibleCount() === 0 && dialog.entries.length > 0;
					text: qsTr("Show at least one column");
					color: Style.errorTextColor;
					elide: Text.ElideRight;
				}
			}
		}
	}

	Component {
		id: resetDialog;

		MessageDialog {
			width: Style.sizeHintM;
			title: qsTr("Confirm reset headers");
			message: qsTr("Reset header settings to default ?");

			onFinished: {
				if (buttonId === Enums.yes){
					dialog.tableItem.resetViewParams();

					dialog.accepted();
				}
			}
		}
	}
}
