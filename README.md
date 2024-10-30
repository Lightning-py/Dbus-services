# DBUS Services

### Инструкция по сборке

Для сборки проекта нужно выполнить следующие команды:

```bash
git clone https://github.com/Lightning-py/Dbus-services
cd Dbus-services
chmod +x installer.sh
./installer.sh
```

Репозиторий клонируется, запустится установщик нужных библиотек. После чего  создастся папка ```build```, в которой будет лежать собранный проект. Прошу обратить внимание, что проект автоматически собирается с флагами ```-Wall -Wextra -Werror```.

Проект расcчитан на сборку на системе Ubuntu и устанавливает библиотеку ```sqlite``` из репозиториев Ubuntu. 

### Использование 

Чтобы запустить программы нужно выполнить команду: ```./PermissionService & ./SystemTimeService & ./Application```

Сначала появятся сообщение с ``pid`` процессов ```PermissionService``` и ```SystemTimeService```, после чего появятся результаты работы приложения ```Application```: 

```
Access denied!
[com.system.UnauthorizedAccess] Permission check failed
Trying to get permission
Got a permission. Trying to get timestamp again.
Date and time: 2024-10-30 10:00:00
```

Это значит, что приложение запустилось, попыталось получить timestamp, после чего ему отказал сервис ```time``` за неимением разрешения на получение времени. После чего приложение обратилось к сервису ```permissions``` и получило разрешение, после чего попыталось получить timestamp еще раз и получило успешно.

При последующем запуске ```Application``` будут появляться только дата и время, так как приложение уже имеет разрешение ```SystemTime```.

После тестирования необходимо убить процессы ```PermissionService``` и ```SystemTimeService```

### Дополнительная информация

- Сервис ```PermissionService``` поддерживает сколько угодно видов разрешений (в пределах типа int32).
- Оба сервиса ```PermissionService``` и ```SystemTimeService``` логируют все запросы и свое существование в файлы ```log.txt``` и ```log_time.txt``` соответственно 

