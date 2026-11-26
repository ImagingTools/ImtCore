# TextEdit с RichText и Markdown в JQML v5

`TextEdit` хранит текст как документ Qt (`QTextDocument`) во всех форматах и работает с ним так же, как `QQuickTextEdit` в Qt 6.12:

- `RichText` (или `AutoText`, когда текст похож на HTML по правилу `Qt::mightBeRichText`) — `text` это HTML документа;
- `MarkdownText` — `text` это Markdown документа;
- `PlainText` — `text` это обычный текст документа, показывает его нативный `textarea`.

Код — `Tools/JQML/v5/Runtime/QtQuick/TextDocument/`, подключён в `Runtime/QtQuick/TextEdit.js`; типы `TextSelection`
(`Runtime/QtQuick/TextSelection.js`) и `TextDocument` (`Runtime/QtQuick/TextDocumentType.js`).

## Из чего состоит

| Файл | Что повторяет в Qt |
|---|---|
| `Css.js` | `qcssparser.cpp`: объявления `style`, таблицы стилей `<style>`, селекторы, значения (цвета, длины, шрифт, поля, рамки, фон) |
| `HtmlParser.js`, `Entities.js` | `QTextHtmlParser`: узлы HTML, исправление неправильной разметки, свойства элементов, атрибуты, сущности |
| `HtmlImporter.js` | `QTextHtmlImporter`: узлы в документ (блоки, пробелы, списки, таблицы, картинки, `<hr>`, якоря) |
| `MarkdownParser.js` | md4c (парсер Markdown внутри Qt): CommonMark и расширения GitHub, которые включает Qt (таблицы, зачёркивание, списки задач, автоссылки, подчёркивание `_`), события блоков, спанов и текста |
| `Markdown.js` | `QTextMarkdownImporter` (события md4c в документ) и `QTextMarkdownWriter` (`toMarkdown()`, перенос на 80 колонках, экранирование), front matter |
| `Document.js` | `QTextDocumentPrivate`: piece table (фрагменты с форматами, блоки с разделителями), списки, таблицы, стек undo (`QTextUndoCommand`, слияние шагов, блоки правок) |
| `Cursor.js` | `QTextCursor`: вставка, удаление, формат символа у курсора, блоки, таблицы, выделение через ячейки, все `MoveOperation` |
| `Fragment.js` | `QTextDocumentFragment`, `QTextCopyHelper`: копия части документа, вставка фрагмента и HTML |
| `HtmlExporter.js` | `QTextHtmlExporter`: `toHtml()` документа и фрагмента |
| `Formats.js` | `QTextCharFormat`, `QTextBlockFormat`, `QTextListFormat` |
| `RichText.js` | `QQuickTextEdit` / `QQuickTextControl`: правки методов и клавиш |
| `Layout.js` | `QTextDocumentLayout`: строки, поля между блоками, отступы, списки и флажки задач, таблицы, выравнивание, высота строк |
| `View.js` | отрисовка раскладки в DOM: текст, фон, выделение, маркеры списков, флажки задач, таблицы, картинки, `<hr>` |

## Как совпадает с Qt

- `text` — HTML (`toHtml()`) или Markdown (`toMarkdown()`), который выдаёт Qt, вплоть до символа; до `componentComplete` — заданная
  строка, как в Qt.
- `getText`, `length`, `selectedText` — позиции документа: U+2029 между блоками, U+2028 для `<br>`, U+FFFC для картинки,
  U+FDD0 / U+FDD1 для ячеек и конца таблицы; пробелы схлопываются по правилам Qt.
- `getFormattedText` — фрагмент HTML (`<!--StartFragment-->`…`<!--EndFragment-->`), в `MarkdownText` — Markdown фрагмента.
- `insert` вставляет HTML (`insertHtml`) или Markdown (`insertMarkdown`), `remove`, `append`, `clear`; курсор сдвигается как у Qt.
- Набор текста берёт формат символа перед курсором (после `text = …` — формат, который был у курсора); `Return` — новый блок с тем же
  форматом, `Shift+Return` — U+2028; `Backspace` в начале пункта списка выводит его из списка, в начале блока с отступом убирает
  один отступ; маркеры ячеек таблицы не удаляются; выделение через несколько ячеек очищает прямоугольник ячеек.
- `undo` / `redo` — стек документа во всех форматах, и в `PlainText` тоже: шаги сливаются как в `QTextDocumentPrivate::appendUndoItem`
  (набранное слово — один шаг, удаление разделителя блоков — отдельный), курсор встаёт на место изменения; `setText` и загрузка
  файла очищают историю.
- `textFormat` — преобразования `QQuickTextEdit::setTextFormat`: rich или Markdown → plain показывает HTML или Markdown как текст,
  plain → rich (Markdown) читает текст как HTML (Markdown), rich → Markdown даёт Markdown документа, Markdown → rich — его HTML;
  `AutoText` сохраняет формат текста (или файла `textDocument`) и распознаёт HTML.
