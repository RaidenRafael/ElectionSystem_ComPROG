#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>

// Struct Definitions
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

// File Names
#define CANDIDATE_FILE "candidat.dat"
#define VOTER_FILE "voter.dat"
#define LOG_FILE "logs.dat"

// Function Prototypes
void adminMenu();
void voterMenu();
void candidateMenu();
void addCandidate();
void addVoter();
void vote();
void viewResults();
void logAction(const char *action);
void listCandidates();
int authenticate(const char *fileName, char *username, char *password);
int countVotes(const char *candidateUsername);
int hasUserVoted(const char *voterUsername);
void loadingScreen();

// Main Function
int main() {
    char input[10]
    int choice;
	loadingScreen();
    do {
		clrscr();
		printf("==[X][X][X][X][X][X][X][X][X][X][X][X][X][X][X][X][X][X][X][X][X][X][X][X][X]==");
        printf("\n");
		printf("		[Herta Systems - Election System]\n");
        printf("			1: Admin Login\n");
        printf("			2: Voter Login\n");
        printf("			3: Candidate Menu\n");
        printf("			4: Exit\n");
        printf("\n\nEnter your choice: ");
        scanf("%d", &choice);

	fgets(input, sizeof(input), stdin);
        
        if (sscanf(input, "%d", &choice) != 1) {
            printf("Invalid input. Please enter a number.\n");
            getch();
            continue;
        }

        switch (choice) {
            case 1: clrscr(); adminMenu(); break;
            case 2: clrscr(); voterMenu(); break;
            case 3: clrscr(); candidateMenu(); break;
            case 4: printf("Herta Systems Is Now Exiting, Press Any Key To Continue"); getch(); break;
            default: printf("Invalid choice. Try again.\n");
        }
    } while (choice != 4);
    return 0;
}

// Loading Screen
void loadingScreen() {
    const char *messages[] = {
        "Initializing modules...",
        "Loading assets...",
        "Connecting to database...",
        "Preparing UI...",
        "Starting services..."
    };
    int i, j;

    clrscr();
    printf("[Herta Systems - ]\n");
    for (i = 0; i < 5; i++) {
        printf("\r%s", messages[i]);
        for (j = 0; j < 3; j++) {
            printf(".");
            fflush(stdout);
            delay(500);  // Replace with delay logic specific to Turbo C
        }
        printf("\n");
    }

    printf("Startup Complete. Press any key to continue...");
    getch();
}

// Admin Login
#define ADMIN_USERNAME "admin"
#define ADMIN_PASSWORD "admin"

int adminLogin() {
    char username[30], password[30];
	printf("			[Herta Systems - Administration Login Portal]\n");
    printf("> Login Username: ");
    scanf("%s", username);
    printf("> Login Password: ");
    scanf("%s", password);

    return strcmp(username, ADMIN_USERNAME) == 0 && strcmp(password, ADMIN_PASSWORD) == 0;
}

// Admin Menu
void adminMenu() {
	int choice;
    if (!adminLogin()) {
        printf("Invalid Admin Credentials.\n");
        return;
    }

    
    do {
        printf("			[Herta Systems - Election Aministration Menu]\n");
        printf("			1: Add Candidate\n");
        printf("			2: Add Voter\n");
        printf("			3: View Results\n");
        printf("			4: Back to Main Menu\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1: addCandidate(); break;
            case 2: addVoter(); break;
            case 3: viewResults(); break;
            case 4: break;
            default: printf("Invalid choice. Try again.\n");
        }
    } while (choice != 4);
}

// Add Candidate
void addCandidate() {
    Candidate c;
    FILE *file = fopen(CANDIDATE_FILE, "a");
    if (!file) {
        printf("Error opening candidate file.\n");
        return;
    }
    printf("Enter candidate username: ");
    scanf("%s", c.username);
    printf("Enter candidate password: ");
    scanf("%s", c.password);
    c.votes = 0;
    fprintf(file, "%s %s %d\n", c.username, c.password, c.votes);
    fclose(file);
    logAction("Added new candidate.");
}

// Add Voter
void addVoter() {
    Voter v;
    FILE *file = fopen(VOTER_FILE, "a");
    if (!file) {
        printf("Error opening voter file.\n");
        return;
    }
    printf("Enter voter username: ");
    scanf("%s", v.username);
    printf("Enter voter password: ");
    scanf("%s", v.password);
    v.hasVoted = 0;
    fprintf(file, "%s %s %d\n", v.username, v.password, v.hasVoted);
    fclose(file);
    logAction("Added new voter.");
}

// Voter Menu
void voterMenu() {
    char username[30], password[20];
    printf("Voter Username: ");
    scanf("%s", username);
    printf("Voter Password: ");
    scanf("%s", password);

    if (authenticate(VOTER_FILE, username, password)) {
        if (hasUserVoted(username)) {
            printf("You have already voted.\n");
        } else {
            vote(username);
        }
    } else {
        printf("Invalid voter credentials.\n");
    }
}

// Candidate Menu
void candidateMenu() {
    listCandidates();
}

