.PHONY: compile-python-bindings

compile-python-bindings:
	@echo "Compiling Python bindings..."
	python setup.py build_ext --inplace