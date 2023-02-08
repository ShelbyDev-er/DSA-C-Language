#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <limits.h>
#include <time.h>
#define MAX 50

struct Menu
{
    int level;
    int id;
    char *item;
    struct Menu **subMenu;
} Menu[5] = {
    {0, 1, "SEARCH YOUR CONTACT",
     (struct Menu *[]){
         &(struct Menu){1, 1, ""},
         &(struct Menu){1, 2, "UPDATE"},
         &(struct Menu){1, 3, "DELETE"},
         &(struct Menu){1, 4, "BACK TO MAIN MENU"},
         NULL}},
    {0, 2, "ADD CONTACT", NULL},
    {0, 3, "DISPLAY FAVOURITES CONTACT",
     (struct Menu *[]){
         &(struct Menu){1, 1, ""},
         &(struct Menu){1, 2, "UPDATE"},
         &(struct Menu){1, 3, "DELETE"},
         &(struct Menu){1, 4, "BACK TO MAIN MENU"},
         NULL}},
    {0, 4, "DISPLAY ALL CONTACT",
     (struct Menu *[]){
         &(struct Menu){1, 1, ""},
         &(struct Menu){1, 2, "UPDATE"},
         &(struct Menu){1, 3, "DELETE"},
         &(struct Menu){1, 4, "BACK TO MAIN MENU"},
         NULL}},
    {0, 5, "EXIT APPLICATION", NULL},
};

typedef struct MenuTree
{
    int value;
    struct MenuTree *next;
} MenuTree;

typedef struct Contact
{
    int id;
    char firstName[10];
    char middleName[10];
    char lastName[10];
    char company[30];
    char phone[11];
    char email[30];
    char address[30];
    char birthday[12];
    char website[30];
    char note[50];
    int status;
    struct Contact *next;
} Contact;

Contact *SortedMerge(Contact *ptr1, Contact *ptr2);
Contact *binarySearch(Contact *head, char value[MAX]);
Contact *middle(Contact *start, Contact *last);
Contact getContact(Contact **contact);
Contact *contactAtIndex(Contact **head, int no);
int countContact(Contact *headRef);
int randomId();
int serilize(Contact *head);
int deserialize(Contact **head);
int menuHandle();
int backToMainMenu(int *currentDisplayMenuLevel);
int listAtIndex(MenuTree *head, int index);
int selectedMenu(int choice, int *currentDisplayMenuLevel, Contact **headRef, int *id, int *no);
int inputNoToViewDetails(int no);
int addOrRemoveFavourite(Contact **headRef, Contact *head, int no);
int fastAndSlow(Contact *fast, Contact *slow, Contact *value);
int FrontBackSplit(Contact *source, Contact **frontRef, Contact **backRef);
int MergeSort(Contact **headRef);
int updateContact(Contact **headRef, Contact *head, int no);
int deleteContact(Contact **headRef, int no);
int addContact(Contact **head, int *id);
char searchContact(char searchName[20]);
void getFavouriteContacts(Contact *head);
void detailContact(Contact *head, int no, int *currentDisplayMenuLevel);
void menuManagement(MenuTree **treeHead, int *currentDisplayMenuLevel, Contact **headRef, int *id, int *no);
void resetOutputColor();
void getSearchContacts(Contact *mid, char value[MAX]);
void getString(char *str, int length);
void getFieldsName(char *str, int length, char field[30]);
void printContacts(Contact *head);
void printLine();
void printWarn(char title[50]);
void printError(char title[50]);
void printSuccess(char title[50]);
void printTitle(char title[50]);
void pressEnterToContinue();

int main()
{
    return menuHandle();
}

int menuHandle()
{
    Contact *headRef = NULL;
    int no, currentDisplayMenuLevel = -1, result, choice = 0, active, id, check;
    deserialize(&headRef);
    MenuTree *menuTree, *menuTreeHead = NULL;
    while (active == 0)
    {
        menuTree = (MenuTree *)malloc(sizeof(MenuTree));
        menuTree->value = choice;
        menuTree->next = NULL;
        if (menuTreeHead == NULL)
        {
            menuTreeHead = menuTree;
        }
        else
        {
            menuTree->next = menuTreeHead;
            menuTreeHead = menuTree;
        }
        if (check == 0)
        {
            menuManagement(&menuTreeHead, &currentDisplayMenuLevel, &headRef, &id, &no);
            printf(" #CHOICE: ");
            scanf("%d", &choice);
            currentDisplayMenuLevel++;
        }
    }
    return 0;
}

