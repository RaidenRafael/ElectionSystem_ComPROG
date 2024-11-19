#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Structures
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

// File names
#define CANDIDATE_FILE "candidates.dat"
#define VOTER_FILE "voters.dat"
#define LOG_FILE "logs.dat"

// Function Prototypes
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
int authenticate(const char *fileName, char *username, char *password);

// Main Function
int main() {
    int choice;
    do {
        printf("\n=== Election System ===\n");
        printf("1. Admin Login\n");
        printf("2. Voter Login\n");
        printf("3. Candidate Login\n");
        printf("4. Exit\n");
        printf("Choose an option: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1: adminMenu(); break;
            case 2: voterMenu(); break;
            case 3: candidateMenu(); break;
            case 4: printf("Exiting the system. Goodbye!\n"); break;
            default: printf("Invalid choice. Try again.\n");
        }
    } while (choice != 4);

    return 0;
}

// Admin Menu
void adminMenu() {
    int choice;
    do {
        printf("\n=== Admin Menu ===\n");
        printf("1. Add Candidate\n");
        printf("2. Add Voter\n");
        printf("3. View Results\n");
        printf("4. Clear System\n");
        printf("5. Back to Main Menu\n");
        printf("Choose an option: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1: addCandidate(); break;
            case 2: addVoter(); break;
            case 3: viewResults(); break;
            case 4: clearSystem(); break;
            case 5: break;
            default: printf("Invalid choice. Try again.\n");
        }
    } while (choice != 5);
}

// Add Candidate
void addCandidate() {
    Candidate c;
    FILE *file = fopen(CANDIDATE_FILE, "ab");
    if (!file) {
        printf("Error opening candidate file.\n");
        return;
    }
    printf("Enter candidate username: ");
    scanf("%s", c.username);
    printf("Enter candidate password: ");
    scanf("%s", c.password);
    c.votes = 0;
    fwrite(&c, sizeof(Candidate), 1, file);
    fclose(file);
    logAction("Added new candidate");
    printf("Candidate added successfully!\n");
}

// Add Voter
void addVoter() {
    Voter v;
    FILE *file = fopen(VOTER_FILE, "ab");
    if (!file) {
        printf("Error opening voter file.\n");
        return;
    }
    printf("Enter voter username: ");
    scanf("%s", v.username);
    printf("Enter voter password: ");
    scanf("%s", v.password);
    v.hasVoted = 0;
    fwrite(&v, sizeof(Voter), 1, file);
    fclose(file);
    logAction("Added new voter");
    printf("Voter added successfully!\n");
}

// Voter Menu
void voterMenu() {
    char username[30], password[20];
    printf("\n=== Voter Login ===\n");
    printf("Enter username: ");
    scanf("%s", username);
    printf("Enter password: ");
    scanf("%s", password);

    if (authenticate(VOTER_FILE, username, password)) {
        printf("Login successful. Welcome, %s!\n", username);
        vote(username);
    } else {
        printf("Invalid credentials.\n");
    }
}

// Voting Function
void vote(char *voterUsername) {
    FILE *file = fopen(CANDIDATE_FILE, "rb+");
    FILE *voterFile = fopen(VOTER_FILE, "rb+");
    Candidate c;
    Voter v;
    int candidateNum, found = 0;

    if (!file || !voterFile) {
        printf("Error opening files.\n");
        return;
    }

    // Check if the voter has already voted
    while (fread(&v, sizeof(Voter), 1, voterFile)) {
        if (strcmp(v.username, voterUsername) == 0 && v.hasVoted) {
            printf("You have already voted!\n");
            fclose(file);
            fclose(voterFile);
            return;
        }
    }
    rewind(voterFile);

    // Display candidates
    printf("\n=== Candidates ===\n");
    int i = 1;
    while (fread(&c, sizeof(Candidate), 1, file)) {
        printf("%d. %s\n", i, c.username);
        i++;
    }
    rewind(file);

    // Vote
    printf("Choose a candidate (number): ");
    scanf("%d", &candidateNum);

    // Update candidate votes
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

    if (found) {
        // Update voter record
        while (fread(&v, sizeof(Voter), 1, voterFile)) {
            if (strcmp(v.username, voterUsername) == 0) {
                v.hasVoted = 1;
                fseek(voterFile, -sizeof(Voter), SEEK_CUR);
                fwrite(&v, sizeof(Voter), 1, voterFile);
                break;
            }
        }
        printf("Vote cast successfully!\n");
        logAction("Vote cast");
    } else {
        printf("Invalid candidate selection.\n");
    }

    fclose(file);
    fclose(voterFile);
}

// Authenticate User
int authenticate(const char *fileName, char *username, char *password) {
    FILE *file = fopen(fileName, "rb");
    if (!file) {
        printf("Error opening file.\n");
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

// Log Action
void logAction(const char *action) {
    FILE *file = fopen(LOG_FILE, "a");
    if (!file) {
        printf("Error opening log file.\n");
        return;
    }
    fprintf(file, "%s\n", action);
    fclose(file);
}

// View Results
void viewResults() {
    FILE *file = fopen(CANDIDATE_FILE, "rb");
    Candidate c;
    if (!file) {
        printf("Error opening candidate file.\n");
        return;
    }
    printf("\n=== Election Results ===\n");
    while (fread(&c, sizeof(Candidate), 1, file)) {
        printf("%s: %d votes\n", c.username, c.votes);
    }
    fclose(file);
}

// Clear System
void clearSystem() {
    remove(CANDIDATE_FILE);
    remove(VOTER_FILE);
    remove(LOG_FILE);
    printf("System cleared successfully!\n");
    logAction("System cleared");
}
