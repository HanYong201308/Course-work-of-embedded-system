`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date:    11:00:35 02/28/2008 
// Design Name: 
// Module Name:    uart_tx 
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

module uart_tx(BaudTick, TxD_data, TxD_start, TxD);

input BaudTick;
input [7:0] TxD_data;
input TxD_start;
output TxD;

reg [3:0] state;
reg muxbit;


always @(posedge BaudTick)
case(state)
	4'b0000: if(TxD_start) state <= 4'b0100;
	4'b0100:  state <= 4'b1000; // start
	4'b1000:  state <= 4'b1001; // bit 0
	4'b1001:  state <= 4'b1010; // bit 1
	4'b1010:  state <= 4'b1011; // bit 2
	4'b1011:  state <= 4'b1100; // bit 3
	4'b1100:  state <= 4'b1101; // bit 4
	4'b1101:  state <= 4'b1110; // bit 5
	4'b1110:  state <= 4'b1111; // bit 6
	4'b1111:  state <= 4'b0001; // bit 7
	4'b0001:  state <= 4'b0010; // stop1
	4'b0010:  state <= 4'b0000; // stop2
	default:  state <= 4'b0000;
endcase 




always @(state[2:0])
case(state[2:0])
	0: muxbit <= TxD_data[0];
	1: muxbit <= TxD_data[1];
	2: muxbit <= TxD_data[2];
	3: muxbit <= TxD_data[3];
	4: muxbit <= TxD_data[4];
	5: muxbit <= TxD_data[5];
	6: muxbit <= TxD_data[6];
	7: muxbit <= TxD_data[7];
//	0: muxbit <= 1;
//	1: muxbit <= 1;
//	2: muxbit <= 0;
//	3: muxbit <= 0;
//	4: muxbit <= 1;
//	5: muxbit <= 0;
//	6: muxbit <= 0;
//	7: muxbit <= 0;
endcase

//full sequence

assign TxD = (state<4) | (state[3] & muxbit);

endmodule