void mainMenu()
{
    printLine();
    printf("   --- VTC ACADEMY CONTACT MANAGEMENT ---\n");
    printLine();
    for (int i = 0; i < sizeof(Menu) / sizeof(*Menu); i++)
    {
        printf(" %d. %s\n", Menu[i].id, Menu[i].item);
    }
    printLine();
}

void menuManagement(MenuTree **treeHead, int *currentDisplayMenuLevel, Contact **headRef, int *id, int *no)
{
    int choice = (*treeHead)->value;
    if (selectedMenu(choice, &*currentDisplayMenuLevel, &*headRef, &*id, &*no) == 0)
    {
        mainMenu();
    }
    else
    {
        Contact *find = *headRef;
        struct Menu **subMenu = Menu[(*treeHead)->value - 1].subMenu;
        while (find != NULL && find->id != *no)
        {
            find = find->next;
        }
        for (int i = 0; subMenu && subMenu[i]; i++)
        {
            if (find->status == 1)
            {
                subMenu[0]->item = "REMOVE FROM FAVOURITES";
            }
            else
            {
                subMenu[0]->item = "ADD TO FAVOURITES";
            }
        }
        if (*currentDisplayMenuLevel == -1)
        {
            mainMenu();
        }
        else
        {
            struct Menu **subMenu = Menu[(*treeHead)->value - 1].subMenu;
            int temp = 0;
            int count = 1;
            while (count <= *currentDisplayMenuLevel && subMenu != NULL)
            {
                *subMenu = *subMenu[listAtIndex(*treeHead, count)]->subMenu;
                count++;
            }
            for (int i = 0; subMenu && subMenu[i]; i++)
            {
                printf(" %d. %s\n", i + 1, subMenu[i]->item);
            }
            printLine();
        }
    }
}

int selectedMenu(int choice, int *currentDisplayMenuLevel, Contact **headRef, int *id, int *no)
{
    char searchName[20], isFavourite;
    int level = *currentDisplayMenuLevel, index = *id;
    index = countContact(*headRef);

    if (choice == 2)
        index++;

    *id = index;
    if (level == -1 || level == 0)
    {
        switch (choice)
        {
        case 1:
            printTitle("SEARCH CONTACT");
            searchName[20] = searchContact(searchName);
            MergeSort(&*headRef);
            if (binarySearch(*headRef, searchName) == NULL)
            {
                printError("Not found!\n");
                return backToMainMenu(&*currentDisplayMenuLevel);
            }
            else
            {
                *no = inputNoToViewDetails(*no);

                if (*no == 0)
                    return 0;

                detailContact(*headRef, *no, &*currentDisplayMenuLevel);
            }
            break;
        case 2:
            printTitle("ADD CONTACT");
            addContact(&*headRef, &*id);
            serilize(*headRef);
            printSuccess("Saving.. Success!\n");
            backToMainMenu(&*currentDisplayMenuLevel);
            break;
        case 3:
            printTitle("FAVOURITE CONTACTS");
            getFavouriteContacts(*headRef);
            *no = inputNoToViewDetails(*no);

            if (*no == 0)
                return backToMainMenu(&*currentDisplayMenuLevel);

            detailContact(*headRef, *no, &*currentDisplayMenuLevel);
            break;
        case 4:
            printTitle("ALL CONTACTS");
            MergeSort(&*headRef);
            printContacts(*headRef);
            *no = inputNoToViewDetails(*no);

            if (*no == 0)
                return backToMainMenu(&*currentDisplayMenuLevel);

            detailContact(*headRef, *no, &*currentDisplayMenuLevel);
            break;
        case 5:
            printSuccess("\n Exiting.. Success!\n\n");
            exit(0);
            break;
        default:
            if (level == 0)
            {
                printLine();
                printError("Please Re-enter Menu 1-5!\n");
                backToMainMenu(&*currentDisplayMenuLevel);
            }
            break;
        }
    }
    else if (level == 1)
    {
        switch (choice)
        {
        case 1:
            addOrRemoveFavourite(&*headRef, *headRef, *no);
            serilize(*headRef);
            printSuccess("Add To / Remove From Favourite Success!\n");
            backToMainMenu(&*currentDisplayMenuLevel);
            break;
        case 2:
            printTitle("UPDATE CONTACTS");
            updateContact(&*headRef, *headRef, *no);
            serilize(*headRef);
            printSuccess("Updating.. Success!\n");
            backToMainMenu(&*currentDisplayMenuLevel);
            break;
        case 3:
            deleteContact(&*headRef, *no);
            serilize(*headRef);
            printSuccess("Deleting.. Success!\n");
            backToMainMenu(&*currentDisplayMenuLevel);
            break;
        case 4:
            backToMainMenu(&*currentDisplayMenuLevel);
            break;
        default:
            printLine();
            printError("Please Re-enter Menu 1-4!\n");
            backToMainMenu(&*currentDisplayMenuLevel);
            break;
        }
    }
}