// List Candidates
void listCandidates() {
    FILE *file = fopen(CANDIDATE_FILE, "r");
	Candidate c;
    if (!file) {
        printf("No candidates found.\n");
        return;
    }

    printf("[Candidates]\n");
    
    while (fscanf(file, "%s %s %d", c.username, c.password, &c.votes) != EOF) {
        printf("Username: %s, Votes: %d\n", c.username, countVotes(c.username));
    }
    fclose(file);
}

// Vote Function
void vote(char *voterUsername) {
	char candidateUsername[30];
    listCandidates();

    printf("Enter candidate username to vote for: ");
    scanf("%s", candidateUsername);

    if (!isValidCandidate(candidateUsername)) {
        printf("Invalid candidate username. Vote not cast.\n");
        return;
    }

    if (!updateVoterLog(voterUsername)) {
        printf("Error updating voter status.\n");
        return;
    }

    if (!incrementCandidateVotes(candidateUsername)) {
        printf("Error updating candidate votes.\n");
        return;
    }

    logAction("Vote cast.");
    printf("Vote successfully cast for %s.\n", candidateUsername);
}

// Helper to validate candidate username
int isValidCandidate(const char *username) {
    FILE *file = fopen(CANDIDATE_FILE, "r");
	Candidate c;
    if (!file) return 0;

    
    while (fscanf(file, "%s %s %d", c.username, c.password, &c.votes) != EOF) {
        if (strcmp(c.username, username) == 0) {
            fclose(file);
            return 1;
        }
    }
    fclose(file);
    return 0;
}

// Update Voter Log to Mark as Voted
int updateVoterLog(const char *voterUsername) {
    FILE *file = fopen(VOTER_FILE, "r");
    FILE *temp = fopen("temp_voter.dat", "w");
	Voter v;
    int updated = 0;

    if (!file || !temp) return 0;

    
    while (fscanf(file, "%s %s %d", v.username, v.password, &v.hasVoted) != EOF) {
        if (strcmp(v.username, voterUsername) == 0) {
            v.hasVoted = 1;  // Mark as voted
            updated = 1;
        }
        fprintf(temp, "%s %s %d\n", v.username, v.password, v.hasVoted);
    }
    fclose(file);
    fclose(temp);

    // Replace old log with updated one
    remove(VOTER_FILE);
    rename("temp_voter.dat", VOTER_FILE);

    return updated;
}

// Increment Candidate Vote Count
int incrementCandidateVotes(const char *candidateUsername) {
    FILE *file = fopen(CANDIDATE_FILE, "r");
    FILE *temp = fopen("temp_candidate.dat", "w");
	Candidate c;
    int updated = 0;

    if (!file || !temp) return 0;

    
    while (fscanf(file, "%s %s %d", c.username, c.password, &c.votes) != EOF) {
        if (strcmp(c.username, candidateUsername) == 0) {
            c.votes++;
            updated = 1;
        }
        fprintf(temp, "%s %s %d\n", c.username, c.password, c.votes);
    }
    fclose(file);
    fclose(temp);

    // Replace old log with updated one
    remove(CANDIDATE_FILE);
    rename("temp_candidate.dat", CANDIDATE_FILE);

    return updated;
}


// View Results
void viewResults() {
    listCandidates();
}

// Authentication Function
int authenticate(const char *fileName, char *username, char *password) {
    FILE *file = fopen(fileName, "r");
    if (!file) {
        return 0;
    }

    if (strcmp(fileName, CANDIDATE_FILE) == 0) {
        Candidate c;
        while (fscanf(file, "%s %s %d", c.username, c.password, &c.votes) != EOF) {
            if (strcmp(c.username, username) == 0 && strcmp(c.password, password) == 0) {
                fclose(file);
                return 1;
            }
        }
    } else if (strcmp(fileName, VOTER_FILE) == 0) {
        Voter v;
        while (fscanf(file, "%s %s %d", v.username, v.password, &v.hasVoted) != EOF) {
            if (strcmp(v.username, username) == 0 && strcmp(v.password, password) == 0) {
                fclose(file);
                return 1;
            }
        }
    }
    fclose(file);
    return 0;
}

// Count Votes
int countVotes(const char *candidateUsername) {
    FILE *file = fopen(CANDIDATE_FILE, "r");
    int totalVotes = 0;
    Candidate c;

    while (fscanf(file, "%s %s %d", c.username, c.password, &c.votes) != EOF) {
        if (strcmp(c.username, candidateUsername) == 0) {
            totalVotes += c.votes;
        }
    }
    fclose(file);
    return totalVotes;
}

// Log Action
void logAction(const char *action) {
    FILE *file = fopen(LOG_FILE, "a");
    fprintf(file, "%s\n", action);
    fclose(file);
}

// Check if User Voted
int hasUserVoted(const char *voterUsername) {
	Voter v;

    FILE *file = fopen(VOTER_FILE, "r");
    if (!file) {
        return 0;
    }

    
    while (fscanf(file, "%s %s %d", v.username, v.password, &v.hasVoted) != EOF) {
        if (strcmp(v.username, voterUsername) == 0 && v.hasVoted == 1) {
            fclose(file);
            return 1;
        }
    }
    fclose(file);
    return 0;
}
