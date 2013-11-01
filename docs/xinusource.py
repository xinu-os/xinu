#
# This is a Sphinx extension for generating links to the Xinu source code.
#
# See: http://sphinx-doc.org/extensions.html.  Also based partly on
# Doc/tools/sphinxext/pyspecific.py in the Python 3.2.2 source.
#

from docutils import nodes,utils
from sphinx.util.nodes import split_explicit_title

# Route links to github web interface for master branch
SOURCE_URI = "https://github.com/xinu-os/xinu/tree/master/%s"

WIKIPEDIA_URI = "https://en.wikipedia.org/wiki/%s"

RFC_URI = "https://tools.ietf.org/html/rfc%s.html"

# See: http://docutils.sourceforge.net/docs/howto/rst-roles.html
def source_role(name, rawtext, text, lineno, inliner, options={}, content=[]):
    has_title, title, target = split_explicit_title(text)
    title = utils.unescape(title)
    target = utils.unescape(target)
    refnode = nodes.reference(title, title, refuri=SOURCE_URI % target)
    return [refnode], []

def wikipedia_role(name, rawtext, text, lineno, inliner, options={}, content=[]):
    has_title, title, target = split_explicit_title(text)
    title = utils.unescape(title)
    target = utils.unescape(target)
    target = target.replace(' ', '_')
    refnode = nodes.reference(title, title, refuri=WIKIPEDIA_URI % target)
    return [refnode], []

def rfc_role(name, rawtext, text, lineno, inliner, options={}, content=[]):
    has_title, title, target = split_explicit_title(text)

    title = utils.unescape(title)
    target = utils.unescape(target)

    try:
        rfcnum = int(target)
    except ValueError:
        msg = inliner.reporter.error('invalid RFC number %s' % text, line=lineno)
        prb = inliner.problematic(rawtext, rawtext, msg)
        return [prb], [msg]


    if not has_title:
        title = "RFC" + target

    refnode = nodes.reference(title, title, refuri=RFC_URI % target)
    return [refnode], []

# See: http://sphinx-doc.org/ext/appapi.html
def setup(app):
    app.add_role('source', source_role)
    app.add_role('wikipedia', wikipedia_role)
    app.add_role('rfc', rfc_role)
