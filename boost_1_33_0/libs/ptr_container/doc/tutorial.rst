++++++++++++++++++++++++++++++++++
 |Boost| Pointer Container Library
++++++++++++++++++++++++++++++++++
 
.. |Boost| image:: cboost.gif

======== 
Tutorial 
======== 

The tutorial shows you the most simple usage of the
library. It is assumed that the reader is familiar
with the use of standard containers. Although
the tutorial is devided into sections, it is recommended
that you read it all from top to bottom.

* `Basic usage`_
* `Indirected interface`_
* `Sequence containers`_
* `Associative containers`_
* `Null values`_
* `Clonability`_
* `New functions`_
* `Algorithms`_

Basic usage
-----------

The most important aspect of a pointer container is that it manages
memory for you. This means that you in most cases do not need to worry
about deleting memory. 

Let us assume that we have an OO-hierarchy of animals

.. parsed-literal::

    class animal : `boost::noncopyable <http://www.boost.org/libs/utility/utility.htm#Class_noncopyable>`_
    {
    public:
        virtual      ~animal() {}
        virtual void eat() = 0;
        // ...
    };
    
    class mammal : public animal
    {
        // ...
    };
    
    class bird : public animal
    {
        // ...
    };


Then the managing of the animals is straight-forward. Imagine a 
Zoo::

    class zoo
    {
        boost::ptr_vector<animal> the_animals;
    public:

        void add_animal( animal* a )
        {
            the_animals.push_back( a );
        }
    };

Notice how just pass the class name to the container; there
is no ``*`` to indicate it is a pointer.
With this declaration we can now say::
    
    zoo the_zoo;
    the_zoo.add_animal( new mammal("joe") );
    the_zoo.add_animal( new bird("dodo") );

Thus we heap-allocate all elements of the container
and never rely on copy-semantics. 

Indirected interface
--------------------

As particular feature of the pointer containers is that
the query interface is indirected. For example, ::

    boost::ptr_vector<animal> vec;
    vec.push_back( new animal ); // you add it as pointer ...
    vec[0].eat();                // but get a reference back

This indirection also happens to iterators, so ::

    typedef std::vector<animal*> std_vec;
    std_vec vec;
    ...
    std_vec::iterator i = vec.begin();
    (*i)->eat(); // '*' needed
    
now becomes ::
   
    typedef boost::ptr_vector<animal>  ptr_vec;
    ptr_vec vec;
    ptr_vec::iterator i = vec.begin();
    i->eat(); // no indirection needed
    

Sequence containers
-------------------

The sequence containers used when you do not need to
keep an ordering on your elements. You can basically
expect all operations of the normal standard containers
to be available. So, for example, with a  ``ptr_deque``
and ``ptr_list`` object you can say::

    boost::ptr_deque<animal> deq;
    deq.push_front( new animal );    
    deq.pop_front();

because ``std::deque`` and ``std::list`` has ``push_front()``
and ``pop_front`` members. 

If the standard sequence support
random access, so does the pointer container; for example::

    for( boost::ptr_deque<animal>::size_type i = 0u;
         i != deq.size(); ++i )
         deq[i].eat();

The ``ptr_vector`` also allows you to specify the size of
the buffer to allocate; for example ::

    boost::ptr_vector<animal> animals( 10u );

will reserve room for 10 animals.              

Associative containers
----------------------

To keep an ordering on our animals, we could use a ``ptr_set``::

    boost::ptr_set<animal> set;
    set.insert( new monkey("bobo") );
    set.insert( new whale("anna") );
    ...
    
This requires that ``operator<()`` is defined for animals. One
way to do this could be ::

    inline bool operator<( const animal& l, const animal& r )
    {
        return l.name() < r.name();
    }
    
if we wanted to keep the animals sorted by name.

Maybe you want to keep all the animals in zoo ordered wrt.
their name, but it so happens that many animals have the
same name. We can then use a ``ptr_multimap``::

    typedef boost::ptr_multimap<std::string,animal> zoo_type;
    zoo_type zoo;
    std::string bobo = "bobo",
                anna = "anna";
    zoo.insert( bobo, new monkey(bobo) );
    zoo.insert( bobo, new elephant(bobo) );
    zoo.insert( anna, new whale(anna) );
    zoo.insert( anna, new emu(anna) );
    
Note that must create the key as an lvalue 
(due to exception-safety issues); the following would not 
have compiled ::

    zoo.insert( "bobo", // this is bad, but you get compile error
                new monkey("bobo") );

If a multimap is not needed, we can use ``operator[]()``
to avoid the clumsiness::

    boost::ptr_map<std::string,animal> animals;
    animals["bobo"].set_name("bobo");

This requires a default constructor for animals and
a function to do the initialization, in this case ``set_name()``;
    
Null values
-----------

By default, if you try to insert null into a container, an exception
is thrown. If you want to allow nulls, then you must
say so explicitly when declaring the container variable ::

    boost::ptr_vector< boost::nullable<animal> > animals_type;
    animals_type animals;
    ...
    animals.insert( animals.end(), new dodo("fido") );
    animals.insert( animals.begin(), 0 ) // ok

