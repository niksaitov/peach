.PHONY: all engine website run clean

all: website

# Engine — real file target, no stamp needed
engine/engine:
	$(MAKE) -C engine

# Venv creation stamp
venv/.stamp:
	python3 -m venv venv
	touch $@

# Deps install stamp — reruns if requirements.txt or the venv itself changes
venv/.deps-stamp: requirements.txt venv/.stamp
	venv/bin/pip install -r requirements.txt -q
	touch $@

# Website is ready when the engine is built and deps are installed
website: engine/engine venv/.deps-stamp

run: website
	venv/bin/python main.py

clean:
	$(MAKE) -C engine clean
	rm -rf venv instance/
