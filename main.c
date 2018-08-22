#include <stdio.h>
#include <stdlib.h>

#include "c_ring_queue.h"

// Проверка возвращаемых значений не выполняется для упрощения.

// Функция печати элемента кольцевой очереди.
void print(void *const _data)
{
    if (_data == NULL)
    {
        return;
    }

    const size_t data = *( (size_t*) _data);
    printf("%Iu\n", data);

    return;
}

int main(int argc, char **argv)
{
    // Создание кольцевой очереди емкостью 5.
    c_ring_queue *const ring_queue = c_ring_queue_create(5);

    size_t value = 0;
    while (1)
    {
        // Добавление в очередь от одного до четырех элементов.
        size_t count = 1 + rand() % 4;
        for (size_t i = 0; i < count; ++i, ++value)
        {
            size_t *data = (size_t*)malloc(sizeof(size_t));
            *data = value;
            c_ring_queue_push(ring_queue, data, free);
        }

        // Вывод содержимого.
        printf("push count: %Iu\n", count);
        c_ring_queue_for_each(ring_queue, print);

        // Удаление из очереди от одного до трех элементов.
        count = 1 + rand() % 3;
        for (size_t i = 0; i < count; ++i)
        {
            c_ring_queue_pop(ring_queue, free);
        }
        // Вывод содержимого.
        printf("pop count: %Iu\n", count);
        c_ring_queue_for_each(ring_queue, print);

        getchar();
    }

    //c_ring_queue_delete(ring_queue, free);

    return 0;
}
