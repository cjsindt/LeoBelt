// License: Apache 2.0. See LICENSE file in root directory.
// Copyright(c) 2017 Intel Corporation. All Rights Reserved.

#include <librealsense2/rs.hpp>
#include "common/example.hpp"
#include "third-party/imgui/imgui.h" // <imgui.h>
#include "third-party/imgui/imgui_impl_glfw.h"
#include <SFML/Graphics.hpp>

#include <sstream>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <cstring>
#include <cmath>

#include <iostream>
#include <vector>
#include <stdio.h>
#include <unistd.h>
#include <stdint.h>
#include "rs232.h"

// 3rd party header for writing png files
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb/stb_image_write.h"

// Client side implementation of UDP client-server model
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
  
#define PORT     8080
#define MAXLINE 1024
#define serverIP "192.168.137.210"

#define BUF_SIZE 123
char str_send[2048][BUF_SIZE]; // send data buffer
int cport_nr = 16;

float get_depth_scale(rs2::device dev);
rs2_stream find_stream_to_align(const std::vector<rs2::stream_profile>& streams);
bool profile_changed(const std::vector<rs2::stream_profile>& current, const std::vector<rs2::stream_profile>& prev);
int* printPixelDepth(rs2::video_frame& other_frame, const rs2::depth_frame& depth_frame, float depth_scale);
void silenceAllFeathers();
void testButton();

