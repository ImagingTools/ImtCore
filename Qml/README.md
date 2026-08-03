# ImtCore QML

Коллекция переиспользуемых QML-модулей ImtCore — визуальные компоненты, экраны и
логика представления для десктопных и веб- (через JQML) приложений на базе ACF/ImtCore.

Каждый подкаталог — это отдельный **QML-модуль** (`module <имя>` в `qmldir`),
подключаемый строкой `import <имя> 1.0`. Компоненты внутри модуля адресуются по
**имени типа** (например, `UserView { }`), а сопоставление «имя типа → файл»
задаётся в `qmldir`. Физический состав ресурса (что попадает в бинарник через
`rcc`) описан в файле `*.qrc` каждого модуля.

## Документация

| Документ | Назначение |
|---|---|
| [STRUCTURE.md](Docs/STRUCTURE.md) | Руководство по размещению нового компонента: как выбрать модуль и подпапку (таблицы модулей и подпапок). |
| [STRUCTURE-app.md](Docs/STRUCTURE-app.md) | Каталог **прикладных** модулей с описанием компонентов (свойства, сигналы, функции). |
| [STRUCTURE-base.md](Docs/STRUCTURE-base.md) | Каталог **базовых** модулей с описанием компонентов. |

## Модули

### Базовые модули

Фундаментальные модули, на которых строятся прикладные. Полное описание — в
[STRUCTURE-base.md](Docs/STRUCTURE-base.md).

| Модуль | Назначение |
|---|---|
| [`imtcontrols`](Docs/STRUCTURE-base.md#imtcontrols) | Библиотека UI-контролов и базовых классов |
| [`imtstyle`](Docs/STRUCTURE-base.md#imtstyle) | Стили и оформление (отдельный модуль, не подпапка) |
| [`imtqmlutils`](Docs/STRUCTURE-base.md#imtqmlutils) | Невизуальные утилиты (математика, валидаторы, модели, события) |
| [`imtgraphics2d`](Docs/STRUCTURE-base.md#imtgraphics2d) | 2D-графика на Canvas (фигуры, графики, матрицы) |
| [`imtgui`](Docs/STRUCTURE-base.md#imtgui) | Каркас приложения и представлений |

### Прикладные модули

Доменные модули, собранные из базовых. Полное описание — в
[STRUCTURE-app.md](Docs/STRUCTURE-app.md).

| Модуль | Назначение |
|---|---|
| [`imtauthgui`](Docs/STRUCTURE-app.md#imtauthgui) | Аутентификация, пользователи и организации |
| [`imtdeskgui`](Docs/STRUCTURE-app.md#imtdeskgui) | Служба поддержки (тикеты) |
| [`imtdocgui`](Docs/STRUCTURE-app.md#imtdocgui) | Документо-ориентированные рабочие области |
| [`imtcolgui`](Docs/STRUCTURE-app.md#imtcolgui) | Коллекции и представления коллекций |
| [`imtguigql`](Docs/STRUCTURE-app.md#imtguigql) | GraphQL/SDL-инфраструктура и удалённые данные |
| [`imtchatgui`](Docs/STRUCTURE-app.md#imtchatgui) | Чат и обмен сообщениями |
| [`imtlicgui`](Docs/STRUCTURE-app.md#imtlicgui) | Лицензирование (продукты, функции, лицензии) |
| [`imtgeogui`](Docs/STRUCTURE-app.md#imtgeogui) | Адреса и географические структуры |
| [`imt3dgui`](Docs/STRUCTURE-app.md#imt3dgui) | 3D-сцены (обёртки над Qt Quick 3D) |

## Добавление компонента

Определите модуль и подпапку по [STRUCTURE.md](Docs/STRUCTURE.md), затем зарегистрируйте
тип в `qmldir` и ресурсе `*.qrc` соответствующего модуля (чек-лист — в конце
[STRUCTURE.md](Docs/STRUCTURE.md)).
