
file = instructions

$(file).pdf : $(file).ps
	ps2pdf $(file).ps

$(file).dvi : $(file).tex
	latex $(file).tex

$(file).ps : $(file).dvi
	dvips $(file).dvi -o $(file).ps -Pwww


