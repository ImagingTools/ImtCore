# Предложение по реорганизации библиотеки ImtCore

**Дата:** Январь 2026  
**Автор:** Анализ кода  
**Статус:** Предложение

## Краткое содержание

Данный документ предлагает реорганизацию структуры библиотеки ImtCore для улучшения обнаруживаемости, поддерживаемости и логической группировки. Текущая плоская структура содержит 61 библиотеку в одном каталоге, что затрудняет понимание взаимосвязей и навигацию по кодовой базе.

## Анализ текущего состояния

### Текущая структура
Папка `Include/` содержит 61 библиотеку, организованную в плоской структуре:
- Все библиотеки имеют префикс `imt`
- Суффиксы указывают на технические уровни: `gui`, `gql`, `db`, `qml`
- Отсутствует иерархическая организация
- Нет четкого разделения между доменами и техническими аспектами

### Выявленные шаблоны

#### 1. **Технические уровни на основе суффиксов**
- `*gui` - Компоненты пользовательского интерфейса на Qt (9 библиотек: imt3dgui, imtauthgui, imtdbgui, imtdevgui, imtgui, imthypegui, imtlicgui, imtloggui, imtreportgui)
- `*gql` - Конечные точки GraphQL API (8 библиотек: imtgql, imtclientgql, imtservergql, imtguigql, imtgqltest, imtauthgql, imtlicgql, imtdbgql)
- `*db` - Уровни постоянного хранения данных (2 библиотеки: imtauthdb, imtlicdb)
- `*qml` - Компоненты UI на QML/JavaScript (3 библиотеки: imtqml, imtqml2d, imtstylecontrolsqml)
- `gen*` - Генераторы кода (3 библиотеки: imtsdlgencpp, imtsdlgenqml, imtddl)

#### 2. **Предметные области** (без суффикса)
- **Основная инфраструктура**: base, core, com, col, test
- **Аутентификация и авторизация**: auth
- **Управление данными**: db, file, doc, repo
- **Безопасность**: crypt, lic
- **Коммуникация**: http, rest, mail, service
- **Контент и медиа**: img, 3d, geo, hype
- **Инструменты разработки**: dev, ddl, sdl, oas, design, py
- **Фреймворки приложений**: app, serverapp, update, log

## Проблемы текущей структуры

### 1. **Проблемы с обнаруживаемостью**
- Сложно найти связанные библиотеки (например, auth, authdb, authgql, authgui находятся отдельно)
- Нет четких точек входа для разработчиков
- Неясно, какие библиотеки являются фундаментальными, а какие специализированными

### 2. **Проблемы с поддержкой**
- Отсутствует визуальная группировка связанного кода
- Сложно понять зависимости между уровнями
- Непоследовательное именование (например, `imtmdbx`, `imtmongo` против `imtdb`)

### 3. **Несогласованность именования**
- Смесь сокращений и полных имен (`img` против `image`, `lic` против `license`)
- Неясное назначение только из имен (`ej`, `oas`, `ddl`, `hype`)
- Некоторые имена конфликтуют с общими терминами (`service`, `test`)

### 4. **Проблемы масштабируемости**
- Плоская структура плохо масштабируется после 60+ библиотек
- Добавление новых функций создает больше беспорядка
- Нет четкого шаблона для размещения нового кода

## Предлагаемая реорганизация

### Вариант А: Иерархическая по доменам (Рекомендуется)

Организация библиотек по бизнес-доменам с подкаталогами для технических уровней:

