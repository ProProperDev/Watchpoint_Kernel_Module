# Watchpoint_Kernel_Module
# Задание
Написать модуль ядра, который будет принимать адрес (например, 0x12345, и поддержать оба варианта через sysfs и module params), устанавливать на этот адрес watch point, что бы при обращении к этому адресу (чтение и запись) вызывался callback из этого модуля (можно отдельные для read и write) и печатал backtrace. ядро использовать для qemu (x86) и собирать в yocto. на выходе должен быть рецепт для модуля и инструкция как это тестировать в run-time
По факту это:
1. Новый драйвер для отладки;
2. Ресип для сборки его в дистрибутиве в конкретном релизе.
# Как тестировать модуль
Сценарий тестирования следующий:
* Помимо самого watchpoint модуля будем использовать test_watchpoint.ko(вспомогательный модуль для тестирования watchpoint), который инкриминирует значение по определённому адресу каждые 10 сек.
1. Загружаем модуль test_watchpoint.ko(тестирующий модуль). И узнаем адрес инкриминируемого значения вызвав `# dmesg | tail`
2. Загружаем модуль watchpoint_module.ko(тестируемый модуль). Можно сразу установить watchpoint на определённый адрес. Например: `# insmod watchpoint_module.ko address="0xd11dc222"`. Если не указывать address, то watchpoint по умолчанию установится на 0x0.
3. Устанавливаем watchpoint на адрес отличный от того, на котором test_watchpoint инкриминирует значение.
4. Отслеживаем, вызывая `# dmesg | tail`, что test_watchpoint продолжает инкриминировать значение, а watchpoint, в свою очередь никак на это не реагирует.
5. Меняем адрес установки watchpoint на тот, на котором test_watchpoint инкриминирует значение.
6. Отслеживаем, вызывая `# dmesg | tail`, что test_watchpoint продолжает инкриминировать значение, а watchpoint, при каждом обращении по адресу печатает backtrace.
# Порядок решения
1. Подготавливаем необходимые пакеты для установки Yocto, следуя инструкции на официальном сайте Yocto. \
   (https://docs.yoctoproject.org/brief-yoctoprojectqs/index.html) \
   `$ sudo apt-get update` \
   `$ sudo apt install gawk wget git diffstat unzip texinfo gcc build-essential chrpath socat cpio python3 python3-pip python3-pexpect xz-utils debianutils iputils-ping python3-git python3-jinja2 libegl1-mesa libsdl1.2-dev python3-subunit mesa-common-dev zstd liblz4-tool file locales` \
   `$ sudo locale-gen en_US.UTF-8`
2. Копируем репозиторий себе на компьютер. \
   `$ git clone git://git.yoctoproject.org/poky`
3. Cоздаём локальную ветку в директории poky/ с именем my-mickledore. Файлы, доступные в этой ветке, точно соответствуют файлам репозитория в ветке выпуска mickledore. \
   `$ cd poky` \
   `$ git checkout -t origin/mickledore -b my-mickledore`
4. Синхронизируемся с удалённым репозиторием. \
   `$ git pull`
5. В директории poky/ запускаем скрипт для автоматической установки окружения для сборки. По завершении скрипта оказываемся в
   директории poky/build.  \
   `$ source oe-init-build-env`
6. Клонируем репозиторий с модулем в директорию poky/. \
   `$ cd ..` \
   `$ git clone https://github.com/ProProperDev/Watchpoint_Kernel_Module`
7. Создаём новый слой в директории poky/ для последующего добавления в сборку.  \
   `$ bitbake-layers create-layer meta-watchpoint-mod`
8. Добавляем наш слой в сборку.  \
   `$ bitbake-layers add-layer meta-watchpoint-mod` 
9. Проверяем, добавился ли наш слой и какие слои вообще добавлены в нашу сборку.  \
   `$ bitbake-layers show-layers`  \
    ![Screenshot](https://github.com/ProProperDev/Watchpoint_Kernel_Module/blob/main/Screenshots/CreateAddCheckNewLayer.png)
10. Копируем директории poky/Watchpoint_Kernel_Module/conf и poky/Watchpoint_Kernel_Module/recipes-kernel в poky/meta-watchpoint-mod c заменой файлов с одинаковым именем. Проверяем структуру директории poky/meta-watchpoint-mod.
    
    Примечание: директория recipes-example сгенерирована автоматически при создании слоя, можно её просто игнорировать, а можно удалить, чтобы не мешала. Будем игнорировать.
    ![Screenshot](https://github.com/ProProperDev/Watchpoint_Kernel_Module/blob/main/Screenshots/CopyInLayerAndCheckThisTree.png)
11. Готовим конфигурацию сборки в файле poky/build/conf/local.conf. Открываем в текстовом редакторе и редактируем.
    Добавляем строку:  \
    `IMAGE_INSTALL:append = " watchpoint-module"`  \
    И нужно раскомментировать строку:  \
    `MACHINE ?= "qemux86"`
    ![Screenshot](https://github.com/ProProperDev/Watchpoint_Kernel_Module/blob/main/Screenshots/PrepareLocalConf.png)
12. Начинаем сборку из директории poky/build. В моём случае сборка заняла ~20 часов.  \
    `$ bitbake core-image-minimal`
    ![Screenshot](https://github.com/ProProperDev/Watchpoint_Kernel_Module/blob/main/Screenshots/AfterBuildingImage.png)
13. Запускаем сборку. Для удобства вызываем дублирующий терминал (Ctrl+Alt+2). По умолчанию пользователь root, без пароля. \
    `$ runqemu qemux86`
    ![Screenshot](https://github.com/ProProperDev/Watchpoint_Kernel_Module/blob/main/Screenshots/RunqemuQemux86.png)
14. Проверяем наличие модулей в /lib/modules/6.1.53-yocto-standard/extra.  \
    ![Screenshot](https://github.com/ProProperDev/Watchpoint_Kernel_Module/blob/main/Screenshots/InModDir.png)
15. Загружаем модуль test_watchpoint.ko, либо из директории /lib/modules/6.1.53-yocto-standard/extra, либо указав полный путь к модулю. И выводим буфер сообщений ядра в стандартный поток вывода.   \
    `# insmod test_watchpoint.ko`  \
    `# dmesg | tail`
    ![Screenshot](https://github.com/ProProperDev/Watchpoint_Kernel_Module/blob/main/Screenshots/TestModIns.png)
16. Аналогично предыдущему пункту поступаем с модулем watchpoint_module.ko.  \
    `# insmod watchpoint_module.ko`  \
    `# dmesg | tail`  \
    Примечание: поскольку мы не задали при загрузке модуля переменную address, watchpoint установится на адрес 0x0.
    ![Screenshot](https://github.com/ProProperDev/Watchpoint_Kernel_Module/blob/main/Screenshots/WatchModIns.png)
17. Сменим адрес watchpoint на тот, на котором модуль test_watchpoint инкриминирует значение каждые 10 сек. И выводим буфер сообщений ядра в стандартный поток вывода. \
    `# echo "0xd13dc284" > /sys/kernel/watchpoint/address`  \
    `# dmesg | tail`
    ![Screenshot](https://github.com/ProProperDev/Watchpoint_Kernel_Module/blob/main/Screenshots/ChangeAddress1.png)
    Watchpoint сменила адрес и при обращении по этому адресу печатает backtrace.
18. Выгружаем наши модули.  \
    `# rmmod test_watchpoint`  \
    `# rmmod watchpoint_module`
    ![Screenshot](https://github.com/ProProperDev/Watchpoint_Kernel_Module/blob/main/Screenshots/RmmodMods.png)
19. Выключаем нашу машину.  \
    `# shutdown -h now`  \
    или  \
    `# poweroff`

    
   
