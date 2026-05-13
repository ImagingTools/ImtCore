# SDL Schema Documentation - Index
# Документация SDL схем - Индекс

Complete guide to SDL schema development in ImtCore, focusing on the `ref` attribute and collection schema patterns.

Полное руководство по разработке SDL схем в ImtCore с фокусом на атрибут `ref` и паттерны схем коллекций.

---

## 📚 Documentation Overview / Обзор документации

This documentation set provides comprehensive coverage of SDL schema development in ImtCore, including the `collectionSchema` block, `ref` attribute, and collection patterns, from basic concepts to advanced patterns.

Этот набор документации обеспечивает полное покрытие разработки SDL схем в ImtCore, включая блок `collectionSchema`, атрибут `ref` и паттерны коллекций, от базовых концепций до продвинутых паттернов.

**Total Size / Общий размер**: ~100KB of documentation / ~100КБ документации  
**Languages / Языки**: English + Russian (bilingual) / Английский + Русский (двуязычная)  
**Last Updated / Последнее обновление**: 2026-02-13

---

## 🎯 Quick Start / Быстрый старт

### For Beginners / Для начинающих
Start here if you're new to SDL schemas:
Начните здесь, если вы новичок в SDL схемах:

0. **[CollectionSchema Guide](./SDL_CollectionSchema_Guide.md)** (27KB) 🌟 **NEW! / НОВОЕ!**
   - What is collectionSchema? / Что такое collectionSchema?
   - Complete bilingual guide (English + Russian)
   - Basic syntax and components
   - Design patterns
   - Code generation
   - Real-world examples

1. **[Cheat Sheet](./SDL_Ref_Cheat_Sheet.md)** (7KB) ⭐ **START HERE**
   - Quick template
   - Basic rules
   - Common patterns
   - Checklist

2. **[Quick Reference](./SDL_Ref_Quick_Reference.md)** (3KB)
   - Basic syntax
   - Simple example
   - Key points
   - Common errors

### For Intermediate Users / Для продвинутых пользователей
For those familiar with SDL basics:
Для тех, кто знаком с основами SDL:

3. **[Visual Diagrams](./SDL_Ref_Diagrams.md)** (21KB)
   - 8 ASCII art diagrams
   - Data flow visualization
   - Error scenarios
   - Complete workflow

4. **[Practical Examples](./SDL_Ref_Examples.md)** (23KB)
   - 5 real-world examples
   - Use case scenarios
   - Best practices
   - Testing checklist

### For Advanced Users / Для экспертов
Deep dive into SDL architecture:
Глубокое погружение в архитектуру SDL:

5. **[Complete Reference Guide](./SDL_Reference_Attribute_Guide.md)** (21KB) 🇷🇺
   - Comprehensive explanation
   - Internal processing
   - Code generation
   - Advanced scenarios

---

## 📖 Documentation Files / Файлы документации

### 0. SDL_CollectionSchema_Guide.md 🌟 NEW!
**Size / Размер**: 27KB  
**Language / Язык**: English + Russian (Bilingual / Двуязычный)  
**Level / Уровень**: Beginner to Advanced / От начинающего до продвинутого  

**Complete bilingual guide to `collectionSchema` in SDL.**

**Полное двуязычное руководство по `collectionSchema` в SDL.**

**Contains / Содержит:**
- ✅ What is collectionSchema? / Что такое collectionSchema?
- ✅ Purpose and role / Назначение и роль
- ✅ Basic syntax / Базовый синтаксис
- ✅ Key components (type, ref, operations, subtype)
- ✅ Complete examples with explanations
- ✅ Design patterns (List+Details, Hierarchical, Nested)
- ✅ Code generation details (C++ and QML)
- ✅ Operation mapping table
- ✅ Requirements and rules
- ✅ Common errors and solutions
- ✅ Best practices
- ✅ Real-world examples from codebase

**Best for / Лучше всего для:**
- Understanding the overall collectionSchema concept
- Learning SDL collection architecture
- Getting started with SDL development
- Reference for both beginners and experts
- Понимание концепции collectionSchema
- Изучение архитектуры коллекций SDL

**[→ Open CollectionSchema Guide](./SDL_CollectionSchema_Guide.md)**

---

### 1. SDL_Ref_Cheat_Sheet.md
**Size / Размер**: 7KB  
**Language / Язык**: English + Russian  
**Level / Уровень**: Beginner / Начинающий  

Quick reference card for daily development. Print and keep it handy!

Быстрая справочная карточка для ежедневной разработки. Распечатайте и держите под рукой!

