compile:
	@echo "snuplc --exe `find . -type f -and -iname \*.mod -exec echo {} \+`"

clean:
	@rm -f *.mod.ast *.mod.ast.dot *.mod.ast.dot.pdf *.mod.tac *.mod.tac.dot *.mod.tac.dot.pdf *.mod.s
	@find . -type f -and -executable -exec rm {} \+
