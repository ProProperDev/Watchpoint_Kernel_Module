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
   `$ source oe-init-build-env`