**Contains / Содержит:**
- ✅ Ready-to-use template
- ✅ Quick rules table
- ✅ 4 common patterns
- ✅ 3 typical mistakes
- ✅ Development checklist
- ✅ Naming conventions
- ✅ CLI commands

**Best for / Лучше всего для:**
- Creating new SDL files
- Quick syntax lookup
- Copy-paste templates

**[→ Open Cheat Sheet](./SDL_Ref_Cheat_Sheet.md)**

---

### 2. SDL_Ref_Quick_Reference.md
**Size / Размер**: 3KB  
**Language / Язык**: English  
**Level / Уровень**: Beginner / Начинающий  

Brief overview of `ref` attribute essentials.

Краткий обзор основ атрибута `ref`.

**Contains / Содержит:**
- ✅ Quick summary
- ✅ Syntax explanation
- ✅ Basic example
- ✅ Key points (4 rules)
- ✅ Common pattern
- ✅ Operation mapping table
- ✅ Error solutions

**Best for / Лучше всего для:**
- Learning `ref` basics
- Understanding syntax
- Quick reference during development

**[→ Open Quick Reference](./SDL_Ref_Quick_Reference.md)**

---

### 3. SDL_Ref_Diagrams.md
**Size / Размер**: 21KB  
**Language / Язык**: English + Russian  
**Level / Уровень**: Intermediate / Средний  

Visual representation of `ref` attribute concepts and workflows.

Визуальное представление концепций и процессов атрибута `ref`.

**Contains / Содержит:**
- ✅ 8 ASCII art diagrams:
  1. Basic ref structure
  2. Data flow
  3. Parser processing
  4. Type hierarchy
  5. List vs Detail pattern
  6. Error scenarios
  7. Union types
  8. Code generation flow
- ✅ Complete workflow diagram
- ✅ Best practices visualization
- ✅ DO's and DON'Ts

**Best for / Лучше всего для:**
- Visual learners
- Understanding data flow
- Debugging issues
- Architecture comprehension

**[→ Open Diagrams](./SDL_Ref_Diagrams.md)**

---

### 4. SDL_Ref_Examples.md
**Size / Размер**: 23KB  
**Language / Язык**: English + Russian  
**Level / Уровень**: Intermediate to Advanced / Средний - Продвинутый  

Practical, real-world examples of `ref` attribute usage.

Практические примеры использования атрибута `ref` из реального мира.

**Contains / Содержит:**
- ✅ 5 Complete examples:
  1. **Blog Posts** - Simple collection
  2. **File System** - Hierarchical collection
  3. **E-commerce Orders** - Nested objects
  4. **Media Library** - Union types
  5. **Task Manager** - Complex real-world
- ✅ Best practices demonstration
- ✅ Testing checklist
- ✅ Common mistakes with fixes
- ✅ Full GraphQL schemas

**Best for / Лучше всего для:**
- Learning by example
- Copy-paste starting points
- Understanding patterns
- Real-world scenarios

**[→ Open Examples](./SDL_Ref_Examples.md)**

---

### 5. SDL_Reference_Attribute_Guide.md
**Size / Размер**: 21KB  
**Language / Язык**: Russian (Русский) 🇷🇺  
**Level / Уровень**: All levels / Все уровни  

Comprehensive, authoritative guide to the `ref` attribute.

Полное, авторитетное руководство по атрибуту `ref`.

**Contains / Содержит:**
- ✅ Complete definition and purpose
- ✅ Internal parser processing (with code)
- ✅ 4 real codebase examples
- ✅ Typical patterns:
  - List + Details
  - Hierarchical data
- ✅ Type requirements
- ✅ Code generation explanation
- ✅ Operation mapping
- ✅ 3 common errors with solutions
- ✅ Best practices:
  - Naming conventions
  - Field optimization
  - Documentation
- ✅ Advanced scenarios:
  - Union types
  - Type imports
- ✅ Code references

**Best for / Лучше всего для:**
- Deep understanding
- Reference during development
- Learning architecture
- Troubleshooting

**[→ Открыть полное руководство](./SDL_Reference_Attribute_Guide.md)**

---

## 🗺️ Learning Path / Путь обучения

### Path 1: Quick Start (30 minutes)
**Быстрый старт (30 минут)**

1. Read [CollectionSchema Guide](./SDL_CollectionSchema_Guide.md) - Introduction sections (10 min)
2. Read [Cheat Sheet](./SDL_Ref_Cheat_Sheet.md) (5 min)
3. Look at [Quick Reference](./SDL_Ref_Quick_Reference.md) (5 min)
4. Create your first SDL file (10 min)

### Path 2: Comprehensive Learning (2-3 hours)
**Полное изучение (2-3 часа)**

