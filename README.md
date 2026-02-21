*[Информация на русском языке](#rus)*


# 🌟 Cossacks: Back to War 1.52

If you love classic strategy games, Cossacks: Back to War is a genre classic. However, it may not work properly on modern computers. The 1.52 update by MR.CODERMAN addresses this issue by improving the game's stability and compatibility. 🌍
#  🔧 What’s New in Version 1.52?

Full Compatibility with Windows, macOS, and Linux The game now runs smoothly on all modern platforms without crashes, delivering stable performance.
Removed 1920x1080 Resolution Limit You can now enjoy the game in high resolutions with no screen size restrictions. 🚀

Improved Graphics and Interface Updated textures and UI make the game easier on the eyes, adding vibrancy and enhancing the visual experience. 🎨

**Vertical synchronization has been enabled by default.**

**Fixed Water and Nation Color Bugs Visual issues related to water and nation colors have been corrected — everything now looks just like in the original game.**

**The ability to target buildings through the fog of war has been restored, preserving familiar online game tactics and enhancing gameplay.**

**In the Diplomatic Center, you can now order multiple infinities of units of the same type at once—for example, three infinities of infantry and one infinity of archers—so that the output will be three infantry units and one archer. (Thanks for the idea and testing by [slovnoslon](https://www.youtube.com/@Slovnoslon) )**

**The issue with the "Great Road" mission has been fixed. It is now fully passable.**


**AI has been improved (AI Pack). Bots are now much more challenging.**

**The issue with music playback in the game has been resolved.**


# ** Added support for 8 players in random map mode. **

Color — Brown.

The game resources do not fully support an 8th nation on the map, so there are limitations:

    • The brown player must be an ally of one of the existing players.

    • Brown peasants will spawn near their ally.

    • If no ally is specified, the brown player will be unable to build structures and will be unplayable.

 The ally can be either a player or an AI.
  

# Integrated Mods:
**Mod 1**

• Adds around 100 new missions (16th–18th centuries);

• Introduces 4 difficulty levels for single-player mode;

• Adds 2 new nations — Switzerland and Hungary (unique architecture and ~4 unique units each);

• Introduces a new unit — Camel Bedouin (available for Turkey and Algeria).


**Reloaded**

• Replaces original unit and building models with assets from American Conquest: Fight Back and Cossacks II;

• Includes visual upgrades and increased detail.

**Ultimate Pack**

• Combines all single-player content from European Wars and The Art of War into Back to War;

• Adds a total of 117 single-player missions, including the Over the Horizon campaign.

 **Includes maps for both multiplayer and single-player modes.**
 <hr>

**Real War mod created by [-TC-]Atabey (mmuslu33@gmail.com)**

The mod replaces the game's original files, so it is installed separately — simply download the archive from this [link](https://archive.org/download/cossacks-back-to-war-v-1.52-2025_202506/Cossacks%20Real%20War.zip), extract it to the game folder, and run RealwarRun.bat.


Multiplayer and random map mode are supported; story missions are not available. For any questions or suggestions, contact the mod author directly.
<hr>

 **Missions Pack**
 

• Adds missions by the developer [Ivan Petkov](https://github.com/MisterCoderman/cossacks-2025/issues/35)

• Includes 1 campaign and 3 standalone missions.

Campaign:

• Glory of Ushakov

Standalone missions:

• Battle of Kolin

• Battle of Naseby

• Battle of Rocroi



#  🧠 Multithreading Support

Previously, the game only used one CPU core, which caused severe slowdowns when many units were present. Now, with multithreading support, the game utilizes multiple cores, significantly improving performance. Tested in multiplayer with 40,000 units on the map — no lags or crashes occurred. In theory, even more units can be used, but this has not been tested.

#  🔧 Memory Leaks Fixed

Issues that caused lag and crashes during long play sessions (over 1 hour) have been completely resolved.
Fully Functional Multiplayer Hundreds of maps have been added for multiplayer and random map modes.

#  ✨ Removed All Dependency on DirectDraw and DirectSound ✨

One of the major improvements in version 1.52 is the complete replacement of DirectDraw and DirectSound. The game now runs entirely on SDL2 and SDL2 Mixer, offering better compatibility and stability on modern systems. No more "Direct Draw init error" issues!
The menu opens in windowed mode and the game switches to fullscreen upon launch. This design choice improves user experience. The menu resolution is 1024×768, which can look terrible in fullscreen on large monitors — hence the windowed menu.

#  🧩 New Features for Single Player (Cheats)

**king:** God mode, millions of resources, maximum of 15,000 units, artillery construction limits removed. Building and unit prices don’t increase, and the artillery depot has almost no construction limits. 🏰

**house:** Bots build a lot of houses. To stop them, enter the cheat again. 🏠

**nocost:** Disables the cost increase for units and buildings. 💰

**peace:** Enables a peace mode at any point during the game. Re-entering it will disable it. ☮️

#  🇷🇺 Localization and Mods

Russian language: The game will automatically switch to Russian if your system language is set to Russian. 🗣️
Mods mod1, reloaded, and ultimate pack: Fully translated into Russian. 🛠️

#  🌍 Language Support

For translating the Reloaded modification: You can create a translation file named reloaded_.gsc (for example, reloaded_ru.gsc for Russian).
For custom translations of the classic game: You can create a translation file named all_.gsc (for example, all_ru.gsc for Russian). 📝

#  ⚙️ Mod Installation

Mod installation: You can add your own mods by simply uploading the override.gsc file to the game folder. 🔧

# 🧪 Testing

Testing on ARM versions of Windows and macOS: The game has been tested on ARM versions of Windows and macOS running on Apple Silicon processors. In both cases, the game runs smoothly and without crashes. 🍏💻

# 📥 Where to Download?

🔹 Updated Version with Mods

📥 You can download the updated version of the game via this [link](https://archive.org/download/cossacks-back-to-war-v-1.52-2025_202506/Cossacks%20Back%20to%20War%20v1.52%20%282025%29.zip). 🌐

To launch mod reloaded, use the batch file Cossacks.Reloaded.bat located next to dmcr.exe.

🔹 Classic Version without Mods, but with Bug Fixes

This is Cossacks: Back to War 1.52 Classic Edition

📥 It is available in a separate archive: [Download Classic Version](https://archive.org/download/cossacks-back-to-war-v-1.52-2025_202506/Cossacks%20Back%20to%20War%20v1.52%20Classic%20Edition.zip)

This version does not include built-in add-ons, but all new cheat codes work as they are embedded in the game engine, not in the game resources.

# 🚨 Troubleshooting
If your game crashes on Windows, try one of the following solutions (or both, if necessary):

Create an empty text file named wine in the game folder.

To disable vertical synchronization, create an empty text file named novsync.

# 🍷 Installation on macOS via CrossOver

For macOS users, ready-made bottles (.cxarchive) are available, allowing you to run the game without manual setup:

📥 🔸 [Download the bottle for the updated version](https://archive.org/download/cossacks-back-to-war-v-1.52-2025_202506/macOS_Cossacks_Back_to_War_1.52.cxarchive)


📥 🔸 [Download the bottle for Classic Edition](https://archive.org/download/cossacks-back-to-war-v-1.52-2025_202506/macOS_Cossacks_Back_to_War_1.52_CE.cxarchive)

Installation:

  Install [CrossOver](https://www.codeweavers.com/crossover?srsltid=AfmBOopGZqKxWKdUpURIthTbxWTpk9u4zl8BwW4IwxWZXI2DJOM8A25b) for macOS if it is not already installed.

  Download the desired bottle.

  Double-click the file — CrossOver will automatically install the game.

  It is recommended to enable Retina mode for better image quality:

      Open CrossOver.

      Go to the bottle settings.

      Enable the “Use Retina mode (high DPI)” option.

# 🖥️ Windows XP Support — For True Retro Enthusiasts

Despite the primary focus on modern operating systems, we haven’t forgotten about Windows XP users — the legendary OS that became a symbol of an entire era.

Starting August 9, 2025, Cossacks: Back to War 1.52 Legacy Edition, specifically adapted for Windows XP SP3, is available.

🧰 System Requirements:

    Processor: Minimum Intel Core 2 Duo

    RAM: From 256 MB (512 MB or higher recommended)

🌟 Features:

    Full compatibility with Windows XP SP3

    All features and improvements of version 1.52

    Full multiplayer compatibility with players on Windows 11, Linux, and macOS

📥 Download Cossacks: Back to War 1.52 Legacy Edition: [👉 Download Archive](https://archive.org/download/cossacks-back-to-war-v-1.52-2025_202506/Cossacks%20Back%20to%20War%20v1.52%20LE%20%282025%29.zip).

📥 Classic Version (LE Classic Edition) without mods: [👉 Download Archive](https://archive.org/download/cossacks-back-to-war-v-1.52-2025_202506/Cossacks%20Back%20to%20War%20v1.52%20LE%20Classic%20Edition.zip).

    💡 Important: This version is also suitable if you are using outdated Linux versions, for example, without support for the latest versions of Wine or Proton. Thanks to enhanced compatibility and low system requirements, Legacy Edition can be an excellent choice for older distributions.

# 🖥 How to run a game on Linux?
To run a game on Linux, use Wine, Proton, or Lutris. Simply copy the game folder to the desired directory and launch one of the following files:

dmcr.exe — for the original version

dmcr.exe /reloaded — to run with the Reloaded mod

🔊 Sound issues (crackling, artifacts)?

If you encounter crackling or artifacts in the sound when launching the game, it may be related to the choice of audio backend in Wine. To fix these issues, it’s recommended to run the game with the audio driver specified manually:

    SDL_AUDIODRIVER=directsound wine dmcr.exe

To avoid entering this variable every time, you can add it to the configuration of your wineprefix. This will automatically apply the setting each time the game is launched.

# ⚠️ Important: Support for Windows 9x/ME and Windows 2000 will not be implemented for the following reasons:

    These systems lack support for multi-core processors

    Version 1.52 requires a minimum of 256 MB of RAM and a dual-core processor (4 cores or higher recommended)

<hr>

## ⚠️ A backup copy of the project and the source code are also available on the Internet Archive.
[Internet Archive](https://archive.org/details/cossacks-back-to-war-v-1.52-2025_202506)



#  🙏 Acknowledgments

I would like to thank the esteemed ereb-thanatos for the massive work on version 1.42. GitHub repository [ereb-thanatos](https://github.com/ereb-thanatos/cossacks-revamp-2017)

I also express my sincere gratitude to the esteemed [rb515proto](https://github.com/MisterCoderman/cossacks-2025/issues/34) for the help with translation and bug fixing 🙏

I would also like to thank [Ivan Petkov](https://github.com/MisterCoderman/cossacks-2025/issues/35) for his contribution and for creating the missions included in version 1.52 🙏


#  👨‍💻 For Developers

Code usage recommendations: It is recommended to use the code of version 1.52 as the basis for future game versions, as it is the most stable and improved version compared to 1.42. 📂

Manual project build: To manually build the project, use Visual Studio 2019 and Windows 11. All necessary files for building are in the Source archive. 🔧

Modding and Mission Development Tools:
A set of utilities for working with game archives and for creating/editing missions is available [here](https://archive.org/download/cossacks-back-to-war-v-1.52-2025_202506/tools.zip).


The repository for Cossacks: Back to War 1.52 Legacy Edition is available [here](https://github.com/MisterCoderman/Cossacks2025-Legacy)



#  ⚖️ Copyright Respect and Developer Support

The Cossacks 1.52 project by MR.CODERMAN is solely intended to support users who have legally purchased the original Cossacks: Back to War and want to enjoy it on modern computers. 🏆
The author is strongly against piracy and copyright violations.
I urge you: do not use this version of the game if you do not already own a legitimate license for the game from any official store, on CD, or DVD. The game must be legally purchased, and the Cossacks 1.52 project only serves to improve the compatibility and stability of the game you already own.
By supporting developers and purchasing games legally, you contribute to the development of the industry and the preservation of classic projects like Cossacks: Back to War. 💎



<hr>

<a name="rus"></a>
# 🌟 Казаки: Снова Война 1.52

Если вы любите старые стратегические игры, то «Казаки: Снова Война» — это классика жанра. Однако на современных компьютерах она может не работать должным образом. Обновление версии 1.52 от MR.CODERMAN решает эту проблему, улучшая стабильность и совместимость игры. 🌍

#  🔧 Что нового в версии 1.52?

Полная совместимость с Windows, macOS и Linux: Игра теперь работает без вылетов на всех современных платформах, обеспечивая стабильную производительность.

Убрано ограничение разрешения 1920x1080: Теперь вы можете наслаждаться игрой в высоком разрешении, без ограничений по размеру экрана. 🚀

Улучшенная графика и интерфейс: Обновлённые текстуры и интерфейс делают игру более приятной для глаз, добавляя яркости и улучшая восприятие. 🎨

**Вертикальная синхронизация включена по умолчанию.**

**Исправлены баги с цветами воды и наций. Теперь всё как в оригинальной игре.**

**Вернули возможность протыкания построек через туман войны, что сохранит привычные тактики онлайн-игры и улучшит геймплей.**

**В дипломатическом центре теперь можно заказывать сразу несколько бесконечностей юнитов одного типа одновременно, например три бесконечности пехотинцев и одну бесконечность лучников, чтобы на выходе получалось три пехотинца и один лучник. (Благодарность за идею и тестирование блогера [slovnoslon](https://www.youtube.com/@Slovnoslon) )**

**Исправлена проблема с миссией "Большая дорога". Теперь её возможно пройти полностью.**

**Улучшен AI (AI Pack). Теперь Боты намного сложнее.**

**Решена проблема с воспроизведением музыки в игре.**

# ** Добавлена поддержка 8 игроков в режиме случайной карты. **
Цвет — коричневый.  
В ресурсах игры нет полноценной поддержки 8-й нации на карте, поэтому есть ограничения:

    • Коричневый игрок обязательно должен быть союзником одного из существующих игроков.  

    • Крестьяне коричневого цвета будут появляться рядом с союзником.  

    • Если союзник не указан, коричневый игрок не сможет строить здания и будет недоступен для игры.  

 Союзником может быть как игрок, так и AI.  


# Интегрированы моды:


**Mod 1**

• Добавляет около 100 новых миссий (XVI–XVIII века);

• Вводит 4 уровня сложности для одиночной игры;

• Добавляет 2 новые нации — Швейцария и Венгрия (уникальная архитектура и ~4 юнита на каждую);

• Вводит нового юнита — бедуина на верблюде (для Турции и Алжира).

**Reloaded**

• Заменяет оригинальные модели юнитов и зданий на графику из American Conquest: Fight Back и Cossacks II;


• Вносит визуальные изменения и повышенную детализацию


**Ultimate Pack**


• Объединяет всё одиночное содержимое из “European Wars” и “Art of War” в “Back to War”;


• Добавляет в общей сложности 117 одиночных миссий, включая кампанию “Over the Horizon”;


**В комплекте идут карты для сетевой и одиночной игры**
<hr>

**Real War mod создан [-TC-]Atabey (mmuslu33@gmail.com)**

Мод заменяет оригинальные файлы игры, поэтому устанавливается отдельно — просто скачайте архив по [ссылке](https://archive.org/download/cossacks-back-to-war-v-1.52-2025_202506/Cossacks%20Real%20War.zip), распакуйте его в папку с игрой и запустите RealwarRun.bat.

Поддерживаются мультиплеер и режим случайной карты, сюжетные миссии недоступны; по всем вопросам и предложениям обращайтесь напрямую к автору мода.
<hr>

**Missions Pack**

• Добавляет миссии от разработчика [Ивана Петкова](https://github.com/MisterCoderman/cossacks-2025/issues/35)

• Включает 1 кампанию и 3 одиночные миссии.


Кампания:

• Слава Ушакова


Одиночные миссии:

• Сражение при Колине

• Сражение при Нейзби

• Сражение при Рокруа

# Поддержка многоядерности:

Ранее игра использовала только одно ядро, что приводило к сильным тормозам при большом количестве юнитов. Теперь, с поддержкой многоядерности, игра использует многопоточную обработку, что значительно улучшает производительность. Протестировано в мультиплеере с 40 000 юнитов на карте — лагов и вылетов не замечено. В теории можно использовать ещё больше юнитов, но это не проверялось.

#  🔧 Исправлены утечки памяти

Проблемы, вызывавшие тормоза и вылеты при длительных игровых сессиях (более 1 часа), полностью устранены.
Полностью рабочий мультиплеер

Добавлены сотни карт для сетевой игры и режима случайной карты.

#  ✨Убрана полностью зависимость от DirectDraw и DirectSound ✨

Одним из значительных улучшений в версии 1.52 является полная замена зависимостей от DirectDraw и DirectSound. Теперь игра работает исключительно на SDL2 и SDL2 Mixer, что обеспечивает лучшую совместимость и стабильность на современных системах. Проблемы с "Direct Draw init error" больше не существует.
Меню открывается в оконном режиме, а игра переходит в полноэкранный режим после запуска. Это сделано специально для удобства пользователей. Размер меню составляет 1024×768 пикселей, из-за чего оно выглядит ужасно на больших мониторах в полноэкранном режиме.

#  🧩 Новые возможности для одиночной игры (Читы)

**king:** Режим бога, миллионы ресурсов, максимум 15 000 юнитов, сняты ограничения на постройку артиллерии. Цена на здания и юнитов не увеличивается, в артиллерийском депо практически нет лимитов на постройку артиллерии. 🏰

**house:** Боты строят много домов. Чтобы остановить их, нужно ввести чит повторно. 🏠

**nocost:** Отключает увеличение стоимости юнитов и зданий. 💰

**peace:** Включает режим перемирия в любой момент игры. Повторный ввод отключит его. ☮️

#  🇷🇺 Локализация и моды

Русский язык: Игра автоматически переключится на русский, если язык вашей системы установлен как русский. 🗣️
Моды mod1, reloaded и ultimate pack: Полностью переведены на русский язык. 🛠️

#  🌍 Поддержка разных языков

Для перевода модификации Reloaded: Вы можете создать файл перевода с именем reloaded_<язык>.gsc (например, reloaded_ru.gsc для русского языка).
Для собственных переводов классической игры: Вы можете создать файл перевода с именем all_<язык>.gsc (например, all_ru.gsc для русского языка). 📝

#  ⚙️ Загрузка модов

Загрузка модов: Вы можете добавлять свои модификации, просто загрузив файл override.gsc в папку с игрой. 🔧

#  🧪 Тестирование

Тестирование на ARM-версии Windows и macOS: Игра была протестирована на ARM-версии Windows и macOS на процессорах Apple Silicon. В обоих случаях игра работает стабильно и без сбоев. 🍏💻





# 📥 Где скачать?

🔹 Обновлённая версия игры с модами

📥 Вы можете скачать обновлённую версию игры по [ссылке](https://archive.org/download/cossacks-back-to-war-v-1.52-2025_202506/Cossacks%20Back%20to%20War%20v1.52%20%282025%29.zip) . 🌐

Для запуска mod reloaded используйте бат-файл Cossacks.Reloaded.bat, который находится рядом с файлом dmcr.exe.

🔹 Классическая версия без модов, но с исправлениями ошибок


Это Cossacks: Back to War 1.52 Classic Edition

📥 Она доступна в отдельном архиве: [ Скачать классическую версию](https://archive.org/download/cossacks-back-to-war-v-1.52-2025_202506/Cossacks%20Back%20to%20War%20v1.52%20Classic%20Edition.zip) 

В этой версии нет встроенных дополнений, но работают все новые чит-коды, так как они прописаны в движке, а не в ресурсах игры.

# 🚨 Решение проблем

Если у вас вылетает игра в Windows, попробуйте одно из следующих решений (а при необходимости оба):

Создайте пустой текстовый файл с именем wine в папке с игрой.

Попробуйте отключить вертикальную синхронизацию, создав пустой текстовый файл novsync.


# 🍷 Установка на macOS через CrossOver

Для пользователей macOS доступны готовые бутылки (.cxarchive), которые позволяют запустить игру без ручной настройки:

📥 🔸 [Скачать бутылку для обновлённой версии](https://archive.org/download/cossacks-back-to-war-v-1.52-2025_202506/macOS_RU_Cossacks_Back_to_War_1.52.cxarchive)


📥 🔸 [Скачать бутылку для Classic Edition](https://archive.org/download/cossacks-back-to-war-v-1.52-2025_202506/macOS_RU_Cossacks_Back_to_War_1.52_CE.cxarchive)

Установка:

  Установите [CrossOver](https://www.codeweavers.com/crossover?srsltid=AfmBOopGZqKxWKdUpURIthTbxWTpk9u4zl8BwW4IwxWZXI2DJOM8A25b) для macOS, если он ещё не установлен.

    Скачайте нужную бутылку.

    Дважды кликните по файлу — CrossOver автоматически установит игру.

    Рекомендуется включить Retina-режим для лучшего качества изображения:

        Откройте CrossOver.

        Перейдите в настройки бутылки.

        Активируйте опцию “Use Retina mode (high DPI)”.



# 🖥️ Поддержка Windows XP — для настоящих ценителей ретро

Несмотря на основной фокус на современные операционные системы, мы не забыли о пользователях Windows XP — легендарной ОС, ставшей символом целой эпохи.

С 9 августа 2025 года доступна Cossacks: Back to War 1.52 Legacy Edition, специально адаптированная для Windows XP SP3.

🧰 Системные требования:

    Процессор: минимум Intel Core 2 Duo

    ОЗУ: от 256 МБ (рекомендуется 512 МБ и выше)

🌟 Особенности:

    Полная совместимость с Windows XP SP3

    Все функции и улучшения версии 1.52

    Полноценный мультиплеер, совместимый с игроками на Windows 11, Linux и macOS

📥 Скачать Cossacks: Back to War 1.52 Legacy Edition: [👉 Загрузить архив](https://archive.org/download/cossacks-back-to-war-v-1.52-2025_202506/Cossacks%20Back%20to%20War%20v1.52%20LE%20%282025%29.zip).

📥 Классическая версия (LE Classic Edition) без модов: [👉 Загрузить архив](https://archive.org/download/cossacks-back-to-war-v-1.52-2025_202506/Cossacks%20Back%20to%20War%20v1.52%20LE%20Classic%20Edition.zip).

    💡 Важно: Эта версия также подойдёт, если вы используете устаревшие версии Linux, например, без поддержки последних версий Wine или Proton. Благодаря повышенной совместимости и низким системным требованиям, Legacy Edition может быть отличным выбором для старых дистрибутивов.

# 🖥 Как запустить игру на Linux?

Для запуска игры на Linux используйте Wine, Proton или Lutris. Просто скопируйте папку с игрой в нужную директорию и запустите один из следующих файлов:

    dmcr.exe — для оригинальной версии

    dmcr.exe /reloaded — для запуска с модом Reloaded

🔊 Проблемы со звуком (хрусты, артефакты) ?

Если при запуске игры вы сталкиваетесь с хрустами или артефактами в звуке, это может быть связано с выбором звукового бэкенда в Wine. Чтобы устранить эти проблемы, рекомендуется запускать игру с указанием аудиодрайвера вручную:

    SDL_AUDIODRIVER=directsound wine dmcr.exe

Чтобы не вводить эту переменную каждый раз, вы можете прописать её в конфигурации вашей бутылки (wineprefix). Это позволит автоматически применять настройку при каждом запуске игры.

# ⚠️ Важно: поддержка Windows 9x/ME и Windows 2000 реализовываться не будет по следующим причинам:

    В этих системах отсутствует поддержка многоядерных процессоров

    Версия 1.52 требует минимум 256 МБ оперативной памяти и двухъядерный процессор (рекомендуется 4 ядра и выше)

<hr>

### ⚠️  Резервная копия проекта и исходный код также доступны в Internet Archive.



[Internet Archive](https://archive.org/details/cossacks-back-to-war-v-1.52-2025_202506)


# 🙏 Благодарности

Хочу выразить благодарность за огромную работу над версией 1.42 уважаемому ereb-thanatos. GitHub репозиторий [ereb-thanatos](https://github.com/ereb-thanatos/cossacks-revamp-2017)

Также выражаю искреннюю благодарность уважаемому [rb515proto](https://github.com/MisterCoderman/cossacks-2025/issues/34) за помощь с переводом и исправлением багов 🙏

Также хочу поблагодарить [Ивана Петкова](https://github.com/MisterCoderman/cossacks-2025/issues/35) за его вклад и создание миссий, которые доступны в версии 1.52 🙏



#  👨‍💻 Для разработчиков

Рекомендации по использованию кода: Рекомендуется использовать код версии 1.52 как основу для будущих версий игры, так как она является самой стабильной и улучшенной версией по сравнению с 1.42. 📂

Сборка проекта вручную: Для сборки проекта вручную используйте Visual Studio 2019 и Windows 11. Все необходимые файлы для сборки находятся в архиве Source. 🔧

Инструменты для моддинга и разработки миссий:
Комплект утилит для работы с игровыми архивами и создания/редактирования миссий доступен [здесь](https://archive.org/download/cossacks-back-to-war-v-1.52-2025_202506/tools.zip).


Репозиторий версии Cossacks: Back to War 1.52 Legacy Edition находится [здесь](https://github.com/MisterCoderman/Cossacks2025-Legacy)


 

# ⚖️ Уважение авторских прав и поддержка разработчиков

Проект Cossacks 1.52 от MR.CODERMAN направлен исключительно на поддержку пользователей, которые законно приобрели оригинальную версию игры «Казаки: Снова Война» и хотят наслаждаться ею на современных компьютерах. 🏆
Автор категорически против любого вида пиратства и нарушений авторских прав.
Призываю: не используйте данную версию игры, если у вас нет ранее купленной лицензии игры в любом из официальных магазинов, на CD или DVD диске. Игра должна быть приобретена легально, и проект Cossacks 1.52 служит лишь для улучшения совместимости и стабильности игры, которую вы уже имеете.
Поддерживая разработчиков и покупая игры легально, вы способствуете развитию индустрии и сохранению классических проектов, таких как «Казаки: Снова Война». 💎


