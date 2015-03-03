// -----------------------------
// Random BMS
// -----------------------------
// CommandLine Description
//	Randombms.exe <filename> <arr> [player]
//		<filename>:	(Requied)BMS filename(full path).
//		<arr>:		(Option)Designate random arrangement. In order from left.
//					Please characters that you enter partitions with a slash(/).
//					(exsample: 1/2/3/4/5/6/7, 7/6/5/4/3/2/1, etc.)
//					You can use special word: "mirror", "random", and "cui".
//					"mirror" is equal to input "7/6/5/4/3/2/1".
//					"random" is able to sort at random.
//					"cui" is able to enter this option on CUI.
//		[player]:	(Option) BMS player's path(full path).
//					Add this option if you want to play generated file.
//					If you omit this option, not play and save it.
// -----------------------------
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <stdlib.h>
#define APP_VERSION "0.01a"

// Playmode Enum
enum Playmode
{
	Key5 = 5,
	Key7 = 7,
	Key10 = 10,
	Key14 = 14,
	PMS = 9,
};

// Generate Random Arr
bool GenRandArray(char* Output, int BufSize, int ModeNum)
{
	// Reset
	memset(Output, NULL, BufSize);
	// Flag
	int UnusedNum = pow(2.0f, ModeNum) - 1;
	// Gen
	while(UnusedNum != 0){
		int Rnd = rand()%ModeNum + 1;
		if(UnusedNum & (int)pow(2.0f, Rnd-1)){
			sprintf_s(Output, BufSize, "%s%d", Output, Rnd);
			UnusedNum ^= (int)pow(2.0f, Rnd-1);
			// If it isn't last, add slash
			if(UnusedNum != 0){
				sprintf_s(Output, BufSize, "%s/", Output);
			}
		}
	}
	return true;
}

// Transrate from input to int array
int* TransArray(const char* Output, int ModeNum)
{
	// return array(generated)
	int* NumArray = new int[ModeNum];
	// Flag
	int UnusedNum = pow(2.0f, ModeNum) - 1;
	// tmp
	char cpy[256];
	char* ctx;
	// gen copy
	strcpy_s(cpy, sizeof(cpy), Output);
	// separated
	char* Sepd = strtok_s(cpy, "/", &ctx);
	// loop
	for(int i = 0; i < ModeNum; i++){
		// to num
		int Val = strtol(Sepd, NULL, 10);
		// error check
		if((UnusedNum & (int)pow(2.0f, Val-1)) == 0){
			// Array Transrate Error
			return NULL;
		}
		UnusedNum ^= (int)pow(2.0f, Val-1);
		// add
		NumArray[i] = Val;
		// separated
		Sepd = strtok_s(NULL, "/", &ctx);
	}
	return NumArray;
}

// Get Num's Pos
int GetArrayPos(int* Arr, int Size, int Check)
{
	for(int i=0; i<Size; i++){
		if(Arr[i]==Check) return i;
	}
	return -1;
}

// Command Analysis(true: notes command)
int CheckCommand(const char* Cmd)
{
	// first str check
	if(strlen(Cmd) == 0 || Cmd[0] != '#'){
		// not command
		return 0;
	}
	// to num
	int Val = strtol(&Cmd[1], NULL, 10);
	return Val;
}

