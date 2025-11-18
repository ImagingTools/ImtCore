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
				let isHovered = (ma.highlightedSegment === seg)
				ctx.fillStyle = isHovered
						? Functions.darkenColor(seg.color, 1.4)
						: seg.color || "#ccc"
				
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

	Rectangle {
		id: tooltip
		visible: false
		z: 9999
		color: Style.baseColor
		radius: Style.radiusS
		border.width: Style.buttonBorderWidth
		border.color: Style.borderColor
	
		Text {
			id: tooltipText
			anchors.centerIn: parent
			color: Style.textColor
			font.pixelSize: Style.fontSizeM
			font.family: Style.fontFamily
		}
	
		width: tooltipText.width + 2*Style.marginS
		height: tooltipText.height + 2*Style.marginXS
	}
	
	MouseArea {
		id: ma
		anchors.fill: parent
		hoverEnabled: true
		preventStealing: true
	
		property var highlightedSegment: null
	
		onPositionChanged: {
			let mx = mouse.x
			let my = mouse.y
	
			let cx = canvas.width / 2
			let cy = canvas.height / 2
			let dx = mx - cx
			let dy = my - cy
			let dist = Math.sqrt(dx*dx + dy*dy)
	
			let r = Math.min(canvas.width, canvas.height) / 2
			let innerR = pieChart.ring ? r * pieChart.ringThickness : 0

			if (dist > r || dist < innerR) {
				highlightedSegment = null
				tooltip.visible = false
				canvas.requestPaint()
				return
			}
	
			let angle = Math.atan2(dy, dx)
			if (angle < -Math.PI/2) angle += 2*Math.PI
	
			let start = pieChart.rotationAngle * Math.PI/180
			let dir = pieChart.clockwise ? 1 : -1
	
			let total = pieChart.getTotalValue()
			let cur = start
	
			let found = null
	
			for (let seg of pieChart.segments) {
				let portion = (seg.value / total) * 2*Math.PI * dir
				let next = cur + portion
	
				let a1 = cur
				let a2 = next
	
				let norm = angle
	
				if (dir === 1) {
					if (norm >= a1 && norm <= a2)
						found = seg
				} else {
					if (norm <= a1 && norm >= a2)
						found = seg
				}
	
				cur = next
			}
	
			if (highlightedSegment !== found) {
				highlightedSegment = found
				canvas.requestPaint()
			}
	
			if (found) {
				tooltipText.text = found.label + ": " + found.value
	
				let pad = 10
				let tx = mx + pad
				let ty = my + pad
	
				if (tx + tooltip.width > canvas.width)
					tx = canvas.width - tooltip.width
	
				if (ty + tooltip.height > canvas.height)
					ty = canvas.height - tooltip.height
	
				tooltip.x = tx
				tooltip.y = ty
				tooltip.visible = true
			} else {
				tooltip.visible = false
			}
		}
	
		onExited: {
			highlightedSegment = null
			tooltip.visible = false
			canvas.requestPaint()
		}
	}
	
}
