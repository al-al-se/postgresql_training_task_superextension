# postgresql_training_task_superextension

## Цель проекта
создать собственное расширение superextension которое:
Должно содержать одну функцию superfunction, написанную на C, принимающую два аргумента N (целое) и A (символ), возвращающую N – записей, состоящих из двух полей «порядковый номер» и «строка». В поле «порядковый номер» выводить число от 1..N, в поле «строка» выводить строку из повторяющихся символов A в количестве, равном значению поля «порядковый номер» для данной записи.

## Структура проекта:
1. superextension.c – исходный код искомой функции + стандартная обвязка расширения (модуля) PostgreSQL.
2. superextension—1.0.sql – необходимый sql-файл для создания объектов БД, предлагаемых данным расширением (модулем). В качестве объекта БД расширение superextension поставляет новую функцию superfunction.
3. superextension.control – управляющий файл для загрузки расширения (модуля) ядром СУБД

## Сборка расширения
1. cc -I/usr/local/pgsql/include/server -fpic -c superextension.c
2. cc -shared -L/usr/local/pgsql/lib -lpq -o superextension.so superextension.o

## Устновка (Deploy)
1. sudo cp superextension.so /usr/local/pgsql/lib/
2. sudo cp superextension--1.0.sql /usr/local/pgsql/share/extension/
3. sudo cp superextension.control /usr/local/pgsql/share/extension/
4. в psql выполнить CREATE EXTENSION superextension;

## Тест
в psql выполнить `select * from superfunction(5, 'A');`

результат:
```
 serial_number | result_string 
---------------+---------------
             1 | A
             2 | AA
             3 | AAA
             4 | AAAA
             5 | AAAAA
(5 rows)
```

  