int countContact(Contact *headRef)
{
    int count = 0;
    if (headRef == 0)
        return 0;
    else
    {
        while (headRef != NULL)
        {
            count++;
            headRef = headRef->next;
        }
    }
    return count;
}

int randomId()
{
    srand(time(0));
    return rand();
}

int serilize(Contact *head)
{
    FILE *file = fopen("contacts.txt", "w");

    if (file == NULL)
        return 0;

    for (Contact *curr = head; curr != NULL; curr = curr->next)
    {
        fprintf(file, "%d,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%d\n", curr->id, curr->firstName, curr->middleName, curr->lastName, curr->company, curr->phone, curr->email, curr->address, curr->birthday, curr->website, curr->note, curr->status);
    }
    fclose(file);
    return 1;
}

void saveContact(Contact **head, Contact *temp)
{
    Contact *newContact = (Contact *)malloc(sizeof(Contact));

    newContact->id = temp->id;
    strcpy(newContact->firstName, temp->firstName);
    strcpy(newContact->middleName, temp->middleName);
    strcpy(newContact->lastName, temp->lastName);
    strcpy(newContact->company, temp->company);
    strcpy(newContact->phone, temp->phone);
    strcpy(newContact->email, temp->email);
    strcpy(newContact->address, temp->address);
    strcpy(newContact->birthday, temp->birthday);
    strcpy(newContact->website, temp->website);
    strcpy(newContact->note, temp->note);
    newContact->status = temp->status;
    newContact->next = NULL;

    if (*head == NULL)
    {
        *head = newContact;
    }
    else
    {
        newContact->next = *head;
        *head = newContact;
    }
}

int deserialize(Contact **head)
{
    FILE *file = fopen("contacts.txt", "r");
    int count = 15;
    Contact *temp = (Contact *)malloc(count * sizeof(Contact));
    if (file == NULL)
        return 0;
    fseek(file, 0, SEEK_SET);
    while ((fscanf(file, "%d,%[^,],%[^,],%[^,],%[^,],%[^,],%[^,],%[^,],%[^,],%[^,],%[^,],%d\n",
                   &(temp->id), &(temp->firstName),
                   &(temp->middleName), &(temp->lastName),
                   &(temp->company), &(temp->phone),
                   &(temp->email), &(temp->address),
                   &(temp->birthday), &(temp->website),
                   &(temp->note), &(temp->status))) > 0)
    {
        saveContact(&*head, temp);
        fseek(file, 0, SEEK_CUR);
        count++;
    }
    fclose(file);
    return 1;
}

void getFavouriteContacts(Contact *head)
{
    if (head == NULL)
    {
        return;
    }

    while (head != NULL)
    {
        if ((head)->status == 1)
        {
            printf("  %2d  |  %s %s\n", head->id, head->lastName, head->firstName);
        }
        head = head->next;
    }
}

