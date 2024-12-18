++++++++++++++++++++++++++++++++++
 |Boost| Pointer Container Library
++++++++++++++++++++++++++++++++++
 
.. |Boost| image:: cboost.gif

=========
Reference
=========

The documentation is divided into a an explanation for 
each container. All the common interface is explained only once,
but links are always provided to the relevant links.
Please make sure you understand 
the `Clonable <reference.html#the-clonable-concept>`_ concept and 
the `Clone Allocator <reference.html#the-clone-allocator-concept>`_ concept. 

- `Conventions <conventions.html>`_
- `The Clonable concept`_
- `The Clone Allocator concept`_
- Class `reversible_ptr_container <reversible_ptr_container.html>`_
- Class `associative_ptr_container <associative_ptr_container.html>`_
- `Pointer container adapters`_

  - `ptr_sequence_adapter <ptr_sequence_adapter.html>`_
  - `ptr_set_adapter <ptr_set_adapter.html>`_
  - `ptr_multiset_adapter <ptr_multiset_adapter.html>`_
  - `ptr_map_adapter <ptr_map_adapter.html>`_
  - `ptr_multimap_adapter <ptr_multimap_adapter.html>`_    
- `Sequence containers`_

  - `ptr_vector <ptr_vector.html>`_
  - `ptr_deque <ptr_deque.html>`_
  - `ptr_list <ptr_list.html>`_
  - `ptr_array <ptr_array.html>`_
- `Associative containers`_

  - `ptr_set <ptr_set.html>`_
  - `ptr_multiset <ptr_multiset.html>`_
  - `ptr_map <ptr_map.html>`_
  - `ptr_multimap <ptr_multimap.html>`_
- `Map iterator operations`_  
- `Indirected functions <indirect_fun.html>`_  
- `Class nullable`_     
- `Exception classes`_         




The Clonable concept
++++++++++++++++++++

**Refinement of**

- Copy Constructible
- Heap Allocable
- Heap Deallocable

The Clonable concept is introduced to formalize the requirements for 
copying heap-allocated objects.  A type ``T`` might be Clonable even though it 
is not Assignable or Copy Constructible.  Notice that many operations on 
the containers does not even require the stored type to be Clonable.  

**Notation**

======================= ============================================  =================== =====================
   **Type**                **Object** (``const`` or non-``const``)        **Pointer**        **Describes**
   ``T``                  ``a``                                           ``ptr``            A Clonable type
======================= ============================================  =================== =====================       
       
**Valid expressions**

===================================== =========================== ========================================================================================
     **Expression**                          **Type**                  **Semantics**
   ``new_clone(a);``                         ``T*``                  Allocate a new object that can be considered equivalent to the ``a`` object
   ``delete_clone(ptr);``                    ``void``                Deallocate an object previously allocated with ``allocate_clone()``. Must not throw 
===================================== =========================== ========================================================================================


Default implementation
----------------------

In the ``<boost/ptr_container/clone_allocator.hpp>`` header a default implementation
of the two functions is given:

.. parsed-literal::

    namespace boost
    {
        template< class T >
        inline T* new_clone( const T& t )
        {
            return new T( t );
        }
    
        template< class T >
        void delete_clone( const T* t )
        {
            checked_delete( r );
        }
    }


Notice that this implementation  makes normal Copy Constructible classes are automatically 
Clonable unless ``operator new()`` or ``operator delete()`` are hidden.  

The two functions represent a layer of indirection which is necessary to support 
classes that are not Copy Constructible by default.  Notice that the implementation 
relies on argument-dependent lookup (ADL) to find the right version of 
``new_clone()`` and ``delete_clone()``. This means that one does not need to overload or specialize 
the function is the boost namespace, but it can be placed together with 
the rest of the interface of the class.  If you are implementing a class 
inline in headers, remember to forward declare the functions.

The Clone Allocator concept
+++++++++++++++++++++++++++

The Clone Allocator concept is introduced to formalize the way
pointer containers controls memory of
the stored objects (and not the pointers to the stored objects).
The clone allocator allows
users to apply custom allocators/deallocators for the cloned objects.

More information can be found below:

..  contents:: :depth: 1 
               :local: 


Clone Allocator requirements
----------------------------

**Notation**

===================== ============================================= ==================================================
   **Type**               **Object** (``const`` or non-``const``)                 **Describes**
       ``T``                 ``a``                                   A type
       ``T*``                ``ptr``                                 A pointer to ``T`` 
===================== ============================================= ==================================================

**Valid expressions**

====================================================== ============= ======================================================================================================================================================
     **Expression**                                      **Type**                              **Semantics**
  ``CloneAllocator::allocate_clone(a);``                   ``T*``                          Allocate a new object that can be considered equivalent to the ``a`` object      
  ``CloneAllocator::deallocate_clone(ptr);``              ``void``                          Deallocate an object previously allocated with ``CloneAllocator::allocate_clone()`` or a compatible allocator. Must not throw.
====================================================== ============= ======================================================================================================================================================