int main(int argc, char * argv[]) try
{
    int bdrate = 57600;
    
    char mode[] ={'8','N','1',0}; // 8 data bits, no parity, 1 stop bit
    unsigned char str_recv[BUF_SIZE]; // recv data buffer
    
    // Strings that will be sent over serial to the Master Feather
    // Format is "Intensity-Intensity-Feather#"
    strcpy(str_send[0], "A#1"); strcpy(str_send[1], "B#1"); strcpy(str_send[2], "C#1"); strcpy(str_send[3], "D#1");
    strcpy(str_send[4], "E#1"); strcpy(str_send[5], "F#1"); strcpy(str_send[6], "G#1"); strcpy(str_send[7], "H#1");
    strcpy(str_send[8], "I#1"); strcpy(str_send[9], "J#1"); strcpy(str_send[10], "K#1"); strcpy(str_send[11], "L#1");
    strcpy(str_send[12], "M#1"); strcpy(str_send[13], "N#1"); strcpy(str_send[14], "O#1"); strcpy(str_send[15], "P#1");
    strcpy(str_send[16], "Q#1"); strcpy(str_send[17], "R#1"); strcpy(str_send[18], "S#1"); strcpy(str_send[19], "T#1");
    strcpy(str_send[20], "U#1"); strcpy(str_send[21], "V#1"); strcpy(str_send[22], "W#1"); strcpy(str_send[23], "X#1");
    strcpy(str_send[24], "Y#1"); strcpy(str_send[25], "Z#1"); strcpy(str_send[26], "a#1"); strcpy(str_send[27], "b#1");
    strcpy(str_send[28], "c#1"); strcpy(str_send[29], "d#1"); strcpy(str_send[30], "e#1"); strcpy(str_send[31], "f#1");
    strcpy(str_send[32], "g#1"); strcpy(str_send[33], "h#1"); strcpy(str_send[34], "i#1"); strcpy(str_send[35], "j#1");
    strcpy(str_send[36], "k#1"); strcpy(str_send[37], "l#1"); strcpy(str_send[38], "m#1"); strcpy(str_send[39], "n#1");
    strcpy(str_send[40], "o#1"); strcpy(str_send[41], "p#1"); strcpy(str_send[42], "q#1"); strcpy(str_send[43], "r#1");
    strcpy(str_send[44], "s#1"); strcpy(str_send[45], "t#1"); strcpy(str_send[46], "u#1"); strcpy(str_send[47], "v#1");
    strcpy(str_send[48], "w#1"); strcpy(str_send[49], "x#1"); strcpy(str_send[50], "y#1"); strcpy(str_send[51], "z#1");
    strcpy(str_send[52], "A$1"); strcpy(str_send[53], "B$1"); strcpy(str_send[54], "C$1"); strcpy(str_send[55], "D$1");
    strcpy(str_send[56], "E$1"); strcpy(str_send[57], "F$1"); strcpy(str_send[58], "G$1"); strcpy(str_send[59], "H$1");
    strcpy(str_send[60], "I$1"); strcpy(str_send[61], "J$1"); strcpy(str_send[62], "K$1"); strcpy(str_send[63], "L$1");
    strcpy(str_send[64], "M$1"); strcpy(str_send[65], "N$1"); strcpy(str_send[66], "O$1"); strcpy(str_send[67], "P$1");
    strcpy(str_send[68], "Q$1"); strcpy(str_send[69], "R$1"); strcpy(str_send[70], "S$1"); strcpy(str_send[71], "T$1");
    strcpy(str_send[72], "U$1"); strcpy(str_send[73], "V$1"); strcpy(str_send[74], "W$1"); strcpy(str_send[75], "X$1");
    strcpy(str_send[76], "Y$1"); strcpy(str_send[77], "Z$1"); strcpy(str_send[78], "a$1"); strcpy(str_send[79], "b$1");
    strcpy(str_send[80], "c$1"); strcpy(str_send[81], "d$1"); strcpy(str_send[82], "e$1"); strcpy(str_send[83], "f$1");
    strcpy(str_send[84], "g$1"); strcpy(str_send[85], "h$1"); strcpy(str_send[86], "i$1"); strcpy(str_send[87], "j$1");
    strcpy(str_send[88], "k$1"); strcpy(str_send[89], "l$1"); strcpy(str_send[90], "m$1"); strcpy(str_send[91], "n$1");
    strcpy(str_send[92], "o$1"); strcpy(str_send[93], "p$1"); strcpy(str_send[94], "q$1"); strcpy(str_send[95], "r$1");
    strcpy(str_send[96], "s$1"); strcpy(str_send[97], "t$1"); strcpy(str_send[98], "u$1"); strcpy(str_send[99], "v$1");
    strcpy(str_send[100], "w$1"); strcpy(str_send[101], "x$1"); strcpy(str_send[102], "y$1"); strcpy(str_send[103], "z$1");
    strcpy(str_send[104], "A%1"); strcpy(str_send[105], "B%1"); strcpy(str_send[106], "C%1"); strcpy(str_send[107], "D%1");
    strcpy(str_send[108], "E%1"); strcpy(str_send[109], "F%1"); strcpy(str_send[110], "G%1"); strcpy(str_send[111], "H%1");
    strcpy(str_send[112], "I%1"); strcpy(str_send[113], "J%1"); strcpy(str_send[114], "K%1"); strcpy(str_send[115], "L%1");
    strcpy(str_send[116], "M%1"); strcpy(str_send[117], "N%1"); strcpy(str_send[118], "O%1"); strcpy(str_send[119], "P%1");
    strcpy(str_send[120], "Q%1"); strcpy(str_send[121], "R%1"); strcpy(str_send[122], "S%1"); strcpy(str_send[123], "T%1");
    strcpy(str_send[124], "U%1"); strcpy(str_send[125], "V%1"); strcpy(str_send[126], "W%1"); strcpy(str_send[127], "X%1");
    strcpy(str_send[128], "Y%1"); strcpy(str_send[129], "Z%1"); strcpy(str_send[130], "a%1"); strcpy(str_send[131], "b%1");
    strcpy(str_send[132], "c%1"); strcpy(str_send[133], "d%1"); strcpy(str_send[134], "e%1"); strcpy(str_send[135], "f%1");
    strcpy(str_send[136], "g%1"); strcpy(str_send[137], "h%1"); strcpy(str_send[138], "i%1"); strcpy(str_send[139], "j%1");
    strcpy(str_send[140], "k%1"); strcpy(str_send[141], "l%1"); strcpy(str_send[142], "m%1"); strcpy(str_send[143], "n%1");
    strcpy(str_send[144], "o%1"); strcpy(str_send[145], "p%1"); strcpy(str_send[146], "q%1"); strcpy(str_send[147], "r%1");
    strcpy(str_send[148], "s%1"); strcpy(str_send[149], "t%1"); strcpy(str_send[150], "u%1"); strcpy(str_send[151], "v%1");
    strcpy(str_send[152], "w%1"); strcpy(str_send[153], "x%1"); strcpy(str_send[154], "y%1"); strcpy(str_send[155], "z%1");
    strcpy(str_send[156], "A&1"); strcpy(str_send[157], "B&1"); strcpy(str_send[158], "C&1"); strcpy(str_send[159], "D&1");
    strcpy(str_send[160], "E&1"); strcpy(str_send[161], "F&1"); strcpy(str_send[162], "G&1"); strcpy(str_send[163], "H&1");
    strcpy(str_send[164], "I&1"); strcpy(str_send[165], "J&1"); strcpy(str_send[166], "K&1"); strcpy(str_send[167], "L&1");
    strcpy(str_send[168], "M&1"); strcpy(str_send[169], "N&1"); strcpy(str_send[170], "O&1"); strcpy(str_send[171], "P&1");
    strcpy(str_send[172], "Q&1"); strcpy(str_send[173], "R&1"); strcpy(str_send[174], "S&1"); strcpy(str_send[175], "T&1");
    strcpy(str_send[176], "U&1"); strcpy(str_send[177], "V&1"); strcpy(str_send[178], "W&1"); strcpy(str_send[179], "X&1");
    strcpy(str_send[180], "Y&1"); strcpy(str_send[181], "Z&1"); strcpy(str_send[182], "a&1"); strcpy(str_send[183], "b&1");
    strcpy(str_send[184], "c&1"); strcpy(str_send[185], "d&1"); strcpy(str_send[186], "e&1"); strcpy(str_send[187], "f&1");
    strcpy(str_send[188], "g&1"); strcpy(str_send[189], "h&1"); strcpy(str_send[190], "i&1"); strcpy(str_send[191], "j&1");
    strcpy(str_send[192], "k&1"); strcpy(str_send[193], "l&1"); strcpy(str_send[194], "m&1"); strcpy(str_send[195], "n&1");
    strcpy(str_send[196], "o&1"); strcpy(str_send[197], "p&1"); strcpy(str_send[198], "q&1"); strcpy(str_send[199], "r&1");
    strcpy(str_send[200], "s&1"); strcpy(str_send[201], "t&1"); strcpy(str_send[202], "u&1"); strcpy(str_send[203], "v&1");
    strcpy(str_send[204], "w&1"); strcpy(str_send[205], "x&1"); strcpy(str_send[206], "y&1"); strcpy(str_send[207], "z&1");
    strcpy(str_send[208], "A'1"); strcpy(str_send[209], "B'1"); strcpy(str_send[210], "C'1"); strcpy(str_send[211], "D'1");
    strcpy(str_send[212], "E'1"); strcpy(str_send[213], "F'1"); strcpy(str_send[214], "G'1"); strcpy(str_send[215], "H'1");
    strcpy(str_send[216], "I'1"); strcpy(str_send[217], "J'1"); strcpy(str_send[218], "K'1"); strcpy(str_send[219], "L'1");
    strcpy(str_send[220], "M'1"); strcpy(str_send[221], "N'1"); strcpy(str_send[222], "O'1"); strcpy(str_send[223], "P'1");
    strcpy(str_send[224], "Q'1"); strcpy(str_send[225], "R'1"); strcpy(str_send[226], "S'1"); strcpy(str_send[227], "T'1");
    strcpy(str_send[228], "U'1"); strcpy(str_send[229], "V'1"); strcpy(str_send[230], "W'1"); strcpy(str_send[231], "X'1");
    strcpy(str_send[232], "Y'1"); strcpy(str_send[233], "Z'1"); strcpy(str_send[234], "a'1"); strcpy(str_send[235], "b'1");
    strcpy(str_send[236], "c'1"); strcpy(str_send[237], "d'1"); strcpy(str_send[238], "e'1"); strcpy(str_send[239], "f'1");
    strcpy(str_send[240], "g'1"); strcpy(str_send[241], "h'1"); strcpy(str_send[242], "i'1"); strcpy(str_send[243], "j'1");
    strcpy(str_send[244], "k'1"); strcpy(str_send[245], "l'1"); strcpy(str_send[246], "m'1"); strcpy(str_send[247], "n'1");
    strcpy(str_send[248], "o'1"); strcpy(str_send[249], "p'1"); strcpy(str_send[250], "q'1"); strcpy(str_send[251], "r'1");
    strcpy(str_send[252], "s'1"); strcpy(str_send[253], "t'1"); strcpy(str_send[254], "u'1"); strcpy(str_send[255], "v'1");
    strcpy(str_send[256], "A#2"); strcpy(str_send[257], "B#2"); strcpy(str_send[258], "C#2"); strcpy(str_send[259], "D#2");
    strcpy(str_send[260], "E#2"); strcpy(str_send[261], "F#2"); strcpy(str_send[262], "G#2"); strcpy(str_send[263], "H#2");
    strcpy(str_send[264], "I#2"); strcpy(str_send[265], "J#2"); strcpy(str_send[266], "K#2"); strcpy(str_send[267], "L#2");
    strcpy(str_send[268], "M#2"); strcpy(str_send[269], "N#2"); strcpy(str_send[270], "O#2"); strcpy(str_send[271], "P#2");
    strcpy(str_send[272], "Q#2"); strcpy(str_send[273], "R#2"); strcpy(str_send[274], "S#2"); strcpy(str_send[275], "T#2");
    strcpy(str_send[276], "U#2"); strcpy(str_send[277], "V#2"); strcpy(str_send[278], "W#2"); strcpy(str_send[279], "X#2");
    strcpy(str_send[280], "Y#2"); strcpy(str_send[281], "Z#2"); strcpy(str_send[282], "a#2"); strcpy(str_send[283], "b#2");
    strcpy(str_send[284], "c#2"); strcpy(str_send[285], "d#2"); strcpy(str_send[286], "e#2"); strcpy(str_send[287], "f#2");
    strcpy(str_send[288], "g#2"); strcpy(str_send[289], "h#2"); strcpy(str_send[290], "i#2"); strcpy(str_send[291], "j#2");
    strcpy(str_send[292], "k#2"); strcpy(str_send[293], "l#2"); strcpy(str_send[294], "m#2"); strcpy(str_send[295], "n#2");
    strcpy(str_send[296], "o#2"); strcpy(str_send[297], "p#2"); strcpy(str_send[298], "q#2"); strcpy(str_send[299], "r#2");
    strcpy(str_send[300], "s#2"); strcpy(str_send[301], "t#2"); strcpy(str_send[302], "u#2"); strcpy(str_send[303], "v#2");
    strcpy(str_send[304], "w#2"); strcpy(str_send[305], "x#2"); strcpy(str_send[306], "y#2"); strcpy(str_send[307], "z#2");
    strcpy(str_send[308], "A$2"); strcpy(str_send[309], "B$2"); strcpy(str_send[310], "C$2"); strcpy(str_send[311], "D$2");
    strcpy(str_send[312], "E$2"); strcpy(str_send[313], "F$2"); strcpy(str_send[314], "G$2"); strcpy(str_send[315], "H$2");
    strcpy(str_send[316], "I$2"); strcpy(str_send[317], "J$2"); strcpy(str_send[318], "K$2"); strcpy(str_send[319], "L$2");
    strcpy(str_send[320], "M$2"); strcpy(str_send[321], "N$2"); strcpy(str_send[322], "O$2"); strcpy(str_send[323], "P$2");
    strcpy(str_send[324], "Q$2"); strcpy(str_send[325], "R$2"); strcpy(str_send[326], "S$2"); strcpy(str_send[327], "T$2");
    strcpy(str_send[328], "U$2"); strcpy(str_send[329], "V$2"); strcpy(str_send[330], "W$2"); strcpy(str_send[331], "X$2");
    strcpy(str_send[332], "Y$2"); strcpy(str_send[333], "Z$2"); strcpy(str_send[334], "a$2"); strcpy(str_send[335], "b$2");
    strcpy(str_send[336], "c$2"); strcpy(str_send[337], "d$2"); strcpy(str_send[338], "e$2"); strcpy(str_send[339], "f$2");
    strcpy(str_send[340], "g$2"); strcpy(str_send[341], "h$2"); strcpy(str_send[342], "i$2"); strcpy(str_send[343], "j$2");
    strcpy(str_send[344], "k$2"); strcpy(str_send[345], "l$2"); strcpy(str_send[346], "m$2"); strcpy(str_send[347], "n$2");
    strcpy(str_send[348], "o$2"); strcpy(str_send[349], "p$2"); strcpy(str_send[350], "q$2"); strcpy(str_send[351], "r$2");
    strcpy(str_send[352], "s$2"); strcpy(str_send[353], "t$2"); strcpy(str_send[354], "u$2"); strcpy(str_send[355], "v$2");
    strcpy(str_send[356], "w$2"); strcpy(str_send[357], "x$2"); strcpy(str_send[358], "y$2"); strcpy(str_send[359], "z$2");
    strcpy(str_send[360], "A%2"); strcpy(str_send[361], "B%2"); strcpy(str_send[362], "C%2"); strcpy(str_send[363], "D%2");
    strcpy(str_send[364], "E%2"); strcpy(str_send[365], "F%2"); strcpy(str_send[366], "G%2"); strcpy(str_send[367], "H%2");
    strcpy(str_send[368], "I%2"); strcpy(str_send[369], "J%2"); strcpy(str_send[370], "K%2"); strcpy(str_send[371], "L%2");
    strcpy(str_send[372], "M%2"); strcpy(str_send[373], "N%2"); strcpy(str_send[374], "O%2"); strcpy(str_send[375], "P%2");
    strcpy(str_send[376], "Q%2"); strcpy(str_send[377], "R%2"); strcpy(str_send[378], "S%2"); strcpy(str_send[379], "T%2");
    strcpy(str_send[380], "U%2"); strcpy(str_send[381], "V%2"); strcpy(str_send[382], "W%2"); strcpy(str_send[383], "X%2");
    strcpy(str_send[384], "Y%2"); strcpy(str_send[385], "Z%2"); strcpy(str_send[386], "a%2"); strcpy(str_send[387], "b%2");
    strcpy(str_send[388], "c%2"); strcpy(str_send[389], "d%2"); strcpy(str_send[390], "e%2"); strcpy(str_send[391], "f%2");
    strcpy(str_send[392], "g%2"); strcpy(str_send[393], "h%2"); strcpy(str_send[394], "i%2"); strcpy(str_send[395], "j%2");
    strcpy(str_send[396], "k%2"); strcpy(str_send[397], "l%2"); strcpy(str_send[398], "m%2"); strcpy(str_send[399], "n%2");
    strcpy(str_send[400], "o%2"); strcpy(str_send[401], "p%2"); strcpy(str_send[402], "q%2"); strcpy(str_send[403], "r%2");
    strcpy(str_send[404], "s%2"); strcpy(str_send[405], "t%2"); strcpy(str_send[406], "u%2"); strcpy(str_send[407], "v%2");
    strcpy(str_send[408], "w%2"); strcpy(str_send[409], "x%2"); strcpy(str_send[410], "y%2"); strcpy(str_send[411], "z%2");
    strcpy(str_send[412], "A&2"); strcpy(str_send[413], "B&2"); strcpy(str_send[414], "C&2"); strcpy(str_send[415], "D&2");
    strcpy(str_send[416], "E&2"); strcpy(str_send[417], "F&2"); strcpy(str_send[418], "G&2"); strcpy(str_send[419], "H&2");
    strcpy(str_send[420], "I&2"); strcpy(str_send[421], "J&2"); strcpy(str_send[422], "K&2"); strcpy(str_send[423], "L&2");
    strcpy(str_send[424], "M&2"); strcpy(str_send[425], "N&2"); strcpy(str_send[426], "O&2"); strcpy(str_send[427], "P&2");
    strcpy(str_send[428], "Q&2"); strcpy(str_send[429], "R&2"); strcpy(str_send[430], "S&2"); strcpy(str_send[431], "T&2");
    strcpy(str_send[432], "U&2"); strcpy(str_send[433], "V&2"); strcpy(str_send[434], "W&2"); strcpy(str_send[435], "X&2");
    strcpy(str_send[436], "Y&2"); strcpy(str_send[437], "Z&2"); strcpy(str_send[438], "a&2"); strcpy(str_send[439], "b&2");
    strcpy(str_send[440], "c&2"); strcpy(str_send[441], "d&2"); strcpy(str_send[442], "e&2"); strcpy(str_send[443], "f&2");
    strcpy(str_send[444], "g&2"); strcpy(str_send[445], "h&2"); strcpy(str_send[446], "i&2"); strcpy(str_send[447], "j&2");
    strcpy(str_send[448], "k&2"); strcpy(str_send[449], "l&2"); strcpy(str_send[450], "m&2"); strcpy(str_send[451], "n&2");
    strcpy(str_send[452], "o&2"); strcpy(str_send[453], "p&2"); strcpy(str_send[454], "q&2"); strcpy(str_send[455], "r&2");
    strcpy(str_send[456], "s&2"); strcpy(str_send[457], "t&2"); strcpy(str_send[458], "u&2"); strcpy(str_send[459], "v&2");
    strcpy(str_send[460], "w&2"); strcpy(str_send[461], "x&2"); strcpy(str_send[462], "y&2"); strcpy(str_send[463], "z&2");
    strcpy(str_send[464], "A'2"); strcpy(str_send[465], "B'2"); strcpy(str_send[466], "C'2"); strcpy(str_send[467], "D'2");
    strcpy(str_send[468], "E'2"); strcpy(str_send[469], "F'2"); strcpy(str_send[470], "G'2"); strcpy(str_send[471], "H'2");
    strcpy(str_send[472], "I'2"); strcpy(str_send[473], "J'2"); strcpy(str_send[474], "K'2"); strcpy(str_send[475], "L'2");
    strcpy(str_send[476], "M'2"); strcpy(str_send[477], "N'2"); strcpy(str_send[478], "O'2"); strcpy(str_send[479], "P'2");
    strcpy(str_send[480], "Q'2"); strcpy(str_send[481], "R'2"); strcpy(str_send[482], "S'2"); strcpy(str_send[483], "T'2");
    strcpy(str_send[484], "U'2"); strcpy(str_send[485], "V'2"); strcpy(str_send[486], "W'2"); strcpy(str_send[487], "X'2");
    strcpy(str_send[488], "Y'2"); strcpy(str_send[489], "Z'2"); strcpy(str_send[490], "a'2"); strcpy(str_send[491], "b'2");
    strcpy(str_send[492], "c'2"); strcpy(str_send[493], "d'2"); strcpy(str_send[494], "e'2"); strcpy(str_send[495], "f'2");
    strcpy(str_send[496], "g'2"); strcpy(str_send[497], "h'2"); strcpy(str_send[498], "i'2"); strcpy(str_send[499], "j'2");
    strcpy(str_send[500], "k'2"); strcpy(str_send[501], "l'2"); strcpy(str_send[502], "m'2"); strcpy(str_send[503], "n'2");
    strcpy(str_send[504], "o'2"); strcpy(str_send[505], "p'2"); strcpy(str_send[506], "q'2"); strcpy(str_send[507], "r'2");
    strcpy(str_send[508], "s'2"); strcpy(str_send[509], "t'2"); strcpy(str_send[510], "u'2"); strcpy(str_send[511], "v'2");
    strcpy(str_send[512], "A#3"); strcpy(str_send[513], "B#3"); strcpy(str_send[514], "C#3"); strcpy(str_send[515], "D#3");
    strcpy(str_send[516], "E#3"); strcpy(str_send[517], "F#3"); strcpy(str_send[518], "G#3"); strcpy(str_send[519], "H#3");
    strcpy(str_send[520], "I#3"); strcpy(str_send[521], "J#3"); strcpy(str_send[522], "K#3"); strcpy(str_send[523], "L#3");
    strcpy(str_send[524], "M#3"); strcpy(str_send[525], "N#3"); strcpy(str_send[526], "O#3"); strcpy(str_send[527], "P#3");
    strcpy(str_send[528], "Q#3"); strcpy(str_send[529], "R#3"); strcpy(str_send[530], "S#3"); strcpy(str_send[531], "T#3");
    strcpy(str_send[532], "U#3"); strcpy(str_send[533], "V#3"); strcpy(str_send[534], "W#3"); strcpy(str_send[535], "X#3");
    strcpy(str_send[536], "Y#3"); strcpy(str_send[537], "Z#3"); strcpy(str_send[538], "a#3"); strcpy(str_send[539], "b#3");
    strcpy(str_send[540], "c#3"); strcpy(str_send[541], "d#3"); strcpy(str_send[542], "e#3"); strcpy(str_send[543], "f#3");
    strcpy(str_send[544], "g#3"); strcpy(str_send[545], "h#3"); strcpy(str_send[546], "i#3"); strcpy(str_send[547], "j#3");
    strcpy(str_send[548], "k#3"); strcpy(str_send[549], "l#3"); strcpy(str_send[550], "m#3"); strcpy(str_send[551], "n#3");
    strcpy(str_send[552], "o#3"); strcpy(str_send[553], "p#3"); strcpy(str_send[554], "q#3"); strcpy(str_send[555], "r#3");
    strcpy(str_send[556], "s#3"); strcpy(str_send[557], "t#3"); strcpy(str_send[558], "u#3"); strcpy(str_send[559], "v#3");
    strcpy(str_send[560], "w#3"); strcpy(str_send[561], "x#3"); strcpy(str_send[562], "y#3"); strcpy(str_send[563], "z#3");
    strcpy(str_send[564], "A$3"); strcpy(str_send[565], "B$3"); strcpy(str_send[566], "C$3"); strcpy(str_send[567], "D$3");
    strcpy(str_send[568], "E$3"); strcpy(str_send[569], "F$3"); strcpy(str_send[570], "G$3"); strcpy(str_send[571], "H$3");
    strcpy(str_send[572], "I$3"); strcpy(str_send[573], "J$3"); strcpy(str_send[574], "K$3"); strcpy(str_send[575], "L$3");
    strcpy(str_send[576], "M$3"); strcpy(str_send[577], "N$3"); strcpy(str_send[578], "O$3"); strcpy(str_send[579], "P$3");
    strcpy(str_send[580], "Q$3"); strcpy(str_send[581], "R$3"); strcpy(str_send[582], "S$3"); strcpy(str_send[583], "T$3");
    strcpy(str_send[584], "U$3"); strcpy(str_send[585], "V$3"); strcpy(str_send[586], "W$3"); strcpy(str_send[587], "X$3");
    strcpy(str_send[588], "Y$3"); strcpy(str_send[589], "Z$3"); strcpy(str_send[590], "a$3"); strcpy(str_send[591], "b$3");
    strcpy(str_send[592], "c$3"); strcpy(str_send[593], "d$3"); strcpy(str_send[594], "e$3"); strcpy(str_send[595], "f$3");
    strcpy(str_send[596], "g$3"); strcpy(str_send[597], "h$3"); strcpy(str_send[598], "i$3"); strcpy(str_send[599], "j$3");
    strcpy(str_send[600], "k$3"); strcpy(str_send[601], "l$3"); strcpy(str_send[602], "m$3"); strcpy(str_send[603], "n$3");
    strcpy(str_send[604], "o$3"); strcpy(str_send[605], "p$3"); strcpy(str_send[606], "q$3"); strcpy(str_send[607], "r$3");
    strcpy(str_send[608], "s$3"); strcpy(str_send[609], "t$3"); strcpy(str_send[610], "u$3"); strcpy(str_send[611], "v$3");
    strcpy(str_send[612], "w$3"); strcpy(str_send[613], "x$3"); strcpy(str_send[614], "y$3"); strcpy(str_send[615], "z$3");
    strcpy(str_send[616], "A%3"); strcpy(str_send[617], "B%3"); strcpy(str_send[618], "C%3"); strcpy(str_send[619], "D%3");
    strcpy(str_send[620], "E%3"); strcpy(str_send[621], "F%3"); strcpy(str_send[622], "G%3"); strcpy(str_send[623], "H%3");
    strcpy(str_send[624], "I%3"); strcpy(str_send[625], "J%3"); strcpy(str_send[626], "K%3"); strcpy(str_send[627], "L%3");
    strcpy(str_send[628], "M%3"); strcpy(str_send[629], "N%3"); strcpy(str_send[630], "O%3"); strcpy(str_send[631], "P%3");
    strcpy(str_send[632], "Q%3"); strcpy(str_send[633], "R%3"); strcpy(str_send[634], "S%3"); strcpy(str_send[635], "T%3");
    strcpy(str_send[636], "U%3"); strcpy(str_send[637], "V%3"); strcpy(str_send[638], "W%3"); strcpy(str_send[639], "X%3"); 
    strcpy(str_send[640], "Y%3"); strcpy(str_send[641], "Z%3"); strcpy(str_send[642], "a%3"); strcpy(str_send[643], "b%3");
    strcpy(str_send[644], "c%3"); strcpy(str_send[645], "d%3"); strcpy(str_send[646], "e%3"); strcpy(str_send[647], "f%3");
    strcpy(str_send[648], "g%3"); strcpy(str_send[649], "h%3"); strcpy(str_send[650], "i%3"); strcpy(str_send[651], "j%3");
    strcpy(str_send[652], "k%3"); strcpy(str_send[653], "l%3"); strcpy(str_send[654], "m%3"); strcpy(str_send[655], "n%3");
    strcpy(str_send[656], "o%3"); strcpy(str_send[657], "p%3"); strcpy(str_send[658], "q%3"); strcpy(str_send[659], "r%3");
    strcpy(str_send[660], "s%3"); strcpy(str_send[661], "t%3"); strcpy(str_send[662], "u%3"); strcpy(str_send[663], "v%3");
    strcpy(str_send[664], "w%3"); strcpy(str_send[665], "x%3"); strcpy(str_send[666], "y%3"); strcpy(str_send[667], "z%3");
    strcpy(str_send[668], "A&3"); strcpy(str_send[669], "B&3"); strcpy(str_send[670], "C&3"); strcpy(str_send[671], "D&3");
    strcpy(str_send[672], "E&3"); strcpy(str_send[673], "F&3"); strcpy(str_send[674], "G&3"); strcpy(str_send[675], "H&3");
    strcpy(str_send[676], "I&3"); strcpy(str_send[677], "J&3"); strcpy(str_send[678], "K&3"); strcpy(str_send[679], "L&3");
    strcpy(str_send[680], "M&3"); strcpy(str_send[681], "N&3"); strcpy(str_send[682], "O&3"); strcpy(str_send[683], "P&3");
    strcpy(str_send[684], "Q&3"); strcpy(str_send[685], "R&3"); strcpy(str_send[686], "S&3"); strcpy(str_send[687], "T&3");
    strcpy(str_send[688], "U&3"); strcpy(str_send[689], "V&3"); strcpy(str_send[690], "W&3"); strcpy(str_send[691], "X&3");
    strcpy(str_send[692], "Y&3"); strcpy(str_send[693], "Z&3"); strcpy(str_send[694], "a&3"); strcpy(str_send[695], "b&3");
    strcpy(str_send[696], "c&3"); strcpy(str_send[697], "d&3"); strcpy(str_send[698], "e&3"); strcpy(str_send[699], "f&3");
    strcpy(str_send[700], "g&3"); strcpy(str_send[701], "h&3"); strcpy(str_send[702], "i&3"); strcpy(str_send[703], "j&3");
    strcpy(str_send[704], "k&3"); strcpy(str_send[705], "l&3"); strcpy(str_send[706], "m&3"); strcpy(str_send[707], "n&3");
    strcpy(str_send[708], "o&3"); strcpy(str_send[709], "p&3"); strcpy(str_send[710], "q&3"); strcpy(str_send[711], "r&3");
    strcpy(str_send[712], "s&3"); strcpy(str_send[713], "t&3"); strcpy(str_send[714], "u&3"); strcpy(str_send[715], "v&3");
    strcpy(str_send[716], "w&3"); strcpy(str_send[717], "x&3"); strcpy(str_send[718], "y&3"); strcpy(str_send[719], "z&3");
    strcpy(str_send[720], "A'3"); strcpy(str_send[721], "B'3"); strcpy(str_send[722], "C'3"); strcpy(str_send[723], "D'3"); 
    strcpy(str_send[724], "E'3"); strcpy(str_send[725], "F'3"); strcpy(str_send[726], "G'3"); strcpy(str_send[727], "H'3"); 
    strcpy(str_send[728], "I'3"); strcpy(str_send[729], "J'3"); strcpy(str_send[730], "K'3"); strcpy(str_send[731], "L'3");
    strcpy(str_send[732], "M'3"); strcpy(str_send[733], "N'3"); strcpy(str_send[734], "O'3"); strcpy(str_send[735], "P'3");
    strcpy(str_send[736], "Q'3"); strcpy(str_send[737], "R'3"); strcpy(str_send[738], "S'3"); strcpy(str_send[739], "T'3");
    strcpy(str_send[740], "U'3"); strcpy(str_send[741], "V'3"); strcpy(str_send[742], "W'3"); strcpy(str_send[743], "X'3");
    strcpy(str_send[744], "Y'3"); strcpy(str_send[745], "Z'3"); strcpy(str_send[746], "a'3"); strcpy(str_send[747], "b'3");
    strcpy(str_send[748], "c'3"); strcpy(str_send[749], "d'3"); strcpy(str_send[750], "e'3"); strcpy(str_send[751], "f'3");
    strcpy(str_send[752], "g'3"); strcpy(str_send[753], "h'3"); strcpy(str_send[754], "i'3"); strcpy(str_send[755], "j'3");
    strcpy(str_send[756], "k'3"); strcpy(str_send[757], "l'3"); strcpy(str_send[758], "m'3"); strcpy(str_send[759], "n'3");
    strcpy(str_send[760], "o'3"); strcpy(str_send[761], "p'3"); strcpy(str_send[762], "q'3"); strcpy(str_send[763], "r'3");
    strcpy(str_send[764], "s'3"); strcpy(str_send[765], "t'3"); strcpy(str_send[766], "u'3"); strcpy(str_send[767], "v'3");
    strcpy(str_send[768], "A#4"); strcpy(str_send[769], "B#4"); strcpy(str_send[770], "C#4"); strcpy(str_send[771], "D#4");
    strcpy(str_send[772], "E#4"); strcpy(str_send[773], "F#4"); strcpy(str_send[774], "G#4"); strcpy(str_send[775], "H#4");
    strcpy(str_send[776], "I#4"); strcpy(str_send[777], "J#4"); strcpy(str_send[778], "K#4"); strcpy(str_send[779], "L#4");
    strcpy(str_send[780], "M#4"); strcpy(str_send[781], "N#4"); strcpy(str_send[782], "O#4"); strcpy(str_send[783], "P#4");
    strcpy(str_send[784], "Q#4"); strcpy(str_send[785], "R#4"); strcpy(str_send[786], "S#4"); strcpy(str_send[787], "T#4");
    strcpy(str_send[788], "U#4"); strcpy(str_send[789], "V#4"); strcpy(str_send[790], "W#4"); strcpy(str_send[791], "X#4");
    strcpy(str_send[792], "Y#4"); strcpy(str_send[793], "Z#4"); strcpy(str_send[794], "a#4"); strcpy(str_send[795], "b#4");
    strcpy(str_send[796], "c#4"); strcpy(str_send[797], "d#4"); strcpy(str_send[798], "e#4"); strcpy(str_send[799], "f#4");
    strcpy(str_send[800], "g#4"); strcpy(str_send[801], "h#4"); strcpy(str_send[802], "i#4"); strcpy(str_send[803], "j#4");
    strcpy(str_send[804], "k#4"); strcpy(str_send[805], "l#4"); strcpy(str_send[806], "m#4"); strcpy(str_send[807], "n#4");
    strcpy(str_send[808], "o#4"); strcpy(str_send[809], "p#4"); strcpy(str_send[810], "q#4"); strcpy(str_send[811], "r#4"); 
    strcpy(str_send[812], "s#4"); strcpy(str_send[813], "t#4"); strcpy(str_send[814], "u#4"); strcpy(str_send[815], "v#4");
    strcpy(str_send[816], "w#4"); strcpy(str_send[817], "x#4"); strcpy(str_send[818], "y#4"); strcpy(str_send[819], "z#4");
    strcpy(str_send[820], "A$4"); strcpy(str_send[821], "B$4"); strcpy(str_send[822], "C$4"); strcpy(str_send[823], "D$4");
    strcpy(str_send[824], "E$4"); strcpy(str_send[825], "F$4"); strcpy(str_send[826], "G$4"); strcpy(str_send[827], "H$4");
    strcpy(str_send[828], "I$4"); strcpy(str_send[829], "J$4"); strcpy(str_send[830], "K$4"); strcpy(str_send[831], "L$4");
    strcpy(str_send[832], "M$4"); strcpy(str_send[833], "N$4"); strcpy(str_send[834], "O$4"); strcpy(str_send[835], "P$4");
    strcpy(str_send[836], "Q$4"); strcpy(str_send[837], "R$4"); strcpy(str_send[838], "S$4"); strcpy(str_send[839], "T$4");
    strcpy(str_send[840], "U$4"); strcpy(str_send[841], "V$4"); strcpy(str_send[842], "W$4"); strcpy(str_send[843], "X$4");
    strcpy(str_send[844], "Y$4"); strcpy(str_send[845], "Z$4"); strcpy(str_send[846], "a$4"); strcpy(str_send[847], "b$4");
    strcpy(str_send[848], "c$4"); strcpy(str_send[849], "d$4"); strcpy(str_send[850], "e$4"); strcpy(str_send[851], "f$4");
    strcpy(str_send[852], "g$4"); strcpy(str_send[853], "h$4"); strcpy(str_send[854], "i$4"); strcpy(str_send[855], "j$4");
    strcpy(str_send[856], "k$4"); strcpy(str_send[857], "l$4"); strcpy(str_send[858], "m$4"); strcpy(str_send[859], "n$4");
    strcpy(str_send[860], "o$4"); strcpy(str_send[861], "p$4"); strcpy(str_send[862], "q$4"); strcpy(str_send[863], "r$4");
    strcpy(str_send[864], "s$4"); strcpy(str_send[865], "t$4"); strcpy(str_send[866], "u$4"); strcpy(str_send[867], "v$4");
    strcpy(str_send[868], "w$4"); strcpy(str_send[869], "x$4"); strcpy(str_send[870], "y$4"); strcpy(str_send[871], "z$4");
    strcpy(str_send[872], "A%4"); strcpy(str_send[873], "B%4"); strcpy(str_send[874], "C%4"); strcpy(str_send[875], "D%4"); 
    strcpy(str_send[876], "E%4"); strcpy(str_send[877], "F%4"); strcpy(str_send[878], "G%4"); strcpy(str_send[879], "H%4");
    strcpy(str_send[880], "I%4"); strcpy(str_send[881], "J%4"); strcpy(str_send[882], "K%4"); strcpy(str_send[883], "L%4");
    strcpy(str_send[884], "M%4"); strcpy(str_send[885], "N%4"); strcpy(str_send[886], "O%4"); strcpy(str_send[887], "P%4");
    strcpy(str_send[888], "Q%4"); strcpy(str_send[889], "R%4"); strcpy(str_send[890], "S%4"); strcpy(str_send[891], "T%4");
    strcpy(str_send[892], "U%4"); strcpy(str_send[893], "V%4"); strcpy(str_send[894], "W%4"); strcpy(str_send[895], "X%4");
    strcpy(str_send[896], "Y%4"); strcpy(str_send[897], "Z%4"); strcpy(str_send[898], "a%4"); strcpy(str_send[899], "b%4");
    strcpy(str_send[900], "c%4"); strcpy(str_send[901], "d%4"); strcpy(str_send[902], "e%4"); strcpy(str_send[903], "f%4");
    strcpy(str_send[904], "g%4"); strcpy(str_send[905], "h%4"); strcpy(str_send[906], "i%4"); strcpy(str_send[907], "j%4");
    strcpy(str_send[908], "k%4"); strcpy(str_send[909], "l%4"); strcpy(str_send[910], "m%4"); strcpy(str_send[911], "n%4");
    strcpy(str_send[912], "o%4"); strcpy(str_send[913], "p%4"); strcpy(str_send[914], "q%4"); strcpy(str_send[915], "r%4");
    strcpy(str_send[916], "s%4"); strcpy(str_send[917], "t%4"); strcpy(str_send[918], "u%4"); strcpy(str_send[919], "v%4");
    strcpy(str_send[920], "w%4"); strcpy(str_send[921], "x%4"); strcpy(str_send[922], "y%4"); strcpy(str_send[923], "z%4");
    strcpy(str_send[924], "A&4"); strcpy(str_send[925], "B&4"); strcpy(str_send[926], "C&4"); strcpy(str_send[927], "D&4");
    strcpy(str_send[928], "E&4"); strcpy(str_send[929], "F&4"); strcpy(str_send[930], "G&4"); strcpy(str_send[931], "H&4");
    strcpy(str_send[932], "I&4"); strcpy(str_send[933], "J&4"); strcpy(str_send[934], "K&4"); strcpy(str_send[935], "L&4");
    strcpy(str_send[936], "M&4"); strcpy(str_send[937], "N&4"); strcpy(str_send[938], "O&4"); strcpy(str_send[939], "P&4");
    strcpy(str_send[940], "Q&4"); strcpy(str_send[941], "R&4"); strcpy(str_send[942], "S&4"); strcpy(str_send[943], "T&4");
    strcpy(str_send[944], "U&4"); strcpy(str_send[945], "V&4"); strcpy(str_send[946], "W&4"); strcpy(str_send[947], "X&4"); 
    strcpy(str_send[948], "Y&4"); strcpy(str_send[949], "Z&4"); strcpy(str_send[950], "a&4"); strcpy(str_send[951], "b&4");
    strcpy(str_send[952], "c&4"); strcpy(str_send[953], "d&4"); strcpy(str_send[954], "e&4"); strcpy(str_send[955], "f&4");
    strcpy(str_send[956], "g&4"); strcpy(str_send[957], "h&4"); strcpy(str_send[958], "i&4"); strcpy(str_send[959], "j&4");
    strcpy(str_send[960], "k&4"); strcpy(str_send[961], "l&4"); strcpy(str_send[962], "m&4"); strcpy(str_send[963], "n&4");
    strcpy(str_send[964], "o&4"); strcpy(str_send[965], "p&4"); strcpy(str_send[966], "q&4"); strcpy(str_send[967], "r&4");
    strcpy(str_send[968], "s&4"); strcpy(str_send[969], "t&4"); strcpy(str_send[970], "u&4"); strcpy(str_send[971], "v&4");
    strcpy(str_send[972], "w&4"); strcpy(str_send[973], "x&4"); strcpy(str_send[974], "y&4"); strcpy(str_send[975], "z&4");
    strcpy(str_send[976], "A'4"); strcpy(str_send[977], "B'4"); strcpy(str_send[978], "C'4"); strcpy(str_send[979], "D'4");
    strcpy(str_send[980], "E'4"); strcpy(str_send[981], "F'4"); strcpy(str_send[982], "G'4"); strcpy(str_send[983], "H'4");
    strcpy(str_send[984], "I'4"); strcpy(str_send[985], "J'4"); strcpy(str_send[986], "K'4"); strcpy(str_send[987], "L'4");
    strcpy(str_send[988], "M'4"); strcpy(str_send[989], "N'4"); strcpy(str_send[990], "O'4"); strcpy(str_send[991], "P'4");
    strcpy(str_send[992], "Q'4"); strcpy(str_send[993], "R'4"); strcpy(str_send[994], "S'4"); strcpy(str_send[995], "T'4");
    strcpy(str_send[996], "U'4"); strcpy(str_send[997], "V'4"); strcpy(str_send[998], "W'4"); strcpy(str_send[999], "X'4");
    strcpy(str_send[1000], "Y'4"); strcpy(str_send[1001], "Z'4"); strcpy(str_send[1002], "a'4"); strcpy(str_send[1003], "b'4");
    strcpy(str_send[1004], "c'4"); strcpy(str_send[1005], "d'4"); strcpy(str_send[1006], "e'4"); strcpy(str_send[1007], "f'4");
    strcpy(str_send[1008], "g'4"); strcpy(str_send[1009], "h'4"); strcpy(str_send[1010], "i'4"); strcpy(str_send[1011], "j'4");
    strcpy(str_send[1012], "k'4"); strcpy(str_send[1013], "l'4"); strcpy(str_send[1014], "m'4"); strcpy(str_send[1015], "n'4");
    strcpy(str_send[1016], "o'4"); strcpy(str_send[1017], "p'4"); strcpy(str_send[1018], "q'4"); strcpy(str_send[1019], "r'4");
    strcpy(str_send[1020], "s'4"); strcpy(str_send[1021], "t'4"); strcpy(str_send[1022], "u'4"); strcpy(str_send[1023], "v'4");
    strcpy(str_send[1024], "A#5"); strcpy(str_send[1025], "B#5"); strcpy(str_send[1026], "C#5"); strcpy(str_send[1027], "D#5");
    strcpy(str_send[1028], "E#5"); strcpy(str_send[1029], "F#5"); strcpy(str_send[1030], "G#5"); strcpy(str_send[1031], "H#5");
    strcpy(str_send[1032], "I#5"); strcpy(str_send[1033], "J#5"); strcpy(str_send[1034], "K#5"); strcpy(str_send[1035], "L#5");
    strcpy(str_send[1036], "M#5"); strcpy(str_send[1037], "N#5"); strcpy(str_send[1038], "O#5"); strcpy(str_send[1039], "P#5");
    strcpy(str_send[1040], "Q#5"); strcpy(str_send[1041], "R#5"); strcpy(str_send[1042], "S#5"); strcpy(str_send[1043], "T#5");
    strcpy(str_send[1044], "U#5"); strcpy(str_send[1045], "V#5"); strcpy(str_send[1046], "W#5"); strcpy(str_send[1047], "X#5");
    strcpy(str_send[1048], "Y#5"); strcpy(str_send[1049], "Z#5"); strcpy(str_send[1050], "a#5"); strcpy(str_send[1051], "b#5");
    strcpy(str_send[1052], "c#5"); strcpy(str_send[1053], "d#5"); strcpy(str_send[1054], "e#5"); strcpy(str_send[1055], "f#5");
    strcpy(str_send[1056], "g#5"); strcpy(str_send[1057], "h#5"); strcpy(str_send[1058], "i#5"); strcpy(str_send[1059], "j#5");
    strcpy(str_send[1060], "k#5"); strcpy(str_send[1061], "l#5"); strcpy(str_send[1062], "m#5"); strcpy(str_send[1063], "n#5");
    strcpy(str_send[1064], "o#5"); strcpy(str_send[1065], "p#5"); strcpy(str_send[1066], "q#5"); strcpy(str_send[1067], "r#5");
    strcpy(str_send[1068], "s#5"); strcpy(str_send[1069], "t#5"); strcpy(str_send[1070], "u#5"); strcpy(str_send[1071], "v#5");
    strcpy(str_send[1072], "w#5"); strcpy(str_send[1073], "x#5"); strcpy(str_send[1074], "y#5"); strcpy(str_send[1075], "z#5");
    strcpy(str_send[1076], "A$5"); strcpy(str_send[1077], "B$5"); strcpy(str_send[1078], "C$5"); strcpy(str_send[1079], "D$5");
    strcpy(str_send[1080], "E$5"); strcpy(str_send[1081], "F$5"); strcpy(str_send[1082], "G$5"); strcpy(str_send[1083], "H$5");
    strcpy(str_send[1084], "I$5"); strcpy(str_send[1085], "J$5"); strcpy(str_send[1086], "K$5"); strcpy(str_send[1087], "L$5");
    strcpy(str_send[1088], "M$5"); strcpy(str_send[1089], "N$5"); strcpy(str_send[1090], "O$5"); strcpy(str_send[1091], "P$5");
    strcpy(str_send[1092], "Q$5"); strcpy(str_send[1093], "R$5"); strcpy(str_send[1094], "S$5"); strcpy(str_send[1095], "T$5");
    strcpy(str_send[1096], "U$5"); strcpy(str_send[1097], "V$5"); strcpy(str_send[1098], "W$5"); strcpy(str_send[1099], "X$5");
    strcpy(str_send[1100], "Y$5"); strcpy(str_send[1101], "Z$5"); strcpy(str_send[1102], "a$5"); strcpy(str_send[1103], "b$5");
    strcpy(str_send[1104], "c$5"); strcpy(str_send[1105], "d$5"); strcpy(str_send[1106], "e$5"); strcpy(str_send[1107], "f$5");
    strcpy(str_send[1108], "g$5"); strcpy(str_send[1109], "h$5"); strcpy(str_send[1110], "i$5"); strcpy(str_send[1111], "j$5");
    strcpy(str_send[1112], "k$5"); strcpy(str_send[1113], "l$5"); strcpy(str_send[1114], "m$5"); strcpy(str_send[1115], "n$5");
    strcpy(str_send[1116], "o$5"); strcpy(str_send[1117], "p$5"); strcpy(str_send[1118], "q$5"); strcpy(str_send[1119], "r$5");
    strcpy(str_send[1120], "s$5"); strcpy(str_send[1121], "t$5"); strcpy(str_send[1122], "u$5"); strcpy(str_send[1123], "v$5");
    strcpy(str_send[1124], "w$5"); strcpy(str_send[1125], "x$5"); strcpy(str_send[1126], "y$5"); strcpy(str_send[1127], "z$5");
    strcpy(str_send[1128], "A%5"); strcpy(str_send[1129], "B%5"); strcpy(str_send[1130], "C%5"); strcpy(str_send[1131], "D%5");
    strcpy(str_send[1132], "E%5"); strcpy(str_send[1133], "F%5"); strcpy(str_send[1134], "G%5"); strcpy(str_send[1135], "H%5");
    strcpy(str_send[1136], "I%5"); strcpy(str_send[1137], "J%5"); strcpy(str_send[1138], "K%5"); strcpy(str_send[1139], "L%5");
    strcpy(str_send[1140], "M%5"); strcpy(str_send[1141], "N%5"); strcpy(str_send[1142], "O%5"); strcpy(str_send[1143], "P%5");
    strcpy(str_send[1144], "Q%5"); strcpy(str_send[1145], "R%5"); strcpy(str_send[1146], "S%5"); strcpy(str_send[1147], "T%5");
    strcpy(str_send[1148], "U%5"); strcpy(str_send[1149], "V%5"); strcpy(str_send[1150], "W%5"); strcpy(str_send[1151], "X%5");
    strcpy(str_send[1152], "Y%5"); strcpy(str_send[1153], "Z%5"); strcpy(str_send[1154], "a%5"); strcpy(str_send[1155], "b%5");
    strcpy(str_send[1156], "c%5"); strcpy(str_send[1157], "d%5"); strcpy(str_send[1158], "e%5"); strcpy(str_send[1159], "f%5");
    strcpy(str_send[1160], "g%5"); strcpy(str_send[1161], "h%5"); strcpy(str_send[1162], "i%5"); strcpy(str_send[1163], "j%5");
    strcpy(str_send[1164], "k%5"); strcpy(str_send[1165], "l%5"); strcpy(str_send[1166], "m%5"); strcpy(str_send[1167], "n%5");
    strcpy(str_send[1168], "o%5"); strcpy(str_send[1169], "p%5"); strcpy(str_send[1170], "q%5"); strcpy(str_send[1171], "r%5");
    strcpy(str_send[1172], "s%5"); strcpy(str_send[1173], "t%5"); strcpy(str_send[1174], "u%5"); strcpy(str_send[1175], "v%5");
    strcpy(str_send[1176], "w%5"); strcpy(str_send[1177], "x%5"); strcpy(str_send[1178], "y%5"); strcpy(str_send[1179], "z%5");
    strcpy(str_send[1180], "A&5"); strcpy(str_send[1181], "B&5"); strcpy(str_send[1182], "C&5"); strcpy(str_send[1183], "D&5");
    strcpy(str_send[1184], "E&5"); strcpy(str_send[1185], "F&5"); strcpy(str_send[1186], "G&5"); strcpy(str_send[1187], "H&5");
    strcpy(str_send[1188], "I&5"); strcpy(str_send[1189], "J&5"); strcpy(str_send[1190], "K&5"); strcpy(str_send[1191], "L&5");
    strcpy(str_send[1192], "M&5"); strcpy(str_send[1193], "N&5"); strcpy(str_send[1194], "O&5"); strcpy(str_send[1195], "P&5");
    strcpy(str_send[1196], "Q&5"); strcpy(str_send[1197], "R&5"); strcpy(str_send[1198], "S&5"); strcpy(str_send[1199], "T&5");
    strcpy(str_send[1200], "U&5"); strcpy(str_send[1201], "V&5"); strcpy(str_send[1202], "W&5"); strcpy(str_send[1203], "X&5");
    strcpy(str_send[1204], "Y&5"); strcpy(str_send[1205], "Z&5"); strcpy(str_send[1206], "a&5"); strcpy(str_send[1207], "b&5");
    strcpy(str_send[1208], "c&5"); strcpy(str_send[1209], "d&5"); strcpy(str_send[1210], "e&5"); strcpy(str_send[1211], "f&5"); 
    strcpy(str_send[1212], "g&5"); strcpy(str_send[1213], "h&5"); strcpy(str_send[1214], "i&5"); strcpy(str_send[1215], "j&5");
    strcpy(str_send[1216], "k&5"); strcpy(str_send[1217], "l&5"); strcpy(str_send[1218], "m&5"); strcpy(str_send[1219], "n&5");
    strcpy(str_send[1220], "o&5"); strcpy(str_send[1221], "p&5"); strcpy(str_send[1222], "q&5"); strcpy(str_send[1223], "r&5");
    strcpy(str_send[1224], "s&5"); strcpy(str_send[1225], "t&5"); strcpy(str_send[1226], "u&5"); strcpy(str_send[1227], "v&5");
    strcpy(str_send[1228], "w&5"); strcpy(str_send[1229], "x&5"); strcpy(str_send[1230], "y&5"); strcpy(str_send[1231], "z&5");
    strcpy(str_send[1232], "A'5"); strcpy(str_send[1233], "B'5"); strcpy(str_send[1234], "C'5"); strcpy(str_send[1235], "D'5");
    strcpy(str_send[1236], "E'5"); strcpy(str_send[1237], "F'5"); strcpy(str_send[1238], "G'5"); strcpy(str_send[1239], "H'5");
    strcpy(str_send[1240], "I'5"); strcpy(str_send[1241], "J'5"); strcpy(str_send[1242], "K'5"); strcpy(str_send[1243], "L'5");
    strcpy(str_send[1244], "M'5"); strcpy(str_send[1245], "N'5"); strcpy(str_send[1246], "O'5"); strcpy(str_send[1247], "P'5");
    strcpy(str_send[1248], "Q'5"); strcpy(str_send[1249], "R'5"); strcpy(str_send[1250], "S'5"); strcpy(str_send[1251], "T'5");
    strcpy(str_send[1252], "U'5"); strcpy(str_send[1253], "V'5"); strcpy(str_send[1254], "W'5"); strcpy(str_send[1255], "X'5"); 
    strcpy(str_send[1256], "Y'5"); strcpy(str_send[1257], "Z'5"); strcpy(str_send[1258], "a'5"); strcpy(str_send[1259], "b'5");
    strcpy(str_send[1260], "c'5"); strcpy(str_send[1261], "d'5"); strcpy(str_send[1262], "e'5"); strcpy(str_send[1263], "f'5");
    strcpy(str_send[1264], "g'5"); strcpy(str_send[1265], "h'5"); strcpy(str_send[1266], "i'5"); strcpy(str_send[1267], "j'5");
    strcpy(str_send[1268], "k'5"); strcpy(str_send[1269], "l'5"); strcpy(str_send[1270], "m'5"); strcpy(str_send[1271], "n'5");
    strcpy(str_send[1272], "o'5"); strcpy(str_send[1273], "p'5"); strcpy(str_send[1274], "q'5"); strcpy(str_send[1275], "r'5");
    strcpy(str_send[1276], "s'5"); strcpy(str_send[1277], "t'5"); strcpy(str_send[1278], "u'5"); strcpy(str_send[1279], "v'5");
    strcpy(str_send[1280], "A#6"); strcpy(str_send[1281], "B#6"); strcpy(str_send[1282], "C#6"); strcpy(str_send[1283], "D#6");
    strcpy(str_send[1284], "E#6"); strcpy(str_send[1285], "F#6"); strcpy(str_send[1286], "G#6"); strcpy(str_send[1287], "H#6");
    strcpy(str_send[1288], "I#6"); strcpy(str_send[1289], "J#6"); strcpy(str_send[1290], "K#6"); strcpy(str_send[1291], "L#6");
    strcpy(str_send[1292], "M#6"); strcpy(str_send[1293], "N#6"); strcpy(str_send[1294], "O#6"); strcpy(str_send[1295], "P#6");
    strcpy(str_send[1296], "Q#6"); strcpy(str_send[1297], "R#6"); strcpy(str_send[1298], "S#6"); strcpy(str_send[1299], "T#6");
    strcpy(str_send[1300], "U#6"); strcpy(str_send[1301], "V#6"); strcpy(str_send[1302], "W#6"); strcpy(str_send[1303], "X#6");
    strcpy(str_send[1304], "Y#6"); strcpy(str_send[1305], "Z#6"); strcpy(str_send[1306], "a#6"); strcpy(str_send[1307], "b#6");
    strcpy(str_send[1308], "c#6"); strcpy(str_send[1309], "d#6"); strcpy(str_send[1310], "e#6"); strcpy(str_send[1311], "f#6");
    strcpy(str_send[1312], "g#6"); strcpy(str_send[1313], "h#6"); strcpy(str_send[1314], "i#6"); strcpy(str_send[1315], "j#6");
    strcpy(str_send[1316], "k#6"); strcpy(str_send[1317], "l#6"); strcpy(str_send[1318], "m#6"); strcpy(str_send[1319], "n#6");
    strcpy(str_send[1320], "o#6"); strcpy(str_send[1321], "p#6"); strcpy(str_send[1322], "q#6"); strcpy(str_send[1323], "r#6");
    strcpy(str_send[1324], "s#6"); strcpy(str_send[1325], "t#6"); strcpy(str_send[1326], "u#6"); strcpy(str_send[1327], "v#6");
    strcpy(str_send[1328], "w#6"); strcpy(str_send[1329], "x#6"); strcpy(str_send[1330], "y#6"); strcpy(str_send[1331], "z#6");
    strcpy(str_send[1332], "A$6"); strcpy(str_send[1333], "B$6"); strcpy(str_send[1334], "C$6"); strcpy(str_send[1335], "D$6");
    strcpy(str_send[1336], "E$6"); strcpy(str_send[1337], "F$6"); strcpy(str_send[1338], "G$6"); strcpy(str_send[1339], "H$6");
    strcpy(str_send[1340], "I$6"); strcpy(str_send[1341], "J$6"); strcpy(str_send[1342], "K$6"); strcpy(str_send[1343], "L$6");
    strcpy(str_send[1344], "M$6"); strcpy(str_send[1345], "N$6"); strcpy(str_send[1346], "O$6"); strcpy(str_send[1347], "P$6");
    strcpy(str_send[1348], "Q$6"); strcpy(str_send[1349], "R$6"); strcpy(str_send[1350], "S$6"); strcpy(str_send[1351], "T$6");
    strcpy(str_send[1352], "U$6"); strcpy(str_send[1353], "V$6"); strcpy(str_send[1354], "W$6"); strcpy(str_send[1355], "X$6");
    strcpy(str_send[1356], "Y$6"); strcpy(str_send[1357], "Z$6"); strcpy(str_send[1358], "a$6"); strcpy(str_send[1359], "b$6");
    strcpy(str_send[1360], "c$6"); strcpy(str_send[1361], "d$6"); strcpy(str_send[1362], "e$6"); strcpy(str_send[1363], "f$6");
    strcpy(str_send[1364], "g$6"); strcpy(str_send[1365], "h$6"); strcpy(str_send[1366], "i$6"); strcpy(str_send[1367], "j$6");
    strcpy(str_send[1368], "k$6"); strcpy(str_send[1369], "l$6"); strcpy(str_send[1370], "m$6"); strcpy(str_send[1371], "n$6");
    strcpy(str_send[1372], "o$6"); strcpy(str_send[1373], "p$6"); strcpy(str_send[1374], "q$6"); strcpy(str_send[1375], "r$6");
    strcpy(str_send[1376], "s$6"); strcpy(str_send[1377], "t$6"); strcpy(str_send[1378], "u$6"); strcpy(str_send[1379], "v$6");
    strcpy(str_send[1380], "w$6"); strcpy(str_send[1381], "x$6"); strcpy(str_send[1382], "y$6"); strcpy(str_send[1383], "z$6");
    strcpy(str_send[1384], "A%6"); strcpy(str_send[1385], "B%6"); strcpy(str_send[1386], "C%6"); strcpy(str_send[1387], "D%6");
    strcpy(str_send[1388], "E%6"); strcpy(str_send[1389], "F%6"); strcpy(str_send[1390], "G%6"); strcpy(str_send[1391], "H%6");
    strcpy(str_send[1392], "I%6"); strcpy(str_send[1393], "J%6"); strcpy(str_send[1394], "K%6"); strcpy(str_send[1395], "L%6");
    strcpy(str_send[1396], "M%6"); strcpy(str_send[1397], "N%6"); strcpy(str_send[1398], "O%6"); strcpy(str_send[1399], "P%6");
    strcpy(str_send[1400], "Q%6"); strcpy(str_send[1401], "R%6"); strcpy(str_send[1402], "S%6"); strcpy(str_send[1403], "T%6");
    strcpy(str_send[1404], "U%6"); strcpy(str_send[1405], "V%6"); strcpy(str_send[1406], "W%6"); strcpy(str_send[1407], "X%6");
    strcpy(str_send[1408], "Y%6"); strcpy(str_send[1409], "Z%6"); strcpy(str_send[1410], "a%6"); strcpy(str_send[1411], "b%6");
    strcpy(str_send[1412], "c%6"); strcpy(str_send[1413], "d%6"); strcpy(str_send[1414], "e%6"); strcpy(str_send[1415], "f%6");
    strcpy(str_send[1416], "g%6"); strcpy(str_send[1417], "h%6"); strcpy(str_send[1418], "i%6"); strcpy(str_send[1419], "j%6");
    strcpy(str_send[1420], "k%6"); strcpy(str_send[1421], "l%6"); strcpy(str_send[1422], "m%6"); strcpy(str_send[1423], "n%6");
    strcpy(str_send[1424], "o%6"); strcpy(str_send[1425], "p%6"); strcpy(str_send[1426], "q%6"); strcpy(str_send[1427], "r%6");
    strcpy(str_send[1428], "s%6"); strcpy(str_send[1429], "t%6"); strcpy(str_send[1430], "u%6"); strcpy(str_send[1431], "v%6");
    strcpy(str_send[1432], "w%6"); strcpy(str_send[1433], "x%6"); strcpy(str_send[1434], "y%6"); strcpy(str_send[1435], "z%6");
    strcpy(str_send[1436], "A&6"); strcpy(str_send[1437], "B&6"); strcpy(str_send[1438], "C&6"); strcpy(str_send[1439], "D&6");
    strcpy(str_send[1440], "E&6"); strcpy(str_send[1441], "F&6"); strcpy(str_send[1442], "G&6"); strcpy(str_send[1443], "H&6");
    strcpy(str_send[1444], "I&6"); strcpy(str_send[1445], "J&6"); strcpy(str_send[1446], "K&6"); strcpy(str_send[1447], "L&6");
    strcpy(str_send[1448], "M&6"); strcpy(str_send[1449], "N&6"); strcpy(str_send[1450], "O&6"); strcpy(str_send[1451], "P&6");
    strcpy(str_send[1452], "Q&6"); strcpy(str_send[1453], "R&6"); strcpy(str_send[1454], "S&6"); strcpy(str_send[1455], "T&6");
    strcpy(str_send[1456], "U&6"); strcpy(str_send[1457], "V&6"); strcpy(str_send[1458], "W&6"); strcpy(str_send[1459], "X&6");
    strcpy(str_send[1460], "Y&6"); strcpy(str_send[1461], "Z&6"); strcpy(str_send[1462], "a&6"); strcpy(str_send[1463], "b&6");
    strcpy(str_send[1464], "c&6"); strcpy(str_send[1465], "d&6"); strcpy(str_send[1466], "e&6"); strcpy(str_send[1467], "f&6");
    strcpy(str_send[1468], "g&6"); strcpy(str_send[1469], "h&6"); strcpy(str_send[1470], "i&6"); strcpy(str_send[1471], "j&6");
    strcpy(str_send[1472], "k&6"); strcpy(str_send[1473], "l&6"); strcpy(str_send[1474], "m&6"); strcpy(str_send[1475], "n&6");
    strcpy(str_send[1476], "o&6"); strcpy(str_send[1477], "p&6"); strcpy(str_send[1478], "q&6"); strcpy(str_send[1479], "r&6");
    strcpy(str_send[1480], "s&6"); strcpy(str_send[1481], "t&6"); strcpy(str_send[1482], "u&6"); strcpy(str_send[1483], "v&6");
    strcpy(str_send[1484], "w&6"); strcpy(str_send[1485], "x&6"); strcpy(str_send[1486], "y&6"); strcpy(str_send[1487], "z&6");
    strcpy(str_send[1488], "A'6"); strcpy(str_send[1489], "B'6"); strcpy(str_send[1490], "C'6"); strcpy(str_send[1491], "D'6");
    strcpy(str_send[1492], "E'6"); strcpy(str_send[1493], "F'6"); strcpy(str_send[1494], "G'6"); strcpy(str_send[1495], "H'6");
    strcpy(str_send[1496], "I'6"); strcpy(str_send[1497], "J'6"); strcpy(str_send[1498], "K'6"); strcpy(str_send[1499], "L'6");
    strcpy(str_send[1500], "M'6"); strcpy(str_send[1501], "N'6"); strcpy(str_send[1502], "O'6"); strcpy(str_send[1503], "P'6");
    strcpy(str_send[1504], "Q'6"); strcpy(str_send[1505], "R'6"); strcpy(str_send[1506], "S'6"); strcpy(str_send[1507], "T'6");
    strcpy(str_send[1508], "U'6"); strcpy(str_send[1509], "V'6"); strcpy(str_send[1510], "W'6"); strcpy(str_send[1511], "X'6");
    strcpy(str_send[1512], "Y'6"); strcpy(str_send[1513], "Z'6"); strcpy(str_send[1514], "a'6"); strcpy(str_send[1515], "b'6");
    strcpy(str_send[1516], "c'6"); strcpy(str_send[1517], "d'6"); strcpy(str_send[1518], "e'6"); strcpy(str_send[1519], "f'6");
    strcpy(str_send[1520], "g'6"); strcpy(str_send[1521], "h'6"); strcpy(str_send[1522], "i'6"); strcpy(str_send[1523], "j'6");
    strcpy(str_send[1524], "k'6"); strcpy(str_send[1525], "l'6"); strcpy(str_send[1526], "m'6"); strcpy(str_send[1527], "n'6");
    strcpy(str_send[1528], "o'6"); strcpy(str_send[1529], "p'6"); strcpy(str_send[1530], "q'6"); strcpy(str_send[1531], "r'6");
    strcpy(str_send[1532], "s'6"); strcpy(str_send[1533], "t'6"); strcpy(str_send[1534], "u'6"); strcpy(str_send[1535], "v'6");
    strcpy(str_send[1536], "A#7"); strcpy(str_send[1537], "B#7"); strcpy(str_send[1538], "C#7"); strcpy(str_send[1539], "D#7");
    strcpy(str_send[1540], "E#7"); strcpy(str_send[1541], "F#7"); strcpy(str_send[1542], "G#7"); strcpy(str_send[1543], "H#7");
    strcpy(str_send[1544], "I#7"); strcpy(str_send[1545], "J#7"); strcpy(str_send[1546], "K#7"); strcpy(str_send[1547], "L#7");
    strcpy(str_send[1548], "M#7"); strcpy(str_send[1549], "N#7"); strcpy(str_send[1550], "O#7"); strcpy(str_send[1551], "P#7");
    strcpy(str_send[1552], "Q#7"); strcpy(str_send[1553], "R#7"); strcpy(str_send[1554], "S#7"); strcpy(str_send[1555], "T#7");
    strcpy(str_send[1556], "U#7"); strcpy(str_send[1557], "V#7"); strcpy(str_send[1558], "W#7"); strcpy(str_send[1559], "X#7");
    strcpy(str_send[1560], "Y#7"); strcpy(str_send[1561], "Z#7"); strcpy(str_send[1562], "a#7"); strcpy(str_send[1563], "b#7");
    strcpy(str_send[1564], "c#7"); strcpy(str_send[1565], "d#7"); strcpy(str_send[1566], "e#7"); strcpy(str_send[1567], "f#7");
    strcpy(str_send[1568], "g#7"); strcpy(str_send[1569], "h#7"); strcpy(str_send[1570], "i#7"); strcpy(str_send[1571], "j#7");
    strcpy(str_send[1572], "k#7"); strcpy(str_send[1573], "l#7"); strcpy(str_send[1574], "m#7"); strcpy(str_send[1575], "n#7");
    strcpy(str_send[1576], "o#7"); strcpy(str_send[1577], "p#7"); strcpy(str_send[1578], "q#7"); strcpy(str_send[1579], "r#7");
    strcpy(str_send[1580], "s#7"); strcpy(str_send[1581], "t#7"); strcpy(str_send[1582], "u#7"); strcpy(str_send[1583], "v#7");
    strcpy(str_send[1584], "w#7"); strcpy(str_send[1585], "x#7"); strcpy(str_send[1586], "y#7"); strcpy(str_send[1587], "z#7");
    strcpy(str_send[1588], "A$7"); strcpy(str_send[1589], "B$7"); strcpy(str_send[1590], "C$7"); strcpy(str_send[1591], "D$7");
    strcpy(str_send[1592], "E$7"); strcpy(str_send[1593], "F$7"); strcpy(str_send[1594], "G$7"); strcpy(str_send[1595], "H$7");
    strcpy(str_send[1596], "I$7"); strcpy(str_send[1597], "J$7"); strcpy(str_send[1598], "K$7"); strcpy(str_send[1599], "L$7");
    strcpy(str_send[1600], "M$7"); strcpy(str_send[1601], "N$7"); strcpy(str_send[1602], "O$7"); strcpy(str_send[1603], "P$7");
    strcpy(str_send[1604], "Q$7"); strcpy(str_send[1605], "R$7"); strcpy(str_send[1606], "S$7"); strcpy(str_send[1607], "T$7");
    strcpy(str_send[1608], "U$7"); strcpy(str_send[1609], "V$7"); strcpy(str_send[1610], "W$7"); strcpy(str_send[1611], "X$7");
    strcpy(str_send[1612], "Y$7"); strcpy(str_send[1613], "Z$7"); strcpy(str_send[1614], "a$7"); strcpy(str_send[1615], "b$7");
    strcpy(str_send[1616], "c$7"); strcpy(str_send[1617], "d$7"); strcpy(str_send[1618], "e$7"); strcpy(str_send[1619], "f$7");
    strcpy(str_send[1620], "g$7"); strcpy(str_send[1621], "h$7"); strcpy(str_send[1622], "i$7"); strcpy(str_send[1623], "j$7");
    strcpy(str_send[1624], "k$7"); strcpy(str_send[1625], "l$7"); strcpy(str_send[1626], "m$7"); strcpy(str_send[1627], "n$7");
    strcpy(str_send[1628], "o$7"); strcpy(str_send[1629], "p$7"); strcpy(str_send[1630], "q$7"); strcpy(str_send[1631], "r$7");
    strcpy(str_send[1632], "s$7"); strcpy(str_send[1633], "t$7"); strcpy(str_send[1634], "u$7"); strcpy(str_send[1635], "v$7");
    strcpy(str_send[1636], "w$7"); strcpy(str_send[1637], "x$7"); strcpy(str_send[1638], "y$7"); strcpy(str_send[1639], "z$7");
    strcpy(str_send[1640], "A%7"); strcpy(str_send[1641], "B%7"); strcpy(str_send[1642], "C%7"); strcpy(str_send[1643], "D%7");
    strcpy(str_send[1644], "E%7"); strcpy(str_send[1645], "F%7"); strcpy(str_send[1646], "G%7"); strcpy(str_send[1647], "H%7");
    strcpy(str_send[1648], "I%7"); strcpy(str_send[1649], "J%7"); strcpy(str_send[1650], "K%7"); strcpy(str_send[1651], "L%7");
    strcpy(str_send[1652], "M%7"); strcpy(str_send[1653], "N%7"); strcpy(str_send[1654], "O%7"); strcpy(str_send[1655], "P%7");
    strcpy(str_send[1656], "Q%7"); strcpy(str_send[1657], "R%7"); strcpy(str_send[1658], "S%7"); strcpy(str_send[1659], "T%7");
    strcpy(str_send[1660], "U%7"); strcpy(str_send[1661], "V%7"); strcpy(str_send[1662], "W%7"); strcpy(str_send[1663], "X%7");
    strcpy(str_send[1664], "Y%7"); strcpy(str_send[1665], "Z%7"); strcpy(str_send[1666], "a%7"); strcpy(str_send[1667], "b%7");
    strcpy(str_send[1668], "c%7"); strcpy(str_send[1669], "d%7"); strcpy(str_send[1670], "e%7"); strcpy(str_send[1671], "f%7");
    strcpy(str_send[1672], "g%7"); strcpy(str_send[1673], "h%7"); strcpy(str_send[1674], "i%7"); strcpy(str_send[1675], "j%7");
    strcpy(str_send[1676], "k%7"); strcpy(str_send[1677], "l%7"); strcpy(str_send[1678], "m%7"); strcpy(str_send[1679], "n%7");
    strcpy(str_send[1680], "o%7"); strcpy(str_send[1681], "p%7"); strcpy(str_send[1682], "q%7"); strcpy(str_send[1683], "r%7");
    strcpy(str_send[1684], "s%7"); strcpy(str_send[1685], "t%7"); strcpy(str_send[1686], "u%7"); strcpy(str_send[1687], "v%7");
    strcpy(str_send[1688], "w%7"); strcpy(str_send[1689], "x%7"); strcpy(str_send[1690], "y%7"); strcpy(str_send[1691], "z%7");
    strcpy(str_send[1692], "A&7"); strcpy(str_send[1693], "B&7"); strcpy(str_send[1694], "C&7"); strcpy(str_send[1695], "D&7");
    strcpy(str_send[1696], "E&7"); strcpy(str_send[1697], "F&7"); strcpy(str_send[1698], "G&7"); strcpy(str_send[1699], "H&7");
    strcpy(str_send[1700], "I&7"); strcpy(str_send[1701], "J&7"); strcpy(str_send[1702], "K&7"); strcpy(str_send[1703], "L&7");
    strcpy(str_send[1704], "M&7"); strcpy(str_send[1705], "N&7"); strcpy(str_send[1706], "O&7"); strcpy(str_send[1707], "P&7");
    strcpy(str_send[1708], "Q&7"); strcpy(str_send[1709], "R&7"); strcpy(str_send[1710], "S&7"); strcpy(str_send[1711], "T&7");
    strcpy(str_send[1712], "U&7"); strcpy(str_send[1713], "V&7"); strcpy(str_send[1714], "W&7"); strcpy(str_send[1715], "X&7");
    strcpy(str_send[1716], "Y&7"); strcpy(str_send[1717], "Z&7"); strcpy(str_send[1718], "a&7"); strcpy(str_send[1719], "b&7");
    strcpy(str_send[1720], "c&7"); strcpy(str_send[1721], "d&7"); strcpy(str_send[1722], "e&7"); strcpy(str_send[1723], "f&7");
    strcpy(str_send[1724], "g&7"); strcpy(str_send[1725], "h&7"); strcpy(str_send[1726], "i&7"); strcpy(str_send[1727], "j&7");
    strcpy(str_send[1728], "k&7"); strcpy(str_send[1729], "l&7"); strcpy(str_send[1730], "m&7"); strcpy(str_send[1731], "n&7");
    strcpy(str_send[1732], "o&7"); strcpy(str_send[1733], "p&7"); strcpy(str_send[1734], "q&7"); strcpy(str_send[1735], "r&7");
    strcpy(str_send[1736], "s&7"); strcpy(str_send[1737], "t&7"); strcpy(str_send[1738], "u&7"); strcpy(str_send[1739], "v&7");
    strcpy(str_send[1740], "w&7"); strcpy(str_send[1741], "x&7"); strcpy(str_send[1742], "y&7"); strcpy(str_send[1743], "z&7");
    strcpy(str_send[1744], "A'7"); strcpy(str_send[1745], "B'7"); strcpy(str_send[1746], "C'7"); strcpy(str_send[1747], "D'7");
    strcpy(str_send[1748], "E'7"); strcpy(str_send[1749], "F'7"); strcpy(str_send[1750], "G'7"); strcpy(str_send[1751], "H'7");
    strcpy(str_send[1752], "I'7"); strcpy(str_send[1753], "J'7"); strcpy(str_send[1754], "K'7"); strcpy(str_send[1755], "L'7");
    strcpy(str_send[1756], "M'7"); strcpy(str_send[1757], "N'7"); strcpy(str_send[1758], "O'7"); strcpy(str_send[1759], "P'7");
    strcpy(str_send[1760], "Q'7"); strcpy(str_send[1761], "R'7"); strcpy(str_send[1762], "S'7"); strcpy(str_send[1763], "T'7"); 
    strcpy(str_send[1764], "U'7"); strcpy(str_send[1765], "V'7"); strcpy(str_send[1766], "W'7"); strcpy(str_send[1767], "X'7");
    strcpy(str_send[1768], "Y'7"); strcpy(str_send[1769], "Z'7"); strcpy(str_send[1770], "a'7"); strcpy(str_send[1771], "b'7");
    strcpy(str_send[1772], "c'7"); strcpy(str_send[1773], "d'7"); strcpy(str_send[1774], "e'7"); strcpy(str_send[1775], "f'7");
    strcpy(str_send[1776], "g'7"); strcpy(str_send[1777], "h'7"); strcpy(str_send[1778], "i'7"); strcpy(str_send[1779], "j'7");
    strcpy(str_send[1780], "k'7"); strcpy(str_send[1781], "l'7"); strcpy(str_send[1782], "m'7"); strcpy(str_send[1783], "n'7");
    strcpy(str_send[1784], "o'7"); strcpy(str_send[1785], "p'7"); strcpy(str_send[1786], "q'7"); strcpy(str_send[1787], "r'7");
    strcpy(str_send[1788], "s'7"); strcpy(str_send[1789], "t'7"); strcpy(str_send[1790], "u'7"); strcpy(str_send[1791], "v'7");
    strcpy(str_send[1792], "A#8"); strcpy(str_send[1793], "B#8"); strcpy(str_send[1794], "C#8"); strcpy(str_send[1795], "D#8");
    strcpy(str_send[1796], "E#8"); strcpy(str_send[1797], "F#8"); strcpy(str_send[1798], "G#8"); strcpy(str_send[1799], "H#8");
    strcpy(str_send[1800], "I#8"); strcpy(str_send[1801], "J#8"); strcpy(str_send[1802], "K#8"); strcpy(str_send[1803], "L#8");
    strcpy(str_send[1804], "M#8"); strcpy(str_send[1805], "N#8"); strcpy(str_send[1806], "O#8"); strcpy(str_send[1807], "P#8");
    strcpy(str_send[1808], "Q#8"); strcpy(str_send[1809], "R#8"); strcpy(str_send[1810], "S#8"); strcpy(str_send[1811], "T#8");
    strcpy(str_send[1812], "U#8"); strcpy(str_send[1813], "V#8"); strcpy(str_send[1814], "W#8"); strcpy(str_send[1815], "X#8");
    strcpy(str_send[1816], "Y#8"); strcpy(str_send[1817], "Z#8"); strcpy(str_send[1818], "a#8"); strcpy(str_send[1819], "b#8");
    strcpy(str_send[1820], "c#8"); strcpy(str_send[1821], "d#8"); strcpy(str_send[1822], "e#8"); strcpy(str_send[1823], "f#8");
    strcpy(str_send[1824], "g#8"); strcpy(str_send[1825], "h#8"); strcpy(str_send[1826], "i#8"); strcpy(str_send[1827], "j#8");
    strcpy(str_send[1828], "k#8"); strcpy(str_send[1829], "l#8"); strcpy(str_send[1830], "m#8"); strcpy(str_send[1831], "n#8");
    strcpy(str_send[1832], "o#8"); strcpy(str_send[1833], "p#8"); strcpy(str_send[1834], "q#8"); strcpy(str_send[1835], "r#8");
    strcpy(str_send[1836], "s#8"); strcpy(str_send[1837], "t#8"); strcpy(str_send[1838], "u#8"); strcpy(str_send[1839], "v#8");
    strcpy(str_send[1840], "w#8"); strcpy(str_send[1841], "x#8"); strcpy(str_send[1842], "y#8"); strcpy(str_send[1843], "z#8");
    strcpy(str_send[1844], "A$8"); strcpy(str_send[1845], "B$8"); strcpy(str_send[1846], "C$8"); strcpy(str_send[1847], "D$8");
    strcpy(str_send[1848], "E$8"); strcpy(str_send[1849], "F$8"); strcpy(str_send[1850], "G$8"); strcpy(str_send[1851], "H$8");
    strcpy(str_send[1852], "I$8"); strcpy(str_send[1853], "J$8"); strcpy(str_send[1854], "K$8"); strcpy(str_send[1855], "L$8");
    strcpy(str_send[1856], "M$8"); strcpy(str_send[1857], "N$8"); strcpy(str_send[1858], "O$8"); strcpy(str_send[1859], "P$8");
    strcpy(str_send[1860], "Q$8"); strcpy(str_send[1861], "R$8"); strcpy(str_send[1862], "S$8"); strcpy(str_send[1863], "T$8");
    strcpy(str_send[1864], "U$8"); strcpy(str_send[1865], "V$8"); strcpy(str_send[1866], "W$8"); strcpy(str_send[1867], "X$8");
    strcpy(str_send[1868], "Y$8"); strcpy(str_send[1869], "Z$8"); strcpy(str_send[1870], "a$8"); strcpy(str_send[1871], "b$8");
    strcpy(str_send[1872], "c$8"); strcpy(str_send[1873], "d$8"); strcpy(str_send[1874], "e$8"); strcpy(str_send[1875], "f$8");
    strcpy(str_send[1876], "g$8"); strcpy(str_send[1877], "h$8"); strcpy(str_send[1878], "i$8"); strcpy(str_send[1879], "j$8");
    strcpy(str_send[1880], "k$8"); strcpy(str_send[1881], "l$8"); strcpy(str_send[1882], "m$8"); strcpy(str_send[1883], "n$8");
    strcpy(str_send[1884], "o$8"); strcpy(str_send[1885], "p$8"); strcpy(str_send[1886], "q$8"); strcpy(str_send[1887], "r$8");
    strcpy(str_send[1888], "s$8"); strcpy(str_send[1889], "t$8"); strcpy(str_send[1890], "u$8"); strcpy(str_send[1891], "v$8");
    strcpy(str_send[1892], "w$8"); strcpy(str_send[1893], "x$8"); strcpy(str_send[1894], "y$8"); strcpy(str_send[1895], "z$8");
    strcpy(str_send[1896], "A%8"); strcpy(str_send[1897], "B%8"); strcpy(str_send[1898], "C%8"); strcpy(str_send[1899], "D%8");
    strcpy(str_send[1900], "E%8"); strcpy(str_send[1901], "F%8"); strcpy(str_send[1902], "G%8"); strcpy(str_send[1903], "H%8");
    strcpy(str_send[1904], "I%8"); strcpy(str_send[1905], "J%8"); strcpy(str_send[1906], "K%8"); strcpy(str_send[1907], "L%8");
    strcpy(str_send[1908], "M%8"); strcpy(str_send[1909], "N%8"); strcpy(str_send[1910], "O%8"); strcpy(str_send[1911], "P%8");
    strcpy(str_send[1912], "Q%8"); strcpy(str_send[1913], "R%8"); strcpy(str_send[1914], "S%8"); strcpy(str_send[1915], "T%8");
    strcpy(str_send[1916], "U%8"); strcpy(str_send[1917], "V%8"); strcpy(str_send[1918], "W%8"); strcpy(str_send[1919], "X%8");
    strcpy(str_send[1920], "Y%8"); strcpy(str_send[1921], "Z%8"); strcpy(str_send[1922], "a%8"); strcpy(str_send[1923], "b%8");
    strcpy(str_send[1924], "c%8"); strcpy(str_send[1925], "d%8"); strcpy(str_send[1926], "e%8"); strcpy(str_send[1927], "f%8");
    strcpy(str_send[1928], "g%8"); strcpy(str_send[1929], "h%8"); strcpy(str_send[1930], "i%8"); strcpy(str_send[1931], "j%8");
    strcpy(str_send[1932], "k%8"); strcpy(str_send[1933], "l%8"); strcpy(str_send[1934], "m%8"); strcpy(str_send[1935], "n%8");
    strcpy(str_send[1936], "o%8"); strcpy(str_send[1937], "p%8"); strcpy(str_send[1938], "q%8"); strcpy(str_send[1939], "r%8");
    strcpy(str_send[1940], "s%8"); strcpy(str_send[1941], "t%8"); strcpy(str_send[1942], "u%8"); strcpy(str_send[1943], "v%8");
    strcpy(str_send[1944], "w%8"); strcpy(str_send[1945], "x%8"); strcpy(str_send[1946], "y%8"); strcpy(str_send[1947], "z%8");
    strcpy(str_send[1948], "A&8"); strcpy(str_send[1949], "B&8"); strcpy(str_send[1950], "C&8"); strcpy(str_send[1951], "D&8");
    strcpy(str_send[1952], "E&8"); strcpy(str_send[1953], "F&8"); strcpy(str_send[1954], "G&8"); strcpy(str_send[1955], "H&8");
    strcpy(str_send[1956], "I&8"); strcpy(str_send[1957], "J&8"); strcpy(str_send[1958], "K&8"); strcpy(str_send[1959], "L&8");
    strcpy(str_send[1960], "M&8"); strcpy(str_send[1961], "N&8"); strcpy(str_send[1962], "O&8"); strcpy(str_send[1963], "P&8");
    strcpy(str_send[1964], "Q&8"); strcpy(str_send[1965], "R&8"); strcpy(str_send[1966], "S&8"); strcpy(str_send[1967], "T&8");
    strcpy(str_send[1968], "U&8"); strcpy(str_send[1969], "V&8"); strcpy(str_send[1970], "W&8"); strcpy(str_send[1971], "X&8");
    strcpy(str_send[1972], "Y&8"); strcpy(str_send[1973], "Z&8"); strcpy(str_send[1974], "a&8"); strcpy(str_send[1975], "b&8");
    strcpy(str_send[1976], "c&8"); strcpy(str_send[1977], "d&8"); strcpy(str_send[1978], "e&8"); strcpy(str_send[1979], "f&8");
    strcpy(str_send[1980], "g&8"); strcpy(str_send[1981], "h&8"); strcpy(str_send[1982], "i&8"); strcpy(str_send[1983], "j&8");
    strcpy(str_send[1984], "k&8"); strcpy(str_send[1985], "l&8"); strcpy(str_send[1986], "m&8"); strcpy(str_send[1987], "n&8");
    strcpy(str_send[1988], "o&8"); strcpy(str_send[1989], "p&8"); strcpy(str_send[1990], "q&8"); strcpy(str_send[1991], "r&8");
    strcpy(str_send[1992], "s&8"); strcpy(str_send[1993], "t&8"); strcpy(str_send[1994], "u&8"); strcpy(str_send[1995], "v&8");
    strcpy(str_send[1996], "w&8"); strcpy(str_send[1997], "x&8"); strcpy(str_send[1998], "y&8"); strcpy(str_send[1999], "z&8");
    strcpy(str_send[2000], "A'8"); strcpy(str_send[2001], "B'8"); strcpy(str_send[2002], "C'8"); strcpy(str_send[2003], "D'8");
    strcpy(str_send[2004], "E'8"); strcpy(str_send[2005], "F'8"); strcpy(str_send[2006], "G'8"); strcpy(str_send[2007], "H'8");
    strcpy(str_send[2008], "I'8"); strcpy(str_send[2009], "J'8"); strcpy(str_send[2010], "K'8"); strcpy(str_send[2011], "L'8");
    strcpy(str_send[2012], "M'8"); strcpy(str_send[2013], "N'8"); strcpy(str_send[2014], "O'8"); strcpy(str_send[2015], "P'8");
    strcpy(str_send[2016], "Q'8"); strcpy(str_send[2017], "R'8"); strcpy(str_send[2018], "S'8"); strcpy(str_send[2019], "T'8");
    strcpy(str_send[2020], "U'8"); strcpy(str_send[2021], "V'8"); strcpy(str_send[2022], "W'8"); strcpy(str_send[2023], "X'8");
    strcpy(str_send[2024], "Y'8"); strcpy(str_send[2025], "Z'8"); strcpy(str_send[2026], "a'8"); strcpy(str_send[2027], "b'8");
    strcpy(str_send[2028], "c'8"); strcpy(str_send[2029], "d'8"); strcpy(str_send[2030], "e'8"); strcpy(str_send[2031], "f'8");
    strcpy(str_send[2032], "g'8"); strcpy(str_send[2033], "h'8"); strcpy(str_send[2034], "i'8"); strcpy(str_send[2035], "j'8");
    strcpy(str_send[2036], "k'8"); strcpy(str_send[2037], "l'8"); strcpy(str_send[2038], "m'8"); strcpy(str_send[2039], "n'8");
    strcpy(str_send[2040], "o'8"); strcpy(str_send[2041], "p'8"); strcpy(str_send[2042], "q'8"); strcpy(str_send[2043], "r'8");
    strcpy(str_send[2044], "s'8"); strcpy(str_send[2045], "t'8"); strcpy(str_send[2046], "u'8"); strcpy(str_send[2047], "v'8");
    
    if(RS232_OpenComport(cport_nr, bdrate, mode)) {
        printf("Can not open comport\n");
        return(0);
    }
    
    usleep(2000000); // Waits 2s for stable condition
    testButton();
    usleep(3000000); // Allow feathers to buzz for 3s (testing functionality)
    silenceAllFeathers();

    // Create Window
    sf::RenderWindow app(sf::VideoMode(640, 480), "LEO Belt"); // Simple window handling

    // Create a pipeline to easily configure and start the camera
    rs2::config cfg;
    cfg.enable_stream(RS2_STREAM_DEPTH, -1, 640, 480, RS2_FORMAT_ANY, 30);
    cfg.enable_stream(RS2_STREAM_COLOR, -1, 640, 480, RS2_FORMAT_ANY, 30);
    rs2::pipeline pipe;
    
    //Calling pipeline's start() without any additional parameters will start the first device
    // with its default streams.
    //The start function returns the pipeline profile which the pipeline used to start the device
    rs2::pipeline_profile profile = pipe.start(cfg);

    // Each depth camera might have different units for depth pixels, so we get it here
    // Using the pipeline's profile, we can retrieve the device that the pipeline uses
    float depth_scale = get_depth_scale(profile.get_device());

    //Pipeline could choose a device that does not have a color stream
    //If there is no color stream, choose to align depth to another stream
    rs2_stream align_to = find_stream_to_align(profile.get_streams());

    // Create a rs2::align object.
    // rs2::align allows us to perform alignment of depth frames to others frames
    // The "align_to" is the stream type to which we plan to align depth frames.
    rs2::align align(align_to);
    
    // Create objects for heatmap
    sf::Texture t1, t7;
    sf::Texture video;
    if (!t1.loadFromFile("white-square.png", sf::IntRect(0,0,214,160))) {std::cout << "could not make texture t1" << std::endl;}
    if (!t7.loadFromFile("white-square.png", sf::IntRect(0,0,320,160))) {std::cout << "could not make texture t7" << std::endl;}
    
    sf::Sprite s1(t1), s2(t1), s3(t1), s4(t1), s5(t1), s6(t1), s7(t7), s8(t7);

    int getFrame = 0;
    
    // Client-side socket
    int sock = 0, valread;
    struct sockaddr_in serv_addr;
    char buffer[1024];
    rs2::video_frame *frame;
    int *sockTest;
    int sockint = 5;
    sockTest = &sockint;
    int len;
    if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        printf("\n Socket creation error \n");
        return -1;
    }
    
    memset(&serv_addr, 0, sizeof(serv_addr));
    
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    serv_addr.sin_addr.s_addr = inet_addr(serverIP);

    while (app.isOpen()) // Application still alive?
    {        
        sf::Event e;
        while (app.pollEvent(e)) {
            if (e.type == sf::Event::Closed) {
                app.close();
            }
        }
        
        // Using the align object, we block the application until a frameset is available
        rs2::frameset frameset = pipe.wait_for_frames();

        if (profile_changed(pipe.get_active_profile().get_streams(), profile.get_streams()))
        {
            //If the profile was changed, update the align object, and also get the new device's depth scale
            profile = pipe.get_active_profile();
            align_to = find_stream_to_align(profile.get_streams());
            align = rs2::align(align_to);
            depth_scale = get_depth_scale(profile.get_device());
        }
    
        //Get processed aligned frame
        //auto processed = align.process(frameset);
    
        // Trying to get both other and aligned depth frames
        rs2::video_frame other_frame = frameset.first(align_to);
        rs2::depth_frame aligned_depth_frame = frameset.get_depth_frame();
    
        //If one of them is unavailable, continue iteration
        if (!aligned_depth_frame || !other_frame)
        {
            continue;
        }
      
        // Obtaining pixel depth and heat map specifications
        int* c;
        c = printPixelDepth(other_frame, aligned_depth_frame, depth_scale);
        
        // Taking dimensions of the window for rendering purposes
        sf::Vector2u size = app.getSize();
        float width = size.x;
        float height = size.y;
        
        // Send info over socket
        frame = &other_frame;
        sendto(sock , frame , (*frame).get_data_size() , MSG_DONTWAIT, (const struct sockaddr *) &serv_addr, sizeof(serv_addr) );
        printf("Frame sent\n");
        recvfrom(sock, (char *) buffer, 1024, MSG_DONTWAIT, (struct sockaddr *) &serv_addr, reinterpret_cast<socklen_t*>(&len));
        std::cout << "Server: " << buffer << std::endl;
        std::cout << getFrame << std::endl;
        
        /*if (getFrame%10==0) {
            // Convert frame to png and upload png to texture
            stbi_write_png("color.png", other_frame.get_width(), other_frame.get_height(),
                other_frame.get_bytes_per_pixel(), other_frame.get_data(), other_frame.get_stride_in_bytes());
            if (!video.loadFromFile("color.png")) {std::cout << "could not make video texture" << std::endl;}
            sf::Sprite view(video);
            
            // Set colors for heat map quadrants
            s1.setColor(sf::Color(*(c+0), 0, *(c+8), 120));
            s2.setColor(sf::Color(*(c+1), 0, *(c+9), 120));
            s3.setColor(sf::Color(*(c+2), 0, *(c+10), 120));
            s4.setColor(sf::Color(*(c+3), 0, *(c+11), 120));
            s5.setColor(sf::Color(*(c+4), 0, *(c+12), 120));
            s6.setColor(sf::Color(*(c+5), 0, *(c+13), 120));
            s7.setColor(sf::Color(*(c+6), 0, *(c+14), 120));
            s8.setColor(sf::Color(*(c+7), 0, *(c+15), 120));
        
            // Creating the heat map
            s1.setPosition(0,0);
            s2.setPosition(width/3,0);
            s3.setPosition(2*width/3,0);
            s4.setPosition(0,height/3);
            s5.setPosition(width/3,height/3);
            s6.setPosition(2*width/3,height/3);
            s7.setPosition(0,2*height/3);
            s8.setPosition(width/2,2*height/3);
        
            app.clear();
            
                app.draw(view);
                app.draw(s1);
                app.draw(s2);
                app.draw(s3);
                app.draw(s4);
                app.draw(s5);
                app.draw(s6);
                app.draw(s7);
                app.draw(s8);
            
            app.display();
        }
        else {
            if (!video.loadFromFile("color.png")) {std::cout << "could not make video texture" << std::endl;}
            sf::Sprite view(video);
            
            // Set colors for heat map quadrants
            s1.setColor(sf::Color(*(c+0), 0, *(c+8), 120));
            s2.setColor(sf::Color(*(c+1), 0, *(c+9), 120));
            s3.setColor(sf::Color(*(c+2), 0, *(c+10), 120));
            s4.setColor(sf::Color(*(c+3), 0, *(c+11), 120));
            s5.setColor(sf::Color(*(c+4), 0, *(c+12), 120));
            s6.setColor(sf::Color(*(c+5), 0, *(c+13), 120));
            s7.setColor(sf::Color(*(c+6), 0, *(c+14), 120));
            s8.setColor(sf::Color(*(c+7), 0, *(c+15), 120));
        
            // Creating the heat map
            s1.setPosition(0,0);
            s2.setPosition(width/3,0);
            s3.setPosition(2*width/3,0);
            s4.setPosition(0,height/3);
            s5.setPosition(width/3,height/3);
            s6.setPosition(2*width/3,height/3);
            s7.setPosition(0,2*height/3);
            s8.setPosition(width/2,2*height/3);
        
            app.clear();
            
                app.draw(view);
                app.draw(s1);
                app.draw(s2);
                app.draw(s3);
                app.draw(s4);
                app.draw(s5);
                app.draw(s6);
                app.draw(s7);
                app.draw(s8);
            
            app.display();
        }*/
        getFrame++;
    }
    std::cout << getFrame << std::endl;
    silenceAllFeathers();
    return EXIT_SUCCESS;
}

