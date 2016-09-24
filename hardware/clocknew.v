`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date:    23:30:48 05/12/2016 
// Design Name: 
// Module Name:    clock 
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
module clocknew(ab,db_in,db_out,segment,aen,iow_n,ior_n,
				iocs16_n,timer,oe_n,but1,but2,but3,clk,tx_data,rx,irq7,irq11);
	input [9:0] ab;
	input [7:0] db_in;
	output [7:0] db_out;
	output reg [7:0] segment;
	output [3:0] timer;
	reg [7:0] db_out;
	reg [3:0] timer;
	input aen;
	input iow_n;
	input ior_n;
	output oe_n;
	// oe_n: 1 for input and 0 for output 
	output iocs16_n;
	
	input but1,but2,but3;
	input clk;
	
	// port of serial
	output tx_data;
	input rx;
	
	
	//interrupt
	output irq7;
	output irq11;
	
	//output rst1,clk1;
	
	//signal for serial
	wire clk_ser;
	wire clk_ser2;
	wire cs_serial;
	wire cs_rx;
	reg [7:0] tx_buf;
	reg [7:0] rx_buf;
	reg start_sig=1;
	reg start_sig_delay;
	reg start_sig_delay2;
	wire tx_start;
	
	wire [7:0] tx_buf_wire;
	wire reset_rx=1;
	
	wire [7:0] rx_data;
	wire data_strobe;
	
	wire rst1;
	wire rst2;
	
	
	//segment matches to segment7,db_in db_out match to db
	
	wire cs1;
	wire cs2;
	
	assign cs1=((ab==10'h300)&&(aen==0))?1:0;
	assign cs2=((ab==10'h301)&&(aen==0))?1:0;
	
	assign iocs16_n=1;
	
	wire cs3;
	wire cs31;
	reg rst1_1=1;
	reg rst2_1=1;
	assign cs3=((ab==10'h302)&&(aen==0))?1:0;
	assign cs31=((ab==10'h303)&&(aen==0))?1:0;
	assign rst1 = rst1_1;
	assign rst2 = rst2_1;
	
	
	assign cs_serial=((ab==10'h305)&&(aen==0))?1:0;
	
	always@(posedge iow_n)
	begin
		
		if(cs1)
		begin
			segment <= db_in;
		end
		if(cs2)
		begin
			timer <= db_in[3:0];
		end
		if (cs3)
		begin 
			rst1_1 <= db_in[0];
		end
		if (cs31)
		begin 
			rst2_1 <= db_in[0];
		end
		
		if (cs_serial)
		begin 
			tx_buf <= db_in;
			start_sig <= ~start_sig;
		end
	end
	
	//code for serial 9600 Hz
	division3 div3_inst(clk,clk_ser);
	division4 div4_inst(clk,clk_ser2);
	
	

	
	
	always@ (posedge clk_ser)
	begin
		start_sig_delay <= start_sig;
		start_sig_delay2 <= start_sig_delay;
	
	end
	
	assign tx_start = start_sig_delay ^ start_sig_delay2;
	
	assign tx_buf_wire = tx_buf;
	uart_tx tx_inst(clk_ser, tx_buf_wire, tx_start, tx_data);
	
	
	
	uart_rx rx_inst(clk_ser2, reset_rx, rx_data, data_strobe, rx);
	
	always@ (negedge data_strobe)
	begin
		rx_buf <= rx_data;
	end
	
	assign cs_rx=((ab==10'h306)&&(aen==0))?1:0;
	
	
	
	// end for serial 
	
	wire cs11,cs12,cs13;
	assign cs11=((ab==10'h310)&&(aen==0))?1:0;
	assign cs12=((ab==10'h311)&&(aen==0))?1:0;
	assign cs13=((ab==10'h312)&&(aen==0))?1:0;
	
	
	wire cs21;
	wire cs22;
	assign cs21=((ab==10'h320)&&(aen==0))?1:0;
	assign cs22=((ab==10'h321)&&(aen==0))?1:0;
	
	

	assign oe_n=ior_n|((~cs11)&&(~cs21)&&(~cs22)&&(~cs_rx));


	
	wire[19:0] count;
	wire[19:0] count2;
	wire clk1;


	division div_inst1(clk,rst1,count,clk1);
	
	wire clk2;
	
	division2 div_inst2(clk,rst2,count2,clk2);
	
	// irq 7 for clock
	assign irq7 = clk1;
	// irq 11 for serial
	assign irq11= data_strobe;
	

	
	
	
	reg [7:0] clkcnt1=8'h0;
	always@ (posedge clk1)
	begin
		if (clkcnt1< 8'hff) 
		begin 
			clkcnt1 <= clkcnt1+1;
		end
		else 
			clkcnt1 <= 8'h0;
	end
	
	
	reg [7:0] clkcnt2=8'h0;
	always@ (posedge clk2)
	begin
		if (clkcnt2< 8'hff) 
		begin 
			clkcnt2 <= clkcnt2+1;
		end
		else 
			clkcnt2 <= 8'h0;
	end
	
	
	always@ (negedge ior_n)
	begin
		
		if (cs11) begin  db_out[0] <= but1;db_out[1] <= but2;db_out[2] <= but3; end
		/*if (cs12) begin  db_out[0] <= but2; end
		if (cs13) begin  db_out[0] <= but3; end*/
		if (cs21) 
		begin
			db_out <= clkcnt1;
		end
		if (cs22)
		begin 
			db_out <= clkcnt2;
		end
		
		if (cs_rx) db_out <= rx_buf;
		
		
	end
	
	

endmodule
