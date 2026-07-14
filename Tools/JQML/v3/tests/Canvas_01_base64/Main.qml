import QtQuick

Item {
    id: root
    anchors.fill: parent
    // Наш источник изображения в формате Base64 (маленький красный квадрат для теста)
    readonly property string base64Image: "data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAAEAAAABACAYAAACqaXHeAAAAAXNSR0IArs4c6QAAAARnQU1BAACxjwv8YQUAAAAJcEhZcwAADsMAAA7DAcdvqGQAAAAZdEVYdFNvZnR3YXJlAFBhaW50Lk5FVCA1LjEuMTGKCBbOAAAAuGVYSWZJSSoACAAAAAUAGgEFAAEAAABKAAAAGwEFAAEAAABSAAAAKAEDAAEAAAACAAAAMQECABEAAABaAAAAaYcEAAEAAABsAAAAAAAAAGAAAAABAAAAYAAAAAEAAABQYWludC5ORVQgNS4xLjExAAADAACQBwAEAAAAMDIzMAGgAwABAAAAAQAAAAWgBAABAAAAlgAAAAAAAAACAAEAAgAEAAAAUjk4AAIABwAEAAAAMDEwMAAAAAAGNdRzso9yOwAAA6JJREFUeF7tm11uEzEQx9ebtEXi6wI0Sh9RyxmgR0Atz8BJKCchfSYVR2h6BlLxyCpwAaBFNNHazOzaIdpumvHGYzskP2llO+rH+u/xfHg3InHMl0/dbp7Lc+h2y0+ckbVa6eHTl1mmx05IdeuMPM8/QON68khXSvla953h1AL06n/VQxbACvZcWkFjAS7Pdt9gq5R4XnxQoGDlxQs9YEIN4H8UAgiRFC1YxkW73c6aCGMtAE4cJo1mHh1CqLdp2hrYCGElwLC/C87NfoXv7Yjk0X07d/PzWiZ/bpQe0QGrONk/Gr3Xw4WQ7+rzxycwcW7zXh6lkhOzPSmQBRAiXcrsxxNldUlpv/oG2KLvdHchpC3gw7u7hhotnOcBqwZJACnz6Pd+Feo9byxAt2sLyQnaJT+YqXFCC8WYFO0ffevp4VycC3BwPLLOLm0Y9juk+EgVYOMDdLu2bATQrXewtkDfUtYY4QgmANYW6FihPb8865Bzd9cEEUBXaxzHZtYEEQBWfuYUKSyhtgC5XufGuwCQyER1nBbCAqKqLL0KEJPzM3gVQKmkeLCRQrWAB6XYhsabAOZQFSe9s12eEm9thVfAmwCQ8BSrj5N+aHlEzglpCVyUw1jGmtU3Avy4ksnNuKhue1C+XhQf1iClyp69+l6cM6xkOayd312rXwg870pTweY4vQgAk6jN9Y1FVK+qc6SsZFPYBTChDydVnRhaw+MHt69tEGEGtskj7AKY0Gfj/OB3prRaKfk5XxNYBcAnShj69LAJPddvhFRhFSDP5XTvi4r5z+Pqt0wmk9IElJKnRYcR7i0wrfrGEO4w7FWvX9dS/0RJDkPzXNSEPk7YBDChz4CTwphfveY9BMY4rrussAmAyQuY8OG8C37klndHazDmj296FB1mSDvTRSZYBc8B8WUGjPsY+hDcEmgVQA/+Tq0F/LcPRmZXHyyE3fkZgguAk8b3gf75AzXw4fwMwQXASePLUMYZQrj0tvpINFvAwJn31xGbAF4nj0QlAHfeX0dMArDn/XVEI4DP0DdLJAL4DX2zBBMgTdNTfJkRMzallPe9bwgmAO53vDDshVp9JCYnGISNALpdW5yXw9wHGTb3QUmrnQsQCyt3HhCKjQC6vRM839PdlYF6zyQB8Dt5ursyUJMrkgC6SlslEcj3SvYBQvg/rGiKzb0SH1iVDPsd/OZYVC851ZAdHI/2dH8hVlEAanZIcri/EbIMalDeI5Uk+Qtnk9dJDrNxzgAAAABJRU5ErkJggg=="

    Canvas {
        id: myCanvas
        anchors.fill: parent

        // 1. Подаем сигнал Canvas загрузить изображение при старте
        Component.onCompleted: {
            myCanvas.loadImage(root.base64Image)
        }

        // 2. Обрабатываем событие завершения загрузки
        onImageLoaded: {
            // Запрашиваем перерисовку холста
            myCanvas.requestPaint()
            console.log('onImageLoaded')
            Qt.quit()
        }

        // 3. Рисуем изображение на холсте
        onPaint: {
            var ctx = getContext("2d")
            
            // Очищаем холст перед рисованием
            ctx.clearRect(0, 0, width, height)

            // Проверяем, готово ли изображение, чтобы избежать ошибок в консоли
            if (myCanvas.isImageLoaded(root.base64Image)) {
                // Рисуем изображение по центру холста
                var x = (width - 48) / 2
                var y = (height - 48) / 2
                ctx.drawImage(root.base64Image, x, y)
            } else {
                console.log("Изображение еще не загружено")
            }
        }
    }
}