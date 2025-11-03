import QtQuick 2.0
import Acf 1.0
import imtcontrols 1.0

Item {
	id: pieChart
	width: Style.sizeHintL
	height: Style.sizeHintM

	property var segments: [] // [{ value: 40, color: "#4CAF50", label: "Active" }, ...]
	property bool ring: true
	property real ringThickness: 0.6
	property bool clockwise: true
	property real rotationAngle: -90
	property bool showLegend: true
	property bool showPercent: true
	property bool legendHorizontal: false

	signal chartUpdated()

	function getTotalValue() {
		let total = 0
		for (let i = 0; i < pieChart.segments.length; ++i)
			total += pieChart.segments[i].value || 0
		return total
	}

	function getPercentText(value) {
		let total = pieChart.getTotalValue()
		if (total <= 0 || pieChart.segments.length === 0)
			return ""
		let percent = Math.round(value / total * 100)
		return percent + "%"
	}

	function updateGui() {
		let totalValue = pieChart.getTotalValue()
		if (totalValue <= 0 || pieChart.segments.length === 0)
			return

		pieChart.segments.sort(function(a, b){
			if (a.value > b.value){
				return -1
			}
			if (a.value < b.value){
				return 1
			}
			return 0
		}
		)

		for (let i = 0; i < pieChart.segments.length; ++i) {
			let seg = pieChart.segments[i]
			let percentText = pieChart.getPercentText(seg.value)
			seg.displayText = seg.value + (percentText ? " (" + percentText + ")" : "")
		}
		chartUpdated()
	}

	onSegmentsChanged: { updateGui(); canvas.requestPaint() }
	onWidthChanged: canvas.requestPaint()
	onHeightChanged: canvas.requestPaint()
	onRingChanged: canvas.requestPaint()
	onClockwiseChanged: canvas.requestPaint()
	onRotationAngleChanged: canvas.requestPaint()

	Canvas {
		id: canvas
		anchors.horizontalCenter: parent.horizontalCenter
		width: pieChart.height * 0.6
		height: width
		antialiasing: true

		onPaint: {
			let ctx = getContext("2d")
			ctx.reset()

			let cx = width / 2
			let cy = height / 2
			let r = width / 2

			let total = pieChart.getTotalValue()
			if (!total)
				return

			let angle = pieChart.rotationAngle * Math.PI / 180
			let direction = pieChart.clockwise ? 1 : -1

			for (let i = 0; i < pieChart.segments.length; ++i) {
				let seg = pieChart.segments[i]
				let portion = (seg.value / total) * 2 * Math.PI * direction
				let nextAngle = angle + portion

				ctx.beginPath()
				ctx.moveTo(cx, cy)
				ctx.arc(cx, cy, r, angle, nextAngle, !pieChart.clockwise)
				ctx.closePath()
				ctx.fillStyle = seg.color || "#ccc"
				ctx.fill()

				angle = nextAngle
			}

			if (pieChart.ring) {
				let innerR = r * pieChart.ringThickness
				ctx.beginPath()
				ctx.arc(cx, cy, innerR, 0, 2 * Math.PI)
				ctx.fillStyle = "white"
				ctx.fill()
			}
		}
	}

	Column {
		id: legend
		anchors.top: canvas.bottom
		anchors.topMargin: Style.marginM
		anchors.horizontalCenter: parent.horizontalCenter
		width: parent.width * 0.9
		visible: pieChart.showLegend

		Repeater {
			model: pieChart.segments
			delegate: Row {
				width: legend.width
				height: 20
				spacing: Style.marginM
			
				Rectangle {
					width: Style.fontSizeM
					height: 14
					radius: 3
					color: modelData.color || "#ccc"
					anchors.verticalCenter: parent.verticalCenter
				}

				Item {
					width: parent.width - Style.fontSizeM - 2 * Style.marginM
					height: parent.height
			
					Row {
						anchors.fill: parent
						spacing: Style.marginS
			
						Text {
							id: labelText
							text: modelData.label || ""
							color: Style.textColor
							font.pixelSize: Style.fontSizeM
							elide: Text.ElideRight
							horizontalAlignment: Text.AlignLeft
							width: parent.width * 0.6
						}
			
						Text {
							id: valueText
							text: modelData.displayText || ""
							color: Style.textColor
							font.pixelSize: Style.fontSizeM
							visible: pieChart.showPercent
							horizontalAlignment: Text.AlignLeft
							width: parent.width * 0.4
						}
					}
				}
			}
		}
	}
}


