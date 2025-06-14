*[Информация на русском языке](#rus)*


# 🌟 Cossacks: Back to War 1.52 — Updated Version for 2025

If you love classic strategy games, Cossacks: Back to War is a genre classic. However, it may not work properly on modern computers. The 1.52 update by MR.CODERMAN addresses this issue by improving the game's stability and compatibility. 🌍
#  🔧 What’s New in Version 1.52?

Full Compatibility with Windows, macOS, and Linux The game now runs smoothly on all modern platforms without crashes, delivering stable performance.
Removed 1920x1080 Resolution Limit You can now enjoy the game in high resolutions with no screen size restrictions. 🚀

Improved Graphics and Interface Updated textures and UI make the game easier on the eyes, adding vibrancy and enhancing the visual experience. 🎨

**Fixed Water and Nation Color Bugs Visual issues related to water and nation colors have been corrected — everything now looks just like in the original game.**

**AI has been improved (AI Pack). Bots are now much more challenging.**

**The issue with music playback in the game has been resolved.**

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


# Mod Reloaded Launch Instructions

To launch mod reloaded, use the batch file Cossacks.Reloaded.bat located next to dmcr.exe. When using Wine on Linux or macOS, launch with the command: dmcr.exe /reloaded. 🔄

#  🧠 Multithreading Support

Previously, the game only used one CPU core, which caused severe slowdowns when many units were present. Now, with multithreading support, the game utilizes multiple cores, significantly improving performance. Tested in multiplayer with 40,000 units on the map — no lags or crashes occurred. In theory, even more units can be used, but this has not been tested.

#  🔧 Memory Leaks Fixed

Issues that caused lag and crashes during long play sessions (over 1 hour) have been completely resolved.
Fully Functional Multiplayer Hundreds of maps have been added for multiplayer and random map modes.

#  ✨ Removed All Dependency on DirectDraw and DirectSound ✨

One of the major improvements in version 1.52 is the complete replacement of DirectDraw and DirectSound. The game now runs entirely on SDL2 and SDL2 Mixer, offering better compatibility and stability on modern systems. No more "Direct Draw init error" issues!
The menu opens in windowed mode and the game switches to fullscreen upon launch. This design choice improves user experience. The menu resolution is 1024×768, which can look terrible in fullscreen on large monitors — hence the windowed menu.

#  🧩 New Features for Single Player (Cheats)

king: God mode, millions of resources, maximum of 15,000 units, artillery construction limits removed. Building and unit prices don’t increase, and the artillery depot has almost no construction limits. 🏰

house: Bots build a lot of houses. To stop them, enter the cheat again. 🏠

nocost: Disables the cost increase for units and buildings. 💰

peace: Enables a peace mode at any point during the game. Re-entering it will disable it. ☮️

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

#  👨‍💻 For Developers

Code usage recommendations: It is recommended to use the code of version 1.52 as the basis for future game versions, as it is the most stable and improved version compared to 1.42. 📂
Manual project build: To manually build the project, use Visual Studio 2019 and Windows 11. All necessary files for building are in the Source archive. 🔧

# 📥 Where to Download?

You can download the updated version of the game [here](https://mega.nz/file/eckQGQyY#1qYEfjJrHGTM70dN-lEHviOtmMZ1Sk7Z0xAuWqRx_Ak) 🌐

## ⚠️ A backup copy of the project and the source code are also available on the Internet Archive.
[Internet Archive](https://archive.org/details/cossacks-back-to-war-v-1.52-2025_202506)


#  🖥 How to Run the Game on Linux or macOS?

To run the game on Linux, use Wine or Proton . To run it on macOS, use [Whisky](https://getwhisky.app/) or [CrossOver](https://www.codeweavers.com/). Simply copy the game folder into the created bottle and launch the dmcr.exe file, or dmcr.exe /reloaded if you want to start the game with the Reloaded mod.

Enjoy the game! 🎮

#  🙏 Acknowledgments

I would like to thank the esteemed ereb-thanatos for the massive work on version 1.42. GitHub repository [ereb-thanatos](https://github.com/ereb-thanatos/cossacks-revamp-2017)

#  ⚖️ Copyright Respect and Developer Support

The Cossacks 1.52 project by MR.CODERMAN is solely intended to support users who have legally purchased the original Cossacks: Back to War and want to enjoy it on modern computers. 🏆
The author is strongly against piracy and copyright violations.
I urge you: do not use this version of the game if you do not already own a legitimate license for the game from any official store, on CD, or DVD. The game must be legally purchased, and the Cossacks 1.52 project only serves to improve the compatibility and stability of the game you already own.
By supporting developers and purchasing games legally, you contribute to the development of the industry and the preservation of classic projects like Cossacks: Back to War. 💎
#  🔒 Checksums

If you happen to come across any archive on the web, you may need the following checksums to verify the integrity of the files:

| File                                                    | SHA‑256                                                         |
|---------------------------------------------------------|----------------------------------------------------------------------|
| Cossacks Back to War v1.52 (2025).zip                   | 6afc688a4c6bd409b1a3b541edad721e0ce28cd7abe4740a0449846e1646c12c |
| cossacks-1.52-source.zip                  | 74f23e1a3524f0cdab4908b4180043a189de2b53185a506086bee25d29257338 |

You will also need the [Microsoft Visual C++ 2015 Redistributable (x86)](https://www.microsoft.com/en-us/download/details.aspx?id=53587) for the game to work correctly. 💻
<hr>

<a name="rus"></a>
# 🌟 Казаки: Снова Война 1.52 — Обновлённая версия 2025 года

Если вы любите старые стратегические игры, то «Казаки: Снова Война» — это классика жанра. Однако на современных компьютерах она может не работать должным образом. Обновление версии 1.52 от MR.CODERMAN решает эту проблему, улучшая стабильность и совместимость игры. 🌍

#  🔧 Что нового в версии 1.52?

Полная совместимость с Windows, macOS и Linux: Игра теперь работает без вылетов на всех современных платформах, обеспечивая стабильную производительность.
Убрано ограничение разрешения 1920x1080: Теперь вы можете наслаждаться игрой в высоком разрешении, без ограничений по размеру экрана. 🚀
Улучшенная графика и интерфейс: Обновлённые текстуры и интерфейс делают игру более приятной для глаз, добавляя яркости и улучшая восприятие. 🎨
**Исправлены баги с цветами воды и наций. Теперь всё как в оригинальной игре.**
**Улучшен AI (AI Pack). Теперь Боты намного сложнее.**
**Решена проблема с воспроизведением музыки в игре.**
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


# Запуск модификации mod reloaded:

Для запуска mod reloaded используйте бат-файл Cossacks.Reloaded.bat, который находится рядом с файлом dmcr.exe. При запуске через Wine на Linux или macOS используйте команду dmcr.exe /reloaded. 🔄

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

king: Режим бога, миллионы ресурсов, максимум 15 000 юнитов, сняты ограничения на постройку артиллерии. Цена на здания и юнитов не увеличивается, в артиллерийском депо практически нет лимитов на постройку артиллерии. 🏰

house: Боты строят много домов. Чтобы остановить их, нужно ввести чит повторно. 🏠

nocost: Отключает увеличение стоимости юнитов и зданий. 💰

peace: Включает режим перемирия в любой момент игры. Повторный ввод отключит его. ☮️

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

#  👨‍💻 Для разработчиков

Рекомендации по использованию кода: Рекомендуется использовать код версии 1.52 как основу для будущих версий игры, так как она является самой стабильной и улучшенной версией по сравнению с 1.42. 📂
Сборка проекта вручную: Для сборки проекта вручную используйте Visual Studio 2019 и Windows 11. Все необходимые файлы для сборки находятся в архиве Source. 🔧

#  📥 Где скачать?

Вы можете скачать обновлённую версию игры по [ссылке](https://mega.nz/file/eckQGQyY#1qYEfjJrHGTM70dN-lEHviOtmMZ1Sk7Z0xAuWqRx_Ak) . 🌐

### ⚠️  Резервная копия проекта и исходный код также доступны в Internet Archive.



[Internet Archive](https://archive.org/details/cossacks-back-to-war-v-1.52-2025_202506)

# 🖥 Как запустить игру на Linux или MacOS?

Для запуска игры на Linux используйте Wine или Proton. Для запуска на MacOS — [Whisky](https://getwhisky.app/) или [CrossOver](https://www.codeweavers.com/). Просто скопируйте папку с игрой в созданную бутылку и запустите файл dmcr.exe или dmcr.exe /reloaded, если хотите загрузить игру с модом Reloaded.

🎮 Приятной игры!

# 🙏 Благодарности

Хочу выразить благодарность за огромную работу над версией 1.42 уважаемому ereb-thanatos. GitHub репозиторий [ereb-thanatos](https://github.com/ereb-thanatos/cossacks-revamp-2017)

# ⚖️ Уважение авторских прав и поддержка разработчиков

Проект Cossacks 1.52 от MR.CODERMAN направлен исключительно на поддержку пользователей, которые законно приобрели оригинальную версию игры «Казаки: Снова Война» и хотят наслаждаться ею на современных компьютерах. 🏆
Автор категорически против любого вида пиратства и нарушений авторских прав.
Призываю: не используйте данную версию игры, если у вас нет ранее купленной лицензии игры в любом из официальных магазинов, на CD или DVD диске. Игра должна быть приобретена легально, и проект Cossacks 1.52 служит лишь для улучшения совместимости и стабильности игры, которую вы уже имеете.
Поддерживая разработчиков и покупая игры легально, вы способствуете развитию индустрии и сохранению классических проектов, таких как «Казаки: Снова Война». 💎

# 🔒 Контрольные суммы

Если вы вдруг наткнётесь на какой-то архив на просторах сети, вам могут понадобиться следующие контрольные суммы для проверки целостности файлов:
| Файл                                                    | SHA‑256                                                         |
|---------------------------------------------------------|----------------------------------------------------------------------|
| Cossacks Back to War v1.52 (2025).zip                   | 6afc688a4c6bd409b1a3b541edad721e0ce28cd7abe4740a0449846e1646c12c |
| cossacks-1.52-source.zip                    | 74f23e1a3524f0cdab4908b4180043a189de2b53185a506086bee25d29257338 |

Также потребуется [Распространяемый компонент Microsoft Visual C++ 2015 (x86)](https://www.microsoft.com/ru-ru/download/details.aspx?id=53587) для корректной работы игры. 💻

