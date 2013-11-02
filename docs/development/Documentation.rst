Documentation
=============

Welcome to the documentation documentation!  Embedded Xinu contains
two main forms of documentation:

1. "reStructuredText" (.rst) files located under the top-level
   ``docs/`` directory.
2. Comments in the source code itself, including Doxygen-compatible
   comments describing function parameters and behaviors.

This section focuses on (1); for information about (2) see
:doc:`Kernel-Normal-Form`.

.. contents::
   :local:

Building the documentation
--------------------------

The documentation for the latest development version of Embedded Xinu
can be read at http://embedded-xinu.readthedocs.org/en/latest/,
and it is automatically updated.

To build the documentation locally, you will need to install the
`Sphinx documentation tool <http://sphinx-doc.org>`__.   On Linux
systems you should find it in the repositories.

HTML
~~~~

Build the HTML documentation with:

.. code-block:: none

    $ cd docs
    $ make html

Point your browser at ``_build/html/index.html``.

PDF
~~~

LaTeX is required for generating the PDF documentation.  Build it
with:

.. code-block:: none

    $ cd docs
    $ make latexpdf

The generated PDF file will be ``_build/latex/EmbeddedXinu.pdf``.

Editing and writing documentation
---------------------------------

reStructuredText is designed to be relatively easy to edit.  Much
information can be found online, so the basics about headings, lists,
tables, external and internal links, footnotes, etc. will not be
repeated here.

Suggestions
~~~~~~~~~~~

A documentation page should be clear and reasonably understandable to
people who may not be familiar with the project and may not have read
many other pages.

The documentation should *supplement the source code*, not be
redundant with it.

Extensions
~~~~~~~~~~

The file ``xinusource.py`` defines several extensions, namely a
``:source:`` command to generate links to XINU source files (for
example, in the github web interface).  Use it like:

  ``:source:`PATH```

or

  ``:source:`DISPLAYTEXT <PATH>```

The command ``:rfc:`` generates a link to the specified Request for
Comments (RFC).  Use it like:

  ``:rfc:`RFCNUM```

or

  ``:rfc:`DISPLAYTEXT <RFCNUM>```

Similarly, the command ``:wikipedia:`` generates a link to the
specified Wikipedia article.  Use it like:

  ``:wikipedia:`ARTICLETITLE```

or

  ``:wikipedia:`DISPLAYTEXT <ARTICLETITLE>```

Organization
~~~~~~~~~~~~

Platform-dependent documentation (e.g. in ``arm/`` and ``mips/``) is
separated from the generic documentation (e.g. in ``features/``).

Although not all platforms support networking, networking above the
device driver level is more or less platform independent and should be
documented in ``features/networking/``.

Every file is supposed to be listed in a `TOC tree
<http://sphinx-doc.org/markup/toctree.html>`__.  However, the
``index.rst`` files containing these lists usually use globs, so no
action should be needed when adding a documentation file to an
existing directory.

Resources
---------

- `reStructuredText Primer <http://sphinx-doc.org/rest.html>`__
- `Quick reStructuredText <http://docutils.sourceforge.net/docs/user/rst/quickref.html>`__
