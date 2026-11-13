# Генерация C++ из SDL: файлы `<Schema>Objects`

English version: [SDL_Objects_Generation_en.md](SDL_Objects_Generation_en.md)

Документ описывает, как генератор SDL разделяет C++ код на файлы и как отключить часть на основе QObject (`NO_OBJECTS`).

## 1. Генерируемые файлы

Для схемы `Foo` генератор создаёт:

| Файл | Содержимое |
|------|------------|
| `Foo.h`, `Foo.cpp` | Простые классы данных (`CType`), enum (`Q_NAMESPACE`, `Q_ENUM_NS`, обёртки `EnumXxx`), сериализация, запросы |
| `Foo_fwd.h` | Предварительные объявления |
| `FooObjects.h` | Классы на основе QObject: `CTypeObject`, `CTypeObjectList`, объекты union, `Register<Schema>QmlTypes()` |
| `moc_Foo.cpp`, `moc_FooObjects.cpp` | Вывод moc, создаётся системой сборки (CMake) |

`FooObjects.cpp` — **промежуточный** файл: пост-процессор дописывает его содержимое в `Foo.cpp`, после чего файл удаляется (и при успехе, и при ошибке).
Он не является выходом сборки.

С опцией `--include-moc` (её всегда передаёт `ImtCoreCustomConfigureSdlCpp`) в конец `Foo.cpp` подключается вывод moc:

```cpp
#include "moc_Foo.cpp"         // всегда: метаобъекты основного заголовка
#include "moc_FooObjects.cpp"  // только если Objects генерируются
```

Таким образом, на каждую схему приходится одна единица трансляции, а файлы moc отдельно не компилируются.

## 2. Отключение Objects

Классы QObject нужны только для использования типов SDL из QML **через модели C++** (такие классы регистрирует в QML код приложения, см. `Register<Schema>QmlTypes()`). QML-часть генератора их не использует: она создаёт собственные QML-типы (файлы `.qml` и `qmldir`) независимо от классов C++. Поэтому схему, которая используется из C++ без этих моделей или только из QML, можно генерировать без Objects.

### CMake

```cmake
ImtCoreCustomConfigureSdlCpp(
	NO_OBJECTS
	SCHEMA_PATH "${CMAKE_CURRENT_LIST_DIR}/../1.0/Foo.sdl"
	VERSION "1.0")

# тот же флаг у макроса 'CppQml' (только C++)
ImtCoreCustomConfigureSdlCppQml(CXX NO_OBJECTS
	SCHEMA_PATH "${CMAKE_CURRENT_LIST_DIR}/../1.0/Foo.sdl"
	VERSION "1.0")
```

* `NO_OBJECTS` допустим вместе с `QML`: он влияет только на генератор C++, генератор QML не меняется. Это случай приложения, целиком написанного на QML.
* Генератор получает `--no-objects`; `FooObjects.h` не создаётся, не объявляется выходом сборки, и moc для него не генерируется.

### Командная строка

| Опция | Назначение |
|-------|------------|
| `--no-objects` | Не генерировать файлы `<Schema>Objects` |
| `--include-moc` | Подключить вывод moc в конце сгенерированного исходного файла |

## 3. Правила и ограничения

1. **Импорты.** Objects схемы подключают Objects импортируемых схем (`BarObjects.h`). Поэтому схема **с** Objects не может импортировать схему, сгенерированную **без** Objects. Схема без Objects может импортировать любую схему.

   Генератор не может знать, как была сгенерирована импортируемая схема (она может принадлежать другому проекту и собираться отдельно). Поэтому проверку выполняет компилятор: каждый Objects-заголовок импортируемой схемы подключается в защищённом виде

   ```cpp
   #if __has_include(<barsdl/SDL/1.0/CPP/BarObjects.h>)
   #include <barsdl/SDL/1.0/CPP/BarObjects.h>
   #else
   #error The objects header <barsdl/SDL/1.0/CPP/BarObjects.h> is not found. The schema, that defines the imported types, is probably generated without objects (NO_OBJECTS). ...
   #endif
   ```

   и вместо «file not found» компиляция завершается ошибкой с указанием причины. Защита не применяется к собственным библиотекам (`imtbase`, `imtsdl`).
2. **Один флаг для схем проекта, которые импортируют друг друга.** Например, если `Rs485.sdl` импортирует `FrameStructure.sdl`, то `NO_OBJECTS` должен быть либо у обеих схем, либо ни у одной.
3. **Коллизии выходных файлов.** `FooObjects.h` — это Objects-заголовок схемы `Foo` и основной заголовок схемы с именем `FooObjects`. Если две схемы порождают один и тот же файл, CMake сообщает об ошибке `SDL output file collision`. Не называйте схему `<Другая>Objects`.
4. **Суффикс `Objects`** задан в двух местах, значения должны совпадать: `ISdlProcessArgumentsParser::s_objectsFileSuffix` (C++) и `IMT_SDL_OBJECTS_SUFFIX` (`ImtCoreSdlCustomConfig.cmake`).
5. **QMake.** Интеграция SDL для QMake (`Config/QMake/SdlConfiguration.pri`) не поддерживает Objects: `<Schema>Objects.h` не обрабатывается moc, режим `--cpp-mode=IMPL` не поддерживается.

## 4. Что изменилось для кода, использующего SDL

* `C<Type>Object`, `C<Type>ObjectList`, объекты union и `Register<Schema>QmlTypes()` перенесены из `Foo.h` в `FooObjects.h`. Код, который ими пользуется, должен подключать `FooObjects.h`.
* `Foo.h` больше не подключает `<imtsdl/TListModelBase.h>` и `<imtbase/CItemModelBase.h>`. Если они используются, подключайте их напрямую.
* Проект, который сам компилирует сгенерированные исходники (не через `ImtCoreCustomConfigureSdlCpp`), должен передавать `--include-moc` и создавать файлы moc рядом со сгенерированным исходником. Иначе классы QObject не слинкуются.

## 5. Тесты

| Тест | Что проверяет | Запуск |
|------|---------------|--------|
| `ImtSdlGenTest` | Генератор: файлы, подключение moc, depfile, пути целевых файлов, защищённые include импортируемых Objects (в том числе компилятором), удаление промежуточного файла | `ImtSdlGenTest` (нужна переменная `IMTCOREDIR`) |
| `ImtSdlGenIntegrationTest` | Сгенерированный код вместе с настоящими библиотеками, включая `NO_OBJECTS` (`NoObjects.sdl`, `NoObjectsConsumer.sdl`) | Опция CMake `IMT_BUILD_SDL_INTEGRATION_TESTS=ON` (требуются `BUILD_TESTING`, `USE_QTDB`, `IMT_BUILD_TOOLS_ONLY=OFF`) |
| `ImtSdlCMakeMacros` | Макросы CMake: проброс `NO_OBJECTS` (генератор QML не затрагивается), независимость флагов схем, коллизии выходных файлов | `ctest --test-dir <каталог сборки ImtSdlGenIntegrationTest>` (нужен `ninja`) |
