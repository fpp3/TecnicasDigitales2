MATERIA := $(notdir $(abspath $(CURDIR)/../..))
TP := $(notdir $(abspath $(CURDIR)/..))
EP := $(notdir $(CURDIR))
OUTPUT_NAME := $(MATERIA)_$(TP)_$(EP)

SRC := main.tex

all:
	latexmk -pdf -jobname=$(OUTPUT_NAME) $(SRC)

clean:
	latexmk -CA -jobname=$(OUTPUT_NAME) $(SRC)
