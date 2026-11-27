# Canvas и Context2D в JQML v5

`Canvas` и его контекст `"2d"` повторяют Qt 6.12 (`qquickcanvasitem.cpp`, `qquickcontext2d.cpp`). Код:

- `Tools/JQML/v5/Runtime/QtQuick/Canvas.js` — элемент;
- `Tools/JQML/v5/Runtime/QtQuick/Context2D.js` — контекст;
- `Tools/JQML/v5/Runtime/QtQuick/Context2DSvg.js` — разбор SVG‑пути для свойства `path`.

## Как устроено

Контекст — это не DOM‑контекст браузера с обёртками, а порт `QQuickJSContext2D` / `QQuickContext2D`. Он хранит собственное состояние Qt
(`QQuickContext2D::State`) и собственный путь (`QPainterPath`: дуги, эллипсы и скруглённые прямоугольники превращаются в кривые Безье
так же, как в Qt, через `qt_curves_for_arc`). DOM‑канва нужна только для отрисовки: перед каждой командой на неё переносится состояние
(матрица с учётом devicePixelRatio, кисть или перо, прозрачность, режим наложения, тень) — как это делает `QQuickContext2DCommandBuffer::replay`.

## Что совпадает с Qt

- **Возвращаемые значения и аргументы.** Методы возвращают то же, что Qt: контекст, `undefined` или `null`. Недостающие и нечисловые
  аргументы игнорируются, а не дают ошибку браузера. Исключения те же, с теми же кодом и текстом: отрицательный радиус, градиенты,
  `createImageData`, `getImageData`, `putImageData`, `drawImage`, `drawFocusRing`, `caretBlinkRate`, `setCaretSelectionRect`.
- **Чтение свойств.**
  - `fillStyle` / `strokeStyle`: `#rrggbb` или `rgba(r, g, b, a)`; градиент возвращается как объект; шаблон из цвета — своим цветом.
  - `shadowColor`: `#rrggbb` без альфы.
  - `font`: в виде `QFont::toString()`.
- **Проверка значений.**
  - `lineWidth`, `miterLimit`, `shadowBlur` принимают только положительные значения; `globalAlpha` — от 0 до 1.
  - `textBaseline` не знает `ideographic`.
  - `globalCompositeOperation` принимает имена Qt (`qt-multiply`, `qt-clear`, `qt-destination` и т. д.); `qt-lighten` читается как `lighter`.
  - Цвета разбираются как в `qt_color_from_string` (`rgb()`, `rgba()`, `hsl()`, `hsla()`, имена, `#rgb`, `#aarrggbb`).
  - Шрифт разбирается как в `qt_font_from_string`: обязательны размер в `px`/`pt` и семейство после него; порядок и повторы
    проверяются; при ошибке шрифт не меняется.
- **Пунктир.** `setLineDash` удваивает список нечётной длины и отвергает отрицательные значения. Штрихи и `lineDashOffset` измеряются
  в ширинах линии (как у `QPen`).
- **Матрица.**
  - `transform(a, b, c, d, e, f)` умножает после текущей матрицы (`state.matrix * transform`), как в Qt.
  - Вырожденная матрица выключает рисование, пока матрицу не зададут заново (`setTransform`, `resetTransform`).
  - Путь остаётся на месте при смене матрицы и при `save`/`restore`.
- **Путь.**
  - `lineTo` в ту же точку не добавляет отрезок.
  - `closePath` у пути-точки ничего не делает.
  - `rect`/`ellipse`/`roundedRect` нулевого размера — это перемещение.
  - `arc` и `arcTo` вычисляются по формулам Qt.
  - `isPointInPath` учитывает `fillRule` и границу пути.
  - `text()` добавляет текст в путь.
  - Свойство `path` принимает SVG‑строку или элемент `Path`.
- **Режимы наложения ограничены нарисованным, как у QPainter.** `copy`, `source-in`, `source-out`, `destination-in`,
  `destination-atop` в DOM действуют на всю канву; здесь они меняют только нарисованное.
