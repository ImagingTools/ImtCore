import QtQuick 2.0

/*!
	\qmltype ComponentGraphView
	\inqmlmodule imtacf
	\brief Интерактивная визуализация графа компонентов AcfRegistry.

	Отображает все зарегистрированные компоненты в виде узлов и рёбер.
	Обновляется автоматически при изменении реестра.

	\qml
	ComponentGraphView {
		anchors.fill: parent
	}
	\endqml
*/
Item {
	id: root

	property int nodeSpacingX: 200
	property int nodeSpacingY: 120
	property int nodeWidth: 170
	property int nodeHeight: 90

	property color backgroundColor: "#f5f5f5"
	property color activeNodeColor: "#e3f2fd"
	property color inactiveNodeColor: "#fce4ec"
	property color nodeBorderColor: "#78909c"
	property color referenceEdgeColor: "#1565c0"
	property color factoryEdgeColor: "#2e7d32"
	property color nodeTextColor: "#263238"

	property var __graphData: ({nodes: [], edges: []})
	property var __nodePositions: ({})

	function refresh() {
		__graphData = AcfRegistry.exportGraph();
		__computeLayout();
		canvas.requestPaint();
	}

	function __computeLayout() {
		var nodes = __graphData.nodes || [];
		var positions = {};
		var columns = Math.max(1, Math.floor(Math.sqrt(nodes.length)));

		for (var i = 0; i < nodes.length; ++i) {
			var col = i % columns;
			var row = Math.floor(i / columns);
			positions[nodes[i].id] = {
				x: 30 + col * root.nodeSpacingX,
				y: 30 + row * root.nodeSpacingY
			};
		}

		__nodePositions = positions;
	}

	function __getNodeCenter(nodeId) {
		var pos = __nodePositions[nodeId];
		if (!pos) return {x: 0, y: 0};
		return {
			x: pos.x + root.nodeWidth / 2,
			y: pos.y + root.nodeHeight / 2
		};
	}

	Rectangle {
		anchors.fill: parent
		color: root.backgroundColor
	}

	Canvas {
		id: canvas
		anchors.fill: parent

		onPaint: {
			var ctx = getContext("2d");
			ctx.clearRect(0, 0, width, height);

			var nodes = root.__graphData.nodes || [];
			var edges = root.__graphData.edges || [];

			// Рёбра
			for (var e = 0; e < edges.length; ++e) {
				var edge = edges[e];
				var fromCenter = root.__getNodeCenter(edge.from);
				var toCenter = root.__getNodeCenter(edge.to);
				if (toCenter.x === 0 && toCenter.y === 0) continue;

				ctx.beginPath();
				ctx.strokeStyle = edge.type === "factory"
					? String(root.factoryEdgeColor)
					: String(root.referenceEdgeColor);
				ctx.lineWidth = edge.isResolved ? 2 : 1;

				if (!edge.isResolved) {
					ctx.setLineDash([5, 5]);
				} else {
					ctx.setLineDash([]);
				}

				ctx.moveTo(fromCenter.x, fromCenter.y);
				ctx.lineTo(toCenter.x, toCenter.y);
				ctx.stroke();

				// Стрелка
				var angle = Math.atan2(toCenter.y - fromCenter.y, toCenter.x - fromCenter.x);
				var arrowLen = 10;
				var ax = toCenter.x - Math.cos(angle) * (root.nodeWidth / 2 + 5);
				var ay = toCenter.y - Math.sin(angle) * (root.nodeHeight / 2 + 5);

				ctx.beginPath();
				ctx.setLineDash([]);
				ctx.moveTo(ax, ay);
				ctx.lineTo(ax - arrowLen * Math.cos(angle - 0.3), ay - arrowLen * Math.sin(angle - 0.3));
				ctx.moveTo(ax, ay);
				ctx.lineTo(ax - arrowLen * Math.cos(angle + 0.3), ay - arrowLen * Math.sin(angle + 0.3));
				ctx.stroke();

				// Подпись ребра
				var midX = (fromCenter.x + toCenter.x) / 2;
				var midY = (fromCenter.y + toCenter.y) / 2;
				ctx.font = "9px sans-serif";
				ctx.fillStyle = ctx.strokeStyle;
				var label = edge.refId || edge.factoryId || "";
				ctx.fillText(label, midX + 3, midY - 3);
			}

			// Узлы
			for (var n = 0; n < nodes.length; ++n) {
				var node = nodes[n];
				var pos = root.__nodePositions[node.id];
				if (!pos) continue;

				var x = pos.x;
				var y = pos.y;
				var w = root.nodeWidth;
				var h = root.nodeHeight;
				var r = 6;

				ctx.fillStyle = node.isActive ? String(root.activeNodeColor) : String(root.inactiveNodeColor);
				ctx.strokeStyle = String(root.nodeBorderColor);
				ctx.lineWidth = 1.5;
				ctx.setLineDash([]);

				// Скруглённый прямоугольник
				ctx.beginPath();
				ctx.moveTo(x + r, y);
				ctx.lineTo(x + w - r, y);
				ctx.quadraticCurveTo(x + w, y, x + w, y + r);
				ctx.lineTo(x + w, y + h - r);
				ctx.quadraticCurveTo(x + w, y + h, x + w - r, y + h);
				ctx.lineTo(x + r, y + h);
				ctx.quadraticCurveTo(x, y + h, x, y + h - r);
				ctx.lineTo(x, y + r);
				ctx.quadraticCurveTo(x, y, x + r, y);
				ctx.closePath();
				ctx.fill();
				ctx.stroke();

				// Заголовок
				ctx.font = "bold 11px sans-serif";
				ctx.fillStyle = String(root.nodeTextColor);
				ctx.fillText(node.id, x + 8, y + 18);

				// Package
				ctx.font = "9px sans-serif";
				ctx.fillStyle = "#546e7a";
				if (node.packageId) ctx.fillText(node.packageId, x + 8, y + 32);

				// Интерфейсы
				var ifaces = node.interfaces || [];
				if (ifaces.length > 0) {
					ctx.font = "italic 9px sans-serif";
					ctx.fillStyle = "#1565c0";
					var ifaceText = ifaces.join(", ");
					if (ifaceText.length > 25) ifaceText = ifaceText.substring(0, 22) + "...";
					ctx.fillText(ifaceText, x + 8, y + 46);
				}

				// Статус
				ctx.font = "8px sans-serif";
				ctx.fillStyle = node.isActive ? "#2e7d32" : "#c62828";
				ctx.fillText(node.isActive ? "● active" : "○ inactive", x + 8, y + h - 8);
			}
		}
	}

	Connections {
		target: AcfRegistry

		function onComponentRegistered() { root.refresh(); }
		function onComponentUnregistered() { root.refresh(); }
		function onReferencesResolved() { root.refresh(); }
	}

	Component.onCompleted: {
		root.refresh();
	}
}
