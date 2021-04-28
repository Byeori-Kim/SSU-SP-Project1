/*
 * 화일명 : my_assembler_20171281.c
 * 설  명 : 이 프로그램은 SIC/XE 머신을 위한 간단한 Assembler 프로그램의 메인루틴으로,
 * 입력된 파일의 코드 중, 명령어에 해당하는 OPCODE를 찾아 출력한다.
 * 파일 내에서 사용되는 문자열 "00000000"에는 자신의 학번을 기입한다.
 */

 /*
  *
  * 프로그램의 헤더를 정의한다.
  *
  */
#define _GNU_SOURCE
#pragma warning(disable: 4996)
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>


  // 파일명의 "00000000"은 자신의 학번으로 변경할 것.
#include "my_assembler_20171281.h"

/* ----------------------------------------------------------------------------------
 * 설명 : 사용자로 부터 어셈블리 파일을 받아서 명령어의 OPCODE를 찾아 출력한다.
 * 매계 : 실행 파일, 어셈블리 파일
 * 반환 : 성공 = 0, 실패 = < 0
 * 주의 : 현재 어셈블리 프로그램의 리스트 파일을 생성하는 루틴은 만들지 않았다.
 *		   또한 중간파일을 생성하지 않는다.
 * ----------------------------------------------------------------------------------
 */
int main(int args, char* arg[])
{
	if (init_my_assembler() < 0)
	{
		printf("init_my_assembler: 프로그램 초기화에 실패 했습니다.\n");
		return -1;
	}

	if (assem_pass1() < 0)
	{
		printf("assem_pass1: 패스1 과정에서 실패하였습니다.  \n");
		return -1;
	}

	//make_opcode_output(NULL);
	//make_opcode_output("output_20171281.txt");

	
	make_symtab_output("symtab_20171281.txt");																// symboltable 출력
	make_literaltab_output("literaltab_20171281.txt");														// literaltable 출력
	if (assem_pass2() < 0)
	{
		printf(" assem_pass2: 패스2 과정에서 실패하였습니다.  \n");
		return -1;
	}

	make_objectcode_output("output_20171281.txt");															// 최종결과 출력
	
	return 0;
}

/* ----------------------------------------------------------------------------------
 * 설명 : 프로그램 초기화를 위한 자료구조 생성 및 파일을 읽는 함수이다.
 * 매계 : 없음
 * 반환 : 정상종료 = 0 , 에러 발생 = -1
 * 주의 : 각각의 명령어 테이블을 내부에 선언하지 않고 관리를 용이하게 하기
 *		   위해서 파일 단위로 관리하여 프로그램 초기화를 통해 정보를 읽어 올 수 있도록
 *		   구현하였다.
 * ----------------------------------------------------------------------------------
 */
int init_my_assembler(void)
{
	int result=0;

	if ((result = init_inst_file("inst.data")) < 0)
		return -1;
	if ((result = init_input_file("input.txt")) < 0)
		return -1;
	return result;
}

/* ----------------------------------------------------------------------------------
 * 설명 : 머신을 위한 기계 코드목록 파일을 읽어 기계어 목록 테이블(inst_table)을
 *        생성하는 함수이다.
 * 매계 : 기계어 목록 파일
 * 반환 : 정상종료 = 0 , 에러 < 0
 * 주의 : 기계어 목록파일 형식은 자유롭게 구현한다. 예시는 다음과 같다.
 *
 *	===============================================================================
 *		   | 이름 | 형식 | 기계어 코드 | 오퍼랜드의 갯수 | NULL|
 *	===============================================================================
 *
 * ----------------------------------------------------------------------------------
 */
int init_inst_file(char* inst_file)
{
	FILE* file;
	int errno;
	int len = 99;
	char* line = malloc(len);
	file = fopen(inst_file, "r");
	inst_index = 0;
	

	if (file == NULL) { 
		errno = -1; 
		return errno;
	}																										// file is not opened

	else {
		while (fscanf(file, "%[^\n]\n", line) != EOF) {
			struct inst_unit in = { 0 };

			char* token;
			int a = 0;
			token = strtok(line, " ");
			inst_table[inst_index] = malloc(sizeof(inst));

			strncpy(in.str, token, sizeof(token));
			token = strtok(NULL, " ");

			in.format1 = *token - 48;
			token = strtok(NULL, " ");

			in.format2 = *token - 48;
			token = strtok(NULL, " ");

			in.op = ((token[2]-48)<<4) + (token[3]-48);
			token = strtok(NULL, " ");

			in.ops = *token - 48;
			token = strtok(NULL, " ");


			*inst_table[inst_index] = in;

			inst_index++;
			;
		}
		errno = 0;
	}

	fclose(file);
	return errno;
}

