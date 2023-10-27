# Watchpoint_Kernel_Module
#Задание
---//---
#Порядок решения
1. Подготавливаем необходимые пакеты для установки Yocto, следуя инструкции на официальном сайте Yocto. \
   (https://docs.yoctoproject.org/brief-yoctoprojectqs/index.html) \
   `$ sudo apt-get update` \
   `$ sudo apt install gawk wget git diffstat unzip texinfo gcc build-essential chrpath socat cpio python3 python3-pip python3-pexpect xz-utils debianutils iputils-ping python3-git python3-jinja2 libegl1-mesa libsdl1.2-dev python3-subunit mesa-common-dev zstd liblz4-tool file locales` \
   `$ sudo locale-gen en_US.UTF-8`
3. Копируем репозиторий себе на компьютер. \
   `$ git clone git://git.yoctoproject.org/poky`
4. Cоздаём локальную ветку в директории poky/ с именем my-mickledore. Файлы, доступные в этой ветке, точно соответствуют файлам репозитория в ветке выпуска mickledore. \
   `$ cd poky` \
   `$ git checkout -t origin/mickledore -b my-mickledore`
5. Синхронизируемся с удалённым репозиторием. \
   `$ git pull`
6. В директории poky/ запускаем скрипт для автоматической установки окружения для сборки. По завершении скрипта оказываемся в
   директории poky/build.  \
   `$ source oe-init-build-env`
8. Клонируем репозиторий с модулем в директорию poky/. \
   `$ cd ..` \
   `$ git clone https://github.com/ProProperDev/Watchpoint_Kernel_Module`
9. Создаём новый слой в директории poky/ для последующего добавления в сборку.  \
   `$ bitbake-layers create-layer meta-watchpoint-mod`
10. Добавляем наш слой в сборку.  \
   `$ bitbake-layers add-layer meta-watchpoint-mod` 
11. Проверяем, добавился ли наш слой и какие слои вообще добавлены в нашу сборку.  \
   `$ bitbake-layers show-layers`
    ![Screenshot](https://github.com/ProProperDev/Watchpoint_Kernel_Module/blob/main/Screenshots/CreateAddCheckNewLayer.png)
12. Копируем директории poky/Watchpoint_Kernel_Module/conf и poky/Watchpoint_Kernel_Module/recipes-kernel в poky/meta-watchpoint-mod c заменой файлов с одинаковым именем. Проверяем структуру директории poky/meta-watchpoint-mod.
    ![Screenshot](https://github.com/ProProperDev/Watchpoint_Kernel_Module/blob/main/Screenshots/CopyInLayerAndCheckThisTree.png)
13. Готовим конфигурацию сборки в файле poky/build/conf/local.conf. Открываем в текстовом редакторе и редактируем.
    Добавляем строку:  \
    `IMAGE_INSTALL:append = " watchpoint-module"`  \
    И нужно раскомментировать строку:  \
    `MACHINE ?= "qemux86"`
    ![Screenshoot](https://github.com/ProProperDev/Watchpoint_Kernel_Module/blob/main/Screenshots/PrepareLocalConf.png)
14. Запускаем сборку из директории poky/build. В моём случае сборка заняла ~20 часов.  \
    `$ bitbake core-image-minimal`
    ![Screenshot](https://github.com/ProProperDev/Watchpoint_Kernel_Module/blob/main/Screenshots/AfterBuildingImage.png)
15. Запускаем сборку.  \
    `$ runqemu qemux86`
   