```
Include/
├── core/                          # Базовые библиотеки
│   ├── base/                      # imtbase → core/base
│   ├── core/                      # imtcore → core/core
│   ├── collections/               # imtcol → core/collections
│   ├── communication/             # imtcom → core/communication
│   └── testing/                   # imttest → core/testing
│
├── auth/                          # Аутентификация и авторизация
│   ├── core/                      # imtauth → auth/core
│   ├── db/                        # imtauthdb → auth/db
│   ├── gql/                       # imtauthgql → auth/gql
│   └── gui/                       # imtauthgui → auth/gui
│
├── database/                      # Постоянное хранение данных
│   ├── core/                      # imtdb → database/core
│   ├── gql/                       # imtdbgql → database/gql
│   ├── gui/                       # imtdbgui → database/gui
│   ├── mdbx/                      # imtmdbx → database/mdbx
│   └── mongodb/                   # imtmongo → database/mongodb
│
├── licensing/                     # Управление лицензиями
│   ├── core/                      # imtlic → licensing/core
│   ├── db/                        # imtlicdb → licensing/db
│   ├── gql/                       # imtlicgql → licensing/gql
│   └── gui/                       # imtlicgui → licensing/gui
│
├── content/                       # Обработка контента и медиа
│   ├── files/                     # imtfile → content/files
│   ├── images/                    # imtimg → content/images
│   ├── documents/                 # imtdoc → content/documents
│   ├── compression/               # imtzip → content/compression
│   └── repositories/              # imtrepo → content/repositories
│
├── visualization/                 # 2D/3D рендеринг
│   ├── 3d/
│   │   ├── core/                  # imt3d → visualization/3d/core
│   │   ├── gui/                   # imt3dgui → visualization/3d/gui
│   │   └── view/                  # imt3dview → visualization/3d/view
│   ├── hypermedia/
│   │   ├── core/                  # imthype → visualization/hypermedia/core
│   │   └── gui/                   # imthypegui → visualization/hypermedia/gui
│   └── geospatial/                # imtgeo → visualization/geospatial
│
├── api/                           # Уровни API
│   ├── graphql/
│   │   ├── core/                  # imtgql → api/graphql/core
│   │   ├── client/                # imtclientgql → api/graphql/client
│   │   ├── server/                # imtservergql → api/graphql/server
│   │   ├── gui/                   # imtguigql → api/graphql/gui
│   │   └── testing/               # imtgqltest → api/graphql/testing
│   ├── rest/                      # imtrest → api/rest
│   ├── http/                      # imthttp → api/http
│   └── openapi/                   # imtoas → api/openapi
│
├── security/                      # Безопасность и шифрование
│   └── cryptography/              # imtcrypt → security/cryptography
│
├── communication/                 # Внешние коммуникации
│   ├── email/                     # imtmail → communication/email
│   └── service/                   # imtservice → communication/service
│
├── ui/                           # Компоненты пользовательского интерфейса
│   ├── core/                      # imtgui → ui/core
│   ├── widgets/                   # imtwidgets → ui/widgets
│   ├── qml/
│   │   ├── core/                  # imtqml → ui/qml/core
│   │   ├── 2d/                    # imtqml2d → ui/qml/2d
│   │   └── controls/              # imtstylecontrolsqml → ui/qml/controls
│   ├── styling/                   # imtstyle → ui/styling
│   └── design-system/             # imtdesign → ui/design-system
│
├── reporting/                     # Отчетность и логирование
│   ├── reports/
│   │   ├── core/                  # imtreport → reporting/reports/core
│   │   └── gui/                   # imtreportgui → reporting/reports/gui
│   └── logging/
│       ├── core/                  # imtlog → reporting/logging/core
│       └── gui/                   # imtloggui → reporting/logging/gui
│
├── codegen/                       # Инструменты генерации кода
│   ├── sdl/
│   │   ├── core/                  # imtsdl → codegen/sdl/core
│   │   ├── cpp/                   # imtsdlgencpp → codegen/sdl/cpp
│   │   └── qml/                   # imtsdlgenqml → codegen/sdl/qml
│   └── ddl/                       # imtddl → codegen/ddl
│
├── development/                   # Инструменты разработчика
│   ├── core/                      # imtdev → development/core
│   ├── gui/                       # imtdevgui → development/gui
│   ├── python/                    # imtpy → development/python
│   └── export/                    # imtej → development/export
│
└── application/                   # Фреймворки приложений
    ├── desktop/                   # imtapp → application/desktop
    ├── server/                    # imtserverapp → application/server
    └── updates/                   # imtupdate → application/updates
```

### Вариант Б: Гибридный подход (Уровень + Домен)

Организация сначала по техническому уровню, затем по домену:

