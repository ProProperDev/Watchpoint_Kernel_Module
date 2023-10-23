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
6. В директории poky/ запускаем скрипт для автоматической установки окружения для сборки. \
   `$ source oe-init-build-env wp_workdir`
7. Возвращаемся в poky/ и копируем рецепт сборки из удалённого репозитория. \
   `$ cd ..` \
   `$ git clone https://github.com/ProProperDev/Watchpoint_Kernel_Module`
8. Переходим в директорию poky/wp_workdir/conf и добавляем в файл bblayers.conf новый слой
   `$ cd ./wp_workdir/conf` \
   Путь до директории poky/ может отличаться. В моём случае bblayers.conf выглядит так: \
   `# POKY_BBLAYERS_CONF_VERSION is increased each time build/conf/bblayers.conf` \
   `# changes incompatibly` \
   `POKY_BBLAYERS_CONF_VERSION = "2"`

   `BBPATH = "${TOPDIR}"` \
   `BBFILES ?= ""`

   `BBLAYERS ?= " \` \
   `/home/mint/poky/meta \` \
   `/home/mint/poky/meta-poky \` \
   `/home/mint/poky/meta-yocto-bsp \` \
   `/home/mint/poky/Watchpoint_Kernel_Module/recipes_and_conf \` \
   `"` 
   
