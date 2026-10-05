module datapath_test();

	logic clk;
	logic [15:0] control;
	logic [3:0] datain,dataout;
	logic [3:0] flags;
	logic [19:0] testvector [3:0]; 
	integer i;
	
	datapath dat(clk,control,datain,flags,dataout);
	
	initial begin
		$dumpfile("ondas.vcd");
    $dumpvars(0, datapath_test);
		$readmemb("../tb/programa.txt", testvector);
		//$readmemb("D:/td2/soft_altera/unidad_proc/programa.txt",testvector);
		for(i=0;i<4;i=i+1) begin
			{control,datain} = testvector[i];
			clk = 0;
			#10;
			clk = 1;
			#10;
		end
	end
		
	
/*	initial begin
		control = 16'b000_0000_001_000_000;
		datain  = 4'b1010;
		clk = 0;
		#10;
		clk = 1;
		#10
		control = 16'b000_0000_010_000_000;
		datain  = 4'b1100;
		clk = 0;
		#10;
		clk = 1;
		#10
		control = 16'b000_0000_011_000_000;
		datain  = 4'b1000;
		clk = 0;
		#10;
		clk = 1;
		#10
		control = 16'b000_0010_100_010_001;
		datain  = 4'b0000;
		clk = 0;
		#10;
		clk = 1;
	end*/
endmodule
		