// Main
int main(int CmdSize, const char* CmdStr[])
{
	Playmode Mode = Key7;	// Playmode(Key7, key14, ...)
	const char* ArrayBase;	// RandomBase(1/5/2/3/4/6/7, etc.)
	char FileName[256];		// FileName(Full path)
	char ArrInput[256];		// RandomArr.Input
	char OutName[256];		// Output FileName(Full path)
	char Player[256];		// BMS file player
	// -----------------------------
	// base output
	printf("-----------------------------\n");
	printf(" Random BMS Ver. " APP_VERSION "\n");
	printf("-----------------------------\n");
	// rand reset
	srand((unsigned)time(NULL));
	// Filename Check
	if(CmdSize <= 1){
		// empty
		printf("Error:: Please input bms file path.\n");
		printf("For command line, please refer to the \"Readme.txt\".\n");
		system("pause");
		return -1;
	}
	// Copy Filename
	sprintf_s(FileName, sizeof(FileName), CmdStr[1]);
	// Arr Check
	if(CmdSize <= 2 || !strcmp(CmdStr[2], "cui")){
		// input on cui
		printf("-Designate Random Arrangement(In order from left).\n");
		printf(" (exsample: 1/2/3/4/5/6/7, 7/6/5/4/3/2/1, etc...)\n :: ");
		fgets(ArrInput, sizeof(ArrInput), stdin);
		CmdStr[2] = ArrInput;
	}
	if(strstr(CmdStr[2], "mirror")){
		// mirror
		ArrayBase = "7/6/5/4/3/2/1";
	}
	else if(strstr(CmdStr[2], "random")){
		// generate random array
		GenRandArray(ArrInput, sizeof(ArrInput), Mode);
		ArrayBase = ArrInput;
	}
	else {
		// Copy array
		ArrayBase = CmdStr[2];
	}
	// Check Player
	strcpy_s(Player, sizeof(Player), (CmdSize>=4 ? CmdStr[3] : ""));
	// -----------------------------
	// Transrate Array
	int* Array = TransArray(ArrayBase, Mode);
	if(Array == NULL){
		// Error
		printf("Transrate Error. Application Terminate.\n");
		system("pause");
		return -1;
	}
	printf("Load and Output Start...\n");
	// -----------------------------
	// Base File Open
	FILE *fpb, *fpo;
	errno_t Err;
	if(Err = fopen_s(&fpb, FileName, "r")){
		strerror_s(ArrInput, sizeof(ArrInput), Err);
		printf("Error: %s\n", ArrInput);
		system("pause");
		return -1;
	}
	// Output File Open
	char *dst, cpy[256];
	strcpy_s(cpy, sizeof(cpy), FileName);
	char* UnExtName = strtok_s(cpy, ".", &dst);
	strcpy_s(OutName, sizeof(OutName), UnExtName);
	for(int i = 0; i < Mode; i++){
		sprintf_s(OutName, sizeof(OutName), "%s%X", OutName, Array[i]);
	}
	sprintf_s(OutName, sizeof(OutName), "%s.bms", OutName);
	fopen_s(&fpo, OutName, "w");
	// File Read and Write
	while(feof(fpb) == 0){
		// Read Buf
		char Get[4096], Out[4096];
		// Read
		fgets(Get, sizeof(Get), fpb);
		// Copy
		strcpy_s(Out, sizeof(Out), Get);
		// Command Check
		if(int CmdNum = CheckCommand(Get)){
			// notes
			if(CmdNum%100 < 10){
				// not main
				// Write
				fputs(Out, fpo);
				continue;
			}
			// Lane
			int Lane = CmdNum - (CmdNum/10)*10, After = 0;
			// scratch check
			if(Lane == 6){
				// not move
				// Write
				fputs(Out, fpo);
				continue;
			}
			// Get After Pos
			if(Lane <= 5){
				After = GetArrayPos(Array, Mode, Lane) + 1;
			} else {
				After = GetArrayPos(Array, Mode, Lane - 2) + 1;
			}
			// After Num Changed
			if(After >= 6){ After += 2; }
			// Text Replace
			Out[5] = After + '0';
			// Write
			fputs(Out, fpo);
		}
		else {
			// not notes
			// Write
			fputs(Out, fpo);
		}
	}
	printf("Output Success.\n");
	// Close
	fclose(fpb);
	fclose(fpo);
	// -----------------------------
	// Played
	if(Player != NULL && strcmp(Player, "") != 0){
		// Play
		char Launch[512];
		sprintf_s(Launch, sizeof(Launch), "%s \"%s\"", Player, OutName);
		printf("Launch Start...\n", Launch);
		system(Launch);
		// Delete File
		remove(OutName);
	}
	else {
		system("pause");
	}
	return 0;
}