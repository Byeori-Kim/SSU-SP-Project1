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
// #define _CRT_SECURE_NO_WARNUNGS
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

	make_opcode_output("output_20171281.txt");

	/*
	 * 추후 프로젝트에서 사용되는 부분
	 *
	make_symtab_output("symtab_20171281");
	if (assem_pass2() < 0)
	{
		printf(" assem_pass2: 패스2 과정에서 실패하였습니다.  \n");
		return -1;
	}

	make_objectcode_output("output_20171281");
	*/
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
	memset(&inst_table, 0, sizeof(inst_table));
	inst_index = 0;

	if (file == NULL) errno = -1;  // file is not opened

	else {
		while (fscanf(file, "%[^\n]\n", line) != EOF) {
			struct inst_unit in = { 0 };

			char* token;
			int a = 0;
			token = strtok(line, " ");

			//printf("[%d]: %-6s  ", a++, token);
			strncpy(in.str, token, sizeof(token) - 1);
			token = strtok(NULL, " ");

			//printf("[%d]: %-6s  ", a++, token);
			in.format1 = *token - 48;
			token = strtok(NULL, " ");

			//printf("[%d]: %-6s  ", a++, token);
			in.format2 = *token - 48;
			token = strtok(NULL, " ");

			//printf("[%d]: %-6s  ", a++, token);
			in.op = ((token[2]-48)<<4) + (token[3]-48);
			token = strtok(NULL, " ");

			//printf("[%d]: %-6s  ", a++, token);
			in.ops = *token - 48;
			token = strtok(NULL, " ");

			//printf("\n");

			//printf("1. %s %d %d %u %d \n", in.str, in.format1, in.format2, in.op, in.ops);
			((inst*)inst_table)[inst_index] = in;
			//printf("2. %s %d %d %u %d \n", ((inst*)inst_table)[inst_index].str, ((inst*)inst_table)[inst_index].format1, ((inst*)inst_table)[inst_index].format2, ((inst*)inst_table)[inst_index].op, ((inst*)inst_table)[inst_index].ops);

			inst_index++;
			;
		}
		errno = 0;
	}


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
	int num = 0;
	int len = 99;
	char* line = malloc(len);

	file = fopen(input_file, "r");

	if (file==NULL) errno = -1; //file is not opened

	else {
		while (fscanf(file, "%[^\n]\n", line) != EOF) {
			//input_data[num] = line;
			strncpy(&(input_data[num]), &line, len);
			printf(" %s\n", input_data[num]);
			num++;
		}
		errno = 0;
	}

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
	char* tok;
	struct token_unit tokunit;
	if (str = ".") {  // comment 줄
		strcpy(&tokunit.comment, &str);
		return 0;
	}
	else if (str != "/t") {  // label이 있는 라인
		tok = strtok(str, "\t");

		strncpy(tokunit.label, tok, sizeof(tok) - 1);
		tok = strtok(NULL, " ");

		strncpy(tokunit.operator, tok, sizeof(tok) - 1);
		tok = strtok(NULL, " ");

		strncpy(tokunit.operand, tok, sizeof(tok) - 1);
		tok = strtok(NULL, " ");

		strncpy(tokunit.comment, tok, sizeof(tok) - 1);
		tok = strtok(NULL, " ");
		return 0;

	}
	else {  // label이 없는 라인
		tok = strtok(str, "\t");

		strncpy(tokunit.operator, tok, sizeof(tok) - 1);
		tok = strtok(NULL, " ");

		strncpy(tokunit.operand, tok, sizeof(tok) - 1);
		tok = strtok(NULL, " ");

		strncpy(tokunit.comment, tok, sizeof(tok) - 1);
		tok = strtok(NULL, " ");
		return 0;
	}
	((token*)token_table)[line_num] = tokunit;
	line_num++;
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
	char* token;
	int index = 0;
	token = strtok(str, "\t");
	if (token == "+") {
		for (int i = 1; token[i]; i++) {
			token[i - 1] = token[i];   // operator 앞에 + 붙어있을 경우 제거하고 서치
		}
	}
	while (inst_table[index] != NULL) {  // label이 없는 경우
		if (inst_table[index]->str == token) {
			return index;
		}
	}
	token = strtok(NULL, "\t");
	while (inst_table[index] != NULL) {  // label이 있는 경우
		if (inst_table[index]->str == token) {
			return index;
		}
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
void make_opcode_output(char* file_name)
{
	/* add your code here */
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
