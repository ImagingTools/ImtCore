# Инициализаторы приложений ImtCore

## 1. Назначение

Документ описывает упрощенную макро-модель инициализации приложений ImtCore.

Модель ориентирована на команды прикладной разработки: вместо большого количества микро-инициализаторов используются готовые крупные профили запуска.

## 2. Базовый API

- Статические функции инициализации в пространстве имен `imtcore`
- `imtcore::CApplicationRunner::Run(..., autoInit)` для запуска приложения

## 3. Макро-профили

### 3.1 Сервер с авторизацией

Функция:
- `imtcore::InitializeImtCoreServerAuth()`

Включает:
- Локализацию
- Базовый server/core слой
- Auth-домен

Когда использовать:
- Сервис требует auth-доменную логику
- Лицензионный домен не обязателен по умолчанию

### 3.2 Сервер с лицензиями

Функция:
- `imtcore::InitializeImtCoreServerLic()`

Включает:
- Локализацию
- Базовый server/core слой
- Lic-домен

Когда использовать:
- Сервис ориентирован на лицензионные сценарии
- Авторизация опциональна или вынесена наружу

### 3.3 Сервер с авторизацией и лицензиями

Функция:
- `imtcore::InitializeImtCoreServerAuthLic()`

Включает:
- Локализацию
- Базовый server/core слой
- Auth-домен
- Lic-домен

Когда использовать:
- И авторизация, и лицензирование являются ключевыми требованиями сервиса

### 3.4 Профиль клиентского приложения

Функция:
- `imtcore::InitializeImtCoreClientApp()`

Включает:
- Локализацию
- Настройку стиля/UI
- Base-домен
- Auth-домен
- Desk-домен
- Lic-домен

Когда использовать:
- Desktop/QML клиентские приложения
- Полнофункциональные UI-приложения

### 3.5 Обратносуместимый профиль по умолчанию

Функция:
- `imtcore::InitializeDefaultImtCoreQml()`

Поведение:
- Делегирует в `InitializeImtCoreClientApp()`

Когда использовать:
- Нужна стандартная точка входа полного QML-профиля

## 4. Примеры использования

### 4.1 Сервер с авторизацией

```cpp
#include <imtcore/CApplicationRunner.h>
#include <imtcore/CImtCoreServerAuthInitializer.h>

imtcore::InitializeImtCoreServerAuth();
return imtcore::CApplicationRunner::Run(argc, argv, appComponent, true);
```

### 4.2 Сервер с лицензиями

```cpp
#include <imtcore/CApplicationRunner.h>
#include <imtcore/CImtCoreServerLicInitializer.h>

imtcore::InitializeImtCoreServerLic();
return imtcore::CApplicationRunner::Run(argc, argv, appComponent, true);
```

### 4.3 Сервер с авторизацией и лицензиями

```cpp
#include <imtcore/CApplicationRunner.h>
#include <imtcore/CImtCoreServerAuthLicInitializer.h>

imtcore::InitializeImtCoreServerAuthLic();
return imtcore::CApplicationRunner::Run(argc, argv, appComponent, true);
```

### 4.4 Клиентское приложение

```cpp
#include <imtcore/CApplicationRunner.h>
#include <imtcore/CImtCoreClientAppInitializer.h>

imtcore::InitializeImtCoreClientApp();
return imtcore::CApplicationRunner::Run(argc, argv, appComponent, true);
```

## 5. Как выбрать профиль

- `InitializeImtCoreServerAuth()` для auth-ориентированных серверов.
- `InitializeImtCoreServerLic()` для lic-ориентированных серверов.
- `InitializeImtCoreServerAuthLic()`, когда нужны оба домена.
- `InitializeImtCoreClientApp()` для клиентских/UI приложений.
- `InitializeDefaultImtCoreQml()` как стандартная точка входа полного профиля.
