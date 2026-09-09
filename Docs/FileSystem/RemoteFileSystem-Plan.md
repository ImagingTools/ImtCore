# План: доведение Remote File System до целевой архитектуры + интеграция в Agentino

> Документ самодостаточен и рассчитан на исполнителя, который приходит «с холода».
> Все пути — относительно корня репозитория `ImtCore` (`d:\ImagingTools\Git\ImtCore`).
> Agentino — соседний репозиторий `../Agentino` (`d:\ImagingTools\Git\Agentino`).

## Принятые решения (зафиксированы, не пересматривать без явного согласования)

1. **Объём FS-операций: только read/browse.** Просмотр каталогов + выбор пути. Никаких
   write-операций (create/rename/delete) — ни в SDL, ни в домене, ни на агенте.
   Интерфейс проектируется чистым (read-only), без «заготовок» write-методов.
2. **SDL-codegen: да.** Перейти на генерируемые из SDL типы (как в Agentino: `sdl::V1_0::…`,
   `CXxxGqlHandlerCompBase`, типизированные request/payload). Ручную сборку `QJsonObject` убрать.
3. **RootPath: жёсткий единый.** Пустой `path` → canonical(`RootPath`); всё за пределами root →
   отказ. Никакого перечисления дисков Windows. Выбор exe возможен только внутри
   сконфигурированного root; вне root — только ручной ввод (fallback в UI).

---

## 1. Исходное состояние (это «наброски», один автокоммит `2d7a2a2723`)

Слои и файлы:

**Domain (imtfile)**
- `Include/imtfile/IFileSystemStructureProvider.h` — один метод
  `GetFileSystemEntries(path, &entries, &resolvedPath, &parentPath, &errorMessage)` (4 out-параметра).
- `Include/imtfile/CFileSystemStructureProviderComp.{h,cpp}` — локальная реализация на `QDir`,
  sandbox по `RootPath` через canonical-path. Атрибуты: `RootPath`, `ShowFiles`, `ShowHidden`.

**Server GQL (imtservergql)**
- `Include/imtservergql/CFileSystemControllerComp.{h,cpp}` — обработчик команды
  `GetFileSystemEntries`, **вручную** собирает JSON. Наследует `CGqlRequestHandlerCompBase` →
  **проверки прав нет**. Ссылается на `IFileSystemStructureProvider` через атрибут `FileSystemProvider`.

**Client GQL (imtclientgql)**
- `Include/imtclientgql/CRemoteFileSystemControllerComp.{h,cpp}` — редирект на другую машину через
  фиксированный `IGqlClient` (атрибут `ApiClient`). Вручную парсит JSON-ответ и пересобирает его.
  **Наследуется от `imtservergql::CGqlRequestHandlerCompBase`** (кросс-слойная зависимость client→server).

**Schema**
- `Sdl/imtbase/1.0/FileSystem.sdl` — только `Query.GetFileSystemEntries`. Codegen НЕ используется
  (весь JSON собирается руками). Схема уже добавлена в `Sdl/imtbase/CMake/CMakeLists.txt`.

**QML (imtguigql)**
- `Qml/imtguigql/ServerPathPicker.qml` — текстовое поле + кнопка Browse, открывает диалог.
- `Qml/imtguigql/FileSystemBrowserDialog.qml` — навигация; шлёт `GetFileSystemEntries` через
  амбиентный `GqlRequestSender` (в «текущий» сервер), **без возможности адресовать конкретную машину/агента**.
- Регистрация: `Qml/imtguigql/qmldir`, `Qml/imtguigql/imtguigqlqml.qrc`.

**Регистрация в пакетах**
- `Impl/ImtFilePck/ImtFilePck.{h,cpp}` → `FileSystemStructureProvider`.
- `Impl/ImtServerGqlPck/ImtServerGqlPck.{h,cpp}` → `FileSystemController`.
- `Impl/ImtClientGqlPck/ImtClientGqlPck.{h,cpp}` → `RemoteFileSystemController`.

**Документация**
- `Docs/FileSystem/FileSystemBrowser.md` — описывает текущую (two-controller) архитектуру.

