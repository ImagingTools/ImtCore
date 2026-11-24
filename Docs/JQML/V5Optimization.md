# JQML v5: аудит, стресс-тесты, оптимизация

Ветка `feature/jqml-v5-audit` от `feature/jqml-v5-qt-compat-2` (PR #1159). Эталон поведения — Qt 6 (исходники
qtdeclarative 6.8, поведение проверено на Qt 6.12). Каждый принятый шаг — отдельный коммит с цифрами «до/после»;
исправления отделены от оптимизаций. v5 развивается в ImtCore (JQMLNext больше не источник); `dist/core.js` собирается
из Runtime/ (`npm run build`, сначала — вклейкой изменённых модулей), каждое изменение проверено на обоих runtime:
исходники Runtime/ (`tests/sourcesBundle.js`) и `dist/core.js`.

## 1. Сводка

Базовая линия — код до аудита (46444ad20a: runtime dc60986270), три прогона; итог — три прогона ветки; медианы.

| Метрика | Базовая линия | Итог | Δ |
|---|---|---|---|
| Сборка ProLife (parse + model + codegen + minify) | 18 217 мс | 9 634 мс | −47% |
| Сборка Lisa | 14 841 мс | 8 932 мс | −40% |
| Сборка Puma (на v5) | 14 521 мс | 8 858 мс | −39% |
| Кадр прокрутки ListView (200 кадров) | 93,9 мс | 15,7 мс | −83% |
| Прокрутка ListView около строки 80 000 (200 кадров) | 5 395 мс | 15,8 мс | −99,7% |
| 10 000 детей одного родителя: создание | 35,3 мс¹ | 14,7 мс | −58% |
| 10 000 детей одного родителя: уничтожение | 14,0 мс¹ | 3,7 мс | −74% |
| Создание всех 2 124 документов ProLife | 4 019 мс | 4 008 мс | в шуме |
| Размер `core.js` (raw / gzip) | 329 651 / 106 130 Б | 336 574 / 108 584 Б | +2,1% / +2,3% (цена исправлений, см. §4) |
| Размер бандла ProLife (raw / gzip) | 9 354 510 / 1 497 081 Б | 9 361 433 / 1 499 508 Б | +0,07% / +0,16% (в шуме) |
| Память на Item / Rectangle / Text / делегат | 828 / 871 / 1 580 / 7 037 Б | 841 / 874 / 1 576 / 7 047 Б | в шуме |

¹ метрики нет в базовой линии; «до» — принятое состояние перед оптимизацией (тот же бенчмарк, `BENCH_CORE`).

Все прочие метрики (привязки, сигналы, кадры Column / anchors / анимаций, создание и уничтожение делегатов,
документы Lisa и Puma) — в пределах шума. Ни одна метрика не ухудшилась сверх шума, кроме размера `core.js`.

Кроме цифр: найдено и исправлено 17 багов (два — зависание и утечка памяти), добавлены бенчмарк, стресс-тесты,
дифференциальные тесты против Qt 6.

## 2. Применено

| # | Изменение | Источник идеи | Файлы | Метрика | До | После | Δ % | Влияние на остальные метрики | Коммит |
|---|---|---|---|---|---|---|---|---|---|
| 1 | Ребёнок, уходящий с конца списка детей, снимается без `indexOf` (уничтожение N детей было O(N²)) | чтение `QtObject.$setParent`, стресс-тест 10 000 детей | Runtime/Core/QtObject.js | scale.children_10000_destroy | 13,97 мс | 2,74 мс | −80 | в шуме | fa097a7a0e |
| 2 | Делегат Repeater, созданный в конце сразу после предыдущего, не переставляется (N делегатов было O(N²)) | чтение `Repeater.$placeAfter` | Runtime/QtQuick/Repeater.js | scale.children_10000_create | 35,34 мс | 14,85 мс | −58 | в шуме (разовый выброс bind.conditional +25% в первом прогоне; повторно рядом с принятым состоянием +1,3%) | 13590fd87d |
| 3 | `mark_enclosed` минификатора с поиском по Set и без повторного обхода функций области; только на время minify | CPU-профиль сборки: `push_uniq` uglify-js — 8,2 из 14,6 с | compiler/uglifyScope.js, compiler/index.js | compile.ProLife.total (Lisa, Puma: −40%, −39%) | 18 217 мс | 9 634 мс | −47 | бандлы побайтно те же; codegen в шуме (вариант с постоянной заменой метода замедлял codegen на 3–8% — исправлено в 85afe7bdb2) | a5b605ccb2, 85afe7bdb2 |
| 4 | Раскладка ListView как в QQuickListView (исправление бага 6, §4) | профиль прокрутки: проход всех строк от первой на каждом кадре | Runtime/QtQuick/ListView.js | frame.listview_scroll_far_200 | 5 249 мс | 15,9 мс | −99,7 | +6,4 КБ `core.js` | 0625181ca2 |

