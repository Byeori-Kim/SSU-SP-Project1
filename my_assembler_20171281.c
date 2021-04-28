/*
 * ȭ�ϸ� : my_assembler_20171281.c
 * ��  �� : �� ���α׷��� SIC/XE �ӽ��� ���� ������ Assembler ���α׷��� ���η�ƾ����,
 * �Էµ� ������ �ڵ� ��, ��ɾ �ش��ϴ� OPCODE�� ã�� ����Ѵ�.
 * ���� ������ ���Ǵ� ���ڿ� "00000000"���� �ڽ��� �й��� �����Ѵ�.
 */

 /*
  *
  * ���α׷��� ����� �����Ѵ�.
  *
  */
#define _GNU_SOURCE
#pragma warning(disable: 4996)
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>


  // ���ϸ��� "00000000"�� �ڽ��� �й����� ������ ��.
#include "my_assembler_20171281.h"

/* ----------------------------------------------------------------------------------
 * ���� : ����ڷ� ���� ����� ������ �޾Ƽ� ��ɾ��� OPCODE�� ã�� ����Ѵ�.
 * �Ű� : ���� ����, ����� ����
 * ��ȯ : ���� = 0, ���� = < 0
 * ���� : ���� ����� ���α׷��� ����Ʈ ������ �����ϴ� ��ƾ�� ������ �ʾҴ�.
 *		   ���� �߰������� �������� �ʴ´�.
 * ----------------------------------------------------------------------------------
 */
int main(int args, char* arg[])
{
	if (init_my_assembler() < 0)
	{
		printf("init_my_assembler: ���α׷� �ʱ�ȭ�� ���� �߽��ϴ�.\n");
		return -1;
	}

	if (assem_pass1() < 0)
	{
		printf("assem_pass1: �н�1 �������� �����Ͽ����ϴ�.  \n");
		return -1;
	}

	//make_opcode_output(NULL);
	//make_opcode_output("output_20171281.txt");

	
	make_symtab_output("symtab_20171281.txt");																// symboltable ���
	make_literaltab_output("literaltab_20171281.txt");														// literaltable ���
	if (assem_pass2() < 0)
	{
		printf(" assem_pass2: �н�2 �������� �����Ͽ����ϴ�.  \n");
		return -1;
	}

	make_objectcode_output("output_20171281.txt");															// ������� ���
	
	return 0;
}