**Agentino (соседний репозиторий)**
- `../Agentino/Include/agentinoqml/Qml/ServiceEditor.qml` — поля `Path`, `Arguments`,
  `Start/Stop Script`, `Settings Path` — простые `TextInputElementView`, пишут в
  `serviceData.m_path` и т.д. (см. `generalPageComp`, поле `pathInput`, строки ~443–466).
- `../Agentino/Sdl/agentino/1.0/Services.sdl` — `ServiceData.path: String`; мутации
  `LoadPlugin/AddService/UpdateService/StartService/StopService/...`.
- `../Agentino/Sdl/agentino/1.0/Topology.sdl` — `Service.agentId: ID!` (сервис принадлежит агенту).
- `../Agentino/Include/agentinogql/CServiceControllerProxyComp.{h,cpp}` — **ключевой механизм**:
  форвардит GQL на конкретного агента через `SendModelRequest<>()` + контекст-заголовок
  `clientid = agentId`. Именно его надо переиспользовать для FS-браузинга.

---

## 2. Проблемы (SOLID / безопасность / качество)

**Архитектура / DIP (главная):** «удалённость» смоделирована на уровне GQL-обработчика — два
разных контроллера (`CFileSystemControllerComp` локальный и `CRemoteFileSystemControllerComp`
редиректящий) реализуют контракт `IGqlRequestHandler`. Следствия:
- кросс-слойная зависимость: `imtclientgql::CRemoteFileSystemControllerComp` наследует
  `imtservergql::CGqlRequestHandlerCompBase`;
- дублирование разбора ответа (server собрал JSON → remote его разобрал и пересобрал);
- контроллер знает о двух способах доступа → не «закрыт для модификации» (OCP).

**Правильно (DIP):** удалённость — за доменным интерфейсом. Один контроллер зависит только от
`IFileSystemStructureProvider`; есть две реализации интерфейса — локальная (`QDir`) и удалённая
(форвардинг). Контроллер не знает, локальный доступ или удалённый.

**Безопасность (критично):**
- `CFileSystemControllerComp` не permissible → листинг ФС без проверки прав. Нужна модель прав
  (`CPermissibleGqlRequestHandlerComp` + permission-id, напр. `BrowseFileSystem`).
  Референс: `Include/imtservergql/CPermissibleGqlRequestHandlerComp.{h,cpp}`.
- Sandbox `RootPath` есть только у локального провайдера; remote-редирект форвардит `path` «как есть».
- `CRemoteFileSystemControllerComp` игнорирует `IUrlParam` в `SendRequest` (нет авторизации/таргетинга).
- Не задана политика симлинков / junction / `..` за пределами явного canonical-guard.

**ISP / OCP интерфейса:**
- `GetFileSystemEntries` с 4 out-параметрами — плохой контракт. Нужен результат-объект.
- Нет `Stat/Exists`, нет пагинации для больших каталогов.

**DRY:** имена полей (`name/path/isDir/size/lastModified`) продублированы строковыми литералами в
server-контроллере, remote-контроллере и QML. SDL — источник истины, но codegen не используется
(при том что остальной современный код Agentino работает на `sdl::V1_0::…` + `CXxxGqlHandlerCompBase`).

**Прочее качество:** нет тестов; фиксированный размер диалога; клиентская фильтрация `showFiles`
дублирует серверную; нет колонок size/date, сортировки, клавнавигации, состояния «пусто»,
локализации дат; нет таймаутов/отмены на remote; remote теряет детализацию ошибок.

---

## 3. Целевая архитектура

```
            ┌─────────────────────── Domain (imtfile) ───────────────────────┐
            │  IFileSystemStructureProvider  (read: Stat + GetEntries → результат-объект)
            │      ▲                    ▲                                       │
            │      │                    │                                       │
   CFileSystemStructureProviderComp   CRemoteFileSystemStructureProviderComp    │
        (локальная, QDir, sandbox)       (форвардинг через IGqlClient)          │
            └──────────────▲────────────────────────────────────────────────┘
                           │ зависит ТОЛЬКО от интерфейса (DIP)
              CFileSystemControllerComp  (permissible, типы из SDL codegen)
                           │
                     FileSystem.sdl  →  CFileSystemGqlHandlerCompBase (генерится)
```