## 3. Опробовано, не подошло

| # | Гипотеза | Источник | Результат замера | Причина отказа |
|---|---|---|---|---|
| 1 | Кэш позиций строк ListView префиксными суммами (те же сложения в том же порядке) | профиль прокрутки | не доведено | вытеснено: раскладка от первой строки сама была багом (зацикливание), заменена моделью Qt |
| 2 | Сигнал свойства привязки хранится в `Binding` вместо четырёх обращений на каждое вычисление | чтение `engine.evaluate` | bind.chain −2,3%, bind.fanout −4,3% (в шуме), heapPerDocument +1,4% | нет выигрыша сверх шума |
| 3 | `familyCss` мемоизирована по семейству (TextLayout строил строку шрифта на каждый `measure`) | CPU-профиль документов (familyCss + measure 1,4%) | docs.*.create −1,8 … −3,2% (порог шума 3,6–6,2%) | нет выигрыша сверх шума |
| 4 | Минификация без манглинга | время сборки | minify 14,0 → 2,6 с | регрессия: бандл ProLife +34% raw, +16% gzip |
| 5 | Ленивое создание DOM-узла Item, пулы Link, кэш `scopeCleared` по эпохе, предвычисление `isQPropertyBinding` | Vue 3.5 (переиспользование узлов зависимостей), Preact Signals, профиль | не реализовано | профиль рантайма плоский: каждая из этих функций ≤ 1% создания документов, ниже порога шума 3,6–6% |
| 6 | Ленивые / топологические вычисления привязок (alien-signals, Vue 3.5, Preact Signals, Solid, Angular, TC39 Signals: pull + version counting, без «глитчей») | обзор реактивных систем | не реализовано | риск семантики: Qt вычисляет привязки сразу по уведомлению, в порядке подписки, с промежуточными значениями (ромб даёт 2 вычисления — так и в Qt, проверено дифференциальным тестом) |

## 4. Исправленные баги