/* ----------------------------------------------------------------------------------
 * ���� : ���α׷� �ʱ�ȭ�� ���� �ڷᱸ�� ���� �� ������ �д� �Լ��̴�.
 * �Ű� : ����
 * ��ȯ : �������� = 0 , ���� �߻� = -1
 * ���� : ������ ��ɾ� ���̺��� ���ο� �������� �ʰ� ������ �����ϰ� �ϱ�
 *		   ���ؼ� ���� ������ �����Ͽ� ���α׷� �ʱ�ȭ�� ���� ������ �о� �� �� �ֵ���
 *		   �����Ͽ���.
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
 * ���� : �ӽ��� ���� ��� �ڵ��� ������ �о� ���� ��� ���̺�(inst_table)��
 *        �����ϴ� �Լ��̴�.
 * �Ű� : ���� ��� ����
 * ��ȯ : �������� = 0 , ���� < 0
 * ���� : ���� ������� ������ �����Ӱ� �����Ѵ�. ���ô� ������ ����.
 *
 *	===============================================================================
 *		   | �̸� | ���� | ���� �ڵ� | ���۷����� ���� | NULL|
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
 * ���� : ����� �� �ҽ��ڵ带 �о� �ҽ��ڵ� ���̺�(input_data)�� �����ϴ� �Լ��̴�.
 * �Ű� : ������� �ҽ����ϸ�
 * ��ȯ : �������� = 0 , ���� < 0
 * ���� : ���δ����� �����Ѵ�.
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
 * ���� : �ҽ� �ڵ带 �о�� ��ū������ �м��ϰ� ��ū ���̺��� �ۼ��ϴ� �Լ��̴�.
 *        �н� 1�� ���� ȣ��ȴ�.
 * �Ű� : �Ľ��� ���ϴ� ���ڿ�
 * ��ȯ : �������� = 0 , ���� < 0
 * ���� : my_assembler ���α׷������� ���δ����� ��ū �� ������Ʈ ������ �ϰ� �ִ�.
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
			*tok.operand[k] = malloc(20);																	// operand ��ū �迭 ���� �Ҵ�
			memset(tok.operand[k], NULL, 20);																// operand ��ū �迭 �ʱ�ȭ
		}
		strcpy(tok.comment, buf);
		*token_table[token_line] = tok;
		token_line++;
		return 0;
	}
	temp = strtok(buf, "\t");
	if (buf[0] != '\t')																						// label ������
	{
		tok.label = (char*)calloc(strlen(temp) / sizeof(char), sizeof(char));								// label token ���� �Ҵ�
		strcpy(tok.label, temp);																			// label token �Է�

		temp = strtok(NULL, "\t");																			// operator �и�
		tok.operator=malloc(sizeof(temp));																	// operator ��ū ���� �Ҵ�
		strcpy(tok.operator, temp);																			// operator ��ū �Է�													

		temp = strtok(NULL, "\t");																			//���� ��ū �и�

		int isop = search_opcode(tok.operator);																// ���� ��ū�� operand���� comment���� ������ ����
		opcode[token_line] = isop;																			// operator�� index ����
		if (isop >= 0)																						// operator �� inst_file�� �ִ°��
		{
																											// operand ���� Ȯ�� ����
			if (inst_table[isop][0].ops == 0)																// operand�� 0���϶� -> ���� ��ū = comment or ���� 
			{
				if (temp == NULL)																			// ���� ��ū ������
				{
					strcpy(tok.comment, "");
					int k, j = 0;																			// operand �и�
					for (k = 0; k < MAX_OPERAND; k++)
					{
						*tok.operand[k] = malloc(20);														// operand ��ū �迭 ���� �Ҵ�
						memset(tok.operand[k], NULL, 20);													// operand ��ū �迭 �ʱ�ȭ
					}
					*token_table[token_line] = tok;
					token_line++;

					return 0;
				}
				else																						// ���� ��ū = comment
				{
					for (int k = 0; k < MAX_OPERAND; k++)
					{
						*tok.operand[k] = malloc(20);														// operand ��ū �迭 ���� �Ҵ�
						memset(tok.operand[k], NULL, 20);													// operand ��ū �迭 �ʱ�ȭ
					}
					*tok.comment = (char*)calloc(strlen(temp) / sizeof(char), sizeof(char));				// comment ��ū ���� �Ҵ�
					strcpy(tok.comment, temp);																// comment ��ū �Է�	
					*token_table[token_line] = tok;
					token_line++;
					return 0;
				}

			}
			else																							//���� ��ū = operand																	
			{
				char* optemp = (char*)calloc(strlen(temp) / sizeof(char), sizeof(char));					// operand ��ū�� �����ϱ� ���� �ӽ� ��ū ���� �Ҵ�
				strcpy(optemp, temp);																		// operand �и��� ���� �ӽ� ����


				temp = strtok(NULL, "	");																	//���� ��ū �и�
				if (temp == NULL)																			// ���� ��ū ������
				{
					int k, j = 0;																			// operand �и�
					for (k = 0; k < MAX_OPERAND; k++)
					{
						*tok.operand[k] = malloc(20);														// operand ��ū �迭 ���� �Ҵ�
						memset(tok.operand[k], NULL, 20);													// operand ��ū �迭 �ʱ�ȭ
					}
					optemp = strtok(optemp, ",");															// operand ��ū �и� �� �Է� ��ƾ
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
				else // ���� ��ū = comment 
				{
					*tok.comment = (char*)calloc(strlen(temp) / sizeof(char), sizeof(char));				// comment ��ū ���� �Ҵ�
					strcpy(tok.comment, temp);											  					// comment ��ū �Է�

					int k, j = 0;																			// operand �и�
					for (k = 0; k < MAX_OPERAND; k++)
					{
						*tok.operand[k] = malloc(20);														// operand ��ū �迭 ���� �Ҵ�
						memset(tok.operand[k], NULL, 20);													// operand ��ū �迭 �ʱ�ȭ
					}
					optemp = strtok(optemp, ",");															// operand ��ū �и� �� �Է� ��ƾ
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
		else																								// inst_file �� ���� operator�� ���
		{
			if (temp == NULL)																				// inst_file �� ���� operator�̸鼭 operand�� comment��� ���� ���� ���� ���
			{
				strcpy(tok.comment, "");
				int k, j = 0;																				// operand �и�
				for (k = 0; k < MAX_OPERAND; k++)
				{
					*tok.operand[k] = malloc(20);															// operand ��ū �迭 ���� �Ҵ�
					memset(tok.operand[k], NULL, 20);														// operand ��ū �迭 �ʱ�ȭ
				}
				*token_table[token_line] = tok;
				token_line++;
				return 0;
			}
			char* optemp = (char*)calloc(strlen(temp) / sizeof(char), sizeof(char));						// operand ��ū�� �����ϱ� ���� �ӽ� ��ū ���� �Ҵ�
			strcpy(optemp, temp);																			// operand �и��� ���� �ӽ� ����							

			temp = strtok(NULL, "\t");
			if (temp == NULL)																				// comment ������
			{
				int k, j = 0;																				// operand �и�
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
			else																							// comment ������
			{
				*tok.comment = (char*)calloc(strlen(temp) / sizeof(char), sizeof(char));
				strcpy(tok.comment, temp);

				int k, j = 0;																				// operand �и�
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
		int isop = search_opcode(tok.operator);																// ���� ��ū�� operand���� comment���� ������ ����
		opcode[token_line] = isop;																			// operator�� index ����

		if (isop >= 0)																						//operator �� inst_file�� �ִ°��
		{
																											//operand ���� Ȯ�� ����
			if (inst_table[isop][0].ops == 0)																//operand�� 0���϶� -> ���� ��ū = comment or ���� 
			{
				if (temp == NULL)																			// ���� ��ū ������
				{
					strcpy(tok.comment, "");
					int k, j = 0;																			// operand �и�
					for (k = 0; k < MAX_OPERAND; k++)
					{
						*tok.operand[k] = malloc(20);														// operand ��ū �迭 ���� �Ҵ�
						memset(tok.operand[k], NULL, 20);													// operand ��ū �迭 �ʱ�ȭ
					}
					*token_table[token_line] = tok;
					token_line++;
					return 0;
				}
				else																						// ���� ��ū = comment
				{
					strcpy(tok.comment, "");
					int k, j = 0;																			// operand �и�
					for (k = 0; k < MAX_OPERAND; k++)
					{
						*tok.operand[k] = malloc(20);														// operand ��ū �迭 ���� �Ҵ�
						memset(tok.operand[k], NULL, 20);													// operand ��ū �迭 �ʱ�ȭ
					}
					*tok.comment = malloc(sizeof(temp));													// comment ��ū ���� �Ҵ�
					strcpy(tok.comment, temp);																// comment ��ū �Է�	
					*token_table[token_line] = tok;
					token_line++;
					return 0;
				}

			}
			else																							//���� ��ū = operand																	
			{
				char* optemp = (char*)calloc(strlen(temp) / sizeof(char), sizeof(char));					// operand ��ū�� �����ϱ� ���� �ӽ� ��ū ���� �Ҵ�
				strcpy(optemp, temp);																		// operand �и��� ���� �ӽ� ����


				temp = strtok(NULL, "\t");																	//���� ��ū �и�
				if (temp == NULL)																			// ���� ��ū ������
				{
					int k, j = 0;																			// operand �и�
					for (k = 0; k < MAX_OPERAND; k++)
					{
						*tok.operand[k] = malloc(20);														// operand ��ū �迭 ���� �Ҵ�
						memset(tok.operand[k], NULL, 20);													// operand ��ū �迭 �ʱ�ȭ
					}
					optemp = strtok(optemp, ",");															// operand ��ū �и� �� �Է� ��ƾ
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
				else // ���� ��ū = comment 
				{
					*tok.comment = (char*)calloc(strlen(temp) / sizeof(char), sizeof(char));				// comment ��ū ���� �Ҵ�
					strcpy(tok.comment, temp);											  					// comment ��ū �Է�

					int k, j = 0;																			// operand �и�
					for (k = 0; k < MAX_OPERAND; k++)
					{
						*tok.operand[k] = malloc(20);														// operand ��ū �迭 ���� �Ҵ�
						memset(tok.operand[k], NULL, 20);													// operand ��ū �迭 �ʱ�ȭ

					}
					optemp = strtok(optemp, ",");															// operand ��ū �и� �� �Է� ��ƾ
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
		else																								// inst_file �� ���� operator�� ���
		{
			if (temp == NULL)																				// inst_file �� ���� operator�̸鼭 operand�� comment��� ���� ���� ���� ���
			{
				strcpy(tok.comment, "");
				int k, j = 0;																				// operand �и�
				for (k = 0; k < MAX_OPERAND; k++)
				{
					*tok.operand[k] = malloc(20);															// operand ��ū �迭 ���� �Ҵ�
					memset(tok.operand[k], NULL, 20);														// operand ��ū �迭 �ʱ�ȭ
				}
				*token_table[token_line] = tok;
				token_line++;
				return 0;
			}
			char* optemp = (char*)calloc(strlen(temp) / sizeof(char), sizeof(char));						// operand ��ū�� �����ϱ� ���� �ӽ� ��ū ���� �Ҵ�
			strcpy(optemp, temp);																			// operand �и��� ���� �ӽ� ����							

			temp = strtok(NULL, "\t");
			if (temp == NULL)																				// comment ������
			{
				int k, j = 0;																				//operand �и�
				for (k = 0; k < MAX_OPERAND; k++)
				{
					*tok.operand[k] = malloc(20);															// operand ��ū �迭 ���� �Ҵ�
					memset(tok.operand[k], NULL, 20);														// operand ��ū �迭 �ʱ�ȭ
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
			else																							// comment ������
			{
				*tok.comment = (char*)calloc(strlen(temp) / sizeof(char), sizeof(char));
				strcpy(tok.comment, temp);

				int k, j = 0;																				// operand �и�
				for (k = 0; k < MAX_OPERAND; k++)
				{
					*tok.operand[k] = malloc(20);															// operand ��ū �迭 ���� �Ҵ�
					memset(tok.operand[k], NULL, 20);														// operand ��ū �迭 �ʱ�ȭ
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
 * ���� : �Է� ���ڿ��� ���� �ڵ������� �˻��ϴ� �Լ��̴�.
 * �Ű� : ��ū ������ ���е� ���ڿ�
 * ��ȯ : �������� = ���� ���̺� �ε���, ���� < 0
 * ���� :
 *
 * ----------------------------------------------------------------------------------
 */