1. Read [CollectionSchema Guide](./SDL_CollectionSchema_Guide.md) completely (45 min)
2. Read [Quick Reference](./SDL_Ref_Quick_Reference.md) (10 min)
3. Study [Visual Diagrams](./SDL_Ref_Diagrams.md) (30 min)
4. Work through all [Examples](./SDL_Ref_Examples.md) (60 min)
5. Skim [Complete Ref Guide](./SDL_Reference_Attribute_Guide.md) (20 min)

### Path 3: Expert Deep Dive (4+ hours)
**Экспертное погружение (4+ часа)**

1. Read [CollectionSchema Guide](./SDL_CollectionSchema_Guide.md) (60 min)
2. Read [Complete Ref Guide](./SDL_Reference_Attribute_Guide.md) (60 min)
3. Study [Visual Diagrams](./SDL_Ref_Diagrams.md) (45 min)
4. Analyze all [Examples](./SDL_Ref_Examples.md) (90 min)
5. Examine existing SDL files in codebase (45 min)

---

## 🎓 Use Cases / Сценарии использования

### I need to... / Мне нужно...

**Understand what collectionSchema is**
**Понять что такое collectionSchema**
→ Start with [CollectionSchema Guide](./SDL_CollectionSchema_Guide.md)

**Create a new collection**
**Создать новую коллекцию**
→ Use [Cheat Sheet](./SDL_Ref_Cheat_Sheet.md) template and [CollectionSchema Guide](./SDL_CollectionSchema_Guide.md)

**Understand how ref works**
**Понять как работает ref**
→ Read [Quick Reference](./SDL_Ref_Quick_Reference.md) and [Diagrams](./SDL_Ref_Diagrams.md)

**Fix a ref error**
**Исправить ошибку ref**
→ Check error scenarios in [Diagrams](./SDL_Ref_Diagrams.md) or [CollectionSchema Guide](./SDL_CollectionSchema_Guide.md)

**Implement a specific pattern**
**Реализовать определенный паттерн**
→ Find similar example in [Examples](./SDL_Ref_Examples.md) or [CollectionSchema Guide](./SDL_CollectionSchema_Guide.md)

**Learn SDL architecture**
**Изучить архитектуру SDL**
→ Study [CollectionSchema Guide](./SDL_CollectionSchema_Guide.md) and [Complete Ref Guide](./SDL_Reference_Attribute_Guide.md)

**Quick syntax lookup**
**Быстрая справка по синтаксису**
→ Use [Cheat Sheet](./SDL_Ref_Cheat_Sheet.md)

---

## 📊 Documentation Coverage / Охват документации

### Topics Covered / Охваченные темы

| Topic / Тема | Coverage / Охват |
|--------------|------------------|
| CollectionSchema concept | ████████████████████ 100% |
| Basic ref syntax | ████████████████████ 100% |
| Type requirements | ████████████████████ 100% |
| Common patterns | ████████████████████ 100% |
| Error handling | ████████████████████ 100% |
| Best practices | ████████████████████ 100% |
| Code generation | ████████████████████ 100% |
| Visual diagrams | ████████████████████ 100% |
| Real examples | ████████████████████ 100% |
| Union types | ████████████████████ 100% |
| Type imports | ████████████████████ 100% |

### Examples by Complexity / Примеры по сложности

- **Simple**: 2 examples (Blog Posts, File System)
- **Medium**: 2 examples (E-commerce Orders, Media Library)
- **Complex**: 1 example (Task Manager)

---

## 🔗 Related Resources / Связанные ресурсы

### Source Code / Исходный код

- **Parser**: `Include/imtsdl/CGqlExtSchemaParser.cpp:136-178`
- **Document Type**: `Include/imtsdl/CSdlDocumentType.h`
- **Generators**: `Include/imtsdlgencpp/`, `Include/imtsdlgenqml/`

### Example SDL Files / Примеры SDL файлов

- `Sdl/imtauth/1.0/Users.sdl` - Users collection
- `Sdl/imtauth/1.0/Groups.sdl` - Groups with hierarchy
- `Sdl/imtauth/1.0/Roles.sdl` - Roles management
- `Sdl/imtlic/1.0/Licenses.sdl` - License definitions
- `Sdl/imtlic/1.0/Products.sdl` - Products with nested data

### Testing / Тестирование

- `Include/imtsdlgencpp/Test/TestData/*.sdl` - Test SDL files
- `Include/imtsdlgenqml/Test/TestData/*.sdl` - QML test files

---

## 🛠️ Tools & Commands / Инструменты и команды