- «Local» и «remote» — **две реализации доменного интерфейса**, а не два GQL-контроллера.
- `CRemoteFileSystemControllerComp` **удаляется**; вместо него — `CRemoteFileSystemStructureProviderComp`,
  реализующий `IFileSystemStructureProvider` и форвардящий через `IGqlClient`.
- Контроллер один, permissible, использует сгенерированные из SDL типы.
- В Agentino удалённость = маршрутизация на агента по заголовку `clientid` (переиспользуем
  существующий proxy-механизм из `CServiceControllerProxyComp`), а не отдельный fixed-ApiClient.

---

## 4. План рефакторинга ImtCore (фазы)

### Sub-0 — Проверка codegen (сделать ПЕРВЫМ, до правок кода)
- Убедиться, что генератор `imtsdl`/`imtsdlgen` обрабатывает `Sdl/imtbase/1.0/FileSystem.sdl` и
  генерирует серверную базу-обработчик `CFileSystemGqlHandlerCompBase` и типизированные
  `CGetFileSystemEntriesGqlRequest` / `CGetFileSystemEntriesPayload` (а не только клиентские хелперы).
- Схема уже включена в `Sdl/imtbase/CMake/CMakeLists.txt` (коммит-набросок). Проверить сборку
  сгенерированных файлов. Если серверная база не генерится — разобраться в конфигурации генератора
  (сравнить с тем, как это устроено для Agentino `Services.sdl`).
- **Гейт:** дальше не идти, пока не подтверждено, что нужные типы генерируются.

### Фаза 1 — SDL как источник истины
1. Расширить `Sdl/imtbase/1.0/FileSystem.sdl`:
   - `FileSystemEntry`: заменить голый `isDir: Boolean!` на `entryType` (enum `File | Dir | Symlink`),
     оставить `size`, `lastModified`.
   - Пагинация: во `GetFileSystemEntriesInput` добавить `offset: Int`, `limit: Int`;
     в `GetFileSystemEntriesPayload` — `totalCount: Int!`, `hasMore: Boolean!`.
   - **Никаких мутаций** (решение №1).
2. Пересобрать codegen, убедиться в наличии типов.

### Фаза 2 — Доменный слой (imtfile)
3. Переписать `Include/imtfile/IFileSystemStructureProvider.h` на результат-объект (убрать 4 out-параметра):
   ```cpp
   struct FileSystemEntry {
       QString name, path;
       enum class Type { File, Dir, Symlink } type = Type::File;
       qint64 size = 0;
       QDateTime lastModified;
   };
   struct FileSystemQuery { QString path; int offset = 0; int limit = 0; /* 0 = без лимита */ };
   struct FileSystemListing {
       QString resolvedPath, parentPath;
       QList<FileSystemEntry> entries;
       int totalCount = 0;
       bool hasMore = false;
   };
   virtual bool GetEntries(const FileSystemQuery& query,
                           FileSystemListing& result, QString& error) const = 0;
   virtual bool Stat(const QString& path, FileSystemEntry& entry, QString& error) const = 0;
   ```
   Никакого `IFileSystemModifier` сейчас не заводим.
4. Обновить `Include/imtfile/CFileSystemStructureProviderComp.{h,cpp}`:
   - Вынести sandbox в приватный `NormalizeAndAuthorize(path, &canonical, &error)` — единственная
     точка проверки (canonical внутри canonical(RootPath)), покрывается тестами.
   - Явная политика симлинков/`..`; заполнять `entryType`; реализовать пагинацию (`offset/limit`,
     `totalCount/hasMore`); пустой `path` → canonical(RootPath) (жёсткий единый root, решение №3).

### Фаза 3 — Транспорт/контроллер
5. Новый `Include/imtservergql/CFileSystemControllerComp.{h,cpp}`:
   - Наследовать `CPermissibleGqlRequestHandlerComp` (проверка прав).
   - Зависеть только от `IFileSystemStructureProvider` (атрибут `FileSystemProvider`).
   - Строить ответ через сгенерённый `CGetFileSystemEntriesPayload::WriteToJsonObject` (не руками).
   - Добавить permission-id (напр. `BrowseFileSystem`) — прописать в конфигах `CommandPermissions`.
