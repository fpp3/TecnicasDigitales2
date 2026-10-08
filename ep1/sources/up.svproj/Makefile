IVL      = iverilog
VVP      = vvp
VIEWER   = gtkwave
YOSYS      = yosys
TOP_MODULE = up_harvard
IVLFLAGS = -g2012 -Wall
IVLFLAGS += -Isrc -Itb
NPM_GLOBAL_ROOT := $(shell npm root -g 2>/dev/null)
Y2D_SCRIPT       := $(NPM_GLOBAL_ROOT)/yosys2digitaljs/process.js
DJS_DIST         := $(NPM_GLOBAL_ROOT)/digitaljs/dist
HTML_OUT         := $(RTL_OUT)/$(MODULE)_interactive.html

BUILD_DIR = build
TARGET    = up_harvard
HTML_OUT      := $(BUILD_DIR)/$(TOP_MODULE)_interactive.html
INTERACTIVE_V := $(BUILD_DIR)/$(TOP_MODULE)_interactive.v

SRCS = $(wildcard src/*.sv)
TB   = tb/up_harvard_tb.sv

all: $(BUILD_DIR)/$(TARGET).vvp

$(BUILD_DIR)/$(TARGET).vvp: $(SRCS) $(TB) | $(BUILD_DIR)
	@echo "Compilando módulos y testbench..."
	$(IVL) $(IVLFLAGS) -o $@ $^

sim: $(BUILD_DIR)/$(TARGET).vvp
	@echo "Ejecutando simulación en el directorio build..."
	cd $(BUILD_DIR) && $(VVP) $(TARGET).vvp

view: sim
	@echo "Abriendo GTKWave en segundo plano..."
	@gtkwave $(BUILD_DIR)/ondas.vcd > /dev/null 2>&1 &

rtl: $(SRCS) | $(BUILD_DIR)
	@echo "Sintetizando modelo RTL..."
	@$(YOSYS) -p "read_verilog -sv $(SRCS); hierarchy -top $(TOP_MODULE); proc; show -colors 2 -width -format dot -prefix $(BUILD_DIR)/rtl_schematic $(TOP_MODULE)" > /dev/null 2>&1
	@sed -i -E 's/\\n\$$paramod(&#9586;|\$$[a-f0-9]+&#9586;)([a-zA-Z0-9_]+)(&#9586;[^|]*)?/\\n\2/g' $(BUILD_DIR)/rtl_schematic.dot
	@echo "Abriendo visor interactivo xdot..."
	@xdot $(BUILD_DIR)/rtl_schematic.dot > /dev/null 2>&1 &

irtl: $(BUILD_DIR)
	@$(YOSYS) -p "read_verilog -sv $(SRCS); hierarchy -check -top $(TOP_MODULE); proc; write_verilog $(INTERACTIVE_V)"
	node "$(Y2D_SCRIPT)" --tmpdir --html "$(INTERACTIVE_V)" > "$(HTML_OUT)"
	cp "$(DJS_DIST)"/*.js $(BUILD_DIR)/
	@echo "Esquematico interactivo generado en: $(HTML_OUT)"
	xdg-open "file://$(abspath $(HTML_OUT))" >/dev/null 2>&1 & \

$(BUILD_DIR):
	mkdir -p $@

clean:
	rm -rf $(BUILD_DIR)

.PHONY: all sim view rtl clean