/* ----------------------------------------------------------------------------------
 * 설명 : 어셈블리 할 소스코드를 읽어 소스코드 테이블(input_data)를 생성하는 함수이다.
 * 매계 : 어셈블리할 소스파일명
 * 반환 : 정상종료 = 0 , 에러 < 0
 * 주의 : 라인단위로 저장한다.
 *
 * ----------------------------------------------------------------------------------
 */
int init_input_file(char* input_file)
{
	FILE* file;
	int errno;
	line_num = 0;
	int len = 99;
	char* line = malloc(len);

	file = fopen(input_file, "r");

	if (file == NULL) {
		errno = -1;
		return errno;
	}																										//file is not opened
	else {
		while ( fgets(line, len, file) != NULL) {
			line[strlen(line) - 1] = '\0';
			input_data[line_num] = malloc(len);
			if (line != 0 && input_data[line_num] != 0) strncpy((input_data[line_num]), line, len);
			line_num++;
		}
		errno = 0;
	}
	fclose(file);
	return errno;
}

/* ----------------------------------------------------------------------------------
 * 설명 : 소스 코드를 읽어와 토큰단위로 분석하고 토큰 테이블을 작성하는 함수이다.
 *        패스 1로 부터 호출된다.
 * 매계 : 파싱을 원하는 문자열
 * 반환 : 정상종료 = 0 , 에러 < 0
 * 주의 : my_assembler 프로그램에서는 라인단위로 토큰 및 오브젝트 관리를 하고 있다.
 * ----------------------------------------------------------------------------------
 */