catch (const rs2::error & e)
{
    std::cerr << "RealSense error calling " << e.get_failed_function() << "(" << e.get_failed_args() << "):\n    " << e.what() << std::endl;
    return EXIT_FAILURE;
}
catch (const std::exception & e)
{
    std::cerr << e.what() << std::endl;
    return EXIT_FAILURE;
}

void silenceAllFeathers(){
    RS232_cputs(cport_nr, str_send[0]);
    RS232_cputs(cport_nr, str_send[256]);
    RS232_cputs(cport_nr, str_send[2*256]);
    RS232_cputs(cport_nr, str_send[3*256]);
    RS232_cputs(cport_nr, str_send[4*256]);
    RS232_cputs(cport_nr, str_send[5*256]);
    RS232_cputs(cport_nr, str_send[6*256]);
    RS232_cputs(cport_nr, str_send[7*256]);
}

int* printPixelDepth(rs2::video_frame& other_frame, const rs2::depth_frame& depth_frame, float depth_scale) {

    const uint16_t* p_depth_frame = reinterpret_cast<const uint16_t*>(depth_frame.get_data());
    
    uint8_t* p_other_frame = reinterpret_cast<uint8_t*>(const_cast<void*>(other_frame.get_data()));

    int width = other_frame.get_width();
    int height = other_frame.get_height();
    int other_bpp = other_frame.get_bytes_per_pixel();

    float closest[8] = {10,10,10,10,10,10,10,10};
    static int color[16] = {};
    
    int x_left = 96.97;
    int x_right = 135.76;
    int y_offset = 78.37;

#pragma omp parallel for schedule(dynamic) //Using OpenMP to try to parallelise the loop
    for (int y = 0; y < height; y++)
    {
        auto depth_pixel_index = y * width;
        for (int x = 0; x < width; x++, ++depth_pixel_index)
        {
            // Get the depth value of the current pixel
            auto pixels_distance = depth_scale * p_depth_frame[depth_pixel_index];
            
            auto offset = depth_pixel_index*other_bpp;
            
            if ((x>x_left) && (y>y_offset) && (x<(width-x_left-x_right)/3+x_left) && (y<(height-2*y_offset)/3+y_offset)) { // Top-left quadrant -- Feather 1
                if (pixels_distance < closest[0] && pixels_distance > .001){
                    closest[0] = pixels_distance;
                }
            } else if ((x>(width-x_left-x_right)/3+x_left) && (y>y_offset) && (x<2*(width-x_left-x_right)/3+x_left) && (y<(height-2*y_offset)/3+y_offset)) { // Top-middle quadrant -- Feather 2
                if (pixels_distance < closest[1] && pixels_distance > .001){
                    closest[1] = pixels_distance;
                }
            } else if ((x>2*(width-x_left-x_right)/3+x_left) && (y>y_offset) && (x<width-x_right) && (y<(height-2*y_offset)/3+y_offset)) { // Top-right quadrant -- Feather 3
                if (pixels_distance < closest[2] && pixels_distance > .001){
                    closest[2] = pixels_distance;
                }
            } else if ((x>x_left) && (y>(height-2*y_offset)/3+y_offset) && (x<(width-x_left-x_right)/3+x_left) && (y<2*(height-2*y_offset)/3+y_offset)) { // Middle-left quadrant -- Feather 4
                if (pixels_distance < closest[3] && pixels_distance > .001){
                    closest[3] = pixels_distance;
                }
            } else if ((x>(width-x_left-x_right)/3+x_left) && (y>(height-2*y_offset)/3+y_offset) && (x<2*(width-x_left-x_right)/3+x_left) && (y<2*(height-2*y_offset)/3+y_offset)) { // Middle-middle quadrant -- Feather 5
                if (pixels_distance < closest[4] && pixels_distance > .001){
                    closest[4] = pixels_distance;
                }
            } else if ((x>2*(width-x_left-x_right)/3+x_left) && (y>(height-2*y_offset)/3+y_offset) && (x<width-x_right) && (y<2*(height-2*y_offset)/3+y_offset)) { // Middle-right quadrant -- Feather 6
                if (pixels_distance < closest[5] && pixels_distance > .001){
                    closest[5] = pixels_distance;
                }
            } else if ((x>x_left) && (y>2*(height-2*y_offset)/3+y_offset) && (x<(width-x_left-x_right)/2+x_left) && (y<height-y_offset)) { // Bottom-left quadrant -- Feather 7 
                if (pixels_distance < closest[6] && pixels_distance > .001){
                    closest[6] = pixels_distance;
                }
            } else if ((x>(width-x_left-x_right)/2+x_left) && (y>2*(height-2*y_offset)/3+y_offset) && (x<width-x_right) && (y<height-y_offset)) { // Bottom-right quadrant -- Feather 8 
                if (pixels_distance < closest[7] && pixels_distance > .001){
                    closest[7] = pixels_distance;
                }
            }
        }
    }
    
    // converting physical distance to 0-255 scale
    for(int i=0; i<=7; i++) {
        //std::cout << "feather: " << i+1 << " distance: " << closest[i];
        closest[i] = 255 + ((0-255.0)*(closest[i]-.001)/(2.0-.001));
        closest[i] = trunc(closest[i]);
        //std::cout << " scaled: " << closest[i] << std::endl;
    }
    
    // Obtaining (R)G(B) color values for each quadrant in heat map
    for(int i=0; i<=7; i++) {
        if(closest[i]<=255 && closest[i]>=0) {
            color[i] = closest[i];
            color[i+8] = 255 - closest[i];
        }
        else if (closest[i]==-1020) {
            color[i] = 255;
            color[i+8] = 0;
            closest[i] = 255;
        }
        // Anything greater than 2m away will remain blue on map
        else {
            color[i] = 0;
            color[i+8] = 255;
        }
        //std::cout << "red: " << color[i] << " blue: " << color[i+8] << std::endl;
    }
    
    //Feather 1
    //std::cout << "Quadrant 1 closest: " << closest[0] << "m -> Intensity: ";
    //printf("Quadrant 1 closest: %.3fm Intensity: ", closest[0]);
    if(closest[0] <= 255 && closest[0] >= 0) {
        int n = closest[0];
        //std::cout << "feather 1: " << n << " " << str_send[n] << std::endl;
        RS232_cputs(cport_nr, str_send[n]);
    }
    else {
        RS232_cputs(cport_nr, str_send[0]);
	}
    
    //Feather 2
    //printf("Quadrant 2 closest: %.3fm Intensity: ", closest[1]);
    if(closest[1] <= 255 && closest[1] >= 0) {
        int n = closest[1] + 256;
        //std::cout << "feather 2: " << n << " " << str_send[n] << std::endl;
		RS232_cputs(cport_nr, str_send[n]);
	}
    else {
        RS232_cputs(cport_nr, str_send[0+256]);
	}
    
    //Feather 3
    //printf("Quadrant 3 closest: %.3fm Intensity: ", closest[2]);
    if(closest[2] <= 255 && closest[2] >= 0) {
        int n = closest[2] + 2*256;
        //std::cout << "feather 3: " << n << " " << str_send[n] << std::endl;
		RS232_cputs(cport_nr, str_send[n]);
	}
    else {
        RS232_cputs(cport_nr, str_send[0+(2*256)]);
	}
    
    //Feather 4
    //printf("Quadrant 4 closest: %.3fm Intensity: ", closest[3]);
    if(closest[3] <= 255 && closest[3] >= 0) {
        int n = closest[3] + 3*256;
        //std::cout << "feather 4: " << n << " " << str_send[n] << std::endl;
		RS232_cputs(cport_nr, str_send[n]);
	}
    else {
        RS232_cputs(cport_nr, str_send[0+(3*256)]);
	}
    
    //Feather 5
    //printf("Quadrant 5 closest: %.3fm Intensity: ", closest[4]);
    if(closest[4] <= 255 && closest[4] >= 0) {
        int n = closest[4] + 4*256;
        //std::cout << "feather 5: " << n << " " << str_send[n] << std::endl;
		RS232_cputs(cport_nr, str_send[n]);
	}
    else {
        RS232_cputs(cport_nr, str_send[0+(4*256)]);
	}
    
    //Feather 6
    //printf("Quadrant 6 closest: %.3fm Intensity: ", closest[5]);
    if(closest[5] <= 255 && closest[5] >= 0) {
        int n = closest[5] + 5*256;
        //std::cout << "feather 6: " << n << " " << str_send[n] << std::endl;
		RS232_cputs(cport_nr, str_send[n]);
	}
    else {
        RS232_cputs(cport_nr, str_send[0+(5*256)]);
	}
    
    //Feather 7
    //printf("Quadrant 7 closest: %.3fm Intensity: ", closest[6]);
    if(closest[6] <= 255 && closest[6] >= 0) {
        int n = closest[6] + 6*256;
        //std::cout << "feather 7: " << n << " " << str_send[n] << std::endl;
		RS232_cputs(cport_nr, str_send[n]);
	}
    else {
        RS232_cputs(cport_nr, str_send[0+(6*256)]);
	}
    
    //Feather 8
    //printf("Quadrant 8 closest: %.3fm Intensity: ", closest[7]);
    if(closest[7] <= 255 && closest[7] >= 0) {
        int n = closest[7] + 7*256;
        //std::cout << "feather 8: " << n << " " << str_send[n] << std::endl;
		RS232_cputs(cport_nr, str_send[n]);
	}
    else {
        RS232_cputs(cport_nr, str_send[0+(7*256)]);
	}    
    
    return color;
}

