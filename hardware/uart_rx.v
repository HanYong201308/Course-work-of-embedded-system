`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date:    11:00:14 02/28/2008 
// Design Name: 
// Module Name:    uart_rx 
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

module uart_rx(clk, reset, rx_data, data_strobe, rx);

input clk;
input reset;
input rx;

output [7:0] rx_data;
output reg data_strobe;

reg start_rx;
reg [7:0] rx_data;
reg [7:0] cnt_rx_reg1;

reg rx0, rx1, rx2, rx3, rx4, rx5, rx6;
wire start_bit_detected;

always@ (posedge clk or negedge reset)
begin
	if (!reset)
	begin
		rx0 <= 1'b0;
		rx1 <= 1'b0;
		rx2 <= 1'b0;
		rx3 <= 1'b0;
		rx4 <= 1'b0;
		rx5 <= 1'b0;
		rx6 <= 1'b0;
	end
	
	else 
	begin
		rx0 <= rx;
		rx1 <= rx0;
		rx2 <= rx1;
		rx3 <= rx2;
		rx4 <= rx3;
		rx5 <= rx4;
		rx6 <= rx5;
	end

end

assign start_bit_detected = (rx6)&(rx5)&(~rx4)&(~rx3)&(~rx2)&(~rx1)&(~rx0);

always@ (posedge clk or negedge reset)
begin
	if (!reset)
	begin
		data_strobe <= 1'b0;
		start_rx <= 1'b0;
	end
	
	else 
	begin
		if (start_bit_detected &&(~start_rx))
		begin
			data_strobe <= 1;
			cnt_rx_reg1 <= 8'd0;
			start_rx <= 1;
		end
		
		if (start_rx)
		begin
			cnt_rx_reg1 <= cnt_rx_reg1 +1;
			if (cnt_rx_reg1 == 8'd7) rx_data[0] <= rx;
			else if (cnt_rx_reg1 == 8'd15) rx_data[1] <= rx;
			else if (cnt_rx_reg1 == 8'd23) rx_data[2] <= rx;
			else if (cnt_rx_reg1 == 8'd31) rx_data[3] <= rx;
			else if (cnt_rx_reg1 == 8'd39) rx_data[4] <= rx;
			else if (cnt_rx_reg1 == 8'd47) rx_data[5] <= rx;
			else if (cnt_rx_reg1 == 8'd55) rx_data[6] <= rx;
			else if (cnt_rx_reg1 == 8'd63) rx_data[7] <= rx;
			
			else if (cnt_rx_reg1 == 8'd64) 
			begin
				start_rx <= 0;
				data_strobe <= 0;
			end
			
		
		end
			
		
	end
end

endmodule