int token_parsing(char* str)
{
	int i;
	char* temp;
	char buf[100] = { 0 };
	strcpy(buf, str);
	struct token_unit tok;
	token_table[token_line] = (token*)malloc(sizeof(token));
	if (buf[0] == '.')
	{
		tok.label = "";
		tok.operator = "";
		int k, j = 0;																	
		for (k = 0; k < MAX_OPERAND; k++)
		{
			*tok.operand[k] = malloc(20);																	// operand 토큰 배열 공간 할당
			memset(tok.operand[k], NULL, 20);																// operand 토큰 배열 초기화
		}
		strcpy(tok.comment, buf);
		*token_table[token_line] = tok;
		token_line++;
		return 0;
	}
	temp = strtok(buf, "\t");
	if (buf[0] != '\t')																						// label 있을때
	{
		tok.label = (char*)calloc(strlen(temp) / sizeof(char), sizeof(char));								// label token 공간 할당
		strcpy(tok.label, temp);																			// label token 입력

		temp = strtok(NULL, "\t");																			// operator 분리
		tok.operator=malloc(sizeof(temp));																	// operator 토큰 공간 할당
		strcpy(tok.operator, temp);																			// operator 토큰 입력													

		temp = strtok(NULL, "\t");																			//다음 토큰 분리

		int isop = search_opcode(tok.operator);																// 다음 토큰이 operand인지 comment인지 없는지 구분
		opcode[token_line] = isop;																			// operator의 index 저장
		if (isop >= 0)																						// operator 가 inst_file에 있는경우
		{
																											// operand 개수 확인 가능
			if (inst_table[isop][0].ops == 0)																// operand가 0개일때 -> 다음 토큰 = comment or 없음 
			{
				if (temp == NULL)																			// 다음 토큰 없을때
				{
					strcpy(tok.comment, "");
					int k, j = 0;																			// operand 분리
					for (k = 0; k < MAX_OPERAND; k++)
					{
						*tok.operand[k] = malloc(20);														// operand 토큰 배열 공간 할당
						memset(tok.operand[k], NULL, 20);													// operand 토큰 배열 초기화
					}
					*token_table[token_line] = tok;
					token_line++;

					return 0;
				}
				else																						// 다음 토큰 = comment
				{
					for (int k = 0; k < MAX_OPERAND; k++)
					{
						*tok.operand[k] = malloc(20);														// operand 토큰 배열 공간 할당
						memset(tok.operand[k], NULL, 20);													// operand 토큰 배열 초기화
					}
					*tok.comment = (char*)calloc(strlen(temp) / sizeof(char), sizeof(char));				// comment 토큰 공간 할당
					strcpy(tok.comment, temp);																// comment 토큰 입력	
					*token_table[token_line] = tok;
					token_line++;
					return 0;
				}

			}
			else																							//다음 토큰 = operand																	
			{
				char* optemp = (char*)calloc(strlen(temp) / sizeof(char), sizeof(char));					// operand 토큰을 세분하기 위한 임시 토큰 공간 할당
				strcpy(optemp, temp);																		// operand 분리를 위한 임시 저장


				temp = strtok(NULL, "	");																	//다음 토큰 분리
				if (temp == NULL)																			// 다음 토큰 없을때
				{
					int k, j = 0;																			// operand 분리
					for (k = 0; k < MAX_OPERAND; k++)
					{
						*tok.operand[k] = malloc(20);														// operand 토큰 배열 공간 할당
						memset(tok.operand[k], NULL, 20);													// operand 토큰 배열 초기화
					}
					optemp = strtok(optemp, ",");															// operand 토큰 분리 및 입력 루틴
					while (optemp != NULL)
					{
						strcpy(tok.operand[j++], optemp);
						optemp = strtok(NULL, ",");
					}
					for (k = 0; k < MAX_OPERAND; k++)
					{
						if (strcmp(tok.operand[k], "") == 0)
							*tok.operand[k] = NULL;
					}
					strcpy(tok.comment, "");
					*token_table[token_line] = tok;
					token_line++;
					return 0;
				}
				else // 다음 토큰 = comment 
				{
					*tok.comment = (char*)calloc(strlen(temp) / sizeof(char), sizeof(char));				// comment 토큰 공간 할당
					strcpy(tok.comment, temp);											  					// comment 토큰 입력

					int k, j = 0;																			// operand 분리
					for (k = 0; k < MAX_OPERAND; k++)
					{
						*tok.operand[k] = malloc(20);														// operand 토큰 배열 공간 할당
						memset(tok.operand[k], NULL, 20);													// operand 토큰 배열 초기화
					}
					optemp = strtok(optemp, ",");															// operand 토큰 분리 및 입력 루틴
					while (optemp != NULL)
					{
						strcpy(tok.operand[j++], optemp);
						optemp = strtok(NULL, ",");
					}
					for (k = 0; k < MAX_OPERAND; k++)
					{
						if (strcmp(token_table[token_line]->operand[k], "") == 0)
							*tok.operand[k] = NULL;
					}
					*token_table[token_line] = tok;
					token_line++;
					return 0;
				}

			}

		}
		else																								// inst_file 에 없는 operator인 경우
		{
			if (temp == NULL)																				// inst_file 에 없는 operator이면서 operand와 comment모두 존재 하지 않을 경우
			{
				strcpy(tok.comment, "");
				int k, j = 0;																				// operand 분리
				for (k = 0; k < MAX_OPERAND; k++)
				{
					*tok.operand[k] = malloc(20);															// operand 토큰 배열 공간 할당
					memset(tok.operand[k], NULL, 20);														// operand 토큰 배열 초기화
				}
				*token_table[token_line] = tok;
				token_line++;
				return 0;
			}
			char* optemp = (char*)calloc(strlen(temp) / sizeof(char), sizeof(char));						// operand 토큰을 세분하기 위한 임시 토큰 공간 할당
			strcpy(optemp, temp);																			// operand 분리를 위한 임시 저장							

			temp = strtok(NULL, "\t");
			if (temp == NULL)																				// comment 없을때
			{
				int k, j = 0;																				// operand 분리
				for (k = 0; k < MAX_OPERAND; k++)
				{
					*tok.operand[k] = malloc(20);
					memset(tok.operand[k], NULL, 20);
				}
				optemp = strtok(optemp, ",");
				while (optemp != NULL)
				{
					strcpy(tok.operand[j++], optemp);
					optemp = strtok(NULL, ",");
				}
				for (k = 0; k < MAX_OPERAND; k++)
				{
					if (strcmp(tok.operand[k], "") == 0)
						*tok.operand[k] = NULL;
				}
				strcpy(tok.comment, "");
				*token_table[token_line] = tok;
				token_line++;
				return 0;
			}
			else																							// comment 있을때
			{
				*tok.comment = (char*)calloc(strlen(temp) / sizeof(char), sizeof(char));
				strcpy(tok.comment, temp);

				int k, j = 0;																				// operand 분리
				for (k = 0; k < MAX_OPERAND; k++)
				{
					*tok.operand[k] = malloc(20);
					memset(tok.operand[k], NULL, 20);
				}
				optemp = strtok(optemp, ",");
				while (optemp != NULL)
				{
					strcpy(tok.operand[j++], optemp);
					optemp = strtok(NULL, ",");
				}
				*token_table[token_line] = tok;
				token_line++;
				return 0;
			}

		}

		*token_table[token_line] = tok;
	}
	else if (buf[0] == '\t')
	{
		tok.label = "";
		tok.operator=(char*)calloc(strlen(temp) / sizeof(char), sizeof(char));								// operator parsing
		strcpy(tok.operator, temp);

		temp = strtok(NULL, "\t");
		int isop = search_opcode(tok.operator);																// 다음 토큰이 operand인지 comment인지 없는지 구분
		opcode[token_line] = isop;																			// operator의 index 저장

		if (isop >= 0)																						//operator 가 inst_file에 있는경우
		{
																											//operand 개수 확인 가능
			if (inst_table[isop][0].ops == 0)																//operand가 0개일때 -> 다음 토큰 = comment or 없음 
			{
				if (temp == NULL)																			// 다음 토큰 없을때
				{
					strcpy(tok.comment, "");
					int k, j = 0;																			// operand 분리
					for (k = 0; k < MAX_OPERAND; k++)
					{
						*tok.operand[k] = malloc(20);														// operand 토큰 배열 공간 할당
						memset(tok.operand[k], NULL, 20);													// operand 토큰 배열 초기화
					}
					*token_table[token_line] = tok;
					token_line++;
					return 0;
				}
				else																						// 다음 토큰 = comment
				{
					strcpy(tok.comment, "");
					int k, j = 0;																			// operand 분리
					for (k = 0; k < MAX_OPERAND; k++)
					{
						*tok.operand[k] = malloc(20);														// operand 토큰 배열 공간 할당
						memset(tok.operand[k], NULL, 20);													// operand 토큰 배열 초기화
					}
					*tok.comment = malloc(sizeof(temp));													// comment 토큰 공간 할당
					strcpy(tok.comment, temp);																// comment 토큰 입력	
					*token_table[token_line] = tok;
					token_line++;
					return 0;
				}

			}
			else																							//다음 토큰 = operand																	
			{
				char* optemp = (char*)calloc(strlen(temp) / sizeof(char), sizeof(char));					// operand 토큰을 세분하기 위한 임시 토큰 공간 할당
				strcpy(optemp, temp);																		// operand 분리를 위한 임시 저장


				temp = strtok(NULL, "\t");																	//다음 토큰 분리
				if (temp == NULL)																			// 다음 토큰 없을때
				{
					int k, j = 0;																			// operand 분리
					for (k = 0; k < MAX_OPERAND; k++)
					{
						*tok.operand[k] = malloc(20);														// operand 토큰 배열 공간 할당
						memset(tok.operand[k], NULL, 20);													// operand 토큰 배열 초기화
					}
					optemp = strtok(optemp, ",");															// operand 토큰 분리 및 입력 루틴
					while (optemp != NULL)
					{
						strcpy(tok.operand[j++], optemp);
						optemp = strtok(NULL, ",");
					}
					for (k = 0; k < MAX_OPERAND; k++)
					{
						if (strcmp(tok.operand[k], "") == 0)
							*tok.operand[k] = NULL;
					}
					strcpy(tok.comment, "");
					*token_table[token_line] = tok;
					token_line++;
					return 0;
				}
				else // 다음 토큰 = comment 
				{
					*tok.comment = (char*)calloc(strlen(temp) / sizeof(char), sizeof(char));				// comment 토큰 공간 할당
					strcpy(tok.comment, temp);											  					// comment 토큰 입력

					int k, j = 0;																			// operand 분리
					for (k = 0; k < MAX_OPERAND; k++)
					{
						*tok.operand[k] = malloc(20);														// operand 토큰 배열 공간 할당
						memset(tok.operand[k], NULL, 20);													// operand 토큰 배열 초기화

					}
					optemp = strtok(optemp, ",");															// operand 토큰 분리 및 입력 루틴
					while (optemp != NULL)
					{
						strcpy(tok.operand[j++], optemp);
						optemp = strtok(NULL, ",");
					}
					for (k = 0; k < MAX_OPERAND; k++)
					{
						if (strcmp(tok.operand[k], "") == 0)
							*tok.operand[k] = NULL;
					}
					*token_table[token_line] = tok;
					token_line++;
					return 0;
				}

			}

		}
		else																								// inst_file 에 없는 operator인 경우
		{
			if (temp == NULL)																				// inst_file 에 없는 operator이면서 operand와 comment모두 존재 하지 않을 경우
			{
				strcpy(tok.comment, "");
				int k, j = 0;																				// operand 분리
				for (k = 0; k < MAX_OPERAND; k++)
				{
					*tok.operand[k] = malloc(20);															// operand 토큰 배열 공간 할당
					memset(tok.operand[k], NULL, 20);														// operand 토큰 배열 초기화
				}
				*token_table[token_line] = tok;
				token_line++;
				return 0;
			}
			char* optemp = (char*)calloc(strlen(temp) / sizeof(char), sizeof(char));						// operand 토큰을 세분하기 위한 임시 토큰 공간 할당
			strcpy(optemp, temp);																			// operand 분리를 위한 임시 저장							

			temp = strtok(NULL, "\t");
			if (temp == NULL)																				// comment 없을때
			{
				int k, j = 0;																				//operand 분리
				for (k = 0; k < MAX_OPERAND; k++)
				{
					*tok.operand[k] = malloc(20);															// operand 토큰 배열 공간 할당
					memset(tok.operand[k], NULL, 20);														// operand 토큰 배열 초기화
				}
				optemp = strtok(optemp, ",");
				while (optemp != NULL)
				{
					strcpy(tok.operand[j++], optemp);
					optemp = strtok(NULL, ",");
				}
				for (k = 0; k < MAX_OPERAND; k++)
				{
					if (strcmp(tok.operand[k], "") == 0)
						*tok.operand[k] = NULL;
				}
				strcpy(tok.comment, "");
				*token_table[token_line] = tok;
				token_line++;
				return 0;
			}
			else																							// comment 있을때
			{
				*tok.comment = (char*)calloc(strlen(temp) / sizeof(char), sizeof(char));
				strcpy(tok.comment, temp);

				int k, j = 0;																				// operand 분리
				for (k = 0; k < MAX_OPERAND; k++)
				{
					*tok.operand[k] = malloc(20);															// operand 토큰 배열 공간 할당
					memset(tok.operand[k], NULL, 20);														// operand 토큰 배열 초기화
				}
				optemp = strtok(optemp, ",");
				while (optemp != NULL)
				{
					strcpy(tok.operand[j++], optemp);
					optemp = strtok(NULL, ",");
				}
				for (k = 0; k < MAX_OPERAND; k++)
				{
					if (strcmp(tok.operand[k], "") == 0)
						*tok.operand[k] = NULL;
				}
				*token_table[token_line] = tok;
				token_line++;
				return 0;
			}

		}
	}
}

