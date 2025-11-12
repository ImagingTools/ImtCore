import QtQuick 2.15
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
		for (let i = 0; i < segments.length; ++i)
			total += segments[i].value || 0
		return total
	}

	function getPercentText(value) {
		let total = getTotalValue()
		if (total <= 0 || segments.length === 0)
			return ""
		let percent = Math.round(value / total * 100)
		return percent + "%"
	}

	function updateGui() {
		let totalValue = getTotalValue()
		if (totalValue <= 0 || segments.length === 0)
			return

		segments.sort(function(a, b) { return b.value - a.value })

		for (let i = 0; i < segments.length; ++i) {
			let seg = segments[i]
			let percentText = getPercentText(seg.value)
			seg.displayText = seg.value + (percentText ? " (" + percentText + ")" : "")
		}
		chartUpdated()
	}

	onSegmentsChanged: { updateGui(); canvas.requestPaint() }

	Canvas {
		id: canvas
		anchors.top: parent.top
		anchors.left: parent.left
		anchors.right: parent.right
		anchors.bottom: legend.visible ? legend.top : parent.bottom
		anchors.bottomMargin: Style.marginM
		antialiasing: true

		onPaint: {
			let ctx = getContext("2d")
			ctx.reset()

			let total = pieChart.getTotalValue()
			if (total <= 0) return

			let cx = width / 2
			let cy = height / 2
			let r = Math.min(width, height) / 2

			let angle = pieChart.rotationAngle * Math.PI / 180
			let direction = pieChart.clockwise ? 1 : -1

			for (let seg of pieChart.segments) {
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
		anchors.bottom: parent.bottom
		anchors.horizontalCenter: parent.horizontalCenter
		width: parent.width * 0.9
		visible: pieChart.showLegend

		clip: true

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

				Text {
					text: modelData.label || ""
					color: Style.textColor
					font.pixelSize: Style.fontSizeM
					elide: Text.ElideRight
					width: legend.width * 0.4
					verticalAlignment: Text.AlignVCenter
				}

				Text {
					text: modelData.displayText || ""
					color: Style.textColor
					font.pixelSize: Style.fontSizeM
					visible: pieChart.showPercent
					elide: Text.ElideRight
					width: legend.width * 0.4
					verticalAlignment: Text.AlignVCenter
				}
			}
		}
	}
}
