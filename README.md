# Arduino breakout

Умный дом с заменой [arduino](https://www.arduino.cc/index.php)-компонент.

# Оглавление

1. [Оглавление](#оглавление)
1. [Описание](#описание)
1. [Структура проекта](#структура-проекта)
   1. [Главное устройство](#главное-устройство)
   1. [Контролируемые устройства](#контролируемые-устройства)
   1. [Сервера](#сервера)
   1. [Бот](#бот)
   1. [Фронт](#фронт)
   1. [Взаимодействие](#взаимодействие)
1. [Уведомления](#уведомления)
   1. [Устройства](#уведомления-устройства)
   1. [Телеграм-бот](#уведомления-телеграм-бот)
1. [Технологии](#технологии)
1. [Софт](#софт)
1. [Пример использования](#пример-использования)
1. [Лицензия](#лицензия)

## Описание

Основное назначение –– автоматизация различных процессов с помощью контроллеров и одноплатных контроллеров. В рамках данного проекта решение применяется для:

* Взаимодействия удаленного между различными участниками системы;
* Управления различных устройств (в данном проекте приведены упрощенные примеры, но несложно подключить и полноценные бытовые приборы).  

Использование данной системы может установлено в абсолютно любом помещении:
* офисные помещения;
* дома;
* магазины и склады;
* банки и другие объекты кредитно-финансовой сферы;
* частный сектор (дома, дачи, квартиры).

## Структура проекта

Структурная схема реализации данного проекта представлена ниже:
![Функциональная схема](/static/functional_scheme.png)  

### Главное устройство

В данном проекте едиснтвенное устройство, которое контролирует все остальные –– [LinkIt Smart 7688 Duo](http://wiki.seeedstudio.com/LinkIt_Smart_7688_Duo/). Внутри данного контроллера располагается web-server на nodejs с сервером `express`, а также python сервер для обращения к устройствам по виртуальным COM-портам. 
 
### Контролируемые устройства

В первоначальной реализации использовались физические платы Arduino Uno как конечные устройства с интерфейсом взаимодействия Bluetooth. Во время дальнейшей реализации проекта пришли к результату, что данные платы требуют слишком много ресурсов: как настроек, количество кода, так и банально цены. 

Было принято решение использовать эмулятор [Proteus](https://ru.wikipedia.org/wiki/Proteus_(система_автоматизированного_проектирования)). В качестве замены Arduino выступил контроллер [PIC16F877](http://radio-hobby.org/uploads/datasheets/pic/pic16f873-pic16f877.pdf).

Список устройств, подключенных к трем таким контроллерам через виртуальные COM-порты:
1. датчик температуры;
1. LCD-экран;
1. шлагбаум (на основе шагового двигателя).

В случае перегрева или переохлаждения датчика температуры автоматически отправляется сообщение через telegram-бота пользователю системы, на LCD-экране показывается специальное уведомление, а также открывается шлагбаум.

### Сервера

| # 	| Название                          	| Описание                                    	| ЯП       	|
|---	|-----------------------------------	|---------------------------------------------	|----------	|
| 1 	| API взаимодействия с устройствами 	| Взаимодействие с конечными устройствами     	| Python 3 	|
| 2 	| Слушатель устройств               	| Проверяет устройства на возможные неопладки 	| Python 3 	|
| 3 	| Статика клиентского приложения    	| Раздача статики                             	| nodejs   	|
| 4 	| Бот                               	| Функции бота                                	| Python 3 	|
