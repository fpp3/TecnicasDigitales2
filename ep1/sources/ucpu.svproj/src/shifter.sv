module shifter #(parameter N = 4) 
              (input logic [N-1:0] D, 
               input logic [2:0] S,
               output logic [N-1:0] Y);

  always_comb
    case (S)
      3'b000: Y = D;                    // No hay corrimiento
      3'b001: Y = { D[N-2:0], 1'b0 };   // SHL
      3'b010: Y = { D[N-1], D[N-1:1] }; // SHR
      3'b011: Y = 4'b0;                 // Bus=0
      3'b100: Y = D;                    // No hay corrimiento
      3'b101: Y = { D[N-2:0], D[N-1] }; // ROL
      3'b110: Y = { D[0], D[N-1:1] };   // ROR
      3'b111: Y = D;                    // No hay corrimiento
    endcase
endmodule