/* ----------------------------------------------------------------------------------
 * 설명 : 입력 문자열이 기계어 코드인지를 검사하는 함수이다.
 * 매계 : 토큰 단위로 구분된 문자열
 * 반환 : 정상종료 = 기계어 테이블 인덱스, 에러 < 0
 * 주의 :
 *
 * ----------------------------------------------------------------------------------
 */
int search_opcode(char* str)
{
	int index = 0;
	if (str[0] == "+") {
		for (int i = 1; str[i]; i++) {
			str[i - 1] = str[i];																			// operator 앞에 + 붙어있을 경우 제거
		}
	}
	while (inst_table[index] != NULL) {

		if (strcmp(inst_table[index][0].str, str)==0) {
		 	return index;
		 }
		index++;

	}
	return -1;
}

/* ----------------------------------------------------------------------------------
* 설명 : 어셈블리 코드를 위한 패스1과정을 수행하는 함수이다.
*		   패스1에서는..
*		   1. 프로그램 소스를 스캔하여 해당하는 토큰단위로 분리하여 프로그램 라인별 토큰
*		   테이블을 생성한다.
*
* 매계 : 없음
* 반환 : 정상 종료 = 0 , 에러 = < 0
* 주의 : 현재 초기 버전에서는 에러에 대한 검사를 하지 않고 넘어간 상태이다.
*	  따라서 에러에 대한 검사 루틴을 추가해야 한다.
*
* -----------------------------------------------------------------------------------
*/
static int assem_pass1(void)
{
	int num = 0;
	token_line = 0;
	while (input_data[num] != NULL) {
		int a=token_parsing(input_data[num]);
		if(a<0) return -1;
		num++;
	}
	return 0;

	/* input_data의 문자열을 한줄씩 입력 받아서
	 * token_parsing()을 호출하여 token_unit에 저장
	 */
}