### Find SDL Files
```bash
# Find all SDL files
find Sdl -name "*.sdl"

# Find SDL files with collectionSchema
find Sdl -name "*.sdl" -exec grep -l "collectionSchema" {} \;

# Count ref usages
grep -r "ref:" Sdl --include="*.sdl" | wc -l
```

### View Examples
```bash
# View user collection
cat Sdl/imtauth/1.0/Users.sdl

# View license collection
cat Sdl/imtlic/1.0/Licenses.sdl

# Search for specific pattern
grep -A5 "ref:" Sdl/imtauth/1.0/*.sdl
```

### Generate Code
```bash
# Run SDL code generator (example)
# Запустить генератор кода SDL (пример)
./ImtSdlGenCpp --schema Sdl/mymodule/1.0/MySchema.sdl
```

---

## 📝 Documentation Standards / Стандарты документации

This documentation follows these principles:
Эта документация следует следующим принципам:

- ✅ **Example-driven**: Every concept has examples
- ✅ **Visual**: Diagrams for complex concepts
- ✅ **Practical**: Real-world use cases
- ✅ **Multilingual**: Russian + English
- ✅ **Cross-referenced**: Easy navigation
- ✅ **Progressive**: Beginner to advanced
- ✅ **Searchable**: Clear structure

---

## 🔄 Documentation Updates / Обновления документации

### Version History / История версий

- **2026-02-13 (Update 2)**: Added comprehensive collectionSchema documentation
  - Created SDL_CollectionSchema_Guide.md (27KB)
  - Complete bilingual guide (English + Russian)
  - Updated documentation index
  - Total 100KB of content now

- **2026-02-13 (Update 1)**: Initial documentation release
  - Created 5 documentation files
  - Total 70KB of content
  - 12 SDL examples
  - 8 visual diagrams

### Future Plans / Планы на будущее

- [ ] Video tutorials
- [ ] Interactive examples
- [ ] SDL validator tool
- [ ] More advanced patterns
- [ ] Performance optimization guide

---

## 💬 Feedback & Contributions / Обратная связь и вклад

Found an error or have a suggestion?  
Нашли ошибку или есть предложение?

1. Check if topic is already covered
2. Review existing examples
3. Open an issue or PR
4. Contact maintainers

---

## 📞 Getting Help / Получение помощи

### When Stuck / Если застряли

1. **Check Cheat Sheet** - Quick syntax reference
2. **Review Examples** - Find similar use case
3. **Study Diagrams** - Understand flow
4. **Read Complete Guide** - Deep dive
5. **Examine Source Code** - See implementation
6. **Ask Team** - Get help from colleagues

### Common Questions / Частые вопросы

**Q: Where do I start?**  
**В: С чего начать?**  
A: Start with [CollectionSchema Guide](./SDL_CollectionSchema_Guide.md) or [Cheat Sheet](./SDL_Ref_Cheat_Sheet.md)

**Q: What is collectionSchema?**  
**В: Что такое collectionSchema?**  
A: See [CollectionSchema Guide](./SDL_CollectionSchema_Guide.md) for complete explanation

**Q: What if ref type not found?**  
**В: Что если ref тип не найден?**  
A: Define type before collectionSchema. See [Diagrams](./SDL_Ref_Diagrams.md) Error Scenario 1

**Q: How to use Union types?**  
**В: Как использовать Union типы?**  
A: See Example 4 in [Examples](./SDL_Ref_Examples.md)

**Q: What are required fields?**  
**В: Какие поля обязательны?**  
A: `id: ID!` and `typeId: ID!`. See [Quick Reference](./SDL_Ref_Quick_Reference.md)

---

## 🎯 Summary / Резюме

This documentation provides everything you need to work with SDL schemas in ImtCore, including `collectionSchema` blocks and the `ref` attribute:

Эта документация предоставляет все необходимое для работы с SDL схемами в ImtCore, включая блоки `collectionSchema` и атрибут `ref`:

- ✅ 6 comprehensive documents
- ✅ 100KB of content
- ✅ Complete bilingual collectionSchema guide
- ✅ 12+ SDL examples
- ✅ 8 visual diagrams
- ✅ Beginner to advanced coverage
- ✅ English + Russian support
- ✅ English + Russian
- ✅ Real codebase examples

**Start your journey here:**  
**Начните свой путь здесь:**

→ [Cheat Sheet](./SDL_Ref_Cheat_Sheet.md) for quick start  
→ [Complete Guide](./SDL_Reference_Attribute_Guide.md) for deep dive  
→ [Examples](./SDL_Ref_Examples.md) for learning by doing

---

**Happy SDL Development! / Удачной разработки SDL!** 🚀