Once you have inserted a null into the container, you must
always check if the value is null before accessing the object ::

    for( animals_type::iterator i = animals.begin();
         i != animals.end(); ++i )
    {
        if( !boost::is_null(i) ) // always check for validity
            i->eat();
    }

If the container support random access, you may also check this as ::

    for( animals_type::size_type i = 0u; 
         i != animals.size(); ++i )
    {
        if( !animals.is_null(i) )
             animals[i].eat();
    }

Note that it is meaningless to insert
null into ``ptr_set`` and ``ptr_multiset``. 

Clonability
-----------

In OO programming it is typical to prohibit copying of objects; the 
objects may sometimes be allowed to be clonable; for example,::

    animal* animal::clone() const
    {
        return do_clone(); // implemented by private virtual function
    }

If the OO hierarchy thus allows cloning, we need to tell the 
pointer containers how cloning is to be done. This is simply
done by defining a free-standing function, ``new_clone()``, 
in the same namespace as
the object hierarchy::

    inline animal* new_clone( const animal& a )
    {
        return a.clone();
    }

That is all, now a lot of functions in a pointer container
can exploit the clonability of the animal objects. For example ::

    typedef boost::ptr_list<animal> zoo_type;
    zoo_type zoo, another_zoo;
    ...
    another_zoo.assign( zoo.begin(), zoo.end() );

will fill another zoo with clones of the first zoo. Similarly,
insert() can now insert clones into your pointer container ::

    another_zoo.insert( another_zoo.begin(), zoo.begin(), zoo.end() );

The whole container can now also be cloned ::

    zoo_type yet_another_zoo = zoo.clone();
    

New functions
-------------

Given that we know we are working with pointers, a few new functions
make sense. For example, say you want to remove an
animal from the zoo ::

    zoo_type::auto_type the_animal = zoo.release( zoo.begin() );
    the_animal->eat();
    animal* the_animal_ptr = the_animal.release(); // now this is not deleted
    zoo.release(2); // for random access containers

You can think of ``auto_type`` as a non-copyable form of 
``std::auto_ptr``. Notice that when you release an object, the
pointer is removed from the container and the containers size
shrinks. You can also release the entire container if you
want to return it from a function ::

    std::auto_ptr< boost::ptr_deque<animal> > get_zoo()
    {
        boost::ptr_deque<animal>  result;
        ...
        return result.release(); // give up ownership
    }
    ...
    boost::ptr_deque<animal> animals = get_zoo();    

Let us assume we want to move an animal object from
one zoo to another. In other words, we want to move the 
animal and the responsibility of it to another zoo ::
    
    another_zoo.transfer( another_zoo.end(), // insert before end 
                          zoo.begin(),       // insert this animal ...
                          zoo );             // from this container
    
This kind of "move-semantics" is different from
normal value-based containers. You can think of ``transfer()``
as the same as ``splice()`` on ``std::list``.

If you want to replace an element, you can easily do so ::

    zoo_type::auto_type old_animal = zoo.replace( zoo.begin(), new monkey("bibi") ); 
    zoo.replace( 2, old_animal.release() ); // for random access containers

A map is a little different to iterator over than standard maps.
Now we say ::

    typedef boost::ptr_map<std::string, boost::nullable<animal> > animal_map;
    animal_map map;
    ...
    for( animal_map::iterator i = map.begin();
         i != map.end(); ++i )
    {
        std::cout << "\n key: " << i.key();
        std::cout << "\n age: ";
        
        if( boost::is_null(i) )
            std::cout << "unknown";
        else
            std::cout << i->age(); 
     }
            
Maps can also be indexed with bounds-checking ::

    try
    {
        animal& bobo = map.at("bobo");
    }
    catch( boost::bad_ptr_container_operation& e )
    {
        // "bobo" not found
    }        

Algorithms
----------

Unfortunately it is not possible to use pointer containers with
mutating algorithms from the standard library. However,
the most useful ones
are instead provided as member functions::

    boost::ptr_vector<animal> zoo;
    ...
    zoo.sort();                               // assume 'bool operator<( const animal&, const animal& )'
    zoo.sort( std::less<animal>() );          // the same, notice no '*' is present
    zoo.sort( zoo.begin(), zoo.begin() + 5 ); // sort selected range

Notice that predicates are automatically wrapped in an `indirect_fun`_ object.

..  _`indirect_fun`: indirect_fun.html

You can remove equal and adjacent elements using ``unique()``::
   
    zoo.unique();                             // assume 'bool operator==( const animal&, const animal& )'
    zoo.unique( zoo.begin(), zoo.begin() + 5, my_comparison_predicate() ); 

If you just want to remove certain elements, use ``erase_if``::

    zoo.erase_if( my_predicate() );

Finally you may want to merge together two sorted containers::

    boost::ptr_vector<animal> another_zoo = ...;
    another_zoo.sort();                      // sorted wrt. to same order as 'zoo'
    zoo.merge( another_zoo );
    BOOST_ASSERT( another_zoo.empty() );    
         
That is all; now you have learned all the basics!

**Navigate**

  - `home <ptr_container.html>`_
  - `examples <examples.html>`_


:copyright:     Thorsten Ottosen 2004-2005. 