/* ----------------------------------------------------------------------------------
* 설명 : 입력된 문자열의 이름을 가진 파일에 프로그램의 결과를 저장하는 함수이다.
*        여기서 출력되는 내용은 명령어 옆에 OPCODE가 기록된 표(과제 3번) 이다.
* 매계 : 생성할 오브젝트 파일명
* 반환 : 없음
* 주의 : 만약 인자로 NULL값이 들어온다면 프로그램의 결과를 표준출력으로 보내어
*        화면에 출력해준다.
*        또한 과제 3번에서만 쓰이는 함수이므로 이후의 프로젝트에서는 사용되지 않는다.
* -----------------------------------------------------------------------------------
*/
/*void make_opcode_output(char* file_name)
{
	FILE* file;
	line_num = 0;
	if (file_name == NULL) {
		while (token_table[line_num] != NULL) {
			if (token_table[line_num][0].operand[0] != NULL) {
				if (token_table[line_num][0].operand[1] != NULL) {
					if (token_table[line_num][0].operand[2] != NULL) {
						if (opcode[line_num] >= 0) printf("%s\t%s\t%s,%s,%s\t%x\n", token_table[line_num][0].label, token_table[line_num][0].operator, token_table[line_num][0].operand[0], token_table[line_num][0].operand[1], token_table[line_num][0].operand[2], inst_table[opcode[line_num]][0].op);
						else  printf("%s\t%s\t%s,%s,%s\n", token_table[line_num][0].label, token_table[line_num][0].operator, token_table[line_num][0].operand[0], token_table[line_num][0].operand[1], token_table[line_num][0].operand[2]);
					}
					else {
						if (opcode[line_num] >= 0) printf("%s\t%s\t%s,%s\t%x\n", token_table[line_num][0].label, token_table[line_num][0].operator, token_table[line_num][0].operand[0], token_table[line_num][0].operand[1], inst_table[opcode[line_num]][0].op);
						else  printf("%s\t%s\t%s,%s\n", token_table[line_num][0].label, token_table[line_num][0].operator, token_table[line_num][0].operand[0], token_table[line_num][0].operand[1]);
					}
				}
				else {
					if (opcode[line_num] >= 0) printf("%s\t%s\t%s\t%x\n", token_table[line_num][0].label, token_table[line_num][0].operator, token_table[line_num][0].operand[0], inst_table[opcode[line_num]][0].op);
					else  printf("%s\t%s\t%s\n", token_table[line_num][0].label, token_table[line_num][0].operator, token_table[line_num][0].operand[0]);
				}
			}
			else {
				if (opcode[line_num] >= 0) printf("%s\t%s\t\t%x\n", token_table[line_num][0].label, token_table[line_num][0].operator, inst_table[opcode[line_num]][0].op);
				else  printf("%s\t%s\t\n", token_table[line_num][0].label, token_table[line_num][0].operator);
			}

			line_num++;
		}
	}
	else {
		file = fopen(file_name, "w");
		while (token_table[line_num] != NULL) {
			if (token_table[line_num][0].operand[0][0] != NULL) {
				if (token_table[line_num][0].operand[1][0] != NULL) {
					if (token_table[line_num][0].operand[2][0] != NULL) {
						if (opcode[line_num] >= 0) fprintf(file, "%s\t%s\t%s,%s,%s\t%x\n", token_table[line_num][0].label, token_table[line_num][0].operator, token_table[line_num][0].operand[0], token_table[line_num][0].operand[1], token_table[line_num][0].operand[2], inst_table[opcode[line_num]][0].op);
						else  fprintf(file, "%s\t%s\t%s,%s,%s\n", token_table[line_num][0].label, token_table[line_num][0].operator, token_table[line_num][0].operand[0], token_table[line_num][0].operand[1], token_table[line_num][0].operand[2]);
					}
					else {
						if (opcode[line_num] >= 0) fprintf(file, "%s\t%s\t%s,%s\t%x\n", token_table[line_num][0].label, token_table[line_num][0].operator, token_table[line_num][0].operand[0], token_table[line_num][0].operand[1], inst_table[opcode[line_num]][0].op);
						else  fprintf(file, "%s\t%s\t%s,%s\n", token_table[line_num][0].label, token_table[line_num][0].operator, token_table[line_num][0].operand[0], token_table[line_num][0].operand[1]);
					}
				}
				else {
					if (opcode[line_num] >= 0) fprintf(file, "%s\t%s\t%s\t%x\n", token_table[line_num][0].label, token_table[line_num][0].operator, token_table[line_num][0].operand[0], inst_table[opcode[line_num]][0].op);
					else  fprintf(file, "%s\t%s\t%s\n", token_table[line_num][0].label, token_table[line_num][0].operator, token_table[line_num][0].operand[0]);
				}
			}
			else {
				if (token_table[line_num][0].comment[0] == 46) fprintf(file, "%s\n", token_table[line_num][0].comment);
				else {
					if (opcode[line_num] >= 0) fprintf(file, "%s\t%s\t\t%x\n", token_table[line_num][0].label, token_table[line_num][0].operator, inst_table[opcode[line_num]][0].op);
					else  fprintf(file, "%s\t%s\t\n", token_table[line_num][0].label, token_table[line_num][0].operator);
				}
			}
			fflush(file);

			line_num++;
		}
		fclose(file);
	}
	return;
}*/

