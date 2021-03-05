#include "ContentCheck.h"

int ContentCheck::NationDetect(char* nation)
{
	/*if (!strncmp(nation, "CHN", 3))
	return 1;

	if (!strncmp(nation, "AIT", 3))
	return 2;

	if (!strncmp(nation, "NLD", 3))
	return 3;

	if (!strncmp(nation, "USA", 3))
	return 4;

	if (!strncmp(nation, "AUS", 3))
	return 5;

	if (!strncmp(nation, "JPN", 3))
	return 6;*/

	if (!strncmp(nation, "AFG", 3))
		return 1;

	if (!strncmp(nation, "ALB", 3))
		return 2;

	if (!strncmp(nation, "AND", 3))
		return 3;

	if (!strncmp(nation, "AGO", 3))
		return 4;

	if (!strncmp(nation, "ARG", 3))
		return 5;

	if (!strncmp(nation, "ARM", 3))
		return 6;

	if (!strncmp(nation, "AUS", 3))
		return 7;

	if (!strncmp(nation, "AUT", 3))
		return 8;

	if (!strncmp(nation, "AZE", 3))
		return 9;

	if (!strncmp(nation, "ARE", 3))
		return 10;

	if (!strncmp(nation, "AIT", 3))
		return 11;

	if (!strncmp(nation, "BHS", 3))
		return 12;

	if (!strncmp(nation, "BHR", 3))
		return 13;

	if (!strncmp(nation, "BGD", 3))
		return 14;

	if (!strncmp(nation, "BRB", 3))
		return 15;

	if (!strncmp(nation, "BLR", 3))
		return 16;

	if (!strncmp(nation, "BLZ", 3))
		return 17;

	if (!strncmp(nation, "BEN", 3))
		return 18;

	if (!strncmp(nation, "BTN", 3))
		return 19;

	if (!strncmp(nation, "BOL", 3))
		return 20;

	if (!strncmp(nation, "BWA", 3))
		return 21;

	if (!strncmp(nation, "BRA", 3))
		return 22;

	if (!strncmp(nation, "BRN", 3))
		return 23;

	if (!strncmp(nation, "BGR", 3))
		return 24;

	if (!strncmp(nation, "BFA", 3))
		return 25;

	if (!strncmp(nation, "BDI", 3))
		return 26;

	if (!strncmp(nation, "CMR", 3))
		return 27;

	if (!strncmp(nation, "CAN", 3))
		return 28;

	if (!strncmp(nation, "CPV", 3))
		return 29;

	if (!strncmp(nation, "CAF", 3))
		return 30;

	if (!strncmp(nation, "CHL", 3))
		return 31;

	if (!strncmp(nation, "CHN", 3))
		return 32;

	if (!strncmp(nation, "CIV", 3))
		return 33;

	if (!strncmp(nation, "COL", 3))
		return 34;

	if (!strncmp(nation, "COM", 3))
		return 35;

	if (!strncmp(nation, "COD", 3))
		return 36;

	if (!strncmp(nation, "CUB", 3))
		return 37;

	if (!strncmp(nation, "CYP", 3))
		return 38;

	if (!strncmp(nation, "CZE", 3))
		return 39;

	if (!strncmp(nation, "CHE", 3))
		return 40;

	if (!strncmp(nation, "DZA", 3))
		return 41;

	if (!strncmp(nation, "DNK", 3))
		return 42;

	if (!strncmp(nation, "DJI", 3))
		return 43;

	if (!strncmp(nation, "DOM", 3))
		return 44;

	if (!strncmp(nation, "DEU", 3))
		return 45;

	if (!strncmp(nation, "EST", 3))
		return 46;

	if (!strncmp(nation, "ERI", 3))
		return 47;

	if (!strncmp(nation, "ECU", 3))
		return 48;

	if (!strncmp(nation, "EGY", 3))
		return 49;

	if (!strncmp(nation, "ESP", 3))
		return 50;

	if (!strncmp(nation, "FJI", 3))
		return 51;

	if (!strncmp(nation, "FIN", 3))
		return 52;

	if (!strncmp(nation, "FRA", 3))
		return 53;

	if (!strncmp(nation, "GAB", 3))
		return 54;

	if (!strncmp(nation, "GNQ", 3))
		return 55;

	if (!strncmp(nation, "GMB", 3))
		return 56;

	if (!strncmp(nation, "GEO", 3))
		return 57;

	if (!strncmp(nation, "GHA", 3))
		return 58;

	if (!strncmp(nation, "GRC", 3))
		return 59;

	if (!strncmp(nation, "GRD", 3))
		return 60;

	if (!strncmp(nation, "GTM", 3))
		return 61;

	if (!strncmp(nation, "GIN", 3))
		return 62;

	if (!strncmp(nation, "GUY", 3))
		return 63;

	if (!strncmp(nation, "GBR", 3))
		return 64;

	if (!strncmp(nation, "HTI", 3))
		return 65;

	if (!strncmp(nation, "HND", 3))
		return 66;

	if (!strncmp(nation, "HUN", 3))
		return 67;

	if (!strncmp(nation, "HRV", 3))
		return 68;

	if (!strncmp(nation, "ISL", 3))
		return 69;

	if (!strncmp(nation, "IND", 3))
		return 70;

	if (!strncmp(nation, "IDN", 3))
		return 71;

	if (!strncmp(nation, "IRN", 3))
		return 72;

	if (!strncmp(nation, "IRQ", 3))
		return 73;

	if (!strncmp(nation, "ISR", 3))
		return 74;

	if (!strncmp(nation, "ITA", 3))
		return 75;

	if (!strncmp(nation, "JPN", 3))
		return 76;

	if (!strncmp(nation, "JOR", 3))
		return 77;

	if (!strncmp(nation, "KAZ", 3))
		return 78;

	if (!strncmp(nation, "KEN", 3))
		return 79;

	if (!strncmp(nation, "KOR", 3))
		return 80;

	if (!strncmp(nation, "KGZ", 3))
		return 81;

	if (!strncmp(nation, "KWT", 3))
		return 82;

	if (!strncmp(nation, "KHM", 3))
		return 83;

	if (!strncmp(nation, "KNA", 3))
		return 84;

	if (!strncmp(nation, "LAO", 3))
		return 85;

	if (!strncmp(nation, "LVA", 3))
		return 86;

	if (!strncmp(nation, "LBN", 3))
		return 87;

	if (!strncmp(nation, "LSO", 3))
		return 88;

	if (!strncmp(nation, "LBR", 3))
		return 89;

	if (!strncmp(nation, "LBY", 3))
		return 90;

	if (!strncmp(nation, "LIE", 3))
		return 91;

	if (!strncmp(nation, "LTU", 3))
		return 92;

	if (!strncmp(nation, "LUX", 3))
		return 93;

	if (!strncmp(nation, "LKA", 3))
		return 94;

	if (!strncmp(nation, "LCA", 3))
		return 95;

	if (!strncmp(nation, "MMR", 3))
		return 96;

	if (!strncmp(nation, "MDG", 3))
		return 97;

	if (!strncmp(nation, "MWI", 3))
		return 98;

	if (!strncmp(nation, "MYS", 3))
		return 99;

	if (!strncmp(nation, "MDV", 3))
		return 100;

	if (!strncmp(nation, "MLI", 3))
		return 101;

	if (!strncmp(nation, "MRT", 3))
		return 102;

	if (!strncmp(nation, "MUS", 3))
		return 103;

	if (!strncmp(nation, "MEX", 3))
		return 104;

	if (!strncmp(nation, "MDA", 3))
		return 105;

	if (!strncmp(nation, "MCO", 3))
		return 106;

	if (!strncmp(nation, "MNG", 3))
		return 107;

	if (!strncmp(nation, "MAR", 3))
		return 108;

	if (!strncmp(nation, "MOZ", 3))
		return 109;

	if (!strncmp(nation, "NAM", 3))
		return 110;

	if (!strncmp(nation, "NRU", 3))
		return 111;

	if (!strncmp(nation, "NPL", 3))
		return 112;

	if (!strncmp(nation, "NLD", 3))
		return 113;

	if (!strncmp(nation, "NZL", 3))
		return 114;

	if (!strncmp(nation, "NIC", 3))
		return 115;

	if (!strncmp(nation, "NER", 3))
		return 116;

	if (!strncmp(nation, "NOR", 3))
		return 117;

	if (!strncmp(nation, "OMN", 3))
		return 118;

	if (!strncmp(nation, "PAK", 3))
		return 119;

	if (!strncmp(nation, "PLW", 3))
		return 120;

	if (!strncmp(nation, "PSE", 3))
		return 121;

	if (!strncmp(nation, "PAN", 3))
		return 122;

	if (!strncmp(nation, "PNG", 3))
		return 123;

	if (!strncmp(nation, "PRY", 3))
		return 124;

	if (!strncmp(nation, "PER", 3))
		return 125;

	if (!strncmp(nation, "PHL", 3))
		return 126;

	if (!strncmp(nation, "POL", 3))
		return 127;

	if (!strncmp(nation, "PRT", 3))
		return 128;

	if (!strncmp(nation, "PRK", 3))
		return 129;

	if (!strncmp(nation, "QAT", 3))
		return 130;

	if (!strncmp(nation, "ROU", 3))
		return 131;

	if (!strncmp(nation, "RUS", 3))
		return 132;

	if (!strncmp(nation, "RWA", 3))
		return 133;

	if (!strncmp(nation, "SMR", 3))
		return 134;

	if (!strncmp(nation, "STP", 3))
		return 135;

	if (!strncmp(nation, "SAU", 3))
		return 136;

	if (!strncmp(nation, "SEN", 3))
		return 137;

	if (!strncmp(nation, "SRB", 3))
		return 138;

	if (!strncmp(nation, "SYC", 3))
		return 139;

	if (!strncmp(nation, "SGP", 3))
		return 140;

	if (!strncmp(nation, "SVK", 3))
		return 141;

	if (!strncmp(nation, "SVN", 3))
		return 142;

	if (!strncmp(nation, "SOM", 3))
		return 143;

	if (!strncmp(nation, "SSD", 3))
		return 144;

	if (!strncmp(nation, "SDN", 3))
		return 145;

	if (!strncmp(nation, "SUR", 3))
		return 146;

	if (!strncmp(nation, "SWZ", 3))
		return 147;

	if (!strncmp(nation, "SYR", 3))
		return 148;

	if (!strncmp(nation, "TWN", 3))
		return 149;

	if (!strncmp(nation, "TJK", 3))
		return 150;

	if (!strncmp(nation, "TZA", 3))
		return 151;

	if (!strncmp(nation, "THA", 3))
		return 152;

	if (!strncmp(nation, "TGO", 3))
		return 153;

	if (!strncmp(nation, "TON", 3))
		return 154;

	if (!strncmp(nation, "TTO", 3))
		return 155;

	if (!strncmp(nation, "TUN", 3))
		return 156;

	if (!strncmp(nation, "TUR", 3))
		return 157;

	if (!strncmp(nation, "UGA", 3))
		return 158;

	if (!strncmp(nation, "UKR", 3))
		return 159;

	if (!strncmp(nation, "USA", 3))
		return 160;

	if (!strncmp(nation, "URY", 3))
		return 161;

	if (!strncmp(nation, "UZB", 3))
		return 162;

	if (!strncmp(nation, "VUT", 3))
		return 163;

	if (!strncmp(nation, "VAT", 3))
		return 164;

	if (!strncmp(nation, "VEN", 3))
		return 165;

	if (!strncmp(nation, "VNM", 3))
		return 166;

	if (!strncmp(nation, "VCT", 3))
		return 167;

	if (!strncmp(nation, "WSM", 3))
		return 168;

	if (!strncmp(nation, "YEM", 3))
		return 169;

	if (!strncmp(nation, "ZMB", 3))
		return 170;

	if (!strncmp(nation, "ZIM", 3))
		return 171;

	if (!strncmp(nation, "ZAF", 3))
		return 172;

	if (!strncmp(nation, "MKD", 3))
		return 173;

	if (!strncmp(nation, "BEL", 3))
		return 174;

	if (!strncmp(nation, "FSM", 3))
		return 175;

	if (!strncmp(nation, "SLB", 3))
		return 176;

	if (!strncmp(nation, "COG", 3))
		return 177;

	if (!strncmp(nation, "TLS", 3))
		return 178;

	if (!strncmp(nation, "TCD", 3))
		return 179;

	if (!strncmp(nation, "MLT", 3))
		return 180;

	if (!strncmp(nation, "NGA", 3))
		return 181;

	if (!strncmp(nation, "MNE", 3))
		return 182;

	if (!strncmp(nation, "SWE", 3))
		return 183;

	if (!strncmp(nation, "ETH", 3))
		return 184;

	if (!strncmp(nation, "CRI", 3))
		return 185;

	if (!strncmp(nation, "SLV", 3))
		return 186;

	if (!strncmp(nation, "ZWE", 3))
		return 187;

	if (!strncmp(nation, "JAM", 3))
		return 188;

	if (!strncmp(nation, "RKS", 3))
		return 189;

	if (!strncmp(nation, "DMA", 3))
		return 190;

	if (!strncmp(nation, "IRL", 3))
		return 191;

	if (!strncmp(nation, "TUV", 3))
		return 260;

	if (!strncmp(nation, "DMA", 3))
		return 261;

	if (!strncmp(nation, "SLB", 3))
		return 262;

	if (!strncmp(nation, "GNB", 3))
		return 263;

	if (!strncmp(nation, "SLE", 3))
		return 264;

	if (!strncmp(nation, "TCA", 3))
		return 265;

	if (!strncmp(nation, "CYM", 3))
		return 266;

	if (!strncmp(nation, "ATG", 3))
		return 267;

	if (!strncmp(nation, "BIH", 3))
		return 268;

	if (!strncmp(nation, "TKM", 3))
		return 269;



	return 0;
}


