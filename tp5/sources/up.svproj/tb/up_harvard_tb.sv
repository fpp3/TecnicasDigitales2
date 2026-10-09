`timescale 1ns/1ps

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