/* ----------------------------------------------------------------------------------
* 설명 : 입력된 문자열의 이름을 가진 파일에 프로그램의 결과를 저장하는 함수이다.
*        여기서 출력되는 내용은 SYMBOL별 주소값이 저장된 TABLE이다.
* 매계 : 생성할 오브젝트 파일명
* 반환 : 없음
* 주의 : 만약 인자로 NULL값이 들어온다면 프로그램의 결과를 표준출력으로 보내어
*        화면에 출력해준다.
*
* -----------------------------------------------------------------------------------
*/
void make_symtab_output(char* file_name)
{
	/* add your code here */
}

/* ----------------------------------------------------------------------------------
* 설명 : 입력된 문자열의 이름을 가진 파일에 프로그램의 결과를 저장하는 함수이다.
*        여기서 출력되는 내용은 LITERAL별 주소값이 저장된 TABLE이다.
* 매계 : 생성할 오브젝트 파일명
* 반환 : 없음
* 주의 : 만약 인자로 NULL값이 들어온다면 프로그램의 결과를 표준출력으로 보내어
*        화면에 출력해준다.
*
* -----------------------------------------------------------------------------------
*/
void make_literaltab_output(char* filen_ame)
{
	/* add your code here */
}

/* ----------------------------------------------------------------------------------
* 설명 : 어셈블리 코드를 기계어 코드로 바꾸기 위한 패스2 과정을 수행하는 함수이다.
*		   패스 2에서는 프로그램을 기계어로 바꾸는 작업은 라인 단위로 수행된다.
*		   다음과 같은 작업이 수행되어 진다.
*		   1. 실제로 해당 어셈블리 명령어를 기계어로 바꾸는 작업을 수행한다.
* 매계 : 없음
* 반환 : 정상종료 = 0, 에러발생 = < 0
* 주의 :
* -----------------------------------------------------------------------------------
*/
static int assem_pass2(void)
{

	/* add your code here */
}

/* ----------------------------------------------------------------------------------
* 설명 : 입력된 문자열의 이름을 가진 파일에 프로그램의 결과를 저장하는 함수이다.
*        여기서 출력되는 내용은 object code (프로젝트 1번) 이다.
* 매계 : 생성할 오브젝트 파일명
* 반환 : 없음
* 주의 : 만약 인자로 NULL값이 들어온다면 프로그램의 결과를 표준출력으로 보내어
*        화면에 출력해준다.
*
* -----------------------------------------------------------------------------------
*/
void make_objectcode_output(char* file_name)
{
	/* add your code here */
}