```
Include/
├── core/                          # Основные библиотеки (без зависимостей)
│   ├── base/
│   ├── core/
│   ├── collections/
│   └── testing/
│
├── infrastructure/                # Инфраструктурные сервисы
│   ├── database/
│   ├── communication/
│   ├── security/
│   └── logging/
│
├── domain/                        # Бизнес-домены
│   ├── auth/
│   ├── licensing/
│   ├── content/
│   └── visualization/
│
├── api/                          # Уровни API
│   ├── graphql/
│   ├── rest/
│   └── http/
│
├── ui/                           # Пользовательские интерфейсы
│   ├── qt/
│   └── qml/
│
└── tools/                        # Инструменты разработки
    ├── codegen/
    └── development/
```

## Детальные рекомендации

### 1. Соглашения об именовании

#### Текущие проблемы
- Несогласованные сокращения (img/image, lic/license, auth/authentication)
- Неясные значения (ej, oas, hype, ddl)
- Избыточные префиксы (все начинаются с `imt`)

#### Предлагаемые стандарты

**Имена каталогов:**
- Использовать полные, описательные имена: `images`, а не `img`, `licensing`, а не `lic`
- Избегать загадочных сокращений
- Использовать kebab-case для многословных имен: `design-system`, `open-api`

**Имена библиотек/пространств имен:**
- Сохранять префикс `imt` для пространств имен/библиотек C++ во избежание конфликтов
- Структура каталогов обеспечивает контекст, имена могут быть проще
- Индикаторы технических уровней (`gui`, `gql`, `db`) остаются понятными

**Примеры:**
```
Текущее:            Предлагаемый каталог:       Имя библиотеки:
imtimg           →  content/images/         →  imtimg или imt::images
imtlic           →  licensing/core/         →  imtlic или imt::licensing
imtej            →  development/export/     →  imtej или imt::export
imtoas           →  api/openapi/            →  imtoas или imt::openapi
imthype          →  visualization/hypermedia/ → imthype или imt::hypermedia
```

### 2. Стратегия миграции

#### Фаза 1: Документация и подготовка (Неделя 1-2)
1. Документировать все межбиблиотечные зависимости
2. Создать скрипты миграции для автоматизированного перемещения файлов
3. Обновить систему сборки (CMake) для поддержки старых и новых путей
4. Уведомить всех разработчиков об изменениях

#### Фаза 2: Постепенная миграция (Неделя 3-6)
1. Создать новую структуру каталогов
2. Добавить символические ссылки со старых на новые местоположения
3. Мигрировать по одному домену за раз (начиная с наименьшего)
4. Постепенно обновлять импорты/включения
5. Тестировать после миграции каждого домена

#### Фаза 3: Очистка (Неделя 7-8)
1. Удалить символические ссылки
2. Удалить старую структуру каталогов
3. Обновить всю документацию
4. Финальное тестирование и проверка

#### Фаза 4: Завершение
1. Обновить документацию по вводу разработчиков
2. Архивировать этот документ миграции
3. Установить руководства для будущих дополнений

### 3. Обновления системы сборки

#### Требуемые изменения CMake
```cmake
# Старый подход
include_directories(${IMTCORE_DIR}/Include/imtauth)
include_directories(${IMTCORE_DIR}/Include/imtauthgui)

# Новый подход (Вариант А)
include_directories(${IMTCORE_DIR}/Include/auth/core)
include_directories(${IMTCORE_DIR}/Include/auth/gui)

# Или с вспомогательным макросом
imt_include_domain(auth)  # Включает все подкаталоги auth/*
```

#### Обратная совместимость
```cmake
# Слой совместимости во время перехода
if(EXISTS ${IMTCORE_DIR}/Include/auth/core)
    set(IMT_NEW_STRUCTURE ON)
    set(IMTAUTH_INCLUDE_DIR ${IMTCORE_DIR}/Include/auth/core)
else()
    set(IMTAUTH_INCLUDE_DIR ${IMTCORE_DIR}/Include/imtauth)
endif()
```

### 4. Лучшие практики структуры кода