The library comes with two predefined clone allocators.

Class ``heap_clone_allocator``
------------------------------

This is the default clone allocator used by all pointer containers. For most
purposes you will never have to change this default. 

**Definition**

.. parsed-literal::

    namespace boost
    {        
        struct heap_clone_allocator
        {
            template< class U >
            static U* allocate_clone( const U& r )
            {
                return new_clone( r );
            }
    
            template< class U >
            static void deallocate_clone( const U* r ) const
            {
                delete_clone( r );
            }
        };
    }

Notice that the above definition allows you to support custom allocation
schemes by relying on ``new_clone()`` and ``delete_clone()``.
   
Class ``view_clone_allocator``
------------------------------

This class provides a way to remove ownership properties of the
pointer containers. As its name implies, this means that you can
instead use the pointer containers as a view into an existing
container.

**Definition**
 
.. parsed-literal::

    namespace boost
    {
        struct view_clone_allocator
        {
            template< class U >
            static U* allocate_clone( const U& r )
            {
                return const_cast<U*>(&r);
            }
    
            template< class U >
            static void deallocate_clone( const U* )
            {
                // empty
            }
        };
    }

**See also**

- `Changing the clone allocator <examples.html#changing-the-clone-allocator>`_

Pointer container adapters
++++++++++++++++++++++++++

The pointer container adapters are used when you
want to make a pointer container starting from
your own "normal" container. For example, you
might have a map class that is extends ``std::map``
in some way; the adapter class then allows you
to use your map class as a basis for a new
pointer container.

The library provides an adapter for each type
of standard container:

- ptr_sequence_adapter_
- ptr_set_adapter_
- ptr_multiset_adapter_
- ptr_map_adapter_
- ptr_multimap_adapter_

.. _ptr_sequence_adapter: ptr_sequence_adapter.html
.. _ptr_set_adapter: ptr_set_adapter.html
.. _ptr_multiset_adapter: ptr_multiset_adapter.html
.. _ptr_map_adapter: ptr_map_adapter.html
.. _ptr_multimap_adapter: ptr_multimap_adapter.html


Pointer containers
++++++++++++++++++

The pointer containers of this library are all built using
the `pointer container adapters`_. There is a pointer container
for each type of "normal" standard container:

Sequence containers
-------------------

    - ptr_vector_
    - ptr_deque_
    - ptr_list_
    - ptr_array_

Associative containers
----------------------

    - ptr_set_
    - ptr_multiset_
    - ptr_map_
    - ptr_multimap_   
     
.. _ptr_vector: ptr_vector.html
.. _ptr_deque: ptr_deque.html
.. _ptr_list: ptr_list.html
.. _ptr_array: ptr_array.html
.. _ptr_set: ptr_set.html
.. _ptr_multiset: ptr_multiset.html
.. _ptr_map: ptr_map.html
.. _ptr_multimap: ptr_multimap.html


Map iterator operations
+++++++++++++++++++++++

The map iterators are a bit different compared to the normal ones.  The 
reason is that it is a bit clumsy to access the key and the mapped object 
through i->first and i->second, and one tends to forget what is what. 
Moreover, and more importantly, we also want to hide the pointer as much as possibble.
The new style can be illustrated with a small example:: 

    typedef ptr_map<string,int> map_t;
    map_t  m;
    m[ "foo" ] = 4; // insert pair
    m[ "bar" ] = 5; // ditto
    ...
    for( map_t::iterator i = m.begin(); i != m.end(); ++i )
    {
             *i += 42; // add 42 to each value
             cout << "value=" << *i << ", key=" << i.key() << "n";
    } 
    
So the difference from the normal map iterator is that 

- ``operator*()`` returns a reference to the mapped object (normally it returns a reference to a ``std::pair``, and
- that the key can be accessed through the ``key()`` function. 

Class ``nullable``
++++++++++++++++++

The purpose of the class is simply to tell the containers
that null values should be allowed. Its definition is
trivial::

    namespace boost
    {
        template< class T >
        struct nullable
        {
            typedef T type;
        };  
    }

Please notice that ``nullable`` has no effect on the containers
interface (except for ``is_null()`` functions). For example, it
does not make sense to do ::

    boost::ptr_vector< boost::nullable<T> > vec;
    vec.push_back( new boost::nullable<T> ); // no no
    boost::nullable<T>& ref = vec[0];        // also no no

Exception classes
+++++++++++++++++

There are three exceptions that are thrown by this library.  The exception 
hierarchy looks as follows::

 
        namespace boost
        {
            class bad_ptr_container_operation : public std::exception
            {
            public:
                bad_ptr_container_operation( const char* what );
            };
            
            class bad_index : public bad_ptr_container_operation
            {
            public:
                bad_index( const char* what );
            };
        
            class bad_pointer : public bad_ptr_container_operation
            {
            public:
                bad_pointer();
                bad_pointer( const char* what );
            };
        }

- `home <ptr_container.html>`_


:copyright:     Thorsten Ottosen 2004-2005. 