int ContentCheck::PassportType(char* type)
{


	if (!strncmp(type, "MA", 2))
		return 1;

	if (!strncmp(type, "PA", 2))
		return 2;

	if (!strncmp(type, "PB", 2))
		return 3;

	if (!strncmp(type, "PO", 2))
		return 4;

	if (!strncmp(type, "PN", 2))
		return 5;

	if (!strncmp(type, "PM", 2))
		return 13;

	if (!strncmp(type, "PS", 2))
		return 14;

	if (!strncmp(type, "PD", 2))
		return 15;

	if (!strncmp(type, "PE", 2))
		return 16;

	if (!strncmp(type, "SD", 2))
		return 17;

	if (!strncmp(type, "PC", 2))
		return 18;

	if (!strncmp(type, "PX", 2))
		return 19;

	if (!strncmp(type, "PG", 2))
		return 20;

	if (!strncmp(type, "PF", 2))
		return 21;

	if (!strncmp(type, "PT", 2))
		return 22;

	if (!strncmp(type, "SE", 2))
		return 22;

	if (!strncmp(type, "DP", 2))
		return 23;

	if (!strncmp(type, "PV", 2))
		return 24;

	if (!strncmp(type, "M", 1))
		return 1;

	if (!strncmp(type, "P", 1))
		return 6;

	if (!strncmp(type, "E", 1))
		return 7;

	if (!strncmp(type, "K", 1))
		return 8;

	if (!strncmp(type, "W", 1))
		return 9;

	if (!strncmp(type, "G", 1))
		return 10;

	if (!strncmp(type, "S", 1))
		return 11;

	if (!strncmp(type, "D", 1))
		return 12;

	return 0;
}