#ifndef INCLUDE_LIST_H
#define INCLUDE_LIST_H

#include <iostream>
#include <cstdlib>
#include <cassert>
#include <cstring>

#define $p getchar ();


typedef int elem_t;

static const int $POISON = -666;

static const int START_SIZE = 20;
static const int ADDITIONAL = 20;
static const int DELTA      = 10;

enum LIST_ERROR_CODE
{
    OK,
    IS_NOT_EMPTY,
    IS_EMPTY,
    IS_NOT_VALID
};

class List_t
{
private:
    elem_t* data    = nullptr;
    long* next      = nullptr;
    long* prev      = nullptr;

    bool sorted     = 0;
    long head       = 0;
    long tail       = 0;
    long size       = 0;
    long total_size = 0;
    long next_free  = 0;

    bool list_memory_controller ()
    {
        if ( size == total_size )
        {
            data = (elem_t*)realloc ( data, sizeof (elem_t) * ( size + 1 + ADDITIONAL) );
            next = (long*)  realloc ( next, sizeof (long)   * ( size + 1 + ADDITIONAL) );
            prev = (long*)  realloc ( prev, sizeof (long)   * ( size + 1 + ADDITIONAL) );
            total_size += ADDITIONAL;

            for (long i = size + 1; i <= total_size; i++)
            {
                data [i] = prev [i] = $POISON;
                next [i] = i + 1;
            }
            next [total_size] = 0;
            next_free = size + 1;
        }

        if (data && next && prev) return true;
        else return false;


    }

public:
    List_t ():
        size ( 0 ),
        head ( 0 ),
        tail ( 0 ),
        sorted (false),
        next_free ( 1 ),
        total_size (START_SIZE)
    {
        data = (elem_t*)calloc ( START_SIZE + 1, sizeof (elem_t) );                            //Defend!!!
        next = (long*)  calloc ( START_SIZE + 1, sizeof (long)   );
        prev = (long*)  calloc ( START_SIZE + 1, sizeof (long)   );

        data [0] = next [0] = prev [0] = $POISON;
        for (long i = 1; i <= START_SIZE; i++)
        {
            data [i] = prev [i] = $POISON;
            next [i] = i + 1;
        }
        next [START_SIZE] = 0;
    }

    ~List_t ()
    {
        free (data);
        free (next);
        free (prev);
    }

    LIST_ERROR_CODE InsertFirst ( elem_t elem_val )
    {
        if ( head != 0 && tail != 0 )
        {
            return IS_NOT_EMPTY;
        }
        head = 1;
        tail = 1;
        next_free = 2;
        data [head] = elem_val;
        next [head] = prev [head] = 0;
        size++;
        sorted = true;
        return OK;
    }

    LIST_ERROR_CODE InsertAfter ( long elem_num, elem_t elem_val )
    {
        assert ( elem_num > 0 );
        if ( size == 0 )
        {
            return IS_EMPTY;
        }
        if ( prev [elem_num] == $POISON )
        {
            return IS_NOT_VALID;
        }
        list_memory_controller ();
        assert (next_free);

        size++;

        data [next_free] = elem_val;
        long new_free    = next [next_free];
        next [next_free] = next [elem_num];
        prev [next_free] = elem_num;

        if ( tail == elem_num )
            tail = next_free;
        else
        {
            prev [ next [ elem_num ] ] = next_free;
            sorted = false;
        }

        next [elem_num] = next_free;
        next_free       = new_free;
        return OK;
    }

    LIST_ERROR_CODE InsertBefore ( long elem_num, elem_t elem_val )
    {
        assert ( elem_num > 0 );

        if ( size == 0 )
        {
            return IS_EMPTY;
        }
        if ( prev [elem_num] == $POISON )
        {
            return IS_NOT_VALID;
        }
        list_memory_controller ();

        assert (next_free);

        InsertAfter (prev[elem_num], elem_val);
    }

    LIST_ERROR_CODE Delete ( long elem_num )
    {
        if ( prev [elem_num] == $POISON )
        {
            return IS_NOT_VALID;
        }
        list_memory_controller ();

        size--;

        next [ prev [ elem_num ] ] = next [elem_num];
        prev [ next [ elem_num ] ] = prev [elem_num];


        if ( elem_num == head )
            head = next [ elem_num ];
        else if ( elem_num == tail )
            tail = prev [ elem_num ];
        else
            sorted = false;

        data [elem_num] = $POISON;
        prev [elem_num] = $POISON;
        next [elem_num] = next_free;
        next_free       = elem_num;

        return OK;
    }

    elem_t front ()
    {
        if ( head == 0 )
        {
            return $POISON;
        }
        return data [head];
    }

    elem_t back ()
    {
        if ( tail == 0 )
        {
            return $POISON;
        }
        return data [tail];
    }

    long tell_size ()
    {
        return size;
    }

    elem_t* get_head_ptr ()
    {
        if (!head) return nullptr;
        else return &data[head];
    }

    elem_t* get_tail_ptr ()
    {
        if (!tail) return nullptr;
        else return &data[tail];
    }

    long get_next_index (long index)
    {
        return next[index];
    }

    long get_prev_index (long index)
    {
        return prev[index];
    }

