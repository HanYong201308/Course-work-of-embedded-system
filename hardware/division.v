`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date:    10:55:28 02/28/2008 
// Design Name: 
// Module Name:    division 
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
module division(clk,rst,count,clk_odd);     
input        clk,rst;
output       clk_odd;
output[19:0] count;
reg          clk_odd;
reg[19:0]     count;
parameter    N = 10000;                     
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