6. Новый `Include/imtclientgql/CRemoteFileSystemStructureProviderComp.{h,cpp}`
   (или в `imtfile`, если по зависимостям чище): реализует `IFileSystemStructureProvider`,
   форвардит через `IGqlClient` (атрибут `ApiClient`), корректно прокидывает `IUrlParam`,
   таймаут, маппинг ошибок. Разбор ответа — через сгенерённые типы.
7. **Удалить** `Include/imtclientgql/CRemoteFileSystemControllerComp.{h,cpp}` и его регистрацию в
   `Impl/ImtClientGqlPck/ImtClientGqlPck.{h,cpp}` (typedef `RemoteFileSystemController` + запись в
   списке компонентов). Проверить, что больше нигде не регистрируется.
8. Обновить регистрации: `Impl/ImtFilePck/ImtFilePck.{h,cpp}`,
   `Impl/ImtServerGqlPck/ImtServerGqlPck.{h,cpp}`, и добавить typedef для нового remote-провайдера
   в соответствующий пакет.

### Фаза 4 — QML
9. `Qml/imtguigql/FileSystemBrowserDialog.qml`:
   - Убрать дублирующую клиентскую фильтрацию `showFiles` (фильтрует сервер).
   - Колонки size/date, сортировка, клавнавигация, состояние «пусто», относительные размеры
     (`Style.*`), локализация дат.
   - Учесть пагинацию (`offset/limit/hasMore`) при большом каталоге.
10. `Qml/imtguigql/ServerPathPicker.qml`:
    - Параметризовать: принимать `headersProvider`/target (чтобы адресовать конкретную машину/агента)
      и `pathKind` (executable/dir/file). Это точка расширения для Agentino.

### Фаза 5 — Тесты и документация
11. Unit-тесты `CFileSystemStructureProviderComp`: path-traversal (`..`, симлинк, абсолютный путь
    вне root), пагинация, пустой root, несуществующий путь.
12. Тест контроллера: отказ без прав, happy-path, проброс ошибки.
13. Обновить `Docs/FileSystem/FileSystemBrowser.md` под новую (single-controller) архитектуру и
    удалить упоминания `CRemoteFileSystemControllerComp`.

---

## 5. План интеграции в Agentino

**Суть:** `Path` сервиса указывает на исполняемый файл **на машине агента** (`Service.agentId`),
а не на центральном сервере. Значит браузинг должен идти на того агента, которому принадлежит
сервис, через тот же WS-канал `clientid`, что уже используют `LoadPlugin/GetService/StartService`
в `CServiceControllerProxyComp`.

### 5.1 Агентная сторона (agent)
- Захостить `CFileSystemControllerComp` + локальный `CFileSystemStructureProviderComp` в конфиге
  агента (там, где живут `agentgql`-контроллеры).
- `RootPath` агента = ОДНА разрешённая корневая директория (напр. каталог установки сервисов).
  Это влияет на UX: exe вне root через UI выбрать нельзя (решение №3) — согласовать значение root
  на этапе конфигурации агента (см. открытый вопрос в §7).
- Импортировать `imtbase/1.0/FileSystem.sdl` в агентную SDL-поверхность (по аналогии с импортом
  ImtCore-схем в `../Agentino/Sdl/agentino/1.0/Services.sdl`).

### 5.2 Серверная сторона (agentino)
- Добавить `CFileSystemControllerProxyComp` (по образцу
  `../Agentino/Include/agentinogql/CServiceControllerProxyComp.cpp`): принимает
  `GetFileSystemEntries`, читает `clientid`/`agentId` из заголовков и
  `SendModelRequest<CGetFileSystemEntriesPayload>()` форвардит на агента. **Отдельный компонент**
  (SRP), не расширять сервисный proxy.
- Права: проверять permission на браузинг (у пользователя должно быть право менять сервис/агента)
  до форвардинга.

### 5.3 Клиент (ServiceEditor)
- Заменить `pathInput` (`TextInputElementView` в `generalPageComp`,
  `../Agentino/Include/agentinoqml/Qml/ServiceEditor.qml`, строки ~443–466) на новый
  `ServerPathPickerElementView` — обёртку над `ServerPathPicker` в конвенции `*ElementView`,
  чтобы вписаться в `GroupElementView`/`getEditorControlWidth`. То же опционально для
  `startScript` / `stopScript` / `settingsPath`.
