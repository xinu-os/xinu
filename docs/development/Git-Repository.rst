Git repository
==============

Embedded Xinu previously used a Subversion repository, but in 2013
switched to `git <http://git-scm.com/>`__ as the preferred
:wikipedia:`source code management system <Revision control>`.

The official public repository is hosted on Github. To get the code,
install git and run::

    $ git clone https://github.com/xinu-os/xinu

This will generate the directory ``xinu/`` containing a local copy of
the repository. Note that this is a standalone repository that can be
used without Internet access to the Github repository. This constitutes
a difference from Subversion, which is more centralized.

Git is documented extensively in many locations, for example in the
`"Pro Git" book <http://git-scm.com/book>`__. But briefly, a very
basic workflow is to modify files, use either ``git add`` and ``git
commit`` or ``git commit -a`` to commit the changes to the repository,
then use ``git push`` to push changes to a remote repository.

Branches in git are fast and easy to use, so it is recommended to
develop experimental features in their own branches. Code pushed to
the "master" branch should not be broken.

At least if you plan to push your changes to the master branch, please
use descriptive commit messages for all commits and use the
:doc:`preferred coding style <Kernel-Normal-Form>`.