int addOrRemoveFavourite(Contact **headRef, Contact *head, int no)
{
    while (*headRef != NULL && (*headRef)->id != no)
    {
        *headRef = (*headRef)->next;
    }

    if ((*headRef)->status == 0)
        (*headRef)->status = 1;
    else
        (*headRef)->status = 0;

    *headRef = head;
    return (*headRef)->status;
}

int updateContact(Contact **headRef, Contact *head, int no)
{
    while (*headRef != NULL && (*headRef)->id != no)
    {
        *headRef = (*headRef)->next;
    }
    getContact(&*headRef);
    *headRef = head;
    return 1;
}

int deleteContact(Contact **headRef, int no)
{
    Contact *head = *headRef;
    Contact *prev = *headRef;
    Contact *current = *headRef;
    Contact *next = *headRef;
    if (no == 1)
    {
        *headRef = current->next;
        free(current);
        current = NULL;
    }
    else
    {
        while (current != NULL && current->id != no)
        {
            prev = current;
            current = current->next;
            prev->id = current->id;
        }
        prev->next = current->next;
        free(current);
        current = NULL;
    }
    while (*headRef != NULL && (*headRef)->id != 1)
    {
        (*headRef)->id -= 1;
        *headRef = (*headRef)->next;
    }
    *headRef = head;
    return 1;
}

int inputNoToViewDetails(int no)
{
    printLine();
    printWarn("*Input 0 to back main menu*\n\n");
    printf(" INPUT CONTACT NO TO VIEW DETAILS: ");
    scanf("%d", &no);
    return no;
}

Contact getContact(Contact **contact)
{
    getFieldsName((*contact)->firstName, 10, "First Name");
    getFieldsName((*contact)->middleName, 10, "Middle Name");
    getFieldsName((*contact)->lastName, 10, "Last Name");
    getFieldsName((*contact)->company, 30, "Company");
    getFieldsName((*contact)->phone, 11, "Phone");
    getFieldsName((*contact)->email, 30, "Email");
    getFieldsName((*contact)->address, 30, "Address");
    getFieldsName((*contact)->birthday, 11, "Birthday");
    getFieldsName((*contact)->website, 30, "Website");
    getFieldsName((*contact)->note, 50, "Note");
    return **contact;
}

int addContact(Contact **head, int *id)
{
    Contact *newContact = (Contact *)malloc(sizeof(Contact));

    newContact->id = *id;
    getContact(&newContact);
    newContact->status = 0;
    newContact->next = NULL;

    if (*head == NULL)
    {
        *head = newContact;
    }
    else
    {
        newContact->next = *head;
        *head = newContact;
    }

    return 1;
}

int fastAndSlow(Contact *fast, Contact *slow, Contact *value)
{
    while (fast != value)
    {
        fast = fast->next;
        if (fast != value)
        {
            slow = slow->next;
            fast = fast->next;
        }
    }
    return 0;
}

int FrontBackSplit(Contact *source, Contact **frontRef, Contact **backRef)
{
    Contact *fast;
    Contact *slow;
    slow = source;
    fast = source->next;
    fastAndSlow(fast, slow, NULL);
    *frontRef = source;
    *backRef = slow->next;
    slow->next = NULL;
    return 0;
}

int MergeSort(Contact **headRef)
{
    Contact *head = *headRef;
    Contact *ptr1;
    Contact *ptr2;
    if ((head == NULL) || (head->next == NULL))
    {
        return 0;
    }
    FrontBackSplit(head, &ptr1, &ptr2);
    MergeSort(&ptr1);
    MergeSort(&ptr2);
    *headRef = SortedMerge(ptr1, ptr2);
    return 1;
}

Contact *SortedMerge(Contact *ptr1, Contact *ptr2)
{
    Contact *result = NULL;
    if (ptr1 == NULL)
        return (ptr2);
    else if (ptr2 == NULL)
        return (ptr1);
    if (strcmp(ptr1->lastName, ptr2->lastName) <= 0)
    {
        result = ptr1;
        result->next = SortedMerge(ptr1->next, ptr2);
    }
    else
    {
        result = ptr2;
        result->next = SortedMerge(ptr1, ptr2->next);
    }
    return (result);
}