- Markdown: заголовки (размер `4 - уровень`, жирный), `*`/`**`, `_` — подчёркивание, `~~`, код в моноширинном шрифте, блоки кода
  (язык и ограда сохраняются), цитаты (отступ 40 на уровень), правило, ссылки (цвет ссылки, подсказка из заголовка), автоссылки
  `www.` и e-mail, картинки, списки `-` (диск), `*` (круг), `+` (квадрат), нумерованные с началом и `)`, списки задач (флажок
  вместо маркера), таблицы с выравниванием, объединение пустых ячеек как в Qt, сущности и HTML внутри Markdown, front matter
  (`---`), поля абзацев 2/3 размера шрифта в пунктах (0 при `pixelSize`).
- `cursorSelection` и `TextSelection` (`QQuickTextSelection`): `text`, `font`, `color`, `alignment` выделения читаются и пишутся
  (запись без выделения берёт слово под курсором; `font.bold = true` и т. п. записывают шрифт целиком), `selectionStart` /
  `selectionEnd`, `moveSelectionStart` / `moveSelectionEnd` по операциям `QTextCursor`, `duplicate`, `linkTo`; свой `TextSelection`
  внутри `TextEdit` работает со своим курсором по тому же документу.
- `textDocument` (`QQuickTextDocument`): `source` загружает файл (`.html`/`.htm` как HTML, `.md`/`.markdown` как Markdown, иначе как
  текст; в `PlainText` — всегда как текст), `status`, `errorString`, `modified` (после загрузки `false`, после правки `true`).
- `selectByKeyboard` (по умолчанию `true`, у `readOnly` — `false`), `renderType`, `inputMethodQuery`.
- `lineCount`, `contentWidth` / `contentHeight`, `paintedWidth` / `paintedHeight`, `implicitWidth` / `implicitHeight`,
  `cursorRectangle`, `positionAt`, `positionToRectangle`, `linkAt`, `hoveredLink`, `linkHovered`, `linkActivated` — по раскладке
  документа; клавиши `Up`, `Down`, `Home`, `End`, `PageUp`, `PageDown` двигают курсор по строкам раскладки.

## Проверка

- Diff с Qt 6.12 (`qmltestrunner`), вывод совпадает построчно: `tests/diff/cases/textedit_richtext.qml`, `textedit_markdown.qml`,
  `textedit_selection.qml`, `textedit_undo.qml`. Запуск: `BENCH_CORE=sources node tests/diff/run.js tests/diff/cases/textedit_markdown.qml`.
- `tests/unit/RichText.test.js` — HTML, текст, фрагменты и правки сверены с выдачей Qt 6.12 (31 конструкция HTML с таблицами,
  12 последовательностей правок), клавиши, undo, `textFormat`, раскладка.
- `tests/unit/Markdown.test.js` — 80 текстов Markdown (`toMarkdown`, текст документа, HTML после перехода в `RichText`) и 17 HTML,
  записанных как Markdown, сверены с выдачей Qt 6.12; события парсера, форматы, `insertMarkdown`, front matter, поля абзацев.
- `tests/web/RichTextWeb.test.js`, `TextEditPlainWeb.test.js`, `MarkdownWeb.test.js` — в Chromium через Playwright: отрисовка
  (шрифты, цвета, фон, ссылки, маркеры списков и флажки задач, таблицы, картинки, курсор-блок терминала), совпадение раскладки с
  нарисованным текстом, мышь, клавиши, undo/redo, буфер обмена, ссылки, метод ввода (IME), `readOnly`, `padding`, `textFormat`,
  `cursorSelection` и свой `TextSelection`, `textDocument.source` с файлом `.md` и `modified`, скорость `append`.
  Запуск: `npm run test:web`; Playwright — пакет `playwright` или путь к нему в `JQML_PLAYWRIGHT` (`node_modules` v5 лежит в git,
  поэтому Playwright туда не ставится); без него тесты пропускаются.

## Чем отличается

- Размеры и положения считаются по метрикам шрифтов браузера, поэтому координаты (`cursorRectangle`, `contentWidth`, позиции
  строк) близки к Qt, но не совпадают до пикселя; с нарисованным в браузере текстом они совпадают (это проверяет web-тест).
- Картинки с `float` (`align="left"` / `"right"`) стоят в строке, а не обтекаются текстом; рамки `-qt-table-type: frame` раскладываются
  как их содержимое.
- Ширины колонок таблицы — по содержимому и заданным ширинам, без всех правил `QTextDocumentLayout::layoutTable`.
- Ввод с клавиатуры идёт через скрытый `textarea` (нужен для методов ввода); вид документа и курсор рисует `View.js`.
- Моноширинный шрифт для Markdown — `monospace` браузера. Qt определяет моноширинность по реальному шрифту, поэтому `<code>` из HTML
  при записи в Markdown даёт обратные кавычки, как в Qt на настоящей платформе (в Qt на платформе `offscreen` их нет: её
  «monospace» не моноширинный шрифт).
- `textDocument.save()` / `saveAs()` не пишут файлы: страница не может, статус `WriteError` (для не локального пути — `NonLocalFileError`).
- Перетаскивание текста мышью не поддерживается.
