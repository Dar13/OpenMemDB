LATEX=pdflatex
DOC=final_document.tex

BIBTEX=bibtex
BIB=final_document.bib

LATEX_FLAGS=-interaction nonstopmode

all:
	$(LATEX) $(LATEX_FLAGS) $(DOC)
	$(BIBTEX) $(BIB)
	$(LATEX) $(LATEX_FLAGS) $(DOC)
	$(LATEX) $(LATEX_FLAGS) $(DOC)
