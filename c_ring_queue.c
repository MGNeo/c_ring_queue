#include <stdlib.h>
#include <stdint.h>
#include <memory.h>

#include "c_ring_queue.h"

struct s_c_ring_queue
{
    size_t push_pos,
           pop_pos,
           capacity,
           count;

    void **data;
};

// Кольцевой инкремент, удерживающий результат в интервале [0; _max).
static void ring_increment(size_t *const _value,
                           const size_t _max)
{
    if (_value == NULL)
    {
        return;
    }

    ++*_value;
    if (*_value >= _max)
    {
        *_value -= _max;// Вычитание универсальнее, чем = 0.
    }

    return;
}

// Создает кольцевую очередь.
// Не позволяет создать кольцевую очередь с буфером нулевой длины.
// В случае успеха возвращает указатель на созданную очередь.
// В случае ошибки возвращает NULL.
c_ring_queue *c_ring_queue_create(const size_t _capacity)
{
    if (_capacity == 0)
    {
        return NULL;
    }

    // Определим, сколько памяти необходимо под буфер.
    const size_t new_data_size = _capacity * sizeof(void*);

    // Проконтролируем переполнение.
    if ( (new_data_size == 0) ||
         (new_data_size / _capacity != sizeof(void*)) )
    {
        return NULL;
    }

    // Попытаемся выделить память под данные.
    void *const *const new_data = malloc(new_data_size);

    // Проконтролируем успешность выделения памяти.
    if (new_data == NULL)
    {
        return NULL;
    }

    // Попытаемся выделить память под кольцевую очередь.
    c_ring_queue *const new_ring_queue = (c_ring_queue*)malloc(sizeof(c_ring_queue));

    // Проконтролируем успешность выделения памяти.
    if (new_ring_queue == NULL)
    {
        free((void**)new_data);
        return NULL;
    }

    // Инициализируем кольцевую очередь.
    new_ring_queue->push_pos = 0;
    new_ring_queue->pop_pos = 0;
    new_ring_queue->capacity = _capacity;
    new_ring_queue->count = 0;

    new_ring_queue->data = (void**)new_data;

    return new_ring_queue;
}

// Удаляет кольцевую очередь.
// В случае успеха возвращает > 0.
// В случае ошибки возвращает < 0.
ptrdiff_t c_ring_queue_delete(c_ring_queue *const _ring_queue,
                              void (*const _del_data)(void *const _data))
{
    if (c_ring_queue_clear(_ring_queue, _del_data) < 0)
    {
        return -1;
    }

    free(_ring_queue->data);
    free(_ring_queue);

    return 1;
}

// Очищает кольцевую очередь ото всех элементов.
// В случае успешного очищения возвращает > 0.
// Если очередь очищать не от чего, возвращает 0.
// В случае ошибки возвращает < 0.
ptrdiff_t c_ring_queue_clear(c_ring_queue *const _ring_queue,
                             void (*const _del_data)(void *const _data))
{
    if (_ring_queue == NULL)
    {
        return -1;
    }

    if (_ring_queue->count == 0)
    {
        return 0;
    }

    // Если задана специальная функция удаления.
    if (_del_data != NULL)
    {
        // Идем по элементам в порядке очереди до тех пор, пока элементы есть.
        while (_ring_queue->count > 0)
        {
            _del_data( _ring_queue->data[_ring_queue->pop_pos] );
            // Уменьшаем кол-во имеющихся элементов.
            --_ring_queue->count;
            // Сдвигаем позицию вынимания.
            ring_increment(&_ring_queue->pop_pos, _ring_queue->capacity);
        }
    }

    _ring_queue->push_pos = 0;
    _ring_queue->pop_pos = 0;

    return 1;
}

// Добавляет элемент в конец очереди.
// Не позволяет вставлять NULL.
// В случае успеха возвращает > 0.
// В случае ошибки возвращает < 0.
// Если перед добавлением элемента очередь была заполнена, наиболее старый элемент затирается.
ptrdiff_t c_ring_queue_push(c_ring_queue *const _ring_queue,
                            const void *const _data,
                            void (*const _del_data)(void *const _data))
{
    if (_ring_queue == NULL)
    {
        return -1;
    }
    if (_data == NULL)
    {
        return -2;
    }

    // Если в очереди нет свободного места, затираем самый старый элемент.
    if (_ring_queue->count == _ring_queue->capacity)
    {
        // Если специальная функция удаления задана, вызываем ее.
        if (_del_data != NULL)
        {
            _del_data(_ring_queue->data[_ring_queue->push_pos]);
        }
        // Сдвигаем позицию вынимания.
        ring_increment(&_ring_queue->pop_pos, _ring_queue->capacity);
    } else {
        // Если в очереди есть место, увеличиваем счетчик элементов.
        ++_ring_queue->count;
    }

    // Захватываем данные в позицию вставки.
    _ring_queue->data[_ring_queue->push_pos] = (void*)_data;

    // Сдвигаем позицию вставки.
    ring_increment(&_ring_queue->push_pos, _ring_queue->capacity);

    return 1;
}

// Вынимает элемент из начала очереди.
// В случае успеха, возвращает > 0, очередь перестает владеть элементом.
// Если очередь пуста, возвращает 0.
// В случае ошибки возвращает < 0.
ptrdiff_t c_ring_queue_pop(c_ring_queue *const _ring_queue,
                           void (*const _del_data)(void *const _data))
{
    if (_ring_queue == NULL)
    {
        return -1;
    }

    if (_ring_queue->count == 0)
    {
        return 0;
    }

    if (_del_data != NULL)
    {
        _del_data(_ring_queue->data[_ring_queue->pop_pos]);
    }

    // Смещаем позицию вытаскивания.
    ring_increment(&_ring_queue->pop_pos, _ring_queue->capacity);

    // Уменьшаем количество элементов в кольцевой очереди.
    --_ring_queue->count;

    return 1;
}

// Обходит все элементы кольцевой очереди и выполняет над ними заданные действия.
// Если функция удаления не задана, это считается ошибкой.
// Элементы обходятся в порядке очереди.
// В случае успешного обхода возвращает > 0.
// Если кольцевая очередь пуста, возвращает 0.
// В случае ошибки возвращает < 0.
ptrdiff_t c_ring_queue_for_each(c_ring_queue *const _ring_queue,
                                void (*const _action_data)(void *const _data))
{
    if (_ring_queue == NULL)
    {
        return -1;
    }
    if (_action_data == NULL)
    {
        return -2;
    }

    // Если кольцевая очередь пуста.
    if (_ring_queue->count == 0)
    {
        return 0;
    }

    for (size_t i = 0, pos = _ring_queue->pop_pos;
         i < _ring_queue->count;
         ++i, ring_increment(&pos, _ring_queue->capacity))
    {
        _action_data(_ring_queue->data[pos]);
    }

    return 1;
}

// Возвращает количество элементов в кольцевой очереди.
// В случае ошибки возвращает 0.
size_t c_ring_queue_count(const c_ring_queue *const _ring_queue)
{
    if (_ring_queue == NULL)
    {
        return 0;
    }

    return _ring_queue->count;
}

// Возвращает емкость кольцевой очереди.
// В случае ошибки возвращает 0.
size_t c_ring_queue_capacity(const c_ring_queue *const _ring_queue)
{
    if (_ring_queue == NULL)
    {
        return 0;
    }

    return _ring_queue->capacity;
}
