# Разметка в Text (RichText, StyledText, AutoText)

`src/richtext.js` разбирает текст `Text` с `textFormat: Text.RichText` и `Text.StyledText` и раскладывает его на строки из кусков с разными
шрифтами. Правила взяты у Qt: размеры, поля, отступы и переносы измерены настоящим Qt и закреплены тестами
`test/desktop/RichText_*` (результат Qt и движка должен совпадать до пикселя) и `test/rich-text.mjs`.

## Что понимается

| | RichText | StyledText |
|---|---|---|
| `<b> <strong> <i> <em> <u> <s> <strike> <del>`, `<font color size face>`, `<a href>`, `<br>` | да | да (без `face`) |
| `<big> <small> <sub> <sup> <tt> <code> <span style="…"> <nobr>` | да | нет |
| `<p> <div> <center> <h1>–<h6> <pre> <blockquote> <ul> <ol> <li> <dl> <hr>` | блоки с полями и отступами | `<p>` и `<h1>`–`<h6>` — только перевод строки |
| CSS в `style` | `color`, `background-color`, `font-size` (px, pt, small…xx-large), `font-weight`, `font-style`, `font-family`, `text-decoration`, `white-space`, `vertical-align`, `margin-*`, `text-indent`, `text-align`, `line-height`, `-qt-list-indent` | нет |
| сущности | `&lt; &gt; &amp; &quot; &nbsp; &#65; &#x41;` и часто встречающиеся имена | то же |
| `<img width height>` | место занимает, картинка не рисуется | нет |

`Text.AutoText` считает текст разметкой (как StyledText), если в нём есть тег (`Qt::mightBeRichText`). Остальные значения — обычный текст.

## Размеры и поля (как у Qt)

- `<font size=N>`: размеры HTML 1…7 = 0,7; 0,8; 1; 1,2; 1,5; 2; 2,4 от размера шрифта текста; `+N` и `-N` — от текущего. Заголовки
  `<h1>`…`<h6>` — размеры 6, 5, 4, 3, 2, 3 и жирный шрифт.
- Поля блоков: у `<p>` по 12 px, у `<h1>` 18 сверху, `<h2>` 16, `<h3>` 14, `<h4>`/`<h5>` 12, снизу по 12 (у `<h6>` нет); у `<pre>`, `<blockquote>` и
  списка 12. Между блоками остаётся большее из двух полей, верхнее поле первого блока и нижнее поле последнего отбрасываются.
- Список сдвигает текст на 40 px, `<blockquote>` на 40 px с каждой стороны. Вложенный список глубже второго уровня полей не имеет.
- `<br>` в конце блока оставляет пустую строку; `</div>` блок не заканчивает: текст после него идёт в той же строке (так делает Qt).
- Строка RichText высотой в межстрочный интервал шрифта, StyledText — в высоту шрифта; lineCount у RichText всегда 1 (как у Qt).

## Ссылки

`Text.linkColor` (по умолчанию `#0000ff`), `linkAt(x, y)`, `hoveredLink`, сигналы `linkHovered(link)` и `linkActivated(link)`. Текст со ссылкой
перехватывает указатель только над самой ссылкой (рука, `hoveredLink`, щелчок); остальное идёт элементам под ним.

## Чего нет

`MarkdownText` (показывается как обычный текст), таблицы `<table>` (ячейки идут одной строкой), рисование `<img>`, `elide` и
`maximumLineCount` для разметки, редактирование разметки в `TextEdit` (он показывает очищенный текст).