#### Принципы группировки
1. **Связность**: Связанная функциональность остается вместе
2. **Слабая связанность**: Четкие границы между доменами
3. **Многоуровневость**: Зависимости идут от core → domain → api → ui
4. **Обнаруживаемость**: Структура соответствует ментальным моделям

#### Организация технических уровней
Каждый домен следует согласованной структуре:
```
domain/
├── core/           # Логика домена (без зависимостей UI/API)
├── db/             # Уровень постоянства
├── gql/            # GraphQL API
├── gui/            # Компоненты Qt GUI
└── qml/            # Компоненты QML UI (при необходимости)
```

### 5. Требования к документации

#### Документация для каждой библиотеки
Каждый каталог библиотеки должен содержать:
- `README.md` - Назначение, зависимости, примеры использования
- `CHANGELOG.md` - История версий
- `CMakeLists.txt` - Конфигурация сборки

#### Документация по доменам
Каждая папка домена должна содержать:
- `README.md` - Обзор домена и его библиотек
- Архитектурные диаграммы, показывающие зависимости уровней
- Руководство по началу работы

### 6. Риски перехода и их снижение

| Риск | Влияние | Снижение |
|------|---------|----------|
| Нарушение сборки | Высокое | Поддерживать слой обратной совместимости в течение 2 циклов выпуска |
| Путаница разработчиков | Среднее | Полная документация, учебные сессии команды |
| Внешние зависимости | Среднее | Документировать все публичные API, предоставить руководство по миграции |
| Потеря истории Git | Низкое | Использовать `git mv` для сохранения истории |
| Конфликты слияния | Высокое | Координация с командой, минимизация параллельной работы во время миграции |

## Сравнение вариантов

| Критерий | Вариант А (Домен-первый) | Вариант Б (Уровень-первый) |
|----------|------------------------|------------------------|
| **Обнаруживаемость** | ⭐⭐⭐⭐⭐ Отлично - связанные функции сгруппированы | ⭐⭐⭐ Хорошо - но нужно искать в нескольких разделах |
| **Поддержка** | ⭐⭐⭐⭐⭐ Весь код auth вместе | ⭐⭐⭐ Код разбросан по уровням |
| **Функциональные команды** | ⭐⭐⭐⭐⭐ Идеально для доменных команд | ⭐⭐⭐ Лучше для платформенных команд |
| **Управление зависимостями** | ⭐⭐⭐⭐ Четко внутри доменов | ⭐⭐⭐⭐⭐ Четко между уровнями |
| **Кривая обучения** | ⭐⭐⭐⭐ Интуитивно для доменной работы | ⭐⭐⭐ Требует понимания обеих осей |
| **Масштабируемость** | ⭐⭐⭐⭐⭐ Растет естественно с функциями | ⭐⭐⭐⭐ Хорошо, но больше каталогов на верхнем уровне |

## Рекомендация

**Принять Вариант А: Иерархическая по доменам**

### Обоснование
1. **Рабочий процесс разработчика**: Большая часть работы происходит в одном домене (например, работа над функциями auth затрагивает auth/core, auth/db, auth/gql, auth/gui)
2. **Владение кодом**: Команды могут владеть целыми доменами
3. **Снижение когнитивной нагрузки**: Поиск "всего кода auth" происходит мгновенно
4. **Отраслевой стандарт**: Соответствует общим шаблонам микросервисов и проектирования на основе доменов
5. **Лучший опыт работы с IDE**: Сворачиваемые папки доменов в IDE

### Приоритет реализации

**Фаза 1 - Высокое влияние, низкий риск:**
1. `auth/` - Четкие границы домена
2. `licensing/` - Самодостаточный
3. `reporting/` - Ограниченные зависимости

**Фаза 2 - Среднее влияние:**
4. `database/` - Основная инфраструктура
5. `visualization/` - Изолированный домен
6. `api/` - Четкая техническая группировка

**Фаза 3 - Сложная (много зависимостей):**
7. `ui/` - Много сквозных проблем
8. `core/` - Базовые зависимости
9. `application/` - Фреймворки верхнего уровня

## Быстрые победы (Немедленные действия)

При планировании полной реорганизации эти немедленные улучшения могут быть сделаны:

