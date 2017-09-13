# Minimal makefile for Sphinx documentation
#

# You can set these variables from the command line.
SPHINXOPTS    =
SPHINXBUILD   = python -msphinx
SPHINXPROJ    = mesos
SOURCEDIR     = .
BUILDDIR      = _build

# Put it first so that "make" without argument is like "make help".
help:
	@$(SPHINXBUILD) -M help "$(SOURCEDIR)" "$(BUILDDIR)" $(SPHINXOPTS) $(O)

.PHONY: help Makefile

prep:
	find . -type f -name "*.md" -exec sed -i.bak -E 's/~~~\{\.([a-z]+)\}/~~~\1/' {} \;
	find . -type f -name "*.md" -exec sed -i.bak -E 's/```\{\.([a-z]+)\}/```\1/' {} \;
	find . -type f -name "*.md" -exec sed -i.bak -E 's/~~~protoc/~~~proto/' {} \;
	find . -type f -name "*.md" -exec sed -i.bak -E 's/\.md/\.html/' {} \;

# Catch-all target: route all unknown targets to Sphinx using the new
# "make mode" option.  $(O) is meant as a shortcut for $(SPHINXOPTS).
%: Makefile
	@$(SPHINXBUILD) -M $@ "$(SOURCEDIR)" "$(BUILDDIR)" $(SPHINXOPTS) $(O)
