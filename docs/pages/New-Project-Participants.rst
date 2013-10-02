New Project Participants
========================

.. raw:: mediawiki

   {{Historical}}

This page is obsolete; see `here <Git>`__ for information about using
git.

Subversion
----------

The code for XINU is stored in a
`Subversion <wikipedia:Subversion_(software)>`__ code repository.

You have two ways of accessing the code:

-  over HTTPS
-  over SSH

For detailed usage information, head over to our `Subversion
Primer <Subversion Primer>`__.

Over HTTPS
----------

To access the repository over HTTPS you'll need to contact the MULUG
system administrator and he/she will generate a password for you (it can
be changed later, do not fear). Once you have an account, you can access
the repositories at:

https://xinu.mscs.mu.edu/svn/mips/trunk

Or some sort of URL.

Over SSH
--------

If you want to access the code via SSH, you will need to create and
register your public key with the 'svn' user on MULUG. There is good
document that details generating an SSH keypair at:

http://kb.iu.edu/data/aews.html

After the key is generated please email (or link) your public SSH key to
the current repository administrator to be added.

Once your key has been registered, you can do a test checkout with the
following command:

svn co svn+ssh://svn@xinu.mscs.mu.edu/mips/trunk xinu

You can also switch an existing working copy with the following from
your working copy's directory:

::

    FROM=`svn info | grep ^URL | awk '{ print $2 }'`
    svn switch --relocate $FROM svn+ssh://svn@xinu.mscs.mu.edu/mips/trunk
    unset FROM

You will be asked for the passphrase that you specified for your private
key every time you perform a svn command that needs the server. This is
because svn is not locally caching your password in plaintext as it was
before (no plaintext cache = better). I highly recommend using a
memory-caching tool like ssh-agent to prevent having to repeatedly enter
your passphrase.

Trac
----

Trac has several nice ways of visualizing activity in the Subversion
repository, including a source code browser and a timeline. It also has
a wiki, which we try to use for internal documentation, and at it's
center is an issue tracking system, which I suggest we consider as a way
to consolidate our various and sundry todo lists.

https://xinu.mscs.mu.edu/trac/

As for user accounts, if you have a password to access the repository
via HTTPS it is the same for logging into trac. If you do not have on,
please email the MULUG system administrator to set one up or generate
your own with the following command:

``htdigest -c ``\ \ `` "XINU Project Repositories" ``\ 

After your account has been setup you can simply visit the trac URI, log
in, and start editing!
