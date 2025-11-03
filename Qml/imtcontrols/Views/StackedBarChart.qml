import QtQuick 2.15
import Acf 1.0

Item {
	id: chart

	property var bars: [] // [{ label: "29 Oct", segments: [{label:"Trial", value:10, color:"#4CAF50"}, ...]}]
	property color axisColor: "#999"
	property color gridColor: "#ddd"
	property color textColor: Style.textColor
	property real barSpacing: Style.spacingM
	property real barWidth: 28
	property real labelFontSize: Style.fontSizeXS
	property bool showValues: false
	property bool showLegend: true
	property bool legendHorizontal: true
	property int ySteps: 1 // кол-во делений по оси Y
	property string xLabel: "Date"
	property string yLabel: "Count"

	property var legendItems: []

	onBarsChanged: {
		buildLegend()
		canvas.requestPaint()
	}

	function buildLegend() {
		let map = {}
		for (let bar of bars) {
			for (let seg of bar.segments || []) {
				if (!map[seg.label])
					map[seg.label] = seg.color || "#ccc"
			}
		}
		let result = []
		for (let key in map)
			result.push({ label: key, color: map[key] })
		legendItems = result
	}

	// === Диаграмма ===
	Canvas {
		id: canvas
		anchors.top: parent.top
		anchors.left: parent.left
		anchors.right: parent.right
		anchors.bottom: legend.top
		anchors.bottomMargin: Style.marginM
		antialiasing: true

		onPaint: {
			let ctx = getContext("2d")
			ctx.reset()
		
			if (!chart.bars || chart.bars.length === 0)
				return
		
			let totalBars = chart.bars.length
			let availableWidth = chart.width
			let offsetBottom = 35
			let offsetLeft = 35
			let availableHeight = canvas.height - offsetBottom - 15
			let baseY = canvas.height - offsetBottom
			let contentWidth = availableWidth - offsetLeft - 10
		
			// === вычисляем максимум ===
			let globalMax = 0
			for (let b of chart.bars) {
				let total = 0
				for (let s of b.segments)
					total += s.value
				globalMax = Math.max(globalMax, total)
			}
			if (globalMax <= 0)
				globalMax = 1
		
			// === шаг ===
			let stepValue = chart.ySteps || 1
			let axisMax = Math.ceil(globalMax / stepValue) * stepValue
			let ySteps = axisMax / stepValue
			let stepHeight = availableHeight / ySteps
		
			// === ширина баров ===
			let spacingRatio = 0.4
			let barWidth = contentWidth / (totalBars * (1 + spacingRatio) + spacingRatio)
			let spacing = barWidth * spacingRatio
			let offsetX = offsetLeft + spacing
		
			// === оси ===
			ctx.strokeStyle = chart.axisColor
			ctx.lineWidth = 1
			ctx.beginPath()
			ctx.moveTo(offsetLeft, baseY)
			ctx.lineTo(offsetLeft, baseY - availableHeight)
			ctx.moveTo(offsetLeft, baseY)
			ctx.lineTo(availableWidth - 10, baseY)
			ctx.stroke()
		
			// === деления по Y ===
			ctx.font = chart.labelFontSize + "px sans-serif"
			ctx.fillStyle = chart.textColor
			ctx.textAlign = "right"
			ctx.textBaseline = "middle"
		
			ctx.strokeStyle = chart.gridColor
			ctx.lineWidth = 0.5
		
			for (let value = 0; value <= axisMax; value += stepValue) {
				let y = baseY - (value / axisMax) * availableHeight
				ctx.beginPath()
				ctx.moveTo(offsetLeft, y)
				ctx.lineTo(availableWidth - 10, y)
				ctx.stroke()
				ctx.fillText(value.toString(), offsetLeft - 6, y)
			}
		
			// === бары ===
			for (let i = 0; i < totalBars; i++) {
				let bar = chart.bars[i]
				let x = offsetX + i * (barWidth + spacing)
				let y = baseY
		
				for (let seg of bar.segments) {
					let segHeight = (seg.value / axisMax) * availableHeight
					y -= segHeight
					ctx.fillStyle = seg.color || "#ccc"
					ctx.fillRect(x, y, barWidth, segHeight)
		
					if (chart.showValues && seg.value > 0) {
						ctx.fillStyle = chart.textColor
						ctx.font = chart.labelFontSize + "px sans-serif"
						ctx.textAlign = "center"
						ctx.textBaseline = "middle"
						ctx.fillText(seg.value, x + barWidth / 2, y + segHeight / 2)
					}
				}
		
				ctx.fillStyle = chart.textColor
				ctx.font = chart.labelFontSize + "px sans-serif"
				ctx.textAlign = "center"
				ctx.textBaseline = "top"
				ctx.fillText(bar.label, x + barWidth / 2, baseY + 6)
			}
		
			// === подпись оси Y ===
			ctx.save()
			ctx.translate(10, baseY - availableHeight / 2)
			ctx.rotate(-Math.PI / 2)
			ctx.textAlign = "center"
			ctx.textBaseline = "middle"
			ctx.font = "bold " + (chart.labelFontSize + 2) + "px sans-serif"
			ctx.fillStyle = chart.textColor
			ctx.fillText(chart.yLabel || "Count", 0, 0)
			ctx.restore()
		
			// === подпись оси X ===
			ctx.font = "bold " + (chart.labelFontSize + 2) + "px sans-serif"
			ctx.textAlign = "center"
			ctx.textBaseline = "top"
			ctx.fillStyle = chart.textColor
			
			let xLabelMargin = 25  // расстояние между осью X и подписью
			ctx.fillText(chart.xLabel, offsetLeft + contentWidth / 2, baseY + xLabelMargin)
		}

	}

	// === Легенда ===
	Flow {
		id: legend
		width: parent.width
		spacing: Style.spacingM
		anchors.bottom: parent.bottom
		anchors.horizontalCenter: parent.horizontalCenter
		visible: chart.showLegend

		Repeater {
			model: chart.legendItems

			delegate: Row {
				spacing: Style.marginM
				Rectangle {
					width: 12
					height: 12
					radius: 3
					color: modelData.color
					anchors.verticalCenter: parent.verticalCenter
				}
				Text {
					text: modelData.label
					color: chart.textColor
					font.pixelSize: chart.labelFontSize
					anchors.verticalCenter: parent.verticalCenter
				}
			}
		}
	}

	onWidthChanged: canvas.requestPaint()
	onHeightChanged: canvas.requestPaint()
}