| ID | Серьёзность | файл:строка | Сценарий | Тест | Коммит | Влияние на метрики |
|---|---|---|---|---|---|---|
| B1 | высокая (зависание вкладки, память) | Runtime/QtQuick/ListView.js:974 (`$addVisible`, вся раскладка) | ListView со строками разной высоты, прыжок `contentY` (полоса прокрутки): позиции считались от первой строки по среднему размеру, среднее менялось после каждой раскладки, polish не заканчивался, удалённые делегаты копились до 4 ГБ | Views.test.js «rows of different heights jumped far down settles»; инварианты на 300 случайных сценариях (45 000 кадров) | 0625181ca2 | прокрутка −83 … −99,7%; `core.js` +6,4 КБ |
| B2 | высокая (страховка от зависаний) | Runtime/Core/engine.js:348 (`runPolish`) | polish, который всегда ставит polish снова, крутился вечно; теперь как `QQuickWindowPrivate::polishItems` / PolishLoopDetector: предупреждение после 1000, отказ на 10 000, кадр рисуется | Lifecycle.test.js «polish loop is given up» | acecce5ec0 | в шуме |
| B3 | средняя (утечка) | Runtime/QtQuick/Loader.js:65 | Loader явного размера держал освобождённый элемент и писал в него размер до следующей загрузки | Lifecycle.test.js «Loader … lets go» | 8f6f8c25e6 | в шуме |
| B4 | средняя | Runtime/Core/engine.js:274 (`endUpdate`) | `Qt.callLater` / `destroy()` из `Component.onDestruction` удалённого объекта ждали постороннего события | Lifecycle.test.js «call and delete queued by Component.onDestruction» | 4766aebd7d | в шуме |
| B5 | средняя (ложные циклы привязок) | Runtime/Core/engine.js:718 (`runBinding`) | привязка, чьё чтение включало отложенную привязку (flushPending), получала зависимости от того, что читает сеттер — «Binding loop detected» на кнопках ProLife | Lifecycle.test.js «takes no dependency from the write» | 0bdd77113a | в шуме |
| B6 | средняя | Runtime/QtQuick/Image.js:174 | `Image.$updateSize` читал `sourceSize` отслеживающими геттерами: 24 предупреждения «Binding loop … width» на странице Orders | QtCompat.test.js «image updating its size» | e2cbbaa436 | в шуме |
| B7 | средняя (исключение) | Runtime/QtQml/Models/ListModel.js:21 | `insert`/`move` больше ~120 000 строк за раз и `append` такого массива при показывающем ListView — RangeError (аргументы `splice`) | Lifecycle.test.js «150000 rows» | adfbdc19ca | в шуме |
| B8 | низкая (диагностика) | Runtime/Core/engine.js:174 (`dropSubscribers`) | `JQ.stats.links` не уменьшался при уничтожении объектов с обработчиками (ложная «утечка») | Lifecycle.test.js «subscriptions … counted out» | 17bcad9049 | в шуме |
| B9 | средняя (сеть) | Runtime/QtQuick/Image.js:58 | иконки перезапрашивались (сервер шлёт `Cache-Control: no-cache`): загрузчик на каждый Image и CSS-фон с тем же url. Теперь как `QQuickPixmapStore`: одна загрузка на url, blob для своего сервера | QtCompat.test.js (3 теста) | dc60986270 | Chromium 99 → 82 запроса за проход (82 иконки), WebKit, Firefox — ровно по одному |
| B11 | средняя (потеря состояния) | Runtime/QtQuick/ListView.js (`$modelMoved`), Runtime/QtQml/Models/ListModel.js (`move`) | `ListModel.move` давал представлениям `reset`: все делегаты пересоздавались и теряли состояние; теперь как QQmlChangeSet move — делегаты перемещённых строк те же объекты, текущая строка следует за своей без прокрутки (при вставке и удалении тоже: moveReason не SetIndex). Repeater, GridView, Instantiator по-прежнему пересобираются | Views.test.js «ListModel.move moves the delegates»; tests/diff/cases/listview_move.qml совпадает с Qt 6.12 (те же объекты делегатов и их число) | c97843f42f | в шуме |
| B12 | средняя (поведение) | Runtime/QtQuick/ListView.js, GridView.js (`$trackedPositionChanged`) | вид прыгал к новой текущей строке сразу; в Qt у вида всегда есть подсветка (без компонента — пустой Item), она едет к текущему элементу (SmoothedAnimation: ListView 400 px/с, GridView 150 мс), вид следует за ней только после нового currentIndex (moveReason SetIndex); после удаления текущей строки текущий элемент создаётся сразу | Views.test.js «the view follows the highlight», «GridView arrows»; diff: listview_follows_current, listview_model, gridview_model совпадают с Qt | d131b3e9b2 | в шуме |
| B13 | средняя (потеря состояния) | Runtime/QtQuick/GridView.js, Repeater.js, Instantiator.js | GridView пересоздавал все делегаты при любой вставке, удалении, перемещении; Repeater и Instantiator — при перемещении; теперь как Qt: объекты остаются, сигналы added/removed для перемещений не испускаются | diff: gridview_model, repeater_instantiator_move совпадают с Qt | 455ac09bd8, d131b3e9b2 | в шуме |
| B14 | средняя | Runtime/Qt/md5.js, Qt.js | `Qt.md5` не было (диалоги смены пароля IotPlatform; в v4 добавлен в #1171) | QtCompat.test.js; diff qt_md5_key_asterisk | d62c82a4d3 | в шуме |
| B15 | низкая | Runtime/QtQml/Models/Instantiator.js (`$clear`) | `objectRemoved` шёл с последнего объекта; в Qt с первого | QtCompat.test.js; diff instantiator_objects | 32e307bc44 | в шуме |
| B16 | средняя | Runtime/QtQuick/Image.js (`SourceSize`, `loadSize`) | `sourceSize` читался как -1 вместо размера картинки, не было `sourceSizeChanged`, svg не увеличивался до запрошенного размера, `PreserveAspectFit/Crop` не покрывали область — как в `QQuickImageProviderWithOptions::loadSize` | QtCompat.test.js; diff image_source_size; 30 замеров из #1171 в Chromium совпадают с Qt | 02e94cfed2 | в шуме |
| B17 | низкая | Runtime/QtQuick/Context2D.js | `arc`/`arcTo` с отрицательным радиусом: ошибка браузера вместо Qt («Incorrect argument radius», Error с кодом) | Canvas.test.js; Canvas_007 из #1171 в Chromium совпадает с Qt | a636045676 | в шуме |
| B10 | высокая (Firefox) | Runtime/Qt/Locale.js:4 | неизвестное имя локали бросало исключение (Firefox под автоматизацией: `navigator.language === "undefined"`) — ProLife висел на «Connecting» | QtCompat.test.js «C locale» | 165db2f3a7 | в шуме |

Цена исправлений: `core.js` +7 КБ (+2,1% raw, +2,3% gzip) — сверх порога 2%; бандлы приложений +0,07% (в шуме).
Почти весь рост — код раскладки ListView по Qt; варианта без него не нашлось.

## 5. Тестирование

| Набор | Что проверяет | Результат |
|---|---|---|
| `npm test` (tests/unit, оба runtime) | +21 тест: жизненный цикл и очереди (Lifecycle.test.js), раскладка ListView, картинки, локаль, эквивалентность минификатора | 326 из 326 |
| `npm run stress` (tests/stress, оба runtime) | утечки (WeakRef, плато кучи, подписки выживших объектов) для 9 видов делегатов Repeater, Loader, ListView со сменой модели, createObject + Connections; очереди (callLater, отложенные удаления, 10 000 таймеров, вложенные позиционеры); привязки 1→10 000, цепочки, ромбы, условные зависимости, циклы; шторм сигналов, connect/disconnect/destroy во время испускания; 100 000 Item, вложенность 1000, 10 000 детей; ListView 100 000 строк с прокруткой и изменениями модели; 10 000 событий указателя по вложенным Flickable/MouseArea; fuzz парсера (3 000 мутаций реального QML + 2 000 сгенерированных); детерминированность сборки; минификатор против исходного uglify на трёх приложениях | 62 из 62 |
| Инварианты ListView (разовый прогон, `/tmp`, сценарий в журнале) | 300 случайных видов: разная высота строк, spacing, header, секции, горизонтальный, BottomToTop, reuseItems, snapMode; прокрутка, прыжки, positionViewAtIndex, изменение размеров, вставки, удаления: видимые строки подряд, позиции последовательны, вид покрыт, contentHeight как в Qt | 45 000 кадров, 0 нарушений |
| `node tests/diff/run.js` (Qt 6.12, `qml -platform offscreen`) | одинаковый QML в Qt и v5, сравнение строк `console.log` | совпадают: порядок привязок и уведомлений, порядок сигналов при connect/disconnect во время испускания, ListView с одной высотой строк (позиции, origin, размер содержимого во всех режимах positionViewAtIndex), перемещения строк ListModel.move (какие объекты делегатов где и сколько создано), следование вида за подсветкой, изменения модели в ListView и GridView, перемещения в Repeater и Instantiator; расходятся — §6 (оценка строк разной высоты) |
| Браузер (Playwright: Chromium, WebKit, Firefox; ProLife) | запросы иконок к серверу (счётный прокси); снимки Orders, прокрутки, фильтра Vendors, редактора клиента; ошибки и предупреждения консоли | снимки побайтно совпадают с кодом до аудита; новых ошибок нет; предупреждения о циклах ширины ушли |
| Бенчмарк (tests/bench) | размер, сборка по фазам, документы приложений, привязки, сигналы, кадры, память; шум из трёх прогонов | baseline.json, results/, experiments.md |

## 6. Найдено, но не исправлено

| Что | Почему не исправлено |
|---|---|
| ListView со строками разной высоты: оценка непостроенных строк (`originY`, `contentHeight`) расходится с Qt на несколько строк (listview_heights.qml) — строки в виде как в Qt | Qt наполняет буфер инкубацией по кадрам, порядок обновления среднего размера иной; на списках одной высоты совпадение полное |
| Подсветка едет по приближённому профилю скорости (InOutQuad вместо QSmoothedAnimation): промежуточные положения отличаются от Qt, конечные совпадают | точный профиль QSmoothedAnimation — отдельная работа |
| Цепочка привязок глубже ~700 звеньев переполняет стек (ошибка в консоли, без зависания); так было и до аудита | изменение идёт по стеку (≈7 кадров на звено); в приложениях таких цепочек нет |
| Захват указателя и список наведённых держат уничтоженный элемент до следующего события мыши | ограниченно (один элемент), без наблюдаемых эффектов |
| Слово `createQmlObject` в комментарии любого файла добавляет в бандл компилятор QML для браузера (397 КБ) | реальные приложения не задеты |
| `Wrap` длинного слова: ширина перебором с конца, O(n²) измерений | двоичный поиск даёт другой результат при немонотонной ширине (кернинг) |
| Цикл привязок высоты в `TableBase.qml` (CustomScrollbar) | ошибка QML-кода, не рантайма (известна по PR #1159) |
| `scopeCleared` обходит не больше 64 родительских контекстов | проверено до 120: освобождение очищает контексты дерева само, не воспроизводится |
| Шумные метрики: `signal.connect_disconnect_10k`, `signal.emit_100connections_1k`, `destroy.delegates_1000`, `frame.anchors_chain100x100` (бимодальна в V8) | работа на замер < 2 мс; решения по ним не принимались |

## 7. Идеи на будущее

| Идея | Оценка |
|---|---|
| Кэш разбора выражений привязок в codegen (uglify parse + print — основное время codegen, 3 с ProLife) | −10…30% codegen; зависит от контекста имён, нужен ключ (текст + scope) |
| Браузерный бенчмарк: первый кадр, длительность кадра прокрутки, число DOM-узлов, forced reflow (Playwright есть, обвязка `walk.js` в сессии) | нужен стабильный стенд: в Node DOM-стоимость не видна |
| Ленивый DOM-узел Item (создавать при первой отрисовке) | память и время создания в браузере; в Node не измерить |
| IotPlatform в бенчмарке | исходников нет на машине; добавить в tests/bench/apps.js |
| Шаг веб-компиляции в CMake не зависит от `dist/core.js` и `compiler/` | после изменения рантайма бандл продукта не пересобирается сам; нужна правка `WebCompiler.cmake` (DEPENDS) |

Источники: qtdeclarative 6.8 (`qquicklistview.cpp`, `qquickitemview.cpp`, `qquickwindow.cpp` PolishLoopDetector,
`qqmlbinding.cpp`, `qquickpixmapcache.cpp`); uglify-js 3.19.3 `lib/scope.js`; https://github.com/stackblitz/alien-signals ;
https://github.com/vuejs/core/pull/10397 (Vue 3.5 reactivity) ; https://preactjs.com/blog/signal-boosting/ ;
https://github.com/tc39/proposal-signals ; https://github.com/slint-ui/slint ; https://github.com/qmlweb/qmlweb ;
https://docs.flutter.dev/resources/architectural-overview#rendering-and-layout ; https://tanstack.com/virtual .

## 8. Схема работы системы

1. **Компиляция** (`compiler/index.js`): parse → model → codegen (один класс на тип, функции привязок, сборка тела
   по слотам id) → minify (uglify-js; **изменено:** `mark_enclosed` без квадратичного поиска, compiler/uglifyScope.js).
2. **Создание** (`JQ.create`, `Component.createObject`): объекты, привязки откладываются (`scheduleBinding`), в
   `endCreate` включаются от последней к первой, затем bindable (x, y, width, height), componentComplete, onCompleted.
   **Изменено:** снятие ребёнка с конца за O(1); Repeater не переставляет делегат, уже стоящий на месте.
3. **Привязки** (`engine.js`): вычисление собирает зависимости (DepLink), изменение уведомляет подписчиков в порядке
   подписки (как QQmlNotifier). **Изменено:** запись значения привязки идёт без активной привязки (B5).
4. **Очереди** (`endUpdate`): callLater → запись в DOM → отложенные удаления. **Изменено:** проходы повторяются, пока
   очереди не пусты (B4).
5. **Кадр** (`runFrame`): таймеры, срок которых пришёл → кадровые элементы → polish. **Изменено:** PolishLoopDetector
   (B2); ListView в polish — layout по Qt (visibleItems, refill, fixup) (B1).