    void draw_list ()
    {
        long counter = 0;
        long elem_p = head;
        FILE* stream = fopen ("list.dot", "w");
        fprintf (stream, "digraph list {\n"
                         "rankdir = \"LR\";\n\n"
                         "node [shape = \"box\"] \n"
                         "\"tail\"[shape = \"circle\", label = \"Tail == %ld\"]\n"
                         "\"head\"[shape = \"circle\", label = \"Head == %ld\"]\n",
                         tail, head);

        while ( elem_p )
        {
            counter++;
            fprintf (stream, "\"list_elem%ld\" [ shape = \"record\", label =  \" Value: %d |"
                                                                                "Logical number: %ld |"
                                                                                "Physical number: %ld |"
                                                                                "<next> Next element: %ld |"
                                                                                "<prev> Previous element: %ld \"]; \n",
                                                                                counter, data [ elem_p ], counter, elem_p , next [ elem_p ], prev [ elem_p ]);
            elem_p = next [ elem_p ];
        }
        fprintf (stream, "\"head\" ->");
        for (long i = 1; i <= counter; i++)
            fprintf (stream, " \"list_elem%ld\": <next> ->", i);
        fprintf (stream, " \"tail\" \n");


        fprintf (stream, "\"tail\" ->");
        for (long i = counter; i >= 1; i--)
            fprintf (stream, " \"list_elem%ld\": <prev> ->", i);
        fprintf (stream, " \"head\" \n}");

        fclose  (stream);
        system  ("dot -Tpng list.dot -o list.png");
    }

    void draw_list (char* arg)
    {
        draw_list ();
        if ( strcmp (arg, "open") == 0 )
            system ("xdg-open list.png");
    }

    long get_index_by_logic (long number)
    {
        if ( sorted )
            return number;

        long elem_p = head;
        if (number > size || number == 0) return 0;
        if (number <= size / 2)
        {
            for (long i = 2; i <= number; i++)
                elem_p = next [elem_p];
            return elem_p;
        }
        else
        {
            elem_p = tail;
            for (long i = size; i > number; i--)
                elem_p = prev [elem_p];
            return elem_p;
        }
    }

    void sort_by_order ()
    {
        long elem_p = head;
        for (long i = 1; i <= size; i++)
        {
            if ( i != elem_p )
                list_elem_swap( i, elem_p );
            elem_p = next [i];
        }
        sorted = true;
    }

    void list_dump ()
    {
        FILE* stream = fopen ("list_dump.dot", "w");
        fprintf (stream, "digraph list {\n");
        for ( long i = 1; i <= total_size; i++)
        {
            fprintf (stream, "\"list_elem%ld\" [ shape = \"record\", label =  \" Value: %d |"
                                                                                "Physical number: %ld |"
                                                                                "Next element: %ld |"
                                                                                "Previous element: %ld \"]; \n",
                                                                                i, data [ i ], i , next [ i ], prev [ i ]);
        }

        for (long i = 1; i <= total_size; i++)
            fprintf (stream, " \"list_elem%ld\" ->", i);
        fprintf (stream, "end;");
        fprintf (stream, "\n}");
        fclose (stream);
        system  ("dot -Tpng list_dump.dot -o list_dump.png");
    }

    void list_dump (char* arg)
    {
        list_dump ();
        if ( strcmp (arg, "open") == 0 )
            system ("xdg-open list_dump.png");
    }

    void list_elem_swap (long left, long right)
    {
        sorted = false;
        assert (left  > 0);
        assert (left  > 0);

        elem_t temp  = data [left];
        data [left]  = data [right];
        data [right] = temp;

        if (prev [left]  > 0)
            next [ prev [ left ] ] = right;
        if (prev [right] > 0)
            next [ prev [ right ] ] = left;

        if (next [left] > 0 )
            prev [ next [ left ] ] = right;
        if (next [right] > 0)
            prev [ next [right] ] = left;

        long t_pos  = next [left];
        next [left]  = next [right];
        next [right] = t_pos;

        t_pos  = prev [left];
        prev [left]  = prev [right];
        prev [right] = t_pos;

        if (head == right)
            head =  left;
        else if (head == left)
            head = right;

        if (tail == right)
            tail =  left;
        else if (tail == left)
            tail = right;
    }

    LIST_ERROR_CODE resize ()
    {
        if (sorted)
            return OK;
        if ( size == 0 )
            return IS_EMPTY;

        long left  = 1;
        long right = total_size;
        while (right > left)
        {
            while (prev [left]  != $POISON)
                left++;
            while (prev [right] == $POISON)
                right--;
            if (right > left)
                list_elem_swap (left, right);
        }

        data = (elem_t*) realloc (data, (size + 2) * sizeof (elem_t) );
        next = (long*)   realloc (next, (size + 2) * sizeof (long)   );
        prev = (long*)   realloc (prev, (size + 2) * sizeof (long)   );
    }
};

int main ()
{
    List_t list;
    list.InsertFirst(1);
    for (int i = 1; i <= 15; i++)
        list.InsertAfter(i, i+1);
    list.Delete(4);
    list.Delete(8);
    list.Delete(13);
    list.Delete(2);
    list.draw_list((char*)"open");
    $p
    list.resize ();
    list.draw_list((char*)"open");
    $p;
    list.sort_by_order();
    list.draw_list((char*)"open");
    return 0;
}

#endif
