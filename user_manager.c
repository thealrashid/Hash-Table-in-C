#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include "hashtable.h"

#define MAX_USERNAME_LENGTH 50
#define MAX_PASSWORD_LENGTH 20

typedef struct _User {
    char username[MAX_USERNAME_LENGTH];
    char password[MAX_PASSWORD_LENGTH];
} User;

typedef struct termios termios;

void user_get_username(char *username) {
    printf("Enter the username: ");
    scanf("%49s", username);
    getchar();
}

void user_get_password(char *password) {
    termios oldt, newt;
    int i = 0;
    char ch;

    printf("Enter the password: ");
    
    // Disable echo
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ECHO);
    newt.c_lflag &= ~(ICANON);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    
    while (i < MAX_PASSWORD_LENGTH) {
        ch = getchar();
        if (ch == '\n' || ch == '\r') { // Enter key pressed
            password[i] = '\0';
            break;
        } else if (ch == 127 && i > 0) { // Handle backspace
            i--;
            printf("\b \b"); // Move cursor back, erase character, move back again
            fflush(stdout);
        } else if (i < MAX_PASSWORD_LENGTH - 1 && ch != 127) {
            password[i++] = ch;
            printf("*");
            fflush(stdout);
        }
    }
    
    // Restore echo
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    printf("\n");
}

int user_password_check(char *password_provided, char *password_stored) {
    if(!strcmp(password_provided, password_stored)) return 1;
    return 0;
}

int user_signin(HashTable *database) {
    User user;

    user_get_username(user.username);
    user_get_password(user.password);

    char *password = (char *)hash_table_get(database, user.username);

    if (!password) {
        printf("User doesn't exist. Please try again.\n");
        return 0;
    }

    if (!user_password_check(user.password, password)) {
        printf("Wrong password provided. Please try again.\n");
        return 0;
    }

    printf("Signed in successfully\n");
    return 1;
}

void user_signup(User *user, HashTable *database) {
    user_get_username(user->username);
    user_get_password(user->password);
    
    hash_table_insert(database, user->username, user->password);
    printf("Signed up successfully\n");
}

size_t user_hash(const void *key) {
    char* username = (char*)key;
    size_t hash = 5381;
    int c;
    while ((c = *username++)) {
        hash = ((hash << 5) + hash) + c; // hash * 33 + c
    }
    return hash;
}

int user_compare(const void *a, const void *b) {
    return strcmp((char *)a, (char *)b);
}

void user_management(User *user) {
    int choice;

    HashTable *database = hash_table_create(16, user_hash, user_compare, NULL, NULL);
    
    while (1) {
        printf("1. Sign in\n2. Sign up\n3. Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);
        
        switch(choice) {
            case 1:
            printf("Please sign in:\n");
            user_signin(database);
            break;
            case 2:
            printf("Please sign up:\n");
            user_signup(user, database);
            break;
            case 3:
            hash_table_destroy(database);
            printf("Exited successfully.\n");
            exit(0);
            default:
            printf("Wrong choice.\n");
        }
        
        printf("----------------------------------\n");
    }
}

int main() {
    User user;
    user_management(&user);

    return 0;
}