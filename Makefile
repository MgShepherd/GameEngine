OUTDIR=build

main:
	cmake -B $(OUTDIR)/ -DCMAKE_EXPORT_COMPILE_COMMANDS=1 && cmake --build $(OUTDIR)

clean:
	rm -rf $(OUTDIR)/
