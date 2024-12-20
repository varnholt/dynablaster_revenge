#include "fontmap.h"

BitmapFont::Parameter MenuFont::sMenuChars[129] = {
   {   0,   0,  0,   0,  0,   0,  0, 24 },
   {   1,   0,  0,   0,  0,   0,  0, 24 },
   {   2,   0,  0,   0,  0,   0,  0, 24 },
   {   3,   0,  0,   0,  0,   0,  0, 24 },
   {   4,   0,  0,   0,  0,   0,  0, 24 },
   {   5,   0,  0,   0,  0,   0,  0, 24 },
   {   6,   0,  0,   0,  0,   0,  0, 24 },
   {   7,   0,  0,   0,  0,   0,  0, 24 },
   {   8,   0,  0,   0,  0,   0,  0, 24 },
   {   9,   0,  0,   0,  0,   0,  0, 24 },
   {  10,   0,  0,   0,  0,   0,  0, 24 },
   {  11,   0,  0,   0,  0,   0,  0, 24 },
   {  12,   0,  0,   0,  0,   0,  0, 24 },
   {  13,   0,  0,   0,  0,   0,  0, 24 },
   {  14,   0,  0,   0,  0,   0,  0, 24 },
   {  15,   0,  0,   0,  0,   0,  0, 24 },
   {  16,   0,  0,   0,  0,   0,  0, 24 },
   {  17,   0,  0,   0,  0,   0,  0, 24 },
   {  18,   0,  0,   0,  0,   0,  0, 24 },
   {  19,   0,  0,   0,  0,   0,  0, 24 },
   {  20,   0,  0,   0,  0,   0,  0, 24 },
   {  21,   0,  0,   0,  0,   0,  0, 24 },
   {  22,   0,  0,   0,  0,   0,  0, 24 },
   {  23,   0,  0,   0,  0,   0,  0, 24 },
   {  24,   0,  0,   0,  0,   0,  0, 24 },
   {  25,   0,  0,   0,  0,   0,  0, 24 },
   {  26,   0,  0,   0,  0,   0,  0, 24 },
   {  27,   0,  0,   0,  0,   0,  0, 24 },
   {  28,   0,  0,   0,  0,   0,  0, 24 },
   {  29,   0,  0,   0,  0,   0,  0, 24 },
   {  30,   0,  0,   0,  0,   0,  0, 24 },
   {  31,   0,  0,   0,  0,   0,  0, 24 },
   { ' ', 949,138,  88,122, -33,-31, 24 },
   { '!', 388,768,  76,121, -33,-31, 13 },
   { '"', 418,894,  90, 85, -33,  6, 28 },
   { '#', 492,  2, 132,122, -33,-31, 70 },
   { '$',  82,  2, 131,126, -33,-33, 69 },
   { '%', 468,768, 123,120, -33,-31, 61 },
   { '&', 818,390, 131,122, -33,-31, 69 },
   { '\'',592,894,  76, 85, -33,  6, 13 },
   { '(',   2,768,  86,122, -33,-31, 24 },
   { ')', 840,642,  86,122, -33,-31, 24 },
   { '*', 218,894,  83, 87, -33,-16, 21 },
   { '+', 840,768, 109,109, -33,-25, 47 },
   { ',', 512,894,  76, 85, -33,-43, 13 },
   { '-', 808,894, 109, 74, -33, -7, 47 },
   { '.', 922,894,  76, 73, -33,-31, 13 },
   { '/', 596,768, 118,120, -33,-31, 56 },
   { '0', 682,390, 131,122, -33,-31, 69 },
   { '1',  92,768,  76,122, -33,-31, 13 },
   { '2',   2,516, 131,122, -33,-31, 69 },
   { '3', 250,642, 120,122, -33,-31, 58 },
   { '4', 274,516, 131,122, -33,-31, 69 },
   { '5', 138,516, 131,122, -33,-31, 69 },
   { '6',   2,390, 131,122, -33,-31, 69 },
   { '7', 900,  2, 120,122, -33,-31, 58 },
   { '8', 274,138, 131,122, -33,-31, 69 },
   { '9', 138,138, 131,122, -33,-31, 69 },
   { ':',   2,894,  76,109, -33,-24, 13 },
   { ';', 308,768,  76,121, -33,-36, 13 },
   { '<', 658,642,  88,122, -33,-31, 26 },
   { '=',  82,894, 131, 94, -33,-18, 69 },
   { '>', 566,642,  88,122, -33,-31, 26 },
   { '?', 274,390, 131,122, -33,-31, 69 },
   { '@', 628,  2, 132,122, -33,-31, 70 },
   { 'A', 410,516, 131,122, -33,-31, 69 },
   { 'B', 410,264, 131,122, -33,-31, 69 },
   { 'C',   2,642, 120,122, -33,-31, 58 },
   { 'D', 546,138, 131,122, -33,-31, 69 },
   { 'E', 794,516, 120,122, -33,-31, 58 },
   { 'F', 670,516, 120,122, -33,-31, 58 },
   { 'G',   2,138, 131,122, -33,-31, 69 },
   { 'H', 764,  2, 132,122, -33,-31, 69 },
   { 'I',  92,768,  76,122, -33,-31, 13 },
   { 'J', 546,516, 120,122, -33,-31, 58 },
   { 'K', 410,138, 131,122, -33,-31, 69 },
   { 'L', 126,642, 120,122, -33,-31, 58 },
   { 'M', 682,138, 131,122, -33,-31, 69 },
   { 'N', 818,138, 131,122, -33,-31, 69 },
   { 'O',   2,264, 131,122, -33,-31, 69 },
   { 'P', 138,264, 131,122, -33,-31, 69 },
   { 'Q', 274,264, 131,122, -33,-31, 69 },
   { 'R', 218,  2, 133,122, -33,-31, 70 },
   { 'S', 546,264, 131,122, -33,-31, 69 },
   { 'T', 682,264, 131,122, -33,-31, 69 },
   { 'U', 818,264, 131,122, -33,-31, 69 },
   { 'V', 356,  2, 132,122, -33,-31, 70 },
   { 'W', 138,390, 131,122, -33,-31, 69 },
   { 'X', 172,768, 131,121, -33,-31, 69 },
   { 'Y', 410,390, 131,122, -33,-31, 69 },
   { 'Z', 546,390, 131,122, -33,-31, 69 },
   { '[', 750,642,  86,122, -33,-31, 24 },
   { '\\',718,768, 118,120, -33,-31, 56 },
   { ']', 930,642,  86,122, -33,-31, 24 },
   { '^', 918,516,  96, 80, -33, 11, 34 },
   { '_', 672,894, 131, 74, -33,-31, 69 },
   { '`', 592,894,  76, 85, -33,  6, 13 },
   { 'a', 410,516, 131,122, -33,-31, 69 },
   { 'b', 410,264, 131,122, -33,-31, 69 },
   { 'c',   2,642, 120,122, -33,-31, 58 },
   { 'd', 546,138, 131,122, -33,-31, 69 },
   { 'e', 794,516, 120,122, -33,-31, 58 },
   { 'f', 670,516, 120,122, -33,-31, 58 },
   { 'g',   2,138, 131,122, -33,-31, 69 },
   { 'h', 764,  2, 132,122, -33,-31, 69 },
   { 'i',  92,768,  76,122, -33,-31, 13 },
   { 'j', 546,516, 120,122, -33,-31, 58 },
   { 'k', 410,138, 131,122, -33,-31, 69 },
   { 'l', 126,642, 120,122, -33,-31, 58 },
   { 'm', 682,138, 131,122, -33,-31, 69 },
   { 'n', 818,138, 131,122, -33,-31, 69 },
   { 'o',   2,264, 131,122, -33,-31, 69 },
   { 'p', 138,264, 131,122, -33,-31, 69 },
   { 'q', 274,264, 131,122, -33,-31, 69 },
   { 'r', 218,  2, 133,122, -33,-31, 70 },
   { 's', 546,264, 131,122, -33,-31, 69 },
   { 't', 682,264, 131,122, -33,-31, 69 },
   { 'u', 818,264, 131,122, -33,-31, 69 },
   { 'v', 356,  2, 132,122, -33,-31, 70 },
   { 'w', 138,390, 131,122, -33,-31, 69 },
   { 'x', 172,768, 131,121, -33,-31, 69 },
   { 'y', 410,390, 131,122, -33,-31, 69 },
   { 'z', 546,390, 131,122, -33,-31, 69 },
   { '{', 374,642,  92,122, -33,-31, 30 },
   { '|',   2,  2,  76,132, -33,-36, 13 },
   { '}', 470,642,  92,122, -33,-31, 30 },
   { '~', 306,894, 108, 86, -35,  5, 49 },
   { 127,   0,  0,   0,  0,   0,  0, 24 },
   {  -1,   0,  0,   0,  0,   0,  0,  0 }
};

