module alu #(parameter N = 4)
          (input logic [N-1:0] a, b,
           input logic [3:0] ALUControl,
           output logic [N-1:0] Result,
           output logic [3:0] ALUFlags);

  logic neg, zero, acarreo, overflow;
  logic [N-1:0] y, sum;
  logic cin, cout;
// ________________________________________________
//|_selección_|_  entrada _|_   F = A + y + cin   _|
//|_ S1  S0  _|_    y     _|_   cin=0     cin=1   _|
//|   0   0   | solo ceros |    F=A       F=A+1    |
//|   0   1   |     B      |    F=A+B     F=A+B+1  |
//|   1   0   |    /B      |    F=A+/B    F=A+/B+1 |
//|_  1   1  _|_solo unos _|_   F=A-1     F=A     _|  
 
  assign cin = ALUControl[0]; 
  assign y = ALUControl[2] ? (ALUControl[1] ? {N {1'b1}}
                                            : ~b )
                           : (ALUControl[1] ? b 
                                            : {N {1'b0}} );
  assign {cout,sum} = a + y + cin;
  
  always_comb
    casex (ALUControl[3:1])
      3'b0??: Result = sum;
      3'b100: Result = a & b;
      3'b101: Result = a | b;
      3'b110: Result = a ^ b;
      3'b111: Result = ~a;
    endcase

  assign neg = Result[N-1];
  assign zero = (Result == {N {1'b0}});
  assign acarreo = (ALUControl[3] == 1'b0) & cout;
  assign overflow = (ALUControl[3] == 1'b0) &
                    (~a[N-1] & ~y[N-1] & sum[N-1] +
                      a[N-1] &  y[N-1] & ~sum[N-1]);
  assign ALUFlags = {overflow, neg, zero, acarreo };
endmodule