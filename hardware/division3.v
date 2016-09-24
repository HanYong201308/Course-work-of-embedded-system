`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date:    10:57:43 02/28/2008 
// Design Name: 
// Module Name:    division3 
// Project Name: 
// Target Devices: 
// Tool versions: 
// Description: 
//
// Dependencies: 
//
// Revision: 
// Revision 0.01 - File Created
// Additional Comments: 
//
//////////////////////////////////////////////////////////////////////////////////
module division3(clk,clk_odd);     
//  1e6/104=9615

input        clk;
output clk_odd;
reg rst=1;
reg          clk_odd;
reg[19:0]     count;
parameter    N = 104;                     
    always @ (posedge clk)
      if(! rst)
        begin
          count <= 1'b0;
          clk_odd <= 1'b0;
        end
      else      
        if ( count < N/2-1)
          begin          
            count <= count + 1'b1;            
          end
        else
          begin        
            count <= 1'b0;
            clk_odd <= ~clk_odd;      
          end
endmodule