float get_depth_scale(rs2::device dev)
{
    // Go over the device's sensors
    for (rs2::sensor& sensor : dev.query_sensors())
    {
        // Check if the sensor if a depth sensor
        if (rs2::depth_sensor dpt = sensor.as<rs2::depth_sensor>())
        {
            return dpt.get_depth_scale();
        }
    }
    throw std::runtime_error("Device does not have a depth sensor");
}

rs2_stream find_stream_to_align(const std::vector<rs2::stream_profile>& streams)
{
    //Given a vector of streams, we try to find a depth stream and another stream to align depth with.
    //We prioritize color streams to make the view look better.
    //If color is not available, we take another stream that (other than depth)
    rs2_stream align_to = RS2_STREAM_ANY;
    bool depth_stream_found = false;
    bool color_stream_found = false;
    for (rs2::stream_profile sp : streams)
    {
        rs2_stream profile_stream = sp.stream_type();
        if (profile_stream != RS2_STREAM_DEPTH)
        {
            if (!color_stream_found)         //Prefer color
                align_to = profile_stream;

            if (profile_stream == RS2_STREAM_COLOR)
            {
                color_stream_found = true;
            }
        }
        else
        {
            depth_stream_found = true;
        }
    }

    if(!depth_stream_found)
        throw std::runtime_error("No Depth stream available");

    if (align_to == RS2_STREAM_ANY)
        throw std::runtime_error("No stream found to align with Depth");

    return align_to;
}

bool profile_changed(const std::vector<rs2::stream_profile>& current, const std::vector<rs2::stream_profile>& prev)
{
    for (auto&& sp : prev)
    {
        //If previous profile is in current (maybe just added another)
        auto itr = std::find_if(std::begin(current), std::end(current), [&sp](const rs2::stream_profile& current_sp) { return sp.unique_id() == current_sp.unique_id(); });
        if (itr == std::end(current)) //If it previous stream wasn't found in current
        {
            return true;
        }
    }
    return false;
}

// button to "test" the feathers. sets all feather output to max intensity on one press
// and to zero upon following press
void testButton() {
    RS232_cputs(cport_nr, str_send[255]);
    RS232_cputs(cport_nr, str_send[255+256]);
    RS232_cputs(cport_nr, str_send[255+2*256]);
    RS232_cputs(cport_nr, str_send[255+3*256]);
    RS232_cputs(cport_nr, str_send[255+4*256]);
    RS232_cputs(cport_nr, str_send[255+5*256]);
    RS232_cputs(cport_nr, str_send[255+6*256]);
    RS232_cputs(cport_nr, str_send[255+7*256]);
}
