++++++++++++++++++++++++++++++++++
 |Boost| Pointer Container Library
++++++++++++++++++++++++++++++++++
 
.. |Boost| image:: cboost.gif

Class ``ptr_set_adapter``
-------------------------

This class is used to build custom pointer containers with
an underlying set-like container. The interface of the class is an extension
of the interface from ``associative_ptr_container``.

**See also:**

- reversible_ptr_container_
- associative_ptr_container_
- ptr_set_

**Navigate:**

- `home <ptr_container.html>`_
- `reference <reference.html>`_

.. _reversible_ptr_container: reversible_ptr_container.html 
.. _associative_ptr_container: associative_ptr_container.html
.. _ptr_set: ptr_set.html

**Synopsis:**

.. parsed-literal::

                     
        namespace boost
        {
            template
            < 
                class Key, 
                class VoidPtrSet,
                class CloneAllocator = heap_clone_allocator 
            >
            class ptr_set_adapter 
            {
                
            public: // `modifiers`_         
                std::pair<iterator,bool>  insert( Key* x );                         
 
            public: // `pointer container requirements`_
                bool      transfer( iterator object, ptr_set_adapter& from );
                size_type transfer( iterator first, iterator last, ptr_set_adapter& from );
                template< class Range >
                size_type transfer( const Range& r, ptr_set_adapter& from );
                size_type transfer( ptr_set_adapter& from );
 
            }; //  class 'ptr_set_adapter'
        
        } // namespace 'boost'  

            
Semantics
---------

.. _`modifiers`:

Semantics: modifiers
^^^^^^^^^^^^^^^^^^^^

- ``std::pair<iterator,bool> insert( key_type* x );``

    - Requirements: ``x != 0``

    - Effects: Takes ownership of ``x`` and insert it if there is no equivalent of it already. The ``bool`` part of the return value indicates insertion and the iterator points to the element with key ``x``.

    - Throws: bad_pointer if ``x == 0``

    - Exception safety: Strong guarantee

.. 
        - ``std::pair<iterator,bool> insert( const key_type& x );``

        - Effects: ``return insert( allocate_clone( x ) );``

        - Exception safety: Strong guarantee

.. _`pointer container requirements`:

Semantics: pointer container requirements
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

- ``bool transfer( iterator object, ptr_set_adapter& from );``

   - Requirements: ``not from.empty()``

   - Effects: Inserts the object defined by ``object`` into the container and remove it from ``from`` 
     iff no equivalent object exists.

   - Returns: whether the object was transfered
   
   - Exception safety: Strong guarantee

- ``void transfer( iterator first, iterator last, ptr__set_adapter& from );``

   - Requirements: ``not from.empty()``

   - Effects: Inserts the objects defined by the range ``[first,last)`` into the container and remove it from ``from``.
     An object is only transferred if no equivalent object exists. 

   - Returns: the number of transfered objects
                 
   - Exception safety: Basic guarantee

- ``template< class Range > void transfer( const Range& r, ptr_set_adapter& from );``

    - Effects: ``return transfer( boost::begin(r), boost::end(r), from );``
                   
- ``size_type transfer( ptr_set_adapter& from );``

   - Effects: ``return transfer( from.begin(), from.end(), from );``.

 
:copyright:     Thorsten Ottosen 2004-2005. 

