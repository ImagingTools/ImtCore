# Docker Test Infrastructure - Implementation Summary

## Цель (Goal)

Создать общую Docker инфраструктуру для тестов в ImtCore, которая будет использоваться приложениями (например, Lisa) для запуска тестов в контейнерах.

Create a common Docker test infrastructure in ImtCore that will be used by applications (e.g., Lisa) to run tests in containers.

## Структура (Structure)

```
Tests/Docker/
├── Dockerfile.linux          # Базовый Docker образ для Linux
├── docker-compose.linux.yml  # Docker Compose конфигурация
├── entrypoint.sh             # Скрипт запуска контейнера
├── .dockerignore             # Исключения для Docker build
├── Resources/                # Базовые ресурсы из ImtCore
│   ├── .gitkeep             # Сохраняет директорию в git
│   └── README.md            # Документация по ресурсам
├── Startup/                  # Базовые скрипты запуска из ImtCore
│   └── .gitkeep             # Сохраняет директорию в git
├── apps-README.md           # Документация для пользовательских приложений
└── README.md                # Основная документация
```

## Порядок выполнения (Execution Order)

### 1. Сборка Docker образа в ImtCore

```bash
cd ImtCore
docker build -f Tests/Docker/Dockerfile.linux -t imtcore-tests:linux .
```

### 2. Запуск тестов из Lisa (или другого приложения)

```bash
#!/bin/bash
# В репозитории Lisa создать скрипт run-tests-in-docker.sh

# Запустить контейнер в фоновом режиме
docker run -d \
  --name "lisa-tests" \
  --network host \
  -e BASE_URL="http://localhost:7776" \
  -e START_POSTGRESQL="true" \
  -e POSTGRES_DB="lisa_test" \
  imtcore-tests:linux \
  sleep infinity

# Скопировать тесты в контейнер
docker cp Tests/GUI lisa-tests:/app/tests/
docker cp Tests/API lisa-tests:/app/tests/

# Скопировать ресурсы (установщики, SQL, конфиги)
docker cp Tests/Docker/Resources/. lisa-tests:/app/custom-apps/resources/

# Скопировать скрипты запуска
docker cp Tests/Docker/Startup/. lisa-tests:/app/custom-apps/startup/
docker exec lisa-tests chmod +x /app/custom-apps/startup/*.sh

# Запустить тесты
docker exec lisa-tests npm test

# Скопировать результаты тестов
docker cp lisa-tests:/app/tests/test-results ./

# Очистка
docker stop lisa-tests
docker rm lisa-tests
```

## Директории (Directories)

### Resources/

**Назначение**: Файлы необходимые для установки приложений

**Содержимое**:
- Установщики: `.exe`, `.msi`, `.run`, `.deb`, `.rpm`, `.tar.gz`, `.zip`
- SQL скрипты: настройка БД, миграции, тестовые данные
- Конфигурационные файлы: `.json`, `.yaml`, `.xml`, `.ini`, `.conf`
- Файлы данных: тестовые данные, резервные копии

**Путь в контейнере**: `/app/custom-apps/resources/`

### Startup/

**Назначение**: Скрипты запуска приложений в контейнере

**Соглашения**:
- Использовать цифровые префиксы для контроля порядка: `01-`, `02-`, `03-`
- Делать скрипты исполняемыми: `chmod +x *.sh`
- Включать проверки готовности сервисов (health checks)

**Примеры**:
- `01-setup-database.sh` - Инициализация базы данных
- `02-start-application.sh` - Запуск приложения
- `03-wait-for-services.sh` - Ожидание готовности сервисов

**Путь в контейнере**: `/app/custom-apps/startup/`

## Последовательность запуска контейнера (Container Startup Sequence)

1. **PostgreSQL** (опционально, если `START_POSTGRESQL=true`)
   - Инициализация и запуск PostgreSQL
   - Создание базы данных, если указан `POSTGRES_DB`

2. **Startup Scripts** (из `/app/custom-apps/startup/`)
   - Выполняются в алфавитном порядке
   - Должны запускать приложения и ждать их готовности

3. **Tests** (из CMD или пользовательской команды)
   - Выполнение тестового набора

## Переменные окружения (Environment Variables)

### Настройки приложения
- `BASE_URL` - URL приложения для тестирования (по умолчанию: `http://localhost:3000`)
- `CI` - Индикатор CI режима (по умолчанию: `true`)

### Настройки PostgreSQL
- `START_POSTGRESQL` - Запустить PostgreSQL в контейнере (по умолчанию: `false`)
- `POSTGRES_DB` - Имя базы данных для создания (опционально)
- `DATABASE_URL` - Строка подключения к PostgreSQL (опционально)

### Пользовательские настройки
- `TEST_USERNAME`, `TEST_PASSWORD` - Учетные данные для тестов
- Другие специфичные для приложения переменные

## Пример использования из Lisa

В репозитории Lisa нужно создать:

### 1. Структура Tests/Docker/

```
Lisa/Tests/Docker/
├── Resources/           # Специфичные для Lisa ресурсы
│   ├── lisa-installer.exe
│   ├── setup.sql
│   └── config.json
└── Startup/            # Специфичные для Lisa скрипты
    ├── 01-setup-db.sh
    └── 02-start-lisa.sh
```

### 2. Скрипт запуска тестов

Создать `Lisa/run-tests-in-docker.sh` как показано выше.

### 3. GitHub Actions (опционально)

```yaml
name: Run Tests in Docker

on: [push, pull_request]

jobs:
  test:
    runs-on: ubuntu-latest
    steps:
      - name: Checkout ImtCore
        uses: actions/checkout@v3
        with:
          repository: ImagingTools/ImtCore
          path: ImtCore

      - name: Checkout Lisa
        uses: actions/checkout@v3
        with:
          path: Lisa

      - name: Build Docker Image
        run: |
          cd ImtCore
          docker build -f Tests/Docker/Dockerfile.linux -t imtcore-tests:linux .

      - name: Run Tests
        run: |
          cd Lisa
          chmod +x run-tests-in-docker.sh
          ./run-tests-in-docker.sh

      - name: Upload Test Results
        if: always()
        uses: actions/upload-artifact@v3
        with:
          name: test-results
          path: Lisa/test-results/
```

## Преимущества (Benefits)

1. **Единая инфраструктура**: Все приложения используют один и тот же базовый Docker образ
2. **Изоляция**: Тесты выполняются в чистом окружении
3. **Воспроизводимость**: Одинаковое окружение на всех машинах
4. **Гибкость**: Каждое приложение может добавлять свои ресурсы и скрипты
5. **Масштабируемость**: Легко добавить новые приложения

## Документация

- [Tests/Docker/README.md](Tests/Docker/README.md) - Основная документация
- [Tests/Docker/apps-README.md](Tests/Docker/apps-README.md) - Документация по пользовательским приложениям
- [Tests/Docker/Resources/README.md](Tests/Docker/Resources/README.md) - Документация по ресурсам

## Следующие шаги для Lisa

1. Создать структуру `Tests/Docker/Resources/` и `Tests/Docker/Startup/` в репозитории Lisa
2. Добавить специфичные для Lisa ресурсы (установщики, SQL скрипты)
3. Создать скрипты запуска Lisa в `Tests/Docker/Startup/`
4. Создать скрипт `run-tests-in-docker.sh` для запуска тестов
5. Протестировать весь процесс локально
6. Интегрировать с CI/CD (GitHub Actions)
