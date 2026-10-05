IVL      = iverilog
VVP      = vvp
VIEWER   = gtkwave
YOSYS      = yosys
TOP_MODULE = datapath
IVLFLAGS = -g2012 -Wall
IVLFLAGS += -Isrc -Itb

BUILD_DIR = build
TARGET    = ucpu_sim

SRCS = $(wildcard src/*.sv)
TB   = tb/datapath_test.sv

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
	@$(YOSYS) -p "read_verilog -sv $(SRCS); hierarchy -top $(TOP_MODULE); proc; opt; show -colors 2 -width -format dot -prefix $(BUILD_DIR)/rtl_schematic $(TOP_MODULE)" > /dev/null 2>&1
	@echo "Abriendo visor interactivo xdot..."
	@xdot $(BUILD_DIR)/rtl_schematic.dot > /dev/null 2>&1 &

$(BUILD_DIR):
	mkdir -p $@

clean:
	rm -rf $(BUILD_DIR)

.PHONY: all sim view rtl clean