int search_opcode(char* str)
{
	int index = 0;
	if (str[0] == "+") {
		for (int i = 1; str[i]; i++) {
			str[i - 1] = str[i];																			// operator �տ� + �پ����� ��� ����
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
* ���� : ����� �ڵ带 ���� �н�1������ �����ϴ� �Լ��̴�.
*		   �н�1������..
*		   1. ���α׷� �ҽ��� ��ĵ�Ͽ� �ش��ϴ� ��ū������ �и��Ͽ� ���α׷� ���κ� ��ū
*		   ���̺��� �����Ѵ�.
*
* �Ű� : ����
* ��ȯ : ���� ���� = 0 , ���� = < 0
* ���� : ���� �ʱ� ���������� ������ ���� �˻縦 ���� �ʰ� �Ѿ �����̴�.
*	  ���� ������ ���� �˻� ��ƾ�� �߰��ؾ� �Ѵ�.
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

	/* input_data�� ���ڿ��� ���پ� �Է� �޾Ƽ�
	 * token_parsing()�� ȣ���Ͽ� token_unit�� ����
	 */
}

/* ----------------------------------------------------------------------------------
* ���� : �Էµ� ���ڿ��� �̸��� ���� ���Ͽ� ���α׷��� ����� �����ϴ� �Լ��̴�.
*        ���⼭ ��µǴ� ������ ��ɾ� ���� OPCODE�� ��ϵ� ǥ(���� 3��) �̴�.
* �Ű� : ������ ������Ʈ ���ϸ�
* ��ȯ : ����
* ���� : ���� ���ڷ� NULL���� ���´ٸ� ���α׷��� ����� ǥ��������� ������
*        ȭ�鿡 ������ش�.
*        ���� ���� 3�������� ���̴� �Լ��̹Ƿ� ������ ������Ʈ������ ������ �ʴ´�.
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
* ���� : �Էµ� ���ڿ��� �̸��� ���� ���Ͽ� ���α׷��� ����� �����ϴ� �Լ��̴�.
*        ���⼭ ��µǴ� ������ SYMBOL�� �ּҰ��� ����� TABLE�̴�.
* �Ű� : ������ ������Ʈ ���ϸ�
* ��ȯ : ����
* ���� : ���� ���ڷ� NULL���� ���´ٸ� ���α׷��� ����� ǥ��������� ������
*        ȭ�鿡 ������ش�.
*
* -----------------------------------------------------------------------------------
*/
void make_symtab_output(char* file_name)
{
	/* add your code here */
}

/* ----------------------------------------------------------------------------------
* ���� : �Էµ� ���ڿ��� �̸��� ���� ���Ͽ� ���α׷��� ����� �����ϴ� �Լ��̴�.
*        ���⼭ ��µǴ� ������ LITERAL�� �ּҰ��� ����� TABLE�̴�.
* �Ű� : ������ ������Ʈ ���ϸ�
* ��ȯ : ����
* ���� : ���� ���ڷ� NULL���� ���´ٸ� ���α׷��� ����� ǥ��������� ������
*        ȭ�鿡 ������ش�.
*
* -----------------------------------------------------------------------------------
*/
void make_literaltab_output(char* filen_ame)
{
	/* add your code here */
}

/* ----------------------------------------------------------------------------------
* ���� : ����� �ڵ带 ���� �ڵ�� �ٲٱ� ���� �н�2 ������ �����ϴ� �Լ��̴�.
*		   �н� 2������ ���α׷��� ����� �ٲٴ� �۾��� ���� ������ ����ȴ�.
*		   ������ ���� �۾��� ����Ǿ� ����.
*		   1. ������ �ش� ����� ��ɾ ����� �ٲٴ� �۾��� �����Ѵ�.
* �Ű� : ����
* ��ȯ : �������� = 0, �����߻� = < 0
* ���� :
* -----------------------------------------------------------------------------------
*/
static int assem_pass2(void)
{

	/* add your code here */
}

/* ----------------------------------------------------------------------------------
* ���� : �Էµ� ���ڿ��� �̸��� ���� ���Ͽ� ���α׷��� ����� �����ϴ� �Լ��̴�.
*        ���⼭ ��µǴ� ������ object code (������Ʈ 1��) �̴�.
* �Ű� : ������ ������Ʈ ���ϸ�
* ��ȯ : ����
* ���� : ���� ���ڷ� NULL���� ���´ٸ� ���α׷��� ����� ǥ��������� ������
*        ȭ�鿡 ������ش�.
*
* -----------------------------------------------------------------------------------
*/
void make_objectcode_output(char* file_name)
{
	/* add your code here */
}
