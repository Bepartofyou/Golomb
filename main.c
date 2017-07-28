#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>
#include <inttypes.h>
#include "cmdline.h"

/*
 * Get the bit value of the `n` bit frome pointer `p`
 * 
 */ 
static uint8_t get_bit(const uint8_t * p, uint64_t n){

	return (p[n/8] & (uint8_t)(1 << (7-(n % 8)))) ? 1 : 0;
}

/*
 * Set the bit value of the `n` bit frome pointer `p`
 * 
 */
static void set_bit(uint8_t * p, uint64_t n, bool flag){

	if(flag)
		p[n/8] |= (uint8_t)(1 << (7-(n % 8)));
	else
		p[n/8] &= (uint8_t)(~(1 << (7-(n % 8))));
}

/*
 *Judging num is a power of 2 or Not
 *
 */
static bool is_2_power(uint64_t num){

	if(num > 0 && ((num & (num - 1)) == 0))
		return true;

	return false;
}

/*
 *Judging sys endian
 *
 */
int is_big_endian(void) {

	long one = 1;
	return !(*((char *)(&one)));
}

/*
 *endian using with union
 *
 */
#if 0
	union {
		uint64_t value;
		uint8_t  tmp[8];
	}src = {0},dst = {0};

	if(is_big_endian()){
		dst.tmp[0] = src.tmp[0];
		dst.tmp[1] = src.tmp[1];
		dst.tmp[2] = src.tmp[2];
		dst.tmp[3] = src.tmp[3];
		dst.tmp[4] = src.tmp[4];
		dst.tmp[5] = src.tmp[5];
		dst.tmp[6] = src.tmp[6];
		dst.tmp[7] = src.tmp[7];

		printf("big endian\n");
	}else{
		dst.tmp[0] = src.tmp[7];
		dst.tmp[1] = src.tmp[6];
		dst.tmp[2] = src.tmp[5];
		dst.tmp[3] = src.tmp[4];
		dst.tmp[4] = src.tmp[3];
		dst.tmp[5] = src.tmp[2];
		dst.tmp[6] = src.tmp[1];
		dst.tmp[7] = src.tmp[0];

		printf("little endian\n");
	}
#endif

/*
 *Golomb-Rice algorithm encoder
 *
 */
static uint64_t rice_golomb_encode(uint8_t ** buf, uint64_t num, uint64_t param){

	if(!is_2_power(param)){

		printf("parameter %u is not a power of 2\n", param);
		return 0;
	}
/*	else
		printf("parameter %u is  a power of 2, power: %u\n", param, (uint64_t)(log(param)/log(2)));*/
	
	uint64_t k = log(param) / log(2);  	//the power of 2
	uint64_t q = num >> k;       	//the result of a division by M  (num / param)
	uint64_t r = num & (param - 1);     //the remainder                  (num % param)
	uint64_t len = q + 1 + k;       //the encode bit length          
	//printf("q(num / param): %lu, r(num %% param): %lu, k(the power of 2): %lu, len(q + 1 + k): %lu\n", q, r, k, len);

	*buf = (uint8_t *)malloc(sizeof(uint8_t) * (len / 8 + 1));

	uint64_t i;
	for(i = 0; i < q; i++){

		set_bit(*buf, i, 1);
	}
	set_bit(*buf, q, 0);
	for(i = 1; i <= k; i++){

		set_bit(*buf, q + i, (bool)(r & 0x01));
		r >>= 1;
	}

	//printf encode bit
	printf("Golomb-Rice encode bits of %lu :\n", num);
	printf("                                ");
	for(i = 0; i < len; i++){
		printf("%d",get_bit(*buf, i));
	}
	printf("\n");

	return len;
}