### 1. Добавить файлы README
Создать `README.md` в каждой библиотеке, объясняющий:
- Назначение и обязанности
- Зависимости
- Ключевые классы/компоненты
- Примеры использования

### 2. Создать индексную документацию
Добавить `Include/README.md` с:
- Обзор всех библиотек
- Диаграмма зависимостей
- Объяснение соглашений об именовании
- Как выбрать правильную библиотеку

### 3. Улучшить имена библиотек (без перемещения файлов)
Документировать более четкие имена/псевдонимы:
- `imtej` → Утилиты экспорта/JSON
- `imtoas` → Инструменты схемы OpenAPI
- `imthype` → Рендеринг гипермедиа
- `imtddl` → Язык определения данных

### 4. Установить руководства по именованию
Документ для будущих библиотек:
- Когда создавать новую библиотеку
- Соглашения об именовании
- Требуемая структура (`core` → `db` → `gql` → `gui`)
- Правила зависимостей

## Метрики успеха

Отслеживать эти метрики до и после реорганизации:

1. **Время поиска кода**: Опрос разработчиков о "времени поиска функции X"
2. **Время сборки**: Убедиться в отсутствии регрессии производительности сборки
3. **Время адаптации**: Время для понимания структуры новыми разработчиками
4. **Стоимость поддержки**: Время для внесения сквозных изменений
5. **Повторное использование кода**: Отслеживание дублирующегося кода в доменах

## Заключение

Текущая плоская структура из 61 библиотеки выполнила свою цель, но достигла пределов масштабируемости. Доменная иерархическая организация:

- Улучшит обнаруживаемость кода
- Поддержит владение функциональными командами
- Снизит когнитивную нагрузку
- Лучше масштабируется по мере роста кодовой базы
- Соответствует современным шаблонам архитектуры программного обеспечения

**Рекомендуемое действие**: Начать с улучшений документации (Быстрые победы) при планировании полной миграции на структуру Варианта А.

## Приложения

### Приложение А: Полное сопоставление библиотек

| Текущая библиотека | Новое местоположение (Вариант А) | Назначение |
|----------------|------------------------|---------|
| imt3d | visualization/3d/core | Ядро 3D-движка |
| imt3dgui | visualization/3d/gui | 3D GUI компоненты |
| imt3dview | visualization/3d/view | 3D окно просмотра |
| imtapp | application/desktop | Фреймворк настольного приложения |
| imtauth | auth/core | Ядро аутентификации |
| imtauthdb | auth/db | Постоянство auth |
| imtauthgql | auth/gql | Auth GraphQL API |
| imtauthgui | auth/gui | Компоненты Auth UI |
| imtbase | core/base | Базовый фреймворк |
| imtclientgql | api/graphql/client | Клиент GraphQL |
| imtcol | core/collections | Утилиты коллекций |
| imtcom | core/communication | Протоколы связи |
| imtcore | core/core | Основная информация о версии |
| imtcrypt | security/cryptography | Услуги шифрования |
| imtdb | database/core | Ядро SQL базы данных |
| imtdbgql | database/gql | База данных GraphQL API |
| imtdbgui | database/gui | UI базы данных |
| imtddl | codegen/ddl | Язык определения данных |
| imtdesign | ui/design-system | Токены дизайна |
| imtdev | development/core | Ядро инструментов разработки |
| imtdevgui | development/gui | UI инструментов разработки |
| imtdoc | content/documents | Обработка документов |
| imtej | development/export | Утилиты экспорта/JSON |
| imtfile | content/files | Файловый ввод-вывод |
| imtgeo | visualization/geospatial | Географические данные |
| imtgql | api/graphql/core | Ядро GraphQL |
| imtgqltest | api/graphql/testing | Тесты GraphQL |
| imtgui | ui/core | Основные виджеты GUI |
| imtguigql | api/graphql/gui | Интеграция GraphQL UI |
| imthttp | api/http | HTTP клиент/сервер |
| imthype | visualization/hypermedia | Рендеринг гипермедиа |
| imthypegui | visualization/hypermedia/gui | UI гипермедиа |
| imtimg | content/images | Обработка изображений |
| imtlic | licensing/core | Управление лицензиями |
| imtlicdb | licensing/db | Постоянство лицензий |
| imtlicgql | licensing/gql | Лицензирование GraphQL API |
| imtlicgui | licensing/gui | UI лицензирования |
| imtlog | reporting/logging/core | Фреймворк логирования |
| imtloggui | reporting/logging/gui | UI логирования |
| imtmail | communication/email | Email/SMTP |
| imtmdbx | database/mdbx | База данных MDBX |
| imtmongo | database/mongodb | Интеграция MongoDB |
| imtoas | api/openapi | Схема OpenAPI |
| imtpy | development/python | Привязки Python |
| imtqml | ui/qml/core | Основные утилиты QML |
| imtqml2d | ui/qml/2d | 2D графика QML |
| imtrepo | content/repositories | Управление репозиториями |
| imtreport | reporting/reports/core | Движок отчетности |
| imtreportgui | reporting/reports/gui | UI отчетов |
| imtrest | api/rest | Фреймворк REST API |
| imtsdl | codegen/sdl/core | Язык определения схемы |
| imtsdlgencpp | codegen/sdl/cpp | Генератор SDL в C++ |
| imtsdlgenqml | codegen/sdl/qml | Генератор SDL в QML |
| imtserverapp | application/server | Фреймворк серверного приложения |
| imtservergql | api/graphql/server | Сервер GraphQL |
| imtservice | communication/service | Фреймворк сервисов |
| imtstyle | ui/styling | Система стилей/тем |
| imtstylecontrolsqml | ui/qml/controls | Элементы управления стилями QML |
| imttest | core/testing | Утилиты тестирования |
| imtupdate | application/updates | Управление обновлениями |
| imtwidgets | ui/widgets | Библиотека виджетов UI |
| imtzip | content/compression | Сжатие/ZIP |

