# Makefile to create a pdf file from latex source.
#
# A pdf file is generated useing pdflatex.
# Be carefull to use only supported image types.
# 'Makefile' have to be in the same folder as latex source.
# pdf is moved to subdirectory 'pdf' in folder 'doc'.
# To remove latex files use 'make clean'.
#
# Usage:
#	make <filename>.pdf 
#	make clean

all:
	echo "Specify file <filename>.pdf"

%.pdf: %.tex
	pdflatex $*.tex
	pdflatex $*.tex		# add index
	mv $*.pdf ../pdf/$*.pdf

clean:
	rm `find * \( \! -name Makefile \)`

.SILENT: all

#end