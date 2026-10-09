module up_harvard #(parameter M = 2, N = 4)
                   (input logic clk,
                    input logic [(N-1):0] dIN, cIN, // dIN: Data In, cIN: Constant In
                    input logic [(3*M+3+5-1):0] regCTRL,
                    output logic [(N-1):0] dOUT, aOUT, // dOUT: Data Out, aOUT: Address Out
                    output logic [3:0] sOUT); // sOUT: Status Out

   logic loadEnable, constInEnable, dataInEnable, aluEnable;
   logic [(M-1):0] aAddress, bAddress, dAddress;
   logic [(N-1):0] aBus, bBus, dBus, aluOut, shifterOut, functionUnitOut;
   logic [3:0] aluControl;
   logic [2:0] shifterControl;
   assign dAddress = regCTRL[(8+3*M-1):(8+2*M)];
   assign bAddress = regCTRL[(8+2*M-1):(8+M)];
   assign aAddress = regCTRL[(8+M-1):8];
   assign loadEnable = regCTRL[7];
   assign constInEnable = regCTRL[6];
   assign dataInEnable = regCTRL[5];
   assign aluEnable = regCTRL[4];
   assign aluControl = regCTRL[3:0];
   assign shifterControl = regCTRL[2:0];

   ram3port #(.N(N), .M(M)) REG_file (clk, loadEnable, aAddress, bAddress, dAddress, aBus, bBus, dBus);
   assign aOUT = aBus;
   assign dOUT = constInEnable ? cIN
                               : bBus;

   alu #(.N(N)) FU_alu (aOUT, dOUT, aluControl, aluOut, sOUT);
   shifter #(.N(N)) FU_shifter (dOUT, shifterControl, shifterOut);

   assign functionUnitOut = aluEnable ? aluOut
                            : shifterOut;
   assign dBus = dataInEnable ? dIN
                              : functionUnitOut;

endmodule
