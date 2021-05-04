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

	
	make_symtab_output("C:\\SP_Parser\\SP\\symtab_20171281.txt");																// symboltable 출력
	//make_symtab_output(NULL);
	//make_literaltab_output(NULL);
	make_literaltab_output("C:\\SP_Parser\\SP\\literaltab_20171281.txt");														// literaltable 출력
	if (assem_pass2() < 0)
	{
		printf(" assem_pass2: 패스2 과정에서 실패하였습니다.  \n");
		return -1;
	}

	make_objectcode_output("C:\\SP_Parser\\SP\\output_20171281.txt");															// 최종결과 출력
	
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

	if ((result = init_inst_file("C:\\SP_Parser\\SP\\inst.data")) < 0)
		return -1;
	if ((result = init_input_file("C:\\SP_Parser\\SP\\input.txt")) < 0)
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
	fopen_s(&file, inst_file, "r");
	inst_index = 0;
	

	if (file == NULL) 
	{ 
		errno = -1; 
		return errno;
	}																										// file is not opened

	else 
	{
		while (fscanf(file, "%[^\n]\n", line) != EOF) 
		{
			struct inst_unit in = { 0 };

			char* token;
			int a = 0;
			token = strtok(line, " ");
			inst_table[inst_index] = malloc(sizeof(inst));

			strncpy(in.str, token, sizeof(token));															// instruction 저장
			token = strtok(NULL, " ");

			in.format1 = *token - 48;																		// format1 저장
			token = strtok(NULL, " ");

			in.format2 = *token - 48;																		// format2 저장, format이 한 개면 0 저장
			token = strtok(NULL, " ");

																											// opcode 저장
			if(token[2] == 'A' || token[2] == 'B' || token[2] == 'C' || token[2] == 'D' || token[2] == 'E' || token[2] == 'F')
				in.op = (token[2] - 55) << 4;
			else if (token[2] > 47 && token[2] < 58)
				in.op = (token[2] - 48) << 4;

			if (token[3] == 'A' || token[3] == 'B' || token[3] == 'C' || token[3] == 'D' || token[3] == 'E' || token[3] == 'F')
				in.op += (token[3] - 55);
			else if (token[3] > 47 && token[3] < 58)
				in.op += (token[3] - 48);
			token = strtok(NULL, " ");

			in.ops = *token - 48;																			// operands 개수 저장
			token = strtok(NULL, " ");


			*inst_table[inst_index] = in;																	// inst_table에 추가

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

	fopen_s(&file, input_file, "r");

	if (file == NULL) 
	{
		errno = -1;
		return errno;
	}																										// file is not opened
	else 
	{
		while ( fgets(line, len, file) != NULL) 
		{
			line[strlen(line) - 1] = '\0';
			input_data[line_num] = malloc(len);
			if (line != 0 && input_data[line_num] != 0) strncpy((input_data[line_num]), line, len);			// input_data table에 저장
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
	int i, j = 0;
	char* temp;
	char buf[100] = { 0 };
	if (str == NULL) 
	{ 
		printf("token_parsing: fail to get input string\n"); 
		return -1;
	}
	strcpy(buf, str);
	struct token_unit tok;
	tok.label = NULL;
	tok.operator = NULL;
	tok.index_num = -1;
	tok.nixbpe = NULL;
	for (i = 0; i < MAX_OPERAND; i++)
	{
		strcpy(tok.operand[i], "");
	}
	strcpy(tok.comment, "");
	token_table[token_line] = (token*)malloc(sizeof(token));
	if (buf[0] == '.')
	{
		strcpy(tok.comment, buf);
		*token_table[token_line] = tok;																		// token_table에 저장
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

		tok.index_num = search_opcode(tok.operator);														// 다음 토큰이 operand인지 comment인지 없는지 구분
		if (tok.index_num >= 0)																				// operator 가 inst_file에 있는경우
		{
																											// operand 개수 확인 가능
			if (inst_table[tok.index_num][0].ops == 0)														// operand가 0개일때 -> 다음 토큰 = comment or 없음 
			{
				if (temp == NULL)																			// 다음 토큰 없을때
				{
					*token_table[token_line] = tok;															// token_table에 저장
					token_line++;
					return 0;
				}
				else																						// 다음 토큰 = comment
				{
					strcpy(tok.comment, temp);																// comment 토큰 입력	
					*token_table[token_line] = tok;															// token_table에 저장
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
					optemp = strtok(optemp, ",");															// operand 토큰 분리 및 입력 루틴
					while (optemp != NULL)
					{
						strcpy(tok.operand[j++], optemp);													// operand 입력
						optemp = strtok(NULL, ",");
					}
					
					*token_table[token_line] = tok;															// token_table에 저장
					token_line++;
					return 0;
				}
				else																						// 다음 토큰 = comment 
				{
					strcpy(tok.comment, temp);											  					// comment 토큰 입력

					optemp = strtok(optemp, ",");															// operand 토큰 분리 및 입력 루틴
					while (optemp != NULL)
					{
						strcpy(tok.operand[j++], optemp);													// operand 입력
						optemp = strtok(NULL, ",");
					}
					
					*token_table[token_line] = tok;															// token_table에 저장
					token_line++;
					return 0;
				}

			}

		}
		else																								// inst_file 에 없는 operator인 경우
		{
			if (temp == NULL)																				// inst_file 에 없는 operator이면서 operand와 comment모두 존재 하지 않을 경우
			{
				*token_table[token_line] = tok;																// token_table에 저장
				token_line++;
				return 0;
			}
			char* optemp = (char*)calloc(strlen(temp) / sizeof(char), sizeof(char));						// operand 토큰을 세분하기 위한 임시 토큰 공간 할당
			strcpy(optemp, temp);																			// operand 분리를 위한 임시 저장							

			temp = strtok(NULL, "\t");
			if (temp == NULL)																				// comment 없을때
			{
				optemp = strtok(optemp, ",");
				while (optemp != NULL)
				{
					strcpy(tok.operand[j++], optemp);														// operand 입력
					optemp = strtok(NULL, ",");
				}
				
				*token_table[token_line] = tok;																// token_table에 저장
				token_line++;
				return 0;
			}
			else																							// comment 있을때
			{
				strcpy(tok.comment, temp);

				optemp = strtok(optemp, ",");
				while (optemp != NULL)
				{
					strcpy(tok.operand[j++], optemp);														// operand 입력
					optemp = strtok(NULL, ",");
				}
				*token_table[token_line] = tok;																// token_table에 저장
				token_line++;
				return 0;
			}
		}
	}
	else if (buf[0] == '\t')																				// label 없을 때
	{
		tok.label = NULL;
		tok.operator=(char*)calloc(strlen(temp) / sizeof(char), sizeof(char));								// operator parsing
		strcpy(tok.operator, temp);

		temp = strtok(NULL, "\t");
		tok.index_num = search_opcode(tok.operator);														// 다음 토큰이 operand인지 comment인지 없는지 구분

		if (tok.index_num >= 0)																				//operator 가 inst_file에 있는경우
		{
																											//operand 개수 확인 가능
			if (inst_table[tok.index_num][0].ops == 0)														//operand가 0개일때 -> 다음 토큰 = comment or 없음 
			{
				if (temp == NULL)																			// 다음 토큰 없을때
				{
					*token_table[token_line] = tok;															// token_table에 저장
					token_line++;
					return 0;
				}
				else																						// 다음 토큰 = comment
				{
					strcpy(tok.comment, temp);																// comment 토큰 입력	
					*token_table[token_line] = tok;															// token_table에 저장
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
					optemp = strtok(optemp, ",");															// operand 토큰 분리 및 입력 루틴
					while (optemp != NULL)
					{
						strcpy(tok.operand[j++], optemp);													// operand 입력
						optemp = strtok(NULL, ",");
					}
					
					*token_table[token_line] = tok;															// token_table에 저장
					token_line++;
					return 0;
				}
				else																						// 다음 토큰 = comment 
				{
					strcpy(tok.comment, temp);											  					// comment 토큰 입력

					optemp = strtok(optemp, ",");															// operand 토큰 분리 및 입력 루틴
					while (optemp != NULL)
					{
						strcpy(tok.operand[j++], optemp);													// operand 입력
						optemp = strtok(NULL, ",");
					}
					
					*token_table[token_line] = tok;															// token_table에 저장
					token_line++;
					return 0;
				}

			}

		}
		else																								// inst_file 에 없는 operator인 경우
		{
			if (temp == NULL)																				// inst_file 에 없는 operator이면서 operand와 comment모두 존재 하지 않을 경우
			{
				*token_table[token_line] = tok;																// token_table에 저장
				token_line++;
				return 0;
			}
			char* optemp = (char*)calloc(strlen(temp) / sizeof(char), sizeof(char));						// operand 토큰을 세분하기 위한 임시 토큰 공간 할당
			strcpy(optemp, temp);																			// operand 분리를 위한 임시 저장							

			temp = strtok(NULL, "\t");
			if (temp == NULL)																				// comment 없을때
			{
				optemp = strtok(optemp, ",");
				while (optemp != NULL)
				{
					strcpy(tok.operand[j++], optemp);														// operand 저장
					optemp = strtok(NULL, ",");
				}
	
				*token_table[token_line] = tok;																// token_table에 저장
				token_line++;
				return 0;
			}
			else																							// comment 있을때
			{
				strcpy(tok.comment, temp);

				optemp = strtok(optemp, ",");
				while (optemp != NULL)
				{
					strcpy(tok.operand[j++], optemp);														// operand 저장
					optemp = strtok(NULL, ",");
				}
				
				*token_table[token_line] = tok;																// token_table에 저장
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
	char* string = calloc(10, sizeof(char));
	strcpy(string, str);
	if (strncmp(string,"+",1)==0) 
	{
		for (int i = 1; string[i]; i++) 
		{
			string[i - 1] = string[i];																		// operator 앞에 + 붙어있을 경우 제거
			string[i] = NULL;
		}
	}
	while (inst_table[index] != NULL) 
	{

		if (strcmp(inst_table[index][0].str, string)==0) 
		 	return index;

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
	int num = 0;																							// input_data와 token_table을 위한 인덱스용 변수 선언
	token_line = 0;
	int a = 0;																								// 주소값 저장을 위한 변수 선언
	int check = 0;
	while (num<line_num) {
		sym_table[num] = (symbol*)calloc(1, sizeof(symbol));
		literal_table[num] = (literal*)calloc(1, sizeof(literal));
		literal_table[num]->literal = NULL;
		if(token_parsing(input_data[num])<0) return -1;
		if (token_table[num]->operator == NULL)
		{
			num++;
			continue;
		}

		if (num == 0)
		{
			token_table[num]->addr = a;																		// 시작주소 저장
		}

		else if (strcmp(token_table[num]->operator, "CSECT") == 0)											// operator가 CSECT인 경우
		{
			endaddr[check] = a;
			a = 0;
			token_table[num]->addr = a;
			check++;
		}
		else if (strcmp(token_table[num]->operator, "RESW") == 0)											// operator가 RESW인 경우
		{
			token_table[num]->addr = a;
			a += 3*atoi(token_table[num]->operand[0]);
		}
		else if (strcmp(token_table[num]->operator, "RESB") == 0)											// operator가 RESB인 경우
		{
			token_table[num]->addr = a;
			a += atoi(token_table[num]->operand[0]);
		}
		else if (strcmp(token_table[num]->operator, "EQU") == 0)											// operator가 EQU인 경우
		{
			if (strcmp(token_table[num]->operand[0], "*") == 0)												// operand가 없는 경우
			{
				token_table[num]->addr = a;
			}
			else																							// operand가 있는 경우
			{
				char* ch = calloc(20, sizeof(char));
				char* temp;
				int n1 = 0;
				int n2 = 0;
				strcpy(ch, token_table[num]->operand[0]);
				temp = strtok(ch, "-");
				for (int i = 0; i < num; i++)
				{
					if (strcmp(temp, sym_table[i]->symbol) == 0)
						n1 = sym_table[i]->addr;
				}
				temp = strtok(NULL, "-");
				for (int i = 0; i < num; i++)
				{
					if (sym_table[i]->symbol[0] != 0 && strcmp(temp, sym_table[i]->symbol) == 0)
						n2 = sym_table[i]->addr;
				}
				token_table[num]->addr = n1 - n2;
			}
		}
		else if (strcmp(token_table[num]->operator, "BYTE") == 0)											// operator가 BYTE인 경우
		{
			token_table[num]->addr = a;
			a += 1;

		}
		else if (strcmp(token_table[num]->operator, "WORD") == 0)											// operator가 WORD인 경우
		{
			token_table[num]->addr = a;
			char* ch = calloc(20, sizeof(char));
			char* temp;
			int n1 = 0;
			int n2 = 0;
			strcpy(ch, token_table[num]->operand[0]);
			temp = strtok(ch, "-");
			for (int i = 0; i < num; i++)
			{
				if (strcmp(temp, sym_table[i]->symbol) == 0)
					n1 = sym_table[i]->addr;
			}
			temp = strtok(NULL, "-");
			for (int i = 0; i < num; i++)
			{
				if (strcmp(temp, sym_table[i]->symbol) == 0)
					n2 = sym_table[i]->addr;
			}
			a += n1 - n2;

		}
		else if (strcmp(token_table[num]->operator, "LTORG") == 0 || num == line_num-1)						// operator가 LTORG이거나 input이 끝나는 경우
		{
			for (int i = 0; i < num; i++)
			{
				if (literal_table[i]->literal != NULL && literal_table[i]->addr == 0)
				{
					literal_table[i]->addr = a;
					a += literal_table[i]->length;
				}
			}
		}
		else if (token_table[num]->index_num >0 || strcmp(token_table[num]->operator, "LDA") == 0)			// operator가 instruction인 경우
		{
			token_table[num]->addr = a;																		
			if(inst_table[token_table[num]->index_num]->format1 == 1)										// 1형식
			{ 
				a += 1;
			}
			else if (inst_table[token_table[num]->index_num]->format1 == 2)									// 2형식
			{ 
				a += 2;
			}
			else if (inst_table[token_table[num]->index_num]->format1 == 3)
			{ 
				if (strncmp(token_table[num]->operator, "+",1) == 0)										// 4형식
				{
					a += 4;
				}
				else																						// 3형식
				{
					a += 3;
				}
				
			}
			if (strncmp(token_table[num]->operand[0], "=", 1) == 0)											// literal이 있는  경우
			{
				char* ch = calloc(20, sizeof(char));
				char* temp;
				int hex = 0;
				int check = 0;
				strcpy(ch, token_table[num]->operand[0]);
				temp = strtok(ch, "'");
				if (strncmp(temp, "=X", 2) == 0)
					hex = 1;

				temp = strtok(NULL, "'");
				for (int i = 0; i < num; i++)
				{
					if (literal_table[i]->literal != NULL && strcmp(literal_table[i]->literal, temp) == 0)
						check = 1;
				}
				if (hex == 0 && check == 0)
				{
					literal_table[num]->literal = (char *)calloc(10, sizeof(char));
					strcpy(literal_table[num]->literal, temp);
					literal_table[num]->length = strlen(temp);
				}
				else if (hex == 1 && check == 0)
				{
					literal_table[num]->literal = (char*)calloc(10, sizeof(char));
					strcpy(literal_table[num]->literal, temp);
					literal_table[num]->length = 1;
				}


			}
		}
		token_table[num]->csect = check;
		if (token_table[num]->label !=NULL)																	// symbol table 만들기
		{
			strcpy(sym_table[num]->symbol, token_table[num]->label);
			sym_table[num]->addr = token_table[num]->addr;
			sym_table[num]->csect = token_table[num]->csect;
		}
		num++;
	}

	return 0;

}


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
	FILE* file;
	int num = 0;
	if (file_name == NULL)																					// file명을 받지 않은 경우
	{
		while (num<line_num) 
		{
			if (sym_table[num]->symbol[0] != 0)
				printf("%s\t\t\t%x\n", sym_table[num]->symbol, sym_table[num]->addr);
			num++;
		}
		printf("\n");
	}
	else {																									// file명을 받은 경우
		file = fopen(file_name, "w");
		while (num<line_num) 
		{
			if (sym_table[num]->symbol[0] != 0)
				fprintf(file, "%s\t\t\t%x\n", sym_table[num]->symbol, sym_table[num]->addr);
			num++;
		}
		fflush(file);
		fclose(file);
	}
	return;
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
	FILE* file;
	int num = 0;
	if (filen_ame == NULL)																					// file명을 받지 않은 경우
	{
		while (num < line_num)
		{
			if (literal_table[num]->literal != NULL)
				printf("%s			%x\n", literal_table[num]->literal, literal_table[num]->addr);
			num++;
		}
	}
	else																									// file명을 받은 경우
	{
		file = fopen(filen_ame, "w");
		while (num < line_num)
		{
			if (literal_table[num]->literal != NULL)
				fprintf(file, "%s			%x\n", literal_table[num]->literal, literal_table[num]->addr);
			num++;
		}
		fclose(file);
	}
	return;
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
	locctr = 0;
	int num = 0;
	while (num < line_num)
	{
		object_table[num] = (object*)calloc(1, sizeof(object));
		modify_table[num] = (modify*)calloc(1, sizeof(modify));
		modify_table[num]->symbol[0] = NULL;
		modify_table[num]->symbol[1] = NULL;
		if (token_table[num]->index_num >= 0)
		{
			if (inst_table[token_table[num]->index_num]->format1 == 1)			
			{																								// 1형식 
				object_table[num]->length = 1;
				object_table[num]->code = calloc(object_table[num]->length, sizeof(unsigned char));
				object_table[num]->code[0] = inst_table[token_table[num]->index_num]->op;
			}
			else if (inst_table[token_table[num]->index_num]->format1 == 2)
			{																								// 2형식
				object_table[num]->length = 2;
				object_table[num]->code = calloc(object_table[num]->length, sizeof(unsigned char));
				object_table[num]->code[0] = inst_table[token_table[num]->index_num]->op;
				if (token_table[num]->operand[0][0] != 0)
				{
					if (strcmp(token_table[num]->operand[0], "A") == 0) object_table[num]->code[1] += 0 << 4;
					else if (strcmp(token_table[num]->operand[0], "X") == 0) object_table[num]->code[1] += 1 << 4;
					else if (strcmp(token_table[num]->operand[0], "L") == 0) object_table[num]->code[1] += 2 << 4;
					else if (strcmp(token_table[num]->operand[0], "B") == 0) object_table[num]->code[1] += 3 << 4;
					else if (strcmp(token_table[num]->operand[0], "S") == 0) object_table[num]->code[1] += 4 << 4;
					else if (strcmp(token_table[num]->operand[0], "T") == 0) object_table[num]->code[1] += 5 << 4;
					else if (strcmp(token_table[num]->operand[0], "F") == 0) object_table[num]->code[1] += 6 << 4;
					else if (strcmp(token_table[num]->operand[0], "PC") == 0) object_table[num]->code[1] += 8 << 4;
					else if (strcmp(token_table[num]->operand[0], "SW") == 0) object_table[num]->code[1] += 9 << 4;
					else if (strncmp(token_table[num]->operand[0], "#", 1) == 0)
					{
						char* temp;
						temp = token_table[num]->operand[0];
						temp = strtok(temp, "#");
						object_table[num]->code[1] += atoi(temp);
					}
				}
				if (token_table[num]->operand[1][0] != 0)
				{
					if (strcmp(token_table[num]->operand[1], "A") == 0) object_table[num]->code[1] += 0;
					else if (strcmp(token_table[num]->operand[1], "X") == 0) object_table[num]->code[1] += 1;
					else if (strcmp(token_table[num]->operand[1], "L") == 0) object_table[num]->code[1] += 2;
					else if (strcmp(token_table[num]->operand[1], "B") == 0) object_table[num]->code[1] += 3;
					else if (strcmp(token_table[num]->operand[1], "S") == 0) object_table[num]->code[1] += 4;
					else if (strcmp(token_table[num]->operand[1], "T") == 0) object_table[num]->code[1] += 5;
					else if (strcmp(token_table[num]->operand[1], "F") == 0) object_table[num]->code[1] += 6;
					else if (strcmp(token_table[num]->operand[1], "PC") == 0) object_table[num]->code[1] += 8;
					else if (strcmp(token_table[num]->operand[1], "SW") == 0) object_table[num]->code[1] += 9;
				}

			}
			else if (inst_table[token_table[num]->index_num]->format1 == 3 && strncmp(token_table[num]->operator, "+", 1) != 0)
			{																								// 3형식
				object_table[num]->length = 3;
				object_table[num]->code = (unsigned char*)calloc(object_table[num]->length, sizeof(unsigned char));
				object_table[num]->code[0] = inst_table[token_table[num]->index_num]->op;
				if (strcmp(token_table[num]->operator, "RSUB") == 0)
				{
					token_table[num]->nixbpe = 0b00110000;
					object_table[num]->code[0] += token_table[num]->nixbpe >> 4;
				}

				else if (token_table[num]->operand[0][0] == '#')											// nixbpe = 010000, immediate
				{
					token_table[num]->nixbpe = 0b00010000;
					object_table[num]->code[0] += token_table[num]->nixbpe / 0x10;
					object_table[num]->code[1] += (token_table[num]->nixbpe % 0x10) * 0x10;
					char* temp;
					temp = token_table[num]->operand[0];
					temp = strtok(temp, "#");
					object_table[num]->code[1] += atoi(temp) / 0x100;
					object_table[num]->code[2] += atoi(temp) % 0x100;

				}
				else if (token_table[num]->operand[0][0] == '@')											// nixbpe = 100010, indirect
				{
					token_table[num]->nixbpe = 0b00100010;
					object_table[num]->code[0] += token_table[num]->nixbpe >> 4;
					object_table[num]->code[1] += token_table[num]->nixbpe << 4;
					char* temp = token_table[num]->operand[0];
					temp = strtok(temp, "@");
					for (int i = 0; i < token_line; i++)
					{

						if (!strcmp(sym_table[i]->symbol, temp) && sym_table[i]->csect == token_table[num]->csect)
						{
							unsigned int addr = sym_table[i]->addr - token_table[num + 1]->addr;
							object_table[num]->code[1] += addr >> 0x8 & 0xF;
							object_table[num]->code[2] += addr & 0xFF;
							break;
						}
						else if (!strcmp(sym_table[i]->symbol, temp) 
							&& sym_table[i]->csect != token_table[num]->csect)
						{
							modify_table[num]->symbol[0] = calloc(10, sizeof(char));
							strcpy(modify_table[num]->symbol[0], sym_table[i]->symbol);
							modify_table[num]->addr[0] = token_table[num]->addr;
							break;
						}
					}
				}
				else																						// ni = 11, simple
				{
					if (strcmp(token_table[num]->operand[1], "X") == 0) 
						token_table[num]->nixbpe += 0b00001000;												// x = 1
					token_table[num]->nixbpe += 0b00110010;													// ni = 11, p = 1
					object_table[num]->code[0] += token_table[num]->nixbpe >> 4;
					object_table[num]->code[1] += token_table[num]->nixbpe << 4;
					for (int i = 0; i < line_num; i++)
					{
						if (!strcmp(sym_table[i]->symbol, token_table[num]->operand[0])
							&& sym_table[i]->csect == token_table[num]->csect)
						{
							unsigned int addr = sym_table[i]->addr - token_table[num + 1]->addr;
							object_table[num]->code[1] += addr >> 0x8 & 0xF;
							object_table[num]->code[2] += addr & 0xFF;
							break;
						}
						else if (!strcmp(sym_table[i]->symbol, token_table[num]->operand[0])
							&& sym_table[i]->csect != token_table[num]->csect)
						{
							modify_table[num]->symbol[0] = calloc(10, sizeof(char));
							strcpy(modify_table[num]->symbol[0], sym_table[i]->symbol);
							modify_table[num]->addr[0] = token_table[num]->addr;
							break;
						}
						else if (token_table[num]->operand[0][0] == '=')
						{
							char* temp = calloc(10, sizeof(char));
							strcpy(temp, token_table[num]->operand[0]);
							temp = strtok(temp, "'");
							temp = strtok(NULL, "'");
							
							if ((literal_table[i]->literal) != NULL && !strcmp(literal_table[i]->literal, temp))
							{
								unsigned int addr = literal_table[i]->addr - token_table[num + 1]->addr;
								object_table[num]->code[1] += addr >> 0x8 & 0xF;
								object_table[num]->code[2] += addr & 0xFF;
								break;
							}
						}
					}

				}
			}
			else if (inst_table[token_table[num]->index_num]->format1 == 3 && strncmp(token_table[num]->operator, "+", 1) == 0)
			{																								// 4형식
				object_table[num]->length = 4;
				object_table[num]->code = calloc(object_table[num]->length, sizeof(unsigned char));
				object_table[num]->code[0] = inst_table[token_table[num]->index_num]->op;
				token_table[num]->nixbpe = 0b00110001;
				if (strcmp(token_table[num]->operand[1], "X") == 0)
					token_table[num]->nixbpe += 0b00001000;												// x = 1
				object_table[num]->code[0] += token_table[num]->nixbpe >> 4;
				object_table[num]->code[1] += token_table[num]->nixbpe << 4;
				for (int i = 0; i < line_num; i++)
				{
					if (!strcmp(sym_table[i]->symbol, token_table[num]->operand[0])
						&& sym_table[i]->csect == token_table[num]->csect)
					{
						unsigned int addr = sym_table[i]->addr - token_table[num + 1]->addr;
						object_table[num]->code[1] += addr >> 0x8 & 0xF;
						object_table[num]->code[2] += (addr & 0xFF00) >> 8;
						object_table[num]->code[3] += addr & 0xFF;
						break;
					}
					else if (!strcmp(sym_table[i]->symbol, token_table[num]->operand[0])
						&& sym_table[i]->csect != token_table[num]->csect)
					{
						modify_table[num]->symbol[0] = calloc(10, sizeof(char));
						strcpy(modify_table[num]->symbol[0], sym_table[i]->symbol);
						modify_table[num]->addr[0] = token_table[num]->addr;
						break;
					}
					else if (strncmp(token_table[num]->operand[0], "=", 1) == 0)
					{
						char* temp;
						temp = token_table[num]->operand[0];
						temp = strtok(temp, "'");
						for (int j = 0; j < line_num; j++)
						{
							if (strcmp(literal_table[j]->literal, temp) == 0)
							{
								unsigned int addr = literal_table[i]->addr - token_table[num + 1]->addr;
								object_table[num]->code[1] += addr >> 0x8 & 0xF;
								object_table[num]->code[2] += (addr & 0xFF00) >> 8;
								object_table[num]->code[3] += addr & 0xFF;
								break;
							}

						}
					}
				}

			}
		}
		else
		{
			if (!strcmp(token_table[num]->operator, "BYTE"))
			{
				char* temp = calloc(10, sizeof(char));
				strcpy(temp, token_table[num]->operand[0]);
				temp = strtok(temp, "'");
				temp = strtok(NULL, "'");
				object_table[num]->length = 1;
				object_table[num]->code = calloc(object_table[num]->length, sizeof(unsigned char));
				if (temp[0] == 'A' || temp[0] == 'B' || temp[0] == 'C' || temp[0] == 'D' || temp[0] == 'E' || temp[0] == 'F')
					object_table[num]->code[0] = (temp[0] - 55) << 4;
				else if (temp[0] > 47 && temp[0] < 58)
					object_table[num]->code[0] = (temp[0] - 48) << 4;

				if (temp[1] == 'A' || temp[1] == 'B' || temp[1] == 'C' || temp[1] == 'D' || temp[1] == 'E' || temp[1] == 'F')
					object_table[num]->code[0] = (temp[0] - 55) << 4;
				else if (temp[1] > 47 && temp[1] < 58)
					object_table[num]->code[0] = (temp[1] - 48) << 4;
			}
			else if (!strcmp(token_table[num]->operator, "WORD"))
			{
				char* temp = calloc(10, sizeof(char));
				object_table[num]->length = 3;
				object_table[num]->code = calloc(object_table[num]->length, sizeof(unsigned char));
				strcpy(temp, token_table[num]->operand[0]);
				temp = strtok(temp, "-");
				unsigned int addr = 0;
				for (int i = 0; i < line_num; i++)
				{
					if (!strcmp(sym_table[i]->symbol, temp)
						&& sym_table[i]->csect == token_table[num]->csect)
					{
						addr = sym_table[i];
						break;
					}
					else if (!strcmp(sym_table[i]->symbol, temp)
						&& sym_table[i]->csect != token_table[num]->csect)
					{
						modify_table[num]->symbol[0] = calloc(10, sizeof(char));
						strcpy(modify_table[num]->symbol[0], sym_table[i]->symbol);
						modify_table[num]->addr[0] = token_table[num]->addr;
						break;
					}
				}
				temp = strtok(NULL, "-");
				for (int i = 0; i < line_num; i++)
				{
					if (!strcmp(sym_table[i]->symbol, temp)
						&& sym_table[i]->csect == token_table[num]->csect)
					{
						addr -= sym_table[i]->addr;
						object_table[num]->code[0] += (addr & 0xff0000) >> 0x10;
						object_table[num]->code[1] += (addr & 0xFF00) >> 8;
						object_table[num]->code[2] += addr & 0xFF;
						break;
					}
					else if (!strcmp(sym_table[i]->symbol, temp)
						&& sym_table[i]->csect != token_table[num]->csect)
					{
						modify_table[num]->symbol[1] = calloc(10, sizeof(char));
						strcpy(modify_table[num]->symbol[1], sym_table[i]->symbol);
						modify_table[num]->addr[1] = token_table[num]->addr;
						break;
					}
				}
			}
		}
		printf("%d: ", num);
		for (int loop = 0; loop < object_table[num]->length; loop++) {
			printf("%02x", object_table[num]->code[loop]);
		}
		printf("\n");
		num++;
	}

	return locctr;
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
	FILE* file;
	file = fopen(file_name, "w");
	int num = 0;
	int csect = 0;
	int mcheck = 0;
	int tcheck = 0;
	char H[3][20] = { 0 };																			// H 줄 저장할 문자열 정의
	char D[70] = { 0 };																				// D 줄 저장할 문자열 정의
	char R[3][70] = { 0 };																			// R 줄 저장할 문자열 정의
	char T[3][5][70] = { 0 };																		// T 줄 저장할 문자열 정의
	char M[3][4][16] = { 0 };																		// M 줄 저장할 문자열 정의
	char E[3][7] = { 0 };																			// E 줄 저장할 문자열 정의
	while (csect < 3)
	{
		// H줄
		
		if (!strcmp(token_table[num]->operator, "START") || !strcmp(token_table[num]->operator, "CSECT"))
		{
			char start[6] = { 0 };
			int start_addr = 0;
			int end_addr = 0;
			strcpy(start, token_table[num]->label);
			start_addr = token_table[num]->addr;
			end_addr = endaddr[csect];
			sprintf(H[csect], "H%-6s%06X%06X\n", token_table[num]->label, start_addr, end_addr);
			csect = token_table[num]->csect;
			continue;
		}

		//D줄
		if (!strcmp(token_table[num]->operator, "EXTDEF"))
		{
			int i = 0;
			sprintf(D[csect], "D");
			while(i<3 || token_table[num]->operand[i][0] != 0)
			{
				for (int j = 0; j < line_num; j++)
				{
					if (!strcmp(token_table[num]->operand[i], sym_table[j]->symbol))
					{
						sprintf(D[csect] + strlen(D[csect]), "%s%06X", sym_table[j]->symbol, sym_table[j]->addr);
					}
				}
				i++;
			}
			sprintf(D[csect] + strlen(D[csect]), "\n");
			continue;
		}

		//R줄
		if (!strcmp(token_table[num]->operator, "EXTREF"))
		{
			int i = 0;
			sprintf(R[csect], "R");
			while (i < 3 || token_table[num]->operand[i][0] != 0)
			{
				for (int j = 0; j < line_num; j++)
				{
					if (!strcmp(token_table[num]->operand[i], sym_table[j]->symbol))
					{
						sprintf(R[csect] + strlen(R[csect]), "%-6s", sym_table[j]->symbol);
					}
				}
				i++;
			}
			sprintf(R[csect] + strlen(R[csect]), "\n");
			continue;
		}

		//T줄


		//M줄
		if (modify_table[num]->symbol[0] != NULL)
		{
			sprintf(M[csect][mcheck], "M");
			if (modify_table[num]->symbol[1] != NULL)
			{
				sprintf(M[csect][mcheck] + strlen(M[csect][mcheck]), "%06X", modify_table[num]->addr[0]);
				sprintf(M[csect][mcheck] + strlen(M[csect][mcheck]), "06+");
				sprintf(M[csect][mcheck] + strlen(M[csect][mcheck]), "%s", modify_table[num]->symbol[0]);

				mcheck++;
				sprintf(M[csect][mcheck] + strlen(M[csect][mcheck]), "%06X", modify_table[num]->addr[1]);
				sprintf(M[csect][mcheck] + strlen(M[csect][mcheck]), "06-");
				sprintf(M[csect][mcheck] + strlen(M[csect][mcheck]), "%s", modify_table[num]->symbol[1]);

			}
			else
			{
				sprintf(M[csect][mcheck] + strlen(M[csect][mcheck]), "%06X", modify_table[num]->addr[0]);
				if(!strncmp(token_table[num]->operator, "+", 1))
					sprintf(M[csect][mcheck] + strlen(M[csect][mcheck]), "05+");
				else
					sprintf(M[csect][mcheck] + strlen(M[csect][mcheck]), "03+");
				sprintf(M[csect][mcheck] + strlen(M[csect][mcheck]), "%s", modify_table[num]->symbol[0]);
			}
			sprintf(M[csect][mcheck] + strlen(M[csect][mcheck]), "\n");
			mcheck++;
		}

		//E줄 
		if (csect == 0)
		{
			sprintf(E[csect], "E000000\n");
		}
		else
		{
			sprintf(E[csect], "E\n");
		}
		num++;
	}

}
