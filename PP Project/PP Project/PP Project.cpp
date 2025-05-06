#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_WORDS 100
#define MAX_LENGTH 50
#define MAX_PLAYERS 100

int lower = 0;
int upper = 1;
char words[MAX_WORDS][MAX_LENGTH];
char guessedWord[20];

int score = 0;
void Play();


void header(){
    printf(
        "\033[30;47m _    _          _   _  _____ __  __          _   _ \n"
        "| |  | |   /\\   | \\ | |/ ____|  \\/  |   /\\   | \\ | |\n"
        "| |__| |  /  \\  |  \\| | |  __| \\  / |  /  \\  |  \\| |\n"
        "|  __  | / /\\ \\ | . ` | | |_ | |\\/| | / /\\ \\ | . ` |\n" 
        "| |  | |/ ____ \\| |\\  | |__| | |  | |/ ____ \\| |\\  |\n"
        "|_|  |_/_/    \\_\\_| \\_|\\_____|_|  |_/_/    \\_\\_| \\_|\n"
        "\033[0m\n\n"
    );
    score > 0 ? printf("\033[93mScore\033[37m: \033[92m%d\n\n\n\033[37m", score) : NULL;
}
void menuInterface() {
    score == 0 ? 
        printf(
        "\033[97m1 \033[37m- \033[32mPlay Game\n"
        "\033[97m2 \033[37m- \033[33mCustom words\n"
        "\033[97m3 \033[37m- \033[96mLeaderboard\n"
        "\033[97m4 \033[37m- \033[90mHistory\n\n"
        "\033[97m0 \033[37m- \033[31mExit\033[93m\n") 
        : 
        printf(
        "\033[97m1 \033[37m- \033[32mPlay Game\n"
        "\033[97m2 \033[37m- \033[33mCustom words\n"
        "\033[97m3 \033[37m- \033[96mLeaderboard\n"
        "\033[97m4 \033[37m- \033[90mHistory\n"
        "\033[97m5 \033[37m- \033[95mSave Score\n\n"
        "\033[97m0 \033[37m- \033[31mExit\033[93m\n");
}

int ReadWords() {
    FILE* file = fopen("words.txt", "r");
    int count = 0;
    if (!file) {
        perror("\033[31mError opening file");
        return 1;
    }
    while (count < MAX_WORDS && fscanf(file, "%49s", words[count]) == 1) count++;
    fclose(file);
    upper = count - 1;
    return 0;
}

#pragma region History
typedef struct {
    char name[MAX_LENGTH];
    int wins;
    int losses;
} PlayerHistory;

char playerName[MAX_LENGTH];
bool didEnterName;
void inputPlayerName() {
    system("cls");
    header();

    printf("Please enter your \033[92mname\033[37m: ");
    void(scanf(" %s", playerName));
    didEnterName = true;
    Play();
}

void SaveHistory(const char* playerName, bool won) {
    PlayerHistory players[MAX_PLAYERS];
    int count = 0;
    bool found = false;

    FILE* file = fopen("playerHistory.txt", "r");
    if (file != NULL) {
        while (fscanf(file, "%99s %d %d", players[count].name, &players[count].wins, &players[count].losses) == 3) {
            count++;
            if (count >= MAX_PLAYERS) break;
        }
        fclose(file);
    }

    for (int i = 0; i < count; ++i) {
        if (strcmp(players[i].name, playerName) == 0) {
            if (won) players[i].wins++;
            else players[i].losses++;
            found = true;
            break;
        }
    }

    if (!found && count < MAX_PLAYERS) {
        strncpy(players[count].name, playerName, MAX_LENGTH);
        players[count].wins = won ? 1 : 0;
        players[count].losses = won ? 0 : 1;
        count++;
    }

    file = fopen("playerHistory.txt", "w");
    if (file == NULL) {
        perror("Could not open history file for writing");
        return;
    }

    for (int i = 0; i < count; ++i) {
        fprintf(file, "%s %d %d\n", players[i].name, players[i].wins, players[i].losses);
    }
    fclose(file);
}
void History() {
    system("cls");
    header();
    FILE* file = fopen("playerHistory.txt", "r");
    if (file == NULL) {
        perror("Could not open playerHistory.txt");
        return;
    }

    char line[256];
    printf(
        "\033[37m=== \033[90mPlayer History \033[37m===\n"
        "\033[90m[Name] [Words Guessed] [Words Failed]\n\n");
    while (fgets(line, sizeof(line), file) != NULL) {
        printf("\033[97m%s", line);
    }

    fclose(file);
    (void)getchar(); (void)getchar();
}
#pragma endregion

#pragma region Leaderboard
typedef struct {
    char name[50];
    int score;
}HighScore;

