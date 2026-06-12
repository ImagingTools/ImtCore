# JqmlDesktopHost

Минимальный desktop-host для QML тестов JQML.

Назначение:
- зарегистрировать `com.imtcore.imtqml` типы в рантайме;
- подложить типовые import-path;
- загрузить тестовый `Main.qml` с диска.

## Сборка

Обычная CMake/Qt сборка из этой папки.

## Запуск

- `JqmlDesktopHost.exe --qml-file <path-to-Main.qml>`
- или `JqmlDesktopHost.exe <path-to-Main.qml>`

Дополнительные import-path:

- `JqmlDesktopHost.exe -I <path1> -I <path2> <qmlFile>`

## Интеграция в тест-раннер JQML

В корне `D:\IT\ImtCore\Tools\JQML\v3` перед запуском тестов:

- `IMT_DESKTOP_RUNNER=<full-path-to-JqmlDesktopHost.exe>`
- `IMT_DESKTOP_RUNNER_ARGS_JSON=["{QML_FILE}"]`

После этого `tests/index.js` будет запускать desktop тесты через host-приложение.
