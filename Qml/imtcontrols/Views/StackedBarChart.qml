import QtQuick 2.15
import Acf 1.0
import imtcontrols 1.0

Item{
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
	property int ySteps: 1
	property string xLabel: "Date"
	property string yLabel: "Count"

	property var legendItems: []

	property var segmentsRects: []
	property var hoveredSegment: null

	onBarsChanged:{
		buildLegend()
		canvas.requestPaint()
	}

	function buildLegend(){
		let map ={}
		for (let bar of bars){
			for (let seg of bar.segments || []){
				if (!map[seg.label])
					map[seg.label] = seg.color || "#ccc"
			}
		}
		let result = []
		for (let key in map)
			result.push({ label: key, color: map[key] })
		legendItems = result
	}

	Rectangle{
		id: tooltip
		width: tooltipText.width + 2*Style.marginS
		height: tooltipText.height + 2*Style.marginXS
		color:  Style.baseColor
		radius: Style.radiusS
		visible: false
		z: 99999
		border.width: Style.buttonBorderWidth
		border.color: Style.borderColor

		Text{
			id: tooltipText
			anchors.centerIn: parent
			color: Style.textColor
			font.family: Style.fontFamily
			font.pixelSize: Style.fontSizeM
		}
	}

	Canvas{
		id: canvas
		anchors.top: parent.top
		anchors.left: parent.left
		anchors.right: parent.right
		anchors.bottom: legend.top
		anchors.bottomMargin: Style.marginM
		antialiasing: true

		onPaint:{
			let ctx = getContext("2d")
			ctx.reset()

			chart.segmentsRects = []

			if (!chart.bars || chart.bars.length === 0)
				return

			let totalBars = chart.bars.length
			let availableWidth = chart.width
			let offsetBottom = 35
			let offsetLeft = 45
			let availableHeight = canvas.height - offsetBottom - 15
			let baseY = canvas.height - offsetBottom
			let contentWidth = availableWidth - offsetLeft - 10

			let globalMax = 0
			for (let b of chart.bars){
				let total = 0
				for (let s of b.segments)
					total += s.value
				globalMax = Math.max(globalMax, total)
			}
			if (globalMax <= 0)
				globalMax = 1

			let stepValue = chart.ySteps || 1
			let axisMax = Math.ceil(globalMax / stepValue) * stepValue
			let ySteps = axisMax / stepValue

			let spacingRatio = 0.4
			let barWidth = contentWidth / (totalBars * (1 + spacingRatio) + spacingRatio)
			let spacing = barWidth * spacingRatio
			let offsetX = offsetLeft + spacing

			ctx.strokeStyle = chart.axisColor
			ctx.lineWidth = 1
			ctx.beginPath()
			ctx.moveTo(offsetLeft, baseY)
			ctx.lineTo(offsetLeft, baseY - availableHeight)
			ctx.moveTo(offsetLeft, baseY)
			ctx.lineTo(availableWidth - 10, baseY)
			ctx.stroke()

			ctx.font = chart.labelFontSize + "px sans-serif"
			ctx.fillStyle = chart.textColor
			ctx.textAlign = "right"
			ctx.textBaseline = "middle"
			ctx.strokeStyle = chart.gridColor
			ctx.lineWidth = 0.5

			for (let value = 0; value <= axisMax; value += stepValue){
				let y = baseY - (value / axisMax) * availableHeight

				ctx.beginPath()
				ctx.moveTo(offsetLeft, y)
				ctx.lineTo(availableWidth - 10, y)
				ctx.stroke()

				ctx.fillText(value.toString(), offsetLeft - 6, y)
			}

			for (let i = 0; i < totalBars; i++){
				let bar = chart.bars[i]
				let x = offsetX + i * (barWidth + spacing)
				let y = baseY

				for (let seg of bar.segments){
					let segHeight = (seg.value / axisMax) * availableHeight
					y -= segHeight

					if (chart.hoveredSegment === seg){
						ctx.fillStyle = Functions.darkenColor(seg.color,1.4)
					} else{
						ctx.fillStyle = seg.color || "#ccc"
					}

					ctx.fillRect(x, y, barWidth, segHeight)

					chart.segmentsRects.push({
						seg: seg,
						x: x,
						y: y,
						w: barWidth,
						h: segHeight
					})

					if (chart.showValues && seg.value > 0){
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

			ctx.save()
			ctx.translate(10, baseY - availableHeight / 2)
			ctx.rotate(-Math.PI / 2)
			ctx.textAlign = "center"
			ctx.textBaseline = "middle"
			ctx.font = (chart.labelFontSize + 2) + "px sans-serif"
			ctx.fillStyle = chart.textColor
			ctx.fillText(chart.yLabel, 0, -3)
			ctx.restore()

			ctx.font = (chart.labelFontSize + 2) + "px sans-serif"
			ctx.textAlign = "center"
			ctx.textBaseline = "top"
			ctx.fillStyle = chart.textColor
			let xLabelMargin = 25
			ctx.fillText(chart.xLabel, offsetLeft + contentWidth / 2, baseY + xLabelMargin)
		}
	}

	Flow{
		id: legend
		width: parent.width
		spacing: Style.spacingM
		anchors.bottom: parent.bottom
		anchors.horizontalCenter: parent.horizontalCenter
		visible: chart.showLegend

		Repeater{
			model: chart.legendItems

			delegate: Row{
				spacing: Style.marginM
				Rectangle{
					width: 12
					height: 12
					radius: 3
					color: modelData.color
					anchors.verticalCenter: parent.verticalCenter
				}
				Text{
					text: modelData.label
					color: chart.textColor
					font.pixelSize: chart.labelFontSize
					anchors.verticalCenter: parent.verticalCenter
				}
			}
		}
	}

	// === Обработка мыши ===
	MouseArea{
		anchors.fill: chart
		hoverEnabled: true
		preventStealing: true

		onPositionChanged:{
			let mX = mouse.x
			let mY = mouse.y

			let hovered = null

			for (let r of chart.segmentsRects){
				if (mX >= r.x && mX <= r.x + r.w &&
					mY >= r.y && mY <= r.y + r.h){
					hovered = r.seg
					break
				}
			}

			if (hovered !== chart.hoveredSegment){
				chart.hoveredSegment = hovered
				canvas.requestPaint()
			}

			if (hovered){
				tooltipText.text = hovered.label + ": " + hovered.value
			
				let padding = 10
				let x = mouse.x + padding
				let y = mouse.y + padding
			
				if (x + tooltip.width > canvas.width)
					x = canvas.width - tooltip.width
			
				if (y + tooltip.height > canvas.height)
					y = canvas.height - tooltip.height

				tooltip.x = x
				tooltip.y = y
				tooltip.visible = true
			}
			else{
				tooltip.visible = false
			}
		}

		onExited:{
			tooltip.visible = false
			chart.hoveredSegment = null
			canvas.requestPaint()
		}
	}

	onWidthChanged: canvas.requestPaint()
	onHeightChanged: canvas.requestPaint()
}