int ReadLeaderboard(const char* filename, HighScore players[], int maxPlayers) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        perror("Error opening file");
        return -1;
    }

    int count = 0;
    while (count < maxPlayers && fscanf(file, "%49s %d", players[count].name, &players[count].score) == 2) {
        count++;
    }

    fclose(file);
    return count;
}
void WriteLeaderboard(const char* filename, HighScore players[], int count) {
    FILE* file = fopen(filename, "w");
    if (!file) {
        perror("Error saving file");
        return;
    }

    for (int i = 0; i < count; ++i) {
        fprintf(file, "%s %d\n", players[i].name, players[i].score);
    }

    fclose(file);
}
int CompareScores(const void* a, const void* b) {
    int scoreA = ((HighScore*)a)->score;
    int scoreB = ((HighScore*)b)->score;
    return (scoreB > scoreA) - (scoreB < scoreA);
}
void AddToLeaderboard(const char* filename, HighScore newPlayer) {
    HighScore players[MAX_PLAYERS];
    int count = ReadLeaderboard(filename, players, MAX_PLAYERS);

    int found = 0;
    for (int i = 0; i < count; ++i) {
        if (strcmp(players[i].name, newPlayer.name) == 0) {
            found = 1;
            if (newPlayer.score > players[i].score) players[i].score = newPlayer.score;
            break;
        }
    }
    if (!found) {
        if (count < MAX_PLAYERS) players[count++] = newPlayer;
        else return;
    }

    qsort(players, count, sizeof(HighScore), CompareScores);
    WriteLeaderboard(filename, players, count);
}
//Show Leaderboard
void Leaderboard(){
    system("cls");
    header();
    HighScore players[MAX_PLAYERS];
    int total = ReadLeaderboard("LeaderBoard.txt", players, MAX_PLAYERS);

    if (total > 0) {
        for (int i = 0; i < total; ++i) {
            if (i + 1 == 1) printf("\033[33m%d\033[37m) ", i + 1);
            else if (i + 1 == 2) printf("\033[90m%d\033[37m) ", i + 1);
            else if (i + 1 == 3) printf("\033[91m%d\033[37m) ", i + 1);
            else printf("\033[97m%d\033[37m) ", i + 1);
            printf("\033[93m%s \033[97m| \033[96mScore\033[37m: \033[92m%d\n\033[37;m", players[i].name, players[i].score);
        }
    }
    (void)getchar(); (void)getchar();
}
#pragma endregion

#pragma region Play
void Lifes(int life){
    switch (life) {
    case 1:
        printf("\033[31mhealth: %d\n", life);
        break;
    case 2:
        printf("\033[91mhealth: %d\n", life);
        break;
    case 3:
        printf("\033[33mhealth: %d\n", life);
        break;
    case 4:
        printf("\033[32mhealth: %d\n", life);
        break;
    case 5:
        printf("\033[92mhealth: %d\n", life);
        break;
    default:
        break;
    }
    printf("\033[37m");
}

void WriteWord(){
    printf("\033[97m%s", guessedWord);
}

void GenerateGuessedWord(char* word) {
    for (int i = 0; i < 20; i++)
        guessedWord[i] = NULL;
    for (int i = 0; i < strlen(word); i++)
        guessedWord[i] = '_';
}
void UpdateWord(char* word, char guess) {
    for (int i = 0; i < strlen(guessedWord); i++)
        if (word[i] == guess)
            guessedWord[i] = guess;
}

void WinScreen(){
    header();
    printf("\033[92mCongrats! \033[97mYou \033[92mguessed\033[97m the word. The word was:\033[92m %s", guessedWord);
    int points = strlen(guessedWord);
    printf("\n\n\033[37mYou obtained \033[32m%d \033[97mpoints\033[37m!", points);
    score += points;
    SaveHistory(playerName, true);
    (void)getchar(); (void)getchar();
}

void LoseScreen(char* word) {
    header();
    printf("\033[91mYou lost!\033[97m The word was: \033[91m%s", word);
    printf("\n\n\033[37mYour \033[92mscore \033[37mhas been \033[91mreset\033[37m!");
    score = 0;
    SaveHistory(playerName, false);
    (void)getchar(); (void)getchar();
}

void Play() {
    char word[50];
    char guess;
    int health = 5;

    srand((unsigned int)time(0));
    strcpy(word, words[rand() % (upper - lower + 1) + lower]);

    GenerateGuessedWord(word);
    do {
        system("cls");
        header();
        Lifes(health);
        WriteWord();
        printf("\033[93m\nEnter a guess: \033[37m");

        if (scanf(" %c", &guess) == 1) {
            if (strchr(word, guess) != NULL)
                UpdateWord(word, guess);
            else health--;
        }
    } while (health > 0 && strcmp(word, guessedWord));
    system("cls");
    if (health > 0) WinScreen();
    else LoseScreen(word);
}
#pragma endregion

#pragma region Add/Remove Words
int AddCustomWord(char* word) {
    FILE* file = fopen("words.txt", "a");
    if (!file) {
        perror("\033[31mError opening file");
        return 1;
    }
    fprintf(file, "\n%s", word);
    fclose(file);
    return 0;
}

