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
#define CANDIDATE_FILE "candidat.dat"
#define VOTER_FILE "vote.dat"
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
void debugCandidateFile();
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

//AdminLogin Credentials
#define ADMIN_USERNAME "admin"
#define ADMIN_PASSWORD "admin"

//AdminLogin Main Function
int adminLogin() {
	char username[30], password[30];

	printf("[Herta Systems - Administration Login]\n");
	printf("Username: ");
	scanf(" %29s", username);
	printf("Password: ");
	scanf(" %29s", password);

	if (strcmp(username, ADMIN_USERNAME) == 0 && strcmp(password, ADMIN_PASSWORD) == 0) {
		printf("\n[Security]: Login Sucessful!. Welcome Administrator\n\nPress Any Key To Continue...\n\n");
		getch();
		clrscr();
		return 1;
	} else {
		printf("\n\n[Security]: Invalid Credentials. Access Denied\n\nPress Any key To Continue...\n");
		getch();
		return 0;
	}
}


//Administration Menu (Void Prototype Reqirements)
void adminMenu() {
	int choice;

	if (!adminLogin()) {
		return;
	}

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
			case 6: clrscr(); debugCandidateFile(); break;
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
	scanf(" %29s", v.username);
	printf("Please Enter Voter Password: ");
	scanf(" %19s", v.password);
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
		clrscr();
		printf("Usrname And Passwrd Has Been Accepted!, Welcome! %s!\n", username);
		vote(username);
	} else {
		printf("Unothorized Login : Invalid Credentials!\nPress any Key To Continue...\n");
		getch();
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
	FILE *candidateFile = fopen(CANDIDATE_FILE, "rb+");
	FILE *voterFile = fopen(VOTER_FILE, "rb+");
	Candidate c;
	Voter v;
	int candidateNum, found = 0, i = 1;

	if (!candidateFile || !voterFile) {
		printf("Error: Could Not Open Candidate Or Voter File.\n");
		return;
	}

	//Checking IF Voter Has already Voted
	while (fread(&v, sizeof(Voter), 1, voterFile)) {
		if (strcmp(v.username, voterUsername) == 0) {
			if (v.hasVoted) {
				printf("You Have Already Voted\n");
				fclose(candidateFile);
				fclose(voterFile);
				return;
			}
			break;
		}
	}

	rewind(candidateFile);

	//Listing candidates function
	printf("[Herta Systems - Candidate List]\n");
	while (fread(&c, sizeof(Candidate), 1, candidateFile)) {
		printf("%d, %s\n", i, c.username);
		i++;
	}

	rewind(candidateFile);

	//Voting Logic Function
	printf("Plese Choose a Candidate By Number: ");
	scanf("%d", &candidateNum);

	i = 1;
	while(fread(&c, sizeof(Candidate), 1, candidateFile)) {
		if (i == candidateNum) {
			c.votes++;
			fseek(candidateFile, -sizeof(Candidate), SEEK_CUR);
			if (fwrite(&c, sizeof(Candidate), 1, candidateFile) != 1) {
				printf("Error: Failled to update Candidate Votes\n");
			}
			found = 1;
			break;
		}
		i++;
	}

	if (!found) {
		printf("Invalid Candidate Selection.\n");
		fclose(candidateFile);
		fclose(voterFile);
		return;
	}

	rewind(voterFile);
	while (fread(&v, sizeof(Voter), 1, voterFile)) {
		if (strcmp(v.username, voterUsername) == 0) {
			v.hasVoted = 1;
			fseek(voterFile, -sizeof(Voter), SEEK_CUR);
			if (fwrite(&v, sizeof(Voter), 1, voterFile) != 1) {
				printf("Error: Failed To update Voter Record.\n");
			}
			break;
		}
	}

	printf("Voted Has been Casted\n");
	logAction("a voter has casted a vote");

	fclose(candidateFile);
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
		if (c.votes < 0 || strlen(c.username) == 0) {
			printf("Corrupted Data Encountered");
			break;
		}
		printf("%s: %d Votes\n", c.username, c.votes);
	}
	fclose(file);
}

//debugCandidtaeFile
void debugCandidateFile() {
	FILE *file = fopen(CANDIDATE_FILE, "rb");
	Candidate c;
	int record = 1;
	if (!file) {
		printf("Error Opening CandidateFile.\n");
		return;
	}
	while(fread(&c, sizeof(Candidate), 1, file)) {
		printf("Record %d: username=%s, Votes=%d\n", record++, c.username, c.votes);
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
			printf("System Has Been Cleared!\n\n\nPress Any Key To Continue...\n\n");
			logAction("System Was Cleared By Administratior");
			getch();
			clrscr();
			break;

		case 'n':
		case 'N':
			printf("System Clear Operation Was Canceled\n\nPress Any Key To Continue...\n\n");
			getch();
			clrscr();
			break;

		default:
			printf("\nInvalid Input, Operation Canceled\n");
			break;
	}
}