- **Градиенты.** Градиент сохраняет те стопы, что были в момент присваивания стилю; стоп в той же позиции заменяется.
  `createConicalGradient` идёт против часовой стрелки.
- **Шаблоны.** `createPattern(цвет, Qt.Dense4Pattern …)` даёт шаблоны кистей Qt 8×8. Текстура — из `CanvasImageData` или URL картинки;
  повторяется всегда.
- **Изображения.**
  - `drawImage`: URL, `Image`, другой `Canvas` (в его логическом размере), `CanvasImageData`; 3, 5 или 9 аргументов, проверка
    исходного прямоугольника; рисуется с состоянием (матрица, прозрачность, тень).
  - `CanvasImageData.data` принимает только целые 0..255 (иначе запись игнорируется); за пределами массива — `undefined`.
- **`putImageData`.** Рисует только с семью аргументами и с состоянием, как Qt 6.12. «Грязный» прямоугольник применяется дважды —
  это поведение Qt 6.12, оно повторено.
- **`painted`.** Испускается, когда команды ушли на канву: после `paint` (в том числе один раз после создания контекста — его `reset()`),
  а также сразу при `getImageData` и `toDataURL`. Если команд не было, сигнала нет.
- **HiDPI.** При любом devicePixelRatio координаты, `getImageData` и размеры другого Canvas — логические, то есть результат такой же,
  как у Qt при DPR 1. Сам Qt на HiDPI читает пиксели устройства по логическим координатам; это не повторяется.

## Проверка

- `tests/web/CanvasWeb.test.js` — три страницы из `tests/web/canvas/*.qml` выполняются в Chromium при DPR 1 и 2, их вывод построчно
  сравнивается с выводом Qt 6.12 (`*.qt.txt`, снят `qml -platform offscreen`). Сравниваются около 180 строк: значения свойств,
  возвраты, исключения, пиксели заливок, обводок, матриц, градиентов, режимов наложения, клипов, теней, дуг, пунктира, шаблонов,
  `drawImage`, `putImageData`, текста, а также сигнал `painted`.
- `tests/unit/Canvas.test.js` — элемент (доступность, размеры, `paint`/`painted`, `requestAnimationFrame`, кэш картинок, `toDataURL`)
  и контекст: состояние и исключения по эталону Qt, массив пикселей, команды DOM‑канве (матрица, пунктир, наложение, тень, градиенты,
  кривые пути, клипы после смены размера).

Обновить эталон: `qml -platform offscreen tests/web/canvas/CanvasState.qml` (Qt 6.12), строки `qml: ` без `painted` и предупреждений
сохранить в `CanvasState.qt.txt`.

## Чем отличается

- **Растеризация браузерная:** края всегда сглажены (QPainter у Canvas по умолчанию без сглаживания), значения пикселей могут
  отличаться на 1–3. Размытие тени — браузерное.
- **Глифы и метрики шрифтов — браузера.** Обобщённые семейства (`serif`, `sans-serif`, `monospace`) остаются этими именами, а Qt
  подставляет семейство платформы (Times New Roman, Helvetica, Menlo). Наличие семейства (`QFontDatabase::hasFamily`) определяется
  замером ширины и по `document.fonts` (шрифты `FontLoader` учитываются).
- **Цвета QML в v5 восьмибитные:** `Qt.rgba(0, 0.5, 1, 0.25)` читается как `rgba(0, 128, 255, 0.25098)`, в Qt — `0.250004`.
- **Текст в пути** (`text()`) рисуется при `fill`/`stroke`, но не участвует в `clip` и `isPointInPath`.
- **Только API Qt.** Методов DOM, которых нет в Qt (`roundRect`, `createConicGradient`, `getTransform`, `isPointInStroke` и т. п.), у контекста нет.
- **Файлы:** `save()` возвращает `false`, `toDataURL` умеет `image/png` и `image/jpeg`.
