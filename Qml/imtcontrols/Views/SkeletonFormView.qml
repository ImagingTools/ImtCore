import QtQuick 2.12
import Acf 1.0

/**
	Skeleton of an editor: an optional page list on the left and a centered form
	of groups with "label - field" rows. Covers and blocks the view below it.
*/
Rectangle {
	id: skeletonForm

	property bool sidePanelVisible: false
	property int sidePanelWidth: Style.sizeHintXXS
	property int sideItemCount: 4
	property int groupCount: 1
	property int rowCount: 5
	property int contentMaxWidth: 900
	property int phase: 0

	color: Style.baseColor

	MouseArea {
		anchors.fill: parent
		hoverEnabled: true
		acceptedButtons: Qt.AllButtons
	}

	Timer {
		interval: 80
		repeat: true
		running: skeletonForm.visible

		onTriggered: {
			skeletonForm.phase = (skeletonForm.phase + 1) % 16
		}
	}

	Item {
		id: content

		anchors.fill: parent
		opacity: 0.45 + 0.55 * (skeletonForm.phase < 8 ? skeletonForm.phase / 8 : (16 - skeletonForm.phase) / 8)

		Column {
			id: sidePanel

			x: Style.marginL
			y: Style.marginXL
			width: skeletonForm.sidePanelWidth - 2 * Style.marginL
			spacing: Style.marginL
			visible: skeletonForm.sidePanelVisible

			Repeater {
				model: skeletonForm.sideItemCount

				delegate: Row {
					spacing: Style.spacingS

					SkeletonBlock {
						width: Style.iconSizeM
						height: Style.iconSizeM
					}

					SkeletonBlock {
						anchors.verticalCenter: parent.verticalCenter
						width: sidePanel.width * (index % 2 === 0 ? 0.55 : 0.4)
						height: Style.iconSizeXS
					}
				}
			}
		}

		Rectangle {
			id: sideDivider

			x: skeletonForm.sidePanelWidth
			width: 1
			height: parent.height
			color: Style.borderColor
			visible: skeletonForm.sidePanelVisible
		}

		Item {
			id: formArea

			anchors.left: skeletonForm.sidePanelVisible ? sideDivider.right : parent.left
			anchors.right: parent.right
			anchors.top: parent.top
			anchors.bottom: parent.bottom
			anchors.margins: Style.marginXL
			clip: true

			Column {
				id: form

				x: Math.max(0, (formArea.width - width) / 2)
				width: Math.min(skeletonForm.contentMaxWidth, formArea.width)
				spacing: Style.marginXL

				Repeater {
					model: skeletonForm.groupCount

					delegate: Column {
						width: form.width
						spacing: Style.marginM

						SkeletonBlock {
							width: form.width * 0.25
							height: Style.fontSizeM
						}

						Rectangle {
							width: form.width
							height: rows.height + 2 * Style.marginL
							radius: Style.radiusM
							color: "transparent"
							border.width: 1
							border.color: Style.borderColor

							Column {
								id: rows

								x: Style.marginL
								y: Style.marginL
								width: parent.width - 2 * Style.marginL
								spacing: Style.marginL

								Repeater {
									model: skeletonForm.rowCount

									delegate: Item {
										width: rows.width
										height: Style.controlHeightM

										SkeletonBlock {
											anchors.verticalCenter: parent.verticalCenter
											width: rows.width * (index % 3 === 0 ? 0.22 : (index % 3 === 1 ? 0.3 : 0.18))
											height: Style.iconSizeXS
										}

										SkeletonBlock {
											anchors.right: parent.right
											anchors.verticalCenter: parent.verticalCenter
											width: rows.width * 0.6
											height: parent.height
											radius: Style.radiusM
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}
}
