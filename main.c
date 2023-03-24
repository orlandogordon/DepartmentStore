//
// Build Instrunctions:
// Open Dev Command Prompt
// Run: "msbuild myproject.vcxproj /p:configuration=debug"
// Navigate to Debug folder and run "myproject"
//

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

int main(void) {
    int x = 1;
    printf("This is working %i", x);
    return 0;
}