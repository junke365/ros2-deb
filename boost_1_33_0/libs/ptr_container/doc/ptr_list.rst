++++++++++++++++++++++++++++++++++
 |Boost| Pointer Container Library
++++++++++++++++++++++++++++++++++
 
.. |Boost| image:: cboost.gif

Class ``ptr_vector``
--------------------

A ``ptr_list<T>`` is a pointer container that uses an underlying ``std:list<void*>``
to store the pointers. 

**See also:**

- reversible_ptr_container_
- ptr_sequence_adapter_

.. _reversible_ptr_container: reversible_ptr_container.html 
.. _ptr_array: ptr_array.html
.. _`ptr_sequence_adapter`: ptr_sequence_adapter.html

**Navigate:**

- `home <ptr_container.html>`_
- `reference <reference.html>`_


**Synopsis:**

.. parsed-literal::  
           
        namespace boost
        {      
        
            template
            < 
                class T, 
                class CloneAllocator = heap_clone_allocator,
                class Allocator      = std::allocator<void*>
            >
            class ptr_list : public ptr_sequence_adapter
                                    <
                                        T,
                                        std::list<void*,Allocator>,
                                        CloneAllocator
                                    >
            {
            
            public: // modifiers_
                void                push_front( T* x );
                auto_type           pop_front();
             
            public: // `list operations`_
                void  reverse();

            }; // class 'ptr_list'

        } // namespace 'boost'  


Semantics
---------

.. _modifiers:
 
Semantics: modifiers
^^^^^^^^^^^^^^^^^^^^

- ``void push_front( T* x );``

    - Requirements: ``x != 0``

    - Effects: Inserts the pointer into container and takes ownership of it
    
    - Throws: ``bad_pointer`` if ``x == 0``

    - Exception safety: Strong guarantee

..
    - ``void push_front( const T& x );``
    
        - Effects: push_front( allocate_clone( x ) );
    
        - Exception safety: Strong guarantee

- ``auto_type pop_front():``

    - Requirements:``not empty()``
    
    - Effects: Removes the first element in the container

    - Postconditions: ``size()`` is one less

    - Throws: ``bad_ptr_container_operation`` if ``empty() == true``
    
    - Exception safety: Strong guarantee

.. _`list operations`:

Semantics: list operations
^^^^^^^^^^^^^^^^^^^^^^^^^^

..
    - ``void splice( iterator before, ptr_list& x );``
    
        - Requirements:``&x != this``
    
        - Effects: inserts the all of ``x``'s elements before ``before``
    
        - Postconditions: ``x.empty()``
        
        - Throws: nothing
    
        - Remark: prefer this to ``transfer( before, x );``
    
    - ``void  splice( iterator before, ptr_list& x, iterator i );``
    
        - Not ready yet
    
    - ``void splice( iterator before, ptr_list& x, iterator first, iterator last );``
    
        - Not ready yet

    - ``void merge( ptr_list& x );``
    
        - Not ready yet
         
    - ``template< typename Compare > 
      void merge( ptr_list& x, Compare comp );``
    
        - Not ready yet
    
- ``void reverse();``

    - Effects: reverses the underlying sequence

    - Throws: nothing


:copyright:     Thorsten Ottosen 2004-2005. 