- Пробросить `agentId` в заголовки запроса браузинга: сейчас `getHeaders()` возвращает `{}`;
  для FS-браузинга вернуть `{ clientid: agentId }`. `agentId` для существующего сервиса известен
  из topology/serviceData.
- **Продуктовое ограничение:** для *нового* несохранённого сервиса агент может быть ещё не выбран →
  браузинг недоступен, пока агент не выбран. В текущем UX плагин и так грузится только после Save —
  применить ту же логику к пикеру: до выбора агента показывать подсказку/ручной ввод.

### 5.4 Обратная совместимость
- `ServiceData.path` остаётся `String` — меняется только способ ввода. Ручной ввод оставить как
  fallback (валидатор пути сохранить). Миграций данных не требуется.

---

## 6. Порядок работ (rollout)

1. **Sub-0** — проверка codegen `FileSystem.sdl` (быстрый build-эксперимент). Гейт.
2. ImtCore **Фаза 1–3** (SDL + domain + single permissible controller + remote provider).
   Ничего не ломает у потребителей, кроме удаления `CRemoteFileSystemControllerComp`
   (проверить, что его никто не регистрирует, кроме пакета).
3. ImtCore **Фаза 4–5** (параметризуемый QML-пикер + тесты + доки).
4. Agentino: **5.1** (агент хостит контроллер) → **5.2** (proxy по `clientid`) → **5.3** (UI).
   Каждая ступень проверяется e2e: сначала «browse через agentino на реальном агенте» в обход UI,
   затем UI.

---

## 7. Открытые вопросы (не блокируют архитектуру)

- **Значение `RootPath` на типичном агенте.** Поскольку root жёсткий, выбор exe ограничен им.
  Согласовать разумный дефолт (напр. каталог установки сервисов). Если пользователям реально нужен
  выбор с произвольного диска — это меняет решение №3 и требует отдельного согласования; текущий
  план исходит из жёсткого root + ручной ввод как fallback.

---

## Приложение A. Референс-компоненты (что читать перед стартом)

- Permissible-обработчик: `Include/imtservergql/CPermissibleGqlRequestHandlerComp.{h,cpp}`.
- База обработчика: `Include/imtservergql/CGqlRequestHandlerCompBase.{h,cpp}`.
- Клиентский GQL-контракт: `Include/imtclientgql/IGqlClient.h` (метод
  `SendRequest(GqlRequestPtr, IUrlParam* = nullptr)`).
- Форвардинг на агента (образец для proxy): `../Agentino/Include/agentinogql/CServiceControllerProxyComp.cpp`
  (`SendModelRequest<>()`, заголовок `clientid`, шаблон `CreateResponse<Req, Resp>`).
- Текущая (устаревающая) документация: `Docs/FileSystem/FileSystemBrowser.md`.

## Приложение B. Чек-лист «Definition of Done»

- [ ] `FileSystem.sdl`: enum `entryType` + пагинация; codegen даёт серверную базу и типы.
- [ ] `IFileSystemStructureProvider` — результат-объект + `Stat`, без out-параметров, без write-методов.
- [ ] Локальный провайдер: sandbox в `NormalizeAndAuthorize`, пагинация, entryType.
- [ ] Один permissible `CFileSystemControllerComp` на сгенерённых типах; permission-id заведён.
- [ ] `CRemoteFileSystemStructureProviderComp` реализует доменный интерфейс; `IUrlParam` прокинут.
- [ ] `CRemoteFileSystemControllerComp` удалён вместе с регистрацией.
- [ ] Пакеты (`ImtFilePck`/`ImtServerGqlPck`/`ImtClientGqlPck`) обновлены.
- [ ] QML: дубль-фильтрация убрана; пикер параметризуем заголовками/target.
- [ ] Тесты домена (path-traversal, пагинация) и контроллера (права) зелёные.
- [ ] Agentino: агент хостит контроллер; `CFileSystemControllerProxyComp` форвардит по `clientid`;
      `ServiceEditor` использует `ServerPathPickerElementView` с прокинутым `agentId`.
- [ ] `Docs/FileSystem/FileSystemBrowser.md` обновлён под single-controller.