Contact *binarySearch(Contact *head, char value[MAX])
{
    Contact *start = head;
    Contact *last = NULL;
    do
    {
        Contact *mid = middle(start, last);
        if (mid == NULL)
            return NULL;
        if (strcmp(mid->lastName, value) == 0)
        {
            getSearchContacts(mid, value);
            return mid;
        }
        else if (strcmp(mid->lastName, value) < 0)
        {
            start = mid->next;
        }
        else
        {
            last = mid;
        }
    } while (last == NULL || last != start);
    return NULL;
}

Contact *middle(Contact *start, Contact *last)
{
    if (start == NULL)
        return NULL;
    struct Contact *slow = start;
    struct Contact *fast = start->next;
    fastAndSlow(fast, slow, last);
    return slow;
}

void getSearchContacts(Contact *mid, char value[MAX])
{
    if (mid->next == NULL)
    {
        printf("  %2d  |  %s %s\n", mid->id, mid->lastName, mid->firstName);
    }
    else
    {
        while (mid != NULL && (strcmp(mid->lastName, value) == 0 || strcmp(mid->firstName, value) == 0))
        {
            printf("  %2d  |  %s %s\n", mid->id, mid->lastName, mid->firstName);
            mid = mid->next;
        }
    }
    return;
}

void detailContact(Contact *head, int no, int *currentDisplayMenuLevel)
{
    while (head != NULL && head->id != no)
    {
        head = head->next;
    }
    printTitle("DETAIL CONTACT");
    printf(" Status: %d\n", head->status);
    printf(" First Name: %s\n", head->firstName);
    printf(" Middle Name: %s\n", head->middleName);
    printf(" Last Name: %s\n", head->lastName);
    printf(" Company: %s\n", head->company);
    printf(" Phone: %s\n", head->phone);
    printf(" Email: %s\n", head->email);
    printf(" Address: %s\n", head->address);
    printf(" Birthday: %s\n", head->birthday);
    printf(" Website: %s\n", head->website);
    printf(" Note: %s\n", head->note);
    printLine();
}

void printContacts(Contact *head)
{
    printf("  %s  |  %s\n", "ID", "NAME");
    while (head != NULL)
    {
        if (head->id >= 0 && head->id < 100)
        {
            printf("  %2d  |  %s %s\n", head->id, head->lastName, head->firstName);
        }
        head = head->next;
    }
    printLine();
}

int backToMainMenu(int *currentDisplayMenuLevel)
{
    pressEnterToContinue();
    if (*currentDisplayMenuLevel != -1)
    {
        *currentDisplayMenuLevel = -1;
    }
    return 0;
}

int listAtIndex(MenuTree *head, int index)
{
    int count = 0;
    struct MenuTree *current = head;
    while (current != NULL && count != index)
    {
        count++;
        current = current->next;
    }
    return current->value;
}

char searchContact(char searchName[20])
{
    printf(" Enter Contact Name You Want To Search: ");
    getString(searchName, MAX);
    printLine();
    return searchName[20];
}

void getFieldsName(char *str, int length, char field[30])
{
    printf(" %s: ", field);
    getString(str, length);
}

void getString(char *str, int length)
{
    fflush(stdin);
    fgets(str, length, stdin);
    str[strlen(str) - 1] = '\0';
    fflush(stdin);
}

void pressEnterToContinue()
{
    printLine();
    printWarn("Press Enter To Continue..");
    fflush(stdin);
    getchar();
    system("cls");
}

void printWarn(char title[50])
{
    printf("\033[0;33m");
    printf(" %s", title);
    resetOutputColor();
}
void printError(char title[50])
{
    printf("\033[1;31m");
    printf(" %s", title);
    resetOutputColor();
}

void printSuccess(char title[50])
{
    printLine();
    printf("\033[0;32m");
    printf(" %s", title);
    resetOutputColor();
}

void printLine()
{
    printf("\033[0;30m");
    printf("============================================\n");
    resetOutputColor();
}

void printTitle(char title[50])
{
    printLine();
    printf(" %s\n", title);
    printLine();
}

void resetOutputColor()
{
    printf("\033[0m");
}