++++++++++++++++++++++++++++++++++
 |Boost| Pointer Container Library
++++++++++++++++++++++++++++++++++
 
.. |Boost| image:: cboost.gif

Class ``ptr_array``
-------------------

A ``ptr_array<T,size>`` is a pointer container that uses an underlying ``boost::array<void*,size>``
to store the pointers. The class is useful when there is no requirement
of dynamic expansion and when absolute no overhead is tolerable.

**See also:**

- reversible_ptr_container_
- ptr_sequence_adapter_
- ptr_vector_

.. _reversible_ptr_container: reversible_ptr_container.html 
.. _ptr_sequence_adapter: ptr_sequence_adapter.html
.. _ptr_vector: ptr_vector.html

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
                size_t N, 
                CloneAllocator = heap_clone_allocator
            >
            class ptr_array : public *implementation-defined*
            {
            public: // `construct/copy/destroy`_
                ptr_array();
                ptr_array( std::auto_ptr<ptr_array>& r );

            public: // `iterators`_

            public: // `capacity`_

            public: // `element access`_
                T&        front();
                const T&  front() const;
                T&        back();
                const T&  back() const;
                
                template< size_t idx >
                T&        at();
                template< size_t idx >
                const T&  at() const;
                T&        at( size_t );
                const T&  at( size_t );

                T&        operator[]( size_t );
                const T&  operator[]( size_t ) const;

            public: // `modifiers`_
                void  swap( ptr_array& r );
 
                template< size_t idx >
                auto_type replace( T* r );
                auto_type replace( size_t idx, T* r );

            public: // `pointer container requirements`_
                std::auto_ptr<ptr_array>  clone() const;    
                std::auto_ptr<ptr_array>  release();
                template< size_t idx >
                bool                      is_null() const;
                bool                      is_null( size_t idx ) const;
             
            }; //  class 'ptr_sequence_adapter'

        } // namespace 'boost'  

.. _iterators: reversible_ptr_container.html#iterators

.. _capacity: reversible_ptr_container.html#capacity

.. _`inherited element access`: reversible_ptr_container.html#element-access

Semantics
---------

.. _`construct/copy/destroy`:

Semantics: construct/copy/destroy
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

- ``ptr_array();``

    - Effects: construct array where each element is null
    
- ``ptr_array( std::auto_ptr<ptr_array>& r );``

    - Effects: take ownership of the supplied pointers
    
.. _`element access`:

Semantics: element access
^^^^^^^^^^^^^^^^^^^^^^^^^


- ``T&       front();``
- ``const T& front() const;``

    - Requirements: ``not empty();``

    - Effects: ``return *begin();``

    - Throws: ``bad_ptr_container_operation`` if ``empty() == true``


- ``T&       back();``
- ``const T& back() const;``

    - Requirements: ``not empty();``

    - Effects: ``return *--end();``

    - Throws: ``bad_ptr_container_operation`` if ``empty() == true``

- ``template< size_t idx > T&       at( size_type n );``
- ``template< size_t idx > const T& at( size_type n ) const;``

    - Requirements: ``idx < size()`` (compile-time enforced)

    - Effects: Returns a reference to the ``n``'th element

    - Throws: nothing

- ``T&       at( size_type n );``
- ``const T& at( size_type n ) const;``

    - Requirements: ``n < size()``

    - Effects: Returns a reference to the ``n``'th element

    - Throws: ``bad_index`` if ``n >=size()``


- ``T&       operator[]( size_type n );``
- ``const T& operator[]( size_type n ) const;``

    - Requirements: ``n < size()``

    - Effects: Returns a reference to the ``n``'th element

    - Throws: Nothing


.. _`modifiers`:

Semantics: modifiers
^^^^^^^^^^^^^^^^^^^^

- ``void swap( ptr_array& r );``

    - Effects: swaps the two arrays
    
    - Complexity: Linear

    - Throws: nothing
    
- ``template< size_t idx > auto_type replace( T* r );``

    - Requirements:
     
            - ``idx < size()`` (compile-time enforced)
            - ``r != 0``

    - Effects: returns the object indexed by ``idx`` and replaces it with ``r``.

    - Throws: ``bad_pointer`` if ``x == 0``.

    - Exception safety: Strong guarantee

- ``auto_type replace( size_t idx, T* r );``
        
    - Requirements: `` x != 0 and idx < size()``

    - Effects: returns the object indexed by ``idx`` and replaces it with ``x``.

    - Throws: ``bad_index`` if ``idx >= size()`` and ``bad_pointer`` if ``x == 0``.

    - Exception safety: Strong guarantee


.. _`pointer container requirements`:

Semantics: pointer container requirements
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

- ``std::auto_ptr<ptr_array>  clone() const;``

    - Effects: Returns a deep copy of the container

    - Throws: ``std::bad_alloc`` if there is not enough memory to make a clone of the container

    - Complexity: Linear

    
- ``std::auto_ptr<ptr_array>  release();``
     
    - Effects: Releases ownership of the container. This is a useful way of returning a container from a function.

    - Postconditions: ``empty() == true`` and all pointers are null

    - Throws: ``std::bad_alloc`` if the return value cannot be allocated

    - Exception safety: Strong guarantee


- ``template< size_t idx > bool is_null() const;``

    - Requirements: ``idx < size()`` (compile-time enforced)

    - Effects: returns whether the pointer at index ``idx`` is null

    - Exception safety: Nothrow guarantee

- ``bool is_null( size_type idx ) const;``

    - Requirements: ``idx < size()``

    - Effects: returns whether the pointer at index ``idx`` is null

    - Exception safety: Nothrow guarantee

:copyright:     Thorsten Ottosen 2004-2005. 

