#include <stdio.h>
#include <stdlib.h>

#include "c_ring_queue.h"

// Функция печати элемента кольцевой очереди.
void print(void *const _data)
{
    if (_data == NULL)
    {
        return;
    }

    const float data = *( (float*) _data);
    printf("%f\n", data);

    return;
}

int main(int argc, char **argv)
{
    size_t error;
    c_ring_queue *ring_queue;

    // Создание кольцевой очереди емкостью 5.
    ring_queue = c_ring_queue_create(5, &error);
    // Если возникла ошибка, покажем ее.
    if (ring_queue == NULL)
    {
        printf("create error: %Iu\n", error);
        printf("Program end.\n");
        getchar();
        return -1;
    }

    // Добавим в начало очереди 7 элементов.
    for (size_t i = 0; i < 7; ++i)
    {
        // Попытаемся выделить память под элемент.
        float *const data = malloc(sizeof(float));
        // Если память выделить не удалось, сообщаем об этом.
        if (data == NULL)
        {
            printf("malloc(): NULL\n");
            printf("Program end.\n");
            getchar();
            return -2;
        }
        // Инициализируем элемент.
        *data = i;
        // Добавляем элемент в начало очереди.
        const ptrdiff_t r_code = c_ring_queue_push(ring_queue, data, free);
        // Если возникла ошибка, покажем ее.
        if (r_code < 0)
        {
            printf("push error, r_code: %Id\n", r_code);
            printf("Program end.\n");
            getchar();
            return -3;
        }
    }

    // Используя обход кольцевой очереди, покажем содержимое каждого элемента.
    {
        const ptrdiff_t r_code = c_ring_queue_for_each(ring_queue, print);
        // Если возникла ошибка, покажем ее.
        if (r_code < 0)
        {
            printf("for each error, r_code: %Id\n", r_code);
            printf("Program end.\n");
            getchar();
            return -4;
        }
    }

    // Удалим кольцевую очередь.
    {
        const ptrdiff_t r_code = c_ring_queue_delete(ring_queue, free);
        // Если возникла ошибка, покажем ее.
        if (r_code < 0)
        {
            printf("delete error, r_code: %Id\n", r_code);
            printf("Program end.\n");
            getchar();
            return -5;
        }
    }

    getchar();
    return 0;
}
