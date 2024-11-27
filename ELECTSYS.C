//Including Libraries
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//TypeDefine Structures
typedef struct {
	char username[30];
	char password[20];
	int votes;
} Candidate;

typedef struct {
	char username[30];
	char password[20];
	int hasVoted;
} Voter;

//File Library (IMPORTANT!)
#define CANDIDATE_FILE "candidates.dat"
#define VOTER_FILE "voters.dat"
#define LOG_FILE "logs.dat"

//Functionalities Prototypes (REQUIRED!)
void adminMenu();
void voterMenu();
void candidateMenu();
void addCandidate();
void addVoter();
void vote();
void viewResults();
void runoffElection();
void logAction(const char *action);
void clearSystem();
void listCandidates();
int authenticate(const char *fileName, char *username, char *password);

//Main Functionality Of Entire Program
int main() {
	int choice;
	do {
		clrscr();
		printf("[Herta Systems - Election System]\n");
		printf("1: Administration Login\n");
		printf("2: Voter Login\n");
		printf("3: Candidate Menu\n");
		printf("4: Exit\n");
		printf("Please Enter A Option: ");
		scanf("%d", &choice);

		switch (choice) {
			case 1: clrscr(); adminMenu(); break;
			case 2: clrscr(); voterMenu(); break;
			case 3: clrscr(); candidateMenu(); break;
			case 4: clrscr(); printf("Herta Systems Is Now Terminating The Program. Goodbye!\nPress any Key To Exit Program"); getch();
			default: printf("Invalid Choice. Please Try again.\n");
		}
	} while (choice != 4);

	return 0;
}

//Administration Menu (Void Prototype Reqirements)
void adminMenu() {
	int choice;
	do {
		printf("[Herta Systems - Administration Menu]\n");
		printf("1: Add Candidate\n");
		printf("2: Add Voter\n");
		printf("3: View Results\n");
		printf("4: Clear System\n");
		printf("5: Go Back To Main Menu\n");
		printf("Please Type A Option: ");
		scanf("%d", &choice);

		switch (choice) {
			case 1: clrscr(); addCandidate(); break;
			case 2: clrscr(); addVoter(); break;
			case 3: clrscr(); viewResults(); break;
			case 4: clrscr(); clearSystem(); break;
			case 5: break;
			default: printf("Invalid Choice. Please Try again\n");
		}
	} while (choice != 5);
}

//Adding Candidate Function Prototype
void addCandidate() {
	Candidate c;
	FILE *file = fopen(CANDIDATE_FILE, "ab");
	if (!file) {
		printf("Error Opening Candidate Master File!\n");
		return;
	}
	printf("Please Enter Candidate Username: ");
	scanf(" %19s", c.username);
	printf("Please Enter Candidate Password: ");
	scanf(" %19s", c.password);
	c.votes = 0;
	fwrite(&c, sizeof(Candidate), 1, file);
	fclose(file);
	logAction("SystemAdmin Has Added A New Candidate");
	printf("Candidate Has Been Added Sucessfully!\n");
}

//Adding Voters Function Prototype
void addVoter() {
	Voter v;
	FILE *file = fopen(VOTER_FILE, "ab");
	if (!file) {
		printf("Error Opening Voter Master File!\n");
		return;
	}
	printf("Please Enter Voter Username: ");
	scanf("%s", v.username);
	printf("Pl2ease Enter Voter Password: ");
	scanf("%s", v.password);
	v.hasVoted = 0;
	fwrite(&v, sizeof(Voter), 1, file);
	fclose(file);
	logAction("SystemAdmin Has Added A New Voter");
	printf("Voter Has Been added Sucessfully!\n");
}

//Voter's Menu Function Prototype
void voterMenu() {
	char username[30], password[20];
	printf("[Herta Systems - Voters login]\n");
	printf("Please Enter Username Of Voter: ");
	scanf(" %19s", username);
	printf("Please Enter Password Of Voter: ");
	scanf(" %19s", password);

	if (authenticate(VOTER_FILE, username, password)) {
		printf("Usrname And Passwrd Has Been Accepted!, Welcome! %s!\n", username);
		vote(username);
	} else {
		printf("Unothorized Login : Invalid Credentials!\n");
	}
}

//Candidate's Menu Function Prototype
void candidateMenu() {
	int choice;
	do {
		printf("[Herta Systems - Candidate Menu]\n");
		printf("1: List Candidates\n");
		printf("2: Back To Main Menu\n");
		printf("Please Enter A Choice: ");
		scanf("%d", &choice);

		switch (choice) {
			case 1: listCandidates(); break;
			case 2: break;
			default: printf("Invalid Choice!, Please Try again\n");
		}
	} while (choice != 2);
}

