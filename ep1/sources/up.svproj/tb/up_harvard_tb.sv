`timescale 1ns/1ps

// (21) D Address (MSB)       ┐M    ┐ 3*M
// (20) D Address (LSB)       ┘     │
// (19) B Address (MSB)       ┐M    │
// (18) B Address (LSB)       ┘     │
// (17) A Address (MSB)       ┐M    │
// (16) A Address (LSB)       ┘     ┘
// (15) Load Enable                 ┐ 3
// (14) Constant In Enable (MUX B)  │
// (13) Data In Enable (MUX D)      ┘
// (12) ALU Enable
// (11) ALU Control Bit 3
// (10) ALU/Shifter Control Bit 2
// (9)  ALU/Shifter Control Bit 1
// (8)  ALU/Shifter Control Bit 0

module up_harvard_tb();
	logic clk;
	logic [13:0] control;
	logic [3:0] constin;
	logic [3:0] datain;
	logic [3:0] dataout, addressout;
	logic [3:0] flags;
	logic [21:0] testvector [7:0]; 
	integer i;

	up_harvard dut (
		.clk(clk),
		.dIN(datain),
		.cIN(constin),
		.regCTRL(control),
		.dOUT(dataout),
		.aOUT(addressout),
		.sOUT(flags)
	);
	
	initial begin
		$dumpfile("ondas.vcd");
		$dumpvars(0, up_harvard_tb);
		$readmemb("../tb/up_harvard_mem.txt", testvector);
		for(i=0; i<8; i=i+1) begin
			{control, constin, datain} = testvector[i];
			clk = 0;
			#10;
			clk = 1;
			#10;
		end
	end
endmodule
