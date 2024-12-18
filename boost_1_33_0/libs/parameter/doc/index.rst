+++++++++++++++++++++++++++++++++++++++++++++++++
 The Boost Parameter Library 
+++++++++++++++++++++++++++++++++++++++++++++++++

|(logo)|__

.. |(logo)| image:: ../../../../boost.png
   :alt: Boost

__ ../../../../index.htm

-------------------------------------

:Abstract: Use this library to write functions that accept
  arguments by name:

  .. parsed-literal::

    new_window("alert", **width=10**, **titlebar=false**);

  Since named arguments can be passed in any order, they are
  especially useful when a function has more than one parameter
  with a useful default value.

-------------------------------------

:Authors:       David Abrahams, Daniel Wallin
:Contact:       dave@boost-consulting.com, dalwan01@student.umu.se
:Organization:  `Boost Consulting`_
:Date:          $Date: 2005/07/18 20:34:31 $

:Copyright:     Copyright David Abrahams, Daniel Wallin
                2005. Distributed under the Boost Software License,
                Version 1.0. (See accompanying file LICENSE_1_0.txt
                or copy at http://www.boost.org/LICENSE_1_0.txt)

.. _`Boost Consulting`: http://www.boost-consulting.com

.. _concepts: ../../../more/generic_programming.html#concept

-------------------------------------

.. contents:: **Table of Contents**

.. role:: concept
   :class: concept

.. role:: vellipsis
   :class: vellipsis

.. section-numbering::

-------------------------------------

==============
 Introduction
==============

In C++, arguments are normally given meaning by their positions
with respect to a parameter list.  That protocol is fine when there
is at most one parameter with a default value, but when there are
even a few useful defaults, the positional interface becomes
burdensome:

* Since an argument's meaning is given by its position, we have to
  choose an (often arbitrary) order for parameters with default
  values, making some combinations of defaults unusable:

  .. parsed-literal::

    window* new_window(
       char const* name, 
       **int border_width = default_border_width,**
       bool movable = true,
       bool initially_visible = true
       );

    const bool movability = false;
    window* w = new_window("alert box", movability);

  In the example above we wanted to make an unmoveable window
  with a default ``border_width``, but instead we got a moveable
  window with a ``border_width`` of zero.  To get the desired
  effect, we'd need to write:

  .. parsed-literal::

    window* w = new_window(
       "alert box", **default_border_width**, movability);


* It can become difficult for readers to understand the meaning of
  arguments at the call site::

    window* w = new_window("alert", 1, true, false);

  Is this window moveable and initially invisible, or unmoveable
  and initially visible?  The reader needs to remember the order
  of arguments to be sure.  

* The author of the call may not remember the order of the
  arguments either, leading to hard-to-find bugs.

This library addresses the problems outlined above by associating
each parameter with a keyword object.  Now users can identify
arguments by keyword, rather than by position:

.. parsed-literal::

  window* w = new_window("alert box", **movable=**\ false); // OK!

.. I'm inclined to leave this part out.  In particular, the 2nd
   point is kinda lame because even with the library, we need to
   introduce overloads -- dwa:

   C++ has two other limitations, with respect to default arguments,
   that are unrelated to its positional interface:

   * Default values cannot depend on the values of other function
     parameters:

     .. parsed-literal::

       // Can we make resize windows to a square shape by default?
       void resize(
         window* w,
         int **width**, 
         int height **= width** // nope, error!
       );

   * Default values in function templates are useless for any
     argument whose type should be deduced when the argument is
     supplied explicitly::

        template <class T> 
        void f(T x = 0);

        f(3.14) // ok: x supplied explicitly; T is double
        f();    // error: can't deduce T from default argument 0!

   As a side effect of using the Boost Parameter library, you may find
   that you circumvent both of these limitations quite naturally.

==========
 Tutorial
==========

In this section we'll show how the Parameter library can be used to
build an expressive interface to the `Boost Graph library`__\ 's
|dfs|_ algorithm. [#old_interface]_ After laying some groundwork
and describing the algorithm's abstract interface, we'll show you
how to build a basic implementation with keyword support.  Then
we'll add support for default arguments and we'll gradually refine the
implementation with syntax improvements.  Finally we'll show how to
streamline the implementation of named parameter interfaces,
improve their participation in overload resolution, and optimize
their runtime efficiency.

__ ../../../graph/index.html

.. _dfs: ../../../graph/doc/depth_first_search.html

.. |dfs| replace:: ``depth_first_search``


Headers And Namespaces
======================

Most components of the Parameter library are declared in a
header named for the component.  For example, ::

  #include <boost/parameter/keyword.hpp>

will ensure ``boost::parameter::keyword`` is known to the
compiler.  There is also a combined header,
``boost/parameter.hpp``, that includes most of the library's
components.  For the the rest of this tutorial, unless we say
otherwise, you can use the rule above to figure out which header
to ``#include`` to access any given component of the library.

Also, the examples below will also be written as if the
namespace alias ::

  namespace parameter = boost::parameter;

has been declared: we'll write ``parameter::xxx`` instead of
``boost::parameter::xxx``.

The Abstract Interface to |dfs|
===============================

The Graph library's |dfs| algorithm is a generic function accepting
from one to four arguments by reference.  If all arguments were
required, its signature might be as follows::

   template <
       class Graph, class DFSVisitor, class Index, class ColorMap
   >
   void depth_first_search(
     , Graph const& graph 
     , DFSVisitor visitor
     , typename graph_traits<g>::vertex_descriptor root_vertex
     , IndexMap index_map
     , ColorMap& color);

However, most of the parameters have a useful default value, as
shown in the table below.

.. _`parameter table`: 
.. _`default expressions`: 

.. table:: ``depth_first_search`` Parameters

  +----------------+----------+----------------------------------+
  | Parameter Name | Dataflow | Default Value (if any)           |
  +================+==========+==================================+
  |``graph``       | in       |none - this argument is required. |
  +----------------+----------+----------------------------------+
  |``visitor``     | in       |``boost::dfs_visitor<>()``        |
  +----------------+----------+----------------------------------+
  |``root_vertex`` | in       |``*vertices(graph).first``        |
  +----------------+----------+----------------------------------+
  |``index_map``   | in       |``get(boost::vertex_index,graph)``|
  +----------------+----------+----------------------------------+
  |``color_map``   | out      |an ``iterator_property_map``      |
  |                |          |created from a ``std::vector`` of |
  |                |          |``default_color_type`` of size    |
  |                |          |``num_vertices(graph)`` and using |
  |                |          |``index_map`` for the index map.  |
  +----------------+----------+----------------------------------+

Don't be intimidated by the complex default values.  For the
purposes of this exercise, you don't need to understand what they
mean. Also, we'll show you how the default for ``color_map`` is
computed later in the tutorial; trust us when we say that the
complexity of its default will become valuable.

Defining the Keywords
=====================

The point of this exercise is to make it possible to call
``depth_first_search`` with keyword arguments, leaving out any
arguments for which the default is appropriate:

.. parsed-literal::

  graphs::depth_first_search(g, **color_map = my_color_map**);

To make that syntax legal, there needs to be an object called
``color_map`` with an assignment operator that can accept a
``my_color_map`` argument.  In this step we'll create one such
**keyword object** for each parameter.  Each keyword object will be
identified by a unique **keyword tag type**.  

We're going to define our interface in namespace ``graphs``.  Since
users need access to the keyword objects, but not the tag types,
we'll define the keyword objects so they're acceessible through
``graphs``, and we'll hide the tag types away in a tested
namespace, ``graphs::tag``.  The library provides a convenient
macro for that purpose (MSVC6.x users see this note__)::

  #include <boost/parameter/keyword.hpp>

  namespace graphs
  {
    BOOST_PARAMETER_KEYWORD(tag, graph)    // Note: no semicolon
    BOOST_PARAMETER_KEYWORD(tag, visitor)
    BOOST_PARAMETER_KEYWORD(tag, root_vertex)
    BOOST_PARAMETER_KEYWORD(tag, index_map)
    BOOST_PARAMETER_KEYWORD(tag, color_map)
  }

__ `Compiler Can't See References In Unnamed Namespace`_

The declaration of the ``visitor`` keyword you see here is
equivalent to::

  namespace graphs 
  {
    namespace tag { struct visitor; }
    namespace { 
      boost::parameter::keyword<tag::visitor>& visitor
      = boost::parameter::keyword<tag::visitor>::get();
    }
  }

This “fancy dance” involving the unnamed namespace and references
is all done to avoid violating the One Definition Rule (ODR)
[#odr]_ when the named parameter interface is used by function
templates that are instantiated in multiple translation
units.

Defining the Implementation Function
====================================

Next we can write the skeleton of the function that implements
the core of ``depth_first_search``::

  namespace graphs { namespace core
  {
    template <class ArgumentPack>
    void depth_first_search(ArgumentPack const& args)
    {
        // algorithm implementation goes here
    }
  }}

.. |ArgumentPack| replace:: :concept:`ArgumentPack`

``core::depth_first_search`` has an |ArgumentPack|
parameter: a bundle of references to the arguments that the caller
passes to the algorithm, tagged with their keywords.  To extract
each parameter, just pass its keyword object to the
|ArgumentPack|\ 's subscript operator.  Just to get a feel for how
things work, let's add some temporary code to print the arguments:

.. parsed-literal::

  namespace graphs { namespace core
  {
    template <class ArgumentPack>
    void depth_first_search(ArgumentPack const& args)
    {
        std::cout << "graph:\\t" << **args[graph]** << std::endl;
        std::cout << "visitor:\\t" << **args[visitor]** << std::endl;
        std::cout << "root_vertex:\\t" << **args[root_vertex]** << std::endl;
        std::cout << "index_map:\\t" << **args[index_map]** << std::endl;
        std::cout << "color_map:\\t" << **args[color_map]** << std::endl;
    }
  }} // graphs::core

It's unlikely that many of the arguments the caller will eventually
pass to ``depth_first_search`` can be printed, but for now the code
above will give us something to experiment with.  To see the
keywords in action, we can write a little test driver:

.. parsed-literal::

  int main()
  {
      using namespace graphs;

      core::depth_first_search(**(**
        graph = 'G', visitor = 2, root_vertex = 3.5, 
        index_map = "hello, world", color_map = false\ **)**);
  }

An overloaded comma operator (``operator,``) combines the results
of assigning to each keyword object into a single |ArgumentPack|
object that gets passed on to ``core::depth_first_search``.  The
extra set of parentheses you see in the example above are required:
without them, each assignment would be interpreted as a separate
function argument and the comma operator wouldn't take effect.
We'll show you how to get rid of the extra parentheses later in
this tutorial.

Of course, we can pass the arguments in any order::

  int main()
  {
      using namespace graphs;

      core::depth_first_search((
        root_vertex = 3.5, graph = 'G', color_map = false, 
        index_map = "hello, world", visitor = 2));
  }

either of the two programs above will print::

  graph:       G
  visitor:     2
  root_vertex: 3.5
  index_map:   hello, world
  color_map:   false

Adding Defaults
===============

Currently, all the arguments to ``depth_first_search`` are
required.  If any parameter can't be found, there will be a
compilation error where we try to extract it from the
|ArgumentPack| using the subscript operator.  To make it
legal to omit an argument we need to give it a default value.

Syntax
------

We can make any of the parameters optional by following its keyword
with the ``|`` operator and the parameter's default value within
the square brackets.  In the following example, we've given
``root_vertex`` a default of ``42`` and ``color_map`` a default of
``"hello, world"``.

.. parsed-literal::

  namespace graphs { namespace core
  {
    template <class ArgumentPack>
    void depth_first_search(ArgumentPack const& args)
    {
        std::cout << "graph:\\t" << args[graph] << std::endl;
        std::cout << "visitor:\\t" << args[visitor] << std::endl;
        std::cout << "root_vertex:\\t" << args[root_vertex\ **|42**\ ] << std::endl;
        std::cout << "index_map:\\t" << args[index_map] << std::endl;
        std::cout << "color_map:\\t" << args[color_map\ **|"hello, world"**\ ] << std::endl;
    }
  }} // graphs::core

Now we can invoke the function without supplying ``color_map`` or
``root_vertex``::

  core::depth_first_search((
    graph = 'G', index_map = "index", visitor = 6));

The call above would print::

  graph:       G
  visitor:     6
  root_vertex: 42
  index_map:   index
  color_map:   hello, world

.. Important::

   The index expression ``args[…]`` always yields a *reference*
   that is bound either to the actual argument passed by the caller
   or, if no argument is passed explicitly, to the specified
   default value.

Getting More Realistic
----------------------

Now it's time to put some more realistic defaults in place.  We'll
have to give up our print statements—at least if we want to see the
defaults work—since, the default values of these
parameters generally aren't printable.

Instead, we'll connect local variables to the arguments and use
those in our algorithm:

.. parsed-literal::

  namespace graphs { namespace core
  {
    template <class ArgumentPack>
    void depth_first_search(ArgumentPack const& args)
    {
        *Graph*   g = args[graph];
        *Visitor* v = args[visitor|\ *default-expression*\ :sub:`1`\ ];
        *Vertex*  s = args[root_vertex|\ *default-expression*\ :sub:`2`\ ];
        *Index*   i = args[index_map|\ *default-expression*\ :sub:`3`\ ];
        *Color*   c = args[visitor|\ *default-expression*\ :sub:`4`\ ];

        *…use g, v, s, i, and c to implement the algorithm…*
    }
  }} // graphs::core

We'll insert the `default expressions`_ in a moment, but first we
need to come up with the types *Graph*, *Visitor*, *Vertex*,
*Index*, and *Color*.

The ``binding`` |Metafunction|_
-------------------------------

To compute the type of a parameter we can use a |Metafunction|_
called ``binding``:

.. parsed-literal::

  binding<ArgumentPack, Keyword, Default = void>
  { typedef *see text* type; };

where ``Default`` is the type of the default argument, if any.

For example, to declare and initialize ``g`` above, we could write:

.. parsed-literal::

  typedef typename parameter::binding<
    ArgumentPack,\ **tag::graph**
  >::type Graph;

  Graph g = args[graph];

As shown in the `parameter table`_, ``graph`` has no default, so
the ``binding`` invocation for *Graph* takes only two arguments.
The default ``visitor`` is ``boost::dfs_visitor<>()``, so the
``binding`` invocation for *Visitor* takes three arguments:

.. parsed-literal::

  typedef typename parameter::binding<
    ArgumentPack,\ **tag::visitor,boost::dfs_visitor<>**
  >::type Visitor;

  Visitor v = args[visitor|\ **boost::dfs_visitor<>()**\ ];

Note that the default ``visitor`` is supplied as a *temporary*
instance of ``dfs_visitor``.  Because ``args[…]`` always yields
a reference, making ``v`` a reference would cause it to bind to
that temporary, and immediately dangle.  Therefore, it's crucial
that we passed ``dfs_visitor<>``, and not ``dfs_visitor<>
const&``, as the last argument to ``binding``.

.. Important:: 

   Never pass ``binding`` a reference type as the default unless
   you know that the default value passed to the |ArgumentPack|\ 's
   indexing operator will outlive the reference you'll bind to it.

Sometimes there's no need to use ``binding`` at all.  The
``root_vertex`` argument is required to be of the graph's
``vertex_descriptor`` type, [#vertex_descriptor]_ so we can just
use that knowledge to bypass ``binding`` altogether.

.. parsed-literal::

  typename **boost::graph_traits<Graph>::vertex_descriptor**
    s = args[root_vertex|\ ***vertices(g).first**\ ];

.. _dangling:

.. |Metafunction| replace:: :concept:`Metafunction`

.. _Metafunction: ../../../mpl/doc/refmanual/metafunction.html

Beyond Ordinary Default Arguments
---------------------------------

Here's how you might write the declaration for the ``index_map``
parameter:

.. parsed-literal::

  typedef typename parameter::binding<
      ArgumentPack
    , tag::index_map
    , **typename boost::property_map<Graph, vertex_index_t>::const_type**
  >::type Index;

  Index i = args[index_map|\ **get(boost::vertex_index,g)**\ ];

Notice two capabilities we've gained over what
plain C++ default arguments provide:

1. The default value of the ``index`` parameter depends on the
   value of the ``graph`` parameter.  That's illegal in plain C++:
   
   .. parsed-literal::

     void f(int **graph**, int index = **graph** + 1); // error

2. The ``index`` parameter has a useful default, yet it is
   templated and its type can be deduced when  an ``index``
   argument is explicitly specified by the caller.  In plain C++, you
   can *specify* a default value for a parameter with deduced type,
   but it's not very useful:

   .. parsed-literal::

     template <class Index>
     int f(Index index **= 42**);  // OK
     int y = f();                // **error; can't deduce Index**

Syntactic Refinement
====================

In this section we'll describe how you can allow callers to invoke
``depth_first_search`` with just one pair of parentheses, and to
omit keywords where appropriate.


Describing the Positional Argument Order
----------------------------------------

.. _ParameterSpec:

.. |ParameterSpec| replace:: :concept:`ParameterSpec`

First, we'll need to build a type that describes the allowed
parameters and their ordering when passed positionally.  This type
is known as a |ParameterSpec| (MSVC6.x users see this note__)::

  namespace graphs
  {
    typedef parameter::parameters<
        tag::graph
      , tag::visitor
      , tag::root_vertex
      , tag::index_map
      , tag::color_map
    > dfs_params;
  }

__ `Can't Declare ParameterSpec Via typedef`_

The ``parameters`` template supplies a function-call
operator that groups all its arguments into an |ArgumentPack|.  Any
arguments passed to it without a keyword label will be associated
with a parameter according to its position in the |ParameterSpec|.
So for example, given an object ``p`` of type ``dfs_params``, ::

  p('G', index_map=1)

yields an |ArgumentPack| whose ``graph`` parameter has a value of
``'G'``, and whose ``index_map`` parameter has a value of ``1``.

Forwarding Functions
--------------------
  
Next we need a family of overloaded ``depth_first_search`` function
templates that can be called with anywhere from one to five
arguments.  These *forwarding functions* will invoke an instance of
``dfs_params`` as a function object, passing their parameters
to its ``operator()`` and forwarding the result on to
``core::depth_first_search``:

.. parsed-literal::

  namespace graphs
  {
    template <class A0>
    void depth_first_search(A0 const& a0)
    {
       core::depth_first_search(dfs_params()(a0));
    }

    template <class A0, class A1>
    void depth_first_search(A0 const& a0, A1 const& a1)
    {
       core::depth_first_search(dfs_params()(a0,a1));
    } :vellipsis:`\ 
    .
    .
    .
   `
    template <class A0, class A1, …class A4>
    void depth_first_search(A0 const& a0, A1 const& a1, …A4 const& a4)
    {
       core::depth_first_search(dfs_params()(a0,a1,a2,a3,a4));
    }
  }

That's it!  We can now call ``graphs::depth_first_search`` with
from one to five arguments passed positionally or via keyword.

“Out” Parameters
----------------

Well, that's not *quite* it.  When passing arguments by keyword,
the keyword object's assignment operator yields a temporary
|ArgumentPack| object.  A conforming C++ compiler will refuse to
bind a non-``const`` reference to a temporary, so to support a
keyword interface for all arguments, the overload set above *must*
take its arguments by ``const`` reference.  On the other hand—as
you may recall from the `parameter table`_\ —\ ``color_map`` is an
“out” parameter, so it really should be passed by *non-*\ ``const``
reference.  

A keyword object has a pair of ``operator=`` overloads that ensure
we can pass anything—temporary or not, ``const`` or not—by name,
while preserving the mutability of non-temporaries:

.. parsed-literal::

  template <class A>                  // handles non-const, 
  |ArgumentPack| operator=(A&);       // non-temporary objects

  template <class A>                  // handles const objects
  |ArgumentPack| operator=(A const&); // and temporaries

However, when an “out” parameter is passed positionally, there's no
keyword object involved.  With our ``depth_first_search`` overload
set above, the ``color_map`` will be passed by ``const`` reference,
and compilation will fail when mutating operations are used on it.
The simple solution is to add another overload that takes a
non-``const`` reference in the position of the “out” parameter:

.. parsed-literal::

   template <class A0, class A1, …class A4>
   void depth_first_search(A0 **const&** a0, A1 **const&** a1, …\ A4\ **&** a4)
   {
       core::depth_first_search(dfs_params()(a0,a1,a2,a3,a4));
   }

That approach works nicely because there is only one “out”
parameter and it is in the last position.  If ``color_map`` had
been the first parameter, we would have needed *ten* overloads.  In
the worst case—where the function has five “out” parameters—2\
:sup:`5` or 32 overloads would be required.  This “\ `forwarding
problem`_\ ” is well-known to generic library authors, and the C++
standard committee is working on a proposal__ to address it.  In
the meantime, you might consider using `Boost.Preprocessor`_ to
generate the overloads you need.

.. _`forwarding problem`: http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2002/n1385.htm

__ http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2004/n1690.html

.. _`Boost.Preprocessor`: ../../../preprocessor

If it is impractical for you to generate or write the overloads
that would be required for positional “out” arguments to be passed
directly, you still have the option to ask users to pass them
through |ref|_, which will ensure that the algorithm implementation
sees a non-``const`` reference:

.. parsed-literal::

  depth_first_search(g, v, s, i, **boost::ref(c)**);

.. |ref| replace:: ``boost::ref``

.. _ref: http://www.boost.org/doc/html/reference_wrapper.html

Generating Forwarding Functions with Macros
-------------------------------------------

To remove some of the tedium of writing overloaded forwarding
functions, the library supplies a macro, suitably located in
``boost/parameter/macros.hpp``, that will generate free function
overloads for you::

  BOOST_PARAMETER_FUN(void, depth_first_search, 1, 5, dfs_params);

will generate a family of five ``depth_first_search`` overloads, in
the current scope, that pass their arguments through
``dfs_params``.  Instead of ``core::depth_first_search``, these
overloads will forward the |ArgumentPack| on to a function called
``depth_first_search_with_named_params``, also in the current
scope.  It's up to you to implement that function.  You could
simply transplant the body of ``core::depth_first_search`` into
``depth_first_search_with_named_params`` if you were going to use
this approach.

Note that ``BOOST_PARAMETER_FUN`` only takes arguments by ``const``
reference, so you will have to add any additional overloads
required to handle positional “out” parameters yourself.  We are
looking into providing a more sophisticated set of macros to
address this problem and others, for an upcoming release of Boost.

Controlling Overload Resolution
===============================

The parameters of our templated forwarding functions are completely
general; in fact, they're a perfect match for any argument type
whatsoever.  The problems with exposing such general function
templates have been the subject of much discussion, especially in
the presence of `unqualified calls`__.  Probably the safest thing
to do is to isolate the forwarding functions in a namespace
containing no types [#using]_, but often we'd *like* our functions
to play nicely with argument-dependent lookup and other function
overloads.  In that case, it's neccessary to remove the functions
from the overload set when the passed argument types aren't
appropriate.

__ http://anubis.dkuug.dk/jtc1/sc22/wg21/docs/lwg-defects.html#225

Updating the |ParameterSpec|
----------------------------

This sort of overload control can be accomplished in C++ by taking
advantage of the SFINAE (Substitution Failure Is Not An Error)
rule. [#sfinae]_ You can take advantage of the Parameter library's
built-in SFINAE support by using the following class templates in
your |ParameterSpec|:

.. parsed-literal::

     template< class KeywordTag, class Predicate = *unspecified* >
     struct required;

     template< class KeywordTag, class Predicate = *unspecified* >
     struct optional;

Instead of using keyword tags directly, we can wrap them in
``required`` and ``optional`` to indicate which function parameters
are required, and optionally pass ``Predicate``\ s to describe the
type requirements for each function parameter.  The ``Predicate``
argument must be a unary `MPL lambda expression`_ that, when
applied to the actual type of the argument, indicates whether that
argument type meets the function's requirements for that parameter
position.

.. _`MPL lambda expression`: ../../../mpl/doc/refmanual/lambda-expression.html

For example, let's say we want to restrict ``depth_first_search()`` so that
the ``graph`` parameter is required and the ``root_vertex``
parameter is convertible to ``int``.  We might write:

.. parsed-literal::

  #include <boost/type_traits/is_convertible.hpp>
  #include <boost/mpl/placeholders.hpp>
  namespace graphs
  {
    using namespace boost::mpl::placeholders;

    struct dfs_params
      : parameter::parameters<
            **parameter::required<tag::graph>**
          , parameter::optional<tag::visitor>
          , **parameter::optional<
                tag::root_vertex, boost::is_convertible<_,int>
            >**
          , parameter::optional<tag::index_map>
          , parameter::optional<tag::color_map>
        >
    {};
  }

Applying SFINAE to the Overload Set
-----------------------------------

Now we add a special defaulted argument to each of our
``depth_first_search`` overloads:

.. parsed-literal::

  namespace graphs
  {
    template <class A0>
    void depth_first_search(
        A0 const& a0
      , typename dfs_params::match<A0>::type p = dfs_params())
    {
       core::depth_first_search(**p**\ (a0));
    }

    template <class A0, class A1>
    void depth_first_search(
        A0 const& a0, A1 const& a1
      , typename dfs_params::match<A0,A1>::type p = dfs_params())
    {
       core::depth_first_search(**p**\ (a0,a1));
    } :vellipsis:`\ 
    .
    .
    .
   `
    template <class A0, class A1, …class A4>
    void depth_first_search(
        A0 const& a0, A1 const& a1, …A4 const& A4
      , typename dfs_params::match<A0,A1,A2,A3,A4>::type p = dfs_params())
    {
       core::depth_first_search(**p**\ (a0,a1,a2,a3,a4));
    }
  }


These additional parameters are not intended to be used directly
by callers; they merely trigger SFINAE by becoming illegal types
when the ``name`` argument is not convertible to ``const
char*``. The ``BOOST_PARAMETER_FUN`` macro described earlier
adds these extra function parameters for you (Borland users see
this note__).

.. _BOOST_PARAMETER_MATCH:

__ `Default Arguments Unsupported on Nested Templates`_

Reducing Boilerplate With Macros
--------------------------------

The library provides a macro you can use to eliminate some of the
repetetiveness of the declaring the optional parameters.
``BOOST_PARAMETER_MATCH`` takes three arguments: the
|ParameterSpec|, a `Boost.Preprocessor sequence`__ of the function
argument types, and a name for the defaulted function parameter
(``p``, above), and it generates the appropriate defaulted
argument.  So we could shorten the overload set definition as
follows:

__ http://boost-consulting.com/mplbook/preprocessor.html#sequences

.. parsed-literal::

  namespace graphs
  {
    template <class A0>
    void depth_first_search(
        A0 const& a0
      , **BOOST_PARAMETER_MATCH(dfs_params, (A0), p)**)
    {
       core::depth_first_search(p(a0));
    }

    template <class A0, class A1>
    void depth_first_search(
        A0 const& a0, A1 const& a1
      , **BOOST_PARAMETER_MATCH(dfs_params, (A0)(A1), p)**)
    {
       core::depth_first_search(p(a0,a1));
    } :vellipsis:`\ 
    .
    .
    .
   `
    template <class A0, class A1, …class A4>
    void depth_first_search(
        A0 const& a0, A1 const& a1, …A4 const& A4
      , **BOOST_PARAMETER_MATCH(dfs_params, (A0)(A1)…(A4), p)**)
    {
       core::depth_first_search(p(a0,a1,a2,a3,a4));
    }
  }

Efficiency Issues
=================

The ``color_map`` parameter gives us a few efficiency issues to
consider.  Here's a first cut at extraction and binding:

.. parsed-literal::

  typedef 
    vector_property_map<boost::default_color_type, Index>
  default_color_map;

  typename parameter::binding<
      ArgumentPack
    , tag::color_map
    , default_color_map
  >::type color = args[color_map|\ **default_color_map(num_vertices(g),i)**\ ];

Eliminating Copies
------------------

The library has no way to know whether an explicitly-supplied
argument is expensive to copy (or even if it is copyable at all),
so ``binding<…,k,…>::type`` is always a reference type when the
*k* parameter is supplied by the caller.  Since ``args[…]``
yields a reference to the actual argument, ``color`` will be bound
to the actual ``color_map`` argument and no copying will be done.

As described above__, because the default is a temporary, it's
important that ``color`` be a non-reference when the default is
used.  In that case, the default value will be *copied* into
``color``.  If we store the default in a named variable, though,
``color`` can be a reference, thereby eliminating the copy:

.. parsed-literal::

  default_color_map default_color(num_vertices(g),i);

  typename parameter::binding<
      ArgumentPack
    , tag::color_map
    , **default_color_map&**
  >::type color = args[color_map|default_color];

__ dangling_

.. Hint:: 

   To avoid making needless copies, pass a *reference to the
   default type* as the third argument to ``binding``.

Lazy Default Computation
------------------------

Of course it's nice to avoid copying ``default_color``, but the
more important cost is that of *constructing* it in the first
place.  A ``vector_property_map`` is cheap to copy, since it holds
its elements via a |shared_ptr|_.  On the other hand, construction of
``default_color`` costs at least two dynamic memory allocations and
``num_vertices(g)`` copies; it would be better to avoid doing this
work when the default value won't be needed.

.. |shared_ptr| replace:: ``shared_ptr``

.. _shared_ptr: ../../../smart_ptr/shared_ptr.htm

To that end, the library allows us to supply a callable object
that—if no argument was supplied by the caller—will be invoked to
construct the default value.  Instead of following the keyword with
the ``|`` operator, we'll use ``||`` and follow it with a
nullary (zero-argument) function object that constructs a
default_color_map.  Here, we build the function object using
Boost.Lambda_: [#bind]_

.. _Boost.Lambda: ../../../lambda/index.html

.. parsed-literal::

  // After #include <boost/lambda/construct.hpp>
  typename parameter::binding<
      ArgumentPack
    , tag::color_map
    , default_color_map
  >::type color = args[
    color_map
    **|| boost::lambda::construct<default_color_map>(num_vertices(g),i)**
  ];

.. sidebar:: Mnemnonics

   To remember the difference between ``|`` and ``||``, recall that
   ``||`` normally uses short-circuit evaluation: its second
   argument is only evaluated if its first argument is ``false``.
   Similarly, in ``color_map[param||f]``, ``f`` is only invoked if
   no ``color_map`` argument was supplied.

Default Forwarding
------------------

Types that are expensive to construct yet cheap to copy aren't all
that typical, and even copying the color map is more expensive than
we might like.  It might be nice to avoid both needless
construction *and* needless copying of the default color map.  The
simplest way to achieve that is to avoid naming it altogether, at
least not in ``core::depth_first_search``.  Instead, we'll
introduce another function template to implement the actual
algorithm:

.. parsed-literal::

  namespace graphs { namespace core
  {
    template <class G, class V, class S, class I, class C>
    void **dfs_impl**\ (G& g, V& v, S& s, I& i, C& c)
    {
        *…actual algorithm implementation…*
    }
  }}

Then, in ``core::depth_first_search``, we'll simply forward the
result of indexing ``args`` to ``core::dfs_impl``::

  core::dfs_impl( 
      g,v,s,i
    , args[
        color_map
        || boost::lambda::construct<default_color_map>(num_vertices(g),i)
      ]);

In real code, after going to the trouble to write ``dfs_impl``,
we'd probably just forward all the arguments.

Dispatching Based on the Presence of a Default
----------------------------------------------

In fact, the Graph library itself constructs a slightly different
``color_map``, to avoid even the overhead of initializing a
|shared_ptr|_::

   std::vector<boost::default_color_type> 
     color_vec(num_vertices(g));

   boost::iterator_property_map<
       typename std::vector<
          boost::default_color_type
       >::iterator
     , Index
   > c(color_vec.begin(), i);

To avoid instantiating that code when it isn't needed, we'll have
to find a way to select different function implementations, at
compile time, based on whether a ``color_map`` argument was
supplied.  By using `tag dispatching`_ on the presence of a
``color_map`` argument, we can do just that:

.. _`tag dispatching`: ../../../../more/generic_programming.html#tag_dispatching

.. parsed-literal::

  #include <boost/type_traits/is_same.hpp>
  #include <boost/mpl/bool.hpp>

  namespace graphs { namespace core {
  
    template <class ArgumentPack>
    void dfs_dispatch(ArgumentPack& args, **mpl::true_**)
    {
        *…use the color map computed in the previous example…*
    }
    
    template <class ArgumentPack>
    void dfs_dispatch(ArgumentPack& args, **mpl::false_**)
    {
        *…use args[color]…*
    }
    
    template <class ArgumentPack>
    void depth_first_search(ArgumentPack& args)
    {
        typedef typename binding<args,tag::color>::type color\_;
        core::dfs_dispatch(args, **boost::is_same<color\_,void>()**\ );
    }
  }}

We've used the fact that the default for ``binding``\ 's third
argument is ``void``: because specializations of ``is_same`` are
``bool``-valued MPL |Integral Constant|_\ s derived either
from ``mpl::true_`` or ``mpl::false_``, the appropriate
``dfs_dispatch`` implementation will be selected.

.. |Integral Constant| replace:: :concept:`Integral Constant`

.. _`Integral Constant`: ../../../mpl/doc/refmanual/integral-constant.html

============================
 Portability Considerations
============================

Use the `regression test results`_ for the latest Boost release of
the Parameter library to see how it fares on your favorite
compiler.  Additionally, you may need to be aware of the following
issues and workarounds for particular compilers.

.. _`regression test results`: http://www.boost.org/regression/release/user/parameter.html

No SFINAE Support
=================

Some older compilers don't support SFINAE.  If your compiler meets
that criterion, then Boost headers will ``#define`` the preprocessor
symbol ``BOOST_NO_SFINAE``, and uses of ``parameters<…>::match`` and
|BOOST_PARAMETER_MATCH| will be harmless, but will have no effect.

No Support for |result_of|_
===========================

.. |result_of| replace:: ``result_of``

.. _result_of: ../../../utility/utility.htm#result_of

`Lazy default computation`_ relies on the |result_of| class
template to compute the types of default arguments given the type
of the function object that constructs them.  On compilers that
don't support |result_of|, ``BOOST_NO_RESULT_OF`` will be
``#define``\ d, and the compiler will expect the function object to
contain a nested type name, ``result_type``, that indicates its
return type when invoked without arguments.  To use an ordinary
function as a default generator on those compilers, you'll need to
wrap it in a class that provides ``result_type`` as a ``typedef``
and invokes the function via its ``operator()``.

Can't Declare |ParameterSpec| via ``typedef``
=============================================

In principle you can declare a |ParameterSpec| as a ``typedef``
for a specialization of ``parameters<…>``, but Microsoft Visual C++
6.x has been seen to choke on that usage.  The workaround is to use
inheritance and declare your |ParameterSpec| as a class:

.. parsed-literal::

     **struct dfs_parameters
       :** parameter::parameters<
           tag::graph, tag::visitor, tag::root_vertex
         , tag::index_map, tag::color_map
     > **{};**

Default Arguments Unsupported on Nested Templates
=================================================

As of this writing, Borland compilers don't support the use of
default template arguments on member class templates.  As a result,
you have to supply ``BOOST_PARAMETER_MAX_ARITY`` arguments to every
use of ``parameters<…>::match``.  Since the actual defaults used
are unspecified, the workaround is to use
|BOOST_PARAMETER_MATCH|_ to declare default arguments for SFINAE.

.. |BOOST_PARAMETER_MATCH| replace:: ``BOOST_PARAMETER_MATCH``

Compiler Can't See References In Unnamed Namespace
==================================================

If you use Microsoft Visual C++ 6.x, you may find that the compiler
has trouble finding your keyword objects.  This problem has been
observed, but only on this one compiler, and it disappeared as the
test code evolved, so we suggest you use it only as a last resort
rather than as a preventative measure.  The solution is to add
*using-declarations* to force the names to be available in the
enclosing namespace without qualification::

    namespace graphs
    {
      using graphs::graph;
      using graphs::visitor;
      using graphs::root_vertex;
      using graphs::index_map;
      using graphs::color_map;
    }

===========
 Reference
===========

.. _reference: reference.html

Follow `this link`__ to the Boost.Parameter reference
documentation.  

__ reference.html

==================
 Acknowledgements
==================

The authors would like to thank all the Boosters who participated
in the review of this library and its documentation, most
especially our review manager, Doug Gregor.

--------------------------

.. [#old_interface] As of Boost 1.33.0 the Graph library was still
   using an `older named parameter mechanism`__, but there are
   plans to change it to use Boost.Parameter (this library) in an
   upcoming release, while keeping the old interface available for
   backward-compatibility.  

__ ../../../graph/doc/bgl_named_params.html

.. [#odr] The **One Definition Rule** says that any given entity in
   a C++ program must have the same definition in all translation
   units (object files) that make up a program.

.. [#vertex_descriptor] If you're not familiar with the Boost Graph
   Library, don't worry about the meaning of any
   Graph-library-specific details you encounter.  In this case you
   could replace all mentions of vertex descriptor types with
   ``int`` in the text, and your understanding of the Parameter
   library wouldn't suffer.

.. [#bind] The Lambda library is known not to work on `some
   less-conformant compilers`__.  When using one of those you could
   define ::
   
      template <class T>
      struct construct2
      {
          typedef T result_type;

          template <class A1, class A2>
          T operator()(A1 a1, A2 a2) { return T(a1,a2); }
      };

    and use `Boost.Bind`_ to generate the function object::

      boost::bind(construct2<default_color_map>(),num_vertices(g),i)

__ http://www.boost.org/regression/release/user/lambda.html
.. _Boost.Bind: ../../../libs/bind/index.html


.. [#using] You can always give the illusion that the function
   lives in an outer namespace by applying a *using-declaration*::

      namespace foo_overloads
      {
        // foo declarations here
        void foo() { ... }
        ...
      }
      using foo_overloads::foo;  


.. [#sfinae] If type substitution during the instantiation of a
   function template results in an invalid type, no compilation
   error is emitted; instead the overload is removed from the
   overload set. By producing an invalid type in the function
   signature depending on the result of some condition, whether or
   not an overload is considered during overload resolution can be
   controlled.  The technique is formalized in the |enable_if|_
   utility.  See
   http://www.semantics.org/once_weakly/w02_SFINAE.pdf for more
   information on SFINAE.

.. |enable_if| replace:: ``enable_if``
.. _enable_if: ../../../utility/enable_if.html