//List Candidates Function Prototype
void listCandidates() {
	Candidate c;
	FILE *file = fopen(CANDIDATE_FILE, "rb");
	if (!file) {
		printf("Error: Could Not Open Master Candidate File!.\n");
		return;
	}

	printf("[Herta Systems - List Of Candidates]\n");
	printf("%-20 %s\n", "Username", "Votes");
	printf("===================================\n");

	while (fread(&c, sizeof(Candidate), 1, file)) {
		printf("%-20s %d\n", c.username, c.votes);
	}

	fclose(file);
}

//Main Voting Function Prototype
void vote(char *voterUsername) {
	FILE *file = fopen(CANDIDATE_FILE, "rb+");
	FILE *voterFile = fopen(VOTER_FILE, "rb+");
	Candidate c;
	Voter v;
	int candidateNum, found = 0;
	int i;

	if (!file || !voterFile) {
		printf("Error Both Opening Candidate And Voter Master Files!\n");
		return;
	}

	//Function Checking if Voter Has Already Voted
	while (fread(&v, sizeof(Voter), 1, voterFile)) {
		if (strcmp(v.username, voterUsername) == 0 && v.hasVoted){
			printf("Sorry! You Have Already Voted!\n");
			fclose(file);
			fclose(voterFile);
			return;
		}
	}
	rewind(voterFile);

	//Displaying Candidates Function
	printf("[Herta Systems - Candidate List]\n");
	i = 1;
	while (fread(&c, sizeof(Candidate), 1, file)) {
		printf("%d. %s\n", i, c.username);
		i++;
	}
	rewind(file);

	//Voting Functionalities
	printf("Please Choose A Candidate (Number): ");
	scanf("%d", &candidateNum);

	//Updating Candidate Votes Function
	i = 1;
	while (fread(&c, sizeof(Candidate), 1, file)) {
		if (i == candidateNum) {
			c.votes++;
			fseek(file, -sizeof(Candidate), SEEK_CUR);
			fwrite(&c, sizeof(Candidate), 1, file);
			found = 1;
			break;
		}
		i++;
	}

	//Update The Voters Record
	if (found) {
		while (fread(&v, sizeof(Voter), 1, voterFile)) {
			if (strcmp(v.username, voterUsername) == 0) {
				v.hasVoted = 1;
				fseek(voterFile, -sizeof(Voter), SEEK_CUR);
				fwrite(&v, sizeof(Voter), 1, voterFile);
				break;
			}
		}
		printf("Your Vote Has Been Casted Successfully!\n");
		logAction("A Voter Has Casted A Vote!");
	} else {
		printf("Invalid Candidate Selection!\n");
	}

	fclose(file);
	fclose(voterFile);
}

//User Authentication Function Prototype
int authenticate(const char *fileName, char *username, char *password) {
	FILE *file = fopen(fileName, "rb");
	if (!file) {
		printf("Error Opening The Files!.\n");
		return 0;
	}
	if (strcmp(fileName, CANDIDATE_FILE) == 0) {
		Candidate c;
		while (fread(&c, sizeof(Candidate), 1, file)) {
			if (strcmp(c.username, username) == 0 && strcmp(c.password, password) == 0) {
				fclose(file);
				return 1;
			}
		}
	} else if (strcmp(fileName, VOTER_FILE) == 0) {
		Voter v;
		while (fread(&v, sizeof(Voter), 1, file)) {
			if (strcmp(v.username, username) == 0 && strcmp(v.password, password) == 0) {
				fclose(file);
				return 1;
			}
		}
	}
	fclose(file);
	return 0;
}

//Logging Action Function Prototype
void logAction(const char *action) {
	FILE *file = fopen(LOG_FILE, "a");
	if (!file) {
		printf("Error Opening Log File!.\n");
		return;
	}
	fprintf(file, "%s\n", action);
	fclose(file);
}

//Vieweing Results Function Prototype
void viewResults() {
	FILE *file = fopen(CANDIDATE_FILE, "rb");
	Candidate c;
	if (!file) {
		printf("Error Opening Candidate Master File!.\n");
		return;
	}
	printf("[Herta Systems - Election Results]");
	while (fread(&c, sizeof(Candidate), 1, file)) {
		printf("%s: %d Votes\n", c.username, c.votes);
	}
	fclose(file);
}

//Clear System Prototype Function (DANGEROUS CMD)
void clearSystem() {
	char confirm;
	printf("[System]: Are You Sure You Want To Clear The System? (y/n): ");
	//scanf("%c", &confirm);
	fflush(stdin);
	confirm = getchar();

	switch (confirm) {
		case 'y':
		case 'Y':
			remove(CANDIDATE_FILE);
			remove(VOTER_FILE);
			remove(LOG_FILE);
			printf("System Has Been Cleared!\n");
			logAction("System Was Cleared By Administratior");
			break;

		case 'n':
		case 'N':
			printf("System Clear Operation Was Canceled\n");
			break;

		default:
			printf("\nInvalid Input, Operation Canceled\n");
			break;
	}
}