### Приложение Б: Анализ зависимостей

Ключевые цепочки зависимостей (снизу вверх):
```
Уровень 0 (Без зависимостей):
  - core/core, core/base, core/testing

Уровень 1 (Только ядро):
  - core/collections, core/communication
  - security/cryptography
  - content/files, content/compression

Уровень 2 (Инфраструктура):
  - database/core, database/mdbx, database/mongodb
  - api/http, communication/email
  
Уровень 3 (Логика домена):
  - auth/core, licensing/core
  - content/images, content/documents
  - visualization/3d/core

Уровень 4 (Постоянство):
  - auth/db, licensing/db
  - database/gql

Уровень 5 (API):
  - auth/gql, licensing/gql
  - api/graphql/server, api/rest

Уровень 6 (UI):
  - auth/gui, licensing/gui
  - ui/core, ui/widgets, ui/qml/core

Уровень 7 (Приложения):
  - application/desktop, application/server
```

### Приложение В: Совместимость внешних интерфейсов

**Публичные заголовки**: Эти заголовки, вероятно, используются внешними проектами:
- `Include/ImtCore.h` - Основной включающий файл (требует обновления)
- Все экспорты библиотек (COM интерфейсы)
- Публичные заголовки API в библиотеках `gql/` и `rest/`

**Стратегия совместимости**:
1. Сохранить `ImtCore.h` на верхнем уровне, обновить включения
2. Предоставить заголовки совместимости в старых местоположениях (устаревшие)
3. Использовать `#pragma message` для предупреждения об устаревших включениях
4. Удалить слой совместимости после 2 основных версий

### Приложение Г: Инструменты для миграции

**Рекомендуемые скрипты**:
```bash
# 1. Найти все операторы включения
grep -r "include.*imtauth" --include="*.h" --include="*.cpp"

# 2. Генератор графа зависимостей
cmake --graphviz=deps.dot .
dot -Tpng deps.dot -o deps.png

# 3. Автоматизированный скрипт миграции
./migrate-library.sh imtauth auth/core

# 4. Валидация
./validate-includes.sh
```

**Миграция Git**:
```bash
# Сохранить историю
git mv Include/imtauth Include/auth/core
git mv Include/imtauthdb Include/auth/db
git mv Include/imtauthgql Include/auth/gql
git mv Include/imtauthgui Include/auth/gui
```