int RemoveWord(int wordNumber, int numberOfWords) {
    wordNumber--;
    for (int i = wordNumber; i < numberOfWords; i++) {
        strcpy(words[i], words[i + 1]);
    }

    FILE* file = fopen("words.txt", "w");
    if (!file) {
        perror("\033[31mError opening file");
        return 1;
    }
    for (int i = 0; i < numberOfWords - 1; i++) {
        if (i == 0) fprintf(file, "%s", words[i]);
        else fprintf(file, "\n%s", words[i]);
    }
    fclose(file);
    return 0;
}

void RemoveCustomWord() {
    int option;
    char input[10];
    bool enteredValidNumber = true;
    do {
        ReadWords();
        system("cls");
        option = -1;
        header();
        printf("Choose what word you want to \033[91mdelete.\n\n");

        int numberOfWords = 0;
        for (int i = 0; i < MAX_WORDS; i++) {
            if (strcmp(words[i], "")) {
                printf("\033[97m%d \033[37m- \033[91m%s\n", i + 1, words[i]);
                numberOfWords++;
            }
        }
        printf("\n\033[97m0 \033[37m- \033[31mGo Back\n");
        
        if (!enteredValidNumber) {
            printf("\n\033[91mPlease enter a \033[31mvalid number\n");
            enteredValidNumber = true;
        }
        printf("\n\033[93mEnter option: \033[37m");

        memset(input, 0, sizeof(input));
        void(scanf(" %9s", input));
        printf("input: %s, option: %d\n", input, option);

        char* endPtr;
        option = strtol(input, &endPtr, 10); // idk why it is screaming at me :(

        if (*endPtr != '\0') {
            enteredValidNumber = false;
            continue;
        }
        if (option < 0 || option > numberOfWords)
            enteredValidNumber = false;
        else if (option == 0) {
            option = -2;
        }
        else {
            RemoveWord(option, numberOfWords);
        }
    } while (option != -2);
}

bool didAddWord = false;
void CustomWords(){
    int option;
    char input;
    char word[50];
    bool enteredValidNumber = true;
    do{
        system("cls");
        option = -1;
        header();
        printf("\033[97m1 \033[37m- \033[92mAdd word\n");
        printf("\033[97m2 \033[37m- \033[91mRemove word\n\n");
        printf("\033[97m0 \033[37m- \033[31mBack\n");

        if (!enteredValidNumber) {
            printf("\n\033[91mPlease enter a \033[31mvalid number\n");
            enteredValidNumber = true;
        }

        printf("\n\033[93mEnter option: \033[37m");
        void(scanf(" %c", &input));
        input = input - '0';

        if (input < 0 || input > 2)
            enteredValidNumber = false;
        else if (input == 0) {
            option = -2;
        }
        else
        {
            option = input;
            switch (option) {
            case 1:
                printf("\033[93m\nWord to add: \033[37m");
                void(scanf(" %s", word));
                didAddWord = AddCustomWord(word);
                break;
            case 2:
                RemoveCustomWord();
                break;
            default:
                break;
            }
        }
    } while (option >= -1 && option <= 2);
}
#pragma endregion

bool savedScore = false;
void SaveScore(){
    system("cls");
    header();

    HighScore newPlayer;
    strcpy(newPlayer.name, playerName);
    newPlayer.score = score;

    AddToLeaderboard("LeaderBoard.txt", newPlayer);
    score = 0;
    savedScore = true;
    didEnterName = false;
}

void menu(int op){
    system("cls");
    ReadWords();
    switch (op){
    case 1: //Play
        if (didEnterName == false) inputPlayerName();
        else Play();
        break;
    case 2: //Custom Words
        CustomWords();
        break;
    case 3: //Leaderboard
        Leaderboard();
        break;
    case 4:
        History();
        break;
    case 5:
        SaveScore();
        break;
    default:
        header();
        printf("\033[37mThank you for playing. \033[91mBye!\n\033[30m");
    }
}

int main(){
    int option;
    char input;
    bool enteredValidNumber = true;
    didEnterName = false;
    do {
        system("cls");
        option = -1;
        header();

        // Print if a word was added.
        if (didAddWord == true) {
            printf("\033[92mWord Added Successfully!\n\n");
            didAddWord = false;
        }

        // Print if score was saved.
        if (savedScore == true) {
            printf("\033[92mScore Saved Successfully!\n\n");
            savedScore = false;
        }

        //Interface
        menuInterface();
        if (!enteredValidNumber) {
            printf("\n\033[91mPlease enter a \033[31mvalid number\n");
            enteredValidNumber = true;
        }
        printf("\n\033[93mEnter option: \033[37m");

        void(scanf(" %c", &input));
        input = input - '0';

        int maxInput = score > 0 ? 5 : 4;

        if (input < 0 || input > maxInput) {
            enteredValidNumber = false;
        }
        else if (input == 0) {
            menu(option);
            option = -2;
        }
        else {
            option = input;
            printf("%d", option);
            menu(option);
        }
    } while (option >= -1 && option <= 5);
    return 0;
}