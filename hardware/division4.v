`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date:    10:58:27 02/28/2008 
// Design Name: 
// Module Name:    division4 
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
module division4(clk,clk_even); 

//generate a clock of 9600*8 Hz;
input        clk;

output       clk_even;
reg rst=1;
reg[3:0]     count1,count2;
reg          clkA,clkB;
wire         clk_even,clk_re;

parameter    N = 13;          

assign clk_re   = ~clk;
assign clk_even = clkA | clkB;
    always @(posedge clk)
      if(! rst)  
        begin
          count1 <= 1'b0;
          clkA <= 1'b0;          
        end
      else
        if(count1 < (N - 1))
         begin
         count1 <= count1 + 1'b1;            
            if(count1 == (N - 1)/2)
              begin
                clkA <= ~clkA;
              end                
          end          
        else
          begin
            clkA <= ~clkA;
            count1 <= 1'b0;
          end          
          
always @ (posedge clk_re)
    if(! rst)
      begin
        count2 <= 1'b0;
        clkB <= 1'b0;
      end
    else
      if(count2 < (N - 1))
        begin
          count2 <= count2 + 1'b1;            
            if(count2 == (N - 1)/2)
              begin
                clkB <= ~clkB;
              end                
        end          
      else
        begin
          clkB <= ~clkB;
          count2 <= 1'b0;
        end          
endmodule

