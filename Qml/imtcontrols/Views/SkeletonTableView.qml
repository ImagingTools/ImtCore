import QtQuick 2.12
import Acf 1.0

/**
	Skeleton of a table or of a list of cards: an optional header and as many rows
	as fit. Same start()/stop() API as Loading; covers and blocks the view below it.
*/
Rectangle {
	id: skeletonTable

	// Same objectName as Loading, so GUI tests waiting for the busy indicator keep working.
	objectName: "BusyIndicator"

	property bool headerVisible: true
	property int columnCount: 4
	property int rowHeight: Style.controlHeightL
	// Card rows: a title line over a shorter description line, without column cells.
	property bool cardRows: false
	property int rowSpacing: 0
	// Rows are centered in a column this wide; 0 lets them span the whole view.
	property int contentMaxWidth: 0
	property int contentTopMargin: 0
	// Caps the rows built; 0 fills the view. Every row costs its creation, so short placeholders stay cheap.
	property int maxRows: 0
	property int phase: 0

	color: Style.baseColor
	visible: false

	function start(){
		skeletonTable.phase = 0
		skeletonTable.visible = true
	}

	function stop(){
		skeletonTable.visible = false
	}

	MouseArea {
		anchors.fill: parent
		hoverEnabled: true
		acceptedButtons: Qt.AllButtons

		onWheel: {
			wheel.accepted = true
		}
	}

	Timer {
		interval: 80
		repeat: true
		running: skeletonTable.visible

		onTriggered: {
			skeletonTable.phase = (skeletonTable.phase + 1) % 16
		}
	}

	Item {
		id: content

		x: (skeletonTable.width - width) / 2
		y: skeletonTable.contentTopMargin
		width: skeletonTable.contentMaxWidth > 0 ? Math.min(skeletonTable.contentMaxWidth, skeletonTable.width) : skeletonTable.width
		height: skeletonTable.height - skeletonTable.contentTopMargin
		clip: true
		opacity: 0.45 + 0.55 * (skeletonTable.phase < 8 ? skeletonTable.phase / 8 : (16 - skeletonTable.phase) / 8)

		Item {
			id: header

			width: parent.width
			height: skeletonTable.headerVisible ? Style.controlHeightM : 0
			visible: skeletonTable.headerVisible

			Row {
				anchors.left: parent.left
				anchors.leftMargin: Style.marginM
				anchors.verticalCenter: parent.verticalCenter
				spacing: Style.marginM

				Repeater {
					model: skeletonTable.columnCount

					delegate: Item {
						width: (header.width - Style.marginM) / skeletonTable.columnCount - Style.marginM
						height: Style.iconSizeXS

						SkeletonBlock {
							width: parent.width * 0.4
							height: parent.height
						}
					}
				}
			}

			Rectangle {
				anchors.bottom: parent.bottom
				width: parent.width
				height: 1
				color: Style.borderColor
			}
		}

		Column {
			id: rows

			anchors.top: header.bottom
			width: parent.width
			spacing: skeletonTable.rowSpacing

			Repeater {
				// Built only while shown: a hidden skeleton would rebuild every row on each layout pass.
				model: !skeletonTable.visible ? 0
					 : skeletonTable.maxRows > 0 ? Math.min(skeletonTable.maxRows, Math.max(1, Math.ceil((content.height - header.height) / (skeletonTable.rowHeight + skeletonTable.rowSpacing))))
					 : Math.max(1, Math.ceil((content.height - header.height) / (skeletonTable.rowHeight + skeletonTable.rowSpacing)))

				delegate: Item {
					id: row

					property int rowIndex: index

					width: rows.width
					height: skeletonTable.rowHeight

					Row {
						anchors.left: parent.left
						anchors.leftMargin: Style.marginM
						anchors.verticalCenter: parent.verticalCenter
						spacing: Style.marginM
						visible: !skeletonTable.cardRows

						Repeater {
							model: skeletonTable.cardRows ? 0 : skeletonTable.columnCount

							delegate: Item {
								width: (rows.width - Style.marginM) / skeletonTable.columnCount - Style.marginM
								height: Style.iconSizeXS

								SkeletonBlock {
									width: parent.width * (0.35 + 0.15 * ((row.rowIndex + index) % 4))
									height: parent.height
								}
							}
						}
					}

					Column {
						anchors.left: parent.left
						anchors.leftMargin: Style.marginM
						anchors.right: parent.right
						anchors.rightMargin: Style.marginM
						anchors.verticalCenter: parent.verticalCenter
						spacing: Style.marginS
						visible: skeletonTable.cardRows

						// Built only for card rows; table rows never show these lines.
						Repeater {
							model: skeletonTable.cardRows ? 1 : 0

							delegate: SkeletonBlock {
								width: parent.width * (0.3 + 0.1 * (row.rowIndex % 3))
								height: Style.fontSizeM
							}
						}

						Repeater {
							model: skeletonTable.cardRows ? 1 : 0

							delegate: SkeletonBlock {
								width: parent.width * (0.55 + 0.1 * (row.rowIndex % 3))
								height: Style.iconSizeXS
							}
						}
					}

					Rectangle {
						anchors.bottom: parent.bottom
						width: parent.width
						height: 1
						color: Style.borderColor
						visible: !skeletonTable.cardRows
					}
				}
			}
		}
	}
}