/*
 *Golomb-Rice algorithm decoder
 *
 */
 static uint64_t rice_golomb_decode(uint8_t * buf, uint64_t len){
	
	uint64_t unary = 0;             //the result of a division by M  (num / m)
	while(get_bit(buf, unary))
		unary++;

	uint64_t k = len - unary - 1;   //the power of 2
	uint64_t m = (uint64_t)pow(2, k);   
	
	uint64_t i;
	uint64_t r = 0;
	for(i = 0; i < k; i++){

		r |= ((bool)get_bit(buf, unary + 1 + i) << i);
		//printf("index:%lu  value:%d\n", unary + i,(bool)get_bit(buf, unary + i));
	}

	//printf("k:%lu  m:%lu  r:%lu  unary:%lu  decode num:%lu\n", k, m, r, unary, unary * m + r);
	printf("Golomb-Rice decode bits of  ");
	for(i = 0; i < len; i++){
		printf("%d",get_bit(buf, i));
	}
	printf("  :\n");
	printf("                                   %lu\n",unary * m + r);

	return unary * m + r; 
}

/*
 *Exponential-Golomb algorithm encoder
 *
 */
static uint64_t exp_golomb_encode(uint8_t ** buf, uint64_t num, uint64_t k){
	
	uint64_t tmp = num;

	if (k > 52){
		printf("k level must little then 52\n");
		return 0;
	}

	if (k != 0)
		num = num + pow(2, k) - 1;  //Exp-Golomb coding for k level

	// 0 level exp-golomb encode
	uint64_t m = (uint64_t)(log(num + 1) / log(2));  // prefix，group id
	uint64_t info = (uint64_t)(num + 1 - pow(2, m)); // group offset
	uint64_t len = 2 * m + 1; 						 // the encode bit length 

	//printf("m: %lu, info: %lu, len: %lu\n", m, info, len);

	*buf = (uint8_t *)malloc(sizeof(uint8_t) * (len / 8 + 1));

	uint64_t i;
	for(i = 0; i < m; i++){

		set_bit(*buf, i, 0);
	}
	set_bit(*buf, m, 1);
	for(i = 1; i <= m; i++){

		set_bit(*buf, m + i, (bool)(info & 0x01));
		info >>= 1;
	}

	//printf encode bit
	printf("Exponential-Golomb encode bits of %lu :\n", tmp);
	printf("                                       ");
	for(i = 0; i < len; i++){
		printf("%d",get_bit(*buf, i));
	}
	printf("\n");

	return len;
}

/*
 *Exponential-Golomb algorithm decoder
 *
 */
 static uint64_t exp_golomb_decode(uint8_t * buf, uint64_t len, uint64_t k){
	
	uint64_t groupID = 0;             //the result of a division by M  (num / m)
	while(!get_bit(buf, groupID))
		groupID++;

	uint64_t count = len - groupID - 1;   //the power of 2
	
	uint64_t i;
	uint64_t r = 0;
	for(i = 0; i < count; i++){

		r |= ((bool)get_bit(buf, groupID + 1 + i) << i);
		//printf("index:%lu  value:%d\n", groupID + i,(bool)get_bit(buf, groupID + i));
	}

	uint64_t num = pow(2, groupID) - 1 + r;

	// k level
	if (k != 0)
		num = num - pow(2, k) + 1;

	//printf("k:%lu  r:%lu  groupID:%lu  decode num:%lu\n", k, r, groupID, num);
	printf("Exponential-Golomb decode bits of  ");
	for(i = 0; i < len; i++){
		printf("%d",get_bit(buf, i));
	}
	printf(" :\n");
	printf("                                   %lu\n", num);
	return num; 
}

int main(int argc, char** argv){

	struct gengetopt_args_info info;
	if (cmdline_parser(argc, argv, &info) != 0) {
		exit(1);
	}

	uint8_t* encodedata = NULL;

	switch(info.algorithm_arg){
		case 1:{
			uint64_t len = rice_golomb_encode(&encodedata, info.encodenum_arg, info.parameter_arg);
			if(len == 0)
				break;

			if(encodedata)
				rice_golomb_decode(encodedata, len);
			else
				printf("buf is null\n");
		}
		break;

		case 2:{
			uint64_t len = exp_golomb_encode(&encodedata, info.encodenum_arg, info.parameter_arg);
			if(len == 0)
				break;

			if(encodedata)
				exp_golomb_decode(encodedata, len, info.parameter_arg);
			else
				printf("buf is null\n");
		}
		break;

		default:
			printf("invalid Golomb algorithm\n");
			break;
	}

	if(encodedata){
		free(encodedata);
		encodedata = NULL;
	}

	return 0;
}
