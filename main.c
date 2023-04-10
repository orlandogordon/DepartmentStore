//
// Build Instrunctions:
// Open Dev Command Prompt
// Run: "msbuild DepartmentStore.vcxproj /p:configuration=debug"
// Navigate to Debug folder and run "myproject"
//

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include "parser.h"

struct _User
{
    char username[100];
    char password[10];
};
typedef struct _User User;

User LoginUser(VOID);

User CreateNewUser(void);


int main(void) {
    User ActiveUser = LoginUser();

    printf("Succesfully logged in as");
    XMLDocument doc;

    if (XMLDocument_Load(&doc, "inventory.xml")) {
        int input = 10;
        char in[50];
        while (input != 0 && input != 6)
        {
            printf("Welcome to the Department Store Register!\n");
            printf("Please select an option below:\n");
            printf("1.) Search for item by name\n");
            printf("2.) Search for item by id #\n");
            printf("3.) Search for item by category\n");
            printf("4.) Cart Summary\n");
            printf("5.) Checkout\n");
            printf("6.) Exit\n");

            printf("Please select an option above:");
            if (fgets(in, sizeof(in), stdin) != NULL) {
                in[strcspn(in, "\n")] = 0;
                input = in[0]-48;
                printf("In buffer = %s\n", in);
                printf("Input var = %d\n", input);
            }
            else {
                printf("There was an error reading from command line");
            }

            switch (input)
            {
            case 1:
                // search for item by name
                break;
            case 2: 
                // searcg for item by id
                break;
            case 3:
                // search for item by category
                break;
            case 4:
                // cart summary
                break;
            case 5: 
                // checkout
                break;
            case 6: 
                // exit
                break;
            default:
                // Default
                break;
            }
        
            XMLNode* str = XMLNode_Child(XMLNode_Child(doc.root, 0), 1);
            printf("%s: %s\n", str->tag, str->attributes.data[0].value);
        
            XMLNodeList* items = XMLNode_Children(str, "item");
            for (int i = 0; i < items->size; i++) {
                XMLNode* item = XMLNodeList_At(items, i);
                XMLAttribute* attr = XMLNode_Attr(item, "name");
                printf("%d.) %s\n", i, attr->value);
            }

            /*

            XMLNode* desc = XMLNode_Child(mainNode, 0);

            XMLNode* anotherNode = XMLNode_Child(doc.root, 1);
            printf("%s\n", anotherNode->inner_text);
            printf("%s: %s\n", doc.root->tag, doc.root->inner_text);

            printf("Attributes:\n");
            for (int i = 0; i < node.attributes.size; i++) {
                XMLAttribute attr = node.attributes.data[i];
            }

            XMLNode* moreNode = XMLNode_Child(XMLNode_Child(doc.root, 0), 0);
            printf("%s: %s\n", moreNode->tag, moreNode->inner_text);

            XMLNode* anotherNode = XMLNode_Child(doc.root, 1);
            */

            //XMLDocument_Write(&doc, "out.xml", 4);
            //XMLDocument_Free(&doc);
        }
    }



    return 0;
}

User LoginUser(VOID)
{
    XMLDocument doc;
    char buffer[50];

    User ActiveUser;
    char Username[100];
    char Password[10];

    if (XMLDocument_Load(&doc, "users.xml")) {
        XMLNode* str =XMLNode_Child(doc.root, 0);
        printf("%s: %s\n", str->tag, str->attributes.data[0].value);
        XMLNodeList* users = XMLNode_Children(str, "user");

        /*
        // For future feature
        for (int i = 0; i < users->size; i++) {
            XMLNode* user = XMLNodeList_At(users, i);
            XMLAttribute* name = XMLNode_Attr(user, "name");
            XMLAttribute* access = XMLNode_Attr(user, "access");
            if (access->value == "admin")
                AdminExists = 1;
        }
        if (AdminExists == 0) {
            // Call a function to create admin
        }
        */

        printf("Would you like to create a new user?");
        fgets(buffer, sizeof(buffer), stdin);
        buffer[strcspn(buffer, "\n")] = 0;
        if (stricmp(buffer, "yes") == 0) {
            ActiveUser = CreateNewUser();
            return ActiveUser;
        }

        while (TRUE) {
            printf("Please enter cashier's name to log in.\n");
            fgets(Username, sizeof(Username), stdin);
            Username[strcspn(Username, "\n")] = 0;
            printf("Username = %s\n", Username);
    
            for (int i = 0; i < users->size; i++) {
                XMLNode* user = XMLNodeList_At(users, i);
                XMLAttribute* name = XMLNode_Attr(user, "name");
                XMLAttribute* pin = XMLNode_Attr(user, "pin");
                if (stricmp(name->value, Username) == 0) {
                    printf("Logging in as %s. Please enter pin to login or 0 to change users:", name->value);
                    while (Password != pin->value)
                    {
                        fgets(Password, sizeof(Password), stdin);
                        Password[strcspn(Password, "\n")] = 0;
                        if (stricmp(pin->value, Password) == 0) {
                            strcpy(ActiveUser.username, Username);
                            strcpy(ActiveUser.password, Password);
                            return ActiveUser;
                        }
                        else if (stricmp(pin->value, "0") == 0) {
                            break;
                        }
                        else {
                            printf("Incorrect pin please try again:");
                        }
                    }
                }
            }

        }
    }
    return ActiveUser;
}

User CreateNewUser(void)
{
    User NewUser;
    char Username[100];
    char Password[10];
    char res[25];
    while (TRUE)
    {
        printf("Please enter desired username for new user:");
        fgets(Username, sizeof(Username), stdin);
        Username[strcspn(Username, "\n")] = 0;
        printf("You entered \"%s\". Is this correct?\n", Username);
        fgets(res, sizeof(res), stdin);
        res[strcspn(res, "\n")] = 0;
        if (stricmp(res, "yes") == 0) {
            while (TRUE) 
            {
                printf("Please enter your pin:");
                fgets(Password, sizeof(Password), stdin);
                Password[strcspn(Password, "\n")] = 0;
                printf("Please confirm your pin again:");
                fgets(res, sizeof(res), stdin);
                res[strcspn(res, "\n")] = 0;
                if (stricmp(res, Password) == 0) {
                    strcpy(NewUser.username, Username);
                    strcpy(NewUser.password, Password);
                    return NewUser;
                }
                else {
                    printf("Pin confirmation does not match, please enter your pin again.\n");
                }
            }
        }
    }

    return NewUser;
}