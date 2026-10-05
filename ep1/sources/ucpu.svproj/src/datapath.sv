module datapath #(parameter N = 4) 
                (input logic clk, 
                 input logic [15:0] ctrl_word,
                 input logic [N-1:0] DATA_in,
					  output logic [3:0] stateBits,
                 output logic [N-1:0] DATA_out);

   logic [N-1:0] busA, busB, RegA, RegB, ALU_out;
	logic [2:0] A, B, D, S, H; 
   logic [3:0] flags, F;
	//HHHFFFFDDDBBBAAA
   assign A = ctrl_word [2:0];
   assign B = ctrl_word [5:3];
   assign D = ctrl_word [8:6];
   assign F = ctrl_word [12:9];
   assign H = ctrl_word [15:13];
	
	ram3port REG_file (clk, (D != 0), A, B, D, RegA, RegB, DATA_out);
   
	assign busA = ((A!=0) ? RegA : DATA_in );
   assign busB = ((B!=0) ? RegB : DATA_in );
   
   
	alu ALU_unit (busA, busB, F, ALU_out, flags);
   
	shifter SHIFTER_unit (ALU_out, H, DATA_out); 

   always_ff @(posedge clk)
     stateBits <= flags;                    
   
endmodule
