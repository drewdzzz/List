#include <iostream>
#include <stdlib.h>
#include <assert.h>
#include <cstring>

#define $p getchar ();


typedef int elem_t;

static const int $POISON = -666;

static const int START_SIZE = 20;
static const int ADDITIONAL = 10;

class List_t
{
private:
    elem_t* data   = nullptr;
    long* next     = nullptr;
    long* prev     = nullptr;

    long head      = 0;
    long tail      = 0;
    long size      = 0;
    long next_free = 0;

public:
    List_t (): size ( 0 ), head ( 0 ), tail ( 0 ), next_free ( 1 )
    {
        data = (elem_t*)calloc ( START_SIZE + 1, sizeof (elem_t) );
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

    bool InsertFirst ( elem_t elem_val )
    {
        if ( head != 0 && tail != 0 )
        {
            fprintf (stderr, "Can't do  Insert First: List is not empty\n");
            return false;
        }
        head = 1;
        tail = 1;
        next_free = 2;
        data [head] = elem_val;
        next [head] = prev [head] = 0;
        size++;
        return true;
    }

    bool InsertAfter ( long elem_num, elem_t elem_val )
    {
        assert ( elem_num > 0 );
        if ( size == 0 )
        {
            fprintf (stderr, "List is empty! Starting InsertFirst\n");
            if ( InsertFirst (elem_val) ) return true;
            else return false;
        }
        if ( prev [elem_num] == $POISON )
        {
            fprintf (stderr, "Indicated number is not part of the list\n");
            return false;
        }
        //Allocation for bigger elements must be here
        //Checking for next_free != 0
        size++;

        data [next_free] = elem_val;
        long new_free    = next [next_free];
        next [next_free] = next [elem_num];
        prev [next_free] = elem_num;

        if ( tail != next_free)
            prev [ next [ elem_num ] ] = next_free;

        next [elem_num] = next_free;
        next_free       = new_free;
    }

    bool Delete ( long elem_num )
    {
        if ( prev [elem_num] == $POISON )
        {
            fprintf (stderr, "Indicated number is not part of the list\n");
            return false;
        }
        //Reallocation fo small list
        size--;

        next [ prev [ elem_num ] ] = next [elem_num];
        prev [ next [ elem_num ] ] = prev [elem_num];


        if ( elem_num == head )
            head = next [ elem_num ];
        if ( elem_num == tail )
            tail = prev [ elem_num ];

        data [elem_num] = $POISON;
        prev [elem_num] = $POISON;
        next [elem_num] = next_free;
        next_free       = elem_num;

        return true;
    }

    elem_t front ()
    {
        if ( head == 0 )
        {
            fprintf (stderr, "Your list is empty!\n");
            return $POISON;
        }
        return data [head];
    }

    elem_t back ()
    {
        if ( tail == 0 )
        {
            fprintf (stderr, "Your list is empty!\n");
            return $POISON;
        }
        return data [tail];
    }

    long tell_size ()
    {
        return size;
    }

    void draw_list ()
    {
        long counter = 0;
        long elem_p = head;
        FILE* stream = fopen ("dirgaph.dot", "w");
        fprintf (stream, "digraph list {\n"
                         "rankdir = \"LR\";\n\n"
                         "node [shape = \"box\"] \n"
                         "\"tail\"[shape = \"circle\", label = \"Tail\"]\n"
                         "\"head\"[shape = \"circle\", label = \"Head\"]\n");
        
        while ( elem_p )
        {
            counter++;
            fprintf (stream, "\"list_elem%ld\" [ shape = \"record\", label =  \" Value: %d |" 
                                                                                "Logical number: %ld |" 
                                                                                "Physical number: %ld |" 
                                                                                "Next element: %ld |" 
                                                                                "Previous element: %ld \"]; \n",
                                                                                counter, data [ elem_p ], counter, elem_p , next [ elem_p ], prev [ elem_p ]);
            elem_p = next [ elem_p ];
        }
        fprintf (stream, "\"head\" ->");

        for (long i = 1; i <= counter; i++)
            fprintf (stream, " \"list_elem%ld\" ->", i);
        fprintf (stream, " \"tail\" \n}");
        fclose  (stream);
        system  ("dot -Tpng dirgaph.dot -o test.png");
    }

    void draw_list (char* arg)
    {
        draw_list ();
        if ( strcmp (arg, "open") == 0 )
            system ("xdg-open test.png");
    }


};

int main ()
{
    List_t list;
    
    list.InsertFirst (10);
    list.draw_list ((char*)"open");
    $p;
    list.InsertAfter (1, 20);
    list.draw_list ();
    $p;
    list.InsertAfter (2, 30);
    list.draw_list ();
    $p;
    list.Delete (1);
    list.draw_list ();
    $p;
    list.InsertAfter (2, 25);
    list.draw_list ();
    
    return 0;
}
